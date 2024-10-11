// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FapMech3DObjCmds.H"
#include "vpmApp/FapEventManager.H"
#include "vpmApp/FapLicenseManager.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "vpmUI/FuiModes.H"
#include "vpmDB/FmRoad.H"

#define LAMBDA_CREATE(MODE) FFaDynCB0S([](){	\
    if (!FapCmdsBase::hasResultsCheck())	\
      FuiModes::setMode(FuiModes::MODE);	\
    })


void FapMech3DObjCmds::init()
{
  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createRevJoint");
  cmdItem->setSmallIcon(revJoint_xpm);
  cmdItem->setText("Revolute Joint");
  cmdItem->setToolTip("Revolute Joint");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKEREVJOINT_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createBallJoint");
  cmdItem->setSmallIcon(ballJoint_xpm);
  cmdItem->setText("Ball Joint");
  cmdItem->setToolTip("Ball Joint");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKEBALLJOINT_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createRigidJoint");
  cmdItem->setSmallIcon(rigidJoint_xpm);
  cmdItem->setText("Rigid Joint");
  cmdItem->setToolTip("Rigid Joint");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKERIGIDJOINT_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createFreeJoint");
  cmdItem->setSmallIcon(freeJoint_xpm);
  cmdItem->setText("Free Joint");
  cmdItem->setToolTip("Free Joint");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKEFREEJOINT_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createFreeJoint2");
  cmdItem->setSmallIcon(freeJoint_xpm);
  cmdItem->setText("Free Joint between Triads");
  cmdItem->setToolTip("Free Joint between existing Triads");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKEFREEJOINTBETWEENTRIADS_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createPrismJoint");
  cmdItem->setSmallIcon(prismJoint_xpm);
  cmdItem->setText("Prismatic Joint");
  cmdItem->setToolTip("Prismatic Joint");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKEPRISMJOINT_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createPrismJoint2");
  cmdItem->setSmallIcon(prismJoint_xpm);
  cmdItem->setText("Prismatic Joint between Triads");
  cmdItem->setToolTip("Prismatic Joint between existing Triads");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKEPRISMJOINTBETWEENTRIADS_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createCylJoint");
  cmdItem->setSmallIcon(cylJoint_xpm);
  cmdItem->setText("Cylindric Joint");
  cmdItem->setToolTip("Cylindric Joint");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKECYLJOINT_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createCylJoint2");
  cmdItem->setSmallIcon(cylJoint_xpm);
  cmdItem->setText("Cylindric Joint between Triads");
  cmdItem->setToolTip("Cylindric Joint between Triads");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKECYLJOINTBETWEENTRIADS_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createCamJoint");
  cmdItem->setSmallIcon(camJoint_xpm);
  cmdItem->setText("Cam Joint");
  cmdItem->setToolTip("Cam Joint");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKECAMJOINT_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createGear");
  cmdItem->setSmallIcon(gear_xpm);
  cmdItem->setText("Gear");
  cmdItem->setToolTip("Gear");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKEGEAR_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createRackPinon");
  cmdItem->setSmallIcon(rackPinon_xpm);
  cmdItem->setText("Rack-and-Pinion");
  cmdItem->setToolTip("Rack-and-Pinion");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKERACKPIN_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createSpring");
  cmdItem->setSmallIcon(spring_xpm);
  cmdItem->setText("Spring");
  cmdItem->setToolTip("Spring");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKESPRING_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createDamper");
  cmdItem->setSmallIcon(damper_xpm);
  cmdItem->setText("Damper");
  cmdItem->setToolTip("Damper");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKEDAMPER_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createTriad");
  cmdItem->setSmallIcon(triad_xpm);
  cmdItem->setText("Triad");
  cmdItem->setToolTip("Triad");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKETRIAD_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createForce");
  cmdItem->setSmallIcon(force_xpm);
  cmdItem->setText("Force");
  cmdItem->setToolTip("Force");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKEFORCE_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createTorque");
  cmdItem->setSmallIcon(torque_xpm);
  cmdItem->setText("Torque");
  cmdItem->setToolTip("Torque");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKETORQUE_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createSimpleSensor");
  cmdItem->setSmallIcon(makeSimpleSensor_xpm);
  cmdItem->setText("Simple Sensor");
  cmdItem->setToolTip("Simple Sensor");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKESIMPLESENSOR_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createRelativeSensor");
  cmdItem->setSmallIcon(makeRelativeSensor_xpm);
  cmdItem->setText("Relative Sensor");
  cmdItem->setToolTip("Relative Sensor");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKERELATIVESENSOR_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createSticker");
  cmdItem->setSmallIcon(sticker_xpm);
  cmdItem->setText("Sticker");
  cmdItem->setToolTip("Sticker");
  cmdItem->setActivatedCB(LAMBDA_CREATE(MAKESTICKER_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createTire");
  cmdItem->setSmallIcon(createTire_xpm);
  cmdItem->setText("Tire");
  cmdItem->setToolTip("Tire");
  cmdItem->setActivatedCB(FFaDynCB0S(FapMech3DObjCmds::createTire));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_createRoad");
  cmdItem->setSmallIcon(createRoad_xpm);
  cmdItem->setText("Road");
  cmdItem->setToolTip("Road");
  cmdItem->setActivatedCB(FFaDynCB0S(FapMech3DObjCmds::createRoad));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_pointToPointMove");
  cmdItem->setSmallIcon(pointToPointMove_xpm);
  cmdItem->setText("Smart Move");
  cmdItem->setToolTip("Smart Move");
  cmdItem->setActivatedCB(LAMBDA_CREATE(PTPMOVE_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_attach");
  cmdItem->setSmallIcon(attach_xpm);
  cmdItem->setText("Attach");
  cmdItem->setToolTip("Attach");
  cmdItem->setActivatedCB(LAMBDA_CREATE(ATTACH_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_detach");
  cmdItem->setSmallIcon(detach_xpm);
  cmdItem->setText("Detach");
  cmdItem->setToolTip("Detach");
  cmdItem->setActivatedCB(LAMBDA_CREATE(DETACH_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_mech3DObj_setObjAppearance");
  cmdItem->setSmallIcon(appearance_xpm);
  cmdItem->setText("Item Appearance...");
  cmdItem->setToolTip("Item Appearance");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FuiModes::setMode(FuiModes::APPEARANCE_MODE); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));
}
//----------------------------------------------------------------------------

void FapMech3DObjCmds::createTire()
{
  if (!FapLicenseManager::hasTireLicense())
    return;

  if (!FapCmdsBase::hasResultsCheck())
    FuiModes::setMode(FuiModes::MAKETIRE_MODE);
}
//----------------------------------------------------------------------------

void FapMech3DObjCmds::createRoad()
{
  if (!FapLicenseManager::hasTireLicense())
    return;

  if (!FapCmdsBase::hasResultsCheck()) {
    FmRoad* road = new FmRoad();
    road->connect();
    FapEventManager::permTotalSelect(road);
  }
}
//----------------------------------------------------------------------------
