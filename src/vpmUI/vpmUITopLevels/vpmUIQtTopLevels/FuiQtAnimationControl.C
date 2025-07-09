// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtRadioButton.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"

#include "FuiQtAnimationControl.H"


FuiAnimationControl* FuiAnimationControl::create(int xpos, int ypos,
						 int width,int height,
						 const char* title,
						 const char* name)
{
  return new FuiQtAnimationControl(xpos,ypos,width,height,title,name);
}


FuiQtAnimationControl::FuiQtAnimationControl(int xpos, int ypos,
					     int width,int height,
					     const char* title,
					     const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,
                       title,name,Qt::WindowStaysOnTopHint)
{
  FFuQtLabelFrame* qCamera;
  FFuQtLabelFrame* qMotion;
  FFuQtLabelFrame* qDeform;
  FFuQtLabelFrame* qLegend;
  FFuQtLabelFrame* qTicks;

  cameraFrame       = qCamera = new FFuQtLabelFrame();
  cameraMenu        = new FuiQtQueryInputField(NULL);

  motionFrame       = qMotion = new FFuQtLabelFrame();
  linkMotionToggle  = new FFuQtToggleButton();
  triadMotionToggle = new FFuQtToggleButton();

  defFrame      = qDeform = new FFuQtLabelFrame();
  defToggle     = new FFuQtToggleButton();
  defScaleField = new FFuQtIOField();

  legendFrame  = qLegend = new FFuQtLabelFrame();
  fringeToggle = new FFuQtToggleButton();
  legendToggle = new FFuQtToggleButton();

  legendColorsMenu   = new FFuQtOptionMenu();
  legendMappingMenu  = new FFuQtOptionMenu();
  legendLookMenu     = new FFuQtOptionMenu();

  legendMaxField = new FFuQtIOField();
  legendMinField = new FFuQtIOField();

  tickMarkFrame      = qTicks = new FFuQtLabelFrame();
  tickMarkCountRadio = new FFuQtRadioButton();
  tickMarkCountField = new FFuQtIOField();
  tickMarkSpaceRadio = new FFuQtRadioButton();
  tickMarkSpaceField = new FFuQtIOField();

  dialogButtons = new FFuQtDialogButtons(NULL,false);

  this->initWidgets();

  QBoxLayout* layout = new QHBoxLayout(qCamera);
  layout->addWidget(new QLabel("Reference Part"));
  layout->addWidget(static_cast<FuiQtQueryInputField*>(cameraMenu),1);

  layout = new QVBoxLayout(qMotion);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(linkMotionToggle));
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(triadMotionToggle));

  QWidget* qDefScale = new QWidget();
  layout = new QHBoxLayout(qDefScale);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Scale"));
  layout->addWidget(static_cast<FFuQtIOField*>(defScaleField),1);

  layout = new QVBoxLayout(qDeform);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(defToggle));
  layout->addWidget(qDefScale);

  QWidget* qContour = new QWidget();
  QGridLayout* gl = new QGridLayout(qContour);
  gl->setContentsMargins(0,0,0,0);
  gl->setHorizontalSpacing(20);
  gl->setColumnStretch(1,1);
  gl->addWidget(new QLabel("Colors"), 0,0);
  gl->addWidget(new QLabel("Mapping"),1,0);
  gl->addWidget(new QLabel("Look"),   2,0);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(legendColorsMenu) ,0,1);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(legendMappingMenu),1,1);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(legendLookMenu),   2,1);

  QWidget* qRange = new QWidget();
  gl = new QGridLayout(qRange);
  gl->setContentsMargins(0,0,0,0);
  gl->addWidget(new QLabel("Max"),0,0);
  gl->addWidget(new QLabel("Min"),1,0);
  gl->addWidget(static_cast<FFuQtIOField*>(legendMaxField),0,1);
  gl->addWidget(static_cast<FFuQtIOField*>(legendMinField),1,1);

  gl = new QGridLayout(qTicks);
  gl->addWidget(dynamic_cast<FFuQtRadioButton*>(tickMarkCountRadio),0,0);
  gl->addWidget(dynamic_cast<FFuQtRadioButton*>(tickMarkSpaceRadio),1,0);
  gl->addWidget(static_cast<FFuQtIOField*>(tickMarkCountField),0,1);
  gl->addWidget(static_cast<FFuQtIOField*>(tickMarkSpaceField),1,1);

  layout = new QVBoxLayout(qLegend);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(fringeToggle));
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(legendToggle));
  layout->addWidget(qContour);
  layout->addWidget(qRange);
  layout->addWidget(qTicks);

  layout = new QVBoxLayout(this);
  layout->addWidget(qCamera);
  layout->addStretch(2);
  layout->addWidget(qMotion);
  layout->addStretch(1);
  layout->addWidget(qDeform);
  layout->addStretch(3);
  layout->addWidget(qLegend);
  layout->addStretch(1);
  layout->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons),
                    0, Qt::AlignBottom);
}
