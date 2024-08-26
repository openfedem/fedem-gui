// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_H
#define FUI_H

#include "FFuLib/FFuDialogType.H"
#include <cstddef>

class FFuTopLevelShell;
class FFuComponentBase;
class FuiMainWindow;
class FuiGraphView;
class FuiTurbWind;

struct UIgeo
{
  int xPos, yPos;
  int width, height;
  UIgeo() { xPos = yPos = width = height = 0; }
};


class Fui
{
public:

  ///////////////////////////////////////////////
  // Fui class initialization
  // Must be run before further use of this class
  ///////////////////////////////////////////////

  // Note: argc must be passed as reference
  // to avoid crash in Qt6 for Release build on Linux
  static void init(int& argc, char** argv);


  ///////////////////////////////////////////////
  // User info interface
  ///////////////////////////////////////////////

  static void list(const char* text); // Output to Fedem list window
  static void tip(const char* value); // Tip label in main window
  static void setTitle(const char* title); // Application title (title bar)

  static void dismissDialog (const char* msg, int type = FFuDialog::WARNING);
  static void okDialog      (const char* msg, int type = FFuDialog::INFO);
  static bool okCancelDialog(const char* msg, int type = FFuDialog::QUESTION);
  static bool yesNoDialog   (const char* msg, int type = FFuDialog::QUESTION);
  static int  genericDialog (const char* msg, const char** buttonTexts,
                             int buttonCount, int type = FFuDialog::QUESTION);


  /////////////////////////
  // UI blocking interface
  /////////////////////////

  static void noUserInputPlease();
  static void okToGetUserInput();


  /////////////////////////////////
  // Global UI parameters interface
  /////////////////////////////////

  static FuiMainWindow*    getMainWindow() { return mainWindow; }
  static FFuComponentBase* getViewer();     // Get the 3D viewer
  static FFuComponentBase* getCtrlViewer(); // Get the control viewer


  //////////////////////////////////////
  // 3D picking and mode/state interface
  //////////////////////////////////////

  static void updateMode();
  static void updateState(int newState);
  static void cancel();


  ///////////////////////
  // Result state Method:
  ///////////////////////

  static void lockModel(bool yesOrNo);


  ////////////////////////
  // UI geometry interface
  ////////////////////////

  static UIgeo getUIgeo(int fuiType);


  //////////////////////////////////////////////////////////////////////////////
  // UI creation and destruction interface
  // onScreen: if true - UI is created and mapped on screen
  // inMem: if true - UI is created/kept in memory
  //                - only relevant if onScreen=false
  //////////////////////////////////////////////////////////////////////////////

  static void start();
  static void mainUI();
  static bool hasGUI() { return mainWindow != NULL; }
  static void updateUICommands();

  // regular toplevels
  static void animationUI        (bool onScreen = true, bool inMem = false);
  static void appearanceUI       (bool onScreen = true, bool inMem = false);
  static void analysisOptionsUI  (bool onScreen = true, bool inMem = false,
                                  bool basicMode = false);
  static void viewSettingsUI     (bool onScreen = true, bool inMem = false);
  static void stressOptionsUI    (bool onScreen = true, bool inMem = false);
  static void eigenmodeOptionsUI (bool onScreen = true, bool inMem = false);
  static void gageOptionsUI      (bool onScreen = true, bool inMem = false);
  static void fppOptionsUI       (bool onScreen = true, bool inMem = false);
  static void dutyCycleOptionsUI (bool onScreen = true, bool inMem = false);
  static void preferencesUI      (bool onScreen = true, bool inMem = false);
  static void animationControlUI (bool onScreen = true, bool inMem = false);
  static void rdbSelectorUI      (bool onScreen = true, bool inMem = false);
  static void rdbMEFatigueUI     (bool onScreen = true, bool inMem = false);
  static void linkRamSettingsUI  (bool onScreen = true, bool inMem = false);
  static void modelPreferencesUI (bool onScreen = true, bool inMem = false);
  static void seaEnvironmentUI   (bool onScreen = true, bool inMem = false);
  static void airEnvironmentUI   (bool onScreen = true, bool inMem = false);
  static FuiTurbWind* turbWindUI (bool onScreen = true, bool inMem = false);
  static void bladeDefinitionUI  (bool onScreen = true, bool inMem = false);
  static void airfoilDefinitionUI(bool onScreen = true, bool inMem = false);
  static void eventDefinitionUI  (bool onScreen = true, bool inMem = false);
  static void turbineAssemblyUI  (bool onScreen = true, bool inMem = false);
  static void turbineTowerUI     (bool onScreen = true, bool inMem = false);
  static void beamstringPairUI   (bool onScreen = true, bool inMem = false);
  static void beamCSSelectorUI   (bool onScreen = true, bool inMem = false);
  static void objectBrowserUI    (bool onScreen = true, bool inMem = false);
  static void resultFileBrowserUI(bool onScreen = true, bool inMem = false);
  static void outputListUI       (bool onScreen = true, bool inMem = false);
  static void modelExportUI      (bool onScreen = true, bool inMem = false);
  static void ctrlGridSnapUI     (bool onScreen = true);
  static void pluginsUI();

  // mdi toplevels
  static void modellerUI    (bool onScreen = true, bool inMem = false);
  static void ctrlModellerUI(bool onScreen = true, bool inMem = false);

  static FuiGraphView* newGraphViewUI(const char* title);

  // context-dependent help
  static void showCHM(const char* topic, const char* subtopic = NULL);

private:

  static FuiMainWindow* mainWindow;

public:

  static bool haveAeroDyn; //!< If \e true, the installation contains AeroDyn

  enum
  {
    //////////////////////////
    // User Interface objects
    FUI_MAINWINDOW_GEO,
    FUI_GRAPHVIEW_GEO,
    FUI_MODELLER_GEO,
    FUI_CTRLMODELLER_GEO,
    FUI_MODELMANAGER_GEO,
    FUI_PROPERTIES_GEO,
    FUI_OUTPUTLIST_GEO,
    FUI_APPEARANCE_GEO,
    FUI_VIEWSETTINGS_GEO,
    FUI_ADVANALYSISOPTIONS_GEO,
    FUI_STRESSOPTIONS_GEO,
    FUI_EIGENMODEOPTIONS_GEO,
    FUI_GAGEOPTIONS_GEO,
    FUI_FPPOPTIONS_GEO,
    FUI_DUTYCYCLEOPTIONS_GEO,
    FUI_PREFERENCES_GEO,
    FUI_ANIMATIONCONTROL_GEO,
    FUI_RDBSELECTOR_GEO,
    FUI_RDBMEFATIGUE_GEO,
    FUI_RESULTFILEBROWSER_GEO,
    FUI_MODELPREFERENCES_GEO,
    FUI_LINKRAMSETTINGS_GEO,
    FUI_SEAENVIRONMENT_GEO,
    FUI_AIRENVIRONMENT_GEO,
    FUI_BLADEDEFINITION_GEO,
    FUI_AIRFOILDEFINITION_GEO,
    FUI_TURBINEASSEMBLY_GEO,
    FUI_TURBINETOWER_GEO,
    FUI_BEAMSTRINGPAIR_GEO,
    FUI_OBJECTBROWSER_GEO,
    FUI_EVENTDEFINITION_GEO,
    FUI_BEAMCSSELECTOR_GEO,
    FUI_MODELEXPORT_GEO
  };
};

#endif
