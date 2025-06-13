// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuBase/FFuModalDialog.H"
#include "FFuLib/FFuDialogButtons.H"


Fmd_SOURCE_INIT(FUI_MODALDIALOG, FFuModalDialog, FFuTopLevelShell);


FFuModalDialog::FFuModalDialog()
{
  Fmd_CONSTRUCTOR_INIT(FFuModalDialog);
}


bool FFuModalDialog::onClose()
{
  this->onDialogButtonClicked(FFuDialogButtons::RIGHTBUTTON);
  return false;
}
