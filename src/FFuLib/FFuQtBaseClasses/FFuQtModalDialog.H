// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_MODAL_DIALOG_H
#define FFU_QT_MODAL_DIALOG_H

#include <QDialog>

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "FFuLib/FFuBase/FFuModalDialog.H"


class FFuQtModalDialog : public QDialog, public FFuQtComponentBase,
                         virtual public FFuModalDialog
{
public:
  FFuQtModalDialog(int xpos, int ypos, int width, int height,
                   const char* title, const char* name);

  virtual void setTitle(const char* title);

  virtual bool execute();
  virtual void closeDialog(bool resultCode);

private:
  // Reimplemented from QWidget to handle various events
  virtual void closeEvent(QCloseEvent*);
  virtual bool event(QEvent*);
};

#endif
