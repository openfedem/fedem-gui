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

#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtTable.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtJointSummary.H"


FuiQtJointSummary::FuiQtJointSummary(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  mySummaryTable      = FuiSummaryTable::createTable(NULL);

  myAddBCLabel        = new FFuQtLabel();
  myDefDamperLabel    = new FFuQtLabel();
  myDOF_TZ_Toggle     = new FFuQtToggleButton();
  mySetAllFreeButton  = new FFuQtPushButton();
  mySetAllFixedButton = new FFuQtPushButton();
  mySwapJointButton   = new FFuQtPushButton();
  myFriction          = new FuiQtQueryInputField(NULL);
  myFrictionDof       = new FFuQtOptionMenu();
  myScrewToggle       = new FFuQtToggleButton();
  myScrewRatioField   = new FFuQtIOField();
  myCamThicknessField = new FFuQtIOField();
  myCamWidthField     = new FFuQtIOField();
  myRadialToggle      = new FFuQtToggleButton();

  myFrictionFrame = new QGroupBox("Friction");
  qFrictionDof = new QWidget();
  myScrewFrame = new QGroupBox("Screw connection");
  myCamFrame = new QGroupBox("Cam geometry");

  this->initWidgets();

  QBoxLayout* layout = new QHBoxLayout(qFrictionDof);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Joint DOF"));
  layout->addWidget(static_cast<FFuQtOptionMenu*>(myFrictionDof));
  layout->addSpacing(20);

  layout = new QVBoxLayout(myFrictionFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(static_cast<FuiQtQueryInputField*>(myFriction));
  layout->addWidget(qFrictionDof);

  layout = new QHBoxLayout(myScrewFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myScrewToggle));
  layout->addWidget(static_cast<FFuQtIOField*>(myScrewRatioField));

  QGridLayout* gl = new QGridLayout(myCamFrame);
  layout->setContentsMargins(5,0,5,5);
  gl->addWidget(new QLabel("Thickness"), 0,0);
  gl->addWidget(new QLabel("Width"), 1,0);
  gl->addWidget(static_cast<FFuQtIOField*>(myCamThicknessField), 0,1);
  gl->addWidget(static_cast<FFuQtIOField*>(myCamWidthField), 1,1);
  gl->addWidget(dynamic_cast<FFuQtToggleButton*>(myRadialToggle), 2,1);

  QWidget* qLeft = new QWidget();
  layout = new QVBoxLayout(qLeft);
  layout->setContentsMargins(5,5,5,5);
  layout->addWidget(myFrictionFrame);
  layout->addWidget(myScrewFrame);

  QWidget* qButtons = new QWidget();
  layout = new QHBoxLayout(qButtons);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(mySetAllFreeButton));
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(mySetAllFixedButton));
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(mySwapJointButton));

  QWidget* qRight = new QWidget();
  layout = new QVBoxLayout(qRight);
  layout->setContentsMargins(5,5,5,5);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myDOF_TZ_Toggle));
  layout->addWidget(qButtons);
  layout->addWidget(myCamFrame);
  layout->addWidget(static_cast<FFuQtLabel*>(myAddBCLabel));
  layout->addWidget(static_cast<FFuQtLabel*>(myDefDamperLabel));

  QWidget* qBoth = new QWidget();
  layout = new QHBoxLayout(qBoth);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(20);
  layout->addWidget(qLeft,1);
  layout->addWidget(qRight,1);

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(2,5,2,2);
  layout->addWidget(static_cast<FFuQtTable*>(mySummaryTable));
  layout->addWidget(qBoth);
}


void FuiQtJointSummary::showFrame(QGroupBox* frame, bool onOff)
{
  if (onOff)
    frame->show();
  else
    frame->hide();
}


void FuiQtJointSummary::showFriction(char onOff)
{
  showFrame(myFrictionFrame,onOff);
  if (onOff > 1)
    qFrictionDof->show();
  else if (onOff)
    qFrictionDof->hide();
}


void FuiQtJointSummary::resizeEvent(QResizeEvent* e)
{
  this->QWidget::resizeEvent(e);
  this->updateTableGeometry();
}
