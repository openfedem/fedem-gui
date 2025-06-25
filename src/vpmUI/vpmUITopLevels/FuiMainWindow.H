// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_MAIN_WINDOW_H
#define FUI_MAIN_WINDOW_H

#include <vector>
#include <array>

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUACommandHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"

class FuiSimModelListView;
class FuiResultListView;
class FuiModelManager;
class FuiProperties;
class FuiWorkSpace;
class FFuMenuBar;
class FFuToolBar;
class FFuaCmdItem;


/*!
  \brief The Fedem main window.

  The main window is not a FFuMultUIComponent although it look like.
  That is because it doesn't need any placeWidgets() geometry management, since
  that is managed by the splitters. Hence the central widget is a splitter.
  If one depreciates the use of splitters, this class should also inherit
  FFuMultUIComponent (but not with Qt branch) and QtMainWin should
  reimplement the resize event. See UserDialog for similarities.

  \author Dag R. Christensen
*/

class FuiMainWindow : virtual public FFuComponentBase,
		      public FFuUAExistenceHandler,
		      public FFuUACommandHandler,
		      public FFuUAFinishHandler
{
public:
  FuiMainWindow();
  virtual ~FuiMainWindow() {}

  static FuiMainWindow* create(int xpos =100, int ypos  =100,
			       int width=100, int height=100,
			       const char* title="Fedem Main Window");

  enum {// toolBars
    STD,
    WINDOWS,
    VIEWCTRL,
    THREEDVIEWS,
    SOLVE,
    MECHWIND,
    MECHCREATE,
    MECHMODELLINGTOOLS,
    CTRLCREATE,
    CTRLMODELLINGTOOLS,
    VIEWCTRL1,
    VIEWCTRL2,
    NTOOLBARS};

public:
  virtual void setTitle(const char* title) = 0;
  virtual const char* getTitle() const = 0;

  FuiSimModelListView* getModelManager();
  FuiResultListView* getResultManager();
  FuiProperties* getProperties() { return this->properties; }
  FuiWorkSpace* getWorkSpace() { return this->workSpace; }

  // Messages
  virtual void setStatusBarMessage(const char* message, int ms = 0) = 0;
  virtual void clearStatusBarMessage() = 0;

  virtual void setHintText(const char* text) = 0;
  virtual void showHintPanel(bool b) = 0;

  virtual void setStatus(const char* statusText) = 0;

  virtual void enableSubSteps(int steps) = 0;
  virtual void setSubStep(int step) = 0;
  virtual void disableSubSteps() = 0;

  virtual void displayTime(int hour, int min, int sec) = 0;
  virtual void clearTime() = 0;

  virtual void setSubTask(const char* taskText) = 0;

  virtual void enableProgress(int nSteps) = 0;
  virtual void setProgress(int progressStep) = 0;
  virtual void disableProgress() = 0;

  // show/hide comps
  void showModelManager(bool show);
  bool isModelManagerShown();

  void showProperties(bool show);
  bool isPropertiesShown();

  void showToolBar(int toolBar, bool show);
  bool isToolBarShown(int toolBar);

  virtual void removeToolBar(int toolBar) = 0;

protected:
  void initWidgets();

private:
  // Re-implemented from FFuUACommandHandler
  virtual void setUICommands(const FFuaUICommands* commands);
  virtual void updateUICommandsSensitivity(const FFuaUICommands* commands);
  virtual void updateUICommandsToggle(const FFuaUICommands* commands);

protected:
  // component "shadow" members
  FFuMenuBar* mainMenuBar;
  std::array<FFuToolBar*,NTOOLBARS> toolBars;

  FuiWorkSpace* workSpace;
  FuiProperties* properties;
  FuiModelManager* modelManager;
  FFuComponentBase* leftSplitter;
};


class FuaMainWindowCommands : public FFuaUICommands
{
public:
  FuaMainWindowCommands() {}
  virtual ~FuaMainWindowCommands() {}

  std::vector<FFuaCmdItem*> menuBar;
  std::array<std::vector<FFuaCmdItem*>,FuiMainWindow::NTOOLBARS> toolBars;
};

#endif
