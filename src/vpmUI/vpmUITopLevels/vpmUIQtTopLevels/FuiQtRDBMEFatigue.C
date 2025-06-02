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
  notesText = new FFuQtLabel();
  dialogButtons = new FFuQtDialogButtons();

  this->initWidgets();

  QWidget* qButtons = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qButtons);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(useProbToggle),1);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(copyDataButton));

  QWidget* qFields = new QWidget();
  QGridLayout* gl = new QGridLayout(qFields);
  gl->setContentsMargins(0,0,0,0);
  gl->setVerticalSpacing(2);
  gl->addWidget(new QLabel("Start time"),0,0);
  gl->addWidget(new QLabel("Stop time"), 1,0);
  gl->addWidget(new QLabel("Standard"),  2,0);
  gl->addWidget(new QLabel("S-N curve"), 3,0);
  gl->addWidget(static_cast<FFuQtIOField*>(startTimeField),0,1);
  gl->addWidget(static_cast<FFuQtIOField*>(stopTimeField), 1,1);
  gl->addWidget(static_cast<FFuQtIOField*>(standardField), 2,1);
  gl->addWidget(static_cast<FFuQtIOField*>(snCurveField),  3,1);

  QWidget* qHalf = new QWidget();
  layout = new QVBoxLayout(qHalf);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  layout->addWidget(qFields);
  layout->addSpacing(10);
  layout->addWidget(new FFuQtNotesLabel());
  layout->addWidget(static_cast<FFuQtLabel*>(notesText));
  layout->addSpacing(5);
  layout->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons));
  QWidget* qFull = new QWidget();
  layout = new QHBoxLayout(qFull);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qHalf,1);
  layout->addStretch(1);

  layout = new QVBoxLayout(this);
  layout->addWidget(static_cast<FFuQtTable*>(tableMain),1);
  layout->addWidget(qButtons);
  layout->addWidget(qFull);
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
