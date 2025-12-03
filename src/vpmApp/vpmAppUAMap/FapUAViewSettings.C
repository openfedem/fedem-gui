// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAViewSettings.H"
#include "vpmUI/vpmUITopLevels/FuiViewSettings.H"
#include "vpmApp/FapLicenseManager.H"
#include "vpmDB/FmGlobalViewSettings.H"
#include "vpmDB/FmDB.H"
#include "vpmPM/FpPM.H"


Fmd_SOURCE_INIT(FAPUAVIEWSETTINGS, FapUAViewSettings, FapUAExistenceHandler);


FapUAViewSettings::FapUAViewSettings(FuiViewSettings* ui)
  : FapUAExistenceHandler(ui), FapUADataHandler(ui)
{
  Fmd_CONSTRUCTOR_INIT(FapUAViewSettings);
}


FFuaUIValues* FapUAViewSettings::createValuesObject()
{
  return new FuaViewSettingsValues();
}


void FapUAViewSettings::setDBValues(FFuaUIValues* values)
{
  FuaViewSettingsValues* viewValues = (FuaViewSettingsValues*)values;
  FmGlobalViewSettings* viewSett = FmDB::getActiveViewSettings();

  viewSett->showRevoluteJoints(viewValues->isRevoluteJointsOn);
  viewSett->showBallJoints(viewValues->isBallJointsOn);
  viewSett->showRigidJoints(viewValues->isRigidJointsOn);
  viewSett->showFreeJoints(viewValues->isFreeJointsOn);
  viewSett->showPrismaticJoints(viewValues->isPrismaticJointsOn);
  viewSett->showCylindricJoints(viewValues->isCylindricJointsOn);
  viewSett->showCamJoints(viewValues->isCamJointsOn);

  viewSett->setFogOn(viewValues->isFogOn);
  viewSett->setAntialiazingOn(viewValues->isAntialiazingOn);
  viewSett->setNiceTransparency(!viewValues->isSimpleTransparencyOn);
  viewSett->setFogVisibility(viewValues->myFogVisibility);

  viewSett->showBeamTriads(viewValues->isBeamTriadsOn);
  viewSett->showBeamCS(viewValues->isBeamCSOn);
  viewSett->showBeams(viewValues->isBeamsOn);
  viewSett->showParts(viewValues->isPartsOn);
  viewSett->showPartCS(viewValues->isPartCSOn);
  viewSett->showInternalPartCSs(viewValues->isInternalPartCSsOn);
  viewSett->showPartCoGCSs(viewValues->isPartCoGCSOn);
  viewSett->showRefPlanes(viewValues->isRefPlaneOn);
  viewSett->showSeaStates(viewValues->isSeaStateOn);
  viewSett->showWaves(viewValues->isWavesOn);

  // symbols
  viewSett->showStrainRosettes(viewValues->isSymbolOn[FuiViewSettings::STRAIN_ROSETTE]);
  viewSett->showFeedbacks(viewValues->isSymbolOn[FuiViewSettings::SENSOR]);

  viewSett->showHPs(viewValues->isSymbolOn[FuiViewSettings::GEAR]);
  viewSett->showJoints(viewValues->isSymbolOn[FuiViewSettings::JOINT]);
  viewSett->showLoads(viewValues->isSymbolOn[FuiViewSettings::LOAD]);
  viewSett->showSprDas(viewValues->isSymbolOn[FuiViewSettings::SPR_DA]);
  viewSett->showStickers(viewValues->isSymbolOn[FuiViewSettings::STICKER]);
  viewSett->showTriads(viewValues->isSymbolOn[FuiViewSettings::TRIADS]);
  viewSett->showTires(viewValues->isSymbolOn[FuiViewSettings::TIRES]);

  viewSett->setSymbolLineWidth(viewValues->myLineWidth);
  viewSett->setSymbolScale(viewValues->mySymbolSize);
  viewSett->setFogVisibility(viewValues->myFogVisibility);

  // colors
  viewSett->setFeedbackColor(viewValues->mySymbolColor[FuiViewSettings::SENSOR]);
  viewSett->setHPColor(viewValues->mySymbolColor[FuiViewSettings::GEAR]);
  viewSett->setJointColor(viewValues->mySymbolColor[FuiViewSettings::JOINT]);
  viewSett->setLoadColor(viewValues->mySymbolColor[FuiViewSettings::LOAD]);
  viewSett->setSprDaColor(viewValues->mySymbolColor[FuiViewSettings::SPR_DA]);
  viewSett->setStickerColor(viewValues->mySymbolColor[FuiViewSettings::STICKER]);
  viewSett->setTriadColor(viewValues->mySymbolColor[FuiViewSettings::TRIADS]);
  viewSett->setTireColor(viewValues->mySymbolColor[FuiViewSettings::TIRES]);
  viewSett->setStrainRosetteColor(viewValues->mySymbolColor[FuiViewSettings::STRAIN_ROSETTE]);

  viewSett->setGroundedTriadColor(viewValues->myGroundedColor);
  viewSett->setInactiveColor(viewValues->myUnattachedColor);
  viewSett->setViewerBackgroundColor(viewValues->myBackgroundColor);

  FpPM::touchModel(); // Indicate that the model needs save
}


