// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FuiQtRDBSelector.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtItemsListViews.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"


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
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,title,name,Qt::WindowStaysOnTopHint)
{
  this->lvRes = new FuiQtSimModelRDBListView(this,"SimModelRDBListView");
  this->lvPos = new FuiQtRDBListView(this,"RDBListView");
  this->resLabel = new FFuQtLabel(this);
  this->posLabel = new FFuQtLabel(this);
  this->notesImage = new FFuQtLabel(this);
  this->notesLabel = new FFuQtLabel(this);
  this->notesText = new FFuQtLabel(this);
  this->dialogButtons = new FFuQtDialogButtons(this);

  this->initWidgets();
}
