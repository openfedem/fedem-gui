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
  notesText = new FFuQtLabel();
  dialogButtons = new FFuQtDialogButtons();

  this->initWidgets();

  QBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(new QLabel("Existing Results"));
  layout->addWidget(static_cast<FuiQtSimModelRDBListView*>(lvRes),3);
  layout->addWidget(new QLabel("Possible Results"));
  layout->addWidget(static_cast<FuiQtRDBListView*>(lvPos),1);
  layout->addWidget(new FFuQtNotesLabel());
  layout->addWidget(static_cast<FFuQtLabel*>(notesText));
  layout->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons));
}
