// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_TOP_LEVEL_SHELL_H
#define FUI_QT_TOP_LEVEL_SHELL_H

#include "FFuLib/FFuQtBaseClasses/FFuQtMultUIComponent.H"
#include "FFuLib/FFuBase/FFuTopLevelShell.H"

class QCloseEvent;


class FFuQtTopLevelShell : public FFuQtMultUIComponent,
                           virtual public FFuTopLevelShell
{
public:
  FFuQtTopLevelShell(QWidget* parent,
		     int xpos = 100, int ypos  = 100,
		     int width= 100, int height= 100,
		     const char* title = "FFuQtTopLevelShellUI",
		     const char* name  = "FFuQtTopLevelShell",
		     Qt::WindowFlags f = Qt::Widget);

  // Qt Implementation of standard window actions :

  virtual void refresh  ();
  virtual void iconify  ();
  virtual void deiconify();
  virtual void setTitle (const char* title);
  // The popUp method is reimplemented here (from FFuComponentBase)
  // to make poPup also raise the window to top.
  // SKE it still doesn't get visible....
  virtual void popUp    ();

  // This one will show the TLS in "normal" mode.
  virtual void showTLSNormal();

private:
  // Reimplementing the Qt event handler on window manager close
  virtual void closeEvent(QCloseEvent* e);
};

#endif
