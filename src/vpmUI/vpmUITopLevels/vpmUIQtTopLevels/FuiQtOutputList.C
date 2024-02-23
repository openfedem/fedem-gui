// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

/*!
  \file FuiQtOutputList.C
  \author Jacob Storen
  \date 13.08.99
*/

#include "FFuLib/FFuQtComponents/FFuQtMemo.H"
#include "FuiQtOutputList.H"


FuiOutputList* FuiOutputList::create(int xpos, int ypos, int width, int height)
{
  return new FuiQtOutputList(xpos,ypos,width,height);
}


FuiQtOutputList::FuiQtOutputList(int xpos, int ypos, int width, int height,
                                 const char* title, const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,title,name,Qt::Dialog)
{
  FFuQtMemo* list = new FFuQtMemo(this,true);
  myMemo = list;

  QFont fnt("Courier",8);
  fnt.setStyleHint(QFont::Courier);
  list->setFont(fnt);

  this->initWidgets();
}
