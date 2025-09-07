// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_TABBED_WIDGET_STACK_H
#define FFU_TABBED_WIDGET_STACK_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <string>


/*!
  Class that provides a tab bar with a stack of widgets.

  \note There is a difference between pos idx and id.
  Pos idx is related to position, while id is just a number to
  keep track of the tabs.
*/

class FFuTabbedWidgetStack : virtual public FFuComponentBase
{
public:
  // If widget is not a child of the WidgetStack, it is reparented
  // (to a child of the WidgetStack)
  virtual bool addTabPage(FFuComponentBase* widget,
                          const std::string& label,
                          const char** icon = NULL,
                          int idx = -1) = 0;
  virtual bool renameTabPage(FFuComponentBase* widget,
                             const std::string& label) = 0;
  virtual bool removeTabPage(FFuComponentBase* widget) = 0;

  virtual void setCurrentTab(int tab) = 0;
  virtual void setCurrentTab(FFuComponentBase* widget) = 0;
  virtual void setCurrentTab(const std::string& name) = 0;

  virtual void setTabSensitivity(int tab, bool sensitive) = 0;
  virtual void setTabSensitivity(FFuComponentBase* widget, bool sensitive) = 0;

  virtual FFuComponentBase* getCurrentTabWidget() const = 0;
  virtual std::string getCurrentTabName() const = 0;

  // Gets the position index of the current tab
  virtual int getCurrentTabPosIdx() const = 0;

  void setTabSelectedCB(const FFaDynCB1<int>& cb) { tabSelectedIdCB = cb; }
  void setTabSelectedCB(const FFaDynCB1<FFuComponentBase*>& cb) { tabSelectedWidgetCB = cb; }

protected:
  FFaDynCB1<int> tabSelectedIdCB;
  FFaDynCB1<FFuComponentBase*> tabSelectedWidgetCB;
};

#endif
