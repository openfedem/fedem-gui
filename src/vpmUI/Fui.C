// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiAdvAnalysisOptions.H"
#include "vpmUI/vpmUITopLevels/FuiCtrlModeller.H"
#include "vpmUI/vpmUITopLevels/FuiModeller.H"
#include "vpmUI/vpmUITopLevels/FuiRDBSelector.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmUI/vpmUITopLevels/FuiRDBMEFatigue.H"
#endif
#include "vpmUI/vpmUITopLevels/FuiStressOptions.H"
#include "vpmUI/vpmUITopLevels/FuiEigenOptions.H"
#include "vpmUI/vpmUITopLevels/FuiGageOptions.H"
#include "vpmUI/vpmUITopLevels/FuiFppOptions.H"
#ifdef FT_HAS_NCODE
#include "vpmUI/vpmUITopLevels/FuiDutyCycleOptions.H"
#endif
#include "vpmUI/vpmUITopLevels/FuiAnimationControl.H"
#include "vpmUI/vpmUITopLevels/FuiPreferences.H"
#include "vpmUI/vpmUITopLevels/FuiViewSettings.H"
#include "vpmUI/vpmUITopLevels/FuiOutputList.H"
#include "vpmUI/vpmUITopLevels/FuiAppearance.H"
#include "vpmUI/vpmUITopLevels/FuiProperties.H"
#include "vpmUI/vpmUITopLevels/FuiMiniFileBrowser.H"
#include "vpmUI/vpmUITopLevels/FuiLinkRamSettings.H"
#include "vpmUI/vpmUITopLevels/FuiModelPreferences.H"
#include "vpmUI/vpmUITopLevels/FuiEventDefinition.H"
#include "vpmUI/vpmUITopLevels/FuiSeaEnvironment.H"
#ifdef FT_HAS_WND
#include "vpmUI/vpmUITopLevels/FuiAirEnvironment.H"
#include "vpmUI/vpmUITopLevels/FuiTurbWind.H"
#include "vpmUI/vpmUITopLevels/FuiAirfoilDefinition.H"
#include "vpmUI/vpmUITopLevels/FuiBladeDefinition.H"
#include "vpmUI/vpmUITopLevels/FuiCreateTurbineAssembly.H"
#include "vpmUI/vpmUITopLevels/FuiCreateTurbineTower.H"
#else
#define FuiTurbWind FFuTopLevelShell
#endif
#include "vpmUI/vpmUITopLevels/FuiCreateBeamstringPair.H"
#include "vpmUI/vpmUITopLevels/FuiObjectBrowser.H"
#include "vpmUI/vpmUITopLevels/FuiCSSelector.H"
#include "vpmUI/vpmUITopLevels/FuiPlugins.H"
#include "vpmUI/vpmUITopLevels/FuiModelExport.H"

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuUserDialog.H"
#include "FFuLib/FFuAuxClasses/FFuaApplication.H"

#include "vpmUI/FuiModes.H"
#include "vpmUI/FuiCtrlModes.H"

#include "vpmUI/vpmUITopLevels/FuiMainWindow.H"
#include "vpmUI/vpmUIComponents/FuiWorkSpace.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmUI/vpmUITopLevels/FuiGraphView.H"
#endif
#include "vpmApp/vpmAppUAMap/FapUAMainWindow.H"
#include "vpmApp/vpmAppUAMap/FapUAModeller.H"
#include "vpmApp/vpmAppUAMap/FapUAProperties.H"

#include "FFaLib/FFaDefinitions/FFaAppInfo.H"
#include "vpmPM/FpPM.H"
#include "vpmUI/Fui.H"
#include "vpmUI/FuiMsg.H"


////////////////////////////////
// Initializing static variables
////////////////////////////////

static int uiTitleBarHeight = 0;
static int uiScreenHeight = 0;
static int uiScreenWidth = 0;
static int uiBorderWidth = 0;

static FuiMainWindow* mainWindow = NULL;

bool Fui::haveAeroDyn = false;


