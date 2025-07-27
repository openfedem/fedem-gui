// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QGroupBox>
#include <QSpacerItem>
#include <QGridLayout>
#include <QVBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtSpringChar.H"


FuiQtSpringChar::FuiQtSpringChar(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  mySpringFunctionDescription = new FFuQtLabel();

  mySpringFunctionField      = new FuiQtQueryInputField(NULL);
  myYieldForceMaxEngineField = new FuiQtQueryInputField(NULL);
  myYieldForceMinEngineField = new FuiQtQueryInputField(NULL);

  for (FFuToggleButton*& button : myToggles)
    button = new FFuQtToggleButton();

  for (int i = 0; i <= MIN_FORCE; i++)
    myFields[i] = new FFuQtIOField();
  myFields[MAX_YIELD_DEFL] = new FFuQtIOField();

  this->initWidgets();

  QGroupBox* mySpringFunctionFrame = new QGroupBox("Spring function");
  QBoxLayout* layout = new QVBoxLayout(mySpringFunctionFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(static_cast<FuiQtQueryInputField*>(mySpringFunctionField));
  layout->addWidget(static_cast<FFuQtLabel*>(mySpringFunctionDescription));

  QGroupBox* myFailureFrame = new QGroupBox("Failure criterions");
  QGridLayout* gl = new QGridLayout(myFailureFrame);
  gl->setContentsMargins(5,0,5,5);
  gl->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[MAX_DEFL]), 0,0);
  gl->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[MIN_DEFL]), 1,0);
  gl->addWidget(static_cast<FFuQtIOField*>(myFields[MAX_DEFL]), 0,1);
  gl->addWidget(static_cast<FFuQtIOField*>(myFields[MIN_DEFL]), 1,1);
  gl->addItem(new QSpacerItem(15,10), 0,2);
  gl->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[MAX_FORCE]), 0,3);
  gl->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[MIN_FORCE]), 1,3);
  gl->addWidget(static_cast<FFuQtIOField*>(myFields[MAX_FORCE]), 0,4);
  gl->addWidget(static_cast<FFuQtIOField*>(myFields[MIN_FORCE]), 1,4);

  QGroupBox* myYieldFrame = new QGroupBox("Yield criterions");
  gl = new QGridLayout(myYieldFrame);
  gl->setContentsMargins(5,0,5,5);
  gl->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[MAX_YIELD_FORCE]), 0,0);
  gl->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[MIN_YIELD_FORCE]), 1,0);
  gl->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[MAX_YIELD_DEFL]), 2,0);
  gl->addWidget(static_cast<FuiQtQueryInputField*>(myYieldForceMaxEngineField), 0,1);
  gl->addWidget(static_cast<FuiQtQueryInputField*>(myYieldForceMinEngineField), 1,1);
  gl->addWidget(static_cast<FFuQtIOField*>(myFields[MAX_YIELD_DEFL]), 2,1);

  layout = new QVBoxLayout(this);
  layout->addWidget(mySpringFunctionFrame);
  layout->addStretch(2);
  layout->addWidget(myFailureFrame);
  layout->addStretch(1);
  layout->addWidget(myYieldFrame);
}
