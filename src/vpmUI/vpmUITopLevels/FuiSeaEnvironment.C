// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiSeaEnvironment.H"
#include "vpmUI/vpmUITopLevels/FuiModelPreferences.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "vpmUI/vpmUIComponents/Fui3DPoint.H"
#include "vpmUI/Fui.H"
#include "vpmPM/FpPM.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuDialogButtons.H"


enum {
  APPLY  = FFuDialogButtons::LEFTBUTTON,
  CANCEL = FFuDialogButtons::MIDBUTTON,
  HELP =   FFuDialogButtons::RIGHTBUTTON
};

Fmd_SOURCE_INIT(FUI_SEAENVIRONMENT,FuiSeaEnvironment,FFuTopLevelShell);

//----------------------------------------------------------------------------

FuiSeaEnvironment::FuiSeaEnvironment()
{
  Fmd_CONSTRUCTOR_INIT(FuiSeaEnvironment);
}
//----------------------------------------------------------------------------

void FuiSeaEnvironment::setSensitivity(bool isSensitive)
{
  this->dialogButtons->setButtonSensitivity(APPLY,isSensitive);
}
//----------------------------------------------------------------------------

void FuiSeaEnvironment::initWidgets()
{
  this->dialogButtons->setButtonClickedCB(FFaDynCB1M(FuiSeaEnvironment,this,
						     onDialogButtonClicked,int));

  this->dialogButtons->setButtonLabel(APPLY,"Save");
  this->dialogButtons->setButtonLabel(CANCEL,"Close");
  this->dialogButtons->setButtonLabel(HELP,"Help");

  this->waterDensityField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->waterDensityField->setDoubleDisplayPrecision(6);

  this->seaDepthField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->seaDepthField->setDoubleDisplayPrecision(6);
  this->seaDepthField->setToolTip("Depth<=0 will be interpreted as deep water");

  this->meanSeaLevelField->setToolTip("Height of the sea surface above global origin\n"
                                      "in opposite direction of the gravity vector");
  this->meanSeaLevelField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->meanSeaLevelField->setDoubleDisplayPrecision(6);

  this->gravitationFrame->setLabel("Gravitation");
  this->gravitationVector->setToolTip("The normal vector of the calm sea surface\n"
                                      "has opposite direction of this vector");

  this->waveDirectionFrame->setLabel("Wave direction");
  this->waveDirectionVector->setToolTip("The wave propagation vector. "
					"Not used if the Wave function\n"
					"also is used in a Vessel motion (RAO). "
					"In that case, the initial local axes of\n"
					"the Vessel Triad are used instead.");

  this->marineGrowthFrame->setLabel("Marine growth");
  this->marineGrowthDensityField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->marineGrowthDensityField->setDoubleDisplayPrecision(6);
  this->marineGrowthThicknessField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->marineGrowthThicknessField->setDoubleDisplayPrecision(6);
  this->marineGrowthUpperLimitField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->marineGrowthUpperLimitField->setDoubleDisplayPrecision(6);
  this->marineGrowthLowerLimitField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->marineGrowthLowerLimitField->setDoubleDisplayPrecision(6);

  this->waveQueryField->setBehaviour(FuiQueryInputField::REF_NONE);
  this->waveQueryField->setButtonMeaning(FuiQueryInputField::EDIT);
  this->waveQueryField->setToolTip("Press the \"Help\" button for details on the\n"
				   "reference coordinate system of the wave kinematics.");
  this->waveQueryField->setRefSelectedCB(FFaDynCB1M(FuiSeaEnvironment,this,
						    onWaveSelected,FmModelMemberBase*));

  this->currQueryField->setBehaviour(FuiQueryInputField::REF_NONE);
  this->currQueryField->setButtonMeaning(FuiQueryInputField::EDIT);
  this->currQueryField->setToolTip("Sea current magnitude as function of Z (and time).\n"
				   "Press the \"Help\" button for details on the\n"
				   "reference coordinate system of the wave kinematics,\n"
				   "which also applies to the current functions.");
  this->currQueryField->setRefSelectedCB(FFaDynCB1M(FuiSeaEnvironment,this,
						    onCurrentSelected,FmModelMemberBase*));

  this->currDirQueryField->setBehaviour(FuiQueryInputField::REF_NONE);
  this->currDirQueryField->setTextForNoRefSelected("0.0");
  this->currDirQueryField->setButtonMeaning(FuiQueryInputField::EDIT);
  this->currDirQueryField->setToolTip("Angle (in radians) between the wave direction "
				      "and current direction as function of Z (and time).\n"
				      "A zero angle implies that the current is in the "
				      "direction of the wave propagation direction.");

  this->currScaleQueryField->setBehaviour(FuiQueryInputField::REF_NONE);
  this->currScaleQueryField->setButtonMeaning(FuiQueryInputField::EDIT);
  this->currScaleQueryField->setToolTip("Optional scaling of the sea current magnitude");

  this->hdfScaleQueryField->setBehaviour(FuiQueryInputField::REF_NONE);
  this->hdfScaleQueryField->setButtonMeaning(FuiQueryInputField::EDIT);
  this->hdfScaleQueryField->setToolTip("Optional scaling of the hydrodynamic forces");

  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//-----------------------------------------------------------------------------

bool FuiSeaEnvironment::onClose()
{
  this->invokeFinishedCB();
  return false;
}
//----------------------------------------------------------------------------

void FuiSeaEnvironment::onDialogButtonClicked(int button)
{
  switch (button)
  {
    case APPLY:
      FpPM::vpmSetUndoPoint("Sea environment");
      this->updateDBValues();
      break;

    case CANCEL:
      this->invokeFinishedCB();
      return;

    case HELP:
      // Display the topic in the help file
      Fui::showCHM("dialogbox/marine/sea-environment.htm");
      break;
  }

  // Also update the model preferences UI,
  // since the Gravitation vector are in both UI's
  FFuTopLevelShell* pref = FFuTopLevelShell::getInstanceByType(FuiModelPreferences::getClassTypeID());
  if (pref) dynamic_cast<FuiModelPreferences*>(pref)->updateUIValues();
}
//----------------------------------------------------------------------------

void FuiSeaEnvironment::onCurrentSelected(FmModelMemberBase* current)
{
  this->currDirQueryField->setSensitivity(current != NULL);
  this->currScaleQueryField->setSensitivity(current != NULL);
}
//----------------------------------------------------------------------------

void FuiSeaEnvironment::onWaveSelected(FmModelMemberBase* wave)
{
  this->seaDepthField->setSensitivity(wave != NULL);
  this->hdfScaleQueryField->setSensitivity(wave != NULL);
}
//----------------------------------------------------------------------------

FFuaUIValues* FuiSeaEnvironment::createValuesObject()
{
  return new FuaSeaEnvironmentValues();
}
//----------------------------------------------------------------------------

void FuiSeaEnvironment::setUIValues(const FFuaUIValues* values)
{
  FuaSeaEnvironmentValues* envValues = (FuaSeaEnvironmentValues*) values;

  this->waterDensityField->setValue(envValues->waterDensity);
  this->meanSeaLevelField->setValue(envValues->seaLevelValue);
  this->seaDepthField->setValue(envValues->seaDepth);
  this->gravitationVector->setValue(envValues->gravitation);
  this->waveDirectionVector->setValue(envValues->waveDirection);

  this->marineGrowthDensityField->setValue(envValues->marineGrowthDensity);
  this->marineGrowthThicknessField->setValue(envValues->marineGrowthThickness);
  this->marineGrowthUpperLimitField->setValue(envValues->marineGrowthUpperLimit);
  this->marineGrowthLowerLimitField->setValue(envValues->marineGrowthLowerLimit);

  this->waveQueryField->setQuery(envValues->waveQuery);
  this->waveQueryField->setSelectedRef(envValues->waveFunction);
  this->waveQueryField->setButtonCB(envValues->editCB);

  this->currQueryField->setQuery(envValues->currQuery);
  this->currQueryField->setSelectedRef(envValues->currFunction);
  this->currQueryField->setButtonCB(envValues->editCB);

  this->currDirQueryField->setQuery(envValues->currQuery);
  this->currDirQueryField->setSelectedRef(envValues->currDirFunction);
  this->currDirQueryField->setButtonCB(envValues->editCB);

  this->currScaleQueryField->setQuery(envValues->scaleQuery);
  this->currScaleQueryField->setSelectedRef(envValues->currScaleEngine);
  this->currScaleQueryField->setButtonCB(envValues->editCB);

  this->hdfScaleQueryField->setQuery(envValues->scaleQuery);
  this->hdfScaleQueryField->setSelectedRef(envValues->hdfScaleEngine);
  this->hdfScaleQueryField->setButtonCB(envValues->editCB);

  this->waveDirectionVector->setSensitivity(envValues->waveDirSens);
  this->onCurrentSelected(envValues->currFunction);
  this->onWaveSelected(envValues->waveFunction);
  this->setSensitivity(envValues->isSensitive);
}
//-----------------------------------------------------------------------------

void FuiSeaEnvironment::getUIValues(FFuaUIValues* values)
{
  FuaSeaEnvironmentValues* envValues = (FuaSeaEnvironmentValues*) values;

  envValues->waterDensity    = waterDensityField->getDouble();
  envValues->seaLevelValue   = meanSeaLevelField->getDouble();
  envValues->seaDepth        = seaDepthField->getDouble();
  envValues->gravitation     = gravitationVector->getValue();
  envValues->waveDirection   = waveDirectionVector->getValue();

  envValues->marineGrowthDensity    = marineGrowthDensityField->getDouble();
  envValues->marineGrowthThickness  = marineGrowthThicknessField->getDouble();
  envValues->marineGrowthLowerLimit = marineGrowthLowerLimitField->getDouble();
  envValues->marineGrowthUpperLimit = marineGrowthUpperLimitField->getDouble();

  envValues->waveFunction    = this->waveQueryField->getSelectedRef();
  envValues->currFunction    = this->currQueryField->getSelectedRef();
  envValues->currDirFunction = this->currDirQueryField->getSelectedRef();
  envValues->currScaleEngine = this->currScaleQueryField->getSelectedRef();
  envValues->hdfScaleEngine  = this->hdfScaleQueryField->getSelectedRef();
}
