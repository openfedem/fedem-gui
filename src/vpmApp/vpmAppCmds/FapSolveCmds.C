// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapSolveCmds.H"
#if defined(FT_HAS_WND) || defined(FT_HAS_RIS)
#include "vpmApp/vpmAppCmds/FapOilWellCmds.H"
#endif
#include "vpmApp/vpmAppProcess/FapSolutionProcessMgr.H"
#include "vpmApp/vpmAppProcess/FapLinkSolver.H"
#include "vpmApp/vpmAppProcess/FapLinkReducer.H"
#include "vpmApp/vpmAppProcess/FapDynamicSolver.H"
#include "vpmApp/vpmAppProcess/FapStressExpander.H"
#include "vpmApp/vpmAppProcess/FapModeShapeExpander.H"
#include "vpmApp/vpmAppProcess/FapGageRecovery.H"
#include "vpmApp/vpmAppProcess/FapDamageRecovery.H"
#include "vpmApp/FapLicenseManager.H"
#include "vpmApp/vpmAppProcess/FapSimEventHandler.H"

#include "vpmUI/Fui.H"
#include "vpmUI/FuiModes.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFuLib/FFuAuxClasses/FFuaTimer.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "FFaLib/FFaDefinitions/FFaAppInfo.H"
#include "FFaLib/FFaCmdLineArg/FFaCmdLineArg.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaString/FFaStringExt.H"

#include "vpmPM/FpModelRDBHandler.H"
#include "vpmPM/FpFileSys.H"
#include "vpmPM/FpPM.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmAnalysis.H"
#include "vpmDB/FmModesOptions.H"
#include "vpmDB/FmGageOptions.H"
#include "vpmDB/FmFppOptions.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmElementGroupProxy.H"
#include "vpmDB/FmStrainRosette.H"
#include "vpmDB/FmSimulationEvent.H"
#include "vpmDB/FmSolverInput.H"

#include <cstdio>
#if defined(win32) || defined(win64)
#include <direct.h>
#define popen  _popen
#define pclose _pclose
#define chdir  _chdir
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

#if FT_HAS_SAP > 1
#include <chrono>
#endif

std::string FapSolveCmds::ourCloudJobId;
std::string FapSolveCmds::ourCloudAppId;
FFuaTimer* FapSolveCmds::ourCloudJobTimer = NULL;
char FapSolveCmds::haveCloudAccess = 0;


//----------------------------------------------------------------------------

//! Convenience macro for defining sensitivity callbacks as Lambda functions.
#define SENSITIVITY_LAMBDA(body) FFaDynCB1S([](bool& sensitive){body;},bool&)


