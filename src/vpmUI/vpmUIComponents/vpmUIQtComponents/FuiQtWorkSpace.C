// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtWorkSpace.H"
#include "FFuLib/FFuBase/FFuMDIWindow.H"
#include <QMdiSubWindow>

//----------------------------------------------------------------------------

FuiQtWorkSpace::FuiQtWorkSpace(QWidget* parent, const char*) : QMdiArea(parent)
{
  this->setWidget(this);

  QObject::connect(this,SIGNAL(subWindowActivated(QMdiSubWindow*)),
		   this,SLOT(fwdWindowActivated(QMdiSubWindow*)));
}
//----------------------------------------------------------------------------

void FuiQtWorkSpace::cascadeWins()
{
  this->cascadeSubWindows();
}
//----------------------------------------------------------------------------

void FuiQtWorkSpace::tileWins()
{
  this->tileSubWindows();
}
//----------------------------------------------------------------------------

void FuiQtWorkSpace::tabWins()
{
  this->setViewMode(QMdiArea::ViewMode::TabbedView);
  this->setTabsClosable(true);
  this->setTabsMovable(true);
}
//----------------------------------------------------------------------------

void FuiQtWorkSpace::subWins()
{
  this->setViewMode(QMdiArea::ViewMode::SubWindowView);
}
//----------------------------------------------------------------------------

FFuMDIWindow* FuiQtWorkSpace::getActiveWindow()
{
  QMdiSubWindow* active = this->activeSubWindow();
  return active ? dynamic_cast<FFuMDIWindow*>(active->widget()) : NULL;
}
//----------------------------------------------------------------------------
