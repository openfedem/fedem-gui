// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiAirEnvironment.H"
#include "vpmUI/vpmUITopLevels/FuiTurbWind.H"
#include "vpmUI/Fui.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmAnalysis.H"
#include "vpmPM/FpPM.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuFileBrowseField.H"
#include "FFuLib/FFuDialogButtons.H"
#include "FFuLib/FFuFileDialog.H"


enum {
  APPLY  = FFuDialogButtons::LEFTBUTTON,
  CANCEL = FFuDialogButtons::MIDBUTTON,
  HELP   = FFuDialogButtons::RIGHTBUTTON
};

Fmd_SOURCE_INIT(FUI_AIRENVIRONMENT,FuiAirEnvironment,FFuTopLevelShell);

//----------------------------------------------------------------------------

FuiAirEnvironment::FuiAirEnvironment()
{
  Fmd_CONSTRUCTOR_INIT(FuiAirEnvironment);

  myAeroDt = 0.0;
  windTurbHubHeight = 0.0;
  windTurbGridSize = 0.0;
  windTurbDuration = 0.0;
  windTurbTimeInc = 0.0;
}
//----------------------------------------------------------------------------

void FuiAirEnvironment::setSensitivity(bool isSensitive)
{
  this->dialogButtons->setButtonSensitivity(APPLY,isSensitive);
}
//----------------------------------------------------------------------------

