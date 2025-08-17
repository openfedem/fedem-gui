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

#include "FFuLib/FFuQtComponents/FFuQtSpinBox.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"

#include "FuiQtGraphDefine.H"


FuiQtGraphDefine::FuiQtGraphDefine(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  titleField = new FFuQtLabelField();
  subtitleField = new FFuQtLabelField();

  xAxisField = new FFuQtLabelField();
  yAxisField = new FFuQtLabelField();

  for (FFuSpinBox*& box : fontSizeBox)
    box = new FFuQtSpinBox();

  gridMenu = new FFuQtOptionMenu();
  legendButton = new FFuQtToggleButton();
  autoscaleButton = new FFuQtToggleButton();

  tminField = new FFuQtLabelField();
  tmaxField = new FFuQtLabelField();
  timeButton = new FFuQtToggleButton();

  this->initWidgets();

  QWidget* qGridSize = new QWidget();
  QBoxLayout* aLayout = new QHBoxLayout(qGridSize);
  aLayout->setContentsMargins(0,0,0,0);
  aLayout->addWidget(new QLabel("Grid:"));
  aLayout->addWidget(gridMenu->getQtWidget());

  std::array<QWidget*,3> qFontSize;
  for (int i = 0; i < 3; i++)
  {
    qFontSize[i] = new QWidget();
    aLayout = new QHBoxLayout(qFontSize[i]);
    aLayout->setContentsMargins(0,0,0,0);
    aLayout->addWidget(new QLabel("Font size:"));
    aLayout->addWidget(fontSizeBox[i]->getQtWidget());
  }

  QWidget* qUpper = new QWidget();
  QGridLayout* layout = new QGridLayout(qUpper);
  layout->setContentsMargins(0,0,0,0);
  layout->setHorizontalSpacing(20);
  layout->setVerticalSpacing(2);
  layout->setColumnStretch(0,1);
  layout->addWidget(titleField->getQtWidget(), 0,0,1,2);
  layout->addWidget(subtitleField->getQtWidget(), 1,0,1,2);
  layout->addWidget(xAxisField->getQtWidget(), 2,0);
  layout->addWidget(yAxisField->getQtWidget(), 3,0);
  layout->addWidget(qFontSize[0], 0,2);
  layout->addWidget(qGridSize, 1,2);
  layout->addWidget(qFontSize[1], 2,1);
  layout->addWidget(qFontSize[2], 3,1);
  layout->addWidget(legendButton->getQtWidget(), 2,2);
  layout->addWidget(autoscaleButton->getQtWidget(), 3,2);

  QWidget* qLower = new QWidget();
  aLayout = new QHBoxLayout(qLower);
  aLayout->setContentsMargins(0,0,0,0);
  aLayout->setSpacing(20);
  aLayout->addWidget(tminField->getQtWidget(),1);
  aLayout->addWidget(tmaxField->getQtWidget(),1);
  aLayout->addWidget(timeButton->getQtWidget());

  aLayout = new QVBoxLayout(this);
  aLayout->addWidget(qUpper);
  aLayout->addWidget(qLower);
}
