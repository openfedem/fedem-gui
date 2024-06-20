// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapFileCmds.H"
#if defined(FT_HAS_WND) || defined(FT_HAS_RIS) || defined(FT_HAS_OWL)
#include "vpmApp/vpmAppCmds/FapOilWellCmds.H"
#endif
#include "vpmApp/FapLicenseManager.H"
#include "vpmApp/vpmAppProcess/FapSimEventHandler.H"
#include "vpmApp/vpmAppProcess/FapSolutionProcessMgr.H"
#include "vpmApp/vpmAppProcess/FapLinkReducer.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFuLib/FFuAuxClasses/FFuaIdentifiers.H"

#include "vpmPM/FpModelRDBHandler.H"
#include "vpmPM/FpFileSys.H"
#include "vpmPM/FpPM.H"
#include "vpmUI/Fui.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmAnalysis.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmStrainRosette.H"
#include "vpmDB/FmAssemblyBase.H"
#ifdef USE_INVENTOR
#include "vpmDB/FmGlobalViewSettings.H"
#include "vpmDisplay/FdPart.H"
#endif
#if defined(FT_HAS_SEV)
#include "assemblyCreators/eventCreator.H"
#endif

#include "FFuLib/FFuFileDialog.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuIOField.H"
#include "FFlLib/FFlLinkHandler.H"
#include "FFlLib/FFlIOAdaptors/FFlReaders.H"
#include "FFlLib/FFlIOAdaptors/FFlAllIOAdaptors.H"
#include "FFaLib/FFaAlgebra/FFaUnitCalculator.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"

#ifdef FT_USE_PROFILER
#include "FFaLib/FFaProfiler/FFaProfiler.H"
#endif

#include <fstream>

typedef std::pair<FmPart*,int> FmPartInt;

//----------------------------------------------------------------------------

void FapFileCmds::init()
{
  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_file_nu");
  cmdItem->setSmallIcon(new_xpm);
  cmdItem->setText("New...");
  cmdItem->setToolTip("New");
  cmdItem->setAccelKey(FFuaKeyCode::CtrlAccel+FFuaKeyCode::N);
  cmdItem->setActivatedCB(FFaDynCB0S(FpPM::vpmModelNew));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapFileCmds::getChangeModelSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_file_open");
  cmdItem->setSmallIcon(open_xpm);
  cmdItem->setText("Open...");
  cmdItem->setToolTip("Open");
  cmdItem->setAccelKey(FFuaKeyCode::CtrlAccel+FFuaKeyCode::O);
  cmdItem->setActivatedCB(FFaDynCB0S(FapFileCmds::open));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapFileCmds::getChangeModelSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_file_save");
  cmdItem->setSmallIcon(save_xpm);
  cmdItem->setText("Save");
  cmdItem->setToolTip("Save");
  cmdItem->setAccelKey(FFuaKeyCode::CtrlAccel+FFuaKeyCode::S);
  cmdItem->setActivatedCB(FFaDynCB0S(FapFileCmds::save));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelTouchable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_file_saveAs");
  cmdItem->setText("Save As...");
  cmdItem->setToolTip("Save As");
  cmdItem->setActivatedCB(FFaDynCB0S(FapFileCmds::saveAs));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapFileCmds::getChangeModelSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_file_exit");
  cmdItem->setText("Exit");
  cmdItem->setToolTip("Exit");
  cmdItem->setAccelKey(FFuaKeyCode::CtrlAccel+FFuaKeyCode::Q);
  cmdItem->setActivatedCB(FFaDynCB0S(FapFileCmds::exit));

  cmdItem = new FFuaCmdItem("cmdId_file_loadLink");
  cmdItem->setSmallIcon(getFELink_xpm);
  cmdItem->setText("Load Part...");
  cmdItem->setToolTip("Load Part");
  // Removed since it does not work after the Qt4 port, see bug #57.
  //cmdItem->setAccelKey(FFuaKeyCode::CtrlAccel+FFuaKeyCode::L);
  cmdItem->setActivatedCB(FFaDynCB0S(FapFileCmds::loadLink));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_file_changeLink");
  cmdItem->setText("Change Part...");
  cmdItem->setToolTip("Change Part");
  cmdItem->setActivatedCB(FFaDynCB0S(FapFileCmds::changeLink));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapFileCmds::getChangeLinkSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_file_setLinkRepository");
  cmdItem->setText("Set FE model repository...");
  cmdItem->setToolTip("Set FE model repository");
  cmdItem->setActivatedCB(FFaDynCB0S(FapFileCmds::setExternalLinkRep));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapFileCmds::getSetModelLinkRepSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_file_createPipeSurface");
  cmdItem->setText("Import Pipe Surface...");
  cmdItem->setToolTip("Create a pipe surface for contact from input file");
  cmdItem->setActivatedCB(FFaDynCB0S(FapFileCmds::importPipeSurface));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_file_createPipeString");
  cmdItem->setText("Import Pipe String...");
  cmdItem->setToolTip("Create a pipe string from input file");
  cmdItem->setActivatedCB(FFaDynCB0S(FapFileCmds::importPipeString));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_file_createDrillString");
  cmdItem->setText("Create Drill String...");
  cmdItem->setToolTip("Create a drill string from imported arc-segment and riser");
  cmdItem->setActivatedCB(FFaDynCB0S(FapFileCmds::importDrillString));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_file_createRiser");
  cmdItem->setSmallIcon(beamstring_xpm);
  cmdItem->setText("Import Beamstring...");
  cmdItem->setToolTip("Create a beamstring model from input file");
  cmdItem->setActivatedCB(FFaDynCB0S(FapFileCmds::importRiser));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_file_createJacket");
  cmdItem->setSmallIcon(jacket_xpm);
  cmdItem->setText("Import Spaceframe...");
  cmdItem->setToolTip("Create a spaceframe model from FE data file");
  cmdItem->setActivatedCB(FFaDynCB0S(FapFileCmds::importJacket));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_file_createPile");
  cmdItem->setSmallIcon(soilpile_xpm);
  cmdItem->setText("Import Soil Pile...");
  cmdItem->setToolTip("Create a soil pile from input file");
  cmdItem->setActivatedCB(FFaDynCB0S(FapFileCmds::importSoilPile));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_file_importAssembly");
  cmdItem->setText("Import Subassembly...");
  cmdItem->setToolTip("Imports a subassembly that has been stored in a separate file from another model");
  cmdItem->setActivatedCB(FFaDynCB0S(FapFileCmds::importAssembly));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_file_createEvents");
  cmdItem->setSmallIcon(eventDef_xpm);
  cmdItem->setText("Import Events...");
  cmdItem->setToolTip("Create simulation events from input file");
  cmdItem->setActivatedCB(FFaDynCB0S(FapFileCmds::importEvents));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_file_importBeamCS");
  cmdItem->setSmallIcon(beamProp_xpm);
  cmdItem->setText("Import Beam Cross Sections...");
  cmdItem->setToolTip("Import generic beam cross sections from database");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ Fui::beamCSSelectorUI(true); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelTouchable,bool&));

  for (int i = 0; i < 10; i++)
  {
    cmdItem = new FFuaCmdItem("cmdId_file_recent" + std::to_string(i));
    cmdItem->setSmallIcon(open_xpm);
    cmdItem->setGetSensitivityCB(FFaDynCB1S(FapFileCmds::getChangeModelSensitivity,bool&));
    switch (i) {
    case 0: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapFileCmds::open(0); })); break;
    case 1: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapFileCmds::open(1); })); break;
    case 2: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapFileCmds::open(2); })); break;
    case 3: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapFileCmds::open(3); })); break;
    case 4: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapFileCmds::open(4); })); break;
    case 5: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapFileCmds::open(5); })); break;
    case 6: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapFileCmds::open(6); })); break;
    case 7: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapFileCmds::open(7); })); break;
    case 8: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapFileCmds::open(8); })); break;
    case 9: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapFileCmds::open(9); })); break;
    }
  }
}
//----------------------------------------------------------------------------

