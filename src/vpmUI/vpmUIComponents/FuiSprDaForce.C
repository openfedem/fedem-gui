// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiSprDaForce.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuToggleButton.H"


void FuiSprDaForce::initWidgets()
{
  this->setAsSpring(true);
  this->setSensitivity(true);

  myFunctionField->setBehaviour(FuiQueryInputField::REF_NUMBER);
  myFunctionField->setChangedCB(FFaDynCB1M(FuiSprDaForce,this,onQIFieldChanged,
					   FuiQueryInputField*));

  myScaleField->setBehaviour(FuiQueryInputField::REF_NONE);
  myScaleField->setChangedCB(FFaDynCB1M(FuiSprDaForce,this,onQIFieldChanged,
					 FuiQueryInputField*));

  myDmpToggle->setLabel("Use deformational velocity");
  myDmpToggle->setToggleCB(FFaDynCB1M(FuiSprDaForce,this,onBoolChanged,bool));
}


void FuiSprDaForce::setAsSpring(bool yesOrNo)
{
  if (yesOrNo == IAmSpringUI) return;

  IAmSpringUI = yesOrNo;

  if (IAmSpringUI) {
    this->setLabel("Spring properties");
    myFunctionLabel->setLabel("Stiffness");
    myFunctionField->setToolTip("Specify a constant spring stiffness,\n"
				"or select a stiffness/force-deflection function\n"
				"or an Advanced spring characteristics property");
    myScaleField->setToolTip("Optional scaling function for the spring stiffness and force.\n"
			     "Note: This should not be a function of the spring deflection or length");
    myDmpToggle->popDown();
  }
  else {
    this->setLabel("Damper properties");
    myFunctionLabel->setLabel("Damping coefficient");
    myFunctionField->setToolTip("Specify a constant damping coefficient,\n"
				"or select a coefficient/force-velocity function");
    myScaleField->setToolTip("Optional scaling function for the damping coefficient and force.\n"
			     "Note: This should not be a function of the damper velocity or length");
    myDmpToggle->popUp();
  }
}


void FuiSprDaForce::setChangedCB(const FFaDynCB0& aDynCB)
{
  myChangedCB = aDynCB;
}


void FuiSprDaForce::setValuesChangedCB(const FFaDynCB1<FuiSprDaForceValues&>& aDynCB)
{
  myValuesChangedCB = aDynCB;
}


//
// Setting and Getting :
//
//////////////////////////////////

void FuiSprDaForce::setCBs(const FuiSprDaForceValues& values)
{
  myFunctionField->setButtonCB(values.myFunctionQIFieldButtonCB);
  myScaleField->setButtonCB(values.myEngineQIFieldButtonCB);
}


void FuiSprDaForce::buildDynamicWidgets(const FuiSprDaForceValues& values)
{
  if (IAmSpringUI) return;

  if (values.showDefDamper) {
    myDmpToggle->popUp();
    myDmpToggle->setSensitivity(IAmSensitive && values.showDefDamper > 0);
  }
  else
    myDmpToggle->popDown();
}


void FuiSprDaForce::setValues(const FuiSprDaForceValues& values)
{
  myFunctionField->setValue(values.constFunction);
  myFunctionField->setQuery(values.functionQuery);
  myFunctionField->setSelectedRef(values.selectedFunction);

  myScaleField->setQuery(values.engineQuery);
  myScaleField->setSelectedRef(values.selectedScaleEngine);

  myDmpToggle->setValue(values.isDefDamper);

  buildDynamicWidgets(values);
}


void FuiSprDaForce::getValues(FuiSprDaForceValues& values)
{
  values.isConstant = myFunctionField->isAConstant();
  values.constFunction = myFunctionField->getValue();
  values.selectedFunction = myFunctionField->getSelectedRef();

  values.selectedScaleEngine = myScaleField->getSelectedRef();

  values.isDefDamper = myDmpToggle->getValue();
}


//
// Internal Callback Forwarding :
//
//////////////////////////////////

void FuiSprDaForce::onQIFieldChanged(FuiQueryInputField*)
{
  this->onBoolChanged(true);
}


void FuiSprDaForce::onBoolChanged(bool)
{
  FuiSprDaForceValues values;
  this->getValues(values);
  myValuesChangedCB.invoke(values);

  myChangedCB.invoke();
}


//
// Setting Up :
//
//////////////////////////////////

void FuiSprDaForce::setSensitivity(bool isSensitive)
{
  myFunctionField->setSensitivity(isSensitive);
  myScaleField->setSensitivity(isSensitive);
  myDmpToggle->setSensitivity(isSensitive);

  IAmSensitive = isSensitive;
}
