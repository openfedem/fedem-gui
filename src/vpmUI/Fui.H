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
class FFuListView;
class FuiMainWindow;
class FuiProperties;
class FuiGraphView;
class FuiTurbWind;


namespace Fui
{
  ///////////////////////////////////////////////
  // UI initialization. Must be run before
  // further use of any other Fui-methods.
  ///////////////////////////////////////////////

  // Note: argc must be passed as reference
  // to avoid crash in Qt6 for Release build on Linux
  void init(int& argc, char** argv);


  ///////////////////////////////////////////////
  // User info interface
  ///////////////////////////////////////////////

  void list(const char* text); // Output to Fedem list window
  void tip(const char* value); // Tip label in main window
  void setTitle(const char* title); // Application title (title bar)

  void dismissDialog (const char* msg, int type = FFuDialog::WARNING);
  void okDialog      (const char* msg, int type = FFuDialog::INFO);
  bool okCancelDialog(const char* msg, int type = FFuDialog::QUESTION);
  bool yesNoDialog   (const char* msg, int type = FFuDialog::QUESTION);
  int  genericDialog (const char* msg, const char** buttonTexts,
                      int buttonCount, int type = FFuDialog::QUESTION);


  /////////////////////////
  // UI blocking interface
  /////////////////////////

  void noUserInputPlease();
  void okToGetUserInput();


  /////////////////////////////////
  // Global UI parameters interface
  /////////////////////////////////

  FuiMainWindow*    getMainWindow(); // Get the main window
  FuiProperties*    getProperties(); // Get the property editor view
  FFuComponentBase* getViewer();     // Get the 3D viewer
  FFuComponentBase* getCtrlViewer(); // Get the control viewer
  FFuListView*      getTopologyList(); // Get the topology list view


  //////////////////////////////////////
  // 3D picking and mode/state interface
  //////////////////////////////////////

  void updateMode();
  void updateState(int newState);
  void cancel();


  ///////////////////////
  // Result state Method:
  ///////////////////////

  void lockModel(bool yesOrNo);


  ////////////////////////
  // UI geometry interface
  ////////////////////////

  struct Geo
  {
    int xPos = 0;
    int yPos = 0;
    int width = 0;
    int height = 0;
  };

  enum UIgeom
  {
    MAINWINDOW_GEO,
    GRAPHVIEW_GEO,
    MODELLER_GEO,
    CTRLMODELLER_GEO,
    OUTPUTLIST_GEO,
    APPEARANCE_GEO,
    VIEWSETTINGS_GEO,
    ADVANALYSISOPTIONS_GEO,
    STRESSOPTIONS_GEO,
    EIGENMODEOPTIONS_GEO,
    GAGEOPTIONS_GEO,
    FPPOPTIONS_GEO,
    PREFERENCES_GEO,
    ANIMATIONCONTROL_GEO,
    RDBSELECTOR_GEO,
    RDBMEFATIGUE_GEO,
    RESULTFILEBROWSER_GEO,
    MODELPREFERENCES_GEO,
    LINKRAMSETTINGS_GEO,
    SEAENVIRONMENT_GEO,
    AIRENVIRONMENT_GEO,
    BLADEDEFINITION_GEO,
    AIRFOILDEFINITION_GEO,
    TURBINEASSEMBLY_GEO,
    TURBINETOWER_GEO,
    BEAMSTRINGPAIR_GEO,
    OBJECTBROWSER_GEO,
    EVENTDEFINITION_GEO,
    BEAMCSSELECTOR_GEO,
    MODELEXPORT_GEO
  };

  Geo getGeo(UIgeom fuiType);


  //////////////////////////////////////////////////////////////////////////////
  // UI creation and destruction interface
  // onScreen: if true - UI is created and mapped on screen
  // inMem: if true - UI is created/kept in memory
  //                - only relevant if onScreen=false
  //////////////////////////////////////////////////////////////////////////////

  void start();
  void mainUI();
  bool hasGUI();
  void updateUICommands();

  // regular toplevels
  void animationUI        (bool onScreen = true, bool inMem = false);
  void appearanceUI       (bool onScreen = true, bool inMem = false);
  void analysisOptionsUI  (bool onScreen = true, bool inMem = false,
                           bool basicMode = false);
  void viewSettingsUI     (bool onScreen = true, bool inMem = false);
  void stressOptionsUI    (bool onScreen = true, bool inMem = false);
  void eigenmodeOptionsUI (bool onScreen = true, bool inMem = false);
  void gageOptionsUI      (bool onScreen = true, bool inMem = false);
  void fppOptionsUI       (bool onScreen = true, bool inMem = false);
  void preferencesUI      (bool onScreen = true, bool inMem = false);
  void animationControlUI (bool onScreen = true, bool inMem = false);
  void rdbSelectorUI      (bool onScreen = true, bool inMem = false);
  void rdbMEFatigueUI     (bool onScreen = true, bool inMem = false);
  void linkRamSettingsUI  (bool onScreen = true, bool inMem = false);
  void modelPreferencesUI (bool onScreen = true, bool inMem = false);
  void seaEnvironmentUI   (bool onScreen = true, bool inMem = false);
  void airEnvironmentUI   (bool onScreen = true, bool inMem = false);
  FuiTurbWind* turbWindUI (bool onScreen = true, bool inMem = false);
  void bladeDefinitionUI  (bool onScreen = true, bool inMem = false);
  void airfoilDefinitionUI(bool onScreen = true, bool inMem = false);
  void eventDefinitionUI  (bool onScreen = true, bool inMem = false);
  void turbineAssemblyUI  (bool onScreen = true, bool inMem = false);
  void turbineTowerUI     (bool onScreen = true, bool inMem = false);
  void beamstringPairUI   (bool onScreen = true, bool inMem = false);
  void beamCSSelectorUI   (bool onScreen = true, bool inMem = false);
  void objectBrowserUI    (bool onScreen = true, bool inMem = false);
  void resultFileBrowserUI(bool onScreen = true, bool inMem = false);
  void outputListUI       (bool onScreen = true, bool inMem = false);
  void modelExportUI      (bool onScreen = true, bool inMem = false);
  void ctrlGridSnapUI     (bool onScreen = true);
  void pluginsUI();

  // mdi toplevels
  void modellerUI    (bool onScreen = true, bool inMem = false);
  void ctrlModellerUI(bool onScreen = true, bool inMem = false);

  FuiGraphView* newGraphViewUI(const char* title);

  //! Context-dependent help.
  void showCHM(const char* topic, const char* subtopic = NULL);

  extern bool haveAeroDyn; //!< If \e true, the installation contains AeroDyn
}

#endif
