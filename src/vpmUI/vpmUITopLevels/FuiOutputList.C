// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

/*!
  \class FuiOutputList FuiOutputList.H

  \brief A scrolled text window with hide and clear

  \author Jacob Storen

  This is a window used to display text output to the user.

  \date 13.08.99
*/

#include "vpmUI/vpmUITopLevels/FuiOutputList.H"
#include "FFuLib/FFuMemo.H"


Fmd_SOURCE_INIT(FUI_OUTPUTLIST, FuiOutputList, FFuTopLevelShell);


FuiOutputList::FuiOutputList()
{
  Fmd_CONSTRUCTOR_INIT(FuiOutputList);
}


void FuiOutputList::initWidgets()
{
  myMemo->setNoWordWrap();
  myMemo->setEditable(false);
  myMemo->scrollToEnd();

  //create ui's UA object
  FFuUAExistenceHandler::invokeCreateUACB(this);
}

void FuiOutputList::placeWidgets(int width,int height)
{
  myMemo->setEdgeGeometry(0,width,0,height);
}

void FuiOutputList::addText(const char * text)
{
  myMemo->setCursorPos(FFuMemo::MOVE_END, false);
  myMemo->insertText((const char*)text);
  myMemo->scrollToEnd();
}

void FuiOutputList::clearList()
{
  myMemo->clearText();
}

void FuiOutputList::copySelected()
{
  myMemo->copyToClipboard();
}

void FuiOutputList::selectAll()
{
  myMemo->selectAllText();
}

bool FuiOutputList::onClose()
{
  this->invokeFinishedCB();
  return false;
}


bool FuiOutputList::hasText()
{
  return !myMemo->getText().empty();
}

bool FuiOutputList::hasSelectedText()
{
  return myMemo->hasSelection();
}
