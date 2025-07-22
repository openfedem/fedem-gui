// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QGridLayout>

#include "FuiQtMotionType.H"
#include "FFuLib/FFuQtComponents/FFuQtRadioButton.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"


FuiQtMotionType::FuiQtMotionType(QWidget* parent, int nButton, const char* name)
  : FFuQtLabelFrame(parent)
{
  this->setObjectName(name);
  this->setLabel("Constraint Type");

  for (int i = 0; i < nButton; i++)
    myMotionTypeButtons.push_back(new FFuQtRadioButton());

  myAddButton = new FFuQtToggleButton();

  this->initWidgets();

  QGridLayout* layout = new QGridLayout(this);
  layout->setContentsMargins(5,0,5,5);
  int row = 0;
  for (FFuRadioButton* button : myMotionTypeButtons)
    if (row == 0)
      layout->addWidget(dynamic_cast<FFuQtRadioButton*>(button), row++,0);
    else
      layout->addWidget(dynamic_cast<FFuQtRadioButton*>(button), row++,0,1,2);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myAddButton), 0,1);
}
