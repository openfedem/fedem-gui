// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAAdvAnalysisOptions.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUACommandHandler.H"
#include "vpmApp/vpmAppUAMap/FapUAQuery.H"
#include "vpmApp/FapLicenseManager.H"

#include "vpmUI/vpmUITopLevels/FuiAdvAnalysisOptions.H"
#include "vpmUI/Fui.H"

#include "vpmPM/FpPM.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmAnalysis.H"
#include "vpmDB/FmDofLoad.H"
#include "vpmDB/FmDofMotion.H"

#include "FFaLib/FFaOS/FFaFilePath.H"

const double pi = 3.1415926536;

Fmd_SOURCE_INIT(FUAUAADVANALYSISOPTIONS, FapUAAdvAnalysisOptions, FapUAExistenceHandler)


//----------------------------------------------------------------------------

FapUAAdvAnalysisOptions::FapUAAdvAnalysisOptions(FuiAdvAnalysisOptions* ui)
  : FapUAExistenceHandler(ui), FapUADataHandler(ui), FapUAFinishHandler(ui)
{
  Fmd_CONSTRUCTOR_INIT(FapUAAdvAnalysisOptions);
}
//----------------------------------------------------------------------------

FFuaUIValues* FapUAAdvAnalysisOptions::createValuesObject()
{
  return new FuaAdvAnalysisOptionsValues();
}
//----------------------------------------------------------------------------

void FapUAAdvAnalysisOptions::finishUI()
{
  Fui::analysisOptionsUI(false,true,false);
}
//----------------------------------------------------------------------------