void FapSolveCmds::init()
{
  //! Lambda function for sensitivity on part-level recovery commands
  auto&& getSolveOnPartSensitivity = [](bool& sensitive)
  {
    if (FpPM::isModelTouchable())
    {
      std::vector<FmModelMemberBase*> selection;
      sensitive = FapCmdsBase::getCurrentSelection(selection);
      for (FmModelMemberBase* sel : selection)
        if (!(sensitive = sel->isOfType(FmPart::getClassTypeID())))
          break;
    }
    else
      sensitive = false;
  };

  //! Lambda function for sensitivity on element group-level recovery commands
  auto&& getSolveOnPartGroupSensitivity = [](bool& sensitive)
  {
    if (FpPM::isModelTouchable())
    {
      std::vector<FmModelMemberBase*> selection;
      sensitive = FapCmdsBase::getCurrentSelection(selection);
      for (FmModelMemberBase* sel : selection)
        if (!(sensitive = (sel->isOfType(FmPart::getClassTypeID()) ||
                           sel->isOfType(FmElementGroupProxy::getClassTypeID()))))
          break;
    }
    else
      sensitive = false;
  };

  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_solve_solveAll");
  cmdItem->setSmallIcon(solveAll_xpm);
  cmdItem->setText("Solve All");
  cmdItem->setToolTip("Solve All");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapSolveCmds::solveAll(false); }));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(sensitive = FpPM::isModelTouchable()));

  cmdItem = new FFuaCmdItem("cmdId_solve_solveEvents");
  cmdItem->setSmallIcon(solveAllEvents_xpm);
  cmdItem->setText("Solve Events");
  cmdItem->setToolTip("Solve all Events");
  cmdItem->setActivatedCB(FFaDynCB0S(FapSolveCmds::solveEvents));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(
    sensitive = FpPM::isModelTouchable() && FmDB::hasObjects(FmSimulationEvent::getClassTypeID()) ));

  cmdItem = new FFuaCmdItem("cmdId_solve_reduceAllLinks");
  cmdItem->setSmallIcon(reduceLinks_xpm);
  cmdItem->setText("Reduce All FE Parts");
  cmdItem->setToolTip("Reduce all FE parts");
  cmdItem->setActivatedCB(FFaDynCB0S(FapSolveCmds::reduceAllParts));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(
    sensitive = FpPM::isModelTouchable() && !FpPM::hasResults() ));

  cmdItem = new FFuaCmdItem("cmdId_solve_solveDynamicsBasic");
  cmdItem->setSmallIcon(solve_xpm);
  cmdItem->setText("Dynamics Solver (Basic Mode)...");
  cmdItem->setToolTip("Dynamics Solver (Basic Mode)");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ Fui::analysisOptionsUI(true,false,true); }));

  cmdItem = new FFuaCmdItem("cmdId_solve_solveDynamicsAdvanced");
  cmdItem->setSmallIcon(ManageSolve_xpm);
  cmdItem->setText("Dynamics Solver (Advanced Mode)...");
  cmdItem->setToolTip("Dynamics Solver (Advanced Mode)...");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ Fui::analysisOptionsUI(true,false,false); }));

  cmdItem = new FFuaCmdItem("cmdId_solve_solveStress");
  cmdItem->setSmallIcon(solveStress_xpm);
  cmdItem->setText("Recover Stresses");
  cmdItem->setToolTip("Recover Stresses");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapSolveCmds::solveStress(false); }));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(sensitive = FpPM::isModelTouchable()));

  cmdItem = new FFuaCmdItem("cmdId_solve_solveModes");
  cmdItem->setSmallIcon(solveModes_xpm);
  cmdItem->setText("Recover Mode Shapes");
  cmdItem->setToolTip("Recover Mode Shapes");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapSolveCmds::solveModes(false); }));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(
    if (FpPM::isModelTouchable() && FmDB::getModesOptions(false)) {
      FmAnalysis* ana = FmDB::getActiveAnalysis(false);
      sensitive = ana ? ana->solveEigenvalues.getValue() : false;
    }
    else
      sensitive = false ));

  cmdItem = new FFuaCmdItem("cmdId_solve_solveRosette");
  cmdItem->setSmallIcon(solveRosette_xpm);
  cmdItem->setText("Recover Strain Rosettes");
  cmdItem->setToolTip("Recover Strain Rosettes");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapSolveCmds::solveRosette(false); }));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(
    sensitive = FpPM::isModelTouchable() && FmDB::hasObjects(FmStrainRosette::getClassTypeID()) ));

  cmdItem = new FFuaCmdItem("cmdId_solve_solveStrainCoat");
  cmdItem->setSmallIcon(solveStrainCoat_xpm);
  cmdItem->setText("Create Strain Coat Recovery Summary");
  cmdItem->setToolTip("Create a summary of stress and strain on strain coat");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapSolveCmds::solveStrainCoat(false); }));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(sensitive = FpPM::isModelTouchable()));

  cmdItem = new FFuaCmdItem("cmdId_solve_reduceLink");
  cmdItem->setSmallIcon(reduceLinks_xpm);
  cmdItem->setText("Reduce");
  cmdItem->setToolTip("Reduce FE part");
  cmdItem->setActivatedCB(FFaDynCB0S(FapSolveCmds::reducePart));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(getSolveOnPartSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_solve_solveLink");
  cmdItem->setSmallIcon(solve_xpm);
  cmdItem->setText("Linear analysis");
  cmdItem->setToolTip("Direct solve FE part");
  cmdItem->setActivatedCB(FFaDynCB0S(FapSolveCmds::solvePart));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(getSolveOnPartSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_solve_solveStressOnLink");
  cmdItem->setSmallIcon(solveStress_xpm);
  cmdItem->setText("Stresses");
  cmdItem->setToolTip("Recover Stresses on FE part");
  cmdItem->setActivatedCB(FFaDynCB0S(FapSolveCmds::solveStressOnPart));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(getSolveOnPartGroupSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_solve_solveRosetteOnLink");
  cmdItem->setSmallIcon(solveRosette_xpm);
  cmdItem->setText("Strain Rosettes");
  cmdItem->setToolTip("Recover Strain Rosettes on FE part");
  cmdItem->setActivatedCB(FFaDynCB0S(FapSolveCmds::solveRosetteOnPart));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(getSolveOnPartSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_solve_solveStrainCoatOnLink");
  cmdItem->setSmallIcon(solveStrainCoat_xpm);
  cmdItem->setText("Strain Coat Summary");
  cmdItem->setToolTip("Create a summary of stress and strain on strain coat on FE part");
  cmdItem->setActivatedCB(FFaDynCB0S(FapSolveCmds::solveStrainCoatOnPart));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(getSolveOnPartGroupSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_solve_stopSolver");
  cmdItem->setSmallIcon(StopProcessing_xpm);
  cmdItem->setText("Stop All Solvers");
  cmdItem->setToolTip("Stop all Solvers");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapSolutionProcessManager::instance()->killAll(); }));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(
    sensitive = !FapSolutionProcessManager::instance()->empty() ));

  cmdItem = new FFuaCmdItem("cmdId_solve_eraseSimuleRes");
  cmdItem->setSmallIcon(eraseSimuleRes_xpm);
  cmdItem->setText("Delete Results");
  cmdItem->setToolTip("Delete results");
  cmdItem->setActivatedCB(FFaDynCB0S(FapSolveCmds::eraseSimuleRes));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(
    if (FpPM::isModelTouchable() && FpPM::hasResults() &&
        FapSolutionProcessManager::instance()->empty()) {
      FmAnalysis* ana = FmDB::getActiveAnalysis(false);
      sensitive = ana ? !ana->overwriteResults.getValue() : false;
    }
    else
      sensitive = false ));

  cmdItem = new FFuaCmdItem("cmdId_solve_eraseEventRes");
  cmdItem->setSmallIcon(eraseSimuleResEvents_xpm);
  cmdItem->setText("Delete Event Results");
  cmdItem->setToolTip("Delete results for all events");
  cmdItem->setActivatedCB(FFaDynCB0S(FapSolveCmds::eraseEventRes));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(
    if (FpPM::isModelTouchable() &&
        FmDB::hasObjects(FmSimulationEvent::getClassTypeID()) &&
        FapSimEventHandler::hasResults(false) &&
        FapSolutionProcessManager::instance()->empty()) {
      FmAnalysis* ana = FmDB::getActiveAnalysis(false);
      sensitive = ana ? !ana->overwriteResults.getValue() : false;
    }
    else
      sensitive = false ));

  cmdItem = new FFuaCmdItem("cmdId_solve_manageSolveStress");
  cmdItem->setSmallIcon(manageSolveStress_xpm);
  cmdItem->setText("Stress Recovery Setup...");
  cmdItem->setToolTip("Stress Recovery Setup");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ Fui::stressOptionsUI(); }));

  cmdItem = new FFuaCmdItem("cmdId_solve_manageSolveModes");
  cmdItem->setSmallIcon(manageSolveModes_xpm);
  cmdItem->setText("Mode Shape Recovery Setup...");
  cmdItem->setToolTip("Mode Shape Recovery Setup");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ Fui::eigenmodeOptionsUI(); }));

  cmdItem = new FFuaCmdItem("cmdId_solve_manageSolveRosette");
  cmdItem->setSmallIcon(manageSolveRosette_xpm);
  cmdItem->setText("Strain Rosette Recovery Setup...");
  cmdItem->setToolTip("Strain Rosette Recovery Setup");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ Fui::gageOptionsUI(); }));

  cmdItem = new FFuaCmdItem("cmdId_solve_manageSolveStrainCoat");
  cmdItem->setSmallIcon(manageSolveStrainCoat_xpm);
  cmdItem->setText("Strain Coat Recovery Summary Setup...");
  cmdItem->setToolTip("Strain Coat Recovery Summary Setup");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ Fui::fppOptionsUI(); }));

  cmdItem = new FFuaCmdItem("cmdId_solve_refreshRDB");
  cmdItem->setSmallIcon(refreshRDB_xpm);
  cmdItem->setText("Refresh Results");
  cmdItem->setToolTip("Refresh Results");
  cmdItem->setActivatedCB(FFaDynCB0S(FapSolveCmds::refreshRDB));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(sensitive = FpPM::isModelTouchable()));

  cmdItem = new FFuaCmdItem("cmdId_solve_deleteStressResults");
  cmdItem->setSmallIcon(deleteStressResults_xpm);
  cmdItem->setText("Delete Stress recovery results");
  cmdItem->setToolTip("Delete Stress recovery results");
  cmdItem->setActivatedCB(FFaDynCB0S([]() {
        FFaMsg::list("===> Removing stress recovery results from the active result database.\n");
        FpModelRDBHandler::removeResults("timehist_rcy",FapSimEventHandler::getActiveRSD());
      }));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(
    sensitive = FpPM::isModelTouchable() && FpPM::hasResults(FpPM::STRESS_RCY) ));

  cmdItem = new FFuaCmdItem("cmdId_solve_deleteModesResults");
  cmdItem->setSmallIcon(deleteModes_xpm);
  cmdItem->setText("Delete Mode Shape recovery results");
  cmdItem->setToolTip("Delete Mode Shape recovery results");
  cmdItem->setActivatedCB(FFaDynCB0S([]() {
        FFaMsg::list("===> Removing mode shape recovery results from the active result database.\n");
        FpModelRDBHandler::removeResults("eigval_rcy",FapSimEventHandler::getActiveRSD());
      }));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(
    sensitive = FpPM::isModelTouchable() && FpPM::hasResults(FpPM::EIGVAL_RCY) ));

  cmdItem = new FFuaCmdItem("cmdId_solve_deleteRosetteResults");
  cmdItem->setSmallIcon(deleteRosetteResults_xpm);
  cmdItem->setText("Delete Strain Rosette results");
  cmdItem->setToolTip("Delete Strain Rosette results");
  cmdItem->setActivatedCB(FFaDynCB0S([]() {
        FFaMsg::list("===> Removing strain rosette results from the active result database.\n");
        FpModelRDBHandler::removeResults("timehist_gage_rcy",FapSimEventHandler::getActiveRSD());
      }));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(
    sensitive = FpPM::isModelTouchable() && FpPM::hasResults(FpPM::GAGE_RCY) ));

  cmdItem = new FFuaCmdItem("cmdId_solve_deleteStrainCoatResults");
  cmdItem->setSmallIcon(deleteStrainCoat_xpm);
  cmdItem->setText("Delete Strain Coat Recovery Summary results");
  cmdItem->setToolTip("Delete Strain Coat recovery summary results");
  cmdItem->setActivatedCB(FFaDynCB0S([]() {
        FFaMsg::list("===> Removing strain coat recovery summary results from the active result database.\n");
        FpModelRDBHandler::removeResults("summary_rcy",FapSimEventHandler::getActiveRSD());
      }));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(
    sensitive = FpPM::isModelTouchable() && FpPM::hasResults(FpPM::SUMMARY_RCY) ));

  cmdItem = new FFuaCmdItem("cmdId_solve_prepareBatch");
  cmdItem->setSmallIcon(prepareForBatch_xpm);
  cmdItem->setText("Prepare for batch execution");
  cmdItem->setToolTip("Create input files for batch execution");
  cmdItem->setActivatedCB(FFaDynCB0S(FapSolveCmds::prepareAllBatch));

  cmdItem = new FFuaCmdItem("cmdId_solve_cloud");
  cmdItem->setSmallIcon(solve_xpm);
  cmdItem->setText("Run simulation in cloud");
  cmdItem->setToolTip("Run the dynamics simulation in the cloud");
  cmdItem->setActivatedCB(FFaDynCB0S(FapSolveCmds::solveInCloud));
  cmdItem->setGetSensitivityCB(SENSITIVITY_LAMBDA(sensitive = FpPM::isModelEditable()));
}

