// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_USER_DIALOG_H
#define FFU_USER_DIALOG_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"


class FFuUserDialog : virtual public FFuTopLevelShell
{
  Fmd_HEADER_INIT();

public:
  FFuUserDialog();

  // Static create method implemented in GUIlib-dependent code.
  // Used instead of the GUIlib-dependent constructor.
  static FFuUserDialog* create(const char*  msgText,
			       int          dialogType,
			       const char** buttonTexts,
			       unsigned int nButtons,
			       bool         isModal = true);

  // Local event loop to be called when a modal behaviour is needed.
  // Returns indices with respect to buttonTexts.
  // Implemented in GUIlib-dependent code.
  virtual int execute() = 0;
};

#endif
