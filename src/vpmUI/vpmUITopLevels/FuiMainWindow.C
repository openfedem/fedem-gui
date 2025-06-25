// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiMainWindow.H"
#include "vpmUI/vpmUITopLevels/FuiProperties.H"
#include "vpmUI/vpmUITopLevels/FuiModelManager.H"
#include "vpmUI/vpmUIComponents/FuiWorkSpace.H"
#include "FFuLib/FFuMenuBar.H"
#include "FFuLib/FFuToolBar.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"


FuiMainWindow::FuiMainWindow()
{
  this->mainMenuBar = NULL;
  this->toolBars.fill(NULL);
  this->workSpace = NULL;
  this->properties = NULL;
  this->modelManager = NULL;
  this->leftSplitter = NULL;
}
//----------------------------------------------------------------------------

void FuiMainWindow::initWidgets()
{
  this->toolBars[STD]->setBarLabel("Standard");
  this->toolBars[WINDOWS]->setBarLabel("Windows");
  this->toolBars[SOLVE]->setBarLabel("Solvers");
  this->toolBars[VIEWCTRL]->setBarLabel("Zoom and Pan");
  this->toolBars[VIEWCTRL1]->setBarLabel("Zoom");
  this->toolBars[VIEWCTRL2]->setBarLabel("Zoom and Pan");
  this->toolBars[THREEDVIEWS]->setBarLabel("3D View Control");
  this->toolBars[MECHWIND]->setBarLabel("Windpower Tools");
  this->toolBars[MECHCREATE]->setBarLabel("Mechanism Creation");
  this->toolBars[MECHMODELLINGTOOLS]->setBarLabel("Mechanism Tools");
  this->toolBars[CTRLCREATE]->setBarLabel("Control Creation");
  this->toolBars[CTRLMODELLINGTOOLS]->setBarLabel("Control Tools");

  // create ui's UA object
  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//----------------------------------------------------------------------------

void FuiMainWindow::setUICommands(const FFuaUICommands* commands)
{
  this->mainMenuBar->deleteItems();

  const FuaMainWindowCommands* cmds = static_cast<const FuaMainWindowCommands*>(commands);
  for (FFuaCmdItem* cmd : cmds->menuBar)
    this->mainMenuBar->insertCmdItem(cmd);

  for (size_t b = 0; b < this->toolBars.size(); b++) {
    this->toolBars[b]->clearBar();
    for (FFuaCmdItem* cmd : cmds->toolBars[b])
      this->toolBars[b]->insertCmdItem(cmd);
  }
}
//----------------------------------------------------------------------------

void FuiMainWindow::updateUICommandsSensitivity(const FFuaUICommands* commands)
{
  for (FFuaCmdItem* cmd : static_cast<const FuaMainWindowCommands*>(commands)->menuBar)
    this->mainMenuBar->updateCmdItem(cmd,true);

  for (FFuToolBar* toolbar : this->toolBars)
    toolbar->updateSensitivityOnAll();
}
//----------------------------------------------------------------------------

void FuiMainWindow::updateUICommandsToggle(const FFuaUICommands* commands)
{
  for (FFuaCmdItem* cmd : static_cast<const FuaMainWindowCommands*>(commands)->menuBar)
    this->mainMenuBar->updateCmdItem(cmd,false);

  for (FFuToolBar* toolbar : this->toolBars)
    toolbar->updateToggleOnAll();
}
//----------------------------------------------------------------------------

void FuiMainWindow::showModelManager(bool show)
{
  if (show)
    this->leftSplitter->popUp();
  else
    this->leftSplitter->popDown();
}
//----------------------------------------------------------------------------

bool FuiMainWindow::isModelManagerShown()
{
  return this->modelManager->isPoppedUp();
}
//----------------------------------------------------------------------------

void FuiMainWindow::showProperties(bool show)
{
  if (show)
    this->properties->popUp();
  else
    this->properties->popDown();
}
//----------------------------------------------------------------------------

bool FuiMainWindow::isPropertiesShown()
{
  return this->properties->isPoppedUp();
}
//----------------------------------------------------------------------------

void FuiMainWindow::showToolBar(int toolBar, bool show)
{
  if (show)
    this->toolBars[toolBar]->popUp();
  else
    this->toolBars[toolBar]->popDown();
}
//----------------------------------------------------------------------------

bool FuiMainWindow::isToolBarShown(int toolBar)
{
  return this->toolBars[toolBar]->isPoppedUp();
}
//----------------------------------------------------------------------------

FuiSimModelListView* FuiMainWindow::getModelManager()
{
  return modelManager->modelManager;
}
//----------------------------------------------------------------------------

FuiResultListView* FuiMainWindow::getResultManager()
{
  return modelManager->resultManager;
}
