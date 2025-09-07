// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiJointDOF.H"
#include "vpmUI/vpmUIComponents/FuiMotionType.H"
#include "vpmUI/vpmUIComponents/FuiVariableType.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuIOField.H"


FuiJointDOF::FuiJointDOF() : dofNo(-1)
{
  motionType = NULL;
  variableType = NULL;
  simpleLoad = NULL;
  springDC = NULL;
  springFS = damperFS = NULL;
  initialVel = NULL;
  freqToggle = NULL;
}


void FuiJointDOF::initWidgets()
{
  initialVel->setLabel("Initial velocity");
  initialVel->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
#ifdef FT_HAS_FREQDOMAIN
  freqToggle->setLabel("Apply in frequency domain");
#endif
  springFS->setAsSpring(true);
  damperFS->setAsSpring(false);
}


void FuiJointDOF::setChangedCB(const FFaDynCB0& aDynCB)
{
  simpleLoad->setChangedCB(aDynCB);
  variableType->setChangedCB(aDynCB);
  springDC->setChangedCB(aDynCB);
  springFS->setChangedCB(aDynCB);
  damperFS->setChangedCB(aDynCB);
}


void FuiJointDOF::getValues(FuiJointDOFValues& values) const
{
  values.myMotionType = motionType->getValue();
  if (values.myMotionType == PRESCRIBED)
    values.myMotionType = PRESCRIBED_DISP + variableType->getValue();

  simpleLoad->getValues(values.myLoadVals);
#ifdef FT_HAS_FREQDOMAIN
  values.freqDomain = freqToggle->getValue();
#endif
  values.myInitVel = initialVel->getValue();
  springDC->getValues(values.mySpringDCVals);
  springFS->getValues(values.mySpringFSVals);
  damperFS->getValues(values.myDamperFCVals);
}


void FuiJointDOF::setValues(const FuiJointDOFValues& values, bool isSensitive)
{
  int mType = values.myMotionType;
  if (mType >= PRESCRIBED_DISP) {
    variableType->setValue(mType-PRESCRIBED_DISP);
    mType = PRESCRIBED;
  }
  else if (mType == PRESCRIBED)
    variableType->setValue(0); // Default is prescribed displacement/angle

  motionType->setValue(mType);
  simpleLoad->setValues(values.myLoadVals);
#ifdef FT_HAS_FREQDOMAIN
  freqToggle->setValue(values.freqDomain);
#endif
  initialVel->setValue(values.myInitVel);
  springDC->setValues(values.mySpringDCVals);
  springFS->setValues(values.mySpringFSVals);
  damperFS->setValues(values.myDamperFCVals);

  bool isFree   = mType == FREE || mType == FREE_DYNAMICS;
  bool isSprDmp = mType == SPRING_CONSTRAINED || mType == SPRING_DYNAMICS;

  if (mType == PRESCRIBED)
    variableType->popUp();
  else
    variableType->popDown();

  if (isFree || isSprDmp)
    simpleLoad->popUp();
  else
    simpleLoad->popDown();

#ifdef FT_HAS_FREQDOMAIN
  if (isFree) {
    freqToggle->popUp();
    freqToggle->setSensitivity(isSensitive && !values.myLoadVals.isConstant);
  }
  else if (mType == PRESCRIBED) {
    freqToggle->popUp();
    freqToggle->setSensitivity(isSensitive && values.mySpringDCVals.selectedLengthEngine);
  }
  else
    freqToggle->popDown();
#else
  // Dummy statement to suppress compiler warning
  if (freqToggle) freqToggle->setSensitivity(isSensitive);
#endif

  if (mType == FIXED)
    initialVel->popDown();
  else
    initialVel->popUp();

  if (isSprDmp) {
    springFS->popUp();
    damperFS->popUp();
  }
  else {
    springFS->popDown();
    damperFS->popDown();
  }
}
