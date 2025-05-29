// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiRDBSelector.H"
#include "vpmUI/vpmUIComponents/FuiItemsListViews.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuDialogButtons.H"


enum {
  DONE = FFuDialogButtons::LEFTBUTTON,
  APPLY = FFuDialogButtons::MIDBUTTON,
  CANCEL = FFuDialogButtons::RIGHTBUTTON
};


Fmd_SOURCE_INIT(FcFUIRDBSELECTOR, FuiRDBSelector, FFuTopLevelShell);


//----------------------------------------------------------------------------

FuiRDBSelector::FuiRDBSelector()
{
  Fmd_CONSTRUCTOR_INIT(FuiRDBSelector);

  lvRes = NULL;
  lvPos = NULL;
  notesText = NULL;
  dialogButtons = NULL;
}
//----------------------------------------------------------------------------

void FuiRDBSelector::initWidgets()
{
  dialogButtons->setButtonClickedCB(FFaDynCB1M(FuiRDBSelector,this,
                                               onDialogButtonClicked,int));
  notesText->setLabel(
    "You can \"drag and drop\" these fields to the results viewer\n"
    "(i.e., the Results tree on the left side of the main window).");

  this->setOkCancelDialog(true);

  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//----------------------------------------------------------------------------

void FuiRDBSelector::setOkCancelDialog(bool yesOrNo)
{
  dialogButtons->setButtonLabel(DONE, yesOrNo ? "OK" : NULL);
  dialogButtons->setButtonLabel(APPLY, yesOrNo ? "Apply" : NULL);
  dialogButtons->setButtonLabel(CANCEL, yesOrNo ? "Cancel" : "Close");
}
//----------------------------------------------------------------------------

void FuiRDBSelector::setApplyable(bool able)
{
  dialogButtons->setButtonSensitivity(DONE,able);
  dialogButtons->setButtonSensitivity(APPLY,able);
}
//----------------------------------------------------------------------------

bool FuiRDBSelector::onClose()
{
  this->invokeFinishedCB();
  return false;
}
//----------------------------------------------------------------------------

void FuiRDBSelector::onDialogButtonClicked(int button)
{
  if (button == DONE || button == APPLY)
    this->resultAppliedCB.invoke();

  if (button == DONE || button == CANCEL)
    this->invokeFinishedCB();
}
