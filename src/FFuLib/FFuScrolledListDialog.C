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

extern const char* info_xpm[];


void FFuScrolledListDialog::initWidgets()
{
  myDialogButtons->setButtonLabel(FFuDialogButtons::LEFTBUTTON, "OK");
  myDialogButtons->setButtonLabel(FFuDialogButtons::MIDBUTTON, "Apply");
  myDialogButtons->setButtonLabel(FFuDialogButtons::RIGHTBUTTON, "Cancel");

  labNotesImage->setPixMap(info_xpm);
  labNotesLabel->setLabel("<b>Notes</b>");
  labNotesImage->popDown();
  labNotesLabel->popDown();
  labNotesText->popDown();

  myDialogButtons->setButtonClickedCB(FFaDynCB1M(FFuScrolledListDialog,this,onDlgButtonClicked,int));
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