void Fui::init(int& argc, char** argv)
{
  bool consoleOnly = FFaAppInfo::isConsole();

  // Init the UAExistenceHandler
  FapUAExistenceHandler::init();

  // GUI lib initialisation
  FFuaApplication::init(argc,argv,!consoleOnly);

#ifdef FT_HAS_WND
  // Check if Windpower features are available through the existence of AeroDyn
#if defined(win32) || defined(win64)
  Fui::haveAeroDyn = !FpPM::getFullFedemPath("AeroDyn.dll",'e').empty();
#else
  Fui::haveAeroDyn = !FpPM::getFullFedemPath("libAeroDyn.so",'e').empty();
#endif
#endif

  // Init user feedback method
  FFaMsg::setMessager(new FuiMsg);
  if (consoleOnly) return;

  // Getting screen geometry
  uiScreenHeight = FFuaApplication::getScreenHeight();
  uiScreenWidth  = FFuaApplication::getScreenWidth();

  // Create the main window
  Fui::Geo geo = Fui::getGeo(MAINWINDOW_GEO);
  mainWindow = FuiMainWindow::create(geo.xPos, geo.yPos, geo.width, geo.height);
}


void Fui::start()
{
  if (!mainWindow) return;

  // Get decoration properties from project UI
  // FIXME - find these values somehow.
  uiTitleBarHeight = 23;
  uiBorderWidth = 8;

  // Create the other UIs
  Fui::outputListUI(false,true);
  Fui::modellerUI(false,true);
}


void Fui::mainUI()
{
  if (!mainWindow) return;

  // Set basic mode
  FuiModes::setMode(FuiModes::EXAM_MODE);

  // Pop up the different ui's

  mainWindow->popUp();
  Fui::updateUICommands();
  Fui::modellerUI();

  // Make all the ui's actually redraw themselves (handle redraw event etc..)

  FFuaApplication::handlePendingEvents();

  FFaMsg::setStatus("Ready");
}


bool Fui::hasGUI()
{
  return mainWindow != NULL;
}


void Fui::updateUICommands()
{
  if (mainWindow)
    static_cast<FapUAMainWindow*>(mainWindow->getUA())->updateUICommands();
}


void Fui::list(const char* text)
{
  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiOutputList::getClassTypeID());

  if (uic)
    dynamic_cast<FuiOutputList*>(uic)->addText(text);
  else
    std::cout << text;
}


void Fui::tip(const char* value)
{
  if (mainWindow)
    mainWindow->setStatusBarMessage(value);
}


void Fui::setTitle(const char* title)
{
  char newTitle[BUFSIZ];
  sprintf(newTitle, "FEDEM %s - %s", FpPM::getVpmVersion(), title);

  if (mainWindow)
    mainWindow->setTitle(newTitle);

  sprintf(newTitle, "Output List: %s", title);

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiOutputList::getClassTypeID());
  if (uic)
    uic->setTitle(newTitle);
}


bool Fui::okCancelDialog(const char* msg, int type)
{
  if (!mainWindow) return true;

  const char* texts[2] = {"OK","Cancel"};
  FFuUserDialog* dialog = FFuUserDialog::create(msg,type,texts,2);
  int answer = dialog->execute();
  delete dialog;

  return answer == 0 ? true : false;
}


bool Fui::yesNoDialog(const char* msg, int type)
{
  if (!mainWindow) return true;

  const char* texts[2] = {"Yes","No"};
  FFuUserDialog* dialog = FFuUserDialog::create(msg,type,texts,2);
  int answer = dialog->execute();
  delete dialog;

  return answer == 0 ? true : false;
}


void Fui::okDialog(const char* msg, int type)
{
  if (!mainWindow) return;

  const char* texts[1] = {"OK"};
  FFuUserDialog* dialog = FFuUserDialog::create(msg,type,texts,1);
  dialog->execute();
  delete dialog;
}


void Fui::dismissDialog(const char* msg, int type)
{
  if (!mainWindow) return;

  const char* texts[1] = {"Dismiss"};
  FFuUserDialog::create(msg,type,texts,1,false);
}


int Fui::genericDialog(const char* msg, const char** texts, int nButtons, int type)
{
  if (!mainWindow) return -1;

  FFuUserDialog* dialog = FFuUserDialog::create(msg,type,texts,nButtons);
  int answer = dialog->execute();
  delete dialog;

  return answer;
}


void Fui::noUserInputPlease()
{
  if (!mainWindow) return;

  FFuaApplication::blockUserEvents(true);
}


