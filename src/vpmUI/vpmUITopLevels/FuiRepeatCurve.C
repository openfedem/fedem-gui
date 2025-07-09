// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiRepeatCurve.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuDialogButtons.H"


Fmd_SOURCE_INIT(FUI_REPEATCURVE, FuiRepeatCurve, FFuModalDialog);


FuiRepeatCurve::FuiRepeatCurve()
{
  Fmd_CONSTRUCTOR_INIT(FuiRepeatCurve);
}


void FuiRepeatCurve::initWidgets()
{
  myLabel->setLabel("Enter object ID range to generate curves for");
  myFromField->setLabel("From");
  myFromField->setLabelWidth(40);
  myToField->setLabel("To");
  myToField->setLabelWidth(40);

  myFromField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  myToField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);

  myDialogButtons->setButtonLabel(FFuDialogButtons::LEFTBUTTON,"OK");
  myDialogButtons->setButtonLabel(FFuDialogButtons::RIGHTBUTTON,"Cancel");
  myDialogButtons->setButtonClickedCB(FFaDynCB1M(FuiRepeatCurve,this,
                                                 onDialogButtonClicked,int));
}


void FuiRepeatCurve::onDialogButtonClicked(int button)
{
  myClickedCB.invoke(button);
}


void FuiRepeatCurve::getValues(int& fromID, int& toID)
{
  fromID = myFromField->myField->getInt();
  toID   = myToField->myField->getInt();
}


void FuiRepeatCurve::setValues(int fromID, int toID)
{
  myFromField->myField->setValue(fromID);
  myToField->myField->setValue(toID);
}


void FuiRepeatCurve::setLabel(const std::string& newLabel)
{
  myLabel->setLabel(newLabel);
}


FuiRepeatCurve* FuiRepeatCurve::getUI(bool onScreen)
{
  FuiRepeatCurve* rep = NULL;
  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiRepeatCurve::getClassTypeID());
  if (!uic)
    uic = rep = FuiRepeatCurve::create(200,200,200,200);
  else
    rep = dynamic_cast<FuiRepeatCurve*>(uic);

  uic->manage(onScreen,true);

  return rep;
}
