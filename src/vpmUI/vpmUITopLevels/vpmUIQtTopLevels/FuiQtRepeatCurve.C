// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QVBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtRepeatCurve.H"


FuiRepeatCurve* FuiRepeatCurve::create(int xpos, int ypos,
				       int width, int height,
				       const char* title,
				       const char* name)
{
  return new FuiQtRepeatCurve(xpos,ypos,width,height,title,name);
}


FuiQtRepeatCurve::FuiQtRepeatCurve(int xpos, int ypos,
				   int width, int height,
				   const char* title,
				   const char* name)
  : FFuQtModalDialog(xpos,ypos,width,height,title,name)
{
  FFuQtLabel*      qLabel;
  FFuQtLabelField* qFromField;
  FFuQtLabelField* qToField;

  myDialogButtons = new FFuQtDialogButtons();

  myLabel = qLabel = new FFuQtLabel();
  myFromField = qFromField = new FFuQtLabelField();
  myToField = qToField = new FFuQtLabelField();
  myDialogButtons = new FFuQtDialogButtons();

  qFromField->setFixedHeight(25);
  qToField->setFixedHeight(25);
  this->initWidgets();

  QBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(qLabel,1);
  layout->addWidget(qFromField);
  layout->addWidget(qToField);
  layout->addWidget(static_cast<FFuQtDialogButtons*>(myDialogButtons));
}
