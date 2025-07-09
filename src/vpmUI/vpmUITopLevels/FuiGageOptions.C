// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiGageOptions.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuPushButton.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"


Fmd_SOURCE_INIT(FUI_GAGEOPTIONS,FuiGageOptions,FFuTopLevelShell);

//----------------------------------------------------------------------------

FuiGageOptions::FuiGageOptions()
{
  Fmd_CONSTRUCTOR_INIT(FuiGageOptions);
}
//----------------------------------------------------------------------------

void FuiGageOptions::setSensitivity(bool sens)
{
  this->importRosFileButton->setSensitivity(sens);
  this->FuiTopLevelDialog::setSensitivity(sens);
}
//----------------------------------------------------------------------------

void FuiGageOptions::initWidgets()
{
  time->setResetCB(FFaDynCB0M(FFaDynCB0,&resetTimeCB,invoke));

  this->autoDacExportToggle->setLabel("Direct export of gage strains to DAC files");
  this->autoDacExportToggle->setToggleCB(FFaDynCB1M(FFuComponentBase,dacSampleRateField,
                                                    setSensitivity,bool));

  this->rainflowToggle->setLabel("Report cyclecount");
  this->rainflowToggle->setToggleCB(FFaDynCB1M(FFuComponentBase,binSizeField,
                                               setSensitivity,bool));

  this->importRosFileButton->setLabel("Import strain rosette file...");

  this->FuiTopLevelDialog::initWidgets();
}
//----------------------------------------------------------------------------

FFuaUIValues* FuiGageOptions::createValuesObject()
{
  return new FuaGageOptionsValues();
}
//----------------------------------------------------------------------------

void FuiGageOptions::setUIValues(const FFuaUIValues* values)
{
  FuaGageOptionsValues* gageValues = (FuaGageOptionsValues*) values;

  this->time->setUIValues(&gageValues->timeValues);
  this->autoDacExportToggle->setValue(gageValues->autoDacSwitch);
  this->dacSampleRateField->setValue(gageValues->dacSampleRate);
  this->dacSampleRateField->setSensitivity(gageValues->autoDacSwitch);
  this->dacSampleRateField->setSensitivity(gageValues->autoDacSwitch);
  this->rainflowToggle->setValue(gageValues->rainflowSwitch);
  this->binSizeField->setValue(gageValues->binSize);
  this->binSizeField->setSensitivity(gageValues->rainflowSwitch);
  this->binSizeField->setSensitivity(gageValues->rainflowSwitch);
  this->setSensitivity(gageValues->isSensitive);
}
//-----------------------------------------------------------------------------

void FuiGageOptions::getUIValues(FFuaUIValues* values)
{
  FuaGageOptionsValues* gageValues = (FuaGageOptionsValues*) values;

  this->time->getUIValues(&gageValues->timeValues);
  gageValues->autoDacSwitch = this->autoDacExportToggle->getValue();
  gageValues->dacSampleRate = this->dacSampleRateField->getDouble();
  gageValues->rainflowSwitch = this->rainflowToggle->getValue();
  gageValues->binSize = this->binSizeField->getInt();
}
//-----------------------------------------------------------------------------

void FuiGageOptions::setTimeUIValues(const FuaTimeIntervalValues* timeValues)
{
  this->time->setUIValues(timeValues);
}
//-----------------------------------------------------------------------------

void FuiGageOptions::setImportRosetteFileCB(const FFaDynCB0& dynCB)
{
  this->importRosFileButton->setActivateCB(dynCB);
}
//-----------------------------------------------------------------------------
