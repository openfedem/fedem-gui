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

#ifndef UITEST
#include "vpmApp/vpmAppUAMap/FapUAModeller.H"
#endif


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

//////////////////////////////////////////////////////////////////////
//
//  Setting external CB's :
//

void FuiModeller::setPointChangedCB(const FFaDynCB2<const FaVec3&,bool>& aDynCB)
{
  my3DpointUI->setPointChangedCB(aDynCB);
}

void FuiModeller::setRefChangedCB( const FFaDynCB1<bool> &aDynCB)
{
  my3DpointUI->setRefChangedCB(aDynCB);
}

void FuiModeller::setAnimTypeChangedCB( const FFaDynCB1<int>& aDynCB)
{
  this->myPlayPanel->setAnimTypeChangedCB(aDynCB);
}

void FuiModeller::setAnimationPressCB(const  FFaDynCB1<int>& aDynCB)
{
  this->myPlayPanel->setAnimationPressCB(aDynCB);
}

void FuiModeller::setRealTimeCB(const  FFaDynCB0 & aDynCB)
{
  this->myPlayPanel->setRealTimeCB(aDynCB);
}

void FuiModeller::setSpeedScaleChangeCB(const FFaDynCB1<int>& aDynCB)
{
  this->myPlayPanel->setSpeedScaleChangeCB(aDynCB);
}

void FuiModeller::setCloseAnimCB(const FFaDynCB0& dynCB)
{
  this->myPlayPanel->setCloseAnimCB(dynCB);
}

void FuiModeller::setAllFramesCB(const FFaDynCB1<bool>& aDynCB)
{
  this->myPlayPanel->setAllFramesCB(aDynCB);
}


//////////////////////////////////////////////////////////////////////
//
// Virtual mode and state handling :
//

void FuiModeller::updateMode()
{
#ifndef UITEST
  FapUAModeller::updateMode();
#endif
}

void FuiModeller::updateState(int newState)
{
#ifndef UITEST
  FapUAModeller::updateState(newState);
#endif
}

void FuiModeller::cancel()
{
#ifndef UITEST
  FapUAModeller::cancel();
#endif
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
