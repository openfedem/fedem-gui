// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUARDBMEFatigue.H"
#include "vpmApp/vpmAppCmds/FapGraphCmds.H"
#include "vpmApp/vpmAppDisplay/FapGraphDataMap.H"
#include "vpmApp/vpmAppProcess/FapSimEventHandler.H"
#include "vpmPM/FpModelRDBHandler.H"
#include "vpmUI/vpmUITopLevels/FuiRDBMEFatigue.H"
#include "vpmUI/Fui.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmCurveSet.H"
#include "vpmDB/FmSimulationEvent.H"
#include "vpmDB/FmMechanism.H"

#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFuLib/FFuProgressDialog.H"
#include "FFuLib/FFuTable.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFpLib/FFpFatigue/FFpSNCurveLib.H"
#include "FFpLib/FFpFatigue/FFpSNCurve.H"


Fmd_SOURCE_INIT(FcFAPUARDBMEFATIGUE, FapUARDBMEFatigue, FapUAExistenceHandler)

//----------------------------------------------------------------------------

FapUARDBMEFatigue::FapUARDBMEFatigue(FuiRDBMEFatigue* uic)
  : FapUAExistenceHandler(uic), FapUAFinishHandler(uic)
{
  Fmd_CONSTRUCTOR_INIT(FapUARDBMEFatigue);

  this->ui = uic;

  // Get selected graphs and curves
  bool dummy;
  std::vector<FmGraph*> selGraphs = FapGraphCmds::findSelectedGraphs(dummy);
  std::vector<FmCurveSet*> selCurves = FapGraphCmds::findSelectedCurves(dummy);

  // Add curves in selected graphs to selCurves
  size_t i, j;
  for (i = 0; i < selGraphs.size(); i++)
  {
    std::vector<FmCurveSet*> curves;
    selGraphs[i]->getCurveSets(curves);
    selCurves.insert(selCurves.end(),curves.begin(),curves.end());
  }

  // Get current active simulation event (if any), or all events if none active
  std::vector<FmSimulationEvent*> events(1,FapSimEventHandler::getActiveEvent());
  if (!events.front())
    FmDB::getAllSimulationEvents(events);

  FmMechanism* mech = FmDB::getMechanismObject();
  FapGraphDataMap eventData;

  // Table setup
  size_t curveCount = selCurves.size(); // in columns
  size_t eventCount = events.empty() ? 1 : events.size(); // in rows
  this->ui->tableMain->setTableReadOnly(true);
  this->ui->tableMain->setSelectionPolicy(4 /*NO_SELECTION*/);
  this->ui->tableMain->setNumberColumns(curveCount+3);
  this->ui->tableMain->setNumberRows(eventCount+1);
  this->ui->tableMain->showColumnHeader(true);
  this->ui->tableMain->showRowHeader(false);
  this->ui->tableMain->setColumnLabel(0, "Incl");
  this->ui->tableMain->setColumnWidth(0, 25);
  this->ui->tableMain->setColumnLabel(1, "Event");
  this->ui->tableMain->setColumnWidth(1, 40);
  this->ui->tableMain->setColumnLabel(curveCount+2, "Prob");
  this->ui->tableMain->setColumnWidth(curveCount+2, 80);
  for (i = 0; i < curveCount; i++) {
    this->ui->tableMain->setColumnLabel(i+2, selCurves[i]->getUserDescription());
    this->ui->tableMain->setColumnWidth(i+2, 80);
  }
  for (i = 0; i < eventCount; i++) {
    this->ui->tableMain->insertCheckBoxItem(i, 0, "", true);
    if (!events.empty())
      this->ui->tableMain->insertText(i, 1, FFaNumStr(events[i]->getID()));
  }
  this->ui->tableMain->insertText(eventCount, 1, "SUM");

  // Set callbacks
  this->ui->tableMain->setCellClickedCB(
    FFaDynCB2M(FapUARDBMEFatigue, this, onCellClicked, int, int));
  this->ui->useProbToggle->setToggleCB(
    FFaDynCB1M(FapUARDBMEFatigue, this, onUsePropToggle, bool));

  // Show progress dialog
  FFuProgressDialog* progDlg =
    FFuProgressDialog::create("Please wait...",
                              "Cancel",
                              "Calculating Damage",
                              eventCount);

  // Used to get overall values:
  //  -1=varying, other=value
  double startTimeAll = 0.0;
  double stopTimeAll = 0.0;
  int    snStandardAll = 0;
  int    snCurveAll = 0;

  // Allocate data arrays
  damage.resize(curveCount,std::vector<double>(eventCount,-1.0));
  probability.resize(eventCount,1.0);

  // Calculate weighted damage, event by event
  for (i = 0; i < eventCount; i++)
  {
    progDlg->setCurrentProgress(i);
    if (progDlg->userCancelled()) break;

    if (!events.empty() && !FapSimEventHandler::getActiveEvent()) {
      FpModelRDBHandler::RDBRelease();
      FpModelRDBHandler::RDBOpen(events[i]->getResultStatusData(),mech);
    }

    // Set event probability value
    probability[i] = events.empty() ? 1.0 : events[i]->getProbability();

    // Update table with probability value
    this->ui->tableMain->insertText(i, curveCount+2,
      FFaNumStr(probability[i], 1, 8, 1.0e+7, 1.0e-5, true));

    // Process all selected curves
    for (j = 0; j < selCurves.size(); j++)
    {
      FmCurveSet* pCurve = selCurves[j];

      // Get curve attributes
      double startTime = pCurve->getFatigueDomain().first;
      double stopTime = pCurve->getFatigueDomain().second;
      int snStandard = pCurve->getFatigueSNStd();
      int snCurve = pCurve->getFatigueSNCurve();
      FFpSNCurve* snC = FFpSNCurveLib::instance()->getCurve(snStandard,snCurve);

      // Calculate overall values
      if (j == 0) {
        startTimeAll = startTime;
        stopTimeAll = stopTime;
        snStandardAll = snStandard;
        snCurveAll = snCurve;
      }
      else {
        if (startTimeAll >= 0.0 && startTimeAll != startTime)
          startTimeAll = -1.0;
        if (stopTimeAll >= 0.0 && stopTimeAll != stopTime)
          stopTimeAll = -1.0;
        if (snStandardAll >= 0 && snStandardAll != snStandard)
          snStandardAll = -1;
        if (snCurveAll >= 0 && snCurveAll != snCurve)
          snCurveAll = -1;
      }

      // Turn off DFT
      FmCurveSet::Analysis tmpAF = pCurve->getAnalysisFlag();
      pCurve->setAnalysisFlag(FmCurveSet::NONE, false);

      // Calculate damage
      std::string errMsg;
      eventData.findPlottingData(pCurve, errMsg);
      if (errMsg.empty() && snC)
        damage[j][i] = eventData.getDamageFromCurve(pCurve, pCurve->getFatigueGateValue(),
                                                    true, pCurve->getFatigueEntireDomain(),
                                                    startTime, stopTime, *snC);
      if (damage[j][i] < 0.0) {
        ListUI <<"===> Damage calculation failed";
        if (!events.empty())
          ListUI <<" for "<< events[i]->getIdString();
        ListUI <<".";
        if (!snC)
          ListUI <<"\n     Invalid SN-curve: StdIndex="<< snStandard
                 <<" CurveIndex="<< snCurve;
        if (!errMsg.empty()) ListUI <<"\n     "<< errMsg;
        FFaMsg::list("\n",true);
        damage[j][i] = 0.0;
      }

      // Reset data analysis flag
      pCurve->setAnalysisFlag(tmpAF, false);
    }
  }

  // Set progress
  progDlg->setCurrentProgress(eventCount);
  delete progDlg;

  if (!events.empty() && !FapSimEventHandler::getActiveEvent()) {
    FpModelRDBHandler::RDBRelease();
    FpModelRDBHandler::RDBOpen(mech->getResultStatusData(),mech);
  }

  // Set overall fields
  if (startTimeAll >= 0.0)
    this->ui->startTimeField->setValue(startTimeAll);
  else
    this->ui->startTimeField->setValue("Varying");
  if (stopTimeAll >= 0.0)
    this->ui->stopTimeField->setValue(stopTimeAll);
  else
    this->ui->stopTimeField->setValue("Varying");
  if (snStandardAll >= 0 && snCurveAll >= 0) {
    FFpSNCurve* snC = FFpSNCurveLib::instance()->getCurve(snStandardAll,snCurveAll);
    if (snC) {
      if (snC->getStdId() == FFpSNCurve::NORSOK)
        this->ui->standardField->setValue("Norsok");
      else if (snC->getStdId() == FFpSNCurve::BRITISH)
        this->ui->standardField->setValue("British");
      else
        this->ui->standardField->setValue("Error");
      this->ui->snCurveField->setValue(snC->getName());
    }
    else {
      this->ui->standardField->setValue("Error");
      this->ui->snCurveField->setValue("Error");
    }
  }
  else {
    this->ui->standardField->setValue("Varying");
    this->ui->snCurveField->setValue("Varying");
  }

  // Update values
  this->calcTableValues();
}
//----------------------------------------------------------------------------

