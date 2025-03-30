// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuDialogButtons.H"
#include "FFuLib/FFuPushButton.H"


//----------------------------------------------------------------------------

void FFuDialogButtons::initWidgets()
{
  buttons[0]->setActivateCB(FFaDynCB0M(FFuDialogButtons,this,onLeftButtonClicked));
  buttons[1]->setActivateCB(FFaDynCB0M(FFuDialogButtons,this,onMidButtonClicked));
  buttons[2]->setActivateCB(FFaDynCB0M(FFuDialogButtons,this,onRightButtonClicked));

  for (FFuPushButton* button : buttons)
    button->popDown();

  for (FFuComponentBase* space : spacers)
    if (space) space->popUp();
}
//----------------------------------------------------------------------------

void FFuDialogButtons::onLeftButtonClicked()
{
  buttonClickedCB.invoke(LEFTBUTTON);
}
//----------------------------------------------------------------------------

void FFuDialogButtons::onMidButtonClicked()
{
  buttonClickedCB.invoke(MIDBUTTON);
}
//----------------------------------------------------------------------------

void FFuDialogButtons::onRightButtonClicked()
{
  buttonClickedCB.invoke(RIGHTBUTTON);
}
//----------------------------------------------------------------------------

void FFuDialogButtons::setButtonLabel(int button, const char* label)
{
  if (label)
  {
    buttons[button]->setLabel(label);
    buttons[button]->popUp();
    if (spacers[button])
      spacers[button]->popDown();
  }
  else
  {
    buttons[button]->popDown();
    if (spacers[button])
      spacers[button]->popUp();
  }
}
//----------------------------------------------------------------------------

void FFuDialogButtons::setButtonSensitivity(int button, bool sensitive)
{
  buttons[button]->setSensitivity(sensitive);
}
//----------------------------------------------------------------------------
