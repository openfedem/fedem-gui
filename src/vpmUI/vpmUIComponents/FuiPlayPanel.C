// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiPlayPanel.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuScale.H"


//////////////////////////////////////////////////////////////////////
//
// Initialise method to be called from subclass constructor :
//

void FuiPlayPanel::init()
{
  myButtons[PLAY_FWD]->setPixMap(animPlay_xpm);
  myButtons[PLAY_REW]->setPixMap(animPlayRwd_xpm);
  myButtons[STEP_FWD]->setPixMap(animStepFwd_xpm);
  myButtons[STEP_REW]->setPixMap(animStepRwd_xpm);
  myButtons[TO_FIRST]->setPixMap(animToFirst_xpm);
  myButtons[TO_LAST] ->setPixMap(animToLast_xpm);
  myButtons[PAUSE]   ->setPixMap(animPause_xpm);
  myButtons[STOP]    ->setPixMap(animStop_xpm);

  myButtons[REALTIME]->setLabel("Real");
  myCloseButton->setLabel("Close");

  myToggles[ONESHOT]->setLabel("Show all frames");
  myToggles[CONTINOUS]->setLabel("Continous");
  myToggles[CONTINOUS_CYCLE]->setLabel("Cycle");

  myButtons[PLAY_FWD]->setActivateCB(FFaDynCB0M(FuiPlayPanel,this,playFwdCB));
  myButtons[PLAY_REW]->setActivateCB(FFaDynCB0M(FuiPlayPanel,this,playRwdCB));
  myButtons[PAUSE]   ->setActivateCB(FFaDynCB0M(FuiPlayPanel,this,pauseCB));
  myButtons[TO_FIRST]->setActivateCB(FFaDynCB0M(FuiPlayPanel,this,toFirstCB));
  myButtons[TO_LAST] ->setActivateCB(FFaDynCB0M(FuiPlayPanel,this,toLastCB));
  myButtons[STEP_FWD]->setActivateCB(FFaDynCB0M(FuiPlayPanel,this,stepFwdCB));
  myButtons[STEP_REW]->setActivateCB(FFaDynCB0M(FuiPlayPanel,this,stepRwdCB));
  myButtons[STOP]    ->setActivateCB(FFaDynCB0M(FuiPlayPanel,this,stopCB));

  myButtons[REALTIME]->setActivateCB(FFaDynCB0M(FuiPlayPanel,this,onRealTimePressed));
  myCloseButton->setActivateCB(FFaDynCB0M(FFaDynCB0,&closeAnimCB,invoke));

  myToggles[ONESHOT]->setToggleCB(FFaDynCB1M(FuiPlayPanel,this,onAllFramesToggled,bool));
  myToggles[CONTINOUS]->setToggleCB(FFaDynCB1M(FuiPlayPanel,this,onAnimTypeChanged,bool));
  myToggles[CONTINOUS_CYCLE]->setToggleCB(FFaDynCB1M(FuiPlayPanel,this,onAnimTypeChanged,bool));

  mySpeedScale->setMinMax(1,100);
  mySpeedScale->setValue(50);
  mySpeedScale->setDragCB(FFaDynCB1M(FuiPlayPanel,this,onSpeedScaleChanged,int));
}


//////////////////////////////////////////////////////////////////////
//
// Set values :
//

void FuiPlayPanel::makeSpeedControllsInactive(bool yesOrNo)
{
  myButtons[REALTIME]->setSensitivity(!yesOrNo);
  mySpeedScale->setSensitivity(yesOrNo);
}

void FuiPlayPanel::setSpeedScale(int value)
{
  mySpeedScale->setValue(value);
}

//////////////////////////////////////////////////////////////////////
//
// Get values :
//

bool FuiPlayPanel::isPingPong()
{
  return myToggles[CONTINOUS_CYCLE]->getValue();
}

bool FuiPlayPanel::isContinous()
{
  return myToggles[CONTINOUS]->getValue();
}

bool FuiPlayPanel::isShowAllFrames()
{
  return myToggles[ONESHOT]->getValue();
}

int FuiPlayPanel::getSpeedScaleValue()
{
  return mySpeedScale->getValue();
}

//////////////////////////////////////////////////////////////////////
//
// Set Callbacks :
//

void FuiPlayPanel::setAnimationPressCB(const FFaDynCB1<int>& aDynCB)
{
  myAnimationPressCB = aDynCB;
}

void FuiPlayPanel::setAnimTypeChangedCB(const FFaDynCB1<int>& aDynCB)
{
  myAnimTypeChangedCB = aDynCB;
}

void FuiPlayPanel::setRealTimeCB(const FFaDynCB0& aDynCB)
{
  myRealtimePressedCB = aDynCB;
}

void FuiPlayPanel::setSpeedScaleChangeCB(const FFaDynCB1<int>& aDynCB)
{
  mySpeedScaleChangedCB = aDynCB;
}

void FuiPlayPanel::setAllFramesCB(const FFaDynCB1<bool>& aDynCB)
{
  myAllFramesToggeledCB = aDynCB;
}

void FuiPlayPanel::setCloseAnimCB(const FFaDynCB0& aDynCB)
{
  closeAnimCB = aDynCB;
}


//////////////////////////////////////////////////////////////////////
//
// Internal Callbacks that invokes the external callbacks :
// Mapping from widget CB's to Component CB's
//

void FuiPlayPanel::playFwdCB()
{
  this->myAnimationPressCB.invoke(PLAY_FWD);
}

void FuiPlayPanel::playRwdCB()
{
  this->myAnimationPressCB.invoke(PLAY_REW);
}

void FuiPlayPanel::pauseCB()
{
  this->myAnimationPressCB.invoke(PAUSE);
}

void FuiPlayPanel::toFirstCB()
{
  this->myAnimationPressCB.invoke(TO_FIRST);
}

void FuiPlayPanel::toLastCB()
{
  this->myAnimationPressCB.invoke(TO_LAST);
}

void FuiPlayPanel::stepFwdCB()
{
  this->myAnimationPressCB.invoke(STEP_FWD);
}

void FuiPlayPanel::stepRwdCB()
{
  this->myAnimationPressCB.invoke(STEP_REW);
}

void FuiPlayPanel::stopCB()
{
  this->myAnimationPressCB.invoke(STOP);
}


void FuiPlayPanel::onAnimTypeChanged(bool)
{
  if (!this->isContinous())
    myAnimTypeChangedCB.invoke(ONESHOT);
  else if (this->isPingPong())
    myAnimTypeChangedCB.invoke(CONTINOUS_CYCLE);
  else
    myAnimTypeChangedCB.invoke(CONTINOUS);
}


void FuiPlayPanel::onRealTimePressed()
{
  this->setSpeedScale(50);
  myRealtimePressedCB.invoke();
}


void FuiPlayPanel::onAllFramesToggled(bool val)
{
  myAllFramesToggeledCB.invoke(val);
}


void FuiPlayPanel::onSpeedScaleChanged(int val)
{
  mySpeedScaleChangedCB.invoke(val);
}


void FuiPlayPanel::onPoppedUp()
{
  this->onAnimTypeChanged(true);
  this->onAllFramesToggled(this->isShowAllFrames());
  this->onSpeedScaleChanged(this->getSpeedScaleValue());
}