static FFuFileDialog* openFile(const char* title)
{
  return FFuFileDialog::create(FmDB::getMechanismObject()->getAbsModelFilePath(),
                               title, FFuFileDialog::FFU_OPEN_FILE, true);
}

static FFuFileDialog* openFiles(const char* title)
{
  return FFuFileDialog::create(FmDB::getMechanismObject()->getAbsModelFilePath(),
                               title, FFuFileDialog::FFU_OPEN_FILES, true);
}
//----------------------------------------------------------------------------

void FapFileCmds::open()
{
  FFuFileDialog* aDialog = openFile("Open model file");
  aDialog->addFilter("Fedem model file", "fmm");
  aDialog->addAllFilesFilter(true);
  aDialog->addUserToggle("LoadParts", "Skip FE-Data (ignore FE-Data Settings)", false);
  aDialog->remember("ModelFileManip");

  std::vector<std::string> retFiles = aDialog->execute();
  bool skipParts = aDialog->getUserToggleSet("LoadParts");
  delete aDialog;

  if (!retFiles.empty())
    FapFileCmds::open(retFiles.front(),!skipParts);
}
//----------------------------------------------------------------------------

bool FapFileCmds::open(const std::string& fileName, bool loadParts)
{
  // Check the specified file. If the file does not exist or cannot be opened,
  // cancel the open command without closing the current model.
  if (!FpFileSys::isFile(fileName)) {
    FFaMsg::dialog("The file: \"" + fileName + "\" does not exist.");
    return false;
  }

  if (!FpFileSys::isReadable(fileName)) {
    FFaMsg::dialog("The file: \"" + fileName + "\" can not be opened.\n"
                   "Check that you have proper read permissions to the file.");
    return false;
  }

#ifdef FT_USE_PROFILER
  FFaMemoryProfiler::reportMemoryUsage("Empty");
  FFaMemoryProfiler::nullifyMemoryUsage("Empty");
#endif

  // If the model opening fails, create an empty new model instead
  bool success = true;
  if (FpPM::closeModel())
    if (!(success = FpPM::vpmModelOpen(fileName,loadParts)))
      FpPM::vpmModelNew();

#ifdef FT_USE_PROFILER
  FFaMemoryProfiler::reportMemoryUsage("ModelOpen");
  FFaMemoryProfiler::nullifyMemoryUsage("ModelOpen");
#endif
  return success;
}
//----------------------------------------------------------------------------

void FapFileCmds::open(size_t recentFileIdx)
{
  const std::vector<std::string>& recentFiles = FpPM::recentModels();
  if (recentFileIdx < recentFiles.size())
    if (!FapFileCmds::open(recentFiles[recentFileIdx],true))
      FpPM::removeRecent(recentFileIdx);
}
//----------------------------------------------------------------------------

