// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiCreateTurbineTower.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "vpmUI/Fui.H"
#include "vpmUI/Pixmaps/turbineTower.xpm"
#include "vpmUI/Pixmaps/turbineModelT.xpm"

#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuDialogButtons.H"

#include "vpmPM/FpPM.H"

#include "vpmApp/vpmAppCmds/FapDBCreateCmds.H"

enum {
  UPDATE = FFuDialogButtons::LEFTBUTTON,
  CLOSE  = FFuDialogButtons::MIDBUTTON,
  HELP   = FFuDialogButtons::RIGHTBUTTON
};

Fmd_SOURCE_INIT(FUI_CREATETURBINETOWER,FuiCreateTurbineTower,FFuTopLevelShell);

//----------------------------------------------------------------------------

FuiCreateTurbineTower::FuiCreateTurbineTower()
{
  Fmd_CONSTRUCTOR_INIT(FuiCreateTurbineTower);
}
//----------------------------------------------------------------------------

void FuiCreateTurbineTower::setSensitivity(bool isSensitive)
{
  this->dialogButtons->setButtonSensitivity(UPDATE,isSensitive);
}
//----------------------------------------------------------------------------

void FuiCreateTurbineTower::initWidgets()
{
  this->headerImage->setPixMap(turbineTower_xpm);
  this->modelImage->setPixMap(turbineModelT_xpm);

  this->dialogButtons->setButtonClickedCB(FFaDynCB1M(FuiCreateTurbineTower,this,
						     onDialogButtonClicked,int));

  this->dialogButtons->setButtonLabel(UPDATE,"Update tower");
  this->dialogButtons->setButtonLabel(CLOSE,"Close");
  this->dialogButtons->setButtonLabel(HELP,"Help");

  this->towerGeometryFrame->setLabel("Geometry");

  this->towerGeometryD1Field->setLabel("D1");
  this->towerGeometryD1Field->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->towerGeometryD1Field->myField->setDoubleDisplayPrecision(6);

  this->towerGeometryD2Field->setLabel("D2");
  this->towerGeometryD2Field->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->towerGeometryD2Field->myField->setDoubleDisplayPrecision(6);

  this->towerGeometryM1Field->setLabel("T");
  this->towerGeometryM1Field->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->towerGeometryM1Field->myField->setDoubleDisplayPrecision(6);
  this->towerGeometryM1Field->setSensitivity(false);

  this->towerGeometryH1Field->setLabel("H1");
  this->towerGeometryH1Field->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->towerGeometryH1Field->myField->setDoubleDisplayPrecision(6);
  this->towerGeometryH1Field->setAcceptedCB(FFaDynCB1M(FuiCreateTurbineTower,this,onHeightChanged,double));

  this->towerGeometryH2Field->setLabel("H2");
  this->towerGeometryH2Field->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->towerGeometryH2Field->myField->setDoubleDisplayPrecision(6);
  this->towerGeometryH2Field->setSensitivity(false);

  this->towerWallThicknField->setLabel("Wall thickness");
  this->towerWallThicknField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->towerWallThicknField->myField->setDoubleDisplayPrecision(6);

  this->towerMeshFrame->setLabel("Beam mesh");

  this->towerMeshN1Field->setLabel("N1");
  this->towerMeshN1Field->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);

  this->towerMeshN2Field->setLabel("N2");
  this->towerMeshN2Field->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);

  this->towerMaterialFrame->setLabel("Material");

  this->towerMaterialMenu->setBehaviour(FuiQueryInputField::REF_NONE);
  this->towerMaterialMenu->setButtonMeaning(FuiQueryInputField::EDIT);

  this->towerMaterialRhoField->setLabel("<font face='Symbol'><font size='+1'>n</font></font>");
  this->towerMaterialRhoField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->towerMaterialRhoField->myField->setDoubleDisplayPrecision(6);
  this->towerMaterialRhoField->setSensitivity(false);

  this->towerMaterialEField->setLabel("E");
  this->towerMaterialEField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->towerMaterialEField->myField->setDoubleDisplayPrecision(6);
  this->towerMaterialEField->setSensitivity(false);

  this->towerMaterialNuField->setLabel("<font face='Symbol'><font size='+1'>n</font></font>");
  this->towerMaterialNuField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->towerMaterialNuField->myField->setDoubleDisplayPrecision(6);
  this->towerMaterialNuField->setSensitivity(false);

  this->towerMaterialGField->setLabel("G");
  this->towerMaterialGField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->towerMaterialGField->myField->setDoubleDisplayPrecision(6);
  this->towerMaterialGField->setSensitivity(false);

  notes->setText("You can update the tower definition of the current wind "
                 "turbine\nmodel here. Click 'Update tower' to update the "
                 "existing model.");

  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//----------------------------------------------------------------------------