void FuiAirEnvironment::initWidgets()
{
  this->dialogButtons->setButtonClickedCB(FFaDynCB1M(FuiAirEnvironment,this,
						     onDialogButtonClicked,int));

  this->dialogButtons->setButtonLabel(APPLY,"Save");
  this->dialogButtons->setButtonLabel(CANCEL,"Close");
  this->dialogButtons->setButtonLabel(HELP,"Help");

  this->aTolerField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->aTolerField->setDoubleDisplayPrecision(3);

  this->airDensField->setLabel("Air density");
  this->airDensField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->airDensField->myField->setDoubleDisplayPrecision(5);

  this->kinViscField->setLabel("Kinematic air viscosity");
  this->kinViscField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->kinViscField->myField->setDoubleDisplayPrecision(5);

  this->dtAeroField->setLabel("Time step for aerodynamic calculations");
  this->dtAeroField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->dtAeroField->myField->setDoubleDisplayPrecision(3);
  this->dtAeroToggle->setLabel("Use same time step as dynamics solver");
  this->dtAeroToggle->setToggleCB(FFaDynCB1M(FuiAirEnvironment,this,onDtToggled,bool));

  this->useCMToggle->setLabel("Use aerodynamic pitching moment mode");

  this->stallModMenu->addOption("STEADY");
  this->stallModMenu->addOption("BEDDOES");
  this->stallModMenu->setOptionSelectedCB(FFaDynCB1M(FuiAirEnvironment,this,onStallChanged,int));

  this->infModMenu->addOption("GDW");
  this->infModMenu->addOption("BEM");
  this->infModMenu->setOptionSelectedCB(FFaDynCB1M(FuiAirEnvironment,this,onInflowChanged,int));

  this->indModMenu->addOption("NONE");
  this->indModMenu->addOption("WAKE");
  this->indModMenu->addOption("SWIRL");
  this->indModMenu->setOptionSelectedCB(FFaDynCB1M(FuiAirEnvironment,this,onInductionChanged,int));

  this->tipLossMenu->addOption("NONE");
  this->tipLossMenu->addOption("PRAND");
  this->tipLossMenu->addOption("GTECH");
  this->tipLossMenu->setOptionSelectedCB(FFaDynCB1M(FuiAirEnvironment,this,onTLossChanged,int));

  this->hubLossMenu->addOption("NONE");
  this->hubLossMenu->addOption("PRAND");
  this->hubLossMenu->setOptionSelectedCB(FFaDynCB1M(FuiAirEnvironment,this,onHLossChanged,int));

  this->windFrame->setLabel("Wind definition");
  this->windToggle->setLabel("Constant wind");
  this->windToggle->setToggleCB(FFaDynCB1M(FuiAirEnvironment,this,onWindToggled,bool));

  this->windTurbButton->setLabel("Create turbulent wind..");
  this->windTurbButton->setActivateCB(FFaDynCB0M(FuiAirEnvironment,this,onWindTurbClicked));
  this->windTurbButton->setToolTip("Launch a NREL/TurbSim front-end for\n"
                                   "creation of turbulent wind input files.\n"
                                   "You must return to this dialog afterwards\n"
                                   "to toggle off 'Constant wind' and specify\n"
                                   "the wind file to use in the dynamics simulation.");

  this->windSpeedField->setLabel("Wind speed");
  this->windDirField->setLabel("Wind direction [deg]");

  this->windField->setLabel("Wind file");
  this->windField->setAbsToRelPath("yes");
  this->windField->setDialogType(FFuFileDialog::FFU_OPEN_FILE);
  this->windField->setDialogRememberKeyword("WindField");
  this->windField->addDialogFilter("Wind file","bts",true,0);
  this->windField->addDialogFilter("Wind file","wnd",false,1);
  this->windField->addAllFilesFilter(true);

  this->towerFrame->setLabel("Tower influence");
  this->towerPotToggle->setLabel("Use potential flow model");
  this->towerPotToggle->setToggleCB(FFaDynCB1M(FuiAirEnvironment,this,onTowerToggled,bool));
  this->towerShdToggle->setLabel("Include downwind tower shadow");
  this->towerShdToggle->setToggleCB(FFaDynCB1M(FuiAirEnvironment,this,onTowerToggled,bool));

  this->towerField->setLabel("Tower drag file");
  this->towerField->setAbsToRelPath("yes");
  this->towerField->setDialogType(FFuFileDialog::FFU_OPEN_FILE);
  this->towerField->setDialogRememberKeyword("TowerDrag");
  this->towerField->addDialogFilter("Tower file","dat",true,0);
  this->towerField->addAllFilesFilter(true);

  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//-----------------------------------------------------------------------------

void FuiAirEnvironment::onPoppedUp()
{
  this->updateUIValues();
}
//----------------------------------------------------------------------------

bool FuiAirEnvironment::onClose()
{
  this->invokeFinishedCB();
  return false;
}
//----------------------------------------------------------------------------

void FuiAirEnvironment::onDialogButtonClicked(int button)
{
  switch (button)
  {
    case APPLY:
      FpPM::vpmSetUndoPoint("Air environment");
      this->updateDBValues();
      break;

    case CANCEL:
      this->invokeFinishedCB();
      break;

    case HELP:
      // Display the topic in the help file
      Fui::showCHM("dialogbox/windpower/aerodynamic-setup.htm");
      break;
  }
}
//----------------------------------------------------------------------------

void FuiAirEnvironment::onStallChanged(int value)
{
  switch (value) {
  case 0:
    this->stallModMenu->setToolTip("Quasi-steady airfoil characteristics");
    break;
  case 1:
    this->stallModMenu->setToolTip("Beddoes-Leishman dynamic stall model");
    break;
  }
}
//----------------------------------------------------------------------------

void FuiAirEnvironment::onInflowChanged(int value)
{
  switch (value) {
  case 0:
    this->infModMenu->setToolTip("Generalized Dynamic Wake");
    this->tipLossMenu->setSensitivity(false);
    this->hubLossMenu->setSensitivity(false);
    break;
  case 1:
    this->infModMenu->setToolTip("Blade Element Momentum");
    this->tipLossMenu->setSensitivity(true);
    this->hubLossMenu->setSensitivity(true);
    break;
  }
}
//----------------------------------------------------------------------------

void FuiAirEnvironment::onInductionChanged(int value)
{
  switch (value) {
  case 0:
    this->indModMenu->setToolTip();
    break;
  case 1:
    this->indModMenu->setToolTip("Calculate axial induction only");
    break;
  case 2:
    this->indModMenu->setToolTip("Calculate axial and tangential induction");
    break;
  }
}
//----------------------------------------------------------------------------

void FuiAirEnvironment::onTLossChanged(int value)
{
  switch (value) {
  case 0:
    this->tipLossMenu->setToolTip();
    break;
  case 1:
    this->tipLossMenu->setToolTip("Prandtl tip loss model");
    break;
  case 2:
    this->tipLossMenu->setToolTip("Georgia Tech correction to the Prandtl model");
    break;
  }
}
//----------------------------------------------------------------------------

void FuiAirEnvironment::onHLossChanged(int value)
{
  switch (value) {
  case 0:
    this->hubLossMenu->setToolTip();
    break;
  case 1:
    this->hubLossMenu->setToolTip("Prandtl hub loss model");
    break;
  }
}
//----------------------------------------------------------------------------

void FuiAirEnvironment::onWindToggled(bool value)
{
  this->windSpeedField->setSensitivity(value);
  this->windDirField->setSensitivity(value);
  this->windField->setSensitivity(!value);
}
//----------------------------------------------------------------------------

void FuiAirEnvironment::onWindTurbClicked(void)
{
  if (windTurbHubHeight > 0.0 && windTurbGridSize > 0.0)
    Fui::turbWindUI()->setValues(windTurbHubHeight,windTurbGridSize,
                                 windTurbTimeInc,windTurbDuration,
                                 windTurbOutputPath.c_str());
  else
    Fui::okDialog("There is no wind turbine model. You have to create a wind\n"
                  "turbine model before you can create turbulent wind files.",
                  FFuDialog::WARNING);
}
//----------------------------------------------------------------------------

void FuiAirEnvironment::onTowerToggled(bool)
{
  this->towerField->setSensitivity(towerPotToggle->getValue() ||
				   towerShdToggle->getValue());
}
//----------------------------------------------------------------------------

void FuiAirEnvironment::onDtToggled(bool value)
{
  if (value)
    this->dtAeroField->setValue(FmDB::getActiveAnalysis()->timeIncr.getValue());
  else
    this->dtAeroField->setValue(myAeroDt);

  this->dtAeroField->setSensitivity(!value);
}
//----------------------------------------------------------------------------

FFuaUIValues* FuiAirEnvironment::createValuesObject()
{
  return new FuaAirEnvironmentValues();
}
//----------------------------------------------------------------------------

void FuiAirEnvironment::setUIValues(const FFuaUIValues* values)
{
  FuaAirEnvironmentValues* envValues = (FuaAirEnvironmentValues*) values;

  this->stallModMenu->selectOption(envValues->stallMod);
  this->onStallChanged(envValues->stallMod);
  this->useCMToggle->setValue(envValues->useCM);
  this->infModMenu->selectOption(envValues->infMod);
  this->onInflowChanged(envValues->infMod);
  this->indModMenu->selectOption(envValues->indMod);
  this->onInductionChanged(envValues->indMod);
  this->aTolerField->setValue(envValues->aToler);
  this->tipLossMenu->selectOption(envValues->tlMod);
  this->onTLossChanged(envValues->tlMod);
  this->hubLossMenu->selectOption(envValues->hlMod);
  this->onHLossChanged(envValues->hlMod);
  this->windToggle->setValue(envValues->constWnd);
  this->onWindToggled(envValues->constWnd);
  this->windSpeedField->setValue(envValues->windVel);
  this->windDirField->setValue(envValues->windDir);
  this->windField->setAbsToRelPath(envValues->modelFilePath);
  this->windField->setFileName(envValues->windFile);
  this->towerPotToggle->setValue(envValues->twrPot);
  this->towerShdToggle->setValue(envValues->twrShad);
  this->onTowerToggled(envValues->twrShad);
  this->towerField->setAbsToRelPath(envValues->modelFilePath);
  this->towerField->setFileName(envValues->twrFile);
  this->airDensField->setValue(envValues->airDens);
  this->kinViscField->setValue(envValues->kinVisc);
  this->dtAeroField->setValue(myAeroDt=envValues->dtAero);
  this->dtAeroToggle->setValue(envValues->useDSdt);
  this->onDtToggled(envValues->useDSdt);
  this->setSensitivity(envValues->isSensitive);

  windTurbHubHeight = envValues->windTurbHubHeight;
  windTurbGridSize = envValues->windTurbGridSize;
  windTurbDuration = envValues->windTurbDuration;
  windTurbTimeInc = envValues->windTurbTimeInc;
  windTurbOutputPath = envValues->modelFilePath;
}
//-----------------------------------------------------------------------------

void FuiAirEnvironment::getUIValues(FFuaUIValues* values)
{
  FuaAirEnvironmentValues* envValues = (FuaAirEnvironmentValues*) values;

  envValues->stallMod = this->stallModMenu->getSelectedOption();
  envValues->useCM    = this->useCMToggle->getToggle();
  envValues->infMod   = this->infModMenu->getSelectedOption();
  envValues->indMod   = this->indModMenu->getSelectedOption();
  envValues->aToler   = this->aTolerField->getDouble();
  envValues->tlMod    = this->tipLossMenu->getSelectedOption();
  envValues->hlMod    = this->hubLossMenu->getSelectedOption();
  envValues->constWnd = this->windToggle->getToggle();
  envValues->windVel  = this->windSpeedField->getValue();
  envValues->windDir  = this->windDirField->getValue();
  envValues->windFile = this->windField->getFileName();
  envValues->twrPot   = this->towerPotToggle->getToggle();
  envValues->twrShad  = this->towerShdToggle->getToggle();
  envValues->twrFile  = this->towerField->getFileName();
  envValues->airDens  = this->airDensField->getValue();
  envValues->kinVisc  = this->kinViscField->getValue();
  envValues->dtAero   = this->dtAeroField->getValue();
  envValues->useDSdt  = this->dtAeroToggle->getValue();
}