void FapUAAdvAnalysisOptions::setDBValues(FFuaUIValues* values)
{
  FuaAdvAnalysisOptionsValues* advValues = (FuaAdvAnalysisOptionsValues*) values;
  FmAnalysis* analysis = FmDB::getActiveAnalysis();

  // Time options
  BoolMap&    timeOptStatus = advValues->valueStatus[FuiAdvAnalysisOptions::TIMEOPTIONS];
  BoolMap&    timeOpToggles = advValues->toggleValues[FuiAdvAnalysisOptions::TIMEOPTIONS];
  IntegerMap& timeIntValues = advValues->integerValues[FuiAdvAnalysisOptions::TIMEOPTIONS];
  DoubleMap&  timeOptValues = advValues->doubleValues[FuiAdvAnalysisOptions::TIMEOPTIONS];

  timeOptStatus[FuiAdvAnalysisOptions::START] = analysis->setStartTime(timeOptValues[FuiAdvAnalysisOptions::START]);
  timeOptStatus[FuiAdvAnalysisOptions::STOP]  = analysis->setEndTime(timeOptValues[FuiAdvAnalysisOptions::STOP]);

  analysis->setTimeIncrement(advValues->myTimeIncValue);
  analysis->setEngine(static_cast<FmEngine*>(advValues->mySelectedTimeEngine));
  timeOptStatus[FuiAdvAnalysisOptions::MIN_TIME_INCR] = analysis->setMinTimeIncrement(timeOptValues[FuiAdvAnalysisOptions::MIN_TIME_INCR]);

  timeOptStatus[FuiAdvAnalysisOptions::CUTBACK_FACTOR] = true;
  analysis->cutbackFactor.setValue(timeOptValues[FuiAdvAnalysisOptions::CUTBACK_FACTOR]);
  timeOptStatus[FuiAdvAnalysisOptions::CUTBACK_STEPS] = true;
  analysis->cutbackSteps.setValue(timeIntValues[FuiAdvAnalysisOptions::CUTBACK_STEPS]);
  analysis->doCutback.setValue(timeOpToggles[FuiAdvAnalysisOptions::CUTBACK]);

  timeOptStatus[FuiAdvAnalysisOptions::RESTART_TIME] = true;
  analysis->restartTime.setValue(timeOptValues[FuiAdvAnalysisOptions::RESTART_TIME]);
  analysis->doRestart.setValue(timeOpToggles[FuiAdvAnalysisOptions::RESTART]);
#ifndef FT_HAS_SOLVERS
  analysis->solverAddOpts.setValue(advValues->addOptions);
#endif

  // Integration options
  BoolMap&    intOptStatus = advValues->valueStatus[FuiAdvAnalysisOptions::INTOPTIONS];
  BoolMap&    intOpToggles = advValues->toggleValues[FuiAdvAnalysisOptions::INTOPTIONS];
  IntegerMap& intOptValues = advValues->integerValues[FuiAdvAnalysisOptions::INTOPTIONS];
  DoubleMap&  intOptReals  = advValues->doubleValues[FuiAdvAnalysisOptions::INTOPTIONS];

  if (intOpToggles[FuiAdvAnalysisOptions::HHT_ALPHA])
    analysis->newmarkDamping.setValue(FmAnalysis::HHT_ALPHA);
  else if (intOpToggles[FuiAdvAnalysisOptions::GENERALIZED_ALPHA])
    analysis->newmarkDamping.setValue(FmAnalysis::GENERALIZED_ALPHA);
  else
    analysis->newmarkDamping.setValue(FmAnalysis::NONE);
  analysis->ignoreTolerance.setValue(intOpToggles[FuiAdvAnalysisOptions::RADIO_IGNORE_TOL]);
  analysis->defaultShadowPosAlg.setValue(intOptValues[FuiAdvAnalysisOptions::SHADOW_POS_ALG]);
  analysis->useDynStressStiffening.setValue(intOpToggles[FuiAdvAnalysisOptions::DYN_STRESS_STIFF]);
  analysis->useMassCorrection.setValue(intOpToggles[FuiAdvAnalysisOptions::MOMENT_CORRECTION]);

  intOptStatus[FuiAdvAnalysisOptions::NUM_ITERS] = analysis->setForceNumIt(intOptValues[FuiAdvAnalysisOptions::NUM_ITERS]);
  intOptStatus[FuiAdvAnalysisOptions::MAX_NUM_ITERS] = analysis->setMaxNumIt(intOptValues[FuiAdvAnalysisOptions::MAX_NUM_ITERS]);
  intOptStatus[FuiAdvAnalysisOptions::MIN_NUM_ITERS] = analysis->setMinNumIt(intOptValues[FuiAdvAnalysisOptions::MIN_NUM_ITERS]);

  intOptStatus[FuiAdvAnalysisOptions::HHT_ALPHA] = analysis->setNewmarkDamping(intOptReals[FuiAdvAnalysisOptions::HHT_ALPHA],
                                                                               intOptReals[FuiAdvAnalysisOptions::GENERALIZED_ALPHA]);
  if (intOpToggles[FuiAdvAnalysisOptions::GENERALIZED_ALPHA])
    intOptStatus[FuiAdvAnalysisOptions::GENERALIZED_ALPHA] = intOptStatus[FuiAdvAnalysisOptions::HHT_ALPHA];

  analysis->useFixedMatrixUpdates.setValue(intOpToggles[FuiAdvAnalysisOptions::RADIO_FIXED_MATRIX_UPDATE]);
  intOptStatus[FuiAdvAnalysisOptions::MIN_MATRIX_UPDATE] = analysis->setMinMatrixUpdates(intOptValues[FuiAdvAnalysisOptions::MIN_MATRIX_UPDATE]);
  intOptStatus[FuiAdvAnalysisOptions::MAX_NO_MATRIX_UPDATE] = analysis->setMaxSequentialNoMatrixUpdates(intOptValues[FuiAdvAnalysisOptions::MAX_NO_MATRIX_UPDATE]);
  intOptStatus[FuiAdvAnalysisOptions::TOL_MATRIX_UPDATE] = analysis->setTolMatrixUpdateFactor(intOptReals[FuiAdvAnalysisOptions::TOL_MATRIX_UPDATE]);

  // Convergence options
  BoolMap&   convOptStatus = advValues->valueStatus[FuiAdvAnalysisOptions::CONVOPTIONS];
  BoolMap&   convOpToggles = advValues->toggleValues[FuiAdvAnalysisOptions::CONVOPTIONS];
  DoubleMap& convOptValues = advValues->doubleValues[FuiAdvAnalysisOptions::CONVOPTIONS];

  // Lambda function converting three bool toggles to an integer value in range [10,12]
  auto&& convertToggle = [&convOpToggles](int iNorm) -> int
  {
    for (int i = 0; i <= FmSolverConvergence::CONV_ALL_OF; i++)
      if (convOpToggles[iNorm+1+FmSolverConvergence::CONV_ALL_OF-i])
        return 10+i;
    return -iNorm;
  };

  convOptStatus[FuiAdvAnalysisOptions::SV_DIS] = analysis->setTolDisplacementNorm(convOptValues[FuiAdvAnalysisOptions::SV_DIS],
                                                                                  convertToggle(FuiAdvAnalysisOptions::SV_DIS));
  convOptStatus[FuiAdvAnalysisOptions::MT_DIS] = analysis->setTolDisplacementTra(convOptValues[FuiAdvAnalysisOptions::MT_DIS],
                                                                                 convertToggle(FuiAdvAnalysisOptions::MT_DIS));
  convOptStatus[FuiAdvAnalysisOptions::MR_DIS] = analysis->setTolDisplacementRot(convOptValues[FuiAdvAnalysisOptions::MR_DIS],
                                                                                 convertToggle(FuiAdvAnalysisOptions::MR_DIS));
  convOptStatus[FuiAdvAnalysisOptions::SV_VEL] = analysis->setTolVelocityNorm(convOptValues[FuiAdvAnalysisOptions::SV_VEL],
                                                                              convertToggle(FuiAdvAnalysisOptions::SV_VEL));
  convOptStatus[FuiAdvAnalysisOptions::SV_RES] = analysis->setTolResidualNorm(convOptValues[FuiAdvAnalysisOptions::SV_RES],
                                                                              convertToggle(FuiAdvAnalysisOptions::SV_RES));
  convOptStatus[FuiAdvAnalysisOptions::MT_RES] = analysis->setTolResidualTra(convOptValues[FuiAdvAnalysisOptions::MT_RES],
                                                                             convertToggle(FuiAdvAnalysisOptions::MT_RES));
  convOptStatus[FuiAdvAnalysisOptions::MR_RES] = analysis->setTolResidualRot(convOptValues[FuiAdvAnalysisOptions::MR_RES],
                                                                             convertToggle(FuiAdvAnalysisOptions::MR_RES));
  convOptStatus[FuiAdvAnalysisOptions::AVG_EN] = analysis->setTolEnergySum(convOptValues[FuiAdvAnalysisOptions::AVG_EN],
                                                                           convertToggle(FuiAdvAnalysisOptions::AVG_EN));
  convOptStatus[FuiAdvAnalysisOptions::MAX_EN] = analysis->setTolEnergyMax(convOptValues[FuiAdvAnalysisOptions::MAX_EN],
                                                                           convertToggle(FuiAdvAnalysisOptions::MAX_EN));

  // Eigen options
  BoolMap&    eigOptStatus = advValues->valueStatus[FuiAdvAnalysisOptions::EIGENOPTIONS];
  BoolMap&    eigOpToggles = advValues->toggleValues[FuiAdvAnalysisOptions::EIGENOPTIONS];
  IntegerMap& eigIntValues = advValues->integerValues[FuiAdvAnalysisOptions::EIGENOPTIONS];
  DoubleMap&  eigOptValues = advValues->doubleValues[FuiAdvAnalysisOptions::EIGENOPTIONS];

  analysis->setSolveEigenvalueFlag(eigOpToggles[FuiAdvAnalysisOptions::EMODE_SOL]);
  analysis->useEigStressStiffening.setValue(eigOpToggles[FuiAdvAnalysisOptions::EMODE_STRESS_STIFF]);
  analysis->dampedEigenvalues.setValue(eigOpToggles[FuiAdvAnalysisOptions::EMODE_DAMPED]);
  analysis->useBCsOnEigenvalues.setValue(eigOpToggles[FuiAdvAnalysisOptions::EMODE_BC]);

  eigOptStatus[FuiAdvAnalysisOptions::EMODE_INTV] = analysis->setEigenvalueSolutionTimeInterval(eigOptValues[FuiAdvAnalysisOptions::EMODE_INTV]);

  eigOptStatus[FuiAdvAnalysisOptions::EMODE_SHIFT_FACT] = true;
  analysis->eigenvalueShiftFactor.setValue((pi+pi)*eigOptValues[FuiAdvAnalysisOptions::EMODE_SHIFT_FACT]);

  eigOptStatus[FuiAdvAnalysisOptions::NUM_EMODES] = analysis->setRequestedEigenmodes(eigIntValues[FuiAdvAnalysisOptions::NUM_EMODES]);

  // Equilibrium options
  BoolMap&    eqOptStatus = advValues->valueStatus[FuiAdvAnalysisOptions::EQOPTIONS];
  BoolMap&    eqOpToggles = advValues->toggleValues[FuiAdvAnalysisOptions::EQOPTIONS];
  IntegerMap& eqIntValues = advValues->integerValues[FuiAdvAnalysisOptions::EQOPTIONS];
  DoubleMap&  eqOptValues = advValues->doubleValues[FuiAdvAnalysisOptions::EQOPTIONS];

  analysis->solveInitEquil.setValue(eqOpToggles[FuiAdvAnalysisOptions::EQL_ITER]);
  analysis->useEquStressStiffening.setValue(eqOpToggles[FuiAdvAnalysisOptions::EQL_STRESS_STIFF]);

  eqOptStatus[FuiAdvAnalysisOptions::EQL_ITER_TOL]   = analysis->setStaticEqulTol(eqOptValues[FuiAdvAnalysisOptions::EQL_ITER_TOL]);
  eqOptStatus[FuiAdvAnalysisOptions::ITER_STEP_SIZE] = analysis->setIterationStepReductionFactor(eqOptValues[FuiAdvAnalysisOptions::ITER_STEP_SIZE]);

  analysis->smoothRamp.setValue(eqOpToggles[FuiAdvAnalysisOptions::RAMP_UP]);
  analysis->rampGrav.setValue(eqOpToggles[FuiAdvAnalysisOptions::RAMP_GRAV]);
  eqOptStatus[FuiAdvAnalysisOptions::RAMP_STEPS] = analysis->setRampSteps(eqIntValues[FuiAdvAnalysisOptions::RAMP_STEPS]);
  eqOptStatus[FuiAdvAnalysisOptions::RAMP_VMAX] = analysis->setRampShape(eqOptValues[FuiAdvAnalysisOptions::RAMP_VMAX],
                                                                         eqOptValues[FuiAdvAnalysisOptions::RAMP_LENGTH]);
  eqOptStatus[FuiAdvAnalysisOptions::RAMP_LENGTH] = eqOptStatus[FuiAdvAnalysisOptions::RAMP_VMAX];
  eqOptStatus[FuiAdvAnalysisOptions::RAMP_DELAY] = analysis->setRampPause(eqOptValues[FuiAdvAnalysisOptions::RAMP_DELAY]);

  // Output options
  for (const std::pair<const int,bool>& outOpToggle : advValues->toggleValues[FuiAdvAnalysisOptions::OUTPUTOPTIONS])
    switch (outOpToggle.first) {
    case FuiAdvAnalysisOptions::AUTO_CURVE_EXPORT:
      analysis->autoCurveExportSwitch.setValue(outOpToggle.second);
      analysis->autoCurveExportFileName.setValue(advValues->autoCurveExportFileName);
      if (advValues->curveFileFormat >= 0)
        analysis->autoCurveExportFileFormat.setValue((FmAnalysis::ExportFileFormatType)advValues->curveFileFormat);
      break;
    case FuiAdvAnalysisOptions::AUTO_VTF_EXPORT:
      analysis->autoSolverVTFExport.setValue(outOpToggle.second && FapLicenseManager::hasVTFExportLicense());
      analysis->solverVTFname.setValue(advValues->autoVTFFileName);
      if (advValues->autoVTFFileType >= 0)
        analysis->solverVTFtype.setValue((VTFFileType)advValues->autoVTFFileType);
      break;
    case FuiAdvAnalysisOptions::AUTO_ANIM:
      analysis->autoAnimateSwitch.setValue(outOpToggle.second);
      break;
    case FuiAdvAnalysisOptions::OVERWRITE:
      analysis->overwriteResults.setValue(outOpToggle.second);
      break;
    }

  // Basic options
  BoolMap&   basOptStatus = advValues->valueStatus[FuiAdvAnalysisOptions::BASICOPTIONS];
  BoolMap&   basOpToggles = advValues->toggleValues[FuiAdvAnalysisOptions::BASICOPTIONS];
  DoubleMap& basOptValues = advValues->doubleValues[FuiAdvAnalysisOptions::BASICOPTIONS];

  analysis->dynamicsEnable.setValue(basOpToggles[FuiAdvAnalysisOptions::TIME_TOGGLE]);
  analysis->stopTimeEnable.setValue(basOpToggles[FuiAdvAnalysisOptions::STOP]);

  analysis->quasistaticEnable.setValue(basOpToggles[FuiAdvAnalysisOptions::QS_TOGGLE]);
  analysis->quasistaticMode.setValue(basOpToggles[FuiAdvAnalysisOptions::QS_UPTOTIME]);
  basOptStatus[FuiAdvAnalysisOptions::QS_UPTOTIME] = analysis->setQuasistaticUpToTime(basOptValues[FuiAdvAnalysisOptions::QS_UPTOTIME]);

  analysis->solveFrequencyDomain.setValue(basOpToggles[FuiAdvAnalysisOptions::FRA_TOGGLE]);

  // update sensitivities
  FapUACommandHandler::updateAllUICommandsSensitivity();

  FpPM::touchModel(); // Indicate that the model needs save
}
//----------------------------------------------------------------------------

