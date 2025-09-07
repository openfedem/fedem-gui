// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAFppOptions.H"
#include "vpmApp/FapLicenseManager.H"
#include "vpmUI/vpmUITopLevels/FuiFppOptions.H"
#include "vpmUI/Fui.H"
#include "vpmDB/FmFppOptions.H"
#include "vpmDB/FmAnalysis.H"
#include "vpmDB/FmDB.H"
#include "vpmPM/FpPM.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"


Fmd_SOURCE_INIT(FAPUAFPPOPTIONS, FapUAFppOptions, FapUAExistenceHandler);


//----------------------------------------------------------------------------

FapUAFppOptions::FapUAFppOptions(FuiFppOptions* uic)
  : FapUAExistenceHandler(uic), FapUADataHandler(uic), FapUAFinishHandler(uic)
{
  Fmd_CONSTRUCTOR_INIT(FapUAFppOptions);

  this->ui = uic;

  ui->setResetTimeCB(FFaDynCB0M(FapUAFppOptions,this,resetTime));
}
//----------------------------------------------------------------------------

FFuaUIValues* FapUAFppOptions::createValuesObject()
{
  return new FuaFppOptionsValues();
}
//----------------------------------------------------------------------------

void FapUAFppOptions::setDBValues(FFuaUIValues* values)
{
  FmFppOptions* dbfppOptions = FmDB::getFppOptions();

  FuaFppOptionsValues* fppValues = (FuaFppOptionsValues*) values;

  dbfppOptions->startTime = fppValues->timeValues.start;
  dbfppOptions->stopTime = fppValues->timeValues.stop;
  dbfppOptions->timeIncr = fppValues->timeValues.incr;
  dbfppOptions->allTimeSteps = fppValues->timeValues.allSteps;
  dbfppOptions->nElemsTogether = fppValues->nElemsTogether;
  dbfppOptions->performRainflow = fppValues->performRainflow;
  dbfppOptions->pvxGate = fppValues->pvxGate;
  dbfppOptions->biaxGate = fppValues->biaxGate;
  if (fppValues->histType >= 0) {
    dbfppOptions->histType = (FmFppOptions::RainFlowType)(fppValues->histType+1);
    dbfppOptions->histRange = std::make_pair(fppValues->min,fppValues->max);
    dbfppOptions->histNBins = fppValues->nBins;
  }

  FpPM::touchModel(); // Indicate that the model needs save
}
//----------------------------------------------------------------------------

void FapUAFppOptions::getDBValues(FFuaUIValues* values)
{
  // Don't create a fpp options object here, if we don't have any yet
  FmFppOptions* dbfppOptions = FmDB::getFppOptions(false);
  // Create a temporary object to get default values
  if (!dbfppOptions) dbfppOptions = new FmFppOptions();

  FuaFppOptionsValues* fppValues = (FuaFppOptionsValues*) values;

  fppValues->timeValues.start = dbfppOptions->startTime.getValue();
  fppValues->timeValues.stop = dbfppOptions->stopTime.getValue();
  fppValues->timeValues.incr = dbfppOptions->timeIncr.getValue();
  fppValues->timeValues.allSteps = dbfppOptions->allTimeSteps.getValue();
  fppValues->nElemsTogether = dbfppOptions->nElemsTogether.getValue();
  fppValues->performRainflow = dbfppOptions->performRainflow.getValue();
  fppValues->pvxGate = dbfppOptions->pvxGate.getValue();
  fppValues->biaxGate = dbfppOptions->biaxGate.getValue();

  // Delete the temporary fpp options object
  if (!FmDB::getFppOptions(false)) dbfppOptions->erase();
}
//----------------------------------------------------------------------------

void FapUAFppOptions::resetTime()
{
  FmAnalysis* analysis = FmDB::getActiveAnalysis();
  if (!analysis) return;

  FuaTimeIntervalValues timeValues;

  timeValues.start = analysis->startTime.getValue();
  timeValues.stop = analysis->stopTime.getValue();
  timeValues.dontTouchIncr = true;
  timeValues.allSteps = true;

  this->ui->setTimeUIValues(&timeValues);
}
//----------------------------------------------------------------------------

void FapUAFppOptions::finishUI()
{
  Fui::fppOptionsUI(false,true);
}
//----------------------------------------------------------------------------
