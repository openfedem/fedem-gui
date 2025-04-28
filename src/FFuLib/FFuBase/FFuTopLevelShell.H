// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_TOP_LEVEL_SHELL_H
#define FFU_TOP_LEVEL_SHELL_H

#include "FFuLib/FFuBase/FFuMultUIComponent.H"
#include <vector>


/*!
  Base class for free windows (windows using borders, etc.).
  Main point of this class is to make them administer themselves,
  and to implement useful windows methods, such as blocking, etc.

  \author Dag R. Cristensen
  \date Jan 14 1999
*/

class FFuTopLevelShell : virtual public FFuMultUIComponent
{
  Fmd_HEADER_INIT();

public:
  FFuTopLevelShell();
  virtual ~FFuTopLevelShell();

  void manage(bool onScreen, bool inMem);

  // Window related methods.
  // Must be reimplemented in UILib dependent subclass :

  virtual void refresh() {}
  virtual void iconify() {}
  virtual void deiconify() {}
  virtual void setTitle(const char*) = 0;

  // SKE: I suspect there is a bug or misunderstood functionality
  // in the QWidget::show/raise methods.
  // They do NOT raise windows that are minimized.
  // The raise method just puts it first in the "queue", giving it focus.
  // QWidget::showNormal will make the window visible,
  // to remedy this for those TLS that need it (e.g. output list).
  virtual void showTLSNormal() {}

  // Virtual mode handling methods to be reimplemented on each spesific
  // TopLevelShell, to make it handle mode and state change on its own.
  // Called when the application changes mode or state, or cancels current mode.

  virtual void updateMode() {}
  virtual void updateState(int) {}
  virtual void cancel() {}

  // Static instance interface

  static FFuTopLevelShell* getInstanceByType(int classType);

protected:

  // Called on windowmanager closeWindow
  // By default it just pops down the window
  // Reimplement to do something more intelligent

  virtual bool onClose();

private:
  static std::vector<FFuTopLevelShell*> ourTopLevelDialogs;
};

#endif