void FapUAAdvAnalysisOptions::getDBValues(FFuaUIValues* values)
{
  FuaAdvAnalysisOptionsValues* advValues = (FuaAdvAnalysisOptionsValues*) values;
  FmAnalysis* analysis = FmDB::getActiveAnalysis();

  // Lambda function extracting a convergence option value and toggle from the analysis field
  auto&& getConvOption = [advValues](int iNorm, const FmSolverConvergence& tol)
  {
    advValues->doubleValues[FuiAdvAnalysisOptions::CONVOPTIONS][iNorm] = tol.value;
    for (int i = 1; i <= 3; i++)
      advValues->toggleValues[FuiAdvAnalysisOptions::CONVOPTIONS][iNorm+i] = i == 3 - tol.policy;
  };

  // Time options
  BoolMap&    timeOpToggles = advValues->toggleValues[FuiAdvAnalysisOptions::TIMEOPTIONS];
  IntegerMap& timeIntValues = advValues->integerValues[FuiAdvAnalysisOptions::TIMEOPTIONS];
  DoubleMap&  timeOptValues = advValues->doubleValues[FuiAdvAnalysisOptions::TIMEOPTIONS];

  timeOptValues[FuiAdvAnalysisOptions::START] = analysis->startTime.getValue();
  timeOptValues[FuiAdvAnalysisOptions::STOP]  = analysis->stopTime.getValue();
  advValues->myTimeIncValue = analysis->timeIncr.getValue();
  advValues->myEditTimeIncEngineCB = FFaDynCB1S(FapUAQuery::onQIFieldButtonCB,FuiQueryInputFieldValues&);
  timeOptValues[FuiAdvAnalysisOptions::MIN_TIME_INCR] = analysis->minTimeIncr.getValue();

  timeOpToggles[FuiAdvAnalysisOptions::CUTBACK]        = analysis->doCutback.getValue();
  timeOptValues[FuiAdvAnalysisOptions::CUTBACK_FACTOR] = analysis->cutbackFactor.getValue();
  timeIntValues[FuiAdvAnalysisOptions::CUTBACK_STEPS]  = analysis->cutbackSteps.getValue();

  timeOpToggles[FuiAdvAnalysisOptions::RESTART]      = analysis->doRestart.getValue();
  timeOptValues[FuiAdvAnalysisOptions::RESTART_TIME] = analysis->restartTime.getValue();

  advValues->myTimeIncQuery = FapUAEngineQuery::instance();
  advValues->mySelectedTimeEngine = analysis->getEngine();
#ifndef FT_HAS_SOLVERS
  advValues->addOptions = analysis->solverAddOpts.getValue();
#endif

  // Integration options
  BoolMap&    intOpToggles = advValues->toggleValues[FuiAdvAnalysisOptions::INTOPTIONS];
  IntegerMap& intOptValues = advValues->integerValues[FuiAdvAnalysisOptions::INTOPTIONS];
  DoubleMap&  intOptReals  = advValues->doubleValues[FuiAdvAnalysisOptions::INTOPTIONS];

  switch (analysis->newmarkDamping.getValue()) {
  case FmAnalysis::NONE:
    intOpToggles[FuiAdvAnalysisOptions::NEWMARK] = true;
    break;
  case FmAnalysis::HHT_ALPHA:
    intOpToggles[FuiAdvAnalysisOptions::HHT_ALPHA] = true;
    break;
  case FmAnalysis::GENERALIZED_ALPHA:
    intOpToggles[FuiAdvAnalysisOptions::GENERALIZED_ALPHA] = true;
    break;
  }
  intOptReals[FuiAdvAnalysisOptions::HHT_ALPHA]         = analysis->newmarkFactors.getValue().first;
  intOptReals[FuiAdvAnalysisOptions::GENERALIZED_ALPHA] = analysis->newmarkFactors.getValue().second;

  intOpToggles[FuiAdvAnalysisOptions::RADIO_IGNORE_TOL] = analysis->ignoreTolerance.getValue();

  intOptValues[FuiAdvAnalysisOptions::NUM_ITERS]     = analysis->fixedNumIt.getValue();
  intOptValues[FuiAdvAnalysisOptions::MAX_NUM_ITERS] = analysis->maxNumIt.getValue();
  intOptValues[FuiAdvAnalysisOptions::MIN_NUM_ITERS] = analysis->minNumIt.getValue();

  intOpToggles[FuiAdvAnalysisOptions::RADIO_FIXED_MATRIX_UPDATE] = analysis->useFixedMatrixUpdates.getValue();
  intOptValues[FuiAdvAnalysisOptions::MIN_MATRIX_UPDATE]         = analysis->minMatrixUpdates.getValue();
  intOptValues[FuiAdvAnalysisOptions::MAX_NO_MATRIX_UPDATE]      = analysis->maxSequentialNoMatrixUpdates.getValue();
  intOptReals[FuiAdvAnalysisOptions::TOL_MATRIX_UPDATE]          = analysis->tolMatrixUpdateFactor.getValue();

  intOptValues[FuiAdvAnalysisOptions::SHADOW_POS_ALG]    = analysis->defaultShadowPosAlg.getValue();
  intOpToggles[FuiAdvAnalysisOptions::DYN_STRESS_STIFF]  = analysis->useDynStressStiffening.getValue();
  intOpToggles[FuiAdvAnalysisOptions::MOMENT_CORRECTION] = analysis->useMassCorrection.getValue();

  // Convergence options

  getConvOption(FuiAdvAnalysisOptions::SV_DIS, analysis->tolDisplacementNorm.getValue());
  getConvOption(FuiAdvAnalysisOptions::MT_DIS, analysis->tolDisplacementTra.getValue());
  getConvOption(FuiAdvAnalysisOptions::MR_DIS, analysis->tolDisplacementRot.getValue());
  getConvOption(FuiAdvAnalysisOptions::SV_VEL, analysis->tolVelocityNorm.getValue());
  getConvOption(FuiAdvAnalysisOptions::SV_RES, analysis->tolResidualNorm.getValue());
  getConvOption(FuiAdvAnalysisOptions::MT_RES, analysis->tolResidualTra.getValue());
  getConvOption(FuiAdvAnalysisOptions::MR_RES, analysis->tolResidualRot.getValue());
  getConvOption(FuiAdvAnalysisOptions::AVG_EN, analysis->tolEnergySum.getValue());
  getConvOption(FuiAdvAnalysisOptions::MAX_EN, analysis->tolEnergyMax.getValue());

  // Eigen options
  BoolMap&    eigOpToggles = advValues->toggleValues[FuiAdvAnalysisOptions::EIGENOPTIONS];
  IntegerMap& eigIntValues = advValues->integerValues[FuiAdvAnalysisOptions::EIGENOPTIONS];
  DoubleMap&  eigOptValues = advValues->doubleValues[FuiAdvAnalysisOptions::EIGENOPTIONS];

  eigOpToggles[FuiAdvAnalysisOptions::EMODE_SOL]          = analysis->solveEigenvalues.getValue();
  eigOpToggles[FuiAdvAnalysisOptions::EMODE_DAMPED]       = analysis->dampedEigenvalues.getValue();
  eigOpToggles[FuiAdvAnalysisOptions::EMODE_BC]           = analysis->useBCsOnEigenvalues.getValue();
  eigOpToggles[FuiAdvAnalysisOptions::EMODE_STRESS_STIFF] = analysis->useEigStressStiffening.getValue();

  eigOptValues[FuiAdvAnalysisOptions::EMODE_INTV]       = analysis->eigenSolveTimeInterval.getValue();
  eigOptValues[FuiAdvAnalysisOptions::EMODE_SHIFT_FACT] = analysis->eigenvalueShiftFactor.getValue()/(pi+pi);

  eigIntValues[FuiAdvAnalysisOptions::NUM_EMODES] = analysis->numEigenmodes.getValue();

  // Equilibrium options
  BoolMap&    eqOpToggles = advValues->toggleValues[FuiAdvAnalysisOptions::EQOPTIONS];
  IntegerMap& eqIntValues = advValues->integerValues[FuiAdvAnalysisOptions::EQOPTIONS];
  DoubleMap&  eqOptValues = advValues->doubleValues[FuiAdvAnalysisOptions::EQOPTIONS];

  eqOpToggles[FuiAdvAnalysisOptions::EQL_ITER]         = analysis->solveInitEquil.getValue();
  eqOpToggles[FuiAdvAnalysisOptions::EQL_STRESS_STIFF] = analysis->useEquStressStiffening.getValue();

  eqOptValues[FuiAdvAnalysisOptions::EQL_ITER_TOL]   = analysis->staticEqlTol.getValue();
  eqOptValues[FuiAdvAnalysisOptions::ITER_STEP_SIZE] = analysis->iterStepLimit.getValue();

  eqOpToggles[FuiAdvAnalysisOptions::RAMP_UP]     = analysis->smoothRamp.getValue();
  eqOpToggles[FuiAdvAnalysisOptions::RAMP_GRAV]   = analysis->rampGrav.getValue();
  eqIntValues[FuiAdvAnalysisOptions::RAMP_STEPS]  = analysis->rampSteps.getValue();
  eqOptValues[FuiAdvAnalysisOptions::RAMP_VMAX]   = analysis->rampVmax.getValue();
  eqOptValues[FuiAdvAnalysisOptions::RAMP_LENGTH] = analysis->rampLength.getValue();
  eqOptValues[FuiAdvAnalysisOptions::RAMP_DELAY]  = analysis->rampPause.getValue();

  // Output options
  BoolMap& outputToggles = advValues->toggleValues[FuiAdvAnalysisOptions::OUTPUTOPTIONS];

  advValues->modelFilePath  = FmDB::getMechanismObject()->getAbsModelFilePath() + FFaFilePath::getPathSeparator();
  advValues->autoCurveExportFileName = analysis->autoCurveExportFileName.getValue();
  advValues->curveFileFormat = analysis->autoCurveExportFileFormat.getValue();
  advValues->autoVTFFileName = analysis->solverVTFname.getValue();
  advValues->autoVTFFileType = FapLicenseManager::checkVTFExportLicense(false) ? (int)analysis->solverVTFtype.getValue() : -1;
  outputToggles[FuiAdvAnalysisOptions::AUTO_CURVE_EXPORT] = analysis->autoCurveExportSwitch.getValue();
  outputToggles[FuiAdvAnalysisOptions::AUTO_VTF_EXPORT]   = analysis->autoSolverVTFExport.getValue();
  outputToggles[FuiAdvAnalysisOptions::AUTO_ANIM] = analysis->autoAnimateSwitch.getValue();
  outputToggles[FuiAdvAnalysisOptions::OVERWRITE] = analysis->overwriteResults.getValue();

  // Basic options
  BoolMap&    basOpToggles = advValues->toggleValues[FuiAdvAnalysisOptions::BASICOPTIONS];
  IntegerMap& basIntValues = advValues->integerValues[FuiAdvAnalysisOptions::BASICOPTIONS];
  DoubleMap&  basOptValues = advValues->doubleValues[FuiAdvAnalysisOptions::BASICOPTIONS];

  basOpToggles[FuiAdvAnalysisOptions::IEQ_TOGGLE] = analysis->solveInitEquil.getValue();

  basOpToggles[FuiAdvAnalysisOptions::TIME_TOGGLE] = analysis->dynamicsEnable.getValue();
  basOptValues[FuiAdvAnalysisOptions::START] = analysis->startTime.getValue();
  basOptValues[FuiAdvAnalysisOptions::STOP]  = analysis->stopTime.getValue();
  basOpToggles[FuiAdvAnalysisOptions::STOP]  = analysis->stopTimeEnable.getValue();

  basOpToggles[FuiAdvAnalysisOptions::QS_TOGGLE]   = analysis->quasistaticEnable.getValue();
  basOpToggles[FuiAdvAnalysisOptions::QS_COMPLETE] = !analysis->quasistaticMode.getValue();
  basOpToggles[FuiAdvAnalysisOptions::QS_UPTOTIME] = analysis->quasistaticMode.getValue();
  basOptValues[FuiAdvAnalysisOptions::QS_UPTOTIME] = analysis->quasistaticUpToTime.getValue();

  basOpToggles[FuiAdvAnalysisOptions::MODES_TOGGLE] = analysis->solveEigenvalues.getValue();
  basIntValues[FuiAdvAnalysisOptions::MODES_COUNT]  = analysis->numEigenmodes.getValue();

  std::vector<FmModelMemberBase*> objs;
  FmDB::getAllOfType(objs,FmDofLoad::getClassTypeID());
  for (FmModelMemberBase* obj : objs)
    if (static_cast<FmDofLoad*>(obj)->freqDomain.getValue())
    {
      advValues->haveFreqDomainLoads = true;
      break;
    }

  if (!advValues->haveFreqDomainLoads)
  {
    FmDB::getAllOfType(objs,FmDofMotion::getClassTypeID());
    for (FmModelMemberBase* obj : objs)
      if (static_cast<FmDofMotion*>(obj)->freqDomain.getValue())
      {
        advValues->haveFreqDomainLoads = true;
        break;
      }
  }

  basOpToggles[FuiAdvAnalysisOptions::FRA_TOGGLE] = advValues->haveFreqDomainLoads && analysis->solveFrequencyDomain.getValue();
}
