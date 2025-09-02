// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtScaleShiftWidget.H"


FuiQtScaleShiftWidget::FuiQtScaleShiftWidget(QWidget* parent, char axis)
  : FFuQtLabelFrame(parent)
{
  std::string label1(1,axis);
  std::string label2(axis == 'X' ? "Horizontal" : "Vertical");
  label1.append("-axis Scale and Shift");
  label2.append(" shift after scale");

  this->setObjectName(label1.c_str());
  this->setLabel(label1.c_str());

  scaleField = new FFuQtIOField();
  shiftField = new FFuQtIOField();
  zeroOutBtn = new FFuQtToggleButton();

  this->initWidgets();

  QLayout* layout2 = new QHBoxLayout();
  layout2->setContentsMargins(0,0,0,0);
  layout2->addWidget(new QLabel("Scale"));
  layout2->addWidget(scaleField->getQtWidget());

  QLayout* layout3 = new QHBoxLayout();
  layout3->setContentsMargins(0,0,0,0);
  layout3->addWidget(new QLabel("Additional shift"));
  layout3->addWidget(shiftField->getQtWidget());

  QGroupBox* shiftFrame = new QGroupBox(label2.c_str());
  QBoxLayout* layout = new QVBoxLayout(shiftFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(zeroOutBtn->getQtWidget());
  layout->addLayout(layout3);

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(5,0,5,5);
  layout->addLayout(layout2);
  layout->addWidget(shiftFrame);
}
