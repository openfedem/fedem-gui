// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FuiCtrlGridAttributes.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuPushButton.H"


void FuiCtrlGridAttributes::initWidgets()
{
  myGridToggleButton->setLabel("Grid On/Off");
  myGridToggleButton->setToggleCB(FFaDynCB1M(FuiCtrlGridAttributes,
					     this,onToggeled,bool));
  myGXField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myGXField->setDoubleDisplayMode(FFuIOField::DECIMAL,2,1);
  myGXField->setAcceptedCB(FFaDynCB1M(FuiCtrlGridAttributes,
				      this,onDoubleChanged,double));

  myGYField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myGYField->setDoubleDisplayMode(FFuIOField::DECIMAL,2,1);
  myGYField->setAcceptedCB(FFaDynCB1M(FuiCtrlGridAttributes,
				      this,onDoubleChanged,double));

  mySnapToggleButton->setLabel("Snap On/Off");
  mySnapToggleButton->setToggleCB(FFaDynCB1M(FuiCtrlGridAttributes,
					     this,onToggeled,bool));
  mySXField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  mySXField->setDoubleDisplayMode(FFuIOField::DECIMAL,2,1);
  mySXField->setAcceptedCB(FFaDynCB1M(FuiCtrlGridAttributes,
				      this,onDoubleChanged,double));
  mySYField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  mySYField->setDoubleDisplayMode(FFuIOField::DECIMAL,2,1);
  mySYField->setAcceptedCB(FFaDynCB1M(FuiCtrlGridAttributes,
				      this,onDoubleChanged,double));

  myCloseButton->setLabel("Close");
  myCloseButton->setActivateCB(FFaDynCB0M(FFuComponentBase,this,popDown));

  FFuUAExistenceHandler::invokeCreateUACB(this);
}


void FuiCtrlGridAttributes::setUIValues(const FFuaUIValues* vals)
{
  const FuaCtrlGridValues* newVals = static_cast<const FuaCtrlGridValues*>(vals);
  myGridToggleButton->setValue(newVals->gridOn);
  myGXField->setValue(newVals->gridXSpacing);
  myGYField->setValue(newVals->gridYSpacing);
  mySnapToggleButton->setValue(newVals->snapOn);
  mySXField->setValue(newVals->snapXSpacing);
  mySYField->setValue(newVals->snapYSpacing);
}


void FuiCtrlGridAttributes::onDoubleChanged(double)
{
  FuaCtrlGridValues newVals;

  newVals.gridOn = myGridToggleButton->getValue();
  newVals.gridXSpacing = myGXField->getDouble();
  newVals.gridYSpacing = myGYField->getDouble();
  newVals.snapOn = mySnapToggleButton->getValue();
  newVals.snapXSpacing = mySXField->getDouble();
  newVals.snapYSpacing = mySYField->getDouble();

  this->invokeSetAndGetDBValuesCB(&newVals);
  this->setUIValues(&newVals);
}
