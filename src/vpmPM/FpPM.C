// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmPM/FpPM.H"
#include "vpmPM/FpFileSys.H"
#include "vpmPM/FpRDBExtractorManager.H"
#include "vpmPM/FpRDBHandler.H"
#include "vpmPM/FpModelRDBHandler.H"
#include "vpmPM/FpProcessManager.H"
#include "vpmApp/vpmAppProcess/FapSolutionProcessMgr.H"
#include "vpmApp/vpmAppProcess/FapSimEventHandler.H"
#include "vpmApp/vpmAppCmds/FapAnimationCmds.H"
#include "vpmApp/vpmAppCmds/FapFileCmds.H"
#include "vpmApp/vpmAppCmds/FapGraphCmds.H"
#include "vpmApp/vpmAppProcess/FapLinkReducer.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUACommandHandler.H"
#include "vpmApp/FapLicenseManager.H"
#include "vpmApp/FapEventManager.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmStrainRosette.H"
#include "vpmDB/FmSimulationEvent.H"
#include "vpmDB/FmBladeProperty.H"
#include "vpmDB/FmSubAssembly.H"
#include "vpmDB/FmTurbine.H"
#include "vpmDB/FmfUserDefined.H"
#include "vpmDB/FmUserDefinedElement.H"
#include "vpmDB/FmModelLoader.H"
#include "vpmDB/FmSolverInput.H"

#include "vpmUI/Fui.H"
#include "vpmUI/FuiModes.H"
#include "FFuLib/FFuProgressDialog.H"
#include "FFuLib/FFuFileDialog.H"
#ifdef FT_HAS_WND
#include "FFuLib/FFuCustom/mvcModels/BladeSelectionModel.H"
#include "FFuLib/FFuCustom/mvcModels/AirfoilSelectionModel.H"
#endif
#ifdef FT_HAS_GRAPHVIEW
#include "FFpLib/FFpFatigue/FFpSNCurveLib.H"
#endif
#include "FFlLib/FFlMemPool.H"

#include "FiDeviceFunctions/FiDeviceFunctionFactory.H"
#include "FiUserElmPlugin/FiUserElmPlugin.H"
#include "FFaFunctionLib/FFaUserFuncPlugin.H"
#include "FFaLib/FFaCmdLineArg/FFaCmdLineArg.H"
#include "FFaLib/FFaDynCalls/FFaSwitchBoard.H"
#include "FFaLib/FFaAlgebra/FFaUnitCalculator.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFaLib/FFaDefinitions/FFaAppInfo.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "Admin/FedemAdmin.H"

#ifdef FT_USE_PROFILER
#include "FFaLib/FFaProfiler/FFaProfiler.H"
#endif

#include <fstream>
#include <quuid.h>
#include <signal.h>
#include <time.h>
#if defined(win32) || defined(win64)
#include <process.h>
#else
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#endif


/////////////////////////////////////
// Initialization of static variables
/////////////////////////////////////

int  FpPM::vpmPID = 0;
int  FpPM::resultsFlag = 0;
bool FpPM::isUsingDefaultModelName = false;
FpPM::PluginMap FpPM::ourPlugins;

enum TouchMode { READ_ONLY = -2, DONT_TOUCH = -1, UNTOUCHED = 0,
		 TOUCHED_RESULTS = 1, TOUCHED_MODEL = 2 };
int FpPM::touchedFlag = DONT_TOUCH;

std::vector<std::string> FpPM::recentFiles;


/////////////////////////////////////////////
// Initiate the PM system and support classes
/////////////////////////////////////////////

