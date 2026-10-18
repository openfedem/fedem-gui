// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiSpringDefCalc.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuRadioButton.H"
#include "FFuLib/FFuLabel.H"


FuiSpringDefCalc::FuiSpringDefCalc()
{
  IAmSensitive = true;
  IAmBlockingVariable = false;
  IAmDeflection = true;
  IAmAngular = false;
  IAmSpring = false;
}


void FuiSpringDefCalc::initWidgets()
{
  myVariableField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myVariableField->setDoubleDisplayMode(FFuIOField::AUTO,8,1);
  myVariableField->setAcceptedCB(FFaDynCB1M(FuiSpringDefCalc,this,
					    onDoubleChanged,double));

  myILengthField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myILengthField->setDoubleDisplayMode(FFuIOField::AUTO,8,1);
  myILengthField->setAcceptedCB(FFaDynCB1M(FuiSpringDefCalc,this,
					   onDoubleChanged,double));

  myIDeflField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myIDeflField->setDoubleDisplayMode(FFuIOField::AUTO,8,1);
  myIDeflField->setAcceptedCB(FFaDynCB1M(FuiSpringDefCalc,this,
					 onDoubleChanged,double));

  myILDToggleGroup.insert(myILToggle);
  myILDToggleGroup.insert(myIDToggle);
  myILDToggleGroup.setExclusive(true);
  myILDToggleGroup.setGroupToggleCB(FFaDynCB2M(FuiSpringDefCalc,this,
					       onIntBoolChanged,int,bool));

  myLengthEngineField->setBehaviour(FuiQueryInputField::REF_NONE);
  myLengthEngineField->setRefSelectedCB(FFaDynCB1M(FuiSpringDefCalc,this,
						   onEngineChanged,int));

  this->setLabels(false,true);
}


void FuiSpringDefCalc::setCBs(const FuiSpringDefCalcValues& values)
{
  myLengthEngineField->setButtonCB(values.myQIFieldButtonCB);
}


void FuiSpringDefCalc::buildDynamicWidgets(const FuiSpringDefCalcValues& values)
{
  IAmBlockingVariable = !values.variableIsEditable;
  if (IAmSensitive)
    myVariableField->setSensitivity(values.variableIsEditable);

  this->setLabels(values.useAngularLabels,values.useSpringLabels,
                  values.selectedLengthEngine);

  if (values.fixedOrFree) {
    myILengthField->popDown();
    myIDeflField->popDown();
    myILToggle->popDown();
    myIDToggle->popDown();
    myLengthChangeLabel->popDown();
    myLengthEngineField->popDown();
  }
  else {
    myILengthField->popUp();
    myIDeflField->popUp();
    myILToggle->popUp();
    myIDToggle->popUp();
    myLengthChangeLabel->popUp();
    myLengthEngineField->popUp();
    myLengthEngineField->setQuery(values.engineQuery);
  }
}


void FuiSpringDefCalc::setValues(const FuiSpringDefCalcValues& values)
{
  this->setLabels(values.useAngularLabels,values.useSpringLabels,
                  values.selectedLengthEngine);

  myVariableField->setValue(values.variable);

  double initLength = 0.0;
  double initDeflection = 0.0;
  if (values.useAsDeflection) {
    myIDToggle->setValue(true);
    initLength = values.variable - values.initLengtOrDeflection;
    initDeflection = values.initLengtOrDeflection;
  }
  else {
    myILToggle->setValue(true);
    initLength = values.initLengtOrDeflection;
    if (IAmDeflection)
      initDeflection = values.variable - values.initLengtOrDeflection;
    else // prescribed velocity or acceleration
      initDeflection = 0.0;
  }

  myIDToggle->setSensitivity(IAmSensitive && IAmDeflection);
  myILengthField->setSensitivity(IAmSensitive && !values.useAsDeflection);
  myIDeflField->setSensitivity(IAmSensitive && values.useAsDeflection);

  myILengthField->setValue(initLength);
  myIDeflField->setValue(initDeflection);

  myLengthEngineField->setSelectedRef(values.selectedLengthEngine);
}


void FuiSpringDefCalc::getValues(FuiSpringDefCalcValues& values)
{
  values.variable = myVariableField->getDouble();

  values.useAsDeflection = myIDToggle->getValue();
  if (values.useAsDeflection)
    values.initLengtOrDeflection = myIDeflField->getDouble();
  else
    values.initLengtOrDeflection = myILengthField->getDouble();

  values.selectedLengthEngine = myLengthEngineField->getSelectedRef();
}


void FuiSpringDefCalc::setSensitivity(bool isSensitive)
{
  IAmSensitive = isSensitive;

  myIDToggle->setSensitivity(isSensitive && IAmDeflection);
  myILToggle->setSensitivity(isSensitive);
  myILengthField->setSensitivity(isSensitive && !myIDToggle->getValue());
  myIDeflField->setSensitivity(isSensitive && myIDToggle->getValue());

  myLengthEngineField->setSensitivity(isSensitive);

  if (!IAmBlockingVariable)
    myVariableField->setSensitivity(isSensitive);
}


void FuiSpringDefCalc::setLabels(char isAngle, bool isSpring, bool isVarying)
{
  if (isAngle == IAmAngular && isSpring == IAmSpring && isVarying == IAmVarying)
    return;

  IAmDeflection = isAngle == 0 || isAngle == 1;
  IAmAngular = isAngle;
  IAmSpring = isSpring;
  IAmVarying = isVarying;

  auto&& makeLabel = [isVarying](const char* text)
  {
    static std::string label;
    label = std::string(isVarying ? "Initial " : "Constant ") + text;
    return label.c_str();
  };

  if (IAmSpring) // Spring controlled motion
  {
    if (IAmAngular > 0) {
      this->setLabel("Stress free angle control");
      myVariableLabel->setLabel("Angle in model");
      myILToggle->setLabel(makeLabel("stress free angle"));
      myIDToggle->setLabel(makeLabel("deflection"));
      myLengthChangeLabel->setLabel("Stress free angle change");
    }
    else {
      this->setLabel("Stress free length control");
      myVariableLabel->setLabel("Length in model");
      myILToggle->setLabel(makeLabel("stress free length"));
      myIDToggle->setLabel(makeLabel("deflection"));
      myLengthChangeLabel->setLabel("Stress free length change");
    }
  }
  else         // Prescribed motion
  {
    if (IAmAngular > 0) {
      this->setLabel("Rotation control");
      myVariableLabel->setLabel("Angle in model");
      myIDToggle->setLabel(makeLabel("rotation"));
    }
    else {
      this->setLabel("Translation control");
      myVariableLabel->setLabel("Length in model");
      myIDToggle->setLabel(makeLabel("displacement"));
    }

    switch (IAmAngular) {
    case 0:
      myILToggle->setLabel(makeLabel("length"));
      myLengthChangeLabel->setLabel("Length change");
      break;
    case 1:
      myILToggle->setLabel(makeLabel("angle"));
      myLengthChangeLabel->setLabel("Angle change");
      break;
    case  2:
    case -1:
      myILToggle->setLabel(makeLabel("velocity"));
      myLengthChangeLabel->setLabel("Velocity change");
      break;
    case  3:
    case -2:
      myILToggle->setLabel(makeLabel("acceleration"));
      myLengthChangeLabel->setLabel("Acceleration change");
      break;
    }
  }
}
