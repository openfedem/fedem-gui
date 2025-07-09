// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuScrolledList.H"
#include "FFuLib/FFuDialogButtons.H"
#include "FFuLib/FFuScrolledListDialog.H"


FFuScrolledListDialog::FFuScrolledListDialog()
{
  myItemSelector  = NULL;
  myNotesLabel    = NULL;
  myNotesText     = NULL;
  myDialogButtons = NULL;
}


void FFuScrolledListDialog::initWidgets()
{
  myDialogButtons->setButtonLabel(FFuDialogButtons::LEFTBUTTON, "OK");
  myDialogButtons->setButtonLabel(FFuDialogButtons::MIDBUTTON, "Apply");
  myDialogButtons->setButtonLabel(FFuDialogButtons::RIGHTBUTTON, "Cancel");

  if (myNotesText)
  {
    myNotesText->setLabel("No notes yet");
    myNotesLabel->popDown();
    myNotesText->popDown();
  }

  myDialogButtons->setButtonClickedCB(FFaDynCB1M(FFuScrolledListDialog,this,
                                                 onDlgButtonClicked,int));
}


void FFuScrolledListDialog::setNotesText(const char* text)
{
  if (myNotesText)
  {
    myNotesText->setLabel(text);
    myNotesLabel->popUp();
    myNotesText->popUp();
  }
}


int FFuScrolledListDialog::getItem() const
{
  return myItemSelector->getSelectedItemIndex();
}


void FFuScrolledListDialog::setOkButtonClickedCB(const FFaDynCB1<int>& aDynCB)
{
  myOkButtonClickedCB = aDynCB;
}

void FFuScrolledListDialog::setOkButtonClickedCB(const FFaDynCB2<int,FFuComponentBase*>& aDynCB)
{
  myOkButtonClickedWPtrCB = aDynCB;
}


void FFuScrolledListDialog::setCancelButtonClickedCB(const FFaDynCB1<int>& aDynCB)
{
  myCancelButtonClickedCB = aDynCB;
}

void FFuScrolledListDialog::setCancelButtonClickedCB(const FFaDynCB2<int,FFuComponentBase*>& aDynCB)
{
  myCancelButtonClickedWPtrCB = aDynCB;
}


void FFuScrolledListDialog::setApplyButtonClickedCB(const FFaDynCB1<int>& aDynCB)
{
  myApplyButtonClickedCB = aDynCB;
}

void FFuScrolledListDialog::setApplyButtonClickedCB(const FFaDynCB2<int,FFuComponentBase*>& aDynCB)
{
  myApplyButtonClickedWPtrCB = aDynCB;
}


void FFuScrolledListDialog::onDlgButtonClicked(int val)
{
  switch (val) {
  case FFuDialogButtons::LEFTBUTTON:
    myOkButtonClickedCB.invoke(myItemSelector->getSelectedItemIndex());
    myOkButtonClickedWPtrCB.invoke(myItemSelector->getSelectedItemIndex(),this);
    break;
  case FFuDialogButtons::MIDBUTTON:
    myApplyButtonClickedCB.invoke(myItemSelector->getSelectedItemIndex());
    myApplyButtonClickedWPtrCB.invoke(myItemSelector->getSelectedItemIndex(),this);
    break;
  case FFuDialogButtons::RIGHTBUTTON:
    myCancelButtonClickedCB.invoke(myItemSelector->getSelectedItemIndex());
    myCancelButtonClickedWPtrCB.invoke(myItemSelector->getSelectedItemIndex(),this);
    break;
  }
}