void Fui::okToGetUserInput()
{
  if (!mainWindow) return;

  FFuaApplication::blockUserEvents(false);
}


void Fui::lockModel(bool yesOrNo)
{
  // Make properties non-editable
  FuiProperties* props = Fui::getProperties();
  if (props) props->setSensitivity(!yesOrNo);

  // Lock/unlock control modeller
  if (FuiCtrlModes::isCtrlModellerOpen())
    FuiCtrlModes::cancel();

  // Lock other top-level dialogs.
  FFuTopLevelShell* tls;

  tls = FFuTopLevelShell::getInstanceByType(FuiAdvAnalysisOptions::getClassTypeID());
  if (tls) tls->setSensitivity(!yesOrNo);

  tls = FFuTopLevelShell::getInstanceByType(FuiPreferences::getClassTypeID());
  if (tls) tls->setSensitivity(!yesOrNo);

  tls = FFuTopLevelShell::getInstanceByType(FuiModelPreferences::getClassTypeID());
  if (tls) tls->setSensitivity(!yesOrNo);

  tls = FFuTopLevelShell::getInstanceByType(FuiSeaEnvironment::getClassTypeID());
  if (tls) tls->setSensitivity(!yesOrNo);

  tls = FFuTopLevelShell::getInstanceByType(FuiEventDefinition::getClassTypeID());
  if (tls) tls->setSensitivity(!yesOrNo);

#ifdef FT_HAS_WND
  tls = FFuTopLevelShell::getInstanceByType(FuiAirEnvironment::getClassTypeID());
  if (tls) tls->setSensitivity(!yesOrNo);

  tls = FFuTopLevelShell::getInstanceByType(FuiTurbWind::getClassTypeID());
  if (tls) tls->setSensitivity(!yesOrNo);

  tls = FFuTopLevelShell::getInstanceByType(FuiAirfoilDefinition::getClassTypeID());
  if (tls) tls->setSensitivity(!yesOrNo);

  tls = FFuTopLevelShell::getInstanceByType(FuiBladeDefinition::getClassTypeID());
  if (tls) tls->setSensitivity(!yesOrNo);

  tls = FFuTopLevelShell::getInstanceByType(FuiCreateTurbineAssembly::getClassTypeID());
  if (tls) tls->setSensitivity(!yesOrNo);

  tls = FFuTopLevelShell::getInstanceByType(FuiCreateTurbineTower::getClassTypeID());
  if (tls) tls->setSensitivity(!yesOrNo);
#endif

  tls = FFuTopLevelShell::getInstanceByType(FuiCreateBeamstringPair::getClassTypeID());
  if (tls) tls->setSensitivity(!yesOrNo);

  tls = FFuTopLevelShell::getInstanceByType(FuiObjectBrowser::getClassTypeID());
  if (tls) tls->setSensitivity(!yesOrNo);

  tls = FFuTopLevelShell::getInstanceByType(FuiModelExport::getClassTypeID());
  if (tls) tls->setSensitivity(!yesOrNo);
}


FuiMainWindow* Fui::getMainWindow()
{
  return mainWindow;
}


FuiProperties* Fui::getProperties()
{
  return mainWindow ? mainWindow->getProperties() : NULL;
}


FFuListView* Fui::getTopologyList()
{
  FuiProperties* props = Fui::getProperties();
  if (!props) return NULL;

  FuiTopologyView* topology = props->getTopologyView();
  return topology ? topology->getListView() : NULL;
}


FFuComponentBase* Fui::getViewer()
{
  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiModeller::getClassTypeID());

  if (uic)
    return dynamic_cast<FuiModeller*>(uic)->activeViewer();
  else
    return NULL;
}


FFuComponentBase* Fui::getCtrlViewer()
{
  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiCtrlModeller::getClassTypeID());

  if (uic)
    return dynamic_cast<FuiCtrlModeller*>(uic)->activeViewer();
  else
    return NULL;
}


void Fui::updateMode()
{
  switch (FuiModes::getMode())
    {
    case FuiModes::APPEARANCE_MODE:
      Fui::appearanceUI(false,true);
    default:
      break;
    }
}


void Fui::updateState(int newState)
{
  FapUAExistenceHandler::doUpdateState(FuiModes::getState(),newState,
                                       FuiModes::getMode());
}


