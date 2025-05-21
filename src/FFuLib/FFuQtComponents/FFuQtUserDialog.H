// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_USER_DIALOG_H
#define FFU_QT_USER_DIALOG_H

#include <QMessageBox>

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "FFuLib/FFuUserDialog.H"


class FFuQtUserDialog : public QMessageBox, public FFuUserDialog,
                        public FFuQtComponentBase
{
public:
  FFuQtUserDialog(const char* msgText, int dialogType,
                  const char** buttonTexts, unsigned int nButton, bool isModal);

  virtual int execute();
};

#endif