//----------------------------------------------------------------------------

static bool doStressRecovery(const FmAnalysis* ana, bool silence = false)
{
  if (ana->stressStartTime.getValue() <= ana->stressStopTime.getValue())
    return true;

  if (!silence)
    FFaMsg::list("===> Set startTime < stopTime to run Stress Recovery\n",true);

  return false;
}

static bool doModesRecovery(const FmModesOptions* mOpt, const FmAnalysis* ana)
{
  if (mOpt && ana->solveEigenvalues.getValue())
    return !mOpt->modesArray.getValue().empty();

  return false;
}

static bool doGageRecovery(const FmAnalysis* ana, bool silence = false)
{
  if (!FmDB::hasObjects(FmStrainRosette::getClassTypeID()))
    return false;

  FmGageOptions* gage = FmDB::getGageOptions(false);
  if (gage)
  {
    if (gage->startTime.getValue() <= gage->stopTime.getValue())
      return true;
  }
  else if (ana->startTime.getValue() <= ana->stopTime.getValue())
    return true;

  if (!silence)
    FFaMsg::list("===> Set startTime < stopTime to run Strain Rosette Recovery\n",true);

  return false;
}

static bool doDamageRecovery(const FmAnalysis* ana,
                             int msgType = FapLicenseManager::DIALOG_MSG)
{
  if (!FapLicenseManager::hasDurabilityLicense(msgType))
    return false;

  FmFppOptions* fpp = FmDB::getFppOptions(false);
  if (fpp)
  {
    if (fpp->startTime.getValue() <= fpp->stopTime.getValue())
      return true;
  }
  else if (ana->startTime.getValue() <= ana->stopTime.getValue())
    return true;

  if (msgType != FapLicenseManager::NO_MSG)
    FFaMsg::list("===> Set startTime < stopTime to run Stain Coat Recovery\n",true);

  return false;
}