void FpPM::init(const char* program)
{
  // Setting process id
#if defined(win32) || defined(win64)
  FpPM::vpmPID = _getpid();
#else
  FpPM::vpmPID = getpid();

  // Initializing signal interface (Linux only)
  sigset_t set;
  sigemptyset(&set); // Empty the signal mask
  // Process signal mask set to empty set - we recieve all signals
  sigprocmask(SIG_SETMASK,NULL,&set);

  std::string fullPath;
  if (FFaFilePath::isRelativePath(program))
  {
    // Transform the relative program path into a full path
    // TODO: Merge this with FFaAppInfo::init()
    fullPath = FFaAppInfo::getCWD();
    size_t nextSlash = fullPath.find_last_of('/');
    size_t prevSlash = nextSlash;
    std::string progName = program;
    if (progName.substr(0,2) == "./")
    {
      prevSlash = fullPath.size();
      progName.erase(0,1);
    }
    else while (progName.substr(0,3) == "../" && nextSlash < fullPath.size())
    {
      prevSlash = nextSlash;
      nextSlash = fullPath.substr(0,nextSlash).find_last_of('/');
      progName.erase(0,3);
    }
    if (prevSlash < fullPath.size())
      fullPath.erase(prevSlash+1);
    fullPath.append(progName);
    program = fullPath.c_str();
  }
#endif

  // Initialize the program path (without the trailing slash)
  FFaAppInfo::init(program);

  // Initialize model member signal connector
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
                          FmModelMemberBase::MODEL_MEMBER_CONNECTED,
                          FFaSlot1S(FpPM,onModelMemberConnected,FmModelMemberBase*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
                          FmModelMemberBase::MODEL_MEMBER_DISCONNECTED,
                          FFaSlot1S(FpPM,onModelMemberConnected,FmModelMemberBase*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
                          FmModelMemberBase::MODEL_MEMBER_CHANGED,
                          FFaSlot1S(FpPM,onModelMemberChanged,FmModelMemberBase*));

  // Initiating debug mode
  bool debugMode = false;
  FFaCmdLineArg::instance()->getValue("debug",debugMode);
  if (debugMode) return; // We do **not** handle signals in debug mode

  // Set up signal handling
  signal(SIGINT , &FpPM::signalHandler); // User presses ctrl-C.
  signal(SIGILL , &FpPM::signalHandler); // Illegal hardware instruction.
  signal(SIGABRT, &FpPM::signalHandler); // Generated by abort function.
  signal(SIGFPE , &FpPM::signalHandler); // Division by 0, overflow, etc ...
  signal(SIGSEGV, &FpPM::signalHandler); // Segmentation violation.
  signal(SIGTERM, &FpPM::signalHandler); // Software termination sent by
  //                                     // kill and system shutdown.

#if !defined(win32) && !defined(win64)
  // Signals not supported on Windows platform:
  signal(SIGQUIT, &FpPM::signalHandler); // User presses ctrl-\.
  signal(SIGBUS , &FpPM::signalHandler); // Bus error.
  signal(SIGSEGV, &FpPM::signalHandler); // Segmentation violation.
  signal(SIGPIPE, &FpPM::signalHandler); // Broken pipe.
  signal(SIGXCPU, &FpPM::signalHandler); // CPU time limit exceeded.
  signal(SIGXFSZ, &FpPM::signalHandler); // File size limit exceeded.
#endif
}


std::string FpPM::getFullFedemPath(const std::string& fName, char check)
{
  auto&& existingFile = [check](std::string& fileName)
  {
    if (!FpFileSys::isReadable(FFaFilePath::checkName(fileName)))
      return false;
#ifdef FP_DEBUG
    else if (check == 'e')
      std::cout <<"   * Found file "<< fileName << std::endl;
#endif
    return true;
  };

  std::string fileName(fName);
  if (check == 'h')
  {
    // First, check if the file exists in the users home directory
    FFaFilePath::makeItAbsolute(fileName,FpFileSys::getHomeDir());
    if (existingFile(fileName))
      return fileName;
    else
      fileName = "resources/" + fName;
  }

  // Next, check if the file exists together with this executable
  FFaFilePath::makeItAbsolute(fileName,FFaAppInfo::getProgramPath());
  if (existingFile(fileName))
    return fileName;

  // Last resort is the current working directory
  fileName = FFaFilePath::getFileName(fileName);
  return check != 'e' || existingFile(fileName) ? fileName : "";
}


void FpPM::loadUnitConvertionFile()
{
  std::string unitFile = FpPM::getFullFedemPath("units.fcd",'h');
  if (!FpFileSys::isFile(unitFile))
    return;

  if (!FFaAppInfo::isConsole())
    std::cout <<"Loading unit conversion file:\n[ "<< unitFile <<" ]\n"<< std::endl;

  FFaUnitCalculatorProvider::instance()->readCalculatorDefs(unitFile);
}


void FpPM::loadResultPosFiles()
{
  // We don't need the RDB extractors when running in batch mode
  if (FFaAppInfo::isConsole())
    return;

  Strings posfiles = FpRDBExtractorManager::instance()->getPredefPosFiles();
  if (!posfiles.empty())
  {
    std::cout <<"Loading results definition files:\n";
    for (const std::string& fName : posfiles)
      std::cout <<"[ "<< fName <<" ]\n";
    std::cout << std::endl;
  }

  FpRDBExtractorManager::instance()->renewExtractors();
}


void FpPM::loadSNCurveFile()
{
#ifdef FT_HAS_GRAPHVIEW
  std::string curveFile = FpPM::getFullFedemPath("sn_curves.fsn",'h');
  if (!FpFileSys::isFile(curveFile))
    return;

  if (!FFaAppInfo::isConsole())
    std::cout <<"Loading S-N curves file:\n[ "<< curveFile <<" ]\n"<< std::endl;

  FFpSNCurveLib::instance()->readSNCurves(curveFile);
#endif
}


void FpPM::loadPropertyLibraries()
{
#ifdef FT_HAS_WND
  std::string instPath = FpPM::getFullFedemPath("Properties");

  // Add library-directories to global BladeSelectionModel, which will also load them
  BladeSelectionModel::instance()->clearModel();
  BladeSelectionModel::instance()->addDirectory(0, instPath.c_str(), true);
  BladeSelectionModel::instance()->addDirectory(0, FmDB::getMechanismObject()->getAbsBladeFolderPath().c_str(), false);

  // Add library-directories to global AirfoilSelectionModel, which will also load them
  AirfoilSelectionModel::instance()->clearModel();
  FFaFilePath::appendToPath(instPath,"AeroData");
  AirfoilSelectionModel::instance()->addDirectory(0, instPath.c_str(), true);
#endif
}


void FpPM::loadAllPlugins()
{
  Strings libs; // Search for plugin libraries
  if (!FpFileSys::getFiles(libs,FpPM::getFullFedemPath("plugins"),"*.dll *.so *.sl"))
    return;

  // Try to load all found files and check if they contain user-defined items.
  // Only one library of each type may be simultaneously loaded.
  for (const std::string& fName : libs)
  {
    char sign[128];
    std::string lname = FFaFilePath::appendFileNameToPath("plugins",fName);
    if (FFaUserFuncPlugin::instance()->validate(lname,128,sign))
      ourPlugins[lname] = PluginId(UDEF_FUNC,sign);
    else if (FiUserElmPlugin::instance()->validate(lname,128,sign))
      ourPlugins[lname] = PluginId(UDEF_ELM,sign);
    else
      ListUI <<"Error :   The file "<< lname  <<" is not a valid plugin library.\n";
  }

  // Load the first detected valid plugin of each kind
  char signature[128];
  std::vector<bool> loaded(UDEF_TYPES,false);
  for (PluginMap::value_type& pl : ourPlugins)
    if (loaded[pl.second.type])
      ListUI <<"\nNote :    Available library "<< pl.first <<" (not loaded).\n";
    else switch (pl.second.type)
      {
      case UDEF_FUNC:
        if (FFaUserFuncPlugin::instance()->load(pl.first))
        {
          pl.second.loaded = loaded[UDEF_FUNC] = true;
          if (FFaUserFuncPlugin::instance()->getSign(127,signature))
            ListUI <<"          "<< signature <<"\n";
        }
        break;
      case UDEF_ELM:
        if (FiUserElmPlugin::instance()->load(pl.first))
        {
          pl.second.loaded = loaded[UDEF_ELM] = true;
          if (FiUserElmPlugin::instance()->getSign(127,signature))
            ListUI <<"          "<< signature <<"\n";
        }
        break;
      default:
        break;
      }
}


void FpPM::getPluginList(std::vector<PluginLib>& plugins)
{
  size_t i = 0;
  plugins.resize(ourPlugins.size());
  for (const PluginMap::value_type& pl : ourPlugins)
  {
    plugins[i].name = FFaFilePath::getFileName(pl.first);
    plugins[i].sign = pl.second.sign;
    plugins[i++].loaded = pl.second.loaded;
  }
}


void FpPM::getActivePlugins(std::vector<std::string>& plugins)
{
  plugins.clear();
  for (const PluginMap::value_type& pl : ourPlugins)
    if (pl.second.loaded)
      plugins.push_back(pl.first);
}


bool FpPM::togglePlugin(const std::string& plugin, bool toggleOn)
{
  PluginMap::iterator it = ourPlugins.find(plugin);
  if (it == ourPlugins.end())
    it = ourPlugins.find(FFaFilePath::appendFileNameToPath("plugins",plugin));
  if (it == ourPlugins.end() || it->second.loaded == toggleOn)
    return false;

  return FpPM::togglePlugin(FmDB::getMechanismObject(),it,toggleOn);
}


bool FpPM::togglePlugin(FmMechanism* mech, PluginMap::iterator it, char toggle)
{
  // Avoid toggling on more than one plugin of each type
  if (toggle)
    for (PluginMap::iterator p = ourPlugins.begin(); p != ourPlugins.end(); ++p)
      if (p != it && p->second.loaded && p->second.type == it->second.type)
      {
        if (toggle == 'f') // Untoggle the currently active plugin first
          FpPM::togglePlugin(p,false,mech);
        else
          return false;
      }

  return FpPM::togglePlugin(it,toggle,mech);
}


bool FpPM::togglePlugin(PluginMap::iterator it, bool toggleOn,
                        FmMechanism* mech)
{
  it->second.loaded = toggleOn;
  bool changedPlugin = false;
  std::vector<FmModelMemberBase*> allUDobj;

  switch (it->second.type) {
  case UDEF_FUNC:
    if (!toggleOn)
    {
      changedPlugin = FFaUserFuncPlugin::instance()->unload(it->first);
      mech->activeFunctionPlugin.setValue("");
    }
    else if (FFaUserFuncPlugin::instance()->load(it->first))
    {
      changedPlugin = true;
      char signature[128];
      if (FFaUserFuncPlugin::instance()->getSign(127,signature))
        ListUI <<"          "<< signature <<"\n";
      mech->activeFunctionPlugin.setValue(it->first);
    }
    if (changedPlugin)
      FmDB::getAllOfType(allUDobj,FmfUserDefined::getClassTypeID());
    break;

  case UDEF_ELM:
    if (!toggleOn)
    {
      changedPlugin = FiUserElmPlugin::instance()->unload(it->first);
      mech->activeElementPlugin.setValue("");
    }
    else if (FiUserElmPlugin::instance()->load(it->first))
    {
      changedPlugin = true;
      char signature[128];
      if (FiUserElmPlugin::instance()->getSign(127,signature))
        ListUI <<"          "<< signature <<"\n";
      mech->activeElementPlugin.setValue(it->first);
    }
    if (changedPlugin)
      FmDB::getAllOfType(allUDobj,FmUserDefinedElement::getClassTypeID());
    break;

  default:
    break;
  }

  // Remove all existing user-defined function/element instances, if any
  FmModelMemberBase::inInteractiveErase = true;
  for (FmModelMemberBase* obj : allUDobj)
    obj->interactiveErase();
  FmModelMemberBase::inInteractiveErase = false;

  return changedPlugin;
}


void FpPM::toggleActivePlugins(FmMechanism* mech)
{
  std::vector<FFaField<std::string>*> plugins;
  if (!mech->activeFunctionPlugin.getValue().empty())
    plugins.push_back(&mech->activeFunctionPlugin);
  if (!mech->activeElementPlugin.getValue().empty())
    plugins.push_back(&mech->activeElementPlugin);

  int nActive = 0;
  PluginMap::iterator it;
  for (FFaField<std::string>* field : plugins)
    if ((it = ourPlugins.find(field->getValue())) == ourPlugins.end())
      ListUI <<"===> ERROR: Plugin library "<< field->getValue()
             <<" does not exist.\n";
    else if (it->second.loaded)
      nActive++;
    else if (FpPM::togglePlugin(mech,it,'f'))
      nActive++;

  if (nActive < 2)
    FpPM::saveActivePlugins(mech);
}


void FpPM::saveActivePlugins(FmMechanism* mech)
{
  for (const PluginMap::value_type& pl : ourPlugins)
    if (pl.second.loaded)
      switch (pl.second.type) {
      case UDEF_FUNC:
        mech->activeFunctionPlugin.setValue(pl.first);
        break;
      case UDEF_ELM:
        mech->activeElementPlugin.setValue(pl.first);
        break;
      default:
        break;
      }
}


void FpPM::start()
{
  FFaSwitchBoard::connect(FpProcessManager::instance(),
			  FpProcessManager::FINISHED,
			  FFaSlot0S(FpRDBHandler,stopRDBChecking));
  FFaSwitchBoard::connect(FpProcessManager::instance(),
			  FpProcessManager::GROUP_STARTED,
			  FFaSlot1S(FpRDBHandler,onProcessGroupStarted,int));
  FFaSwitchBoard::connect(FpProcessManager::instance(),
			  FpProcessManager::FINISHED,
			  FFaSlot0S(FapAnimationCmds,onSimulationFinished));
}


/////////////////////////////////////
// FEDEM Copyright notice to console
/////////////////////////////////////

void FpPM::CopyrightNotice()
{
  if (FFaAppInfo::isConsole())
  {
    std::cout <<"\nCopyright 2016 - "<< FedemAdmin::getBuildYear()
	      <<" SAP SE. All Rights Reserved.\n\n";
    std::cout <<"You are running Fedem version "
	      << FedemAdmin::getVersion() <<" "<< FedemAdmin::getBuildDate()
	      <<" in batch mode.\n"<< std::endl;
    return;
  }

  std::cout <<"\n\n\n"
    "                        COPYRIGHT 2016 - "<< FedemAdmin::getBuildYear() <<
    "\n                               SAP SE\n"
    "\n"
    "                         All Rights Reserved\n"
    "\n\n";

  std::cout <<"You are running Fedem version "
	    << FedemAdmin::getVersion() <<" "<< FedemAdmin::getBuildDate()
	    <<"\n\n"<< std::endl;
}


const char* FpPM::getVpmVersion()
{
  static std::string version(FedemAdmin::getVersion());

  return version.c_str();
}


///////////////////////////////////////////////////////////////
// Quitting the application nicely
///////////////////////////////////////////////////////////////

void FpPM::quitVPM(int exitCode)
{
  // Close the log-file
  FFaMsg::getMessager().closeListFile();

  // Close the console window (if any) and exit
  if (FFaAppInfo::isConsole())
  {
    bool debugMode = false;
    FFaCmdLineArg::instance()->getValue("debug",debugMode);
    if (debugMode) FFaAppInfo::closeConsole();
  }
  exit(exitCode);
}


///////////////////////////
// Fedem project interface
///////////////////////////

static std::string getDefaultStartDir()
{
  if (FFaAppInfo::isInCwd())
    return FpFileSys::getHomeDir();
  else
    return FFaAppInfo::getCWD();
}


static void completeModelFileName(std::string& completeName)
{
  // Add the default extension, if no extension yet
  if (FFaFilePath::getExtension(completeName).empty())
    completeName += ".fmm";

  FFaFilePath::makeItAbsolute(completeName,getDefaultStartDir());
  FFaFilePath::checkName(completeName);
}


int FpPM::dontTouchModel()
{
  int oldMode = FpPM::touchedFlag;
  if (oldMode >= UNTOUCHED)
    FpPM::touchedFlag = DONT_TOUCH;
  return oldMode;
}


void FpPM::touchModel(bool resultsOnly)
{
  TouchMode newMode = resultsOnly ? TOUCHED_RESULTS : TOUCHED_MODEL;

  if (FpPM::touchedFlag < UNTOUCHED || FpPM::touchedFlag >= newMode)
    return;

  FpPM::touchedFlag = newMode;
  FpPM::updateGuiTitle();
}


void FpPM::unTouchModel(bool setReadOnly)
{
  TouchMode newMode = setReadOnly ? READ_ONLY : UNTOUCHED;
  if (FpPM::touchedFlag == newMode)
    return;

  FpPM::touchedFlag = newMode;
  FpPM::updateGuiTitle();
}


void FpPM::updateGuiTitle()
{
  std::string newName = FmDB::getMechanismObject()->getModelFileName();
  FmSimulationEvent* simEvent = FapSimEventHandler::getActiveEvent();
  if (simEvent) newName += " (" + simEvent->getIdString(true) + ")";
  if (FpPM::touchedFlag > UNTOUCHED)
    newName += "*";
  else if (FpPM::touchedFlag == READ_ONLY)
    newName += " (view only)";

  Fui::setTitle(FFaFilePath::getFileName(newName).c_str());
}


bool FpPM::isModelTouched(bool ignoreResults)
{
  if (ignoreResults)
    return FpPM::touchedFlag >= TOUCHED_MODEL;
  else
    return FpPM::touchedFlag >= TOUCHED_RESULTS;
}


bool FpPM::isModelTouchable()
{
  return FpPM::touchedFlag != READ_ONLY;
}


bool FpPM::isModelRunable()
{
  if (FpPM::resultsFlag > 0 || FpPM::touchedFlag == READ_ONLY)
    return false;

  return FapSolutionProcessManager::instance()->empty();
}


bool FpPM::isModelEditable()
{
  if (FpPM::resultsFlag > 0 || FpPM::touchedFlag == READ_ONLY)
    return false;
  else if (FapSolutionProcessManager::instance()->empty())
    return !FapSimEventHandler::hasResults();
  else
    return false;
}


bool FpPM::isModelRestartable()
{
  if (FpPM::hasResults(SECONDARY) && FpPM::touchedFlag != READ_ONLY)
    return FapSolutionProcessManager::instance()->empty();
  else
    return false;
}


void FpPM::vpmModelNew()
{
  std::string modelFilePath = FmDB::getMechanismObject()->getAbsModelFilePath();
  if (FpPM::closeModel())
    FpPM::openTemplateFile(modelFilePath);
}


void FpPM::openCmdLineFile()
{
  std::string modelFileName;
  FFaCmdLineArg::instance()->getValue("f", modelFileName);
  if (modelFileName == "untitled.fmm")
    FpPM::openTemplateFile(getDefaultStartDir());
  else
  {
    bool skipFEData = false;
    bool batchExport = false;
    if (FFaAppInfo::isConsole())
      FFaCmdLineArg::instance()->getValue("exportAnimations", batchExport);
    else
      FFaCmdLineArg::instance()->getValue("noFEData", skipFEData);

    // If the model opening fails, create an empty new model instead
    if (!FpPM::vpmModelOpen(modelFileName, batchExport || !skipFEData))
      FpPM::vpmModelNew();
  }

  FFaMsg::setStatus("Ready");
}


bool FpPM::closeModel(bool saveOnBatchExit, bool pruneEmptyDirs, bool isExiting)
{
  bool wasUsingDefaultModelName = FpPM::isUsingDefaultModelName;
  FmMechanism* mech = FmDB::getMechanismObject();
  std::string oldRDBPath = mech->getAbsModelRDBPath();
  std::string oldBladePath = mech->getAbsBladeFolderPath();

  // Delete old ".undoPoint" files
  /* Temporarily disabled (KMO)
  const std::string& oldFile = mech->getModelFileName();
  if (!oldFile.empty()) {
    std::string logFile = FFaFilePath::getBaseName(oldFile) + ".log";
    FpFileSys::deleteFile(oldFile+".undoPoint");
    FpFileSys::deleteFile(logFile+".undoPoint");
  */

  if (FpPM::isModelTouched())
  {
    // Ask for user approval and model file save
    std::string msg;
    if (wasUsingDefaultModelName)
      msg = "Save current model";
    else
      msg = "Save \"" + mech->getModelFileName() + "\"";
    if (isExiting)
      msg += " before exiting ?";
    else
      msg += " ?";
    if (!FpProcessManager::instance()->empty())
      msg += "\nThe running solve process will be killed first.";
    const char* texts[3] = {"Yes","No","Cancel"};

    switch (Fui::genericDialog(msg.c_str(),texts,3))
    {
      case -1: // no GUI (batch run)
        if (saveOnBatchExit)
          FpPM::vpmModelSave(pruneEmptyDirs);
        break;
      case 0: // yes
        if (wasUsingDefaultModelName)
          FapFileCmds::saveAs();
        else if (!FpPM::vpmModelSave(pruneEmptyDirs))
          return false;
        break;
      case 1: // no
        break;
      case 2: // cancel
        return false;
    }
  }

  FapGraphCmds::killAllGraphViews();

  // Running child processs must be killed at this point,
  // or else the disk clean up below will fail
  FpProcessManager::instance()->killAll();

  FapSimEventHandler::RDBClose(pruneEmptyDirs);
  FapSimEventHandler::activate(NULL,false,false);

  if (wasUsingDefaultModelName)
  {
    // Delete the old RDB completely if it belonged to an untitled model
    if (FpFileSys::isDirectory(oldRDBPath))
      FpFileSys::removeDir(oldRDBPath);

    // Delete blade folder and contents if it belonged to an untitled model
    if (FpFileSys::isDirectory(oldBladePath))
      FpFileSys::removeDir(oldBladePath);
  }

  // Skip memory cleaning when exiting
  if (isExiting) return true;

  FuiModes::cancel();
  Fui::analysisOptionsUI(false);
  Fui::stressOptionsUI(false);
  Fui::eigenmodeOptionsUI(false);
  Fui::gageOptionsUI(false);
  Fui::fppOptionsUI(false);
  Fui::rdbSelectorUI(false);
  Fui::rdbMEFatigueUI(false);
  Fui::resultFileBrowserUI(false);
  Fui::noUserInputPlease(); // This block is ended in vpmModelOpen

  // Clean up memory
  FapEventManager::permUnselectAll();
  FapAnimationCmds::hide();
  FpPM::touchedFlag = DONT_TOUCH; // suppress touching while erasing this model
  FFaMsg::pushStatus("Clearing mechanism");
  FmDB::eraseAll(true);
  FpPM::setResultFlag(); // Reset result flag for command sensitivity update
  FiDeviceFunctionFactory::removeInstance();
  FFlMemPool::deleteVisualsMemPools();
  FFlMemPool::deleteAllLinkMemPools();
  FFaMsg::popStatus();

  return true;
}


/*!
  Opens the model file \a givenName, and loads the part data if \a doLoadParts
  is true. A log file is also opened, if specified on the command line.
  The name on this file is either set by using the model file name, swapping the
  ending with .log for .fmm, or using \a newFileNameForLogFile which is
  supposed to be the new model file name this model is supposed to become.
  \sa FapCadConnectionCmds::newFedemModelFromCad()
*/

bool FpPM::vpmModelOpen(const std::string& givenName, bool doLoadParts,
			const std::string& newFileNameForLogFile)
{
  // Check the given model file name and complete it, if needed
  std::string name(givenName);
  completeModelFileName(name);

  // Check that the directory of the given model file exists, create it if not
  std::string path = FFaFilePath::getPath(name);
  if (!FpFileSys::verifyDirectory(path))
  {
    Fui::okToGetUserInput(); // This block is started in FpPM::closeModel
    return false;
  }

  // When getting here the model file name is not "default"
  FpPM::isUsingDefaultModelName = false;

  // Open a log-file for a copy of the Output List messages
  std::string logFileName;
  bool writeLogFile = false;
  FFaCmdLineArg::instance()->getValue("logFile", writeLogFile);
  if (writeLogFile)
  {
    // Open a new log-file
    if (newFileNameForLogFile.empty())
      logFileName = FFaFilePath::getBaseName(name) + ".log";
    else
      logFileName = FFaFilePath::getBaseName(newFileNameForLogFile) + ".log";
  }

  int existingFile = Fedem::loadModel(name,logFileName);
  if (existingFile < 0) return false;

  FmMechanism* mech = FmDB::getMechanismObject();
  FpPM::saveActivePlugins(mech);
  FpPM::loadPropertyLibraries();

#ifdef FT_USE_PROFILER
  bool doMemPoll;
  FFaCmdLineArg::instance()->getValue("memPoll", doMemPoll);
  if (doMemPoll)
  {
    // Memory polling for profiling
    Fui::okDialog("Mech. Model");
    FFaMemoryProfiler::reportMemoryUsage("Mech. Model");
  }
#endif

  Fui::setTitle(FFaFilePath::getFileName(name).c_str());

  // Check availability of model part repository
  std::string mlr;
  FFaCmdLineArg::instance()->getValue("feRepository", mlr);
  if (mlr.empty()) mlr = mech->modelLinkRepository.getValue();
  if (!mlr.empty() && !FpFileSys::isDirectory(FFaFilePath::makeItAbsolute(mlr,path)))
  {
    // The model part repository specified in the model file
    // does not exist, check if provided via environment variable
    const char* fedemLDB = getenv("FEDEM_LDB");
    if (fedemLDB && FpFileSys::isDirectory(fedemLDB) && mlr != fedemLDB)
    {
      ListUI <<"Note: Changing FE part repository of this model"
             <<"\n      from "<< mlr
             <<"\n        to "<< fedemLDB <<"\n";
      mech->modelLinkRepository.setValue(fedemLDB);
    }
    else
    {
      std::string errMsg = "Could not locate FE model repository\n"+ mlr +"\n\n";
      errMsg += "Do you want to change to internal repository,\n"
        "search for the original repository, or create a new one?\n\n"
        "Note: Fedem will anyway look for the FE models using\n"
        "the default search path (see Output List for details).";
      const char* texts[2] = {"Use internal","Find it or create new..."};
      if (Fui::genericDialog(errMsg.c_str(),texts,2) < 1) // use internal
      {
        ListUI <<"===> Switching to internal FE model repository.\n";
        mech->modelLinkRepository.setValue("");
      }
      else // search for model part repository
      {
        Fui::okToGetUserInput();
        FapFileCmds::setModelLinkRep();
        Fui::noUserInputPlease();
      }
    }
    if (mech->modelLinkRepository.getValue() != mlr)
    {
      // Force model touching (overriding DONT_TOUCH)
      FpPM::touchedFlag = UNTOUCHED;
      FpPM::touchModel();
    }
  }

  // Control skipping FE data loading. Error messages and stuff
  std::vector<FmPart*> allParts;
  if (FapLicenseManager::isFreeEdition() || FapLicenseManager::isLimEdition())
    doLoadParts = existingFile; // always try to load FE parts in the Free/Limited edition
  else if (!doLoadParts && existingFile)
  {
    if (FmDB::getModelFileVer() < FFaVersionNumber(3,1,0,3))
    {
      std::string errMsg =
        "This model was saved using Fedem version 3.0 or earlier.\n"
        "When you skip the FE-Data on such a model, Fedem will not be\n"
        "able to verify whether previously reduced matrices are valid.\n"
        "The FE parts will get status \"Needs Reduction\" even when\n"
        "valid reducer data is present.\n"
        "The correct status for a part will be detected if you load its\n"
        "FE-Data at a later stage though.";
      const char* texts[2] = {"Skip FE-Data","Load FE-Data"};
      int retVar = Fui::genericDialog(errMsg.c_str(),texts,2);
      if (retVar == 0) // Continue without loading FE-Data
        ListUI <<"\n===> FE data skipped.\n"
               <<"Warning: This model was saved using Fedem version 3.0 or earlier.\n"
               <<"         When you skip the FE-Data on such a model, Fedem will not be\n"
               <<"         able to verify whether previously reduced matrices are valid.\n"
               <<"         The FE parts will get status \"Needs Reduction\" even when\n"
               <<"         valid reducer data is present.\n";
      else if (retVar == 1) // Load FE-Data anyway
        doLoadParts = true;
      else if (retVar < 0) // Load FE-Data anyway, needed for the first batch execution
      {
        doLoadParts = true;
        ListUI <<"\n===> Forcing read of FE model data to obtain external node information.\n"
               <<"     This might take a while for big models...\n";
      }
    }
    else if (FFaAppInfo::isConsole())
    {
      FmDB::getUnsavedParts(allParts);
      if (!allParts.empty())
      {
        // Some or all parts are missing their saved ftl-file.
        // The part-repository may have been deleted. When running batch we
        // must then re-import all parts before the solvers can be executed.
        doLoadParts = true;
        ListUI <<"\n===> Some files could not be found in the FE model repository.\n"
               <<"     Forcing re-import of FE model data for the unsaved parts.\n"
               <<"     This might take a while for big models...\n";
      }
    }
  }

  if (doLoadParts)
  {
    // Actually load the FE data
    if (allParts.empty())
      FmDB::getAllParts(allParts);

    if (!allParts.empty())
    {
      Fui::okToGetUserInput();
      FpPM::loadParts(allParts);
      Fui::noUserInputPlease();
    }
  }

#ifdef FT_USE_PROFILER
  if (doMemPoll)
  {
    Fui::okDialog("Parts");
    FFaMemoryProfiler::reportMemoryUsage("Parts");
  }
#endif

  // Now that all FE data is loaded we can syncronize the strain rosettes
  FmStrainRosette::syncStrainRosettes();

  // Create the visualization of the mechanism and show it
  FFaMsg::pushStatus("Creating visualization");
  FmDB::displayAll();
  FFaMsg::popStatus();

#ifdef FT_USE_PROFILER
  if (doMemPoll)
  {
    Fui::okDialog("Part viz");
    FFaMemoryProfiler::reportMemoryUsage("Part viz");
  }
#endif

  if (FpPM::touchedFlag == DONT_TOUCH)
    FpPM::unTouchModel();

  // Syncronize the FE part RSD with actual contents on disk
  FmDB::getFEParts(allParts);
  for (FmPart* part : allParts)
    if (part->syncRSD(true))
      FpPM::touchModel(); // Indicate that the model has changed

  // Check for results
  FFaMsg::pushStatus("Reading result info");
  FpModelRDBHandler::RDBOpen(mech->getResultStatusData(),mech,true,true);
  mech->getResultStatusData(false)->copy(mech->getResultStatusData());
  FFaMsg::popStatus();

#ifdef FT_USE_PROFILER
  if (doMemPoll)
  {
    Fui::okDialog("RDB Headers");
    FFaMemoryProfiler::reportMemoryUsage("RDB Headers");
  }
#endif

  // Finishing up

  if (existingFile)
    ListUI <<"\n===> Done reading\n"
	   <<"========================================"
	   <<"========================================\n\n";

  // Update the object browser
  FapUAExistenceHandler::doUpdateSession();

  // Update all command sensitivities + toggle on model open
  FapUACommandHandler::updateAllUICommands(true,true);

  // Reset all file dialog memorizers to the new model file location
  FFuFileDialog::resetMemoryMap(path);

  Fui::okToGetUserInput(); // This block is started in FpPM::closeModel

  // Move the file name to the top of the recent file list
  FpPM::addRecent(name);
  Fui::updateUICommands();

  return true;
}


void FpPM::addRecent(const std::string& name)
{
  std::vector<std::string>::iterator it = std::find(recentFiles.begin(),
                                                    recentFiles.end(),name);
  if (it != recentFiles.end())
    recentFiles.erase(it);

  recentFiles.insert(recentFiles.begin(),name);
}


void FpPM::removeRecent(size_t idx)
{
  if (idx < recentFiles.size())
    recentFiles.erase(recentFiles.begin()+idx);
}


bool FpPM::vpmAssemblyOpen(const std::string& givenName, bool doLoadParts)
{
  // Check the given assembly file name and complete it, if needed
  std::string name(givenName);
  completeModelFileName(name);

  // Check for existence of the given assembly file
  if (!FpFileSys::isFile(name))
  {
    ListUI <<"===> Non-existing Subassembly file "<< name <<"\n";
    return false;
  }

  FmSubAssembly* newAss = NULL;
  FFaMsg::pushStatus("Reading Subassembly");
  bool status = FmSubAssembly::readFMF(name,&newAss);
  FFaMsg::popStatus();
  if (status && newAss)
    newAss->resolveAfterRead();
  else
    return false;

  // Control skipping FE data loading
  std::vector<FmPart*> allParts;
  if (doLoadParts || FapLicenseManager::isFreeEdition() || FapLicenseManager::isLimEdition())
  {
    // Actually load the FE data
    FmDB::getAllParts(allParts,newAss);
    if (!allParts.empty())
      FpPM::loadParts(allParts);
  }

  // Now that all FE data is loaded we can syncronize the strain rosettes
  FmStrainRosette::syncStrainRosettes();

  // Create the visualization of the mechanism and show it
  FFaMsg::pushStatus("Creating visualization");
  FmDB::displayAll(*newAss->getHeadMap());
  FFaMsg::popStatus();

  // Update the object browser
  FapUAExistenceHandler::doUpdateSession();

  return true;
}


bool FpPM::loadParts(const std::vector<FmPart*>& allParts)
{
  FFuProgressDialog* progDlg = NULL;
  if (!FFaAppInfo::isConsole())
    progDlg = FFuProgressDialog::create("Please wait...", "Cancel",
                                        "Loading FE models", allParts.size());

  int  partNr = 0;
  bool doLoadParts = true;
  bool allowFEparts = true;
  Strings erroneousParts, deniedParts;

  FFaMsg::list("===> Reading FE parts\n");
  FFaMsg::pushStatus("Loading FE/Cad data");
  FFaMsg::enableSubSteps(allParts.size());
  for (FmPart* part : allParts)
  {
    FFaMsg::setSubStep(++partNr);
    if (progDlg)
      progDlg->setCurrentProgress(partNr-1);

    // If user has cancelled loading, just switch ram usage level such that
    // the FE data may be re-enabled later through the FE-Data settings
    if (progDlg && progDlg->userCancelled()) doLoadParts = false;
    if (!doLoadParts) part->ramUsageLevel = FmPart::NOTHING;

    // Load FE data if it is an FE part. If it is a generic part, use
    // the visualization file if it exists. If not, use the CAD visualization.
    // If that is not present either, use the FE data.

    bool loadFEdata = false;
    bool loadCadData = false;
    if (!part->useGenericProperties.getValue())
      loadFEdata = true;
    else if (part->visDataFile.getValue().empty())
    {
      if (!part->baseCadFileName.getValue().empty())
        loadCadData = doLoadParts;
      else if (!part->baseFTLFile.getValue().empty())
        loadFEdata = true;
    }

    if (loadFEdata)
    {
      if (allowFEparts)
      {
        if (!part->openFEData())
        {
          if (part->lockLevel.getValue() == FmPart::FM_DENY_LINK_USAGE)
            deniedParts.push_back(part->getLinkIDString());
          else
            erroneousParts.push_back(part->getLinkIDString());
        }
      }
      else if (!part->useGenericProperties.getValue())
      {
        part->lockLevel.setValue(FmPart::FM_DENY_LINK_USAGE);
        deniedParts.push_back(part->getLinkIDString());
      }
    }
    else if (loadCadData)
      if (!part->openCadData())
        erroneousParts.push_back(part->getLinkIDString());

    part->updateTriadTopologyRefs(true,1);
  }

  if (progDlg)
    progDlg->setCurrentProgress(allParts.size());

  FFaMsg::disableSubSteps();
  FFaMsg::setSubTask("");
  FFaMsg::popStatus();

  delete progDlg;

  if (erroneousParts.empty() && deniedParts.empty())
    return true;

  std::string msg;
  size_t nMax = 30; // To limit the size of the message box
  if (!erroneousParts.empty())
  {
    msg = "The following parts could not be loaded due to errors in their\n"
      "respective FE data files (see output list for details):";
    for (size_t i = 0; i < erroneousParts.size() && i < nMax; i++)
      msg += "\n\t" + erroneousParts[i];
    if (erroneousParts.size() > nMax) msg += "\n\t...";
    if (!deniedParts.empty()) msg += "\n\n";
    if (erroneousParts.size() >= nMax)
      nMax = 1;
    else
      nMax -= erroneousParts.size();
  }

  if (!deniedParts.empty())
  {
    msg += "The following parts could not be loaded ";
    if (allowFEparts && FapLicenseManager::isDemoEdition())
      msg += "due to violation of the\nsize limits in this edition "
        "(see output list for details):";
    else if (FapLicenseManager::isLimEdition())
      msg += "because you are using the Limited edition of Fedem:\n";
    else
      msg += "due to demo license failure:";
    for (size_t i = 0; i < deniedParts.size() && i < nMax; i++)
      msg += "\n\t" + deniedParts[i];
    if (deniedParts.size() > nMax) msg += "\n\t...";
    msg += "\n\nFurther manipulation of the current model is disabled due to this.\n"
      "To continue with this model, you have to save it into a new model first.";
    if (FapLicenseManager::isLimEdition())
      msg += "\n\nTo use flexible parts, "
        "you need to have the FullFlex edition of Fedem.\n"
        "You can also try out the Demo edition.";
    FpPM::unTouchModel(true); // set model view-only
  }

  FFaMsg::list("===> FE model loading failed.\n\n",true);
  Fui::dismissDialog(msg.c_str(),FFuDialog::ERROR);
  return false;
}


bool FpPM::openTemplateFile(const std::string& modelPath)
{
  // Create default non-existing file name in the specified model file path
  int nextInc = FpFileSys::getNextIncrement(modelPath,"fmm",1,"untitled_*");
  std::string untitled = FFaNumStr("untitled_%d.fmm",nextInc);
  std::string fileName = FFaFilePath::appendFileNameToPath(modelPath,untitled);

  // Flag that the default model file name is being used
  FpPM::isUsingDefaultModelName = true;

  // Parse the template model file
  std::string defaultFile = FpPM::getFullFedemPath("Templates/default.fmm");
  bool writeLogFile = false;
  FFaCmdLineArg::instance()->getValue("logFile",writeLogFile);
  int existingFile = Fedem::loadTemplate(fileName,defaultFile,writeLogFile);
  if (existingFile < 0) return false; // invalid file path

  FmMechanism* mech = NULL;
  if (existingFile > 0)
    mech = FmDB::getMechanismObject();
  else
  {
    // Non-existing template file, create an empty mechanism
    mech = FmDB::newMechanism();
    mech->syncPath(fileName,true);
  }

  FpPM::saveActivePlugins(mech);
  FpPM::loadPropertyLibraries();

  FFaMsg::pushStatus("Creating visualization");
  FmDB::displayAll();
  FFaMsg::popStatus();

  FpPM::unTouchModel();

  // Initialise results database, it should be empty at this point...
  const std::string& mainRDBPath = mech->getAbsModelRDBPath();
  mech->getResultStatusData()->setPath(mainRDBPath);
  mech->getResultStatusData(false)->copy(mech->getResultStatusData());

  // But if it wasn't, make sure it is empty now
  if (FpFileSys::isDirectory(mainRDBPath))
  {
    ListUI <<"===> Results directory "<< mainRDBPath
	   <<" already exists, and is deleted.\n";
    FpFileSys::removeDir(mainRDBPath);
  }
  FpPM::setResultFlag();

  // Finishing up

  if (existingFile)
    ListUI <<"\n===> Done reading\n"
	   <<"========================================"
	   <<"========================================\n\n";

  // Update the object browser
  FapUAExistenceHandler::doUpdateSession();

  // Update all command sensitivities + toggle on model open
  FapUACommandHandler::updateAllUICommands(true,true);

  Fui::okToGetUserInput(); // This block is started in FpPM::closeModel

  return true;
}


bool FpPM::vpmModelSave(bool pruneEmptyDirs)
{
  FmMechanism* mech = FmDB::getMechanismObject();
  const std::string& modelFile = mech->getModelFileName();
  if (modelFile.empty())
    return false;

  // Check for different Fedem version and ask user for confirmation
  if (!FmDB::updateModelVersionOnSave(FpFileSys::isFile(modelFile)))
    return false; // don't overwrite the older model file

  // Reset simulation event modifications, if any
  bool didSwitchEvent = FapSimEventHandler::activate(NULL,true,false);
  if (didSwitchEvent)
    ListUI <<"===> Switching to master Simulation event.\n";

  // Delete in-active joint springs, -dampers, -loads and -motions
  bool purgeOnSave = false;
  FFaCmdLineArg::instance()->getValue("purgeOnSave", purgeOnSave);
  if (purgeOnSave)
    FmDB::purgeJointComponents();

  bool isModelSaved = false;
  FFaMsg::list("===> Saving " + modelFile + "\n");

  // Save the model in <modelFile>.tmp so we don't loose the old file
  // in case of write failure due to disk full, etc.
  std::string tempFile = modelFile + ".tmp";
  std::ofstream s(tempFile,std::ios::out);
  if (s)
  {
    Fui::noUserInputPlease();
    FpFileSys::verifyDirectory(mech->getAbsModelRDBPath());

    std::vector<FmPart*> allParts;
    FmDB::getAllParts(allParts);
    for (FmPart* part : allParts)
      part->saveFEData();

    FapSimEventHandler::RDBSave(pruneEmptyDirs);
    FmSubAssembly::mainFilePath = mech->getAbsModelFilePath();
    isModelSaved = FmDB::reportAll(s);
    s.close();

    Fui::okToGetUserInput();
  }

  if (isModelSaved)
  {
    FpFileSys::renameFile(modelFile, modelFile+".bak");
    FpFileSys::renameFile(tempFile, modelFile);
    FFaMsg::list("  -> Model saved in " + modelFile + "\n");
    FpPM::unTouchModel();
  }
  else
  {
    FpFileSys::deleteFile(tempFile);
    FFaMsg::list("  -> Error: Could NOT save " + modelFile + "\n",true);
  }

  const time_t currentTime = time(NULL);
  FFaMsg::list("===> Done saving: " + std::string(ctime(&currentTime)));

  if (!isModelSaved)
    ListUI <<"===> WARNING: The model was not completely saved.\n"
	   <<"     Try to free some space on disk and save the model again.\n"
	   <<"     If you exit Fedem now you will loose any unsaved data.\n\n";

  else if (didSwitchEvent)
    // Reopen the RDB for the master event
    FpModelRDBHandler::RDBOpen(mech->getResultStatusData(),mech);

  return isModelSaved;
}


// Note OIH 2012-06-19 on undo/redo:
// ---------------------------------
// The undo functionality can/should be extended with
// - a "lighter" undo model for smaller undo-operations (e.g. properties),
//   where the values are basically changed back to their original value.
// - the current load/save model should continue be used for major changes.
// - we should also rewrite the load/save functionality in the software so
//   that we can load/save to/from a std::stringstream. This will avoid the
//   need for ".undoPoint" files.
// - we should also have a undo- and redo-stack of std::stringstream's that
//   will containe multiple undo/redo operations.

void FpPM::vpmSetUndoPoint(const char* /*title*/)
{
  /* This does way too much.... Disabled by KMO
  // Get file names
  int touchedFlagBak = FpPM::touchedFlag;
  bool isUsingDefaultModelName = FpPM::isUsingDefaultModelName;
  std::string absModFileName = FmDB::getMechanismObject()->getModelFileName();
  std::string absLogFileName = FFaFilePath::getBaseName(absModFileName) + ".log";

  // If file exists, then back it up
  if (!isUsingDefaultModelName) {
    FpFileSys::renameFile(absModFileName, absModFileName+".undoBak");
    FFaMsg::getMessager().closeListFile();
    FpFileSys::renameFile(absLogFileName, absLogFileName+".undoBak");
  }

  // Save to "undo point file" (using the main file name)
  vpmModelSaveAs(absModFileName, true, true);

  // Delete old ".undoPoint" files
  FpFileSys::deleteFile(absModFileName+".undoPoint");
  FpFileSys::deleteFile(absLogFileName+".undoPoint");

  // Rename the file to ".undoPoint"
  FpFileSys::renameFile(absModFileName, absModFileName+".undoPoint");
  FFaMsg::getMessager().closeListFile();
  FpFileSys::renameFile(absLogFileName, absLogFileName+".undoPoint");

  // Hide ".undoPoint" files
#ifdef win32
  std::string strTmp;
  strTmp = absModFileName+".undoPoint";
  DWORD dwFileAttrs = ::GetFileAttributes(strTmp.c_str());
  ::SetFileAttributes(strTmp.c_str(), dwFileAttrs | FILE_ATTRIBUTE_HIDDEN);
  strTmp = absLogFileName+".undoPoint";
  dwFileAttrs = ::GetFileAttributes(strTmp.c_str());
  ::SetFileAttributes(strTmp.c_str(), dwFileAttrs | FILE_ATTRIBUTE_HIDDEN);
#endif //win32

  // Restore old file
  if (!isUsingDefaultModelName) {
    FpFileSys::renameFile(absModFileName+".undoBak", absModFileName);
    FpFileSys::renameFile(absLogFileName+".undoBak", absLogFileName);
    FFaMsg::getMessager().openListFile(absLogFileName.c_str());
  }

  // New file?
  if (isUsingDefaultModelName)
    FpPM::isUsingDefaultModelName = true;

  // Touch flag
  FpPM::touchedFlag = touchedFlagBak;
  FpPM::updateGuiTitle();

  // Update sensitivity of the undo command
  FFuaCmdItem* pCmdUndo = FFuaCmdItem::getCmdItem("cmdId_edit_undo");
  std::string strText = "Undo: ";
  strText += title;
  pCmdUndo->setText(strText);
  pCmdUndo->setToolTip(strText);
  FapUACommandHandler::updateAllUICommands();
  */
}


void FpPM::vpmUndo()
{
  std::cerr <<"Sorry, undo not available yet..."<< std::endl;

  /* Temporarily disabled (KMO)
  // Get file names
  int touchedFlagBak = FpPM::touchedFlag;
  bool isUsingDefaultModelName = FpPM::isUsingDefaultModelName;
  std::string absModFileName = FmDB::getMechanismObject()->getModelFileName();
  std::string absLogFileName = FFaFilePath::getBaseName(absModFileName) + ".log";

  // Check that we have a undo point
  if (!FpFileSys::isFile(absModFileName+".undoPoint"))
    return; // no undo point!

  // If main file exists, then back it up
  if (!isUsingDefaultModelName) {
    FpFileSys::renameFile(absModFileName, absModFileName+".undoBak");
    FFaMsg::getMessager().closeListFile();
    FpFileSys::renameFile(absLogFileName, absLogFileName+".undoBak");
  }

  // Rename the ".undoPoint" files to use main file names
  FpFileSys::renameFile(absModFileName+".undoPoint", absModFileName);
  FpFileSys::renameFile(absLogFileName+".undoPoint", absLogFileName);

  // Close current model
  FpPM::touchedFlag = UNTOUCHED;
  if (!FpPM::closeModel())
    return;

  // Load the "undo point file"
  if (!FpPM::vpmModelOpen(absModFileName, true))
    return;

  // Delete old "undo point files"
  FpFileSys::deleteFile(absModFileName);
  FFaMsg::getMessager().closeListFile();
  FpFileSys::deleteFile(absLogFileName);

  // Restore old files
  if (!isUsingDefaultModelName) {
    FpFileSys::renameFile(absModFileName+".undoBak", absModFileName);
    FpFileSys::renameFile(absLogFileName+".undoBak", absLogFileName);
    FFaMsg::getMessager().openListFile(absLogFileName.c_str());
  }

  // New file?
  if (isUsingDefaultModelName)
    FpPM::isUsingDefaultModelName = true;

  // Touch flag
  FpPM::touchedFlag = touchedFlagBak;
  FpPM::updateGuiTitle();

  // Update sensitivity of the undo command
  FapUACommandHandler::updateAllUICommandsSensitivity();
  */
}


void FpPM::vpmGetUndoSensitivity(bool& isSensitive)
{
  isSensitive = FpFileSys::isFile(FmDB::getMechanismObject()->getModelFileName()+".undoPoint");
}


/*!
  \brief Static helper that copies an FE part to a new location.
*/

static bool copyFEPart(FmPart* workPart, int reducedData,
                       const std::string& oldDir, const std::string& newDir,
                       const std::string& oldPath, const std::string& newPath)
{
  bool didSaveSomething = false;
  workPart->myRSD.getValue().setPath(newDir);
  if (workPart->ramUsageLevel.getValue() == FmPart::FULL_FE)
  {
    // This FE part resides in core, export a fresh copy of it
    // (in case it contains some unsaved modifications)
    bool hadOP2Files = workPart->hasOP2files();
    didSaveSomething = workPart->saveFEData(true);
    if (!hadOP2Files && workPart->externalSource.getValue())
      workPart->copyExternalFiles(oldPath,newPath);
  }
  else if (!workPart->baseFTLFile.getValue().empty())
  {
    // This FE part is currently not loaded, copy the last saved version
    FmMechanism* workMech = FmDB::getMechanismObject();
    const std::string& ftlName = workPart->baseFTLFile.getValue();
    ListUI <<"     ["<< workPart->getID() <<"] "<< ftlName;
    if (FpFileSys::verifyDirectory(workMech->getAbsModelRDBPath()))
      if (FpFileSys::verifyDirectory(newDir))
        if ((didSaveSomething = FpFileSys::copyFile(ftlName,oldDir,newDir)))
          if (workPart->externalSource.getValue())
            workPart->copyExternalFiles(oldPath,newPath);

    if (didSaveSomething)
      ListUI <<" (copied)";
    else
      ListUI <<" (failed)\n";
  }

  if (!reducedData)
    return didSaveSomething;

  // Get files from the part RSD
  StringSet filesToCopy;
  if (reducedData == 1) // get everything
    workPart->myRSD.getValue().getAllFileNames(filesToCopy);
  else // only get files needed by the solver processes
  {
    workPart->myRSD.getValue().getAllFileNames(filesToCopy,"fmx");
    if (reducedData > 2) // include sam-file needed for recovery
      workPart->myRSD.getValue().getAllFileNames(filesToCopy,"fsm");
    else // exclude displacement recovery matrices (B- and E)
      for (StringSet::iterator it = filesToCopy.begin(); it != filesToCopy.end();)
      {
        if (it->at(it->size()-5) == 'B' || it->at(it->size()-5) == 'E')
          it = filesToCopy.erase(it);
        else
          ++it;
      }
  }

  if (filesToCopy.empty())
  {
    if (didSaveSomething)
      ListUI <<" (no reduced data)";
    return didSaveSomething;
  }

  // Copy all result files from the FE part reduction
  int nFilesCopied = 0;
  std::string taskName = workPart->myRSD.getValue().getCurrentTaskDirName();
  std::string taskDir = FFaFilePath::appendFileNameToPath(newDir,taskName);
  if (FpFileSys::verifyDirectory(taskDir))
    for (const std::string& fName : filesToCopy)
    {
      std::string oName = FFaFilePath::getRelativeFilename(newDir,fName);
      if (!FpFileSys::copyFile(FFaFilePath::makeItAbsolute(oName,oldDir),fName))
      {
        std::cerr <<" *** Failed to copy "<< oName
                  <<"\n                 to "<< fName << std::endl;
        perror("                  ");
      }
      else if (++nFilesCopied == 1 && !didSaveSomething)
        ListUI <<"     ["<< workPart->getID() <<"]";
    }

  if (nFilesCopied > 0)
  {
    ListUI <<" ("<< nFilesCopied <<" result files copied)";
    didSaveSomething = true;
  }

  return didSaveSomething;
}


/*!
  \brief Static helper that copies the blade design directory to new location.
*/

static bool copyBladeDir(const std::string& oldDir, const std::string& newDir)
{
  if (!FpFileSys::verifyDirectory(newDir))
  {
    ListUI <<"  -> Problems creating "<< newDir <<"\n";
    return false;
  }

  Strings bladeFiles;
  if (!FpFileSys::getFiles(bladeFiles,oldDir,"*.fmm"))
    return true; // Empty old directory, silently ignore

  FmBladeDesign* pDesign = NULL;
  if (FmDB::getTurbineObject() != NULL)
    pDesign = dynamic_cast<FmBladeDesign*>(FmDB::getTurbineObject()->bladeDef.getPointer());

  // Copy all blades from old directory to new one
  bool copySuccesful = true;
  for (const std::string& fName : bladeFiles)
  {
    std::string oldPath = FFaFilePath::appendFileNameToPath(oldDir,fName);
    std::string newPath = FFaFilePath::appendFileNameToPath(newDir,fName);
    if (!FpFileSys::copyFile(oldPath,newPath))
    {
      ListUI <<"  -> Problems copying "<< oldPath <<" to "<< newPath <<"\n";
      copySuccesful = false;
    }

    if (pDesign != NULL && pDesign->myModelFile.getValue() == oldPath)
      pDesign->myModelFile.setValue(newPath);

    // Copy the airfoil files
    Strings airfoilFiles;
    std::string srcAirfoilFolder = FFaFilePath::getBaseName(oldPath).append("_airfoils");
    std::string dstAirfoilFolder = FFaFilePath::getBaseName(newPath).append("_airfoils");
    if (FpFileSys::isDirectory(srcAirfoilFolder))
    {
      if (!FpFileSys::verifyDirectory(dstAirfoilFolder))
      {
        ListUI <<"  -> Problems creating "<< dstAirfoilFolder <<"\n";
        copySuccesful = false;
      }
      else if (FpFileSys::getFiles(airfoilFiles,srcAirfoilFolder,"*.dat"))
        for (const std::string& fName : airfoilFiles)
        {
          std::string oldAirfoil = FFaFilePath::appendFileNameToPath(srcAirfoilFolder,fName);
          std::string newAirfoil = FFaFilePath::appendFileNameToPath(dstAirfoilFolder,fName);
          if (!FpFileSys::copyFile(oldAirfoil,newAirfoil))
          {
            ListUI <<"  -> Problems copying "<< oldAirfoil <<" to "<< newAirfoil <<"\n";
            copySuccesful = false;
          }
        }
    }
  }

  return copySuccesful;
}


bool FpPM::vpmModelSaveAs(const std::string& name, bool saveReducedParts,
                          bool saveResults, double atTime)
{
  FmMechanism* mech = FmDB::getMechanismObject();
  std::string oldRDBPath = mech->getAbsModelRDBPath();
  std::string newRDBPath = FFaFilePath::getBaseName(name) +"_RDB";

  // Check if a name without extension was given
  std::string fmmName(name);
  if (name.size() < 4 || name.substr(name.size()-4) != ".fmm")
    if (FpFileSys::isFile(fmmName.append(".fmm")) && newRDBPath != oldRDBPath)
      if (!FFaMsg::dialog("Model file \"" + fmmName + "\" already exists.\n"
                          "Do you want to replace it?", FFaMsg::YES_NO))
        return FapFileCmds::saveAs(); // Recursively invoke Save-As dialog

  // Check if there is an existing RDB with the same name at the new location
  if (newRDBPath != oldRDBPath && FpFileSys::isDirectory(newRDBPath))
    if (!FFaMsg::dialog("The results directory \"" + newRDBPath + "\" exists.\n"
                        "All the files in this directory will be deleted.\n"
                        "Continue ?", FFaMsg::OK_CANCEL))
      return false;

  bool wasUsingDefaultModelName = FpPM::isUsingDefaultModelName;
  FpPM::isUsingDefaultModelName = false;

  // Check if a new log-file needs to be created
  bool writeLogFile = false;
  if (newRDBPath != oldRDBPath || wasUsingDefaultModelName)
    FFaCmdLineArg::instance()->getValue("logFile", writeLogFile);

  if (writeLogFile)
  {
    // Open a new log-file, copying the contents of the current file
    std::string logFileName = FFaFilePath::getBaseName(name) + ".log";
    FFaMsg::getMessager().openListFile(logFileName.c_str(),true);
  }

  // Check if we save to ourselves
  if (newRDBPath == oldRDBPath)
    return atTime < 0.0 && FpPM::isModelTouchable() ? FpPM::vpmModelSave(true) : false;

  // OK, we are saving to a new model

  Fui::resultFileBrowserUI(false);
  FpPM::touchedFlag = DONT_TOUCH; // suppress touching while saving to new model

  // Reset simulation event changes, if any
  if (FapSimEventHandler::activate(NULL,true,false))
    ListUI <<"===> Switching to master Simulation event.\n";

  Fui::noUserInputPlease();
  ListUI <<"===> Saving Fedem model in "<< fmmName <<"\n";

  if (FpFileSys::isDirectory(newRDBPath))
  {
    ListUI <<"===> Results directory "<< newRDBPath
           <<" already exists, and will be deleted.\n";
    FpFileSys::removeDir(newRDBPath);
  }

  std::vector<FmPart*> allParts;
  FmDB::getAllParts(allParts);

  // Make sure the part RSDs are up to date with data on disk
  std::set<FmPart*> saveReducedPart;
  for (FmPart* part : allParts)
    if (!saveReducedParts)
      part->clearSupelFiles(false);
    else if (!part->externalSource.getValue() && FapLinkReducer::isReduced(part))
      saveReducedPart.insert(part);

  std::string oldModelP = mech->getAbsModelFilePath();
  std::string oldPartDB = mech->getAbsModelLRDBPath();

  std::string oldBladeDirectory = mech->getAbsBladeFolderPath();

  // Save the simulation results, if requested
  if (atTime < 0.0)
  {
    if (saveResults)
      FFaMsg::list("  -> Saving Results\n");
    else
      FFaMsg::list("  -> Results discarded.\n");
    FapSimEventHandler::RDBSaveAs(newRDBPath,!saveResults);
    if (saveResults)
      FFaMsg::list("  -> Done saving results\n");
  }
  else
  {
    // Instead of keeping the results database,
    // we update all position matrices in the model
    // to reflect the configuration at specified time.
    // Thereby defining the new stress-free modelling configuration.
    FFaMsg::list("  -> Update model configuration\n");
    atTime = FpModelRDBHandler::updateModel(atTime);
    FapSimEventHandler::RDBSaveAs(newRDBPath,true);
    if (atTime < 0.0)
      ListUI <<"  -> Failed to update model configuration at time = "<< -atTime
             <<"\n     Existing results are discarded.\n";
    else
    {
      ListUI <<"  -> Model successfully updated with the state at time = "<< atTime <<"\n";
      FFaMsg::pushStatus("Update visualization");
      FmDB::displayAll();
      FFaMsg::popStatus();
    }
  }

  // Update the mechanism to reflect path name changes
  mech->syncPath(fmmName);

  // Translate all relative path names according to new model file location
  const std::string& newModelP = mech->getAbsModelFilePath();
  FmDB::translateRelativePaths(oldModelP,newModelP);
  std::string newPartDB = mech->getAbsModelLRDBPath();

  int nSaved = 0; // Save all part files ...
  for (FmPart* workPart : allParts)
  {
    // ... but not for those already saved and using an external repository
    if (workPart->usesRepository() && workPart->isSaved())
      continue;

    if (++nSaved == 1)
      ListUI <<"  -> Saving FE parts:\n";

    int red = saveReducedPart.find(workPart) != saveReducedPart.end();
    if (copyFEPart(workPart,red,oldPartDB,newPartDB,oldModelP,newModelP))
    {
      ListUI <<"  ... Done\n";
      workPart->onChanged();
    }
  }

  // Delete the old RDB completely if it belonged to an untitled model
  if (wasUsingDefaultModelName)
    if (FpFileSys::isDirectory(oldRDBPath))
      FpFileSys::removeDir(oldRDBPath);

  // Copy the blade design of the model, if any such exist
  if (FpFileSys::isDirectory(oldBladeDirectory))
    if (copyBladeDir(oldBladeDirectory,mech->getAbsBladeFolderPath()))
      // Delete the old blade folder if it belonged to an untitled model
      if (wasUsingDefaultModelName)
        FpFileSys::removeDir(oldBladeDirectory);

  // Delete in-active joint springs, -dampers, -loads and -motions
  bool purgeOnSave = false;
  FFaCmdLineArg::instance()->getValue("purgeOnSave", purgeOnSave);
  if (purgeOnSave)
    FmDB::purgeJointComponents();

  // Finally save the model file
  FFaMsg::list("  -> Saving Model File ");
  bool isModelSaved = false;
  std::ofstream s(fmmName,std::ios::out);
  if (s)
  {
    FmDB::updateModelVersionOnSave(false);
    FmSubAssembly::mainFilePath = newModelP;
    isModelSaved = FmDB::reportAll(s);
    s.close();
  }
  if (isModelSaved)
    FFaMsg::list(" ... Done.\n");
  else
    FFaMsg::list(" ... Failed!\n",true);

  // Reopen the RDB from the new location
  FpModelRDBHandler::RDBOpen(mech->getResultStatusData(),mech,true);

  // We are through ...
  const time_t currentTime = time(NULL);
  FFaMsg::list("===> Done saving: " + std::string(ctime(&currentTime)));

  // Reset the file dialog memorizers if we save to a new location.
  // But only for those that actually were using the model file path.
  if (newModelP != oldModelP)
    FFuFileDialog::resetMemoryMap(newModelP,oldModelP);

  Fui::okToGetUserInput();

  if (!isModelSaved)
    ListUI <<"===> WARNING: The model was not completely saved to the new location.\n"
           <<"     Review the messages above to find out what was actually saved.\n"
           <<"     Try to free some space on disk and save the model again.\n"
           <<"     If you exit Fedem now you will loose any unsaved data.\n\n";

  // Move the file name to the top of the recent file list
  FpPM::addRecent(fmmName);
  Fui::updateUICommands();

  FapEventManager::permUnselectAll();
  return isModelSaved;
}


bool FpPM::vpmModelExport(const std::string& name, FmAnalysis* analysis, const char* model)
{
  FmMechanism* mech = FmDB::getMechanismObject();
  std::string oldRDBPath = mech->getAbsModelRDBPath();
  std::string newRDBPath = FFaFilePath::getBaseName(name) +"_RDB";
  bool exportSolverInput = analysis != NULL;

  // Check if we export to ourselves (not allowed)
  if (newRDBPath == oldRDBPath) return false;

  // Check if there is an existing RDB with the same name at the new location
  bool removeRDBdir = FpFileSys::isDirectory(newRDBPath);
  if (removeRDBdir)
    if (!FFaMsg::dialog("The results directory \"" + newRDBPath + "\" exists.\n"
                        "All the files in this directory will be deleted.\n"
                        "Continue ?", FFaMsg::OK_CANCEL))
      return false;

  Fui::resultFileBrowserUI(false);
  int touchedStatus = FpPM::touchedFlag;
  FpPM::touchedFlag = DONT_TOUCH; // suppress touching while exporting the model

  // Reset simulation event changes, if any
  if (FapSimEventHandler::activate(NULL,true,false))
    ListUI <<"===> Switching to master Simulation event.\n";

  Fui::noUserInputPlease();
  ListUI <<"===> Exporting Fedem model to "<< name <<"\n";

  if (removeRDBdir)
  {
    ListUI <<"===> Results directory "<< newRDBPath
           <<" already exists, and will be deleted.\n";
    FpFileSys::removeDir(newRDBPath);
  }

  std::vector<FmPart*> allParts;
  FmDB::getAllParts(allParts);

  // Find set of FE parts with valid reduction data
  Strings oldPartDB;
  oldPartDB.reserve(allParts.size());
  std::set<FmPart*> saveReducedPart;
  for (FmPart* part : allParts)
  {
    oldPartDB.push_back(part->getAbsFilePath());
    if (!part->externalSource.getValue() && FapLinkReducer::isReduced(part))
      saveReducedPart.insert(part);
  }

  // Store old file paths
  std::string oldModelN = mech->getModelFileName();
  std::string oldModelP = mech->getAbsModelFilePath();
  std::string oldPartRepository = mech->modelLinkRepository.getValue();
  std::string oldBladeDirectory = mech->getAbsBladeFolderPath();

  // Update the mechanism to reflect path name changes
  mech->syncPath(name,exportSolverInput);
  mech->modelLinkRepository.setValue("");

  // Exported model file location
  const std::string& newModelP = mech->getAbsModelFilePath();
  std::string newRootP(newModelP);
  if (exportSolverInput)
    newRootP = FFaFilePath::getPath(FFaFilePath::getPath(newModelP),false);

  // Find all path names in the model and store a pointer to each in a vector
  std::vector<FFaField<std::string>*> filePaths;
  FmDB::getAllPaths(filePaths);
  // Keep track of the old path names
  StringSet uniquePathNames;
  Strings oldPathNames;
  oldPathNames.reserve(filePaths.size());
  // Modify the path names to become relative to the exported model location
  // and copy the referenced files physically
  for (FFaField<std::string>* field : filePaths)
  {
    oldPathNames.push_back(field->getValue());
    std::string oldPath = oldPathNames.back();
    FFaFilePath::makeItAbsolute(oldPath,oldModelP);
    if (FpFileSys::isFile(oldPath))
    {
      std::string newName = FFaFilePath::getFileName(oldPath);
      field->setValue(newName);
      if (uniquePathNames.insert(oldPath).second)
      {
        if (FpFileSys::copyFile(oldPath,
                                FFaFilePath::appendFileNameToPath(newRootP,
                                                                  newName)))
          ListUI <<"  -> Copied "<< newName <<" to "<< newRootP <<"\n";
        else
          ListUI <<" *** Failed to copy "<< newName <<"\n";
      }
    }
    else
    {
      ListUI <<"  -> Skipping "<< oldPath <<" which is not a file\n";
      field->setValue(""); // Switch from link-specific to internal repository
    }
  }

  std::string newPartDB = mech->getAbsModelLRDBPath(!allParts.empty());
  if (!allParts.empty() && !newPartDB.empty())
  {
    // Export all FE part files
    ListUI <<"  -> Exporting FE parts:\n";
    size_t iPart = 0;
    for (FmPart* part : allParts)
    {
      int red = saveReducedPart.find(part) != saveReducedPart.end();
      if (red && exportSolverInput)
        // Only include those reduction files needed by the solvers
        red = part->recoveryDuringSolve.getValue() > 0 ? 3 : 2;
      const std::string& oldDir = oldPartDB[iPart++];
      if (copyFEPart(part,red,oldDir,newPartDB,oldModelP,newModelP))
        ListUI <<"  ... Done\n";
    }
  }

  // Copy the blade design of the model, if any such exist
  if (!exportSolverInput && FpFileSys::isDirectory(oldBladeDirectory))
    copyBladeDir(oldBladeDirectory,mech->getAbsBladeFolderPath());

  bool isModelSaved = false;
  if (exportSolverInput)
  {
    // Create solver input files for batch execution
    Strings rdbPath;
    std::string msg = Fedem::createSolverInput(analysis,mech,NULL,"fedem_solver",{},rdbPath);
    if (msg.find("fedem_solver") > 0) // Failure
      FFaMsg::list(msg + "\n",true);
    else if (model && !rdbPath.empty())
    {
      // The solver input files were created in mech->getAbsModelRDBPath()/response_0001
      // Now move them two levels up to where we want them to be
      std::string modelDir = FFaFilePath::getPath(newModelP);
      isModelSaved = FpFileSys::renameFile(rdbPath.front(), modelDir + model);
      if (isModelSaved)
        ListUI <<"  -> Solver input files exported to "<< modelDir << model <<"\n";
      else
        std::cerr <<" *** Failed to move directory "<< rdbPath.front() << std::endl;
      if (!newPartDB.empty() && FpFileSys::renameFile(newPartDB, modelDir + "link_DB"))
        ListUI <<"  -> Internal link repository exported to "<< modelDir <<"link_DB\n";
      else if (!newPartDB.empty())
        std::cerr <<" *** Failed to move directory "<< newPartDB << std::endl;
      if (FpFileSys::removeDir(FFaFilePath::getPath(newRDBPath)) < 0)
        std::cerr <<" *** Failed to delete directory "<< FFaFilePath::getPath(newRDBPath) << std::endl;
    }
    else
      ListUI <<" ==> Solver input files exported to "<< mech->getAbsModelRDBPath() <<"\n";
  }
  else
  {
    // Finally, save the model file
    FFaMsg::list("  -> Writing Model File ");
    std::ofstream s(name,std::ios::out);
    if (s)
    {
      FmDB::updateModelVersionOnSave(false);
      FmSubAssembly::mainFilePath = newModelP;
      isModelSaved = FmDB::reportAll(s);
      s.close();
    }
    if (isModelSaved)
      FFaMsg::list(" ... Done.\n");
    else
      FFaMsg::list(" ... Failed!\n",true);
  }

  // We are through ...
  const time_t currentTime = time(NULL);
  FFaMsg::list("===> Done exporting: " + std::string(ctime(&currentTime)));

  // Reset file path names in the model
  mech->syncPath(oldModelN,exportSolverInput);
  mech->modelLinkRepository.setValue(oldPartRepository);
  for (size_t i = 0; i < allParts.size(); i++)
    allParts[i]->myRSD.getValue().setPath(oldPartDB[i]);
  for (size_t i = 0; i < oldPathNames.size(); i++)
    filePaths[i]->setValue(oldPathNames[i]);
  FmSubAssembly::mainFilePath = oldModelP;
  FpPM::touchedFlag = touchedStatus;
  Fui::okToGetUserInput();

  return isModelSaved;
}


void FpPM::setResultFlag()
{
  // Check the result status for the active event
  FpPM::resultsFlag = 0;
  FmResultStatusData* rsd = FapSimEventHandler::getActiveRSD();
  if (rsd)
  {
    if (FpModelRDBHandler::hasResults(rsd, "timehist_prim"))
      FpPM::resultsFlag += PRIMARY;
    if (FpModelRDBHandler::hasResults(rsd, "timehist_sec"))
      FpPM::resultsFlag += SECONDARY;
    if (FpModelRDBHandler::hasResults(rsd, "eigval"))
      FpPM::resultsFlag += EIGVAL;
    if (FpModelRDBHandler::hasResults(rsd, "timehist_rcy"))
      FpPM::resultsFlag += STRESS_RCY;
    if (FpModelRDBHandler::hasResults(rsd, "eigval_rcy"))
      FpPM::resultsFlag += EIGVAL_RCY;
    if (FpModelRDBHandler::hasResults(rsd, "timehist_gage_rcy"))
      FpPM::resultsFlag += GAGE_RCY;
    if (FpModelRDBHandler::hasResults(rsd, "summary_rcy"))
      FpPM::resultsFlag += SUMMARY_RCY;
    if (rsd->hasFileNames("res"))
      FpPM::resultsFlag += RES_FILE;
  }

  // Notify UI about possibly change in result state
  Fui::lockModel(FpPM::resultsFlag > 0 || FpPM::touchedFlag == READ_ONLY ||
                 FapSimEventHandler::hasResults());
}


bool FpPM::hasResults(RDBType mask)
{
  if (mask == ANY)
    return FpPM::resultsFlag > 0;
  else
    return FpPM::resultsFlag & mask ? true : false;
}


std::string FpPM::createUuid()
{
  return QUuid::createUuid().toString().toStdString();
}


/*!
  Signal handler interface.

  We are **not** going to do X-things inside the signal handler,
  therefore the X-event loop is not considered here.
  We also use unreliable signals in sake of simplicity -
  should be easy to upgrade to reliable signals later if neccessary.
  We do **not** handle signals in debug mode (sometimes a coredump is needed)
*/

void FpPM::signalHandler(int sig)
{
  std::string strsig;
  enum { SAVE_AND_EXIT, EXIT, CLEAN_UP_CHILDREN } action;

  switch (sig)
    {
    case SIGINT: // User presses ctrl-C.
      strsig = "SIGINT Interrupt";
      action = EXIT;
      break;

#if !defined(win32) && !defined(win64)
    case SIGQUIT: // User presses ctrl-\.
      strsig = "SIGQUIT Quit";
      action = EXIT;
      break;

    case SIGILL: // Illegal hardware instruction.
      strsig = "SIGILL Illegal instruction";
      action = SAVE_AND_EXIT;
      break;

    case SIGABRT: // Generated by abort function.
      strsig = "SIGABRT Abort";
      action = SAVE_AND_EXIT;
      break;

    case SIGFPE: // Division by 0, overflow, etc.
      strsig = "SIGFPE Arithmetic exception";
      action = SAVE_AND_EXIT;
      break;

    case SIGBUS: // Bus error.
      strsig = "SIGBUS Bus error";
      action = SAVE_AND_EXIT;
      break;

    case SIGSEGV: // Segmentation violation.
      strsig = "SIGSEGV Segmentation violation";
      action = SAVE_AND_EXIT;
      break;

#ifndef linux
    case SIGSYS: // Bad system call.
      strsig = "SIGSYS Bad system call";
      action = EXIT;
      break;
#endif

    case SIGPIPE: // Broken pipe.
      strsig = "SIGPIPE Broken pipe";
      action = CLEAN_UP_CHILDREN;
      break;

    case SIGTERM: // Software termination sent by kill and system shutdown.
      strsig = "SIGTERM Software termination";
      action = EXIT;
      break;

    case SIGXCPU: // CPU time limit exceeded.
      strsig = "SIGXCPU CPU time limit exceeded";
      action = SAVE_AND_EXIT;
      break;

    case SIGXFSZ: // File size limit exceeded.
      strsig = "SIGXFSZ File size limit exceeded";
      action = SAVE_AND_EXIT;
      break;
#endif

    default: // Unknown signal - do nothing
      return;
    }

  std::cout <<"\n *** Fedem recieved signal "<< strsig << std::endl;

  switch (action) {
  case SAVE_AND_EXIT:
    std::cout <<" *** Fedem trying emergency save and exit"<< std::endl;
    FmDB::emergencyExitSave();
    FpPM::quitVPM(-1);
    break;

  case EXIT:
    std::cout <<" *** Fedem exiting"<< std::endl;
    FpPM::quitVPM(-1);
    break;

  case CLEAN_UP_CHILDREN:
    std::cout <<" *** Fedem cleaning up child processes"<< std::endl;
    FpProcessManager::instance()->killAll();
    signal(sig,&FpPM::signalHandler); // Resetting signal handler
  }
}


void FpPM::onModelMemberConnected(FmModelMemberBase*)
{
  FpPM::touchModel();
}


void FpPM::onModelMemberChanged(FmModelMemberBase* item)
{
  if (item == FmDB::getMechanismObject())
    FpPM::toggleActivePlugins(FmDB::getMechanismObject());

  FpPM::touchModel();
}