void FapFileCmds::save(bool pruneEmptyDirs)
{
  if (FpPM::isUsingDefaultName())
    FapFileCmds::saveAs();
  else if (FmDB::getMechanismObject()->getModelFileName().empty())
    FapFileCmds::saveAs();
  else
    FpPM::vpmModelSave(pruneEmptyDirs);
}
//----------------------------------------------------------------------------

class FapSaveAsOptions
{
public:
  FFuToggleButton* discardTgl;
  FFuToggleButton* updateTgl;
  FFuIOField*      timeFld;

  FapSaveAsOptions() : discardTgl(NULL), updateTgl(NULL), timeFld(NULL) {}

  void onDiscardFEred(bool onOff)
  {
    if (onOff && discardTgl)
      discardTgl->setValue(true);
    if (onOff)
      this->onDiscardToggled(true);
  }

  void onDiscardToggled(bool onOff)
  {
    if (onOff && updateTgl)
      updateTgl->setValue(false);
    if (updateTgl)
      updateTgl->setSensitivity(!onOff);
    if (onOff && timeFld)
      timeFld->setSensitivity(false);
  }

  void onUpdateToggled(bool onOff)
  {
    if (timeFld)
      timeFld->setSensitivity(onOff);
  }
};


void FapFileCmds::saveAs()
{
  // Lambda function checking if reduced FE parts exist in current model.
  auto&& haveReducedParts = []()
  {
    std::vector<FmPart*> allParts;
    FmDB::getAllParts(allParts);
    for (FmPart* part : allParts)
      if (part->hasResults())
        return true;
    return false;
  };

  FapSaveAsOptions options;
  FFuFileDialog* aDialog = FFuFileDialog::create(FmDB::getMechanismObject()->getAbsModelFilePath(),
                                                 "Save model file as", FFuFileDialog::FFU_SAVE_FILE);
  aDialog->setDefaultName(FmDB::getMechanismObject()->getModelFileName());
  aDialog->addFilter("Fedem model file", "fmm");
  if (haveReducedParts())
  {
    options.discardTgl = aDialog->addUserToggle("dontSaveReduced", "Discard reduced FE models", false);
    options.discardTgl->setToggleCB(FFaDynCB1M(FapSaveAsOptions,&options,onDiscardFEred,bool));
  }
  if (FpPM::isModelTouchable() && FpPM::hasResults(FpPM::PRIMARY))
  {
    options.discardTgl = aDialog->addUserToggle("dontSaveRes", "Discard simulation results", false);
    options.discardTgl->setToggleCB(FFaDynCB1M(FapSaveAsOptions,&options,onDiscardToggled,bool));
    options.updateTgl  = aDialog->addUserToggle("updatConfig", "Create new modeling configuration", false);
    options.updateTgl->setToggleCB(FFaDynCB1M(FapSaveAsOptions,&options,onUpdateToggled,bool));
    options.timeFld = aDialog->addUserField("Model configuration time:",
                                            FmDB::getActiveAnalysis()->stopTime.getValue());
    options.timeFld->setSensitivity(false);
  }
  else
    options.discardTgl = NULL;
  aDialog->remember("ModelFileManip");

  double atTime = -1.0;
  std::vector<std::string> retFiles = aDialog->execute();
  bool saveRedParts = !aDialog->getUserToggleSet("dontSaveReduced");
  bool saveRes = !aDialog->getUserToggleSet("dontSaveRes");
  if (aDialog->getUserToggleSet("updatConfig"))
    atTime = aDialog->getUserFieldValue("Model configuration time:");
  delete aDialog;

  if (!retFiles.empty())
    if (FpPM::vpmModelSaveAs(retFiles.front(),saveRedParts,saveRes,atTime))
      FpPM::unTouchModel();
}
//----------------------------------------------------------------------------

void FapFileCmds::exit(bool saveOnBatchExit, bool pruneEmptyDirs, int status)
{
  if (FpPM::closeModel(saveOnBatchExit,pruneEmptyDirs,true))
    FpPM::quitVPM(status);
}
//----------------------------------------------------------------------------

static void drawPart(FmPart* part, const std::string& fName)
{
  FFaMsg::pushStatus("Creating visualization");
  FFaMsg::setSubTask(FFaFilePath::getFileName(fName));

  part->draw();

#ifdef USE_INVENTOR
  // Scale the local beam axis system markers, if any
  FdPart* fdp = static_cast<FdPart*>(part->getFdPointer());
  if (fdp) fdp->updateSpecialLines(FmDB::getActiveViewSettings()->getSymbolScale());
#endif

  std::vector<FmTriad*> triads;
  part->getTriads(triads);
  for (FmTriad* triad : triads)
    triad->draw();

  FFaMsg::popStatus();
  FFaMsg::setSubTask("");
}
//----------------------------------------------------------------------------

