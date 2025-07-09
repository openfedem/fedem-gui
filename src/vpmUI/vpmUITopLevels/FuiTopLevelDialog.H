// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_TOP_LEVEL_DIALOG_H
#define FUI_TOP_LEVEL_DIALOG_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"

class FFuDialogButtons;


class FuiTopLevelDialog : virtual public FFuTopLevelShell,
                          public FFuUAExistenceHandler,
			  public FFuUADataHandler,
                          public FFuUAFinishHandler
{
public:
  FuiTopLevelDialog() { dialogButtons = NULL; }

  virtual void setSensitivity(bool sens);

protected:
  virtual void initWidgets();
  virtual void onPoppedUp();
  virtual bool onClose();

private:
  void onDialogButtonClicked(int);

protected:
  FFuDialogButtons* dialogButtons;
};

#endif