//----------------------------------------------------------------------------

bool FapSolveCmds::solveAll(bool prepareForBatchOnly)
{
  FmSimulationEvent* event = FapSimEventHandler::getActiveEvent();
  if (event && !FapLicenseManager::checkSimEventLicense())
    return false;

  FuiModes::cancel();
  FapSolverBase* proc = NULL;
  FmAnalysis* analysis = FmDB::getActiveAnalysis();
  FmModesOptions* modeOpts = FmDB::getModesOptions(false);

  std::vector<FmPart*> allParts;
  FmDB::getFEParts(allParts,true);

  if (doDamageRecovery(analysis,FapLicenseManager::NO_MSG))
    for (FmPart* part : allParts)
      if (part->hasStrainCoat())
      {
        proc = new FapDamageRecovery(part,event,prepareForBatchOnly);
        FapSolutionProcessManager::instance()->pushSolverProcess(proc);
      }

  if (doGageRecovery(analysis,true))
    for (FmPart* part : allParts)
    {
      proc = new FapGageRecovery(part,event,prepareForBatchOnly);
      FapSolutionProcessManager::instance()->pushSolverProcess(proc);
    }

  bool last = true;
  std::string vtfFile;
  if (doModesRecovery(modeOpts,analysis))
    for (FmPart* part : allParts)
    {
      proc = new FapModeShapeExpander(part,event,prepareForBatchOnly,last);
      if (last && modeOpts->autoVTFExport.getValue())
        vtfFile = proc->eventName(modeOpts->vtfFileName.getValue());
      FapSolutionProcessManager::instance()->pushSolverProcess(proc);
      last = false;
    }

  if (!vtfFile.empty())
    // Create VTF-file for modes results
    FapModeShapeExpander::writeVTFHeader(allParts,vtfFile);

  last = true;
  vtfFile.clear();
  if (doStressRecovery(analysis,true))
    for (FmPart* part : allParts)
    {
      if (part->ignoreInRecovery.getValue())
        ListUI <<"===> Skipping recovery for "<< part->getIdString(true) <<"\n";
      else
      {
        proc = new FapStressExpander(part,event,prepareForBatchOnly,last);
        if (!last && analysis->autoStressVTFExport.getValue())
          vtfFile = proc->eventName(analysis->stressVTFname.getValue());
        FapSolutionProcessManager::instance()->pushSolverProcess(proc);
        last = false;
      }
    }

  if (!vtfFile.empty())
    // Create VTF-file for stress results
    FapStressExpander::writeVTFHeader(allParts,vtfFile);

  // If no recovery processes were created, run dynamics only
  if (!proc && !FpPM::hasResults())
  {
    proc = new FapDynamicSolver(event,prepareForBatchOnly);
    FapSolutionProcessManager::instance()->pushSolverProcess(proc);
  }

  return FapSolutionProcessManager::instance()->run();
}

//----------------------------------------------------------------------------

void FapSolveCmds::prepareAllBatch()
{
  FapSolveCmds::solveAll(true);
}

//----------------------------------------------------------------------------

void FapSolveCmds::solveEvents()
{
  if (!FapLicenseManager::checkSimEventLicense()) return;

  FuiModes::cancel();
  FapSolverBase* proc = NULL;
  FmAnalysis* analysis = FmDB::getActiveAnalysis();
  FmModesOptions* modeOpts = FmDB::getModesOptions(false);

  std::vector<FmSimulationEvent*> allEvents;
  FmDB::getAllSimulationEvents(allEvents,true);

  std::vector<FmPart*> allParts;
  if (FapSolutionProcessManager::instance()->empty())
    FmDB::getFEParts(allParts,true); // don't start recovery while events running

  if (doDamageRecovery(analysis,FapLicenseManager::NO_MSG))
    for (FmPart* part : allParts)
      if (part->hasStrainCoat())
        for (FmSimulationEvent* event : allEvents)
        {
          proc = new FapDamageRecovery(part,event);
          FapSolutionProcessManager::instance()->pushSolverProcess(proc);
        }

  if (doGageRecovery(analysis,true))
    for (FmPart* part : allParts)
      for (FmSimulationEvent* event : allEvents)
      {
        proc = new FapGageRecovery(part,event);
        FapSolutionProcessManager::instance()->pushSolverProcess(proc);
      }

  bool last = true;
  if (doModesRecovery(modeOpts,analysis))
    for (FmPart* part : allParts)
    {
      for (FmSimulationEvent* event : allEvents)
      {
        proc = new FapModeShapeExpander(part,event,false,last);
        FapSolutionProcessManager::instance()->pushSolverProcess(proc);
      }
      last = false;
    }

  if (!last && modeOpts->autoVTFExport.getValue())
    for (FmSimulationEvent* event : allEvents)
    {
      // Create VTF-files for modes results
      std::string vtfFile = event->eventName(modeOpts->vtfFileName.getValue());
      FapModeShapeExpander::writeVTFHeader(allParts,vtfFile);
    }

  last = true;
  if (doStressRecovery(analysis,true))
    for (FmPart* part : allParts)
    {
      if (part->ignoreInRecovery.getValue())
        ListUI <<"===> Skipping recovery for "<< part->getIdString(true) <<"\n";
      else
      {
        for (FmSimulationEvent* event : allEvents)
        {
          proc = new FapStressExpander(part,event,false,last);
          FapSolutionProcessManager::instance()->pushSolverProcess(proc);
        }
        last = false;
      }
    }

  if (!last && analysis->autoStressVTFExport.getValue())
    for (FmSimulationEvent* event : allEvents)
    {
      // Create VTF-files for stress results
      std::string vtfFile = event->eventName(analysis->stressVTFname.getValue());
      FapStressExpander::writeVTFHeader(allParts,vtfFile);
    }

  // If no recovery processes were created, run dynamics only
  std::vector<FapSolverBase*> emptyEvents;
  if (!proc)
    for (FmSimulationEvent* event : allEvents)
    {
      if (FpModelRDBHandler::hasResults(event->getResultStatusData()))
        ListUI <<"===> "<< event->getIdString(true) <<" has results.\n";
      else if (FapSolutionProcessManager::instance()->empty() || proc)
      {
        // Solver stack was empty, add events the usual way
        proc = new FapDynamicSolver(event);
        FapSolutionProcessManager::instance()->pushSolverProcess(proc);
      }
      else // Solver stack not empty, collect the empty events to be appended
        emptyEvents.push_back(new FapDynamicSolver(event));
    }

  if (emptyEvents.empty())
    FapSolutionProcessManager::instance()->run();
  else
    FapSolutionProcessManager::instance()->appendProcs(emptyEvents);
}

