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
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "vpmDB/FmModelMemberBase.H"


void FuiJointSummary::initWidgets()
{
  myDOF_TZ_Toggle->setLabel("Z translation DOF");

  myCamThicknessField->setLabel("Thickness");
  myCamThicknessField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myCamWidthField->setLabel("Width");
  myCamWidthField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myRadialToggle->setLabel("Use radial stiffness");

  myScrewFrame->setLabel("Screw connection");
  myScrewRatioField->setLabel("Output ratio");
  myScrewRatioField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  mySetAllFreeButton->setLabel("Set All Free");
  mySetAllFixedButton->setLabel("Set All Fixed");

  myFrictionFrame->setLabel("Friction");
  myFrictionLabel->setLabel("Joint DOF");
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
  IAmShowingFixFreeAll = yesOrNo;
  if (yesOrNo) {
    mySetAllFreeButton->popUp();
    mySetAllFixedButton->popUp();
  }
  else {
    mySetAllFreeButton->popDown();
    mySetAllFixedButton->popDown();
  }
}


void FuiJointSummary::showTzToggle(bool yesOrNo)
{
  IAmShowingTzToggle = yesOrNo;
  if (yesOrNo)
    myDOF_TZ_Toggle->popUp();
  else
    myDOF_TZ_Toggle->popDown();
}


void FuiJointSummary::showFriction(char yesOrNo)
{
  IAmShowingFriction = yesOrNo;
  if (yesOrNo > 1) {
    myFrictionFrame->popUp();
    myFriction->popUp();
    myFrictionLabel->popUp();
    myFrictionDof->popUp();
  }
  else if (yesOrNo > 0) {
    myFrictionFrame->popUp();
    myFriction->popUp();
    myFrictionLabel->popDown();
    myFrictionDof->popDown();
  }
  else {
    myFrictionFrame->popDown();
    myFriction->popDown();
    myFrictionLabel->popDown();
    myFrictionDof->popDown();
  }
}


void FuiJointSummary::showScrew(bool yesOrNo)
{
  IAmShowingScrew = yesOrNo;
  if (yesOrNo) {
    myScrewFrame->popUp();
    myScrewToggle->popUp();
    myScrewRatioField->popUp();
  }
  else {
    myScrewFrame->popDown();
    myScrewToggle->popDown();
    myScrewRatioField->popDown();
  }
}

void FuiJointSummary::showCamVars(bool yesOrNo)
{
  IAmShowingCamVars = yesOrNo;
  if (yesOrNo) {
    myCamThicknessField->popUp();
    myCamWidthField->popUp();
    myRadialToggle->popUp();
  }
  else {
    myCamThicknessField->popDown();
    myCamWidthField->popDown();
    myRadialToggle->popDown();
  }
}


