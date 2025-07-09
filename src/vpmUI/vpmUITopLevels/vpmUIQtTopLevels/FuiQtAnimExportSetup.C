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

#include "FFuLib/FFuQtComponents/FFuQtSpinBox.H"
#include "FFuLib/FFuQtComponents/FFuQtRadioButton.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtAnimExportSetup.H"


FuiAnimExportSetup* FuiAnimExportSetup::create(int xpos, int ypos,
					       int width, int height,
					       const char* title,
					       const char* name)
{
  return new FuiQtAnimExportSetup(xpos,ypos,width,height,title,name);
}


FuiQtAnimExportSetup::FuiQtAnimExportSetup(int xpos, int ypos,
					   int width, int height,
					   const char* title,
					   const char* name)
  : FFuQtModalDialog(xpos,ypos,width,height,title,name)
{
  myAllBtn  = new FFuQtRadioButton();
  myRealBtn = new FFuQtRadioButton();
  myOmitBtn = new FFuQtRadioButton();
  mySomeBtn = new FFuQtRadioButton();

  omitNthSpinBox  = new FFuQtSpinBox();
  everyNthSpinBox = new FFuQtSpinBox();

  myFileField    = new FFuQtIOField();
  myBrowseButton = new FFuQtPushButton();

  dialogButtons = new FFuQtDialogButtons();

  this->initWidgets();

  QWidget* qFileField = new QWidget();
  QLayout* layout = new QHBoxLayout(qFileField);
  layout->addWidget(new QLabel("File"));
  layout->addWidget(static_cast<FFuQtIOField*>(myFileField));
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(myBrowseButton));

  QWidget* qOptions = new QWidget();
  QGridLayout* gl = new QGridLayout(qOptions);
  gl->addWidget(dynamic_cast<FFuQtRadioButton*>(myAllBtn)  ,0,0);
  gl->addWidget(dynamic_cast<FFuQtRadioButton*>(myRealBtn) ,1,0);
  gl->addWidget(dynamic_cast<FFuQtRadioButton*>(myOmitBtn) ,2,0);
  gl->addWidget(dynamic_cast<FFuQtRadioButton*>(mySomeBtn) ,3,0);
  gl->addWidget(static_cast<FFuQtSpinBox*>(omitNthSpinBox) ,2,1);
  gl->addWidget(static_cast<FFuQtSpinBox*>(everyNthSpinBox),3,1);

  layout = new QVBoxLayout(this);
  layout->addWidget(qFileField);
  layout->addWidget(qOptions);
  layout->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons));
}
