// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiTopLevelDialog.H"
#include "FFuLib/FFuDialogButtons.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"


enum {
  DONE = FFuDialogButtons::LEFTBUTTON,
  APPLY = FFuDialogButtons::MIDBUTTON,
  CANCEL = FFuDialogButtons::RIGHTBUTTON
};


void FuiTopLevelDialog::initWidgets()
{
  this->dialogButtons->setButtonClickedCB(FFaDynCB1M(FuiTopLevelDialog,this,
						     onDialogButtonClicked,int));
  this->dialogButtons->setButtonLabel(DONE,"OK");
  this->dialogButtons->setButtonLabel(APPLY,"Apply");
  this->dialogButtons->setButtonLabel(CANCEL,"Cancel");

  FFuUAExistenceHandler::invokeCreateUACB(this);
}


void FuiTopLevelDialog::onPoppedUp()
{
  this->updateUIValues();
}


void FuiTopLevelDialog::onDialogButtonClicked(int button)
{
  switch (button) {
  case DONE:
    if (this->updateDBValues(false))
      this->invokeFinishedCB();
    break;
  case APPLY:
    this->updateDBValues(true);
    break;
  case CANCEL:
    this->invokeFinishedCB();
  }
}


bool FuiTopLevelDialog::onClose()
{
  this->invokeFinishedCB();
  return false;
}


void FuiTopLevelDialog::setSensitivity(bool sens)
{
  this->dialogButtons->setButtonSensitivity(DONE,sens);
  this->dialogButtons->setButtonSensitivity(APPLY,sens);
}