void Fui::cancel()
{
  FapUAProperties* uap = NULL;
  switch (FuiModes::getMode())
    {
    case FuiModes::APPEARANCE_MODE:
      Fui::appearanceUI(false);
      break;
    case FuiModes::ADDMASTERINLINJOINT_MODE:
    case FuiModes::PICKLOADATTACKPOINT_MODE:
    case FuiModes::PICKLOADFROMPOINT_MODE:
    case FuiModes::PICKLOADTOPOINT_MODE:
      if ((uap = FapUAProperties::getPropertiesHandler()))
      {
        uap->setIgnorePickNotify(false);
        uap->updateUIValues();
      }
      break;
    default:
      break;
    }
}


Fui::Geo Fui::getGeo(UIgeom fuiType)
{
  const int mainWidth = 1020;
  const int mainHeight = 820;

  const int modellerWidth  = mainWidth *783/1000; // 78.3% of main window width
  const int modellerHeight = mainHeight*648/1000; // 64.8% of main window height

  Geo geo;

  switch (fuiType)
    {
    case MAINWINDOW_GEO:
      geo.xPos   = 280;
      geo.yPos   = 180;
      geo.width  = mainWidth;
      geo.height = mainHeight;
      break;
    case GRAPHVIEW_GEO:
      geo.width  = 300;
      geo.height = 300;
      break;
    case MODELLER_GEO:
      geo.width  = modellerWidth;
      geo.height = modellerHeight;
      break;
    case CTRLMODELLER_GEO:
      geo.width  = modellerWidth;
      geo.height = modellerHeight - 25;
      break;
    case OUTPUTLIST_GEO:
      geo.width  = 600;
      geo.height = 150;
      geo.xPos   = uiScreenWidth - geo.width - 3;
      geo.yPos   = uiScreenHeight - geo.height - 40; // Room for Windows task bar
      break;
    case APPEARANCE_GEO:
      geo.xPos   = 4*modellerWidth/5;
      geo.yPos   = modellerHeight/5;
      geo.width  = 385;
      geo.height = 430;
      break;
    case ADVANALYSISOPTIONS_GEO:
      geo.xPos   = modellerWidth/2 + 30;
      geo.yPos   = modellerHeight/5;
      geo.width  = 420;
#if defined(win32) || defined(win64)
      geo.height = 570;
#else
      geo.height = 660;
#endif
      break;
    case STRESSOPTIONS_GEO:
      geo.xPos   = 2*modellerWidth/9;
      geo.yPos   = modellerHeight - (275-uiBorderWidth-uiTitleBarHeight);
      geo.width  = 375;
      geo.height = 550;
      break;
    case EIGENMODEOPTIONS_GEO:
      geo.width  = 350;
      geo.height = 460;
      geo.xPos   = modellerWidth - geo.width;
      geo.yPos   = modellerHeight - 275 - 210 - uiBorderWidth;
      break;
    case GAGEOPTIONS_GEO:
      geo.xPos   = 100;
      geo.yPos   = 350;
      geo.width  = 325;
      geo.height = 400;
      break;
    case FPPOPTIONS_GEO:
      geo.xPos   = 100;
      geo.yPos   = 350;
      geo.width  = 370;
      geo.height = 590;
      break;
#ifdef FT_HAS_NCODE
    case DUTYCYCLEOPTIONS_GEO:
      geo.width  = 650;
      geo.height = 400;
      geo.xPos   = modellerWidth - geo.width;
      geo.yPos   = modellerHeight - 275 - 210 - uiBorderWidth;
      break;
#endif
    case ANIMATIONCONTROL_GEO:
      geo.xPos   = 100;
      geo.yPos   = 350;
      geo.width  = 260;
      geo.height = 565;
      break;
    case VIEWSETTINGS_GEO:
      geo.xPos   = uiScreenWidth - 300;
      geo.yPos   = 2*uiTitleBarHeight + uiBorderWidth;
      geo.width  = 300;
      geo.height = 900;
      break;
    case RDBSELECTOR_GEO:
      geo.width  = 300;
      geo.height = modellerHeight - 2*uiBorderWidth - uiTitleBarHeight;
      geo.xPos   = 10;
      geo.yPos   = 20;
      break;
    case RDBMEFATIGUE_GEO:
      geo.width  = 400;
      geo.height = 450;
      geo.xPos   = (uiScreenWidth - geo.width)/2;
      geo.yPos   = modellerHeight/3;
      break;
    case PREFERENCES_GEO:
      geo.width  = 430;
      geo.height = 650;
      geo.xPos   = uiScreenWidth - geo.width;
      geo.yPos   = modellerHeight/3;
      break;
    case RESULTFILEBROWSER_GEO:
      geo.width  = 800;
      geo.height = 500;
      geo.xPos   = 100;
      geo.yPos   = 100;
      break;
    case LINKRAMSETTINGS_GEO:
      geo.xPos   = 100;
      geo.yPos   = 350;
      geo.width  = 370;
      geo.height = 590;
      break;
    case MODELPREFERENCES_GEO:
      geo.width  = 400;
      geo.height = 600;
      geo.xPos   = (uiScreenWidth - geo.width)/2;
      geo.yPos   = modellerHeight/3;
      break;
    case SEAENVIRONMENT_GEO:
      geo.width  = 400;
      geo.height = 470;
      geo.xPos   = (uiScreenWidth - geo.width)/2;
      geo.yPos   = modellerHeight/3;
      break;
    case AIRENVIRONMENT_GEO:
      geo.width  = 400;
      geo.height = 550;
      geo.xPos   = (uiScreenWidth - geo.width)/2;
      geo.yPos   = modellerHeight/3;
      break;
    case BLADEDEFINITION_GEO:
      geo.width  = 885;
      geo.height = 660;
      geo.xPos   = (uiScreenWidth - geo.width)/2;
      geo.yPos   = modellerHeight/3;
      break;
    case AIRFOILDEFINITION_GEO:
      geo.width  = 827;
      geo.height = 710;
      geo.xPos   = (uiScreenWidth - geo.width)/2;
      geo.yPos   = modellerHeight/3;
      break;
    case EVENTDEFINITION_GEO:
      geo.width  = 827;
      geo.height = 620;
      geo.xPos   = (uiScreenWidth - geo.width)/2;
      geo.yPos   = modellerHeight/3;
      break;
    case TURBINEASSEMBLY_GEO:
      geo.width  = 827;
      geo.height = 670;
      geo.xPos   = (uiScreenWidth - geo.width)/2;
      geo.yPos   = modellerHeight/3;
      break;
    case TURBINETOWER_GEO:
      geo.width  = 672;
      geo.height = 494;
      geo.xPos   = (uiScreenWidth - geo.width)/2;
      geo.yPos   = modellerHeight/3;
      break;
    case BEAMSTRINGPAIR_GEO:
      geo.width  = 827;
      geo.height = 350;
      geo.xPos   = (uiScreenWidth - geo.width)/2;
      geo.yPos   = modellerHeight/3;
      break;
    case OBJECTBROWSER_GEO:
      geo.width  = 827;
      geo.height = 650;
      geo.xPos   = (uiScreenWidth - geo.width)/2;
      geo.yPos   = modellerHeight/3;
      break;
    case BEAMCSSELECTOR_GEO:
      geo.width  = 350;
      geo.height = 900;
      geo.xPos   = 200;
      geo.yPos   = 200;
      break;
    case MODELEXPORT_GEO:
      geo.width  = 1000;
      geo.height = 550;
      geo.xPos   = (uiScreenWidth - geo.width) / 2;
      geo.yPos   = modellerHeight / 3;
      break;
    default:
      geo.xPos   = 200;
      geo.yPos   = 200;
      geo.width  = 200;
      geo.height = 200;
      break;
    }

  return geo;
}


