// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiTimeInterval.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuToggleButton.H"


void FuiTimeInterval::setSensitivity(bool isSensitive)
{
  this->startField->setSensitivity(isSensitive);
  this->stopField->setSensitivity(isSensitive);
  this->incrField->setSensitivity(isSensitive && !this->allStepsToggle->getValue());

  this->allStepsToggle->setSensitivity(isSensitive);
  this->resetButton->setSensitivity(isSensitive);
}
//----------------------------------------------------------------------------

void FuiTimeInterval::initWidgets()
{
  allStepsToggle->setToggleCB(FFaDynCB1M(FFuComponentBase,incrField,setDisabled,bool));
  resetButton->setActivateCB(FFaDynCB0M(FFaDynCB0,&resetCB,invoke));
//-----------------------------------------------------------------------------

  this->startField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->startField->setDoubleDisplayMode(FFuIOField::AUTO,12,1);
  this->stopField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->stopField->setDoubleDisplayMode(FFuIOField::AUTO,12,1);
  this->incrField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->incrField->setDoubleDisplayMode(FFuIOField::AUTO,12,1);

  this->allStepsToggle->setLabel("Use all time steps");
  this->resetButton->setLabel("Reset");
}
//-----------------------------------------------------------------------------

void FuiTimeInterval::setUIValues(const FuaTimeIntervalValues* timeValues)
{
  this->startField->setValue(timeValues->start);
  this->stopField->setValue(timeValues->stop);
  if (!timeValues->dontTouchIncr)
    this->incrField->setValue(timeValues->incr);
  this->incrField->setSensitivity(!timeValues->allSteps);
  this->allStepsToggle->setValue(timeValues->allSteps);
}
//-----------------------------------------------------------------------------

void FuiTimeInterval::getUIValues(FuaTimeIntervalValues* timeValues)
{
  timeValues->start = this->startField->getDouble();
  timeValues->stop = this->stopField->getDouble();
  timeValues->incr = this->incrField->getDouble();
  timeValues->allSteps = this->allStepsToggle->getValue();
}
//-----------------------------------------------------------------------------