void FapFileCmds::loadLink()
{
  FFl::initAllReaders();

  // Set up the file dialog
  FFuFileDialog* aDialog = openFiles("Load Part");

  // Obtain filters from the part reader factory
  bool vrmlIsDefault = (FapLicenseManager::isFreeEdition() ||
			FapLicenseManager::isLimEdition());
  FFlReaderInfo defReader = FFlReaders::instance()->getDefaultReader();
  std::vector<FFlReaderInfo> regReaders;
  if (vrmlIsDefault || defReader.name.empty())
    FFlReaders::instance()->getRegisteredReaders(regReaders);
  else
  {
    FFlReaders::instance()->getRegisteredReaders(regReaders, false);
    aDialog->addFilter(defReader.name, defReader.extensions, true);
  }

  for (const FFlReaderInfo& reader : regReaders)
    aDialog->addFilter(reader.name, reader.extensions);

  // ...and for CAD-model file names
  std::vector<std::string> vrmlExts;
  vrmlExts.push_back("wrl");
  vrmlExts.push_back("vrml");
  vrmlExts.push_back("vrl");
  vrmlExts.push_back("wrz");
  aDialog->addFilter("VRML model", vrmlExts, vrmlIsDefault);
  /*
  vrmlExts.clear();
  vrmlExts.push_back("igs");
  vrmlExts.push_back("iges");
  aDialog->addFilter("IGES model", vrmlExts);

  vrmlExts.clear();
  vrmlExts.push_back("stp");
  vrmlExts.push_back("step");
  aDialog->addFilter("STEP based model", vrmlExts);

  vrmlExts.clear();
  vrmlExts.push_back("brep");
  vrmlExts.push_back("rle");
  aDialog->addFilter("Open Cascade based model", vrmlExts);
  */
  vrmlExts.clear();
  vrmlExts.push_back("ftc");
  aDialog->addFilter("Fedem Technology CAD model", vrmlExts);

  aDialog->addAllFilesFilter(true);

  std::vector<std::string> definedCalcs;
  FFaUnitCalculatorProvider::instance()->getCalculatorNames(definedCalcs);
  if (!definedCalcs.empty())
  {
    aDialog->addUserOptionMenu("UC", "Unit conversion:");
    aDialog->addUserOption("UC", "none");
    for (const std::string& calc : definedCalcs)
      aDialog->addUserOption("UC", calc);
  }
  aDialog->addUserToggle("autoTriads", "Create triads at spider reference nodes", false);
  aDialog->addUserToggle("relToggle", "Use path relative to model-file location", true);
  aDialog->addUserToggle("LLR", "Use part-specific repository for this part", false);
  aDialog->remember("OpenPart");

  // Execute the file dialog
  std::vector<std::string> retFiles = aDialog->execute();
  std::string converter = aDialog->getSelectedUserOption("UC");
  bool useRelativePath = aDialog->getUserToggleSet("relToggle");
  bool useSpecificPartRepository = aDialog->getUserToggleSet("LLR");
  bool autoGenTriads = aDialog->getUserToggleSet("autoTriads");
  delete aDialog;
  if (retFiles.empty()) return;

  FpPM::vpmSetUndoPoint("Load part");

  const FFaUnitCalculator* uc = FFaUnitCalculatorProvider::instance()->getCalculator(converter);

  std::vector<FmModelMemberBase*> csel;
  FapCmdsBase::getCurrentSelection(csel);
  FmAssemblyBase* parent = csel.size() == 1 ? dynamic_cast<FmAssemblyBase*>(csel.front()) : NULL;

  // Loop over all selected files; check the file names,
  // create parts, read data and set up the part properties

  int partNr = 0;
  Fui::noUserInputPlease();
  FFaMsg::enableSubSteps(retFiles.size());
  for (const std::string& fName : retFiles)
  {
    partNr++;
    if (!FpFileSys::isReadable(fName))
    {
      std::string warnString = "Could not open FE data file" + fName;
      Fui::okDialog(warnString.c_str());
      continue;
    }

    FmPart* part = new FmPart;
    part->setParentAssembly(parent);
    part->connect();

    if (part->setVisualizationFile(fName,false))
    {
      FFaMsg::list("===> Open visualization file: " + fName + "\n");

      if (useRelativePath)
        part->visDataFile = FmDB::getMechanismObject()->getRelativePath(fName);

      if (uc)
        part->visDataFileUnitConverter = *uc;

      part->useGenericProperties.setValue(true);
      part->myCalculateMass.setValue(FmPart::FROM_GEOMETRY);
      part->updateMassProperties();
    }
    else
    {
      FFaMsg::list("===> Open FE data file: " + fName + "\n");
      FFaMsg::pushStatus("Loading FE part");
      FFaMsg::setSubStep(partNr);
      FFaMsg::setSubTask(FFaFilePath::getFileName(fName));

      part->myCalculateMass.setValue(FmPart::FROM_FEM);

      bool ok = part->importPart(fName,uc,useRelativePath,autoGenTriads);
      FFaMsg::popStatus();
      FFaMsg::setSubTask("");

      if (!ok)
      {
	FFaMsg::list("===> Loading FE data failed.\n",true);
	part->erase();
	continue;
      }
    }

    part->setUserDescription(FFaFilePath::getBaseName(fName,true));
    part->onChanged();

    if (useSpecificPartRepository)
    {
      part->myRepository = FmDB::getMechanismObject()->getRelativePath(FFaFilePath::getPath(fName));
      FFaMsg::list("  -> Using part-specific repository: " +
		   part->myRepository.getValue() + "\n");
    }

    drawPart(part,fName);
  }

  FFaMsg::disableSubSteps();
  Fui::okToGetUserInput();
}
//----------------------------------------------------------------------------

