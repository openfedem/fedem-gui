// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtTable.H"

#include "FuiQtTriadSummary.H"


FuiQtTriadSummary::FuiQtTriadSummary(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  myFENodeField = new FFuQtLabelField();
  mySetAllFreeButton = new FFuQtPushButton();
  mySetAllFixedButton = new FFuQtPushButton();

  myMassField = new FFuQtLabelField();
  myIxField = new FFuQtLabelField();
  myIyField = new FFuQtLabelField();
  myIzField = new FFuQtLabelField();

  myTriadLabel = new FFuQtLabel();
  mySysDirMenu = new FFuQtOptionMenu();
  myConnectorMenu = new FFuQtOptionMenu();
  myAddBCLabel = new FFuQtLabel();

  mySummaryTable = FuiSummaryTable::createTable(NULL);

  this->initWidgets();

  myMassFrame = new QGroupBox("Additional masses");
  QBoxLayout* layout = new QVBoxLayout(myMassFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(static_cast<FFuQtLabelField*>(myMassField));
  layout->addWidget(static_cast<FFuQtLabelField*>(myIxField));
  layout->addWidget(static_cast<FFuQtLabelField*>(myIyField));
  layout->addWidget(static_cast<FFuQtLabelField*>(myIzField));

  mySysDirFrame = new QGroupBox("System directions");
  layout = new QVBoxLayout(mySysDirFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(static_cast<FFuQtOptionMenu*>(mySysDirMenu));

  myConnectorFrame = new QGroupBox("Surface connector");
  layout = new QVBoxLayout(myConnectorFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(static_cast<FFuQtOptionMenu*>(myConnectorMenu));

  QWidget* left = new QWidget();
  layout = new QVBoxLayout(left);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(static_cast<FFuQtLabelField*>(myFENodeField));
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(mySetAllFreeButton));
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(mySetAllFixedButton));
  layout->addStretch(1);
  layout->addWidget(static_cast<FFuQtLabel*>(myTriadLabel));

  QWidget* middle = new QWidget();
  layout = new QVBoxLayout(middle);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(myMassFrame);
  layout->addWidget(mySysDirFrame);
  layout->addWidget(myConnectorFrame);

  QWidget* right = new QWidget();
  layout = new QVBoxLayout(right);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(static_cast<FFuQtTable*>(mySummaryTable));
  layout->addStretch(1);
  layout->addWidget(static_cast<FFuQtLabel*>(myAddBCLabel));
  layout->addSpacing(1);

  layout = new QHBoxLayout(this);
  layout->setContentsMargins(10,5,2,2);
  layout->setSpacing(10);
  layout->addWidget(left,1);
  layout->addWidget(middle,1);
  layout->addWidget(right,2);
}


void FuiQtTriadSummary::showFrame(QGroupBox* frame, bool onOff)
{
  if (onOff)
    frame->show();
  else
    frame->hide();
}

void FuiQtTriadSummary::showMass(int onOff)
{
  showFrame(myMassFrame,onOff);

  if (onOff > 3)
  {
    myIxField->popUp();
    myIyField->popUp();
    myIzField->popUp();
  }
  else if (onOff)
  {
    myIxField->popDown();
    myIyField->popDown();
    myIzField->popDown();
  }
}


void FuiQtTriadSummary::resizeEvent(QResizeEvent* e)
{
  this->QWidget::resizeEvent(e);
  this->updateTableGeometry();
}
