// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiSpringChar.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuToggleButton.H"
#include "vpmDB/FmMathFuncBase.H"


FuiSpringChar::FuiSpringChar() : IAmSensitive(false)
{
  mySpringFunctionDescription = NULL;

  mySpringFunctionField = NULL;
  myYieldForceMaxEngineField = myYieldForceMinEngineField = NULL;

  myToggles.fill(NULL);
  myFields.fill(NULL);
}


void FuiSpringChar::initWidgets()
{
  myToggles[MAX_DEFL]->setLabel("Max Deflection");
  myToggles[MIN_DEFL]->setLabel("Min Deflection");
  myToggles[MAX_FORCE]->setLabel("Max Force");
  myToggles[MIN_FORCE]->setLabel("Min Force");
  myToggles[MAX_YIELD_FORCE]->setLabel("Max Yield Force");
  myToggles[MIN_YIELD_FORCE]->setLabel("Min Yield Force");
  myToggles[MAX_YIELD_DEFL]->setLabel("Max Yield Deflection");

  for (FFuToggleButton* button : myToggles)
    if (button)
      button->setToggleCB(FFaDynCB1M(FuiSpringChar,this,onBoolChanged,bool));

  for (FFuIOField* fld : myFields)
    if (fld)
    {
      fld->setInputCheckMode(FFuIOField::DOUBLECHECK);
      fld->setDoubleDisplayMode(FFuIOField::AUTO,8,1);
      fld->setAcceptedCB(FFaDynCB1M(FuiSpringChar,this,onValuesChanged,double));
    }

  std::array<FuiQueryInputField*,3> queryFields = {
    mySpringFunctionField,
    myYieldForceMaxEngineField,
    myYieldForceMinEngineField
  };

  for (FuiQueryInputField* fld : queryFields)
  {
    fld->setBehaviour(FuiQueryInputField::REF_NUMBER);
    fld->setChangedCB(FFaDynCB1M(FuiSpringChar,this,
                                 onQIFieldChanged,FuiQueryInputField*));
  }
}


void FuiSpringChar::setChangedCB(const FFaDynCB0& aDynCB)
{
  myChangedCB = aDynCB;
}


void FuiSpringChar::setValuesChangedCB(const FFaDynCB1<FuiSpringCharValues&>& aDynCB)
{
  myValuesChangedCB = aDynCB;
}


//
// Setting and getting :
//
//////////////////////////////////

void FuiSpringChar::setCBs(const FuiSpringCharValues& values)
{
  mySpringFunctionField->setButtonCB(values.springFunctionFieldButtonCB);
  myYieldForceMaxEngineField->setButtonCB(values.yieldForceFieldMaxButtonCB);
  myYieldForceMinEngineField->setButtonCB(values.yieldForceFieldMinButtonCB);
}


void FuiSpringChar::setValues(const FuiSpringCharValues& values)
{
  myToggles[MAX_DEFL]->setValue(values.useDeflectionMax);
  myToggles[MIN_DEFL]->setValue(values.useDeflectionMin);
  myToggles[MAX_FORCE]->setValue(values.useForceMax);
  myToggles[MIN_FORCE]->setValue(values.useForceMin);
  myToggles[MAX_YIELD_FORCE]->setValue(values.useYieldForceMax);
  myToggles[MIN_YIELD_FORCE]->setValue(values.useYieldForceMin);
  myToggles[MAX_YIELD_DEFL]->setValue(values.useYieldDeflectionMax);

  myFields[MAX_DEFL]->setValue(values.deflectionMax);
  myFields[MIN_DEFL]->setValue(values.deflectionMin);
  myFields[MAX_FORCE]->setValue(values.forceMax);
  myFields[MIN_FORCE]->setValue(values.forceMin);
  myFields[MAX_YIELD_DEFL]->setValue(values.yieldDeflectionMax);

  mySpringFunctionField->setValue(values.constantStiffness);
  mySpringFunctionField->setQuery(values.springFunctionQuery);
  mySpringFunctionField->setSelectedRef(values.springFunction);

  myYieldForceMaxEngineField->setValue(values.constantYieldForceMax);
  myYieldForceMaxEngineField->setQuery(values.yieldForceEngineQuery);
  myYieldForceMaxEngineField->setSelectedRef(values.yieldForceMaxEngine);

  myYieldForceMinEngineField->setValue(values.constantYieldForceMin);
  myYieldForceMinEngineField->setQuery(values.yieldForceEngineQuery);
  myYieldForceMinEngineField->setSelectedRef(values.yieldForceMinEngine);

  if (values.springFunction)
    switch (((FmMathFuncBase*)values.springFunction)->getFunctionUse()) {
    case FmMathFuncBase::SPR_TRA_STIFF:
      mySpringFunctionDescription->setLabel("Translational spring, Stiffness - deflection");
      break;
    case FmMathFuncBase::SPR_TRA_FORCE:
      mySpringFunctionDescription->setLabel("Translational spring, Force - deflection");
      break;
    case FmMathFuncBase::SPR_ROT_STIFF:
      mySpringFunctionDescription->setLabel("Rotational spring, Stiffness - rotation");
      break;
    case FmMathFuncBase::SPR_ROT_TORQUE:
      mySpringFunctionDescription->setLabel("Rotational spring, Torque - rotation");
      break;
    default:
      break;
    }
  else if (values.isTranslationalSpring)
    mySpringFunctionDescription->setLabel("Translation spring, Constant stiffness");
  else
    mySpringFunctionDescription->setLabel("Rotational spring, Constant stiffness");

  this->setSensitivity(IAmSensitive);
}


