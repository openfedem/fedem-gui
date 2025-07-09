// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiMooringLine.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuDialogButtons.H"


Fmd_SOURCE_INIT(FUI_MOORINGLINE, FuiMooringLine, FFuModalDialog);


FuiMooringLine::FuiMooringLine()
{
  Fmd_CONSTRUCTOR_INIT(FuiMooringLine);
}


void FuiMooringLine::initWidgets()
{
  myNumField->setLabel("Number of elements in mooring line");
  myNumField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);

  myLengthField->setLabel("Total length of mooring line");
  myLengthField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  myDialogButtons->setButtonLabel(FFuDialogButtons::LEFTBUTTON,"OK");
  myDialogButtons->setButtonLabel(FFuDialogButtons::RIGHTBUTTON,"Cancel");
}


void FuiMooringLine::addElmType(const std::string& etype, bool clear)
{
  if (clear) myTypeMenu->clearOptions();
  myTypeMenu->addOption(etype.c_str());
}


int FuiMooringLine::getElmType() const
{
  return myTypeMenu->getSelectedOption();
}


int FuiMooringLine::getNumSeg() const
{
  return myNumField->myField->getInt();
}


double FuiMooringLine::getLength() const
{
  return myLengthField->getValue();
}
