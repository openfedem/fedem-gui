// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFrame>

#include "FFuLib/FFuQtComponents/FFuQtScale.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtColorSelector.H"


FFuQtColorSelector::FFuQtColorSelector(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  FFuQtScale* qtRedScale;
  FFuQtScale* qtBlueScale;
  FFuQtScale* qtGreenScale;

  FFuQtIOField* qtRedValue;
  FFuQtIOField* qtBlueValue;
  FFuQtIOField* qtGreenValue;

  myRedScale   = qtRedScale   = new FFuQtScale();
  myGreenScale = qtGreenScale = new FFuQtScale();
  myBlueScale  = qtBlueScale  = new FFuQtScale();
  myRedValue   = qtRedValue   = new FFuQtIOField();
  myGreenValue = qtGreenValue = new FFuQtIOField();
  myBlueValue  = qtBlueValue  = new FFuQtIOField();

  qtRedScale->setOrientation(Qt::Horizontal);
  qtBlueScale->setOrientation(Qt::Horizontal);
  qtGreenScale->setOrientation(Qt::Horizontal);

  this->init();

  mySelector = new QWidget();
  QGridLayout* layout = new QGridLayout(mySelector);
  layout->setContentsMargins(0,0,0,0);
  layout->setColumnStretch(1,3);
  layout->setColumnStretch(2,1);
  layout->addWidget(new QLabel("Red")  ,0,0);
  layout->addWidget(new QLabel("Green"),1,0);
  layout->addWidget(new QLabel("Blue"), 2,0);
  layout->addWidget(qtRedScale,  0,1);
  layout->addWidget(qtGreenScale,1,1);
  layout->addWidget(qtBlueScale, 2,1);
  layout->addWidget(qtRedValue,  0,2);
  layout->addWidget(qtGreenValue,1,2);
  layout->addWidget(qtBlueValue, 2,2);

  mySampleFrame = new QFrame();
  mySampleFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  mySampleFrame->setLineWidth(2);

  QBoxLayout* layout2 = new QHBoxLayout(this);
  layout2->setContentsMargins(0,0,0,0);
  layout2->addWidget(mySelector);
  layout2->addWidget(mySampleFrame);

  this->updateSampleFrame();
}


void FFuQtColorSelector::updateSampleFrame()
{
  char background[64];
  sprintf(background, "background-color: rgb(%d,%d,%d)", myColor[0], myColor[1], myColor[2]);
  mySampleFrame->setStyleSheet(background);
}


void FFuQtColorSelector::resizeEvent(QResizeEvent* e)
{
  this->QWidget::resizeEvent(e);

  int h = this->height();
  int w = this->width();
  mySelector->resize(w-h-10,h);
  mySampleFrame->setMinimumWidth(h);
  mySampleFrame->setGeometry(w-h,0,h,h);
}
