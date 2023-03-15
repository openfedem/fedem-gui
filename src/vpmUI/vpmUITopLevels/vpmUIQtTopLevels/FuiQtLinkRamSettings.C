// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FuiQtLinkRamSettings.H"

#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"
#include "FFuLib/FFuQtComponents/FFuQtTable.H"


FuiLinkRamSettings* FuiLinkRamSettings::create(int xpos, int ypos,
                                               int width,int height,
                                               const char* title,
                                               const char* name)
{
  return new FuiQtLinkRamSettings(0,xpos,ypos,width,height,title,name);
}


FuiQtLinkRamSettings::FuiQtLinkRamSettings(QWidget* parent,
					   int xpos, int ypos,
					   int width,int height,
					   const char* title,
					   const char* name)
  : FFuQtTopLevelShell(parent,xpos,ypos,width,height,title,name)
{
  myTable = new FFuQtTable(this, false, true);

  dialogButtons = new FFuQtDialogButtons(this);

  this->initWidgets();
}