//----------------------------------------------------------------------------

void FapSolveCmds::reduceAllParts()
{
  FuiModes::cancel();

  std::vector<FmPart*> allParts;
  FmDB::getFEParts(allParts,true);
  if (allParts.empty()) return;

  for (FmPart* part : allParts)
    FapSolutionProcessManager::instance()->pushSolverProcess(new FapLinkReducer(part));

  FapSolutionProcessManager::instance()->run();
}

//----------------------------------------------------------------------------

bool FapSolveCmds::prepareForBatchReduction()
{
  std::vector<FmPart*> allParts;
  FmDB::getFEParts(allParts);
  if (allParts.empty()) return true;

  for (FmPart* part : allParts)
  {
    std::string rdbPath;
    FapLinkReducer reducerProc(part,false,true);
    if (reducerProc.createInput(rdbPath) == FapSolverBase::FAP_READY_TO_RUN)
      ListUI <<"===> Input files for batch reduction created in:\n     "
             << rdbPath <<"\n";
  }

  return FapSolutionProcessManager::instance()->run(); // Saves model file only
}

//----------------------------------------------------------------------------

void FapSolveCmds::solveDynamics()
{
  FmSimulationEvent* event = FapSimEventHandler::getActiveEvent();
  if (event && !FapLicenseManager::checkSimEventLicense()) return;

  FuiModes::cancel();
  FapSolverBase* proc = new FapDynamicSolver(event);

  FapSolutionProcessManager::instance()->pushSolverProcess(proc);
  FapSolutionProcessManager::instance()->run();
}

//----------------------------------------------------------------------------

bool FapSolveCmds::prepareForBatchSolving()
{
  FmSimulationEvent* event = FapSimEventHandler::getActiveEvent();
  if (event && !FapLicenseManager::checkSimEventLicense())
    return false;

  FuiModes::cancel();
  FapSolverBase* proc = new FapDynamicSolver(event,true);

  FapSolutionProcessManager::instance()->pushSolverProcess(proc);
  return FapSolutionProcessManager::instance()->run();
}

//----------------------------------------------------------------------------

bool FapSolveCmds::solveStress(bool prepareForBatchOnly)
{
  FmAnalysis* analysis = FmDB::getActiveAnalysis();
  if (!doStressRecovery(analysis))
    return false;

  FmSimulationEvent* event = FapSimEventHandler::getActiveEvent();
  if (event && !FapLicenseManager::checkSimEventLicense())
    return false;

  FuiModes::cancel();
  FapSolverBase* proc = NULL;
  bool last = true;
  std::string vtfFile;

  std::vector<FmPart*> allParts;
  FmDB::getFEParts(allParts,true);

  for (FmPart* part : allParts)
    if (part->ignoreInRecovery.getValue())
      ListUI <<"===> Skipping recovery for "<< part->getIdString(true) <<"\n";
    else
    {
      proc = new FapStressExpander(part,event,prepareForBatchOnly,last);
      if (last && analysis->autoStressVTFExport.getValue())
        vtfFile = proc->eventName(analysis->stressVTFname.getValue());
      FapSolutionProcessManager::instance()->pushSolverProcess(proc);
      last = false;
    }

  if (!vtfFile.empty())
    // Create VTF-file for stress results
    FapStressExpander::writeVTFHeader(allParts,vtfFile);

  return FapSolutionProcessManager::instance()->run();
}

//----------------------------------------------------------------------------

bool FapSolveCmds::solveModes(bool prepareForBatchOnly)
{
  FmSimulationEvent* event = FapSimEventHandler::getActiveEvent();
  if (event && !FapLicenseManager::checkSimEventLicense())
    return false;

  FuiModes::cancel();
  FmModesOptions* modeOpts = FmDB::getModesOptions();
  FapSolverBase* proc = NULL;
  bool last = true;
  std::string vtfFile;

  std::vector<FmPart*> allParts;
  FmDB::getFEParts(allParts,true);

  for (FmPart* part : allParts)
  {
    proc = new FapModeShapeExpander(part,event,prepareForBatchOnly,last);
    if (last && modeOpts->autoVTFExport.getValue())
      vtfFile = proc->eventName(modeOpts->vtfFileName.getValue());
    FapSolutionProcessManager::instance()->pushSolverProcess(proc);
    last = false;
  }

  if (!vtfFile.empty())
    // Create VTF-file for modes results
    FapModeShapeExpander::writeVTFHeader(allParts,vtfFile);

  return FapSolutionProcessManager::instance()->run();
}

//---------------------------------------------------------------------------

bool FapSolveCmds::solveRosette(bool prepareForBatchOnly)
{
  if (!doGageRecovery(FmDB::getActiveAnalysis()))
    return false;

  FmSimulationEvent* event = FapSimEventHandler::getActiveEvent();
  if (event && !FapLicenseManager::checkSimEventLicense())
    return false;

  FuiModes::cancel();
  FapSolverBase* proc = NULL;

  std::vector<FmPart*> allParts;
  FmDB::getFEParts(allParts,true);

  for (FmPart* part : allParts)
  {
    proc = new FapGageRecovery(part,event,prepareForBatchOnly);
    FapSolutionProcessManager::instance()->pushSolverProcess(proc);
  }

  return FapSolutionProcessManager::instance()->run();
}

