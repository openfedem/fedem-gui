// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtPositionData.H"


FuiQtPositionData::FuiQtPositionData(QWidget* parent, int xpos, int ypos,
                                     int width, int height, const char* name)
  : FFuQtMultUIComponent(parent,xpos,ypos,width,height,name)
{
  FFuQtLabel*   qLabel;
  FFuQtIOField* qField;

  QGridLayout* gLayout;
  QVBoxLayout* vLayout;

  // The translation frame
  FFuQtLabelFrame* translationFrame = new FFuQtLabelFrame();
  translationFrame->setTitle("Position");

  vLayout = new QVBoxLayout(translationFrame);
  vLayout->setContentsMargins(5,3,5,5);
  vLayout->setSpacing(2);

  FFuQtOptionMenu* posViewTypeMenu = new FFuQtOptionMenu();
  FuiQtQueryInputField* posRefCSField = new FuiQtQueryInputField(NULL);
  posRefCSField->setMinimumHeight(posViewTypeMenu->getHeightHint());

  gLayout = new QGridLayout();
  gLayout->setHorizontalSpacing(2);
  gLayout->addWidget(new QLabel("Reference CS"), 0, 0);
  gLayout->addWidget(new QLabel("Coordinate type"), 0, 1);
  gLayout->addWidget(posRefCSField, 1, 0);
  gLayout->addWidget(posViewTypeMenu, 1, 1);
  vLayout->addLayout(gLayout);

  gLayout = new QGridLayout();
  gLayout->setHorizontalSpacing(2);
  for (int i = 0; i < 3; i++) {
    myLabels[i] = qLabel = new FFuQtLabel();
    myFields[i] = qField = new FFuQtIOField();
    gLayout->addWidget(qLabel, 0, i);
    gLayout->addWidget(qField, 1, i);
  }
  vLayout->addLayout(gLayout);

  // The rotation frame
  FFuQtLabelFrame* rotationFrame = new FFuQtLabelFrame();
  rotationFrame->setTitle("Orientation");

  vLayout = new QVBoxLayout(rotationFrame);
  vLayout->setContentsMargins(5,3,5,5);
  vLayout->setSpacing(2);

  FFuQtOptionMenu* rotViewTypeMenu = new FFuQtOptionMenu();
  FuiQtQueryInputField* rotRefCSField = new FuiQtQueryInputField(NULL);
  rotRefCSField->setMinimumHeight(rotViewTypeMenu->getHeightHint());

  gLayout = new QGridLayout();
  gLayout->setHorizontalSpacing(2);
  gLayout->addWidget(new QLabel("Reference CS"), 0, 0);
  gLayout->addWidget(new QLabel("Coordinate type"), 0, 1);
  gLayout->addWidget(rotRefCSField, 1, 0);
  gLayout->addWidget(rotViewTypeMenu, 1, 1);
  vLayout->addLayout(gLayout);

  gLayout = new QGridLayout();
  gLayout->setHorizontalSpacing(2);
  for (int i = 3; i < 9; i++) {
    myLabels[i] = qLabel = new FFuQtLabel();
    myFields[i] = qField = new FFuQtIOField();
    gLayout->addWidget(qLabel, i < 6 ? 0 : 2, i%3);
    gLayout->addWidget(qField, i < 6 ? 1 : 3, i%3);
  }
  vLayout->addLayout(gLayout);

  // Position follow toggle group
  FFuQtLabelFrame* followFrame = new FFuQtLabelFrame();
  followFrame->setLabel("Relative positioning of Triads");
  FFuQtToggleButton* masterToggle = new FFuQtToggleButton();
  masterToggle->setText("Master triad follows joint");
  FFuQtToggleButton* slaveToggle = new FFuQtToggleButton();
  slaveToggle->setText("Slave triad follows joint");

  myTranslationFrame = translationFrame;
  myRotationFrame = rotationFrame;

  myPosRefCSField = posRefCSField;
  myRotRefCSField = rotRefCSField;

  myPosViewTypeMenu = posViewTypeMenu;
  myRotViewTypeMenu = rotViewTypeMenu;

  myTriadPosFollowFrame = followFrame;
  myMasterFollowToggle = masterToggle;
  mySlaveFollowToggle = slaveToggle;

  this->initWidgets();

  // Layout for the position follow frame
  vLayout = new QVBoxLayout(followFrame);
  vLayout->setContentsMargins(10,3,10,3);
  vLayout->setSpacing(2);
  vLayout->addWidget(masterToggle);
  vLayout->addWidget(slaveToggle);

  // The main layout
  gLayout = new QGridLayout(this);
  gLayout->setContentsMargins(2,2,2,2);
  gLayout->addWidget(translationFrame, 0, 0, 2, 1, Qt::AlignTop);
  gLayout->addWidget(rotationFrame, 0, 1, Qt::AlignTop);
  gLayout->addWidget(followFrame, 1, 1, Qt::AlignTop);
}
