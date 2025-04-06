// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiTriadSummary.H"
#include "vpmUI/vpmUIComponents/FuiTriadDOF.H"
#include "vpmDB/FmModelMemberBase.H"
#include "FFuLib/FFuFrame.H"
#include "FFuLib/FFuTable.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFaLib/FFaString/FFaStringExt.H"


void FuiTriadSummary::initWidgets()
{
  myFENodeField->setLabel("FE Node");
  myFENodeField->setSensitivity(false);
  myFENodeField->toFront();

  mySetAllFreeButton->setLabel("Set All Free");
  mySetAllFixedButton->setLabel("Set All Fixed");

  myMassFrame->setLabel("Additional Masses");
  myMassField->setLabel("Mass");
  myMassField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myIxField->setLabel("Ix");
  myIxField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myIyField->setLabel("Iy");
  myIyField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myIzField->setLabel("Iz");
  myIzField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  mySysDirFrame->setLabel("System directions");
  mySysDirMenu->addOption("Global");
  mySysDirMenu->addOption("Local, initial");
  mySysDirMenu->addOption("Local, withrotated");

  myConnectorFrame->setLabel("Surface connector");
  myConnectorMenu->addOption("Rigid");
  myConnectorMenu->addOption("Flexible");

  mySummaryTable->setNumberColumns(3, true);
  mySummaryTable->setColumnLabel(0, "Constraint", 35);
  mySummaryTable->setColumnLabel(1, "Load/Motion", 35);
  mySummaryTable->setColumnLabel(2, "Initial velocity", 30);

  this->FuiSummaryTable::initWidgets();
}


void FuiTriadSummary::placeWidgets(int width, int height)
{
  int fontHeight = this->getFontHeigth();
  int fieldHeight = getGridLinePos(height,115);
  if (fieldHeight > fontHeight+5) fieldHeight = fontHeight+5;

  int hBorder = getGridLinePos(width,5);
  int vBorder = getGridLinePos(height,12);

  int v1 = 2*hBorder;
  int v4 = width/4 - 2*hBorder;
  int v5 = v4 + 2*hBorder;
  int v6 = v5 + 2*hBorder;
  int v7 = width/2 - 2*hBorder;
  int v8 = v7 + hBorder;
  int v9 = v8 + 2*hBorder;

  int h1 = vBorder;
  int h10 = height - vBorder;
  int numFld = myConnectorMenu->isPoppedUp() ? 6 : 5;
  int fields = h10-h1 - (numFld == 6 ? 3*fontHeight+16*vBorder : 2*fontHeight+11*vBorder);
  int fieldH = numFld*fieldHeight > fields ? fields/numFld : fieldHeight;
  int h2 = h1 + fontHeight + vBorder;
  int h3 = h1 + vBorder;
  int h4 = h3 + fieldHeight;
  int h5 = h2 + 5*vBorder + 4*fieldH;
  int h6 = h5 + 2*vBorder;
  int h7 = h6 + fontHeight + 3*vBorder + fieldH;
  int h61 = h7 + 2*vBorder;
  int h71 = h61 + fontHeight + 3*vBorder + fieldH;
  int h9 = h10 - fontHeight;

  myFENodeField->setEdgeGeometry(v1,v4,h3,h4);
  myTriadLabel->setEdgeGeometry(v1,v4,h9,h10);

  h4 += 2*vBorder;
  mySetAllFreeButton->setEdgeGeometry(v1,v4,h4,h4+fieldHeight);
  h4 += 2*vBorder + fieldHeight;
  mySetAllFixedButton->setEdgeGeometry(v1,v4,h4,h4+fieldHeight);

  myMassFrame->setEdgeGeometry(v5,v8,h1,h5);
  myMassField->setEdgeGeometry(v6,v7,h2,h2+fieldH); h2 += fieldH+vBorder;
  myIxField->  setEdgeGeometry(v6,v7,h2,h2+fieldH); h2 += fieldH+vBorder;
  myIyField->  setEdgeGeometry(v6,v7,h2,h2+fieldH); h2 += fieldH+vBorder;
  myIzField->  setEdgeGeometry(v6,v7,h2,h2+fieldH);

  int massLabelWidth = myMassField->myLabel->getWidthHint();
  myMassField->setLabelWidth(massLabelWidth);
  myIxField->setLabelWidth(massLabelWidth);
  myIyField->setLabelWidth(massLabelWidth);
  myIzField->setLabelWidth(massLabelWidth);

  mySysDirFrame->setEdgeGeometry(v5,v8,h6,h7); h6 += fontHeight + vBorder;
  mySysDirMenu->setEdgeGeometry(v6,v7,h6,h7-2*vBorder);
  myConnectorFrame->setEdgeGeometry(v5,v8,h61,h71); h61 += fontHeight + vBorder;
  myConnectorMenu->setEdgeGeometry(v6,v7,h61,h71-2*vBorder);

  int tabHeight = h1 + (7*(mySummaryTable->getNumberRows()+1)/5+2)*fontHeight;
  int tabBottom = myAddBCLabel->isPoppedUp() ? h10-fontHeight : h10;
  if (tabHeight > tabBottom) tabHeight = tabBottom;
  mySummaryTable->setEdgeGeometry(v9,width,h1,tabHeight); tabHeight += vBorder;
  myAddBCLabel->setEdgeGeometry(v9,width,tabHeight,tabHeight+fontHeight);

  mySummaryTable->updateColumnWidths();
}


void FuiTriadSummary::setSummary(int dof, const FuiTriadDOFValues& tval)
{
  // Constraint type
  switch (tval.myMotionType) {
  case FuiTriadDOF::FIXED:
    mySummaryTable->insertText(dof, 0, "Fixed");
    break;
  case FuiTriadDOF::FREE:
    mySummaryTable->insertText(dof, 0, "Free");
    break;
  case FuiTriadDOF::FREE_DYNAMICS:
    mySummaryTable->insertText(dof, 0, "Free*");
    myAddBCLabel->popUp();
    break;
  case FuiTriadDOF::PRESCRIBED:
  case FuiTriadDOF::PRESCRIBED_DISP:
    mySummaryTable->insertText(dof, 0, "Prescribed deflection");
    break;
  case FuiTriadDOF::PRESCRIBED_VEL:
    mySummaryTable->insertText(dof, 0, "Prescribed velocity");
    break;
  case FuiTriadDOF::PRESCRIBED_ACC:
    mySummaryTable->insertText(dof, 0, "Prescribed acceleration");
    break;
  }
  if (tval.myMotionType == FuiTriadDOF::FIXED) return;

  // Load/Motion
  if (tval.myLoadVals.isConstant)
    mySummaryTable->insertText(dof, 1, FFaNumStr(tval.myLoadVals.constValue,1,8));
  else if (tval.myLoadVals.selectedEngine)
    mySummaryTable->insertText(dof, 1, tval.myLoadVals.selectedEngine->getInfoString());
  else
    mySummaryTable->insertText(dof, 1, "");

  // Initial velocity
  mySummaryTable->insertText(dof, 2, FFaNumStr(tval.myInitVel,1,8));

  mySummaryTable->setTableRowReadOnly(dof, true);
}