void FapFileCmds::changeLink()
{
  // Get the selected part (only one)
  FmPart* part = FapCmdsBase::getTheSelectedPart();
  if (!part) return;

  FFl::initAllReaders();

  // Set up the file dialog
  FFuFileDialog* aDialog = openFile("Change Part");

  // Obtain filters from the part reader factory
  FFlReaderInfo defReader = FFlReaders::instance()->getDefaultReader();
  std::vector<FFlReaderInfo> regReaders;
  if (defReader.name.empty())
    FFlReaders::instance()->getRegisteredReaders(regReaders);
  else
  {
    FFlReaders::instance()->getRegisteredReaders(regReaders, false);
    aDialog->addFilter(defReader.name, defReader.extensions, true);
  }

  for (const FFlReaderInfo& reader : regReaders)
    aDialog->addFilter(reader.name, reader.extensions);

  aDialog->addAllFilesFilter(true);

  FmMechanism* mech = FmDB::getMechanismObject();
  std::string origPart = part->originalFEFile.getValue();
  bool isRelativeNameWanted = FFaFilePath::isRelativePath(origPart);

  if (!origPart.empty())
  {
    FFaFilePath::checkName(origPart);
    FFaFilePath::makeItAbsolute(origPart,mech->getAbsModelFilePath());
    if (!FpFileSys::verifyDirectory(FFaFilePath::getPath(origPart),false))
      FFaFilePath::setPath(origPart,mech->getAbsModelFilePath());
    aDialog->setDefaultName(origPart);
  }

  std::vector<std::string> definedCalcs;
  FFaUnitCalculatorProvider::instance()->getCalculatorNames(definedCalcs);
  if (!definedCalcs.empty())
  {
    aDialog->addUserOptionMenu("UC", "Unit conversion:");
    aDialog->addUserOption("UC", "none");
    const std::string& partUC = part->importConverter.getValue().getName();
    for (const std::string& calc : definedCalcs)
      aDialog->addUserOption("UC", calc, partUC == calc);
  }

  aDialog->addUserToggle("autoTriads", "Create triads at spider reference nodes", false);
  aDialog->addUserToggle("relToggle", "Use path relative to model-file location",
			 isRelativeNameWanted);
  aDialog->addUserToggle("LLR", "Use part-specific repository for this part",
			 !part->myRepository.getValue().empty());
  aDialog->remember("OpenPart");

  // Execute the file dialog
  std::vector<std::string> retFile = aDialog->execute();
  std::string converter = aDialog->getSelectedUserOption("UC");
  bool useRelativePath = aDialog->getUserToggleSet("relToggle");
  bool useSpecificPartRepository = aDialog->getUserToggleSet("LLR");
  bool autoGenTriads = aDialog->getUserToggleSet("autoTriads");
  delete aDialog;
  if (retFile.empty()) return;

  const std::string& fName = retFile.front();
  if (!FpFileSys::isReadable(fName))
  {
    std::string warnString = "Could not open " + fName + ", part was not updated";
    Fui::okDialog(warnString.c_str());
    return;
  }

  const FFaUnitCalculator* uc = FFaUnitCalculatorProvider::instance()->getCalculator(converter);

  Fui::noUserInputPlease();
  FFaMsg::list("===> Open FE data file: " + fName + "\n");

  if (part->ramUsageLevel.getValue() != FmPart::FULL_FE)
  {
    FFaMsg::list("  -> Changing RAM usage setting to full FE model.\n");
    part->ramUsageLevel = FmPart::FULL_FE;
  }

  part->readyForUpdate();
  part->setUserDescription(FFaFilePath::getBaseName(fName,true));

  // Actually import the part
  FFaMsg::pushStatus("Loading FE part");
  FFaMsg::setSubTask(FFaFilePath::getFileName(fName));
  bool ok = part->importPart(fName,uc,useRelativePath,autoGenTriads);
  FFaMsg::popStatus();
  FFaMsg::setSubTask("");
  if (!ok)
  {
    FFaMsg::list("===> Loading FE data failed.\n",true);
    Fui::okToGetUserInput();
    return;
  }

  part->updateTriadTopologyRefs();
  part->onChanged();

  if (useSpecificPartRepository)
  {
    part->myRepository.setValue(mech->getRelativePath(FFaFilePath::getPath(fName)));
    FFaMsg::list("  -> Using part-specific repository: " +
		 part->myRepository.getValue() + "\n");
  }
  else
    part->myRepository.setValue("");

  FmStrainRosette::syncStrainRosettes(part);

  drawPart(part,fName);

  Fui::okToGetUserInput();
}
//----------------------------------------------------------------------------

void FapFileCmds::getChangeLinkSensitivity(bool& isSensitive)
{
  FapCmdsBase::isModelEditable(isSensitive);
  if (!isSensitive) return;

  FmPart* part = FapCmdsBase::getTheSelectedPart();
  if (!part)
    isSensitive = false;
  else if (FapLicenseManager::isLimEdition())
    isSensitive = false;
  else if (!FapLicenseManager::isProEdition())
    isSensitive = part->originalFEFile.getValue().empty();
}
//----------------------------------------------------------------------------

/*!
 - All saved part data files are copied to the new repository.
 - All the active reduced data is copied, unless the new repository already
   has a valid reduction for that part. Unused reduced data is not copied.
 - The old repository is left untouched unless it is considered "internal".
   In the latter case, it is deleted when the user confirms.
 - Force a save of the model, but only when the old repository is deleted.
*/

