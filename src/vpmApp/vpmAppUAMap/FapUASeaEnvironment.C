// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUASeaEnvironment.H"
#include "vpmApp/vpmAppUAMap/FapUAQuery.H"
#include "vpmUI/vpmUITopLevels/FuiSeaEnvironment.H"
#include "vpmUI/Fui.H"
#include "vpmPM/FpPM.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmSeaState.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmVesselMotion.H"


Fmd_SOURCE_INIT(FAPUASEAENVIRONMENT, FapUASeaEnvironment, FapUAExistenceHandler);


//----------------------------------------------------------------------------

FapUASeaEnvironment::FapUASeaEnvironment(FuiSeaEnvironment* uic)
  : FapUAExistenceHandler(uic), FapUADataHandler(uic), FapUAFinishHandler(uic)
{
  Fmd_CONSTRUCTOR_INIT(FapUASeaEnvironment);

  this->ui = uic;
}
//----------------------------------------------------------------------------

FFuaUIValues* FapUASeaEnvironment::createValuesObject()
{
  return new FuaSeaEnvironmentValues();
}
//----------------------------------------------------------------------------

void FapUASeaEnvironment::setDBValues(FFuaUIValues* values)
{
  FuaSeaEnvironmentValues* seaValues = (FuaSeaEnvironmentValues*) values;

  FmSeaState* sea = FmDB::getSeaStateObject();
  bool newDepth = sea->setSeaDepth(seaValues->seaDepth,false);
  bool newLevel = sea->setMeanSeaLevel(seaValues->seaLevelValue,false);
  bool newWaveD = sea->setWaveDir(seaValues->waveDirection,false);

  sea->waterDensity.setValue(seaValues->waterDensity);
  sea->growthDensity.setValue(seaValues->marineGrowthDensity);
  sea->growthThickness.setValue(seaValues->marineGrowthThickness);
  sea->growthLimit.setValue({ seaValues->marineGrowthUpperLimit, seaValues->marineGrowthLowerLimit });

  FmDB::getMechanismObject()->setGravity(seaValues->gravitation);

  if (sea->waveFunction.setRef(seaValues->waveFunction) ||
      newDepth || newLevel || newWaveD) sea->draw();

  sea->currFunction.setRef(seaValues->currFunction);
  sea->currentDir.setRef(seaValues->currDirFunction);
  sea->currScale.setRef(seaValues->currScaleEngine);
  sea->hdfScale.setRef(seaValues->hdfScaleEngine);

  FpPM::touchModel(); // Indicate that the model needs save
}
//----------------------------------------------------------------------------

void FapUASeaEnvironment::getDBValues(FFuaUIValues* values)
{
  FuaSeaEnvironmentValues* seaValues = (FuaSeaEnvironmentValues*) values;

  // Don't create a sea state object here, if we don't have any yet
  FmSeaState* sea = FmDB::getSeaStateObject(false);
  // Create a temporary object to get default values
  if (!sea) sea = new FmSeaState();

  seaValues->waterDensity  = sea->waterDensity.getValue();
  seaValues->seaLevelValue = sea->meanSeaLevel.getValue();
  seaValues->seaDepth      = sea->seaDepth.getValue();
  seaValues->waveDirection = sea->waveDir.getValue();

  seaValues->marineGrowthDensity    = sea->growthDensity.getValue();
  seaValues->marineGrowthThickness  = sea->growthThickness.getValue();
  seaValues->marineGrowthUpperLimit = sea->growthLimit.getValue().first;
  seaValues->marineGrowthLowerLimit = sea->growthLimit.getValue().second;

  seaValues->gravitation = FmDB::getMechanismObject()->gravity.getValue();

  seaValues->editCB = FFaDynCB1S(FapUAQuery::onQIFieldButtonCB,FuiQueryInputFieldValues&);

  seaValues->waveQuery       = FapUAWaveFuncQuery::instance();
  seaValues->waveFunction    = sea->waveFunction.getPointer();
  seaValues->currQuery       = FapUACurrFuncQuery::instance();
  seaValues->currFunction    = sea->currFunction.getPointer();
  seaValues->currDirFunction = sea->currentDir.getPointer();
  seaValues->currScaleEngine = sea->currScale.getPointer();
  seaValues->hdfScaleEngine  = sea->hdfScale.getPointer();
  seaValues->scaleQuery      = FapUAEngineQuery::instance();

  // Delete the temporary sea state object
  if (!FmDB::getSeaStateObject(false)) sea->erase();

  // The wave direction vector is not used if we have vessel motions.
  // In that case, the vessel triad is used to define the wave direction.
  if (seaValues->waveFunction)
    if (FmVesselMotion* vm; seaValues->waveFunction->hasReferringObjs(vm,"waveFunction"))
      if (vm->getVesselTriad()) seaValues->waveDirSens = false;

  seaValues->isSensitive = FpPM::isModelEditable();
}
//----------------------------------------------------------------------------

void FapUASeaEnvironment::finishUI()
{
  Fui::seaEnvironmentUI(false,true);
}
//----------------------------------------------------------------------------
