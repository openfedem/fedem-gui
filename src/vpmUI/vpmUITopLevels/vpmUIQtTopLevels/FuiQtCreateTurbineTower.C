// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtCreateTurbineTower.H"


FuiCreateTurbineTower* FuiCreateTurbineTower::create(int xpos, int ypos,
                                                     int width, int height,
                                                     const char* title,
                                                     const char* name)
{
  return new FuiQtCreateTurbineTower(xpos,ypos,width,height,title,name);
}


FuiQtCreateTurbineTower::FuiQtCreateTurbineTower(int xpos, int ypos,
                                                 int width, int height,
                                                 const char* title,
                                                 const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,
                       title,name,Qt::MSWindowsFixedSizeDialogHint)
{
  headerImage = new FFuQtLabel();
  modelImage  = new FFuQtLabel();
  notes       = new FFuQtNotes();

  std::array<FFuQtLabelField*,12> qFields;
  for (FFuQtLabelField*& field : qFields)
    field = new FFuQtLabelField();

  towerGeometryFrame   = new FFuQtLabelFrame();
  towerGeometryD1Field = qFields[0];
  towerGeometryD2Field = qFields[1];
  towerGeometryM1Field = qFields[2];
  towerGeometryH1Field = qFields[3];
  towerGeometryH2Field = qFields[4];
  towerWallThicknField = qFields[5];

  towerMaterialFrame    = new FFuQtLabelFrame();
  towerMaterialMenu     = new FuiQtQueryInputField(NULL);
  towerMaterialRhoField = qFields[6];
  towerMaterialEField   = qFields[7];
  towerMaterialNuField  = qFields[8];
  towerMaterialGField   = qFields[9];

  towerMeshFrame   = new FFuQtLabelFrame();
  towerMeshN1Field = qFields[10];
  towerMeshN2Field = qFields[11];

  dialogButtons = new FFuQtDialogButtons();

  this->initWidgets();

  // Crop the right edge of header image (size 827x169 pixels)
  FFuQtLabel* qHeader = static_cast<FFuQtLabel*>(headerImage);
  qHeader->setPixmap(qHeader->pixmap().copy(0,0,675,169));

  for (FFuQtLabelField*& field : qFields)
    field->setFixedHeight(20);
  static_cast<FuiQtQueryInputField*>(towerMaterialMenu)->setFixedHeight(20);

  int i, labelWidth = towerWallThicknField->getLabelWidth(true);
  for (int i = 0; i <= 6; i++)
    qFields[i]->setLabelWidth(labelWidth);
  for (int i = 6; i < 12; i++)
    qFields[i]->setLabelWidth(20);

  QBoxLayout* layout = NULL;
  layout = new QVBoxLayout(towerGeometryFrame->getQtWidget());
  layout->setContentsMargins(10,2,10,10);
  layout->setSpacing(2);
  for (i = 0; i < 6; i++)
    layout->addWidget(qFields[i]);

  layout = new QVBoxLayout(towerMaterialFrame->getQtWidget());
  layout->addWidget(towerMaterialMenu->getQtWidget());
  for (i = 6; i < 9; i++)
    layout->addWidget(qFields[i]);

  layout = new QVBoxLayout(towerMeshFrame->getQtWidget());
  for (i = 10; i < 12; i++)
    layout->addWidget(qFields[i]);

  QGridLayout* gl = new QGridLayout();
  gl->setContentsMargins(0,0,0,0);
  gl->setHorizontalSpacing(10);
  gl->setVerticalSpacing(5);
  gl->setColumnStretch(0,5);
  gl->setColumnStretch(1,4);
  gl->addWidget(towerGeometryFrame->getQtWidget(), 0,0);
  gl->addWidget(towerMaterialFrame->getQtWidget(), 0,1);
  gl->addWidget(towerMeshFrame->getQtWidget(),     1,1);
  gl->addWidget(notes->getQtWidget(),         2,0,1,-1);
  gl->addWidget(dialogButtons->getQtWidget(), 3,0,1,-1);

  QBoxLayout* mainLayout = new QHBoxLayout();
  mainLayout->setContentsMargins(10,0,0,10);
  mainLayout->setSpacing(40);
  mainLayout->addLayout(gl);
  mainLayout->addWidget(modelImage->getQtWidget(),1);

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(headerImage->getQtWidget());
  layout->addLayout(mainLayout);
}