void FuiJointSummary::placeWidgets(int width, int height)
{
  int fontHeight = this->getFontHeigth();
  int fontWidth  = this->getFontWidth("  Ry ");

  mySummaryTable->setColumnHeaderHeight(fontHeight+4);
  mySummaryTable->setRowHeaderWidth(fontWidth);

  double numLines = (mySummaryTable->getNumberRows()+1)*1.5 + 0.125;

  if (IAmShowingCamVars)
    numLines += 4.775; // = 1/2 + 3*(1.3+1/8)
  else if (IAmShowingFriction > 1)
    numLines += 4.525; // = 3.0 + 1.4 + 1/8
  else if (IAmShowingFriction > 0)
    numLines += 3.125; // = 3.0 + 1/8

  if (myScrewToggle->isPoppedUp())
    numLines += 3.125; // = 3.0 + 1/8

  if (numLines*fontHeight > height)
    fontHeight = (int)(height/numLines);

  int h = fontHeight * 3*(mySummaryTable->getNumberRows()+1)/2 + 5;
  mySummaryTable->setEdgeGeometry(0, width, 0, h);

  int border = 3*fontWidth/10;
  int vBorder = fontHeight/8;
  int v1 = border;
  int v2 = width/2;
  int v3 = v2 + fontWidth;
  int v4 = width - border;

  int curHL = h + vBorder;
  int curHR = curHL;

  int fieldH = 7*fontHeight/5;

  int curH = curHL;
  int v5 = IAmShowingCamVars ? v1 : v3;
  int v6 = v5 + width/2 - fontWidth;
  myAddBCLabel->setEdgeGeometry(v5, v6, curH, curH+fontHeight);
  if (IAmShowingFootNotes%2)
    curH += fontHeight + vBorder;

  myDefDamperLabel->setEdgeGeometry(v5, v6, curH, curH+fontHeight);
  if (IAmShowingFootNotes/2)
    curH += fontHeight + vBorder;

  curH += 2*vBorder;
  if (IAmShowingFootNotes)
  {
    if (IAmShowingCamVars)
      curHL += curH-h;
    else
      curHR += curH-h;
  }

  if (IAmShowingFriction > 0) {
    curH = curHL;
    int frameH = (IAmShowingFriction > 1 ? 22 : 15)*fontHeight/5;
    myFrictionFrame->setEdgeGeometry(0, v2, curH, curH+frameH);
    curH += 5*fontHeight/4;
    myFriction->setEdgeGeometry(border, v2-border, curH, curH+fieldH);
    curH += fieldH;
    if (IAmShowingFriction > 1) {
      int labelW = this->getFontWidth(myFrictionLabel->getLabel().c_str());
      myFrictionLabel->setEdgeGeometry(border, border+labelW, curH, curH+fieldH);
      myFrictionDof->setEdgeGeometry(2*border+labelW, v2-border-fieldH, curH, curH+fieldH);
    }
    curHL += frameH+vBorder;
  }

  fieldH = 13*fontHeight/10;

  if (IAmShowingScrew) {
    myScrewFrame->setEdgeGeometry(0, v2, curHL, curHL + fontHeight*3);
    curHL += 5*fontHeight/4;
    myScrewToggle->setCenterYGeometry(v1, curHL+fieldH/2, getGridLinePos(width,40), fontHeight);
    myScrewRatioField->setEdgeGeometry(v1+getGridLinePos(width,40), v2-border, curHL, curHL+fieldH);
    curHL += 7*fontHeight/4+vBorder;
  }

  if (IAmShowingTzToggle) {
    if (IAmShowingFriction && !IAmShowingFootNotes) curHR += 3*fontHeight/2;
    myDOF_TZ_Toggle->setEdgeGeometry(v3, v4, curHR, curHR+fontHeight);
    curHR += fontHeight+vBorder;
  }

  if (IAmShowingCamVars) {
    int labelW = this->getFontWidth(myCamThicknessField->myLabel->getLabel().c_str());
    myCamWidthField->setLabelWidth(labelW);
    myCamThicknessField->setLabelWidth(labelW);
    curHR += fontHeight/2;
    myCamThicknessField->setEdgeGeometry(v3, v4, curHR, curHR+fieldH);
    curHR += fieldH+vBorder;
    myCamWidthField->setEdgeGeometry(v3, v4, curHR, curHR+fieldH);
    curHR += fieldH+vBorder;
    myRadialToggle->setEdgeGeometry(v3 + labelW+9, v4, curHR, curHR+fieldH);
    curHR += fieldH+vBorder;
  }

  if (IAmShowingFixFreeAll) {
    mySetAllFreeButton->setEdgeGeometry(v3,v3+150,curHR,curHR+16); v3 += 150+border;
    mySetAllFixedButton->setEdgeGeometry(v3,v3+150,curHR,curHR+16);
  }

  mySummaryTable->updateColumnWidths();
}


