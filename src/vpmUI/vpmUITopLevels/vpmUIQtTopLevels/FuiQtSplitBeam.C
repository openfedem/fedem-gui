// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QVBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtSplitBeam.H"


FuiSplitBeam* FuiSplitBeam::create(int xpos, int ypos, int width, int height,
                                   const char* title, const char* name)
{
  return new FuiQtSplitBeam(xpos,ypos,width,height,title,name);
}


FuiQtSplitBeam::FuiQtSplitBeam(int xpos, int ypos, int width, int height,
                               const char* title, const char* name)
  : FFuQtModalDialog(xpos,ypos,width,height,title,name)
{
  FFuQtLabelField* qField = new FFuQtLabelField();
  myDialogButtons = new FFuQtDialogButtons();

  qField->setFixedHeight(25);
  myNumField = qField;

  this->initWidgets();

  QLayout* layout = new QVBoxLayout(this);
  layout->addWidget(qField);
  layout->addWidget(static_cast<FFuQtDialogButtons*>(myDialogButtons));
}
