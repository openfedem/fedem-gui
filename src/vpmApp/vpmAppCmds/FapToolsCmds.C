// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapToolsCmds.H"
#include "vpmApp/FapLicenseManager.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "vpmUI/Fui.H"
#include "vpmUI/FuiModes.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#ifdef USE_INVENTOR
#include "vpmDisplay/FdCtrlDB.H"
#endif
#include "vpmPM/FpPM.H"
#include "vpmDB/FmFileSys.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmTurbine.H"

#if defined(win32) || defined(win64)
#include "Admin/FedemAdmin.H"
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#define MAX_PATH PATH_MAX
#endif

#define LAMBDA(func) FFaDynCB0S([](){ func; })
#define LAMBDA_CTRL(func) FFaDynCB0S([](){ if (FapLicenseManager::hasCtrlLicense()) func; })

std::vector<std::string> FapToolsCmds::ourAddons;


//------------------------------------------------------------------------------

void FapToolsCmds::init()
{
  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_mech_show");
  cmdItem->setSmallIcon(openMechModeller_xpm);
  cmdItem->setText("Show Modeler");
  cmdItem->setToolTip("Show Modeler");
  cmdItem->setActivatedCB(LAMBDA(Fui::modellerUI();FuiModes::cancel()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl_show");
  cmdItem->setSmallIcon(ctrlSystem_xpm);
  cmdItem->setText("Show Control Editor");
  cmdItem->setToolTip("Show Control Editor");
#ifdef USE_INVENTOR
  cmdItem->setActivatedCB(LAMBDA_CTRL(FdCtrlDB::openCtrl()));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl_gridSnap");
  cmdItem->setSmallIcon(ctrlGrid_xpm);
  cmdItem->setText("Control Editor Grid/Snap...");
  cmdItem->setToolTip("Control Editor Grid/Snap");
  cmdItem->setActivatedCB(LAMBDA_CTRL(Fui::ctrlGridSnapUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_preferences");
  cmdItem->setSmallIcon(additionalSolverOptions_xpm);
  cmdItem->setText("Additional Solver Options...");
  cmdItem->setToolTip("Additional Solver Options");
  cmdItem->setActivatedCB(LAMBDA(Fui::preferencesUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_viewFilter");
  cmdItem->setSmallIcon(viewFilter_xpm);
  cmdItem->setText("General Appearance...");
  cmdItem->setToolTip("General Appearance");
  cmdItem->setActivatedCB(LAMBDA(Fui::viewSettingsUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_objectBrowser");
  cmdItem->setSmallIcon(objectBrowser_xpm);
  cmdItem->setText("Object Browser...");
  cmdItem->setToolTip("Object Browser");
  cmdItem->setActivatedCB(LAMBDA(Fui::objectBrowserUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_outputList");
  cmdItem->setSmallIcon(infoList_xpm);
  cmdItem->setText("Show Output List");
  cmdItem->setToolTip("Show Output List");
  cmdItem->setActivatedCB(LAMBDA(Fui::outputListUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_miniFileBrowser");
  cmdItem->setSmallIcon(browseRDB_xpm);
  cmdItem->setText("Result File Browser...");
  cmdItem->setToolTip("Result File Browser");
  cmdItem->setActivatedCB(LAMBDA(Fui::resultFileBrowserUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_linkRamSettings");
  cmdItem->setSmallIcon(linkRamSettings_xpm);
  cmdItem->setText("FE-Data Settings...");
  cmdItem->setToolTip("FE-Data settings");
  cmdItem->setActivatedCB(LAMBDA(Fui::linkRamSettingsUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isProEdition,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_modelPreferences");
  cmdItem->setText("Model Preferences...");
  cmdItem->setToolTip("Model Preferences");
  cmdItem->setActivatedCB(LAMBDA(Fui::modelPreferencesUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_seaEnvironment");
  cmdItem->setSmallIcon(sea_xpm);
  cmdItem->setText("Sea Environment...");
  cmdItem->setToolTip("Sea Environment");
  cmdItem->setActivatedCB(LAMBDA(Fui::seaEnvironmentUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_airEnvironment");
  cmdItem->setSmallIcon(windAirEnv_xpm);
  cmdItem->setText("Aerodynamic Setup...");
  cmdItem->setToolTip("Aerodynamic Setup");
  cmdItem->setActivatedCB(LAMBDA(Fui::airEnvironmentUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_airfoilDefinition");
  cmdItem->setSmallIcon(windAirFoil_xpm);
  cmdItem->setText("Browse Airfoils...");
  cmdItem->setToolTip("Browse Airfoils for Turbine Blades");
  cmdItem->setActivatedCB(LAMBDA(Fui::airfoilDefinitionUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_bladeDefinition");
  cmdItem->setSmallIcon(windBladeProp_xpm);
  cmdItem->setText("Blade Definition...");
  cmdItem->setToolTip("Blade definition for Turbine Assembly");
  cmdItem->setActivatedCB(LAMBDA(Fui::bladeDefinitionUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_createTurbineAssembly");
  cmdItem->setSmallIcon(windTurbine_xpm);
  cmdItem->setText("Turbine Definition...");
  cmdItem->setToolTip("Turbine Definition");
  cmdItem->setActivatedCB(LAMBDA(Fui::turbineAssemblyUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_createTurbineTower");
  cmdItem->setSmallIcon(windTower_xpm);
  cmdItem->setText("Tower Definition...");
  cmdItem->setToolTip("Turbine Tower Definition");
  cmdItem->setActivatedCB(LAMBDA(Fui::turbineTowerUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S([](bool& isSensitive){
        FmTurbine* turbine = FmDB::getTurbineObject();
        isSensitive = (turbine && turbine->getTower());
      },bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_createBeamstringPair");
  cmdItem->setSmallIcon(beamstringPairDef_xpm);
  cmdItem->setText("Beamstring Pair Definition...");
  cmdItem->setToolTip("Beamstring Pair Definition");
  cmdItem->setActivatedCB(LAMBDA(Fui::beamstringPairUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_setFileAssociations");
  cmdItem->setText("Set File Associations...");
  cmdItem->setToolTip("Set File Associations");
  cmdItem->setActivatedCB(FFaDynCB0S(FapToolsCmds::setFileAssociations));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_plugins");
  cmdItem->setText("Plug-Ins...");
  cmdItem->setToolTip("Plug-Ins");
  cmdItem->setActivatedCB(LAMBDA(Fui::pluginsUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_eventDefinition");
  cmdItem->setSmallIcon(eventDef_xpm);
  cmdItem->setText("Event Definitions...");
  cmdItem->setToolTip("Events");
  cmdItem->setActivatedCB(LAMBDA(Fui::eventDefinitionUI()));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_distance");
  cmdItem->setSmallIcon(measureDistance_xpm);
  cmdItem->setText("Measure distance...");
  cmdItem->setToolTip("Measure distance between two points");
  cmdItem->setActivatedCB(LAMBDA(FuiModes::setMode(FuiModes::MEASURE_DISTANCE_MODE)));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_tools_angle");
  cmdItem->setSmallIcon(measureAngle_xpm);
  cmdItem->setText("Measure angle...");
  cmdItem->setToolTip("Measure angle between two points");
  cmdItem->setActivatedCB(LAMBDA(FuiModes::setMode(FuiModes::MEASURE_ANGLE_MODE)));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  for (int i = 0; i < 40; i++)
  {
    cmdItem = new FFuaCmdItem("cmdId_tools_addon" + std::to_string(i));
    cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));
    switch (i) {
    case  0: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch( 0))); break;
    case  1: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch( 1))); break;
    case  2: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch( 2))); break;
    case  3: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch( 3))); break;
    case  4: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch( 4))); break;
    case  5: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch( 5))); break;
    case  6: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch( 6))); break;
    case  7: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch( 7))); break;
    case  8: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch( 8))); break;
    case  9: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch( 9))); break;
    case 10: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(10))); break;
    case 11: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(11))); break;
    case 12: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(12))); break;
    case 13: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(13))); break;
    case 14: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(14))); break;
    case 15: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(15))); break;
    case 16: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(16))); break;
    case 17: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(17))); break;
    case 18: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(18))); break;
    case 19: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(19))); break;
    case 20: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(20))); break;
    case 21: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(21))); break;
    case 22: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(22))); break;
    case 23: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(23))); break;
    case 24: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(24))); break;
    case 25: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(25))); break;
    case 26: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(26))); break;
    case 27: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(27))); break;
    case 28: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(28))); break;
    case 29: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(29))); break;
    case 30: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(30))); break;
    case 31: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(31))); break;
    case 32: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(32))); break;
    case 33: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(33))); break;
    case 34: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(34))); break;
    case 35: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(35))); break;
    case 36: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(36))); break;
    case 37: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(37))); break;
    case 38: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(38))); break;
    case 39: cmdItem->setActivatedCB(LAMBDA(FapToolsCmds::addonLaunch(39))); break;
    }
    // The text and tooltip with Addon names are set later, when the menu is activated
  }
}