void Fui::animationUI(bool onScreen, bool)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiModeller::getClassTypeID());

  if (uic) dynamic_cast<FuiModeller*>(uic)->mapAnimControls(onScreen);
}


void Fui::appearanceUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  inMem = true; // Always keep FuiAppearance in memory,
  // because the Done button has not finished when this function is called.
  // Deleting the window will make the Done button access already-freed memory.

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiAppearance::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
    {
      Fui::Geo geo = Fui::getGeo(APPEARANCE_GEO);
      uic = FuiAppearance::create(geo.xPos, geo.yPos, geo.width, geo.height);
    }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::viewSettingsUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiViewSettings::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
    {
      Fui::Geo geo = Fui::getGeo(VIEWSETTINGS_GEO);
      uic = FuiViewSettings::create(geo.xPos, geo.yPos, geo.width, geo.height);
    }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::analysisOptionsUI(bool onScreen, bool inMem, bool basicMode)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiAdvAnalysisOptions::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
  {
    Fui::Geo geo = Fui::getGeo(ADVANALYSISOPTIONS_GEO);
    uic = FuiAdvAnalysisOptions::create(geo.xPos, geo.yPos, geo.width, geo.height, basicMode);
  }
  else if (onScreen && uic)
    dynamic_cast<FuiAdvAnalysisOptions*>(uic)->setBasicMode(basicMode);

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::stressOptionsUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiStressOptions::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
    {
      Fui::Geo geo = Fui::getGeo(STRESSOPTIONS_GEO);
      uic = FuiStressOptions::create(geo.xPos, geo.yPos, geo.width, geo.height);
    }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::eigenmodeOptionsUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiEigenOptions::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
    {
      Fui::Geo geo = Fui::getGeo(EIGENMODEOPTIONS_GEO);
      uic = FuiEigenOptions::create(geo.xPos, geo.yPos, geo.width, geo.height);
    }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::gageOptionsUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiGageOptions::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
    {
      Fui::Geo geo = Fui::getGeo(GAGEOPTIONS_GEO);
      uic = FuiGageOptions::create(geo.xPos, geo.yPos, geo.width, geo.height);
    }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::fppOptionsUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiFppOptions::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
    {
      Fui::Geo geo = Fui::getGeo(FPPOPTIONS_GEO);
      uic = FuiFppOptions::create(geo.xPos, geo.yPos, geo.width, geo.height);
    }

  if (uic) uic->manage(onScreen,inMem);
}