//----------------------------------------------------------------------------

bool FapSolveCmds::solveStrainCoat(bool prepareForBatchOnly)
{
  if (!doDamageRecovery(FmDB::getActiveAnalysis()))
    return false;

  FmSimulationEvent* event = FapSimEventHandler::getActiveEvent();
  if (event && !FapLicenseManager::checkSimEventLicense())
    return false;

  FuiModes::cancel();
  FapSolverBase* proc = NULL;

  std::vector<FmPart*> allParts;
  FmDB::getFEParts(allParts,true);

  for (FmPart* part : allParts)
  {
    proc = new FapDamageRecovery(part,event,prepareForBatchOnly);
    FapSolutionProcessManager::instance()->pushSolverProcess(proc);
  }

  return FapSolutionProcessManager::instance()->run();
}

//----------------------------------------------------------------------------

void FapSolveCmds::reducePart()
{
  std::vector<FmPart*> parts;
  FapCmdsBase::getSelectedParts(parts);

  FuiModes::cancel();

  for (FmPart* part : parts)
    FapSolutionProcessManager::instance()->pushSolverProcess(new FapLinkReducer(part));

  FapSolutionProcessManager::instance()->run();
}

//----------------------------------------------------------------------------

void FapSolveCmds::solvePart()
{
  std::vector<FmPart*> parts;
  FapCmdsBase::getSelectedParts(parts);

  FuiModes::cancel();

  for (FmPart* part : parts)
    FapSolutionProcessManager::instance()->pushSolverProcess(new FapLinkSolver(part));

  FapSolutionProcessManager::instance()->run();
}

//----------------------------------------------------------------------------

void FapSolveCmds::solveStressOnPart()
{
  FmAnalysis* analysis = FmDB::getActiveAnalysis();
  if (!doStressRecovery(analysis)) return;

  FapPartGroupsMap parts;
  FapCmdsBase::getSelectedPartsWithGroups(parts);

  FuiModes::cancel();
  FapStressExpander* proc = NULL;
  bool last = true;
  std::string vtfFile;

  std::vector<FmPart*> feParts;
  for (const FapPartGroupsMap::value_type& part : parts)
  {
    proc = new FapStressExpander(part.first,part.second,last);
    if (last && analysis->autoStressVTFExport.getValue())
      vtfFile = proc->eventName(analysis->stressVTFname.getValue());
    FapSolutionProcessManager::instance()->pushSolverProcess(proc);
    feParts.insert(feParts.begin(),part.first);
    last = false;
  }

  if (!vtfFile.empty())
    // Create VTF-file for stress results
    FapStressExpander::writeVTFHeader(feParts,vtfFile);

  FapSolutionProcessManager::instance()->run();
}

//----------------------------------------------------------------------------

void FapSolveCmds::solveRosetteOnPart()
{
  if (!doGageRecovery(FmDB::getActiveAnalysis())) return;

  std::vector<FmPart*> parts;
  FapCmdsBase::getSelectedParts(parts);

  FuiModes::cancel();

  for (FmPart* part : parts)
  {
    FapGageRecovery* proc = new FapGageRecovery(part);
    FapSolutionProcessManager::instance()->pushSolverProcess(proc);
  }

  FapSolutionProcessManager::instance()->run();
}

//----------------------------------------------------------------------------

void FapSolveCmds::solveStrainCoatOnPart()
{
  if (!doDamageRecovery(FmDB::getActiveAnalysis())) return;

  FapPartGroupsMap parts;
  FapCmdsBase::getSelectedPartsWithGroups(parts);

  FuiModes::cancel();

  for (const FapPartGroupsMap::value_type& part : parts)
  {
    FapDamageRecovery* proc = new FapDamageRecovery(part.first,part.second);
    FapSolutionProcessManager::instance()->pushSolverProcess(proc);
  }

  FapSolutionProcessManager::instance()->run();
}

//----------------------------------------------------------------------------

void FapSolveCmds::eraseSimuleRes()
{
  FpModelRDBHandler::RDBIncrement(FapSimEventHandler::getActiveRSD(),
				  FmDB::getMechanismObject());
  FmDB::getActiveAnalysis()->doRestart.setValue(false);

  if (FapSimEventHandler::getActiveEvent())
    FapSimEventHandler::getActiveEvent()->onChanged();
}

//----------------------------------------------------------------------------

void FapSolveCmds::eraseEventRes()
{
  if (FapLicenseManager::checkSimEventLicense()) {
    FapSimEventHandler::RDBIncrement(false);
    FmDB::getActiveAnalysis()->doRestart.setValue(false);
  }
}

//----------------------------------------------------------------------------

void FapSolveCmds::refreshRDB()
{
  FFaMsg::list("===> Refreshing the Result Data Base.\n");
  std::vector<std::string> addedFiles;
  FpModelRDBHandler::RDBSync(FapSimEventHandler::getActiveRSD(),
			     FmDB::getMechanismObject(),addedFiles,
			     true,false,true);
  if (FapSimEventHandler::getActiveEvent())
    FapSimEventHandler::getActiveEvent()->onChanged();

  if (addedFiles.empty()) return;

  FFaMsg::list("     Added files:");
  for (const std::string& file : addedFiles)
    FFaMsg::list("\n\t" + file);
  FFaMsg::list("\n",true);
}

//----------------------------------------------------------------------------

void FapSolveCmds::degradeSoil(double degradeTime)
{
#if defined(FT_HAS_WND) || defined(FT_HAS_RIS)
  if (FapOilWellCmds::degradeSoilSprings(degradeTime))
    FapSolveCmds::eraseSimuleRes();
#else
  // Dummy statement to suppress compiler warning
  std::cout <<"FapSolveCmds::degradeSoil("<< degradeTime <<") dummy"<< std::endl;
#endif
}

