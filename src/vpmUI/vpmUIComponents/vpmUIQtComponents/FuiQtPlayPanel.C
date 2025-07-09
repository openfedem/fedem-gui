// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtScale.H"

#include "FuiQtPlayPanel.H"


FuiQtPlayPanel::FuiQtPlayPanel(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  for (FFuPushButton*& button : myButtons)
    button = new FFuQtPushButton();

  for (FFuToggleButton*& toggle : myToggles)
    toggle = new FFuQtToggleButton();

  FFuQtScale* qtSpeedScale = new FFuQtScale();
  qtSpeedScale->setOrientation(Qt::Horizontal);
  qtSpeedScale->setMinimumSize(5,10);

  mySpeedScale = qtSpeedScale;
  myCloseButton = new FFuQtPushButton(this);

  this->init();

  QWidget* row1 = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(row1);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(myButtons[PLAY_REW]));
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(myButtons[PAUSE]));
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(myButtons[PLAY_FWD]));

  QWidget* row2 = new QWidget();
  layout = new QHBoxLayout(row2);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(myButtons[TO_FIRST]));
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(myButtons[STEP_REW]));
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(myButtons[STEP_FWD]));
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(myButtons[TO_LAST]));

  QWidget* row4 = new QWidget();
  layout = new QHBoxLayout(row4);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(20);
  layout->addWidget(new QLabel("Slow"),1,Qt::AlignLeft);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(myButtons[REALTIME]));
  layout->addWidget(new QLabel("Fast"),1,Qt::AlignRight);

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(5,5,5,5);
  layout->setSpacing(0);
  layout->addWidget(row1);
  layout->addWidget(row2);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(myButtons[STOP]));
  layout->addWidget(row4);
  layout->addWidget(qtSpeedScale);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[ONESHOT]));
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[CONTINOUS]));
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[CONTINOUS_CYCLE]));
}


void FuiQtPlayPanel::showEvent(QShowEvent* e)
{
  // Position the close button explicitly
  // on top of the VBox layout of the play panel
  int x = this->getWidth() - 50;
  int y = this->getHeight() - 25;
  myCloseButton->setSizeGeometry(x,y,45,20);
  myCloseButton->toFront();

  this->QWidget::showEvent(e);
  this->onPoppedUp();
}
