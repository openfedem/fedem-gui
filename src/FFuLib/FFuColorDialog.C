// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuColorDialog.H"
#include "FFuLib/FFuColorSelector.H"
#include "FFuLib/FFuDialogButtons.H"


/*!
  \class FFuColorDialog FFuColorDialog.H
  \brief Simple color selectoion dialog

  The dialog is based on the FFoColorSelector object, and is
  implemented as a top-level-shell to help stand-alone use of the dialog.

  \author Jens Lien
*/

//! Widget initialization
void FFuColorDialog::init()
{
  myDialogButtons->setButtonLabel(FFuDialogButtons::LEFTBUTTON, "OK");
  myDialogButtons->setButtonLabel(FFuDialogButtons::RIGHTBUTTON, "Cancel");

  myDialogButtons->setButtonClickedCB(FFaDynCB1M(FFuColorDialog,this,onDlgButtonClicked,int));

  myColorSelector->setColorChangedCB(FFaDynCB1M(FFuColorDialog,this,onColorChanged,const FFuColor&));
}


//! Returns the current normalized RGB colors
const FFuColor& FFuColorDialog::getColor() const
{
  return myColorSelector->getColor();
}


/*!
  Set the initial color
  \sa FFuColorSelector::setInitialColor
*/
void FFuColorDialog::setInitialColor(const FFuColor& aColor)
{
  myColorSelector->setInitialColor(aColor);
}

/*!
  Set the current color
  \sa FFuColorSelector::setColor
*/
void FFuColorDialog::setColor(const FFuColor& aColor)
{
  myColorSelector->setColor(aColor);
}


void FFuColorDialog::onDlgButtonClicked(int val)
{
  switch (val)
    {
    case FFuDialogButtons::LEFTBUTTON:
      myOkButtonClickedCB.invoke(myColorSelector->getColor());
      myOkButtonClickedCB2.invoke(myColorSelector->getColor(),this);
      break;
    case FFuDialogButtons::RIGHTBUTTON:
      myCancelButtonClickedCB.invoke(myColorSelector->getInitialColor());
      myCancelButtonClickedCB2.invoke(myColorSelector->getInitialColor(),this);
      break;
    default:
      break;
    }
}

void FFuColorDialog::onColorChanged(const FFuColor& aColor)
{
  myColorChangedCB.invoke(aColor);
  myColorChangedCB2.invoke(aColor,this);
}

void FFuColorDialog::setColorChangedCB(const DynCB1& aDynCB)
{
  myColorChangedCB = aDynCB;
}

void FFuColorDialog::setColorChangedCB(const DynCB2& aDynCB)
{
  myColorChangedCB2 = aDynCB;
}

void FFuColorDialog::setOkButtonClickedCB(const DynCB1& aDynCB)
{
  myOkButtonClickedCB = aDynCB;
}

void FFuColorDialog::setOkButtonClickedCB(const DynCB2& aDynCB)
{
  myOkButtonClickedCB2 = aDynCB;
}

void FFuColorDialog::setCancelButtonClickedCB(const DynCB1& aDynCB)
{
  myCancelButtonClickedCB = aDynCB;
}

void FFuColorDialog::setCancelButtonClickedCB(const DynCB2& aDynCB)
{
  myCancelButtonClickedCB2 = aDynCB;
}
