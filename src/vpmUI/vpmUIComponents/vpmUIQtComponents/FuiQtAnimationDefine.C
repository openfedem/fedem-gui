// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include "FFuLib/FFuQtComponents/FFuQtTabbedWidgetStack.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtRadioButton.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtAnimationDefine.H"

//----------------------------------------------------------------------------

FuiQtAnimationDefine::FuiQtAnimationDefine(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  timeRadio = new FFuQtRadioButton();
  eigenRadio = new FFuQtRadioButton();
  summaryRadio = new FFuQtRadioButton();

  loadFringeToggle = new FFuQtToggleButton();
  loadLineFringeToggle = new FFuQtToggleButton();
  loadDefToggle = new FFuQtToggleButton();
  loadAnimButton = new FFuQtPushButton();

  tabStack = new FFuQtTabbedWidgetStack(NULL);
  fringeSheet = new FuiQtFringeSheet(NULL);
  timeSheet = new FuiQtTimeSheet(NULL);
  modesSheet = new FuiQtModesSheet(NULL);

  this->initWidgets();

  QGroupBox* qTypeFrame = new QGroupBox("Type");
  QBoxLayout* layout = new QVBoxLayout(qTypeFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(timeRadio->getQtWidget());
  layout->addWidget(eigenRadio->getQtWidget());
  layout->addWidget(summaryRadio->getQtWidget());

  QWidget* qLeft = new QWidget();
  layout = new QVBoxLayout(qLeft);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qTypeFrame,4);
  layout->addStretch(1);
  layout->addWidget(loadFringeToggle->getQtWidget());
  layout->addWidget(loadLineFringeToggle->getQtWidget());
  layout->addWidget(loadDefToggle->getQtWidget());
  layout->addStretch(1);
  layout->addWidget(loadAnimButton->getQtWidget());

  layout = new QHBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qLeft);
  layout->addWidget(tabStack->getQtWidget());
}

/////////////////////////////////////////////////////////////////////////////