//----------------------------------------------------------------------------
#if FT_HAS_SAP > 1

/*!
  \brief Static helper executing a DTS command.
*/

static std::string runDtsCommand(const char* command, const std::string& args)
{
  // Lambda function returning an error message.
  auto&& error=[command](const char* msg = NULL)
  {
    std::string ret = " *** DTS command " + std::string(command) + " failed";
    if (msg)
      ret += ": " + std::string(msg);
    else
      ret += ".";
    return ret;
  };

  std::string dtsjob(FFaAppInfo::getProgramPath("dts"));
  if (dtsjob.empty()) return error("Not found");

  dtsjob += " " + std::string(command) + " " + args;

  FILE* fd = popen(dtsjob.c_str(),"r");
  if (!fd) return error("Not executable");

  bool failed = false;
  char cline[256];
  std::string output;
  while (fgets(cline,256,fd))
  {
#ifdef FAP_DEBUG
    std::cout <<"### DTS run: "<< cline << std::flush;
#endif
    if (!strncmp(cline,"FAILED",6))
      failed = true;
    else if (failed)
    {
      output = error(cline);
      break;
    }
    else
      output += cline;
  }

  pclose(fd);
  return output;
}

//------------------------------------------------------------------------------

static bool exportSimulationApp(const char* appname)
{
  FFaMsg::pushStatus("Export app " + std::string(appname));
  FmMechanism* mech = FmDB::getMechanismObject();

  // Copy template app-files
  std::string templPath = FpPM::getFullFedemPath("Templates/cloudsim");
  std::string appPath = FFaFilePath::appendFileNameToPath(mech->getAbsModelFilePath(), appname);
  bool ok = FpFileSys::verifyDirectory(appPath, true);
  ok &= FpFileSys::copyFile("app.json", templPath, appPath);
  ok &= FpFileSys::copyFile("resource-config.yml", templPath, appPath);

  std::string libPath = FFaFilePath::appendToPath(appPath, "lib");
  ok &= FpFileSys::verifyDirectory(libPath, true);
  ok &= FpFileSys::copyFile("driver.py", templPath, libPath);

  // Save model file with dependencies to app folder
  std::string currPath = mech->getModelFileName();
  std::string newFMMPath = libPath;
  FFaFilePath::appendToPath(newFMMPath, FFaFilePath::getFileName(currPath));
  ok &= FpPM::vpmModelExport(newFMMPath);
  FFaMsg::popStatus();
  return ok;
}


static bool pushAppToCloud(const char* appname)
{
  FFaMsg::pushStatus("Pushing app "+ std::string(appname) +" to cloud");
  std::string msg = runDtsCommand("push",appname);
  FFaMsg::popStatus();
  if (msg.find("OK") > msg.size())
  {
    FFaMsg::list(msg+"\n",true);
    return false;
  }

  std::cout << msg <<"App "<< appname
            <<" successfully pushed to the cloud.\n"<< std::endl;
  return true;
}


static int startAppWhenReady(const char* appname, std::string& jobId)
{
  // Check the app status in the cloud
  std::string msg = runDtsCommand("app",appname);
  if (msg.find("OK") < msg.size())
  {
    size_t ideps = msg.find("Deployment state");
    size_t istat = msg.find("ready");
    if (ideps >= istat || istat >= msg.size())
      return 0; // App is not ready to run yet
  }
  else if (msg.find("does not exist.") < msg.size())
    return 0; // App does not exist yet
  else
  {
    FFaMsg::list(msg+"\n",true);
    return -1;
  }

  std::cout << msg <<"App "<< appname
            <<" is ready to start in the cloud.\n"<< std::endl;

  // Start the simulation
  FFaMsg::pushStatus("Starting the simulation");
  msg = runDtsCommand("start-simulation",appname);
  if (msg.find("OK") > msg.size())
  {
    FFaMsg::list(msg+"\n",true);
    FFaMsg::popStatus();
    return -2;
  }

  size_t ijob = msg.find("Job ");
  size_t jjob = msg.find(" started for app");
  if (ijob >= jjob || jjob > msg.size())
  {
    // Should not happen, malformed output from dts
    FFaMsg::list("===> Could not extract job Id from:\n"+msg+"\n",true);
    FFaMsg::popStatus();
    return -3;
  }

  jobId = msg.substr(ijob+4,jjob-ijob-4);
  std::cout << msg <<"Job "<< jobId <<" for the app "
            << appname <<" has now started.\n"<< std::endl;
  ListUI <<" ==> Successfully started simulation job "<< appname
         <<" "<< jobId <<" in the cloud.\n";
  FFaMsg::popStatus();
  return 1;
}


static int checkCloudJob(const char* appname, const std::string& jobId)
{
  std::string msg = runDtsCommand("job",appname+std::string(" --job ")+jobId);
  if (msg.find("Job "+jobId+" not exist for app") < msg.size())
    return 0; // Workaround, in the beginning the job is not found (dts bug?)
  else if (msg.find("OK") > msg.size())
  {
    FFaMsg::list(msg+"\n",true);
    return -4;
  }

  size_t iStart = msg.find("Process state");
  size_t iState = msg.find("Completed\n");
  if (iState > iStart && iState < msg.size())
  {
    std::cout << msg <<"Job "<< jobId <<" for the app "
              << appname <<" has now finished.\n"<< std::endl;
    return 2; // Finished
  }

  iState = msg.find("Started\n");
  if (iState > iStart && iState < msg.size())
    return 1; // Still running

  iState = msg.find("Starting\n");
  if (iState > iStart && iState < msg.size())
    return 0; // Not started yet

  return -5; // Cannot tell, something is wrong
}


