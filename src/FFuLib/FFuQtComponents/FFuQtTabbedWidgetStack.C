// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuQtComponents/FFuQtTabbedWidgetStack.H"

//----------------------------------------------------------------------------

FFuQtTabbedWidgetStack::FFuQtTabbedWidgetStack(QWidget* parent, bool docMode)
  : QTabWidget(parent)
{
  this->setWidget(this);
  this->setDocumentMode(docMode);

  connect(this, SIGNAL(currentChanged(int)), this, SLOT(onTabSelected(int)));
}
//----------------------------------------------------------------------------

void FFuQtTabbedWidgetStack::onTabSelected(int tab)
{
  if (!this->isPoppedUp()) return;

  this->tabSelectedIdCB.invoke(tab);
  this->tabSelectedWidgetCB.invoke(dynamic_cast<FFuComponentBase*>(this->widget(tab)));
}
//----------------------------------------------------------------------------

bool FFuQtTabbedWidgetStack::addTabPage(FFuComponentBase* widget,
                                        const std::string& label,
                                        const char** icon, int idx)
{
  QWidget* qwidget = widget->getQtWidget();
  if (!qwidget || this->indexOf(qwidget) >= 0)
    return false; // widget already exists

  if (icon)
    this->addTab(qwidget, QIcon(QPixmap(icon)), QString(label.c_str()));
  else if (idx < 0)
    this->addTab(qwidget, QString(label.c_str()));
  else
    this->insertTab(idx, qwidget, QString(label.c_str()));

  return true;
}
//----------------------------------------------------------------------------

bool FFuQtTabbedWidgetStack::renameTabPage(FFuComponentBase* widget,
                                           const std::string& label)
{
  int tab = this->indexOf(widget->getQtWidget());
  if (tab < 0) return false;

  this->setTabText(tab,QString(label.c_str()));
  return true;
}
//----------------------------------------------------------------------------

bool FFuQtTabbedWidgetStack::removeTabPage(FFuComponentBase* widget)
{
  int tab = this->indexOf(widget->getQtWidget());
  if (tab < 0) return false;

  this->removeTab(tab);
  return true;
}
//----------------------------------------------------------------------------

void FFuQtTabbedWidgetStack::setTabSensitivity(int tab, bool sensitive)
{
  this->setTabEnabled(tab,sensitive);
}

void FFuQtTabbedWidgetStack::setTabSensitivity(FFuComponentBase* widget,
                                               bool sensitive)
{
  int tab = this->indexOf(widget->getQtWidget());
  if (tab > -1) this->setTabEnabled(tab,sensitive);
}
//----------------------------------------------------------------------------

void FFuQtTabbedWidgetStack::setCurrentTab(int tab)
{
  this->setCurrentIndex(tab);
}

void FFuQtTabbedWidgetStack::setCurrentTab(FFuComponentBase* widget)
{
  this->setCurrentWidget(widget->getQtWidget());
}

void FFuQtTabbedWidgetStack::setCurrentTab(const std::string& name)
{
  // Loop over tabs and set current by pos index
  for (int idx = 0; idx < this->count(); idx++)
    if (this->tabText(idx).toStdString() == name) {
      this->setCurrentIndex(idx);
      return;
    }

  // If we get here, select first
  this->setCurrentIndex(0);
}
//----------------------------------------------------------------------------

std::string FFuQtTabbedWidgetStack::getCurrentTabName() const
{
  return this->tabText(this->currentIndex()).toStdString();
}

int FFuQtTabbedWidgetStack::getCurrentTabPosIdx() const
{
  return this->currentIndex();
}

FFuComponentBase* FFuQtTabbedWidgetStack::getCurrentTabWidget() const
{
  return dynamic_cast<FFuComponentBase*>(this->currentWidget());
}
//----------------------------------------------------------------------------
