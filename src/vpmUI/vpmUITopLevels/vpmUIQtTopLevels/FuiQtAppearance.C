// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtScale.H"
#include "FFuLib/FFuQtComponents/FFuQtColorSelector.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtAppearance.H"


FuiAppearance* FuiAppearance::create(int xpos, int ypos,
                                     int width, int height,
                                     const char* title,
                                     const char* name)
{
  return new FuiQtAppearance(xpos,ypos,width,height,title,name);
}


FuiQtAppearance::FuiQtAppearance(int xpos, int ypos,
                                 int width, int height,
                                 const char* title,
                                 const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,
                       title,name,Qt::WindowStaysOnTopHint)
{
  FFuQtLabelFrame* qtDetailsFrame;
  FFuQtLabelFrame* qtColorFrame;
  FFuQtLabelFrame* qtMaterialFrame;

  myDetailsFrame  = qtDetailsFrame  = new FFuQtLabelFrame();
  myColorFrame    = qtColorFrame    = new FFuQtLabelFrame();
  myMaterialFrame = qtMaterialFrame = new FFuQtLabelFrame();
  myDialogButtons = new FFuQtDialogButtons(NULL,false);

  myPolygonOptions = new FFuQtOptionMenu();
  myLinesOptions   = new FFuQtOptionMenu();

  myColorSelector = new FFuQtColorSelector(NULL);

  FFuQtScale* qtShininessScale;
  FFuQtScale* qtTransperanceScale;
  myShininessScale    = qtShininessScale    = new FFuQtScale();
  myTransparencyScale = qtTransperanceScale = new FFuQtScale();
  qtShininessScale->setOrientation(Qt::Horizontal);
  qtTransperanceScale->setOrientation(Qt::Horizontal);

  this->initWidgets();

  QGridLayout* glayout = new QGridLayout(qtDetailsFrame);
  glayout->setColumnStretch(1,1);
  glayout->addWidget(new QLabel("Polygons"),0,0);
  glayout->addWidget(new QLabel("Lines")   ,1,0);
  glayout->addWidget(static_cast<FFuQtOptionMenu*>(myPolygonOptions),0,1);
  glayout->addWidget(static_cast<FFuQtOptionMenu*>(myLinesOptions)  ,1,1);

  QBoxLayout* qlayout = new QVBoxLayout(qtColorFrame);
  qlayout->addWidget(static_cast<FFuQtColorSelector*>(myColorSelector));

  glayout = new QGridLayout(qtMaterialFrame);
  glayout->addWidget(new QLabel("Shininess")   ,0,0);
  glayout->addWidget(new QLabel("Transparency"),1,0);
  glayout->addWidget(qtShininessScale   ,0,1);
  glayout->addWidget(qtTransperanceScale,1,1);

  qlayout = new QVBoxLayout(this);
  qlayout->addWidget(qtDetailsFrame);
  qlayout->addWidget(qtColorFrame);
  qlayout->addWidget(qtMaterialFrame);
  qlayout->addWidget(static_cast<FFuQtDialogButtons*>(myDialogButtons),
                     0, Qt::AlignBottom);
}
