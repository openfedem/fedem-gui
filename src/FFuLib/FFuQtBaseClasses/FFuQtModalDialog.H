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


class FFuQtModalDialog : public QDialog,
                         public FFuQtComponentBase,
                         virtual public FFuModalDialog
{
public:
  FFuQtModalDialog(int xpos = 100, int ypos  = 100,
		   int width= 100, int height= 100,
		   const char* title = "FFuQtModalDialogUI",
		   const char* name  = "FFuQtModalDialog",
		   Qt::WindowFlags f = Qt::Widget);

  virtual void setTitle(const char* title);

  virtual bool execute();
  virtual void closeDialog(bool resultCode);

private:
  // Reimplemented from QWidget to handle various events
  virtual void resizeEvent(QResizeEvent*);
  virtual void closeEvent(QCloseEvent*);
  virtual bool event(QEvent*);
};

#endif