#ifdef FT_HAS_NCODE
void Fui::dutyCycleOptionsUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiDutyCycleOptions::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
    {
      Fui::Geo geo = Fui::getGeo(DUTYCYCLEOPTIONS_GEO);
      uic = FuiDutyCycleOptions::create(geo.xPos, geo.yPos, geo.width, geo.height);
    }

  if (uic) uic->manage(onScreen,inMem);
}
#endif


void Fui::preferencesUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiPreferences::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
    {
      Fui::Geo geo = Fui::getGeo(PREFERENCES_GEO);
      uic = FuiPreferences::create(geo.xPos, geo.yPos, geo.width, geo.height);
    }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::modelPreferencesUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiModelPreferences::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
    {
      Fui::Geo geo = Fui::getGeo(MODELPREFERENCES_GEO);
      uic = FuiModelPreferences::create(geo.xPos, geo.yPos, geo.width, geo.height);
    }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::linkRamSettingsUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiLinkRamSettings::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
    {
      Fui::Geo geo = Fui::getGeo(LINKRAMSETTINGS_GEO);
      uic = FuiLinkRamSettings::create(geo.xPos, geo.yPos, geo.width, geo.height);
    }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::animationControlUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiAnimationControl::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
    {
      Fui::Geo geo = Fui::getGeo(ANIMATIONCONTROL_GEO);
      uic = FuiAnimationControl::create(geo.xPos, geo.yPos, geo.width, geo.height);
    }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::rdbSelectorUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiRDBSelector::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
    {
      Fui::Geo geo = Fui::getGeo(RDBSELECTOR_GEO);
      uic = FuiRDBSelector::create(geo.xPos, geo.yPos, geo.width, geo.height);
    }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::rdbMEFatigueUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

#ifdef FT_HAS_GRAPHVIEW
  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiRDBMEFatigue::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
  {
    Fui::Geo geo = Fui::getGeo(RDBMEFATIGUE_GEO);
    uic = FuiRDBMEFatigue::create(geo.xPos, geo.yPos, geo.width, geo.height);
  }
#else
  FFuTopLevelShell* uic = NULL;
