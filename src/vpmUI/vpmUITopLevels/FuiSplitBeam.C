// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiSplitBeam.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuDialogButtons.H"


Fmd_SOURCE_INIT(FUI_SPLITBEAM, FuiSplitBeam, FFuModalDialog);


FuiSplitBeam::FuiSplitBeam()
{
  Fmd_CONSTRUCTOR_INIT(FuiSplitBeam);
}


void FuiSplitBeam::initWidgets()
{
  myNumField->setLabel("Number of segments to split beam into");
  myNumField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);

  myDialogButtons->setButtonLabel(FFuDialogButtons::LEFTBUTTON,"OK");
  myDialogButtons->setButtonLabel(FFuDialogButtons::RIGHTBUTTON,"Cancel");
  myDialogButtons->setButtonClickedCB(FFaDynCB1M(FuiSplitBeam,this,
                                                 onDialogButtonClicked,int));
}


void FuiSplitBeam::onDialogButtonClicked(int button)
{
  myClickedCB.invoke(button);
}


int FuiSplitBeam::getValue()
{
  return myNumField->myField->getInt();
}


FuiSplitBeam* FuiSplitBeam::getUI(bool onScreen)
{
  FuiSplitBeam* spb = NULL;
  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiSplitBeam::getClassTypeID());
  if (!uic)
    uic = spb = FuiSplitBeam::create(100,100,300,100);
  else
    spb = dynamic_cast<FuiSplitBeam*>(uic);

  uic->manage(onScreen,true);

  return spb;
}
