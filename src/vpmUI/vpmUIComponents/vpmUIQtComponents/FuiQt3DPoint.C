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


FuiQt3DPoint::FuiQt3DPoint(QWidget* parent, const char* name,
                           int refMenu, bool nodeField)
  : FFuQtWidget(parent,name)
{
  for (int i = 0; i < 3; i++)
    myFields[i] = new FFuQtIOField();
  if (nodeField)
    myFields[3] = new FFuQtIOField();
  if (refMenu)
    myRefMenu = new FFuQtOptionMenu();

  this->init();

  QWidget* qPoint = this;
  QBoxLayout* myLayout = NULL;
  QGridLayout* gl = new QGridLayout();
  if (refMenu || nodeField)
  {
    qPoint = new QWidget();
    myLayout = new QVBoxLayout(this);
    gl->setContentsMargins(0,0,0,0);
  }
  gl->setHorizontalSpacing(20);
  gl->setVerticalSpacing(2);
  qPoint->setLayout(gl);

  char label[2] = "X";
  for (int i = 0; i < 3; i++, label[0]++)
  {
    gl->addWidget(new QLabel(label),i,0);
    gl->addWidget(myFields[i]->getQtWidget(),i,1);
  }

  if (nodeField)
  {
    myNodeField = new QWidget();
    QLayout* layout = new QHBoxLayout(myNodeField);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(new QLabel("FE node"));
    layout->addWidget(myFields[3]->getQtWidget());
    if (refMenu == 1)
      myLayout->setContentsMargins(0,0,0,0);
    myLayout->addStretch(2);
    myLayout->addWidget(myNodeField);
  }

  if (refMenu)
  {
    QLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(new QLabel("Reference"));
    layout->addWidget(myRefMenu->getQtWidget());
    if (refMenu == 1)
      myLayout->setContentsMargins(0,0,0,0);
    myLayout->addStretch(3);
    myLayout->addLayout(layout);
    myLayout->addStretch(1);
  }

  if (myLayout)
    myLayout->addWidget(qPoint);
}


void FuiQt3DPoint::showNodeField(int ID)
{
  if (ID > 0)
    myNodeField->show();
  else if (ID < 0)
    myNodeField->hide();
}