void FapUAViewSettings::getDBValues(FFuaUIValues* values)
{
  FuaViewSettingsValues* viewValues = (FuaViewSettingsValues*)values;
  FmGlobalViewSettings* viewSett = FmDB::getActiveViewSettings();

  viewValues->isRevoluteJointsOn = viewSett->visibleRevoluteJoints();
  viewValues->isBallJointsOn = viewSett->visibleBallJoints();
  viewValues->isRigidJointsOn = viewSett->visibleRigidJoints();
  viewValues->isFreeJointsOn = viewSett->visibleFreeJoints();
  viewValues->isPrismaticJointsOn = viewSett->visiblePrismaticJoints();
  viewValues->isCylindricJointsOn = viewSett->visibleCylindricJoints();
  viewValues->isCamJointsOn = viewSett->visibleCamJoints();

  viewValues->isFogOn                = viewSett->isFogOn();
  viewValues->isAntialiazingOn       = viewSett->isAntialiazingOn();
  viewValues->isSimpleTransparencyOn = !viewSett->getNiceTransparency();
  viewValues->myFogVisibility        = viewSett->getFogVisibility();
  viewValues->isBeamTriadsOn         = viewSett->visibleBeamTriads();
  viewValues->isBeamsOn              = viewSett->visibleBeams();
  viewValues->isBeamCSOn             = viewSett->visibleBeamCS();
  viewValues->isPartsOn              = viewSett->visibleParts();
  viewValues->isInternalPartCSsOn    = viewSett->visibleInternalPartCSs();
  viewValues->isPartCSOn             = viewSett->visiblePartCS();
  viewValues->isPartCoGCSOn          = viewSett->visiblePartCoGCSs();
  viewValues->isRefPlaneOn           = viewSett->visibleRefPlanes();
  viewValues->isSeaStateOn           = viewSett->visibleSeaStates();
  viewValues->isWavesOn              = viewSett->visibleWaves();

  // symbols
  viewValues->isSymbolOn[FuiViewSettings::SENSOR]  = viewSett->visibleFeedbacks();
  viewValues->isSymbolOn[FuiViewSettings::GEAR]    = viewSett->visibleHPs();
  viewValues->isSymbolOn[FuiViewSettings::JOINT]   = viewSett->visibleJoints();
  viewValues->isSymbolOn[FuiViewSettings::LOAD]    = viewSett->visibleLoads();
  viewValues->isSymbolOn[FuiViewSettings::SPR_DA]  = viewSett->visibleSprDas();
  viewValues->isSymbolOn[FuiViewSettings::STICKER] = viewSett->visibleStickers();
  viewValues->isSymbolOn[FuiViewSettings::TRIADS]  = viewSett->visibleTriads();
  viewValues->isSymbolOn[FuiViewSettings::TIRES]   = viewSett->visibleTires();
  viewValues->isSymbolOn[FuiViewSettings::STRAIN_ROSETTE] = viewSett->visibleStrainRosettes();

  viewValues->myLineWidth     = viewSett->getSymbolLineWidth();
  viewValues->mySymbolSize    = viewSett->getSymbolScale();
  viewValues->myFogVisibility = viewSett->getFogVisibility();

  // colors
  viewValues->mySymbolColor[FuiViewSettings::SENSOR]  = viewSett->getFeedbackColor();
  viewValues->mySymbolColor[FuiViewSettings::GEAR]    = viewSett->getHPColor();
  viewValues->mySymbolColor[FuiViewSettings::JOINT]   = viewSett->getJointColor();
  viewValues->mySymbolColor[FuiViewSettings::LOAD]    = viewSett->getLoadColor();
  viewValues->mySymbolColor[FuiViewSettings::SPR_DA]  = viewSett->getSprDaColor();
  viewValues->mySymbolColor[FuiViewSettings::STICKER] = viewSett->getStickerColor();
  viewValues->mySymbolColor[FuiViewSettings::TRIADS]  = viewSett->getTriadColor();
  viewValues->mySymbolColor[FuiViewSettings::TIRES]   = viewSett->getTireColor();
  viewValues->mySymbolColor[FuiViewSettings::STRAIN_ROSETTE] = viewSett->getStrainRosetteColor();

  viewValues->myGroundedColor   = viewSett->getGroundedTriadColor();
  viewValues->myUnattachedColor = viewSett->getInactiveColor();
  viewValues->myBackgroundColor = viewSett->getViewerBackgroundColor();

  viewValues->showTireSettings = FapLicenseManager::hasFeature("FA-TIR");
}
