// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiTriadSummary.H"
#include "vpmUI/vpmUIComponents/FuiTriadDOF.H"
#include "vpmDB/FmModelMemberBase.H"
#include "FFuLib/FFuTable.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFaLib/FFaString/FFaStringExt.H"


void FuiTriadSummary::initWidgets()
{
  myFENodeField->setLabel("FE Node");
  myFENodeField->setSensitivity(false);

  mySetAllFreeButton->setLabel("Set All Free");
  mySetAllFixedButton->setLabel("Set All Fixed");

  myMassField->setLabel("Mass");
  myMassField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myIxField->setLabel("Ix");
  myIxField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myIyField->setLabel("Iy");
  myIyField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myIzField->setLabel("Iz");
  myIzField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  mySysDirMenu->addOption("Global");
  mySysDirMenu->addOption("Local, initial");
  mySysDirMenu->addOption("Local, withrotated");

  myConnectorMenu->addOption("Rigid");
  myConnectorMenu->addOption("Flexible");

  mySummaryTable->setNumberColumns(3, true);
  mySummaryTable->setColumnLabel(0, "Constraint", 35);
  mySummaryTable->setColumnLabel(1, "Load/Motion", 35);
  mySummaryTable->setColumnLabel(2, "Initial velocity", 30);

  this->FuiSummaryTable::initWidgets();

  int massLabelWidth = myMassField->myLabel->getWidthHint();
  myIxField->setLabelWidth(massLabelWidth);
  myIyField->setLabelWidth(massLabelWidth);
  myIzField->setLabelWidth(massLabelWidth);
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