#endif

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::modellerUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiModeller::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
    {
      Fui::Geo geo = Fui::getGeo(MODELLER_GEO);
      FFuComponentBase* parent = mainWindow->getWorkSpace();
      FuiModeller* modeller = FuiModeller::create(parent,geo.xPos, geo.yPos, geo.width, geo.height);
      uic = modeller;

      FapUAModeller::init(modeller);
    }

  if (uic) uic->manage(onScreen,inMem);

  // Show 3D views toolbar
  mainWindow->showToolBar(FuiMainWindow::THREEDVIEWS, onScreen);
  // Show view control toolbar including zoom-to
  if (mainWindow->isToolBarShown(FuiMainWindow::VIEWCTRL))
  {
    if (!onScreen)
    {
      // Hide the zoom-to button
      mainWindow->showToolBar(FuiMainWindow::VIEWCTRL, false);
      mainWindow->showToolBar(FuiMainWindow::VIEWCTRL2, true);
    }
  }
  else if (mainWindow->isToolBarShown(FuiMainWindow::VIEWCTRL2))
  {
    if (onScreen)
    {
      // Show the zoom-to button
      mainWindow->showToolBar(FuiMainWindow::VIEWCTRL2, false);
      mainWindow->showToolBar(FuiMainWindow::VIEWCTRL, true);
    }
  }
  else
    mainWindow->showToolBar(FuiMainWindow::VIEWCTRL1, onScreen);
}


void Fui::ctrlModellerUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiCtrlModeller::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
    {
      Fui::Geo geo = Fui::getGeo(CTRLMODELLER_GEO);
      FFuComponentBase* parent = mainWindow->getWorkSpace();
      uic = FuiCtrlModeller::create(parent,geo.xPos, geo.yPos, geo.width, geo.height);
    }

  if (uic) uic->manage(onScreen,inMem);

  // Show toolbars (control design and creation)
  mainWindow->showToolBar(FuiMainWindow::CTRLCREATE, onScreen);
  mainWindow->showToolBar(FuiMainWindow::CTRLMODELLINGTOOLS, onScreen);
}


void Fui::ctrlGridSnapUI(bool onScreen)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiCtrlModeller::getClassTypeID());

  if (uic) dynamic_cast<FuiCtrlModeller*>(uic)->showGridUI(onScreen);
}


void Fui::outputListUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiOutputList::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL) {
    Fui::Geo geo = Fui::getGeo(OUTPUTLIST_GEO);
    uic = FuiOutputList::create(geo.xPos, geo.yPos, geo.width, geo.height);
  }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::resultFileBrowserUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiMiniFileBrowser::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL) {
    Fui::Geo geo = Fui::getGeo(RESULTFILEBROWSER_GEO);
    uic = FuiMiniFileBrowser::create(geo.xPos, geo.yPos, geo.width, geo.height);
  }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::seaEnvironmentUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiSeaEnvironment::getClassTypeID());
  if ((onScreen || inMem) && uic == NULL) {
    Fui::Geo geo = Fui::getGeo(SEAENVIRONMENT_GEO);
    uic = FuiSeaEnvironment::create(geo.xPos, geo.yPos, geo.width, geo.height);
  }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::airEnvironmentUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

#ifdef FT_HAS_WND
  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiAirEnvironment::getClassTypeID());
  if ((onScreen || inMem) && uic == NULL && Fui::haveAeroDyn) {
    Fui::Geo geo = Fui::getGeo(AIRENVIRONMENT_GEO);
    uic = FuiAirEnvironment::create(geo.xPos, geo.yPos, geo.width, geo.height);
  }
#else
  FFuTopLevelShell* uic = NULL;
#endif

  if (uic) uic->manage(onScreen,inMem);
}


FuiTurbWind* Fui::turbWindUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return NULL;

#ifdef FT_HAS_WND
  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiTurbWind::getClassTypeID());
  if ((onScreen || inMem) && uic == NULL && Fui::haveAeroDyn)
    uic = FuiTurbWind::create();
#else
  FFuTopLevelShell* uic = NULL;
#endif

  if (uic) uic->manage(onScreen,inMem);
  return dynamic_cast<FuiTurbWind*>(uic);
}


void Fui::bladeDefinitionUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

#ifdef FT_HAS_WND
  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiBladeDefinition::getClassTypeID());
  if ((onScreen || inMem) && uic == NULL && Fui::haveAeroDyn) {
    Fui::Geo geo = Fui::getGeo(BLADEDEFINITION_GEO);
    uic = FuiBladeDefinition::create(geo.xPos, geo.yPos, geo.width, geo.height);
  }
