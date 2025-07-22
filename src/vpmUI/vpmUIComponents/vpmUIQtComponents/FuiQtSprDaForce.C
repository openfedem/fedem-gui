// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtSprDaForce.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"


FuiQtSprDaForce::FuiQtSprDaForce(QWidget* parent, const char* name)
  : FFuQtLabelFrame(parent)
{
  this->setObjectName(name);

  myFunctionLabel = new FFuQtLabel();
  myFunctionField = new FuiQtQueryInputField(NULL);
  myScaleField    = new FuiQtQueryInputField(NULL);
  myDmpToggle     = new FFuQtToggleButton();

  this->initWidgets();

  myFunctionField->setMaxHeight(20);
  myScaleField->setMaxHeight(20);

  QWidget* qFunc = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qFunc);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(static_cast<FFuQtLabel*>(myFunctionLabel));
  layout->addWidget(static_cast<FuiQtQueryInputField*>(myFunctionField));

  QWidget* qScale = new QWidget();
  layout = new QHBoxLayout(qScale);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Scale"));
  layout->addWidget(static_cast<FuiQtQueryInputField*>(myScaleField));

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(5,5,5,5);
  layout->addWidget(qFunc);
  layout->addWidget(qScale);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myDmpToggle));
}
