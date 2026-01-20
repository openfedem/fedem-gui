// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QCloseEvent>

#include "FFuLib/FFuAuxClasses/FFuaApplication.H"
#include "FFuQtModalDialog.H"


FFuQtModalDialog::FFuQtModalDialog(int xpos, int ypos, int width, int height,
                                   const char* title, const char* name)
  : QDialog(NULL,Qt::Dialog)
{
  this->setWidget(this);
  this->setObjectName(name);
  this->setModal(true);
  this->setWindowTitle(title);
  this->setGeometry(xpos,ypos,width,height);
}


void FFuQtModalDialog::setTitle(const char* title)
{
  this->setWindowTitle(title);
}


bool FFuQtModalDialog::execute()
{
  FFuaApplication::breakUserEventBlock(true);
  int ret = this->exec();
  FFuaApplication::breakUserEventBlock(false);

  return (ret == QDialog::Accepted);
}


void FFuQtModalDialog::closeDialog(bool resultCode)
{
  this->done(resultCode ? QDialog::Accepted : QDialog::Rejected);
}


void FFuQtModalDialog::showEvent(QShowEvent* e)
{
  this->QWidget::showEvent(e);
  this->onPoppedUp();
}


void FFuQtModalDialog::hideEvent(QHideEvent* e)
{
  this->QWidget::hideEvent(e);
  this->onPoppedDown();
}


void FFuQtModalDialog::closeEvent(QCloseEvent* e)
{
  if (this->onClose())
    e->accept();
  else
    e->ignore();
}