void FuiSpringChar::getValues(FuiSpringCharValues& values)
{
  values.useDeflectionMax      = myToggles[MAX_DEFL]->getValue();
  values.useDeflectionMin      = myToggles[MIN_DEFL]->getValue();
  values.useForceMax           = myToggles[MAX_FORCE]->getValue();
  values.useForceMin           = myToggles[MIN_FORCE]->getValue();
  values.useYieldForceMax      = myToggles[MAX_YIELD_FORCE]->getValue();
  values.useYieldForceMin      = myToggles[MIN_YIELD_FORCE]->getValue();
  values.useYieldDeflectionMax = myToggles[MAX_YIELD_DEFL]->getValue();

  values.deflectionMax         = myFields[MAX_DEFL]->getDouble();
  values.deflectionMin         = myFields[MIN_DEFL]->getDouble();
  values.forceMax              = myFields[MAX_FORCE]->getDouble();
  values.forceMin              = myFields[MIN_FORCE]->getDouble();
  values.yieldDeflectionMax    = myFields[MAX_YIELD_DEFL]->getDouble();
  values.constantStiffness     = mySpringFunctionField->getValue();
  values.springFunction        = mySpringFunctionField->getSelectedRef();
  values.constantYieldForceMax = myYieldForceMaxEngineField->getValue();
  values.yieldForceMaxEngine   = myYieldForceMaxEngineField->getSelectedRef();
  values.constantYieldForceMin = myYieldForceMinEngineField->getValue();
  values.yieldForceMinEngine   = myYieldForceMinEngineField->getSelectedRef();
}


//
// Internal callback forwarding :
//
//////////////////////////////////

void FuiSpringChar::onBoolChanged(bool)
{
  this->onValuesChanged();
}

void FuiSpringChar::onBoolChanged(int, bool)
{
  this->onValuesChanged();
}

void FuiSpringChar::onQIFieldChanged(FuiQueryInputField*)
{
  this->onValuesChanged();
}

void FuiSpringChar::onValuesChanged(double)
{
  FuiSpringCharValues values;
  this->getValues(values);
  myValuesChangedCB.invoke(values);
  myChangedCB.invoke();
}


//
// Setting up :
//
//////////////////////////////////

void FuiSpringChar::setSensitivity(bool isSensitive)
{
  mySpringFunctionField->setSensitivity(IAmSensitive = isSensitive);

  for (int i = 0; i < NUM_FIELDS; i++)
  {
    myToggles[i]->setSensitivity(isSensitive);
    if (myFields[i])
      myFields[i]->setSensitivity(isSensitive && myToggles[i]->getValue());
  }

  myYieldForceMaxEngineField->setSensitivity(isSensitive && myToggles[MAX_YIELD_FORCE]->getValue());
  myYieldForceMinEngineField->setSensitivity(isSensitive && myToggles[MIN_YIELD_FORCE]->getValue());
}
