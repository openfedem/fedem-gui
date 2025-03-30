// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_DIALOG_BUTTONS_H
#define FFU_DIALOG_BUTTONS_H

#include <array>

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuPushButton;


class FFuDialogButtons : virtual public FFuComponentBase
{
public:
  FFuDialogButtons() { buttons.fill(NULL); spacers.fill(NULL); }

  enum Buttons {
    LEFTBUTTON  = 0,
    MIDBUTTON   = 1,
    RIGHTBUTTON = 2
  };

  void setButtonLabel(int button, const char* label);
  void setButtonSensitivity(int button, bool sensitive);
  void setButtonClickedCB(const FFaDynCB1<int>& cb) { buttonClickedCB = cb; }

protected:
  void initWidgets();

private:
  void onLeftButtonClicked();
  void onMidButtonClicked();
  void onRightButtonClicked();

protected:
  std::array<FFuPushButton*,3>    buttons;
  std::array<FFuComponentBase*,3> spacers;

private:
  FFaDynCB1<int> buttonClickedCB;
};

#endif
