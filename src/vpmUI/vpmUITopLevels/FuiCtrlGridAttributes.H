// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_CTRL_GRID_ATTRIBUTES_H
#define FUI_CTRL_GRID_ATTRIBUTES_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"

class FFuIOField;
class FFuToggleButton;
class FFuPushButton;


class FuiCtrlGridAttributes : virtual public FFuTopLevelShell,
			      public FFuUAExistenceHandler,
			      public FFuUADataHandler
{
public:
  virtual void setUIValues(const FFuaUIValues* values);

protected:
  void initWidgets();

  virtual void onPoppedUp() { this->updateUIValues(); }

private:
  void onDoubleChanged(double);
  void onToggeled(bool) { this->onDoubleChanged(0.0); }

protected:
  FFuToggleButton* myGridToggleButton;
  FFuIOField*      myGXField;
  FFuIOField*      myGYField;

  FFuToggleButton* mySnapToggleButton;
  FFuIOField*      mySXField;
  FFuIOField*      mySYField;

  FFuPushButton*   myCloseButton;
};


class FuaCtrlGridValues : public FFuaUIValues
{
public:
  bool   gridOn = false;
  double gridXSpacing = 0.0;
  double gridYSpacing = 0.0;
  bool   snapOn = false;
  double snapXSpacing = 0.0;
  double snapYSpacing = 0.0;
};

#endif
