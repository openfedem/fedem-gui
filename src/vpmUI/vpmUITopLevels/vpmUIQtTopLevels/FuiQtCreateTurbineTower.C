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
  notesText   = new FFuQtLabel();

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
  layout = new QVBoxLayout(static_cast<FFuQtLabelFrame*>(towerGeometryFrame));
  layout->setContentsMargins(10,2,10,10);
  layout->setSpacing(2);
  for (i = 0; i < 6; i++)
    layout->addWidget(qFields[i]);

  layout = new QVBoxLayout(static_cast<FFuQtLabelFrame*>(towerMaterialFrame));
  layout->addWidget(static_cast<FuiQtQueryInputField*>(towerMaterialMenu));
  for (i = 6; i < 9; i++)
    layout->addWidget(qFields[i]);

  layout = new QVBoxLayout(static_cast<FFuQtLabelFrame*>(towerMeshFrame));
  for (i = 10; i < 12; i++)
    layout->addWidget(qFields[i]);

  QWidget* qLeft = new QWidget();
  QGridLayout* gl = new QGridLayout(qLeft);
  gl->setContentsMargins(0,0,0,0);
  gl->setColumnStretch(0,5);
  gl->setColumnStretch(1,4);
  gl->addWidget(static_cast<FFuQtLabelFrame*>(towerGeometryFrame), 0,0);
  gl->addWidget(static_cast<FFuQtLabelFrame*>(towerMaterialFrame), 0,1);
  gl->addWidget(static_cast<FFuQtLabelFrame*>(towerMeshFrame),     1,1);
  gl->addWidget(new FFuQtNotesLabel(), 2,0);
  gl->addWidget(static_cast<FFuQtLabel*>(notesText), 3,0,1,-1);
  gl->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons), 4,0,1,-1);

  QWidget* qMain = new QWidget();
  layout = new QHBoxLayout(qMain);
  layout->setContentsMargins(10,0,0,10);
  layout->setSpacing(40);
  layout->addWidget(qLeft);
  layout->addWidget(static_cast<FFuQtLabel*>(modelImage),1);

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(static_cast<FFuQtLabel*>(headerImage));
  layout->addWidget(qMain);
}