FuiQtFringeSheet::FuiQtFringeSheet(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  resultClassMenu = new FFuQtOptionMenu();
  resultMenu = new FFuQtOptionMenu();
  resultOperMenu = new FFuQtOptionMenu();

  setByOperRadio = new FFuQtRadioButton();
  setOperMenu = new FFuQtOptionMenu();
  setByNameRadio = new FFuQtRadioButton();
  setNameMenu = new FFuQtOptionMenu();

  averageOperMenu = new FFuQtOptionMenu();
  averageOnMenu = new FFuQtOptionMenu();
  shellAngleField = new FFuQtIOField();
  elementToggle = new FFuQtToggleButton();

  elemGrpOperLabel = new FFuQtLabel();
  elemGrpOperMenu = new FFuQtOptionMenu();

  autoExportToggle = new FFuQtToggleButton();

  this->initWidgets();

  QGroupBox* qFringeFrame = new QGroupBox("Contour value");
  QGridLayout* gl = new QGridLayout(qFringeFrame);
  gl->setContentsMargins(5,0,5,5);
  gl->addWidget(new QLabel("Result class"), 0,0);
  gl->addWidget(new QLabel("Result"), 1,0);
  gl->addWidget(new QLabel("Operation"), 2,0);
  gl->addWidget(resultClassMenu->getQtWidget(), 0,1);
  gl->addWidget(resultMenu->getQtWidget(), 1,1);
  gl->addWidget(resultOperMenu->getQtWidget(), 2,1);

  QGroupBox* qResultSetFrame = new QGroupBox("Result set");
  gl = new QGridLayout(qResultSetFrame);
  gl->setContentsMargins(5,0,5,5);
  gl->setColumnStretch(1,1);
  gl->addWidget(setByOperRadio->getQtWidget(), 0,0);
  gl->addWidget(setByNameRadio->getQtWidget(), 1,0);
  gl->addWidget(setOperMenu->getQtWidget(), 0,1);
  gl->addWidget(setNameMenu->getQtWidget(), 1,1);

  QWidget* qAvgOper = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qAvgOper);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Operation"));
  layout->addWidget(averageOperMenu->getQtWidget(),1);
  layout->addWidget(new QLabel("on"));
  layout->addWidget(averageOnMenu->getQtWidget(),1);

  QWidget* qAvgAngle = new QWidget();
  layout = new QHBoxLayout(qAvgAngle);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Max shell average angle"));
  layout->addWidget(shellAngleField->getQtWidget(),1);

  QGroupBox* qAverageFrame = new QGroupBox("Averaging");
  layout = new QVBoxLayout(qAverageFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(qAvgOper);
  layout->addWidget(qAvgAngle);
  layout->addWidget(elementToggle->getQtWidget());

  QGroupBox* qMultiFaceFrame = new QGroupBox("Multiple face results");
  layout = new QHBoxLayout(qMultiFaceFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(elemGrpOperLabel->getQtWidget());
  layout->addWidget(elemGrpOperMenu->getQtWidget(),1);

  gl = new QGridLayout(this);
  gl->setContentsMargins(2,0,2,2);
  gl->setRowStretch(0,3);
  gl->setRowStretch(1,2);
  gl->addWidget(qFringeFrame, 0,0);
  gl->addWidget(qResultSetFrame, 1,0, 2,1);
  gl->addWidget(qAverageFrame, 0,1);
  gl->addWidget(qMultiFaceFrame, 1,1);
  gl->addWidget(autoExportToggle->getQtWidget(), 2,1);
}

/////////////////////////////////////////////////////////////////////////////

FuiQtTimeSheet::FuiQtTimeSheet(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  completeSimRadio = new FFuQtRadioButton();
  timeIntRadio = new FFuQtRadioButton();
  startField = new FFuQtIOField();
  stopField = new FFuQtIOField();

  mostFramesRadio = new FFuQtRadioButton();
  leastFramesRadio = new FFuQtRadioButton();

  this->initWidgets();

  QWidget* qInterval = new QWidget();
  QGridLayout* gl = new QGridLayout(qInterval);
  gl->setContentsMargins(20,0,0,0);
  gl->addWidget(new QLabel("Start"), 0,0);
  gl->addWidget(new QLabel("Stop"), 1,0);
  gl->addWidget(startField->getQtWidget(), 0,1);
  gl->addWidget(stopField->getQtWidget(), 1,1);

  QGroupBox* qTimeFrame = new QGroupBox("Time window");
  QBoxLayout* layout = new QVBoxLayout(qTimeFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addStretch(1);
  layout->addWidget(completeSimRadio->getQtWidget());
  layout->addStretch(2);
  layout->addWidget(timeIntRadio->getQtWidget());
  layout->addWidget(qInterval,4);

  QGroupBox* qFramesFrame = new QGroupBox("Animation frames");
  layout = new QVBoxLayout(qFramesFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(mostFramesRadio->getQtWidget());
  layout->addWidget(leastFramesRadio->getQtWidget());

  layout = new QHBoxLayout(this);
  layout->setContentsMargins(2,0,2,2);
  layout->addWidget(qTimeFrame);
  layout->addWidget(qFramesFrame);
}

/////////////////////////////////////////////////////////////////////////////

FuiQtModesSheet::FuiQtModesSheet(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  typeMenu = new FFuQtOptionMenu();
  linkLabel = new FFuQtLabel();
  linkMenu = new FuiQtQueryInputField(NULL);
  timeLabel = new FFuQtLabel();
  timeMenu = new FFuQtOptionMenu();
  modeMenu = new FFuQtOptionMenu();
  scaleField = new FFuQtIOField();

  framesPrField = new FFuQtIOField();

  lengthFrame = new FFuQtLabelFrame();
  timeLengthRadio = new FFuQtRadioButton();
  timeLengthField = new FFuQtIOField();
  nCyclRadio = new FFuQtRadioButton();
  nCyclField = new FFuQtIOField();
  dampedRadio = new FFuQtRadioButton();
  dampedField = new FFuQtIOField();

  this->initWidgets();

  QGroupBox* qMode = new QGroupBox("Mode selection");
  QGridLayout* gl = new QGridLayout(qMode);
  gl->setContentsMargins(5,0,5,5);
  gl->addWidget(typeMenu->getQtWidget(), 0,0, 1,2);
  gl->addWidget(linkLabel->getQtWidget(), 1,0);
  gl->addWidget(timeLabel->getQtWidget(), 2,0);
  gl->addWidget(new QLabel("Mode"), 3,0);
  gl->addWidget(new QLabel("Scale"), 4,0);
  gl->addWidget(linkMenu->getQtWidget(), 1,1);
  gl->addWidget(timeMenu->getQtWidget(), 2,1);
  gl->addWidget(modeMenu->getQtWidget(), 3,1);
  gl->addWidget(scaleField->getQtWidget(), 4,1);

  QWidget* qFrames = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qFrames);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Frames per cycle"));
  layout->addWidget(framesPrField->getQtWidget());

  gl = new QGridLayout(lengthFrame->getQtWidget());
  gl->setContentsMargins(5,0,5,5);
  gl->addWidget(timeLengthRadio->getQtWidget(), 0,0);
  gl->addWidget(timeLengthField->getQtWidget(), 0,1);
  gl->addWidget(nCyclRadio->getQtWidget(), 1,0);
  gl->addWidget(nCyclField->getQtWidget(), 1,1);
  gl->addWidget(dampedRadio->getQtWidget(), 2,0);
  gl->addWidget(dampedField->getQtWidget(), 2,1);

  QGroupBox* qFrameGen = new QGroupBox("Frame generation");
  layout = new QVBoxLayout(qFrameGen);
  layout->setContentsMargins(5,5,5,5);
  layout->addWidget(qFrames,1,Qt::AlignTop);
  layout->addWidget(lengthFrame->getQtWidget(),4);

  layout = new QHBoxLayout(this);
  layout->setContentsMargins(2,0,2,2);
  layout->addWidget(qMode);
  layout->addWidget(qFrameGen);
}
