// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiRDBMEFatigue.H"
#include "vpmUI/vpmUIComponents/FuiItemsListViews.H"
#include "vpmUI/Fui.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuTable.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuDialogButtons.H"

enum {
  DONE = FFuDialogButtons::LEFTBUTTON,
  HELP = FFuDialogButtons::RIGHTBUTTON
};

Fmd_SOURCE_INIT(FcFUIRDBMEFATIGUE, FuiRDBMEFatigue, FFuTopLevelShell);

//----------------------------------------------------------------------------

FuiRDBMEFatigue::FuiRDBMEFatigue()
{
  Fmd_CONSTRUCTOR_INIT(FuiRDBMEFatigue);
  this->dialogButtons = NULL;
}
//----------------------------------------------------------------------------

void FuiRDBMEFatigue::initWidgets()
{
  this->dialogButtons->setButtonClickedCB(FFaDynCB1M(FuiRDBMEFatigue,this,
						     onDialogButtonClicked,int));

  this->useProbToggle->setLabel("Use probability weighted results");
  this->useProbToggle->setValue(true);
  this->copyDataButton->setLabel("Copy data");
  this->startTimeField->setSensitivity(false);
  this->stopTimeField->setSensitivity(false);
  this->standardField->setSensitivity(false);
  this->snCurveField->setSensitivity(false);

  this->copyDataButton->setActivateCB(FFaDynCB0M(FuiRDBMEFatigue,this,onCopyDataClicked));

  notes->setText("This table shows a fatigue summary of all events and curves. "
                 "Click to toggle whether the row is included in the sum.");

  this->dialogButtons->setButtonLabel(DONE,"Close");
  this->dialogButtons->setButtonLabel(HELP,"Help");

  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//----------------------------------------------------------------------------

void FuiRDBMEFatigue::onDialogButtonClicked(int button)
{
  switch (button) {
  case DONE:
    this->invokeFinishedCB();
    break;
  case HELP:
    // Display the topic in the help file
    Fui::showCHM("dialogbox/fatigue-summary.htm");
    break;
  }
}
//----------------------------------------------------------------------------

bool FuiRDBMEFatigue::onClose()
{
  this->invokeFinishedCB();
  return false;
}
