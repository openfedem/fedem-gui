// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QVBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"
#include "FFuLib/FFuQtComponents/FFuQtTable.H"

#include "FuiQtLinkRamSettings.H"


FuiLinkRamSettings* FuiLinkRamSettings::create(int xpos, int ypos,
                                               int width,int height,
                                               const char* title,
                                               const char* name)
{
  return new FuiQtLinkRamSettings(xpos,ypos,width,height,title,name);
}


FuiQtLinkRamSettings::FuiQtLinkRamSettings(int xpos, int ypos,
                                           int width,int height,
                                           const char* title,
                                           const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,title,name)
{
  myTable = new FFuQtTable(NULL,false,true);
  dialogButtons = new FFuQtDialogButtons(NULL,false);

  this->initWidgets();

  QLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(5,0,5,10);
  layout->addWidget(static_cast<FFuQtTable*>(myTable));
  layout->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons));
}


void FuiQtLinkRamSettings::resizeEvent(QResizeEvent* e)
{
  this->QWidget::resizeEvent(e);

  myTable->updateColumnWidths();
}