bool FapToolsCmds::haveAddons()
{
#if defined(win32) || defined(win64)
  const char* filter = "*.exe";
#else
  const char* filter = "*";
#endif
  return FmFileSys::getFiles(ourAddons,FpPM::getFullFedemPath("addons"),filter);
}


bool FapToolsCmds::getAddonExe(size_t index, std::string& addon, bool fullPath)
{
  if (index >= ourAddons.size())
    return false;

  if (fullPath)
  {
    addon = FpPM::getFullFedemPath("addons");
#if defined(win32) || defined(win64)
    addon += "\\";
#else
    addon += "/";
#endif
    addon += ourAddons[index];
  }
  else
    addon = ourAddons[index].substr(0,ourAddons[index].find_first_of('.'));

  return true;
}


void FapToolsCmds::addonLaunch(int index)
{
#if defined(win32) || defined(win64)
  std::string exePath;
  if (FapToolsCmds::getAddonExe(index,exePath,true))
  {
    std::wstring cmd(exePath.begin(),exePath.end());
    cmd.append(L" " + std::to_wstring(GetCurrentProcessId()));
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
    CreateProcess(NULL, const_cast<wchar_t*>(cmd.c_str()),
                  NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
  }
#else
  std::cerr <<"  ** FapToolsCmds::addonLaunch("<< index
            <<"): No Linux support"<< std::endl;
#endif
}


#if defined(win32) || defined(win64)
/*!
  This function creates a windows registry key with a default value.
*/
static bool FmRegCreateKey(const std::string& key, const std::string& val, bool* pbOk)
{
  if (!*pbOk)
    return false; // don't proceed

  // Fix slashes
  std::wstring strKey(key.begin(),key.end());
  for (wchar_t& c : strKey)
    if (c == '/') c = '\\';

  // Create key
  HKEY regKey = NULL;
  LONG err = ::RegCreateKeyEx(HKEY_CLASSES_ROOT, // Root key
                              strKey.c_str(), // Sub key
                              0, NULL, // Dummy
			      REG_OPTION_NON_VOLATILE, // Keep in registry
			      KEY_ALL_ACCESS, // Give me all access rights
			      NULL,    // regKey does not need to be inherited
			      &regKey, // Returned key
			      NULL);   // Irrelevant if it was created or opened
  if (err != ERROR_SUCCESS)
    return *pbOk = false;

  // Set default value
  err = ::RegSetValueEx(regKey,
                        L"",
                        NULL,
                        REG_EXPAND_SZ,
                        (BYTE*)val.c_str(),
                        val.size() + 1);
  if (err != ERROR_SUCCESS)
    *pbOk = false;

  ::RegCloseKey(regKey);

  return *pbOk;
}


static void FmRegTypicalWordPad(const char* fileExt, const char* fileDesc, const char* iconIndex, bool* pbOk)
{
  std::string strFileExt(fileExt);
  FFaUpperCaseString strFileExtU(fileExt);

  // App path
  std::string appPath = "\"" + FpPM::getFullFedemPath("Fedem.exe") + "\"";
  std::string appWordPad = "\"WORDPAD.EXE\" \"%1\"";

  // Register
  FmRegCreateKey("." + strFileExt, strFileExtU + "-file", pbOk);
  FmRegCreateKey("." + strFileExt + "/OpenWithList", "", pbOk);
  FmRegCreateKey("." + strFileExt + "/OpenWithList/wordpad.exe", "", pbOk);
  FmRegCreateKey(strFileExtU + "-file", fileDesc, pbOk);
  FmRegCreateKey(strFileExtU + "-file/DefaultIcon", appPath + "," + iconIndex, pbOk);
  FmRegCreateKey(strFileExtU + "-file/shell", "", pbOk);
  FmRegCreateKey(strFileExtU + "-file/shell/open", "Open with &WordPad", pbOk);
  FmRegCreateKey(strFileExtU + "-file/shell/open/command", appWordPad, pbOk);
}
#endif


void FapToolsCmds::setFileAssociations()
{
  if (FFaMsg::dialog("Would you like to register file associations?\n\n"
		     "Note: You may have to run Fedem in administrator mode.",
		     FFaMsg::YES_NO_CANCEL) != 1) return;

  if (FapToolsCmds::setFileAssociationsEx())
    FFaMsg::dialog("Successfully registered file associations!\n\n"
		   "Note: You may have to restart Windows for the changes to take effect.",
		   FFaMsg::OK);
  else
    FFaMsg::dialog("Unable to register file associations!", FFaMsg::WARNING);
}


bool FapToolsCmds::setFileAssociationsEx()
{
  bool bOk = true;
#if defined(win32) || defined(win64)

  std::string appPath = "\"" + FpPM::getFullFedemPath("Fedem.exe") + "\"";

  // FMM
  FmRegCreateKey(".fmm", "FMM-file", &bOk);
  FmRegCreateKey(".fmm/OpenWithList", "", &bOk);
  FmRegCreateKey(".fmm/OpenWithList/wordpad.exe", "", &bOk);
  FmRegCreateKey("FMM-file", "Fedem Model File", &bOk);
  FmRegCreateKey("FMM-file/DefaultIcon", appPath + ",8", &bOk);
  FmRegCreateKey("FMM-file/shell", "", &bOk);
  FmRegCreateKey("FMM-file/shell/open", "Open with &Fedem", &bOk);
  FmRegCreateKey("FMM-file/shell/open/command", appPath + " -f \"%1\"", &bOk);

  // FTC
  FmRegCreateKey(".ftc", "FTC-file", &bOk);
  FmRegCreateKey("FTC-file", "Fedem Cad File", &bOk);
  FmRegCreateKey("FTC-file/DefaultIcon", appPath + ",3", &bOk);

  // FMX
  FmRegCreateKey(".fmx", "FMX-file", &bOk);
  FmRegCreateKey("FMX-file", "Fedem Matrix File", &bOk);
  FmRegCreateKey("FMX-file/DefaultIcon", appPath + ",7", &bOk);

  // FSM
  FmRegCreateKey(".fsm", "FSM-file", &bOk);
  FmRegCreateKey("FSM-file", "Fedem SAM Data", &bOk);
  FmRegCreateKey("FSM-file/DefaultIcon", appPath + ",15", &bOk);

  // ERR, BDF, FLM, FTL, NAS, FAO, FCO, FOP, FSI, FRS, RES, FCD
  FmRegTypicalWordPad("err", "Fedem Error File", "1", &bOk);
  FmRegTypicalWordPad("bdf", "Nastran Bulk Data", "2", &bOk);
  FmRegTypicalWordPad("flm", "Fedem Link File", "4", &bOk);
  FmRegTypicalWordPad("ftl", "Fedem Link File", "5", &bOk);
  FmRegTypicalWordPad("nas", "Nastran Bulk Data", "6", &bOk);
  FmRegTypicalWordPad("fao", "Fedem Additional Options", "9", &bOk);
  FmRegTypicalWordPad("fco", "Fedem Computational Options", "10", &bOk);
  FmRegTypicalWordPad("fop", "Fedem Output Options", "11", &bOk);
  FmRegTypicalWordPad("fsi", "Fedem Solver Input", "12", &bOk);
  FmRegTypicalWordPad("frs", "Fedem Result File", "13", &bOk);
  FmRegTypicalWordPad("res", "Fedem Result File", "14", &bOk);
  FmRegTypicalWordPad("fcd", "Fedem Unit Conversion", "16", &bOk);

  // Internal
  std::string key("FMM-file/internal/");
  key += FedemAdmin::getVersion();
  FmRegCreateKey(key.c_str(),"registered",&bOk);
#else
  bOk = false;
  FFaMsg::list("ERROR: File associations is available on Windows only.",true);
#endif
  return bOk;
}


bool FapToolsCmds::checkFileAssociations()
{
#if defined(win32) || defined(win64)

  // Get version
  std::string key("FMM-file\\internal\\");
  key += FedemAdmin::getVersion();
  std::wstring wkey(key.begin(),key.end());
  // Open registry key
  HKEY hk;
  LONG err = ::RegOpenKeyEx(HKEY_CLASSES_ROOT,wkey.c_str(),
                            0,KEY_QUERY_VALUE,&hk);
  if (err != ERROR_SUCCESS)
    return false;

  // Get registry value
  DWORD cbData = 0;
  err = ::RegQueryValueEx(hk,L"",NULL,NULL,NULL,&cbData);
  ::RegCloseKey(hk);
  return (err == ERROR_SUCCESS && cbData == 11);

#else
  return true; // no Linux!
#endif
}