void FuiJointSummary::setRowLabel(int jv, const FuiJointDOFValues& jval, const char** labels)
{
  mySummaryTable->setRowLabel(jv,labels[jval.myDofNo]);
  if (jv == 0) IAmShowingFootNotes = 0;

  if (jval.myMotionType == FuiJointDOF::FREE_DYNAMICS ||
      jval.myMotionType == FuiJointDOF::SPRING_DYNAMICS)
    IAmShowingFootNotes = 2*(IAmShowingFootNotes/2) + 1;

  if (jval.myMotionType == FuiJointDOF::SPRING_CONSTRAINED ||
      jval.myMotionType == FuiJointDOF::SPRING_DYNAMICS)
    if (jval.myDamperFCVals.isDefDamper)
      if (jval.mySpringDCVals.selectedLengthEngine)
	IAmShowingFootNotes = IAmShowingFootNotes%2 + 2;
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

  col++; // Load
  std::string colText;
  if (isFree || isSprDmp)
  {
    if (jval.myLoadVals.selectedEngine)
      colText = jval.myLoadVals.selectedEngine->getInfoString();
    else if (jval.myLoadVals.isConstant)
      colText = FFaNumStr(jval.myLoadVals.constValue,1,8);
  }

  mySummaryTable->insertText(jv, col, colText);

  col++; // Model length
  colText = FFaNumStr(jval.mySpringDCVals.variable,1,8);

  mySummaryTable->insertText(jv, col, colText);

  col++; // Initial deflection
  colText = "";
  if (isSprDmp || isPrescribed)
  {
    if (jval.mySpringDCVals.useAsDeflection)
      colText = FFaNumStr(jval.mySpringDCVals.initLengtOrDeflection,1,8);
    else
      colText = FFaNumStr(jval.mySpringDCVals.variable - jval.mySpringDCVals.initLengtOrDeflection,1,8);
  }

  mySummaryTable->insertText(jv, col, colText);

  col++; // Length change
  colText = "";
  if (isSprDmp || isPrescribed)
    if (jval.mySpringDCVals.selectedLengthEngine)
      colText = jval.mySpringDCVals.selectedLengthEngine->getInfoString();

  mySummaryTable->insertText(jv, col, colText);

  col++; // Initial velocity
  colText = "";
  if (isFree || isSprDmp || isPrescribed)
    colText = FFaNumStr(jval.myInitVel,1,8);

  mySummaryTable->insertText(jv, col, colText);

  col++; // Spring
  colText = "";
  if (isSprDmp)
  {
    if (jval.mySpringFSVals.selectedFunction)
      colText = jval.mySpringFSVals.selectedFunction->getInfoString();
    else
      colText = FFaNumStr(jval.mySpringFSVals.constFunction,1,8);
  }

  mySummaryTable->insertText(jv, col, colText);

  col++; // Spring scale
  colText = "";
  if (isSprDmp)
    if (jval.mySpringFSVals.selectedScaleEngine)
      colText = jval.mySpringFSVals.selectedScaleEngine->getInfoString();

  mySummaryTable->insertText(jv, col, colText);

  col++; // Damper
  colText = "";
  if (isSprDmp)
  {
    if (jval.myDamperFCVals.selectedFunction)
      colText = jval.myDamperFCVals.selectedFunction->getInfoString();
    else
      colText = FFaNumStr(jval.myDamperFCVals.constFunction,1,8);
    if (jval.myDamperFCVals.isDefDamper)
      if (jval.mySpringDCVals.selectedLengthEngine)
      {
	colText += "**";
	myDefDamperLabel->popUp();
      }
  }
  mySummaryTable->insertText(jv, col, colText);

  col++; // Damper scale
  colText = "";
  if (isSprDmp)
    if (jval.myDamperFCVals.selectedScaleEngine)
      colText = jval.myDamperFCVals.selectedScaleEngine->getInfoString();

  mySummaryTable->insertText(jv, col, colText);

  mySummaryTable->setTableRowReadOnly(jv, true);
}
