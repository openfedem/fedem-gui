// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiSimpleLoad.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"


void FuiSimpleLoad::initWidgets()
{
  myEngineField->setToolTip("Sets the magnitude as a constant value, or\n"
    "as a function (with the constant value added).");
  myEngineField->useAddedConstValue = true; // shows const value as added to function
  myEngineField->setBehaviour(FuiQueryInputField::REF_NUMBER);
  myEngineField->setChangedCB(FFaDynCB1M(FuiSimpleLoad,this,onQIFieldChanged,
					 FuiQueryInputField*));
}


void FuiSimpleLoad::setChangedCB(const FFaDynCB0& aDynCB)
{
  myChangedCB = aDynCB;
}


void FuiSimpleLoad::setValuesChangedCB(const FFaDynCB1<FuiSimpleLoadValues&>& aDynCB)
{
  myValuesChangedCB = aDynCB;
}


void FuiSimpleLoad::setCBs(const FuiSimpleLoadValues& values)
{
  myEngineField->setButtonCB(values.myEngineQIFieldButtonCB);
}


void FuiSimpleLoad::setValues(const FuiSimpleLoadValues& values)
{
  myEngineField->setValue(values.constValue);
  myEngineField->setQuery(values.engineQuery);
  myEngineField->setSelectedRef(values.selectedEngine);
}


void FuiSimpleLoad::getValues(FuiSimpleLoadValues& values)
{
  values.isConstant = myEngineField->isAConstant();
  values.constValue = myEngineField->getValue();
  values.selectedEngine = myEngineField->getSelectedRef();
}


void FuiSimpleLoad::onQIFieldChanged(FuiQueryInputField*)
{
  this->onValuesChanged();
}


void FuiSimpleLoad::onValuesChanged()
{
  FuiSimpleLoadValues values;
  this->getValues(values);
  myValuesChangedCB.invoke(values);
  myChangedCB.invoke();
}


void FuiSimpleLoad::setSensitivity(bool isSensitive)
{
  myEngineField->setSensitivity(isSensitive);
}
