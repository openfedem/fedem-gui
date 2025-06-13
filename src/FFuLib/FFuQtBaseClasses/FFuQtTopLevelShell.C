// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QCloseEvent>

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"


FFuQtTopLevelShell::FFuQtTopLevelShell(QWidget* parent,
				       int xpos, int ypos,
				       int width, int height,
				       const char* title,
				       const char* name,
				       Qt::WindowFlags f)
  : FFuQtWidget(parent,name)
{
  this->setGeometry(xpos,ypos,width,height);
  this->setWindowTitle(title);
  this->setWindowFlags(f);
}


void FFuQtTopLevelShell::popUp()
{
  this->show();
  this->raise();
}


void FFuQtTopLevelShell::showTLSNormal()
{
  this->showNormal();
  this->setFocus();
}


void FFuQtTopLevelShell::setTitle(const char* title)
{
  this->setWindowTitle(title);
}


void FFuQtTopLevelShell::setVisible(bool visible)
{
  bool wasvisible = this->isVisible();
  this->QWidget::setVisible(visible);

  if (visible != wasvisible)
    appearanceCB.invoke(visible);
}


void FFuQtTopLevelShell::showEvent(QShowEvent* e)
{
  this->QWidget::showEvent(e);
  this->onPoppedUp();
}


void FFuQtTopLevelShell::hideEvent(QHideEvent* e)
{
  this->QWidget::hideEvent(e);
  this->onPoppedDown();
}


void FFuQtTopLevelShell::closeEvent(QCloseEvent* e)
{
  if (this->onClose())
    e->accept();
  else
    e->ignore();
}
