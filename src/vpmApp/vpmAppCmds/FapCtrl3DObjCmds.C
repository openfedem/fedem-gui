// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FapCtrl3DObjCmds.H"
#include "vpmApp/FapLicenseManager.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "vpmUI/FuiCtrlModes.H"
#include "vpmDB/FmControlAdmin.H"
#include "vpmDB/FmcInput.H"
#include "vpmDB/FmcOutput.H"
#include "vpmDB/FmcElements.H"
#ifdef USE_INVENTOR
#include "vpmDisplay/FdCtrlDB.H"
#include <Inventor/SbVec2f.h>
#include <Inventor/SbVec3f.h>
#endif

#define LAMBDA_CREATE(XX) FFaDynCB0S([](){FapCtrl3DObjCmds::ctrlCreate(XX::getClassTypeID());})
#define LAMBDA_EDIT(mode) FFaDynCB0S([](){FapCtrl3DObjCmds::ctrlEdit(FuiCtrlModes::mode);})

//----------------------------------------------------------------------------

void FapCtrl3DObjCmds::init()
{
  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createInput");
  cmdItem->setSmallIcon(ctrlElemIn_xpm);
  cmdItem->setText("Input");
  cmdItem->setToolTip("Input");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcInput));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createOutput");
  cmdItem->setSmallIcon(ctrlElemOut_xpm);
  cmdItem->setText("Output");
  cmdItem->setToolTip("Output");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcOutput));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createAmplifier");
  cmdItem->setSmallIcon(ctrlAmplifier_xpm);
  cmdItem->setText("Amplifier");
  cmdItem->setToolTip("Amplifier");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcAmplifier));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createPower");
  cmdItem->setSmallIcon(ctrlPower_xpm);
  cmdItem->setText("Power");
  cmdItem->setToolTip("Power");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcPower));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createComparator");
  cmdItem->setSmallIcon(ctrlComparator_xpm);
  cmdItem->setText("Comparator");
  cmdItem->setToolTip("Comparator");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcComparator));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createAdder");
  cmdItem->setSmallIcon(ctrlAdder_xpm);
  cmdItem->setText("Adder");
  cmdItem->setToolTip("Adder");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcAdder));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createMultiplyer");
  cmdItem->setSmallIcon(ctrlMultiplier_xpm);
  cmdItem->setText("Multiplier");
  cmdItem->setToolTip("Multiplier");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcMultiplier));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createIntegrator");
  cmdItem->setSmallIcon(ctrlIntegrator_xpm);
  cmdItem->setText("Integrator");
  cmdItem->setToolTip("Integrator");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcIntegrator));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createLimDerivator");
  cmdItem->setSmallIcon(ctrlLimDerivator_xpm);
  cmdItem->setText("Derivator");
  cmdItem->setToolTip("Derivator");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcLimDerivator));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createTimeDelay");
  cmdItem->setSmallIcon(ctrlTimeDelay_xpm);
  cmdItem->setText("Delay");
  cmdItem->setToolTip("Delay");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcTimeDelay));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createSampleHold");
  cmdItem->setSmallIcon(ctrlSampleHold_xpm);
  cmdItem->setText("Sample and Hold");
  cmdItem->setToolTip("Sample and Hold");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcSampleHold));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createLogicalSwitch");
  cmdItem->setSmallIcon(ctrlLogicalSwitch_xpm);
  cmdItem->setText("Logic Switch");
  cmdItem->setToolTip("Logic Switch");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcLogicalSwitch));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createLimitation");
  cmdItem->setSmallIcon(ctrlLimitation_xpm);
  cmdItem->setText("Limiter");
  cmdItem->setToolTip("Limiter");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcLimitation));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createDeadZone");
  cmdItem->setSmallIcon(ctrlDeadZone_xpm);
  cmdItem->setText("Dead Zone");
  cmdItem->setToolTip("Dead Zone");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcDeadZone));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createHysteresis");
  cmdItem->setSmallIcon(ctrlHysteresis_xpm);
  cmdItem->setText("Hysteresis");
  cmdItem->setToolTip("Hysteresis");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcHysteresis));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createPid");
  cmdItem->setSmallIcon(ctrlPid_xpm);
  cmdItem->setText("PID Controller");
  cmdItem->setToolTip("PID Controller");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcPid));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createPi");
  cmdItem->setSmallIcon(ctrlPi_xpm);
  cmdItem->setText("PI Controller");
  cmdItem->setToolTip("PI Controller");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcPi));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createPd");
  cmdItem->setSmallIcon(ctrlPd_xpm);
  cmdItem->setText("PD Controller");
  cmdItem->setToolTip("PD Controller");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcPd));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createPlimIlimD");
  cmdItem->setSmallIcon(ctrlPlimIlimD_xpm);
  cmdItem->setText("P+lim I+lim D Controller");
  cmdItem->setToolTip("P+lim I+lim D Controller");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcPlimIlimD));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createPIlimD");
  cmdItem->setSmallIcon(ctrlPIlimD_xpm);
  cmdItem->setText("PI+lim D Controller");
  cmdItem->setToolTip("PI+lim D Controller");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcPIlimD));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createPlimI");
  cmdItem->setSmallIcon(ctrlPlimI_xpm);
  cmdItem->setText("P+lim I Controller");
  cmdItem->setToolTip("P+lim I Controller");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcPlimI));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createPlimD");
  cmdItem->setSmallIcon(ctrlPlimD_xpm);
  cmdItem->setText("P+lim D Controller");
  cmdItem->setToolTip("P+lim D Controller");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcPlimD));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createRealPole");
  cmdItem->setSmallIcon(ctrlRealPole_xpm);
  cmdItem->setText("Real Pole");
  cmdItem->setToolTip("Real Pole");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcRealPole));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_createCompConjPole");
  cmdItem->setSmallIcon(ctrlCompConjPole_xpm);
  cmdItem->setText("Complex Conjungate Poles");
  cmdItem->setToolTip("Complex Conjungate Poles");
  cmdItem->setActivatedCB(LAMBDA_CREATE(FmcCompConjPole));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_create1ordTF");
  cmdItem->setSmallIcon(ctrl1ordTF_xpm);
  cmdItem->setText("1st Order Transfer Function");
  cmdItem->setToolTip("1st Order Transfer Function");
  cmdItem->setActivatedCB(LAMBDA_CREATE(Fmc1ordTF));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_create2ordTF");
  cmdItem->setSmallIcon(ctrl2ordTF_xpm);
  cmdItem->setText("2nd Order Transfer Function");
  cmdItem->setToolTip("2nd Order Transfer Function");
  cmdItem->setActivatedCB(LAMBDA_CREATE(Fmc2ordTF));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_rotate");
  cmdItem->setSmallIcon(ctrlRotate_xpm);
  cmdItem->setText("Flip Element Direction");
  cmdItem->setToolTip("Flip Element Direction");
  cmdItem->setActivatedCB(LAMBDA_EDIT(ROTATE_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_addPoint");
  cmdItem->setSmallIcon(ctrlAddPoint_xpm);
  cmdItem->setText("Add Breakpoint");
  cmdItem->setToolTip("Add Breakpoint");
  cmdItem->setActivatedCB(LAMBDA_EDIT(ADDLINEPOINT_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_ctrl3DObj_removePoint");
  cmdItem->setSmallIcon(ctrlRemovePoint_xpm);
  cmdItem->setText("Remove Breakpoint");
  cmdItem->setToolTip("Remove Breakpoint");
  cmdItem->setActivatedCB(LAMBDA_EDIT(REMOVELINEPOINT_MODE));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isCtrlModellerEditable,bool&));
}

//----------------------------------------------------------------------------

void FapCtrl3DObjCmds::ctrlCreate(int elemType)
{
  if (!FapLicenseManager::hasCtrlLicense(elemType)) return;
  if (FapCmdsBase::hasResultsCheck()) return;

  FuiCtrlModes::setMode(FuiCtrlModes::CREATE_MODE);
#ifdef USE_INVENTOR
  SbVec2f secondPos(2,2); // (1,1) is upper right screen corner
  // Using (2,2) here to ensure that the user can't see the element symbol
  SbVec3f startPos = FdCtrlDB::getNewVec(secondPos);
  FdCtrlDB::setCreatedElem(FmControlAdmin::createElement(elemType,startPos[0],startPos[1]));
#endif
}
//----------------------------------------------------------------------------

void FapCtrl3DObjCmds::ctrlEdit(int editMode)
{
  if (FapLicenseManager::hasCtrlLicense() && !FapCmdsBase::hasResultsCheck())
    FuiCtrlModes::setMode(editMode);
}
//----------------------------------------------------------------------------