void FuiCreateTurbineTower::setCB(const FFaDynCB1<FmModelMemberBase*>& aDynCB)
{
  this->towerMaterialMenu->setRefSelectedCB(aDynCB);
}
//-----------------------------------------------------------------------------

void FuiCreateTurbineTower::onPoppedUp()
{
  this->updateUIValues();
}
//----------------------------------------------------------------------------

bool FuiCreateTurbineTower::onClose()
{
  this->invokeFinishedCB();
  return false;
}
//----------------------------------------------------------------------------

void FuiCreateTurbineTower::onDialogButtonClicked(int button)
{
  switch (button)
  {
    case UPDATE:
      FpPM::vpmSetUndoPoint("Tower definition");
      this->updateDBValues();
      if (FapDBCreateCmds::updateWindTurbineTower())
	Fui::okDialog("Wind turbine mechanism successfully updated.");
      else
	Fui::okDialog("Failed to update turbine mechanism.");
      break;

    case CLOSE:
      this->invokeFinishedCB();
      break;

    case HELP:
      // Display the topic in the help file
      Fui::showCHM("dialogbox/windpower/turbine-tower.htm");
      break;
  }
}
//----------------------------------------------------------------------------

void FuiCreateTurbineTower::onHeightChanged(double)
{
  double m1 = this->towerGeometryM1Field->getValue();
  double h1 = this->towerGeometryH1Field->getValue();
  this->towerGeometryH2Field->myField->setValue(m1 - h1);
}
//----------------------------------------------------------------------------

FFuaUIValues* FuiCreateTurbineTower::createValuesObject()
{
  return new FuaCreateTurbineTowerValues();
}
//----------------------------------------------------------------------------

void FuiCreateTurbineTower::setValues(double rho, double E, double nu, double G)
{
  this->towerMaterialRhoField->setValue(rho);
  this->towerMaterialEField->setValue(E);
  this->towerMaterialNuField->setValue(nu);
  this->towerMaterialGField->setValue(G);
}
//----------------------------------------------------------------------------

void FuiCreateTurbineTower::setUIValues(const FFuaUIValues* values)
{
  FuaCreateTurbineTowerValues* twrValues = (FuaCreateTurbineTowerValues*) values;

  this->towerMeshN1Field->myField->setValue(twrValues->N1);
  this->towerMeshN2Field->myField->setValue(twrValues->N2);
  this->towerGeometryD1Field->setValue(twrValues->D1);
  this->towerGeometryD2Field->setValue(twrValues->D2);
  this->towerGeometryM1Field->setValue(twrValues->M1);
  this->towerGeometryH1Field->setValue(twrValues->H1);
  this->onHeightChanged(twrValues->M1);
  this->towerWallThicknField->setValue(twrValues->wallThickness);
  this->towerMaterialMenu->setQuery(twrValues->materialQuery);
  this->towerMaterialMenu->setSelectedRef(twrValues->materialObject);
  this->towerMaterialMenu->setButtonCB(twrValues->editCB);
  this->setSensitivity(twrValues->isSensitive);
}
//-----------------------------------------------------------------------------

void FuiCreateTurbineTower::getUIValues(FFuaUIValues* values)
{
  FuaCreateTurbineTowerValues* twrValues = (FuaCreateTurbineTowerValues*) values;

  twrValues->N1 = this->towerMeshN1Field->myField->getInt();
  twrValues->N2 = this->towerMeshN2Field->myField->getInt();
  twrValues->D1 = this->towerGeometryD1Field->getValue();
  twrValues->D2 = this->towerGeometryD2Field->getValue();
  twrValues->M1 = this->towerGeometryM1Field->getValue();
  twrValues->H1 = this->towerGeometryH1Field->getValue();
  twrValues->wallThickness = this->towerWallThicknField->getValue();
  twrValues->materialObject = this->towerMaterialMenu->getSelectedRef();
}