void FapFileCmds::setModelLinkRep(bool switchToInternal)
{
  FmMechanism* mech = FmDB::getMechanismObject();
  std::string selectedRepDir, oldRepDir(mech->modelLinkRepository.getValue());

  if (!switchToInternal)
  {
    FFuFileDialog* aDialog = FFuFileDialog::create(mech->getAbsModelFilePath(),
						   "Select FE part repository directory",
						   FFuFileDialog::FFU_DIRECTORY_ONLY);
    aDialog->addUserToggle("relToggle", "Use path relative to model-file location",
                           FFaFilePath::isRelativePath(oldRepDir));
    aDialog->remember("PartRepository");

    std::vector<std::string> dirs = aDialog->execute();
    bool useRelativePath = aDialog->getUserToggleSet("relToggle");
    delete aDialog;
    if (dirs.empty()) return;

    if (dirs.front().empty())
      switchToInternal = true;
    else if (useRelativePath && mech->getAbsModelFilePath() != dirs.front())
      selectedRepDir = mech->getRelativePath(dirs.front());
    else
      selectedRepDir = dirs.front();
  }
  if (selectedRepDir == oldRepDir) return; // not changed

  std::string oldRepDirFullPath = mech->getAbsModelLRDBPath();

  std::vector<FmPartInt> savedParts;
  if (FpFileSys::verifyDirectory(oldRepDirFullPath,false))
  {
    std::vector<FmPart*> allParts;
    FmDB::getAllParts(allParts);
    for (FmPart* part : allParts)
      if (part->usesRepository() != FmPart::LINK_SPECIFIC)
      {
        int isSaved = 0;
        if (part->isSaved(false)) isSaved += 1;
        if (FapLinkReducer::isReduced(part)) isSaved += 2;
        if (isSaved > 0) savedParts.push_back(std::make_pair(part,isSaved));
      }
  }

  mech->modelLinkRepository.setValue(selectedRepDir);
  std::string newRepDirFullPath = mech->getAbsModelLRDBPath();

  if (!savedParts.empty() && newRepDirFullPath != oldRepDirFullPath)
  {
    std::string msg("The following parts will be copied into the new repository:");
    for (const FmPartInt& part : savedParts)
    {
      msg += "\n\t" + part.first->getLinkIDString();
      if (part.second == 3)
	msg += " - with reduced data";
      else if (part.second == 2)
	msg += " - only reduced data";
      else
	msg += " - without reduced data";
    }
    msg += "\nThe reduced data (if any) will be copied only for those parts"
	"\nwhich do not already have a valid reduction in the new repository."
	"\n\nContinue ?";
    if (!Fui::okCancelDialog(msg.c_str(),FFuDialog::WARNING))
    {
      mech->modelLinkRepository.setValue(oldRepDir);
      return;
    }
  }

  FpPM::touchModel();
  if (switchToInternal)
    ListUI <<"===> Switching to internal FE model repository\n";
  else
    ListUI <<"===> Moving FE model repository to "<< newRepDirFullPath <<"\n";

  if (savedParts.empty() || newRepDirFullPath == oldRepDirFullPath)
    return; // if no saved data yet or changing to the same location, we're done

  if (!switchToInternal)
    switchToInternal = true;
  else
    switchToInternal = FpFileSys::verifyDirectory(mech->getAbsModelRDBPath());
  if (!switchToInternal || !FpFileSys::verifyDirectory(newRepDirFullPath))
  {
    ListUI <<"===> Could not access directory "<< newRepDirFullPath <<"\n";
    return;
  }

  const char* bTexts[4] = {"Yes", "Yes to all", "No", NULL};

  bool okToAll = false;
  ListUI <<"     FE-mod  Red.data  Part Name\n";

  for (const FmPartInt& part : savedParts)
  {
    std::string status = "        ";
    if (part.second%2 == 1) // this part has been saved
    {
      bool okToCopy = true;
      const std::string& ftlName = part.first->baseFTLFile.getValue();
      if (!okToAll && FpFileSys::isFile(FFaFilePath::appendFileNameToPath(newRepDirFullPath,ftlName)))
	switch (FFaMsg::dialog("The file \"" + ftlName +
			       "\" already exists in the new repository."
			       "\nDo you want to overwrite this file ?",
			       FFaMsg::GENERIC, bTexts))
	  {
	  case 0: // Yes
	    break;
	  case 1: // Yes to all
	    okToAll = true;
	    break;
	  case 2: // No
	    status = "Skipped ";
	    okToCopy = false;
	    break;
	  }

      if (okToCopy)
      {
	if (FpFileSys::copyFile(ftlName,oldRepDirFullPath,newRepDirFullPath))
	  status = "Copied  ";
	else
	  status = "FAILED  ";
      }
    }

    ListUI <<"     "<< status;

    if (part.second < 2)
      status = "        "; // this part has not been reduced yet
    else if (FapLinkReducer::isReduced(part.first,true))
    {
      status = "Existing"; // the new repository already has a valid reduction
      // Syncronize the part RSD and extractor with the new repository position.
      // This must be done before trying to delete the old repository, or else
      // deleting frs-files will fail because they are open by the extractor.
      std::string taskName = part.first->myRSD.getValue().getCurrentTaskDirName();
      std::string taskDir  = FFaFilePath::appendFileNameToPath(newRepDirFullPath,taskName);
      FpModelRDBHandler::RDBSync(part.first,mech,false,taskDir);
    }
    else
    {
      // Copy reduction results from the old part DB to the new repository
      std::string taskName = part.first->myRSD.getValue().getCurrentTaskDirName();
      std::string taskDir  = FFaFilePath::appendFileNameToPath(newRepDirFullPath,taskName);

      if (FpFileSys::verifyDirectory(taskDir))
      {
	std::set<std::string> filesToCopy;
	part.first->myRSD.getValue().getAllFileNames(filesToCopy);

	int nFilesCopied = 0;
	int nFilesFailed = 0;
	for (const std::string& oName : filesToCopy)
	{
	  bool okToCopy = true;
	  std::string bName = FFaFilePath::getRelativeFilename(oldRepDirFullPath,oName);
	  std::string fName = FFaFilePath::appendFileNameToPath(newRepDirFullPath,bName);
	  if (!okToAll && FpFileSys::isFile(fName))
	    switch (FFaMsg::dialog("The file \"" + bName +
				   "\" already exists in the new repository."
				   "\nDo you want to overwrite this file ?",
				   FFaMsg::GENERIC, bTexts))
	      {
	      case 0: // Yes
		break;
	      case 1: // Yes to all
		okToAll = true;
		break;
	      case 2: // No
		okToCopy = false;
		break;
	      }

	  if (okToCopy)
          {
	    if (FpFileSys::copyFile(oName,fName))
	      nFilesCopied++;
	    else
	      nFilesFailed++;
          }
	}

	if (nFilesCopied > 0)
	{
	  if (nFilesFailed == 0)
	    status = "Copied  ";
	  else
	    status = "Failed " + std::to_string(nFilesFailed);
	}
	else if (nFilesFailed > 0)
	  status = "FAILED  ";
      }

      // Syncronize the part RSD and extractor with the new repository position.
      // This must be done before trying to delete the old repository, or else
      // deleting frs-files will fail because they are open by the extractor.
      FpModelRDBHandler::RDBSync(part.first,mech,false,taskDir);
    }

    ListUI << status <<"  "<< part.first->getLinkIDString() <<"\n";
  }

  ListUI <<"===> Done moving FE model repository.\n";

  // Need to reopen the results database - since the reducer frs-files have been moved
  FpModelRDBHandler::RDBRelease(true);

  // If we moved from the internal part DB, ask whether to remove it
  bool deleteOld = oldRepDir.empty() &&
    FFaMsg::dialog("The part data (FE models and active reduced data)\n"
                   "has been successfully copied to the new repository.\n\n"
                   "Do you want to delete the internal FE model repository to free up disk space?\n"
                   "The model will be automatically saved when this operation is completed.",
                   FFaMsg::YES_NO);
  if (deleteOld)
  {
    ListUI <<"  -> Deleting \""<< oldRepDirFullPath <<"\"\n";
    FpFileSys::removeDir(oldRepDirFullPath);
  }

  FpModelRDBHandler::RDBOpen(mech->getResultStatusData(),mech,true);

  // Force a save here such that the model file is in sync with the disk
  if (deleteOld) FapFileCmds::save();
}
//----------------------------------------------------------------------------

