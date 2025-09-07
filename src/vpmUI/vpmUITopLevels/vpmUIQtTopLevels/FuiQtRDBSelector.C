// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QVBoxLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtItemsListViews.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtRDBSelector.H"


FuiRDBSelector* FuiRDBSelector::create(int xpos, int ypos,
				       int width, int height,
				       const char* title,
				       const char* name)
{
  return new FuiQtRDBSelector(xpos,ypos,width,height,title,name);
}


FuiQtRDBSelector::FuiQtRDBSelector(int xpos, int ypos,
				   int width, int height,
				   const char* title,
				   const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,
                       title,name,Qt::WindowStaysOnTopHint)
{
  lvRes = new FuiQtSimModelRDBListView(NULL,"SimModelRDBListView");
  lvPos = new FuiQtRDBListView(NULL,"RDBListView");
  notes = new FFuQtNotes();
  dialogButtons = new FFuQtDialogButtons();

  this->initWidgets();

  QBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(new QLabel("Existing Results"));
  layout->addWidget(lvRes->getQtWidget(),3);
  layout->addWidget(new QLabel("Possible Results"));
  layout->addWidget(lvPos->getQtWidget(),1);
  layout->addSpacing(5);
  layout->addWidget(notes->getQtWidget());
  layout->addWidget(dialogButtons->getQtWidget());
}
