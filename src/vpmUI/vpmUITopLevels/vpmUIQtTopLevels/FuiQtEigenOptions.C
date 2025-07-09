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

#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtListView.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtFileBrowseField.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtEigenOptions.H"

//----------------------------------------------------------------------------

FuiEigenOptions* FuiEigenOptions::create(int xpos, int ypos,
					 int width, int height,
					 const char* title,
					 const char* name)
{
  return new FuiQtEigenOptions(xpos,ypos,width,height,title,name);
}
//----------------------------------------------------------------------------

FuiQtEigenOptions::FuiQtEigenOptions(int xpos, int ypos,
				     int width, int height,
				     const char* title,
				     const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,title,name)
{
  modeMenu = new FFuQtOptionMenu();
  timeMenu = new FFuQtOptionMenu();

  addButton = new FFuQtPushButton();
  deleteButton = new FFuQtPushButton();

  selectedModesView = new FFuQtListView();

  autoVTFToggle = new FFuQtToggleButton();
  autoVTFField  = new FFuQtFileBrowseField(NULL);

  dialogButtons = new FFuQtDialogButtons();

  this->initWidgets();

  QWidget* qModeSelector = new QWidget();
  QGridLayout* gl = new QGridLayout(qModeSelector);
  gl->setContentsMargins(0,0,0,0);
  gl->setHorizontalSpacing(20);
  gl->addWidget(new QLabel("Mode:"),0,0);
  gl->addWidget(new QLabel("Time:"),0,1);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(modeMenu),1,0);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(timeMenu),1,1);

  QWidget* qAddButton = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qAddButton);
  layout->setContentsMargins(0,0,0,0);
  layout->addStretch(1);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(addButton));

  QWidget* qDeleteButton = new QWidget();
  layout = new QHBoxLayout(qDeleteButton);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("List of mode shapes to recover"));
  layout->addStretch(1);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(deleteButton));

  layout = new QVBoxLayout(this);
  layout->addWidget(new QLabel("Select mode shape to recover"));
  layout->addWidget(qModeSelector);
  layout->addWidget(qAddButton);
  layout->addWidget(new FFuQtSeparator());
  layout->addWidget(qDeleteButton);
  layout->addWidget(dynamic_cast<FFuQtListView*>(selectedModesView));
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(autoVTFToggle));
  layout->addWidget(static_cast<FFuQtFileBrowseField*>(autoVTFField));
  layout->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons),
                    0, Qt::AlignBottom);
}
