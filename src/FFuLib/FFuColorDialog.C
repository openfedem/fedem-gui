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

  myDialogButtons->setButtonClickedCB(FFaDynCB1M(FFuColorDialog,this,onDlgButtonClicked, int));

  myColorSelector->setColorChangedCB(FFaDynCB1M(FFuColorDialog,this,onColorChanged,FFuColor));
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
      myOkButtonClickedWPtrCB.invoke(myColorSelector->getColor(),this);
      break;
    case FFuDialogButtons::RIGHTBUTTON:
      myCancelButtonClickedCB.invoke(myColorSelector->getInitialColor());
      myCancelButtonClickedWPtrCB.invoke(myColorSelector->getInitialColor(),this);
      break;
    default:
      break;
    }
}

void FFuColorDialog::onColorChanged(FFuColor aColor)
{
  myColorChangedCB.invoke(aColor);
  myColorChangedWPtrCB.invoke(aColor,this);
}

void FFuColorDialog::setColorChangedCB(const FFaDynCB1<FFuColor>& aDynCB)
{
  myColorChangedCB = aDynCB;
}

void FFuColorDialog::setColorChangedCB(const FFaDynCB2<FFuColor,FFuComponentBase*>& aDynCB)
{
  myColorChangedWPtrCB = aDynCB;
}

void FFuColorDialog::setOkButtonClickedCB(const FFaDynCB1<FFuColor>& aDynCB)
{
  myOkButtonClickedCB = aDynCB;
}

void FFuColorDialog::setOkButtonClickedCB(const FFaDynCB2<FFuColor,FFuComponentBase*>& aDynCB)
{
  myOkButtonClickedWPtrCB = aDynCB;
}

void FFuColorDialog::setCancelButtonClickedCB(const FFaDynCB1<FFuColor>& aDynCB)
{
  myCancelButtonClickedCB = aDynCB;
}

void FFuColorDialog::setCancelButtonClickedCB(const FFaDynCB2<FFuColor,FFuComponentBase*>& aDynCB)
{
  myCancelButtonClickedWPtrCB = aDynCB;
}
