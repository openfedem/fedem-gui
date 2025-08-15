// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QGridLayout>

#include "FuiQtCurveAxisDefinition.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"


FuiQtCurveAxisDefinition::FuiQtCurveAxisDefinition(QWidget* parent,
                                                   const char* title,
                                                   const char* name)
  : FFuQtLabelFrame(parent)
{
  this->setTitle(title);
  this->setObjectName(name);

  resultLabel = new FFuQtLabel();
  editButton  = new FFuQtPushButton();
  operMenu    = new FFuQtOptionMenu();

  this->initWidgets();

  QGridLayout* layout = new QGridLayout(this);
  layout->setContentsMargins(5,0,5,2);
  layout->addWidget(resultLabel->getQtWidget(), 0,0,1,3);
  layout->addWidget(editButton->getQtWidget(), 1,0);
  layout->addWidget(operMenu->getQtWidget(), 1,2);
}


FuiQtCurveTimeRange::FuiQtCurveTimeRange(QWidget* parent, const char* name)
  : FFuQtLabelFrame(parent)
{
  this->setObjectName(name);

  spaceObj = new FFuQtLabel();
  minField = new FFuQtIOField();
  maxField = new FFuQtIOField();
  operMenu = new FFuQtOptionMenu();

  this->initWidgets();

  QBoxLayout* layout = new QHBoxLayout(this);
  layout->setContentsMargins(5,2,5,2);
  layout->addWidget(new QLabel("Spatial domain:"));
  layout->addWidget(spaceObj->getQtWidget(),1);
  layout->addWidget(new QLabel("Time (range)"));
  layout->addWidget(minField->getQtWidget());
  layout->addWidget(maxField->getQtWidget());
  layout->addWidget(operMenu->getQtWidget());
}
