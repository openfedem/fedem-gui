// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QApplication>
#include <QClipboard>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtItemsListViews.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtTable.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtRDBMEFatigue.H"


FuiRDBMEFatigue* FuiRDBMEFatigue::create(int xpos, int ypos,
					 int width, int height,
					 const char* title,
					 const char* name)
{
  return new FuiQtRDBMEFatigue(xpos,ypos,width,height,title,name);
}


FuiQtRDBMEFatigue::FuiQtRDBMEFatigue(int xpos, int ypos,
				     int width, int height,
				     const char* title,
				     const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,
                       title,name,Qt::WindowStaysOnTopHint)
{
  tableMain = new FFuQtTable(NULL);
  useProbToggle = new FFuQtToggleButton();
  startTimeField = new FFuQtIOField();
  stopTimeField = new FFuQtIOField();
  standardField = new FFuQtIOField();
  snCurveField = new FFuQtIOField();
  copyDataButton = new FFuQtPushButton();
  notes = new FFuQtNotes();
  dialogButtons = new FFuQtDialogButtons();

  this->initWidgets();

  QBoxLayout* buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0,0,0,0);
  buttonLayout->addWidget(useProbToggle->getQtWidget(),1);
  buttonLayout->addWidget(copyDataButton->getQtWidget());

  QGridLayout* gl = new QGridLayout();
  gl->setContentsMargins(0,0,0,0);
  gl->setHorizontalSpacing(10);
  gl->setVerticalSpacing(2);
  gl->setColumnStretch(2,1);
  gl->addWidget(new QLabel("Start time"),0,0);
  gl->addWidget(new QLabel("Stop time"), 1,0);
  gl->addWidget(new QLabel("Standard"),  2,0);
  gl->addWidget(new QLabel("S-N curve"), 3,0);
  gl->addWidget(startTimeField->getQtWidget(),0,1);
  gl->addWidget(stopTimeField->getQtWidget(), 1,1);
  gl->addWidget(standardField->getQtWidget(), 2,1);
  gl->addWidget(snCurveField->getQtWidget(),  3,1);

  QBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(tableMain->getQtWidget(),1);
  layout->addLayout(buttonLayout);
  layout->addLayout(gl);
  layout->addSpacing(10);
  layout->addWidget(notes->getQtWidget());
  layout->addSpacing(5);
  layout->addWidget(dialogButtons->getQtWidget());
}


void FuiQtRDBMEFatigue::onCopyDataClicked()
{
  std::string text;
  int rowCount = this->tableMain->getNumberRows();
  int colCount = this->tableMain->getNumberColumns();

  // Get header
  for (int j = 0; j < colCount; j++) {
    text.append(this->tableMain->getColumnLabel(j));
    if (j < colCount-1)
      text.append("\t");
  }
  text.append("\n");

  // Get data
  for (int i = 0; i < rowCount; i++) {
    for (int j = 0; j < colCount; j++) {
      if (j > 0)
        text.append(this->tableMain->getText(i,j));
      else if (this->tableMain->getCheckBoxItemToggle(i,j))
        text.append("X");
      if (j < colCount-1)
	text.append("\t");
    }
    text.append("\n");
  }

  QApplication::clipboard()->setText(text.c_str());
}
