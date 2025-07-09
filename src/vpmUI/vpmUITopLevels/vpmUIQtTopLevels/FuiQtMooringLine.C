// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtMooringLine.H"


FuiMooringLine* FuiMooringLine::create(int xpos, int ypos,
                                       int width, int height,
                                       const char* title,
                                       const char* name)
{
  return new FuiQtMooringLine(xpos,ypos,width,height,title,name);
}


FuiQtMooringLine::FuiQtMooringLine(int xpos, int ypos,
                                   int width, int height,
                                   const char* title,
                                   const char* name)
  : FFuQtModalDialog(xpos,ypos,width,height,title,name)
{
  FFuQtLabelField* qNumField;
  FFuQtLabelField* qLenField;
  FFuQtOptionMenu* qTypeMenu;

  myNumField = qNumField = new FFuQtLabelField();
  myTypeMenu = qTypeMenu = new FFuQtOptionMenu();
  myLengthField = qLenField = new FFuQtLabelField();
  myDialogButtons = new FFuQtDialogButtons();

  qNumField->setFixedHeight(22);
  qLenField->setFixedHeight(22);
  this->initWidgets();

  QWidget* qMenu = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qMenu);
  layout->setContentsMargins(0,5,0,5);
  layout->addWidget(new QLabel("Element type"));
  layout->addWidget(qTypeMenu,1);

  layout = new QVBoxLayout(this);
  layout->addWidget(qNumField);
  layout->addWidget(qMenu);
  layout->addWidget(qLenField);
  layout->addWidget(static_cast<FFuQtDialogButtons*>(myDialogButtons));

  myDialogButtons->setButtonClickedCB(FFaDynCB1M(FuiQtMooringLine,this,
                                                 onDialogButtonClicked,int));
}


void FuiQtMooringLine::onDialogButtonClicked(int button)
{
  this->done(button == 0 ? QDialog::Accepted : QDialog::Rejected);
}
