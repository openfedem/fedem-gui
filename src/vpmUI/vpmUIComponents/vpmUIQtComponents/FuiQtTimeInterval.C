// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"

#include "FuiQtTimeInterval.H"


FuiQtTimeInterval::FuiQtTimeInterval(QWidget* parent, const char* name)
  : FFuQtLabelFrame(parent)
{
  this->setObjectName(name);
  this->setLabel("Time Interval");

  startField = new FFuQtIOField();
  stopField  = new FFuQtIOField();
  incrField  = new FFuQtIOField();
  
  FFuQtToggleButton* qToggle = new FFuQtToggleButton();
  FFuQtPushButton*  qPushBtn = new FFuQtPushButton();
  allStepsToggle = qToggle;
  resetButton    = qPushBtn;

  this->initWidgets();

  QWidget* qInterval = new QWidget();
  QGridLayout* grLay = new QGridLayout(qInterval);
  grLay->setContentsMargins(0,0,0,0);
  grLay->addWidget(new QLabel("Start")    ,0,0);
  grLay->addWidget(new QLabel("Stop")     ,1,0);
  grLay->addWidget(new QLabel("Increment"),2,0);
  grLay->addWidget(static_cast<FFuQtIOField*>(startField),0,1);
  grLay->addWidget(static_cast<FFuQtIOField*>(stopField) ,1,1);
  grLay->addWidget(static_cast<FFuQtIOField*>(incrField) ,2,1);
  grLay->addWidget(qToggle,3,1);

  QWidget*    qReset = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qReset);
  layout->setContentsMargins(0,0,0,0);
  layout->addStretch(1);
  layout->addWidget(qPushBtn);

  layout = new QVBoxLayout(this);
  layout->setSpacing(0);
  layout->addWidget(qInterval);
  layout->addWidget(qReset);
}
