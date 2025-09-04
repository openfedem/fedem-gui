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
  // Lambda function creating a pull-down selection menu with a label above.
  auto newMenu = [](const char* label)
  {
    return new FuiQtQueryInputField(NULL,label,true);
  };

  headerImage     = new FFuQtLabel();
  beamstringMenu1 = newMenu("First beamstring");
  beamstringMenu2 = newMenu("Second beamstring");
  stiffnessMenu   = newMenu("Contact stiffness function");
  beamstringNotes = new FFuQtNotes(NULL,"Beamstring",10,3);
  radialToggle    = new FFuQtToggleButton();
  createButton    = new FFuQtPushButton();
  eraseButton     = new FFuQtPushButton();
  closeButton     = new FFuQtPushButton();
  helpButton      = new FFuQtPushButton();

  this->initWidgets();

  QGridLayout* menuLayout = new QGridLayout();
  menuLayout->setContentsMargins(10,5,10,0);
  menuLayout->setHorizontalSpacing(20);
  menuLayout->setRowMinimumHeight(0,45);
  menuLayout->addWidget(beamstringMenu1->getQtWidget(),0,0);
  menuLayout->addWidget(beamstringMenu2->getQtWidget(),0,1);
  menuLayout->addWidget(stiffnessMenu->getQtWidget(),0,2);
  menuLayout->addWidget(radialToggle->getQtWidget(),1,2);

  int hspace = headerImage->getWidthHint()/10;
  QBoxLayout* buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(10,0,10,0);
  buttonLayout->addWidget(createButton->getQtWidget());
  buttonLayout->addSpacing(hspace);
  buttonLayout->addWidget(eraseButton->getQtWidget());
  buttonLayout->addSpacing(hspace);
  buttonLayout->addWidget(closeButton->getQtWidget());
  buttonLayout->addSpacing(hspace);
  buttonLayout->addWidget(helpButton->getQtWidget());

  QBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,10);
  layout->addWidget(headerImage->getQtWidget());
  layout->addLayout(menuLayout);
  layout->addStretch(1);
  layout->addWidget(beamstringNotes->getQtWidget());
  layout->addStretch(1);
  layout->addWidget(new FFuQtSeparator());
  layout->addLayout(buttonLayout);
}
