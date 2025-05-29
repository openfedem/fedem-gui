// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtCreateBeamstringPair.H"


FuiCreateBeamstringPair* FuiCreateBeamstringPair::create(int xpos, int ypos,
							 int width, int height,
							 const char* title,
							 const char* name)
{
  return new FuiQtCreateBeamstringPair(xpos,ypos,width,height,title,name);
}


FuiQtCreateBeamstringPair::FuiQtCreateBeamstringPair(int xpos, int ypos,
						     int width, int height,
						     const char* title,
						     const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,
                       title,name,Qt::MSWindowsFixedSizeDialogHint)
{
  headerImage            = new FFuQtLabel();
  beamstringMenu1        = new FuiQtQueryInputField(NULL);
  beamstringMenu2        = new FuiQtQueryInputField(NULL);
  stiffnessFunctionMenu  = new FuiQtQueryInputField(NULL);
  useRadialSpringsToggle = new FFuQtToggleButton();
  notesText              = new FFuQtLabel();
  createButton           = new FFuQtPushButton();
  eraseButton            = new FFuQtPushButton();
  closeButton            = new FFuQtPushButton();
  helpButton             = new FFuQtPushButton();

  this->initWidgets();

  QWidget* qMenu1 = new QWidget();
  QBoxLayout* layout = new QVBoxLayout(qMenu1);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  layout->addWidget(new QLabel("First beamstring"));
  layout->addWidget(static_cast<FuiQtQueryInputField*>(beamstringMenu1));

  QWidget* qMenu2 = new QWidget();
  layout = new QVBoxLayout(qMenu2);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  layout->addWidget(new QLabel("Second beamstring"));
  layout->addWidget(static_cast<FuiQtQueryInputField*>(beamstringMenu2));

  QWidget* qMenu3 = new QWidget();
  layout = new QVBoxLayout(qMenu3);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  layout->addWidget(new QLabel("Contact stiffness function"));
  layout->addWidget(static_cast<FuiQtQueryInputField*>(stiffnessFunctionMenu));

  QWidget* qMenus = new QWidget();
  QGridLayout* l2 = new QGridLayout(qMenus);
  l2->setContentsMargins(10,5,10,0);
  l2->setHorizontalSpacing(20);
  l2->setRowMinimumHeight(0,45);
  l2->addWidget(qMenu1,0,0);
  l2->addWidget(qMenu2,0,1);
  l2->addWidget(qMenu3,0,2);
  l2->addWidget(dynamic_cast<FFuQtToggleButton*>(useRadialSpringsToggle),1,2);

  QWidget* qNotes = new QWidget();
  layout = new QVBoxLayout(qNotes);
  layout->setContentsMargins(10,0,10,0);
  layout->setSpacing(0);
  layout->addWidget(new FFuQtNotesLabel());
  layout->addWidget(static_cast<FFuQtLabel*>(notesText));

  int hspace = headerImage->getWidthHint()/10;
  QWidget* qButtons = new QWidget();
  layout = new QHBoxLayout(qButtons);
  layout->setContentsMargins(10,0,10,0);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(createButton));
  layout->addSpacing(hspace);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(eraseButton));
  layout->addSpacing(hspace);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(closeButton));
  layout->addSpacing(hspace);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(helpButton));

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,10);
  layout->addWidget(static_cast<FFuQtLabel*>(headerImage));
  layout->addWidget(qMenus);
  layout->addWidget(qNotes);
  layout->addWidget(new FFuQtSeparator());
  layout->addWidget(qButtons);
}
