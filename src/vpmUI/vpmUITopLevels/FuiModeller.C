// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiModeller.H"
#include "vpmUI/FuiModes.H"
#include "vpmUI/Fui.H"

#include "vpmUI/vpmUITopLevels/FuiMainWindow.H"//qtworkspace hack
#include "vpmUI/vpmUIComponents/FuiWorkSpace.H"//qtworkspace hack

#include "vpmUI/vpmUIComponents/FuiPlayPanel.H"
#include "vpmUI/vpmUIComponents/Fui3DPoint.H"


Fmd_SOURCE_INIT(FUI_MODELLER, FuiModeller, FFuMDIWindow);


FuiModeller::FuiModeller()
{
  Fmd_CONSTRUCTOR_INIT(FuiModeller);

  myViewer = NULL;
  my3DpointUI = NULL;
  myPlayPanel = NULL;
}

//////////////////////////////////////////////////////////////////////
//
//  Initialisation to be called from GUILib-dependent subclass constructor
//

void FuiModeller::initWidgets()
{
  if (myViewer)
    myViewer->toBack();

  my3DpointUI->toFront();
  myPlayPanel->toFront();
  myPlayPanel->popDown();
}


//////////////////////////////////////////////////////////////////////
//
//  Access interface :
//

void FuiModeller::mapAnimControls(bool yesOrNo)
{
  if (yesOrNo)
    this->myPlayPanel->popUp();
  else
    this->myPlayPanel->popDown();
}


bool FuiModeller::onClose()
{
  //TODO introduce the finish hadler
  FuiModes::cancel();
  Fui::modellerUI(false,true);
  return false;
}


void FuiModeller::onPoppedDownToMem()
{
  //TMP hack since qworkspace works bad
  Fui::getMainWindow()->getWorkSpace()->sendWindowActivated();
}
