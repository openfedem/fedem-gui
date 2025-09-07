// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiTriadDOF.H"
#include "vpmUI/vpmUIComponents/FuiMotionType.H"
#include "vpmUI/vpmUIComponents/FuiVariableType.H"
#ifdef FT_HAS_FREQDOMAIN
#include "FFuLib/FFuToggleButton.H"
#endif
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuIOField.H"


FuiTriadDOF::FuiTriadDOF()
{
  motionType = NULL;
  variableType = NULL;
  loadField = NULL;
  initialVel = NULL;
  freqToggle = NULL;
}


void FuiTriadDOF::initWidgets()
{
  initialVel->setLabel("Initial velocity");
  initialVel->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
#ifdef FT_HAS_FREQDOMAIN
  freqToggle->setLabel("Apply in frequency domain");
#endif
}


void FuiTriadDOF::setChangedCB(const FFaDynCB0& aDynCB)
{
  motionType->setChangedCB(aDynCB);
  loadField->setChangedCB(aDynCB);
  variableType->setChangedCB(aDynCB);
}


void FuiTriadDOF::getValues(FuiTriadDOFValues& values) const
{
  values.myMotionType = motionType->getValue();
  if (values.myMotionType == PRESCRIBED)
    values.myMotionType = PRESCRIBED_DISP + variableType->getValue();

  loadField->getValues(values.myLoadVals);
#ifdef FT_HAS_FREQDOMAIN
  values.freqDomain = freqToggle->getValue();
#endif
  values.myInitVel = initialVel->getValue();
}


void FuiTriadDOF::setValues(const FuiTriadDOFValues& values,
                            bool canSwitchMotionType,
                            bool isSensitive)
{
  int mType = values.myMotionType;
  if (mType >= PRESCRIBED_DISP) {
    variableType->setValue(mType-PRESCRIBED_DISP);
    mType = PRESCRIBED;
  }

  switch (mType) {
  case FIXED:
    variableType->popDown();
    loadField->popDown();
#ifdef FT_HAS_FREQDOMAIN
    freqToggle->popDown();
#endif
    initialVel->popDown();
    break;

  case PRESCRIBED:
    variableType->popUp();
    loadField->setLabel("Motion magnitude");
    loadField->popUp();
#ifdef FT_HAS_FREQDOMAIN
    freqToggle->popUp();
#endif
    initialVel->popUp();
    break;

  default:
    variableType->popDown();
    loadField->setLabel("Load magnitude");
    loadField->popUp();
#ifdef FT_HAS_FREQDOMAIN
    freqToggle->popUp();
#endif
    initialVel->popUp();
  }

  motionType->setValue(mType);
  motionType->setSensitivity(isSensitive && canSwitchMotionType);
  loadField->setValues(values.myLoadVals);
#ifdef FT_HAS_FREQDOMAIN
  freqToggle->setValue(values.freqDomain);
  freqToggle->setSensitivity(isSensitive && !values.myLoadVals.isConstant);
#endif
  initialVel->setValue(values.myInitVel);
}
