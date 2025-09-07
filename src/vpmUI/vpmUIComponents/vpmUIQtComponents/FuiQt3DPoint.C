// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"

#include "FuiQt3DPoint.H"


FuiQt3DPoint::FuiQt3DPoint(QWidget* parent, const char* name, bool refMenu)
  : FFuQtWidget(parent,name)
{
  myXField = new FFuQtIOField();
  myYField = new FFuQtIOField();
  myZField = new FFuQtIOField();
  myRefMenu = refMenu ? new FFuQtOptionMenu() : NULL;

  this->init();

  QWidget* qPoint = refMenu ? new QWidget() : this;
  QGridLayout* gl = new QGridLayout(qPoint);
  if (refMenu) gl->setContentsMargins(0,0,0,0);
  gl->setHorizontalSpacing(20);
  gl->setVerticalSpacing(2);
  gl->addWidget(new QLabel("X"),0,0);
  gl->addWidget(new QLabel("Y"),1,0);
  gl->addWidget(new QLabel("Z"),2,0);
  gl->addWidget(static_cast<FFuQtIOField*>(myXField),0,1);
  gl->addWidget(static_cast<FFuQtIOField*>(myYField),1,1);
  gl->addWidget(static_cast<FFuQtIOField*>(myZField),2,1);

  if (refMenu)
  {
    QWidget* qRef = new QWidget();
    QBoxLayout* layout = new QHBoxLayout(qRef);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(new QLabel("Reference"));
    layout->addWidget(static_cast<FFuQtOptionMenu*>(myRefMenu));
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(qRef);
    layout->addWidget(qPoint);
  }
}