static bool downloadCloud(const char* appname, const std::string& jobId)
{
  FFaMsg::pushStatus("Downloading simulation results");
  FmMechanism* mech = FmDB::getMechanismObject();
  std::string path = mech->getAbsModelFilePath();
  std::string msg = runDtsCommand("download-job-output",
                                  appname+std::string(" --job ") + jobId +
                                  std::string(" --folder ") +
                                  FFaFilePath::appendToPath(path,"download"));
  FFaMsg::popStatus();
  if (msg.find("OK") > msg.size())
  {
    FFaMsg::list(msg+"\n",true);
    return false;
  }

  std::cout << msg <<"Results for job "<< jobId
            <<" successfully downloaded.\n"<< std::endl;
  ListUI <<" ==> Simulation results successfully downloaded to "
         << FFaFilePath::appendToPath(path,"results") <<"\n";

  // Move results from the download folder into the RDB folder of this model
  FmResultStatusData* topRSD = mech->getResultStatusData();
  if (topRSD->getTaskName() == "noname") topRSD->setTaskName("response");
  std::string newPath = topRSD->getCurrentTaskDirName(true);

  FpFileSys::verifyDirectory(topRSD->getPath(), true);

  if (FpFileSys::renameFile(path,newPath))
    ListUI <<"     Moved to "<< newPath << "\n";
  else
  {
    perror(newPath.c_str());
    return false;
  }

  // Remove the download folder (which now should be empty)
  return FpFileSys::removeDir(FFaFilePath::getPath(path,false), false);
}
#endif
//------------------------------------------------------------------------------

bool FapSolveCmds::haveCloud()
{
#if FT_HAS_SAP > 1
  if (!haveCloudAccess)
  {
    std::string msg = runDtsCommand("apps","");
    haveCloudAccess = msg.find("OK") < msg.size() ? 'y' : 'n';
    if (haveCloudAccess == 'n') std::cout << msg << std::endl;
  }
#endif

  return haveCloudAccess == 'y';
}

//------------------------------------------------------------------------------

void FapSolveCmds::solveInCloud()
{
#if FT_HAS_SAP > 1
  FuiModes::cancel();

  // Check that all FE parts have been reduced
  FmMechanism* mech = FmDB::getMechanismObject();
  std::vector<FmPart*> allParts;
  FmDB::getFEParts(allParts);
  bool ok = true;
  bool needMass = FmDB::getActiveAnalysis()->needMassMatrix();
  for (FmPart* part : allParts)
    if (part->setValidBaseFTLFile().empty())
    {
      ListUI <<"===> No FE data file for "<< part->getIdString(true) <<"\n";
      ok = false;
    }
    else if (Fedem::checkReducerFiles(part,needMass) <= 0)
    {
      ListUI <<" ==> "<< part->getIdString(true) <<" is not reduced\n";
      ok = false;
    }

  if (!ok)
  {
    FFaMsg::dialog("Some FE parts are not reduced.\n"
                   "They have to be explicitly reduced\n"
                   "before a cloud simulation can be performed.\n"
                   "Check Output List for details.",FFaMsg::ERROR);
    return;
  }

  // Create unique app-id
  long long currTimeSeconds = static_cast<long long>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
  ourCloudAppId = FFaLowerCaseString(FFaFilePath::getBaseName(mech->getModelFileName(), true) + std::to_string(currTimeSeconds));

  // Create simulation app-folder with content
  ok = exportSimulationApp(ourCloudAppId.c_str());
  if (!ok)
  {
    FFaMsg::dialog("Failed to export simulation app.\n"
                   "Check Output List for details.\n"
                   "Cloud simulation did not start.", FFaMsg::ERROR);
    return;
  }

  // Change working directory to App folder
  char* oldwd = getcwd(NULL,128);
  std::string appPath = mech->getAbsModelFilePath();
  if (chdir(FFaFilePath::appendToPath(appPath, ourCloudAppId).c_str()))
  {
    perror(appPath.c_str());
    free(oldwd);
    oldwd = NULL;
    return;
  }

  // Launch the dynamics solver in the cloud
  Fui::noUserInputPlease();
  ok = pushAppToCloud(ourCloudAppId.c_str());
  Fui::okToGetUserInput();

  if (oldwd)
  {
    // Reset working directory
    if (chdir(oldwd)) perror(oldwd);
    free(oldwd);
    oldwd = NULL;
  }

  if (ok)
    ListUI <<" ==> Successfully pushed simulation app "<< ourCloudAppId
           <<" to the cloud.\n";
  else
  {
    FFaMsg::dialog("Failed to push cloud simulation app.\n"
                   "Check Output List for details.",FFaMsg::ERROR);
    return;
  }

  if (!ourCloudJobTimer)
    ourCloudJobTimer = FFuaTimer::create(FFaDynCB0S(checkRunningCloudJob));

  int deltaT = 1000; // Check status every second by default
  FFaCmdLineArg::instance()->getValue("checkCloudInterval",deltaT);
  ourCloudJobTimer->start(deltaT);
#endif
}

//------------------------------------------------------------------------------

void FapSolveCmds::checkRunningCloudJob()
{
#if FT_HAS_SAP > 1
  const char* appName = ourCloudAppId.c_str();

  // Check job status in the cloud
  int status = 0;
  if (ourCloudJobId.empty())
    status = startAppWhenReady(appName,ourCloudJobId);
  else
    status = checkCloudJob(appName,ourCloudJobId);

  if (status == 0 || status == 1) return; // Still, or not yet running

  // The simulation job is finished
  ourCloudJobTimer->stop();
  ListUI <<" ==> Cloud simulation job "<< appName <<" "<< ourCloudJobId;
  if (status == 2)
    ListUI <<" finished.\n";
  else
    ListUI <<" failed ("<< status <<").\n";

  // Download result files and refresh GUI
  if (downloadCloud(appName,ourCloudJobId))
    FapSolveCmds::refreshRDB();

  // Clean up the app
  runDtsCommand("delete",appName+std::string(" -f"));

  FmMechanism* mech = FmDB::getMechanismObject();
  std::string path = mech->getAbsModelFilePath();
  FpFileSys::removeDir(FFaFilePath::appendToPath(path,appName));
  ourCloudAppId.clear();
  ourCloudJobId.clear();
#endif
}