#else
  FFuTopLevelShell* uic = NULL;
#endif

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::airfoilDefinitionUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

#ifdef FT_HAS_WND
  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiAirfoilDefinition::getClassTypeID());
  if ((onScreen || inMem) && uic == NULL && Fui::haveAeroDyn) {
    Fui::Geo geo = Fui::getGeo(AIRFOILDEFINITION_GEO);
    uic = FuiAirfoilDefinition::create(geo.xPos, geo.yPos, geo.width, geo.height);
  }
#else
  FFuTopLevelShell* uic = NULL;
#endif

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::turbineAssemblyUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

#ifdef FT_HAS_WND
  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiCreateTurbineAssembly::getClassTypeID());
  if ((onScreen || inMem) && uic == NULL && Fui::haveAeroDyn) {
    Fui::Geo geo = Fui::getGeo(TURBINEASSEMBLY_GEO);
    uic = FuiCreateTurbineAssembly::create(geo.xPos, geo.yPos, geo.width, geo.height);
  }
#else
  FFuTopLevelShell* uic = NULL;
#endif

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::turbineTowerUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

#ifdef FT_HAS_WND
  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiCreateTurbineTower::getClassTypeID());
  if ((onScreen || inMem) && uic == NULL && Fui::haveAeroDyn) {
    Fui::Geo geo = Fui::getGeo(TURBINETOWER_GEO);
    uic = FuiCreateTurbineTower::create(geo.xPos, geo.yPos, geo.width, geo.height);
  }
#else
  FFuTopLevelShell* uic = NULL;
#endif

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::beamstringPairUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiCreateBeamstringPair::getClassTypeID());
  if ((onScreen || inMem) && uic == NULL) {
    Fui::Geo geo = Fui::getGeo(BEAMSTRINGPAIR_GEO);
    uic = FuiCreateBeamstringPair::create(geo.xPos, geo.yPos, geo.width, geo.height);
  }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::objectBrowserUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiObjectBrowser::getClassTypeID());
  if ((onScreen || inMem) && uic == NULL) {
    Fui::Geo geo = Fui::getGeo(OBJECTBROWSER_GEO);
    uic = FuiObjectBrowser::create(geo.xPos, geo.yPos, geo.width, geo.height);
  }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::eventDefinitionUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiEventDefinition::getClassTypeID());
  if ((onScreen || inMem) && uic == NULL) {
    Fui::Geo geo = Fui::getGeo(EVENTDEFINITION_GEO);
    uic = FuiEventDefinition::create(geo.xPos, geo.yPos, geo.width, geo.height);
  }

  if (uic) uic->manage(onScreen,inMem);
}

void Fui::modelExportUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiModelExport::getClassTypeID());

  if ((onScreen || inMem) && uic == NULL)
  {
    Fui::Geo geo = Fui::getGeo(MODELEXPORT_GEO);
    uic = FuiModelExport::create(geo.xPos, geo.yPos, geo.width, geo.height);
  }

  if (uic) uic->manage(onScreen, inMem);
}

void Fui::beamCSSelectorUI(bool onScreen, bool inMem)
{
  if (!mainWindow) return;

  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiCSSelector::getClassTypeID());
  if ((onScreen || inMem) && uic == NULL) {
    Fui::Geo geo = Fui::getGeo(BEAMCSSELECTOR_GEO);
    uic = FuiCSSelector::create(geo.xPos, geo.yPos, geo.width, geo.height);
  }

  if (uic) uic->manage(onScreen,inMem);
}


void Fui::pluginsUI()
{
  FuiPlugins::getUI();
}


FuiGraphView* Fui::newGraphViewUI(const char* title)
{
  if (!mainWindow) return NULL;

#ifdef FT_HAS_GRAPHVIEW
  Fui::Geo geo = Fui::getGeo(GRAPHVIEW_GEO);
  return FuiGraphViewTLS::create(mainWindow->getWorkSpace(),
                                 geo.xPos,geo.yPos,geo.width,geo.height,title);
#else
  // Dummy statement to avoid compiler warning
  std::cout <<"  ** Fui::newGraphViewUI("<< title <<"): Not created."<< std::endl;
  return NULL;
#endif
}