void FapFileCmds::getSetModelLinkRepSensitivity(bool& isSensitive)
{
  if (FapLicenseManager::isProEdition())
    FapCmdsBase::isModelEditable(isSensitive);
  else
    isSensitive = false;
}
//----------------------------------------------------------------------------

void FapFileCmds::getChangeModelSensitivity(bool& isSensitive)
{
  isSensitive = FapSolutionProcessManager::instance()->empty();
}
//----------------------------------------------------------------------------

void FapFileCmds::importPipeSurface()
{
#if defined(FT_HAS_OWL)
  FFuFileDialog* aDialog = openFiles("Read well geometry");
  aDialog->addAllFilesFilter(true);
  aDialog->remember("importOilWell");

  std::vector<std::string> retFiles = aDialog->execute();
  delete aDialog;
  if (retFiles.empty()) return;

  FpPM::vpmSetUndoPoint("Import pipe surface");
  FapOilWellCmds::createPipeSurface(retFiles.front());
#endif
}
//----------------------------------------------------------------------------

void FapFileCmds::importPipeString()
{
#if defined(FT_HAS_OWL)
  FFuFileDialog* aDialog = openFiles("Read pipe string data");
  aDialog->addAllFilesFilter(true);
  aDialog->remember("importOilWell");

  std::vector<std::string> retFiles = aDialog->execute();
  delete aDialog;
  if (retFiles.empty()) return;

  FpPM::vpmSetUndoPoint("Import pipe string");
  FapOilWellCmds::createPipeString(retFiles.front());
#endif
}
//----------------------------------------------------------------------------

void FapFileCmds::importDrillString()
{
#if defined(FT_HAS_OWL)
  FFuFileDialog* aDialog = openFiles("Read arc segment data");
  aDialog->addAllFilesFilter(true);
  aDialog->remember("importArcSegment");

  std::vector<std::string> retFiles = aDialog->execute();
  delete aDialog;
  if (retFiles.empty()) return;

  FpPM::vpmSetUndoPoint("Import drill string");
  FapOilWellCmds::createDrillString(retFiles.front());
#endif
}
//----------------------------------------------------------------------------

void FapFileCmds::importRiser()
{
#if defined(FT_HAS_WND) || defined(FT_HAS_RIS)
  FFuFileDialog* aDialog = openFiles("Read Beamstring data");
  aDialog->addAllFilesFilter(true);
  aDialog->remember("importRiser");

  std::vector<std::string> retFiles = aDialog->execute();
  delete aDialog;
  if (retFiles.empty()) return;

  FpPM::vpmSetUndoPoint("Import beamstring");
  FapOilWellCmds::createRiser(retFiles.front());
#endif
}
//----------------------------------------------------------------------------

void FapFileCmds::importEvents()
{
  if (!FapLicenseManager::checkLicense("FA-SEV"))
    return;

  FFuFileDialog* aDialog = openFiles("Read Simulation Event data");
  aDialog->addAllFilesFilter(true);
  aDialog->remember("importEvents");

  std::vector<std::string> retFiles = aDialog->execute();
  delete aDialog;
  if (retFiles.empty()) return;

  Fui::noUserInputPlease();
  FpPM::vpmSetUndoPoint("Import events");
  FapSimEventHandler::activate(NULL);
  FapFileCmds::createEvents(retFiles.front());
  Fui::okToGetUserInput();
}


