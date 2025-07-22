// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QGridLayout>
#include <QVBoxLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtSpringDefCalc.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtRadioButton.H"


FuiQtSpringDefCalc::FuiQtSpringDefCalc(QWidget* parent, const char* name)
  : FFuQtLabelFrame(parent)
{
  this->setObjectName(name);

  myVariableLabel     = new FFuQtLabel();
  myVariableField     = new FFuQtIOField();

  myILengthField      = new FFuQtIOField();
  myIDeflField        = new FFuQtIOField();

  myILToggle          = new FFuQtRadioButton();
  myIDToggle          = new FFuQtRadioButton();

  myLengthChangeLabel = new FFuQtLabel();
  myLengthEngineField = new FuiQtQueryInputField(NULL);

  this->initWidgets();

  QWidget* qFields = new QWidget();
  QGridLayout* qgl = new QGridLayout(qFields);
  qgl->setContentsMargins(0,0,0,0);
  qgl->addWidget(static_cast<FFuQtLabel*>(myVariableLabel),   0,0);
  qgl->addWidget(static_cast<FFuQtIOField*>(myVariableField), 0,1);
  qgl->addWidget(dynamic_cast<FFuQtRadioButton*>(myILToggle), 1,0);
  qgl->addWidget(static_cast<FFuQtIOField*>(myILengthField),  1,1);
  qgl->addWidget(dynamic_cast<FFuQtRadioButton*>(myIDToggle), 2,0);
  qgl->addWidget(static_cast<FFuQtIOField*>(myIDeflField),    2,1);

  QBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(5,5,5,5);
  layout->addWidget(qFields);
  layout->addStretch(1);
  layout->addWidget(static_cast<FFuQtLabel*>(myLengthChangeLabel));
  layout->addWidget(static_cast<FuiQtQueryInputField*>(myLengthEngineField));
  layout->addStretch(2);
}
