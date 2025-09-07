// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtResultTabs.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"


FuiQtTriadResults::FuiQtTriadResults(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  for (FFuToggleButton*& toggle : myToggles)
    toggle = new FFuQtToggleButton(this);

  this->initWidgets();

  QGroupBox* myGlobalFrame = new QGroupBox("Global");
  QBoxLayout* layout = new QVBoxLayout(myGlobalFrame);
  layout->setContentsMargins(10,5,10,5);
  for (int i = 0; i < 3; i++)
    layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[i]));
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[6]));
  layout->addStretch(1);

  QGroupBox* myLocalFrame = new QGroupBox("Local");
  layout = new QVBoxLayout(myLocalFrame);
  layout->setContentsMargins(10,5,10,5);
  for (int i = 3; i < 6; i++)
    layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[i]));
  layout->addStretch(1);

  layout = new QHBoxLayout(this);
  layout->setSpacing(20);
  layout->addWidget(myGlobalFrame);
  layout->addWidget(myLocalFrame);
  layout->addStretch(1);
}


FuiQtJointResults::FuiQtJointResults(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  for (FFuToggleButton*& toggle : myToggles)
    toggle = new FFuQtToggleButton(this);

  this->initWidgets();

  QGroupBox* myJointFrame = new QGroupBox("Joint variables");
  QGridLayout* gLayout = new QGridLayout(myJointFrame);
  gLayout->setContentsMargins(10,5,10,5);
  for (size_t i = 0; i < 5; i++)
    gLayout->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[i]), i%3,i/3);
  gLayout->setRowStretch(3,1);

  mySpringFrame = new QGroupBox("Spring variables");
  QBoxLayout* layout = new QVBoxLayout(mySpringFrame);
  layout->setContentsMargins(10,5,10,5);
  for (size_t i = 5; i < 10; i++)
    layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[i]));
  layout->addStretch(1);

  myDamperFrame = new QGroupBox("Damper variables");
  layout = new QVBoxLayout(myDamperFrame);
  layout->setContentsMargins(10,5,10,5);
  for (size_t i = 10; i < myToggles.size(); i++)
    layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myToggles[i]));
  layout->addStretch(1);

  layout = new QHBoxLayout(this);
  layout->setSpacing(20);
  layout->addWidget(myJointFrame);
  layout->addWidget(mySpringFrame);
  layout->addWidget(myDamperFrame);
  layout->addStretch(1);
}


void FuiQtJointResults::setSpringDamper(bool onOff)
{
  if (onOff)
  {
    mySpringFrame->show();
    myDamperFrame->show();
  }
  else
  {
    mySpringFrame->hide();
    myDamperFrame->hide();
  }
}
