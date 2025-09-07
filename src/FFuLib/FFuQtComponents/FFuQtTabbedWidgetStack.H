// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_TABBED_WIDGET_STACK_H
#define FFU_QT_TABBED_WIDGET_STACK_H

#include <QTabWidget>

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "FFuLib/FFuTabbedWidgetStack.H"


class FFuQtTabbedWidgetStack : public QTabWidget, public FFuTabbedWidgetStack,
                               public FFuQtComponentBase
{
  Q_OBJECT

public:
  FFuQtTabbedWidgetStack(QWidget* parent = NULL, bool docMode = false);

  virtual bool addTabPage(FFuComponentBase* widget,
                          const std::string& label, const char** icon, int idx);
  virtual bool renameTabPage(FFuComponentBase* widget,
                             const std::string& label);
  virtual bool removeTabPage(FFuComponentBase* widget);

  virtual void setCurrentTab(int tab);
  virtual void setCurrentTab(FFuComponentBase* widget);
  virtual void setCurrentTab(const std::string& name);

  virtual FFuComponentBase* getCurrentTabWidget() const;
  virtual int getCurrentTabPosIdx() const;
  virtual std::string getCurrentTabName() const;

  virtual void setTabSensitivity(int tab, bool sensitive);
  virtual void setTabSensitivity(FFuComponentBase* widget, bool sensitive);

private slots:
  virtual void onTabSelected(int tab);
};

#endif
