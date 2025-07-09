// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQt3DPoint.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtMemo.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtFileBrowseField.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtModelPreferences.H"


FuiModelPreferences* FuiModelPreferences::create(int xpos, int ypos,
						 int width, int height,
						 const char* title,
						 const char* name)
{
  return new FuiQtModelPreferences(NULL,xpos,ypos,width,height,title,name);
}


FuiQtModelPreferences::FuiQtModelPreferences(QWidget* parent,
					     int xpos, int ypos,
					     int width, int height,
					     const char* title,
					     const char* name)
  : FFuQtTopLevelShell(parent,xpos,ypos,width,height,title,name)
{
  FFuQtLabelFrame* qDescr;
  FFuQtLabelFrame* qUnits;
  FFuQtLabelFrame* qToler;
  FFuQtLabelFrame* qGrav;
  FFuQtLabelFrame* qInitU;
  FFuQtLabelFrame* qRepos;
  FFuQtLabelFrame* qExtFn;

  descriptionFrame = qDescr = new FFuQtLabelFrame();
  descriptionMemo  = new FFuQtMemo();

  unitsFrame = qUnits = new FFuQtLabelFrame();
  unitsMenu  = new FFuQtOptionMenu();

  modelingTolFrame = qToler = new FFuQtLabelFrame();
  modelingTolField = new FFuQtIOField();

  gravitationFrame  = qGrav = new FFuQtLabelFrame();
  gravitationVector = new FuiQt3DPoint(NULL,"g",false);

  initialVelFrame  = qInitU = new FFuQtLabelFrame();
  initialVelVector = new FuiQt3DPoint(NULL,"v0",false);

  repositoryFrame = qRepos = new FFuQtLabelFrame();
  repositoryField = new FFuQtIOField();
  changeButton    = new FFuQtPushButton();
  switchButton    = new FFuQtPushButton();
  overwriteButton = new FFuQtToggleButton();

  extFuncFileFrame  = qExtFn = new FFuQtLabelFrame();
  extFuncFileButton = new FFuQtToggleButton();
  extFuncFileField  = new FFuQtFileBrowseField(NULL);

  dialogButtons = new FFuQtDialogButtons(NULL,false);

  this->initWidgets();

  QBoxLayout* layout = new QHBoxLayout(qDescr);
  layout->setContentsMargins(1,0,1,2);
  layout->addWidget(static_cast<FFuQtMemo*>(descriptionMemo));

  QLabel* qTolLabel = new QLabel("Absolute modeling tolerance");
  layout = new QHBoxLayout(qToler);
  layout->addWidget(qTolLabel);
  layout->addWidget(static_cast<FFuQtIOField*>(modelingTolField));

  QLabel* qUnitLabel = new QLabel("Model database units");
  qUnitLabel->setMinimumWidth(qTolLabel->sizeHint().width());
  layout = new QHBoxLayout(qUnits);
  layout->addWidget(qUnitLabel);
  layout->addWidget(static_cast<FFuQtOptionMenu*>(unitsMenu),1);

  layout = new QHBoxLayout(qGrav);
  layout->setContentsMargins(0,0,0,1);
  layout->addWidget(static_cast<FuiQt3DPoint*>(gravitationVector));

  layout = new QHBoxLayout(qInitU);
  layout->setContentsMargins(0,0,0,1);
  layout->addWidget(static_cast<FuiQt3DPoint*>(initialVelVector));

  QWidget* qGravAndU0 = new QWidget();
  layout = new QHBoxLayout(qGravAndU0);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qGrav);
  layout->addWidget(qInitU);

  QWidget* qRepField = new QWidget();
  layout = new QHBoxLayout(qRepField);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(static_cast<FFuQtIOField*>(repositoryField),1);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(changeButton));

  layout = new QVBoxLayout(qRepos);
  layout->addWidget(qRepField);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(switchButton));
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(overwriteButton));

  layout = new QVBoxLayout(qExtFn);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(extFuncFileButton));
  layout->addWidget(static_cast<FFuQtFileBrowseField*>(extFuncFileField));

  layout = new QVBoxLayout(this);
  layout->addWidget(qDescr,1);
  layout->addWidget(qUnits);
  layout->addWidget(qToler);
  layout->addWidget(qGravAndU0);
  layout->addWidget(qRepos);
  layout->addWidget(qExtFn);
  layout->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons));
}