void FapUARDBMEFatigue::finishUI()
{
  Fui::rdbMEFatigueUI(false,true);
}
//----------------------------------------------------------------------------

void FapUARDBMEFatigue::onCellClicked(int row, int col)
{
  // Toggle check boxes on/off
  if (col == 0 && row < (int)probability.size()) {
    bool t = this->ui->tableMain->getCheckBoxItemToggle(row, col);
    this->ui->tableMain->setCheckBoxItemToggle(row, col, !t);
    this->calcTableValues();
  }
}
//----------------------------------------------------------------------------

void FapUARDBMEFatigue::onUsePropToggle(bool)
{
  this->calcTableValues();
}
//----------------------------------------------------------------------------

void FapUARDBMEFatigue::calcTableValues()
{
  // Set damage values in table
  for (size_t j = 0; j < damage.size(); j++)
  {
    double sumDamage = 0.0;
    double sumProbabilities = 0.0;
    int sumProbabilitiesCount = 0;

    for (size_t i = 0; i < probability.size() && i < damage[j].size(); i++)
    {
      // Get weighted damage of this event
      double prob = this->ui->useProbToggle->getValue() ? probability[i] : 1.0;

      // Update table with damage values (w/wo probability adjustment)
      this->ui->tableMain->insertText(i, j+2,
        FFaNumStr(damage[j][i]*prob, 1, 8, 1.0e+7, 1.0e-5, true));

      // Calculate sum from all toggled rows
      if (this->ui->tableMain->getCheckBoxItemToggle(i,0)) {
        sumDamage += damage[j][i] * prob;
        sumProbabilities += prob;
        sumProbabilitiesCount++;
      }
    }

    // Update table with sum damage values
    this->ui->tableMain->insertText(probability.size(), j+2,
      FFaNumStr(sumDamage, 1, 8, 1.0e+7, 1.0e-5, true));

    // Update table with sum/average probabilities values
    if (sumProbabilitiesCount > 0)
      sumProbabilities /= sumProbabilitiesCount;
    this->ui->tableMain->insertText(probability.size(), damage.size()+2,
      FFaNumStr(sumProbabilities, 1, 8, 1.0e+7, 1.0e-5, true));
  }
}
