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

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtBeamPropSummary.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"


FuiQtBeamPropSummary::FuiQtBeamPropSummary(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  myCrossSectionTypeMenu  = new FFuQtOptionMenu();
  myMaterialDefField      = new FuiQtQueryInputField(NULL);
  myImage                 = new FFuQtLabel();
  myDependencyButton      = new FFuQtToggleButton();

  for (FFuLabelField*& fld : myFields)
    fld = new FFuQtLabelField();

  this->initWidgets();

  QGroupBox* myCrossSectionTypeFrame = new QGroupBox("Cross section type");
  QBoxLayout* layout = new QVBoxLayout(myCrossSectionTypeFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myCrossSectionTypeMenu->getQtWidget());

  myMaterialDefFrame = new QGroupBox("Material");
  layout = new QVBoxLayout(myMaterialDefFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myMaterialDefField->getQtWidget());

  QWidget* qMenus = new QWidget();
  layout = new QHBoxLayout(qMenus);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(myCrossSectionTypeFrame);
  layout->addWidget(myMaterialDefFrame);

  QWidget* qPipeDiameters = new QWidget();
  layout = new QVBoxLayout(qPipeDiameters);
  layout->setContentsMargins(0,0,0,0);
  for (int i = DO; i <= DI; i++)
    layout->addWidget(myFields[i]->getQtWidget());

  myPipeDefFrame = new QGroupBox("Definition");
  layout = new QHBoxLayout(myPipeDefFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(qPipeDiameters);
  layout->addWidget(myImage->getQtWidget());

  myGenericDefFrame = new QGroupBox("Definition");
  QGridLayout* gl = new QGridLayout(myGenericDefFrame);
  gl->setContentsMargins(5,0,5,5);
  gl->setHorizontalSpacing(20);
  for (int i = GEA; i <= GIP; i++)
    gl->addWidget(myFields[i]->getQtWidget(), (i-GEA)%3, (i-GEA)/3);

  QWidget* qLeft = new QWidget();
  layout = new QVBoxLayout(qLeft);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qMenus);
  layout->addWidget(myPipeDefFrame,1);
  layout->addWidget(myGenericDefFrame,1);

  myShearRedFrame = new QGroupBox("Shear reduction factors");
  layout = new QVBoxLayout(myShearRedFrame);
  layout->setContentsMargins(5,0,5,5);
  for (int i = KY; i <= KZ; i++)
    layout->addWidget(myFields[i]->getQtWidget());

  myShearStiffFrame = new QGroupBox("Shear stiffness");
  layout = new QVBoxLayout(myShearStiffFrame);
  layout->setContentsMargins(5,0,5,5);
  for (int i = GASY; i <= GASZ; i++)
    layout->addWidget(myFields[i]->getQtWidget());

  QGroupBox* myShearCenterFrame = new QGroupBox("Shear center offset");
  layout = new QVBoxLayout(myShearCenterFrame);
  layout->setContentsMargins(5,0,5,5);
  for (int i = SY; i <= SZ; i++)
    layout->addWidget(myFields[i]->getQtWidget());

  QWidget* qMiddle = new QWidget();
  layout = new QVBoxLayout(qMiddle);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(myShearRedFrame);
  layout->addWidget(myShearStiffFrame);
  layout->addWidget(myShearCenterFrame);

  myDependentFrame = new QGroupBox("Dependent properties");
  layout = new QVBoxLayout(myDependentFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myDependencyButton->getQtWidget());
  for (int i = AREA; i <= IP; i++)
    layout->addWidget(myFields[i]->getQtWidget());

  layout = new QHBoxLayout(this);
  layout->addWidget(qLeft);
  layout->addWidget(qMiddle);
  layout->addWidget(myDependentFrame);
}


void FuiQtBeamPropSummary::onCrossSectionTypeChanged(int value)
{
  switch (value) {
  case 0: // Pipe
    myMaterialDefFrame->show();
    myPipeDefFrame->show();
    myGenericDefFrame->hide();
    myShearRedFrame->show();
    myShearStiffFrame->hide();
    myDependentFrame->show();
    break;

  case 1: // Generic
    myMaterialDefFrame->hide();
    myPipeDefFrame->hide();
    myGenericDefFrame->show();
    myShearRedFrame->hide();
    myShearStiffFrame->show();
    myDependentFrame->hide();
    break;
  }
}