#if defined(FT_HAS_SEV)
bool FapFileCmds::createEvents(const std::string& fileName)
{
  std::ifstream is(fileName.c_str());
  if (!is) return false;

  ListUI <<"===> Reading event definitions from "<< fileName <<"\n";
  if (!FWP::createEvents(is))
    return false;

  FWP::resolveEvents();
  return true;
}
#else
bool FapFileCmds::createEvents(const std::string&) { return false; }
#endif

//----------------------------------------------------------------------------

void FapFileCmds::importJacket()
{
#if defined(FT_HAS_WND) || defined(FT_HAS_RIS)
  FFl::initAllReaders();

  FFuFileDialog* aDialog = openFiles("Load Spaceframe");

  FFlReaderInfo defReader = FFlReaders::instance()->getDefaultReader();
  std::vector<FFlReaderInfo> regReaders;
  if (defReader.name.empty())
    FFlReaders::instance()->getRegisteredReaders(regReaders);
  else
  {
    FFlReaders::instance()->getRegisteredReaders(regReaders, false);
    aDialog->addFilter(defReader.name, defReader.extensions, true);
  }

  for (const FFlReaderInfo& reader : regReaders)
    aDialog->addFilter(reader.name, reader.extensions);

  std::vector<std::string> definedCalcs;
  FFaUnitCalculatorProvider::instance()->getCalculatorNames(definedCalcs);
  if (!definedCalcs.empty())
  {
    aDialog->addUserOptionMenu("UC", "Unit conversion:");
    aDialog->addUserOption("UC", "none");
    for (const std::string& calc : definedCalcs)
      aDialog->addUserOption("UC", calc);
  }
  aDialog->addUserField("Added mass coefficient, Ca", 1.0);
  aDialog->addUserField("Added mass coefficient, Cm", 2.0);
  aDialog->addUserField("Drag coefficient, Cd");
  aDialog->addIntField("User ID offset");
  aDialog->remember("OpenPart");

  double Morison[3];
  std::vector<std::string> retFiles = aDialog->execute();
  std::string converter = aDialog->getSelectedUserOption("UC");
  Morison[0] = aDialog->getUserFieldValue("Added mass coefficient, Ca");
  Morison[1] = aDialog->getUserFieldValue("Added mass coefficient, Cm");
  Morison[2] = aDialog->getUserFieldValue("Drag coefficient, Cd");
  int IDoffset = aDialog->getIntFieldValue("User ID offset");
  delete aDialog;
  if (retFiles.empty()) return;

  Fui::noUserInputPlease();
  FFaMsg::list("===> Open FE data file: " + retFiles.front() + "\n");

  FFlLinkHandler jacket;
  if (FFlReaders::instance()->read(retFiles.front(),&jacket) > 0)
  {
    FpPM::vpmSetUndoPoint("Import spaceframe");
    jacket.convertUnits(FFaUnitCalculatorProvider::instance()->getCalculator(converter));
    FapOilWellCmds::createJacket(&jacket,retFiles.front(),Morison,IDoffset);
  }

  Fui::okToGetUserInput();
#endif
}
//----------------------------------------------------------------------------

void FapFileCmds::importSoilPile()
{
#if defined(FT_HAS_WND) || defined(FT_HAS_RIS)
  FFuFileDialog* aDialog = openFiles("Read Soil Pile data");
  aDialog->addAllFilesFilter(true);
  aDialog->addUserField("Top position, X",0.0);
  aDialog->addUserField("Top position, Y",0.0);
  aDialog->addUserField("Top position, Z",0.0);
  aDialog->addUserField("Height of top point above mudline",0.0);
  aDialog->addUserToggle("interConn","Use interconnected p-y springs",false);
  aDialog->addUserToggle("scale","Use soil data per length unit",true);
  aDialog->addUserToggle("cyclic","Cyclic springs with secant stiffness",false);
  aDialog->remember("importPile");

  FaVec3 top;
  std::vector<std::string> retFiles = aDialog->execute();
  top.x(aDialog->getUserFieldValue("Top position, X"));
  top.y(aDialog->getUserFieldValue("Top position, Y"));
  top.z(aDialog->getUserFieldValue("Top position, Z"));
  double H = aDialog->getUserFieldValue("Height of top point above mudline");
  bool interconnectXY = aDialog->getUserToggleSet("interConn");
  bool scale = aDialog->getUserToggleSet("scale");
  int cyclic = aDialog->getUserToggleSet("cyclic") ? 3 : 0;
  delete aDialog;
  if (retFiles.empty()) return;

  FpPM::vpmSetUndoPoint("Import soil pipe");
  FapOilWellCmds::createPile(retFiles.front(),top,H,interconnectXY,scale,cyclic);
#endif
}


void FapFileCmds::importAssembly()
{
  FFuFileDialog* aDialog = openFile("Import Subassembly from file");
  aDialog->addFilter("Fedem model file", "fmm");
  aDialog->addAllFilesFilter(true);
  aDialog->remember("ModelFileManip");

  std::vector<std::string> retFiles = aDialog->execute();
  delete aDialog;
  if (retFiles.empty()) return;

  FpPM::vpmSetUndoPoint("Import subassembly");
  FpPM::vpmAssemblyOpen(retFiles.front());
}
