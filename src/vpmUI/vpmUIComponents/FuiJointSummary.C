// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiJointSummary.H"
#include "vpmUI/vpmUIComponents/FuiJointDOF.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "FFuLib/FFuTable.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "vpmDB/FmModelMemberBase.H"


void FuiJointSummary::initWidgets()
{
  myDOF_TZ_Toggle->setLabel("Z translation DOF");

  myCamThicknessField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myCamWidthField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myRadialToggle->setLabel("Use radial stiffness");

  myScrewRatioField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  mySetAllFreeButton->setLabel("Set All Free");
  mySetAllFixedButton->setLabel("Set All Fixed");

  myFriction->setBehaviour(FuiQueryInputField::REF_NONE);
  myFrictionDof->addOption("Tx");
  myFrictionDof->addOption("Ty");
  myFrictionDof->addOption("Tz");
  myFrictionDof->addOption("Rx");
  myFrictionDof->addOption("Ry");
  myFrictionDof->addOption("Rz");

  mySummaryTable->setNumberColumns(10, true);
  mySummaryTable->setColumnLabel(0, "Constraint"   , 14.0);
  mySummaryTable->setColumnLabel(1, "Load"         , 14.0);
  mySummaryTable->setColumnLabel(2, "Model length" , 16.0);
  mySummaryTable->setColumnLabel(3, "Init disp."   , 12.0);
  mySummaryTable->setColumnLabel(4, "Length change", 20.0);
  mySummaryTable->setColumnLabel(5, "Init vel."    , 10.0);
  mySummaryTable->setColumnLabel(6, "Spring"       , 20.0);
  mySummaryTable->setColumnLabel(7, "Spr. scale"   , 15.0);
  mySummaryTable->setColumnLabel(8, "Damper"       , 15.0);
  mySummaryTable->setColumnLabel(9, "Dmp. scale"   , 14.0);

  myDefDamperLabel->setLabel("**) Use deformational velocity in damping force calculation");

  this->FuiSummaryTable::initWidgets();
  this->showFixFreeAll(false);
  this->showTzToggle(false);
  this->showFriction(false);
  this->showScrew(false);
  this->showCamVars(false);
}


void FuiJointSummary::showFixFreeAll(bool yesOrNo)
{
  if (yesOrNo) {
    mySetAllFreeButton->popUp();
    mySetAllFixedButton->popUp();
  }
  else {
    mySetAllFreeButton->popDown();
    mySetAllFixedButton->popDown();
  }
}


void FuiJointSummary::showSwapJoint(int jType)
{
  if (jType > 0) {
    if (jType == 1)
      mySwapJointButton->setLabel("Convert to Cylindric joint");
    else
      mySwapJointButton->setLabel("Convert to Prismatic joint");
    mySwapJointButton->popUp();
  }
  else
    mySwapJointButton->popDown();
}


void FuiJointSummary::showTzToggle(bool yesOrNo)
{
  if (yesOrNo)
    myDOF_TZ_Toggle->popUp();
  else
    myDOF_TZ_Toggle->popDown();
}


void FuiJointSummary::setSummary(int jv, const FuiJointDOFValues& jval)
{
  bool isFree = false;
  bool isSprDmp = false;
  bool isPrescribed = false;

  int col = 0; // Constraint type
  switch (jval.myMotionType) {
  case FuiJointDOF::FIXED:
    mySummaryTable->insertText(jv, col, "Fixed");
    break;
  case FuiJointDOF::FREE:
    isFree = true;
    mySummaryTable->insertText(jv, col, "Free");
    break;
  case FuiJointDOF::FREE_DYNAMICS:
    isFree = true;
    mySummaryTable->insertText(jv, col, "Free*");
    myAddBCLabel->popUp();
    break;
  case FuiJointDOF::PRESCRIBED:
  case FuiJointDOF::PRESCRIBED_DISP:
    isPrescribed = true;
    mySummaryTable->insertText(jv, col, "Presc.");
    break;
  case FuiJointDOF::PRESCRIBED_VEL:
    isPrescribed = true;
    mySummaryTable->insertText(jv, col, "Presc. vel.");
    break;
  case FuiJointDOF::PRESCRIBED_ACC:
    isPrescribed = true;
    mySummaryTable->insertText(jv, col, "Presc. acc.");
    break;
  case FuiJointDOF::SPRING_CONSTRAINED:
    isSprDmp = true;
    mySummaryTable->insertText(jv, col, "Spr/Dmp");
    break;
  case FuiJointDOF::SPRING_DYNAMICS:
    isSprDmp = true;
    mySummaryTable->insertText(jv, col, "Spr/Dmp*");
    myAddBCLabel->popUp();
    break;
  }

  // Load
  std::string colText;
  if (isFree || isSprDmp)
  {
    if (jval.myLoadVals.selectedEngine)
      colText = jval.myLoadVals.selectedEngine->getInfoString();
    else if (jval.myLoadVals.isConstant)
      colText = FFaNumStr(jval.myLoadVals.constValue,1,8);
  }
  mySummaryTable->insertText(jv, ++col, colText);

  // Model length
  colText = FFaNumStr(jval.mySpringDCVals.variable,1,8);
  mySummaryTable->insertText(jv, ++col, colText);

  // Initial deflection
  if (isSprDmp || isPrescribed)
    colText = FFaNumStr(jval.mySpringDCVals.getInitDeflection(),1,8);
  else
    colText.clear();
  mySummaryTable->insertText(jv, ++col, colText);

  // Length change
  if ((isSprDmp || isPrescribed) && jval.mySpringDCVals.selectedLengthEngine)
    colText = jval.mySpringDCVals.selectedLengthEngine->getInfoString();
  else
    colText.clear();
  mySummaryTable->insertText(jv, ++col, colText);

  // Initial velocity
  if (isFree || isSprDmp || isPrescribed)
    colText = FFaNumStr(jval.myInitVel,1,8);
  else
    colText.clear();
  mySummaryTable->insertText(jv, ++col, colText);

  // Spring
  if (isSprDmp)
  {
    if (jval.mySpringFSVals.selectedFunction)
      colText = jval.mySpringFSVals.selectedFunction->getInfoString();
    else
      colText = FFaNumStr(jval.mySpringFSVals.constFunction,1,8);
  }
  else
    colText.clear();
  mySummaryTable->insertText(jv, ++col, colText);

  // Spring scale
  if (isSprDmp && jval.mySpringFSVals.selectedScaleEngine)
    colText = jval.mySpringFSVals.selectedScaleEngine->getInfoString();
  else
    colText.clear();
  mySummaryTable->insertText(jv, ++col, colText);

  // Damper
  if (isSprDmp)
  {
    if (jval.myDamperFCVals.selectedFunction)
      colText = jval.myDamperFCVals.selectedFunction->getInfoString();
    else
      colText = FFaNumStr(jval.myDamperFCVals.constFunction,1,8);
    if (jval.myDamperFCVals.isDefDamper &&
	jval.mySpringDCVals.selectedLengthEngine)
    {
      colText += "**";
      myDefDamperLabel->popUp();
    }
  }
  else
    colText.clear();
  mySummaryTable->insertText(jv, ++col, colText);

  // Damper scale
  if (isSprDmp && jval.myDamperFCVals.selectedScaleEngine)
    colText = jval.myDamperFCVals.selectedScaleEngine->getInfoString();
  else
    colText.clear();
  mySummaryTable->insertText(jv, ++col, colText);

  mySummaryTable->setTableRowReadOnly(jv, true);
}
