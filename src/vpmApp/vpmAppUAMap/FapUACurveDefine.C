// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUACurveDefine.H"
#include "vpmApp/vpmAppUAMap/FapUACurveAxisDefinition.H"
#include "vpmApp/vpmAppUAMap/FapUAProperties.H"
#include "vpmApp/vpmAppUAMap/FapUARDBSelector.H"
#include "vpmApp/vpmAppUAMap/FapUARDBMEFatigue.H"
#include "vpmApp/vpmAppUAMap/FapUAQuery.H"
#include "vpmApp/vpmAppUAMap/FapUAGraphView.H"
#include "vpmApp/vpmAppCmds/FapGraphCmds.H"
#include "vpmApp/vpmAppProcess/FapSimEventHandler.H"

#include "vpmPM/FpModelRDBHandler.H"

#include "vpmUI/vpmUIComponents/FuiCurveDefine.H"
#include "vpmUI/vpmUIComponents/FuiCurveAxisDefinition.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmSimulationEvent.H"
#include "vpmDB/FmCurveSet.H"

#include "FFpLib/FFpFatigue/FFpSNCurveLib.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"

#include "FFaMathExpr/FFaMathExprFactory.H"

#include "FFuLib/FFuProgressDialog.H"
#include "FiDeviceFunctions/FiDeviceFunctionFactory.H"
#include "FiDeviceFunctions/FiASCFile.H"


Fmd_SOURCE_INIT(FAPUACURVEDEFINE, FapUACurveDefine, FapUAExistenceHandler);

//----------------------------------------------------------------------------

FapUACurveDefine::FapUACurveDefine(FuiCurveDefine* uic)
 : FapUAExistenceHandler(uic), FapUADataHandler(uic),
   selSignalConnector(this), stackSignalConnector(this), signalConnector(this)
{
  Fmd_CONSTRUCTOR_INIT(FapUACurveDefine);

  //get curveAxisDefinition ua's
  for (int d = 0; d < FuiCurveDefine::NDIRS; d++)
    this->axesUA[d] = (FapUACurveAxisDefinition*) uic->getAxisUI(d)->getUA();

  //set callbacks
  //from ui
  uic->setAutoLegendCB(FFaDynCB1M(FapUACurveDefine,this,setAutoLegend,bool));
  uic->setProvideChannelListCB(FFaDynCB1M(FapUACurveDefine,this,getChannelList,const std::string&));
  uic->setReloadCurveCB(FFaDynCB0M(FapUACurveDefine,this,reloadCurve));

  uic->setGetCurveStatisticsCB(FFaDynCB4M(FapUACurveDefine,this,fetchStatistics,bool,bool,double,double));
  uic->setApplyVerticalMarkersCB(FFaDynCB2M(FapUACurveDefine,this,applyVerticalMarkers,double,double));
  uic->setRemoveVerticalMarkersCB(FFaDynCB0M(FapUACurveDefine,this,removeVerticalMarkers));

  uic->setGetDamageFromCurveCB(FFaDynCB4M(FapUACurveDefine,this,fetchCurveDamage,bool,bool,double,double));

  //from ua
  this->axesUA[FuiCurveDefine::X]->setAxisCompletedCB(FFaDynCB0M(FapUACurveDefine,this,onAxisCompleted));
  this->axesUA[FuiCurveDefine::X]->setEditAxisCB(FFaDynCB0M(FapUACurveDefine,this,onEditXAxis));

  this->axesUA[FuiCurveDefine::Y]->setAxisCompletedCB(FFaDynCB0M(FapUACurveDefine,this,onAxisCompleted));
  this->axesUA[FuiCurveDefine::Y]->setEditAxisCB(FFaDynCB0M(FapUACurveDefine,this,onEditYAxis));

  this->ui = uic;
  this->dbcurve = this->cachedDBCurve = NULL;
  this->ignoreEverythingButCurves = false;
  this->IAmEditingCurveAxis = false;
  this->wait = false;
}

//----------------------------------------------------------------------------

FFuaUIValues* FapUACurveDefine::createValuesObject()
{
  return new FuaCurveDefineValues();
}

/*!
  This function is called when the users sets a multi-event simulation active.
  This is basically a (bug fix) approach for deleting/ignoring the cached
  curve so that we can see the actual (e.g. damage) properties of the curve
  after the user has chosen a new simulation event.
*/

void FapUACurveDefine::clearCachedDBCurve(FmCurveSet* curve)
{
  std::vector<FapUAExistenceHandler*> uas;
  FapUAExistenceHandler::getAllOfType(FapUACurveDefine::getClassTypeID(),uas);
  for (FapUAExistenceHandler* ua : uas)
    static_cast<FapUACurveDefine*>(ua)->resetCachedDBCurve(curve);
}

void FapUACurveDefine::resetCachedDBCurve(FmCurveSet* curve)
{
  if (!curve || curve == this->cachedDBCurve)
    this->cachedDBCurve = NULL;
}

//----------------------------------------------------------------------------

void FapUACurveDefine::onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
					      const std::vector<FFaViewItem*>&,
					      const std::vector<FFaViewItem*>&)
{
  FmCurveSet* curve = NULL;
  size_t i = totalSelection.size();
  while (i > 0 && !curve)
    curve = dynamic_cast<FmCurveSet*>(totalSelection[--i]);

  if (curve) // we have (at least) one selected curve
  {
    // If multi-selection, find all selected curves
    this->dbcurve = curve;
    this->mySelectedCurves.clear();
    for (size_t j = 0; j < i; j++)
      this->mySelectedCurves.push_back(dynamic_cast<FmCurveSet*>(totalSelection[j]));

    if (this->dbcurve != this->cachedDBCurve)
    {
      this->curveData.clear();
      this->updateUIValues();
      this->cachedDBCurve = this->dbcurve;
    }
  }
  else if (!this->ignoreEverythingButCurves)
  {
    this->dbcurve = NULL;
    this->mySelectedCurves.clear();
  }
}

//----------------------------------------------------------------------------

void FapUACurveDefine::onPermSelectionStackChanged(bool pushed)
{
  if (!IAmEditingCurveAxis) return;

  this->ignoreEverythingButCurves = pushed;
  FapUAProperties* uap = FapUAProperties::getPropertiesHandler();
  if (uap) uap->setIgnorePickNotifyNotCurves(pushed);
  if (!pushed) IAmEditingCurveAxis = false;
}

//----------------------------------------------------------------------------

void FapUACurveDefine::updateUIValues()
{
  if (!this->dbcurve) return;

  this->FapUADataHandler::updateUI();

  int d = FuiCurveDefine::X;
  if (this->dbcurve->getOwnerGraph()->isBeamDiagram())
    d = FuiCurveDefine::Y;

  for (; d < FuiCurveDefine::NDIRS; d++)
    this->axesUA[d]->setAxisData(this->dbcurve->getResult(d),
				 this->dbcurve->getResultOper(d));
}

//----------------------------------------------------------------------------

void FapUACurveDefine::setDBValues(FFuaUIValues* values)
{
  if (!this->dbcurve) return;

  FuaCurveDefineValues* curveValues = (FuaCurveDefineValues*) values;

  this->dbcurve->setLegend(curveValues->legend);
  this->dbcurve->setAutoLegend(curveValues->autoLegend);

  this->dbcurve->useInputMode(static_cast<FmCurveSet::InputMode>(curveValues->inputMode));
  this->dbcurve->setToBeExportedBatch(curveValues->autoExport);

  // --- Appearance sheet ---
  this->dbcurve->setColor(curveValues->color);
  this->dbcurve->setCurveStyle(curveValues->curveType);
  this->dbcurve->setCurveWidth(curveValues->curveWidth);
  this->dbcurve->setCurveSymbol(curveValues->curveSymbol);
  this->dbcurve->setCurveSymbolSize(curveValues->symbolSize);
  this->dbcurve->setCurveNumSymbols(curveValues->numSymbols);

  // --- Spatial result curves ---
  if (curveValues->inputMode == FmCurveSet::SPATIAL_RESULT) {
    this->dbcurve->setTimeRange(curveValues->timeRange);
    this->dbcurve->setTimeOper(curveValues->timeOper);
    this->dbcurve->setResultOper(FmCurveSet::XAXIS,curveValues->spaceOper);
  }

  // --- Combined curves ---
  size_t ncc = FFaMathExprFactory::countArgs(curveValues->expression,
                                             FmCurveSet::getCompNames());
  int retVal = FFaMathExprFactory::instance()->create(this->dbcurve->getBaseID(),
						      curveValues->expression,ncc,
						      FmCurveSet::getCompNames());
  if (retVal > 0)
    this->dbcurve->setExpression(curveValues->expression);
  else if (retVal == -4)
    FFaMsg::dialog("Kommas ',' in math expressions are not allowed.\n"
		   "Use only dots '.' as decimal markers.",FFaMsg::ERROR);
  else if (!curveValues->expression.empty())
    ListUI <<"ERROR: Invalid expression for Curve "<< this->dbcurve->getID()
	   <<":\n'"<< curveValues->expression <<"'.\n";

  size_t i = 0;
  for (FmModelMemberBase* comp : curveValues->curveComps)
    if (i < ncc) this->dbcurve->setCurveComp(dynamic_cast<FmCurveSet*>(comp),i++);

  // --- From file ---
  const std::string& mPath = FmDB::getMechanismObject()->getAbsModelFilePath();
  bool newFile = this->dbcurve->setFilePath(curveValues->filePath);
  switch (FiDeviceFunctionFactory::identify(curveValues->filePath,mPath)) {
  case ASC_FILE:
    if (newFile) {
      std::vector<std::string> channels;
      std::string fName(curveValues->filePath);
      FFaFilePath::makeItAbsolute(fName,mPath);
      FiASCFile reader(fName.c_str());
      if (reader.open(FiDeviceFunctionBase::Read_Only))
        if (reader.getChannelList(channels) && !channels.empty() && channels.front() != "1")
          curveValues->channel = channels.front();
    }
  case RPC_TH_FILE:
  case ASC_MC_FILE:
    this->dbcurve->setChannelName(curveValues->channel);
  default:
    break;
  }
  if (this->dbcurve->getUserDescription().empty())
    if (!curveValues->channel.empty() && curveValues->channel != "Not set")
      this->dbcurve->setUserDescription(curveValues->channel);

  // --- Internal function or preview curve ---
  if (!this->dbcurve->setFunctionRef(curveValues->functionRef))
    this->dbcurve->setUseSmartPoints(curveValues->functionDomain.autoInc);
  this->dbcurve->setFuncDomain(curveValues->functionDomain.X);
  this->dbcurve->setIncX(curveValues->functionDomain.dX);

  // --- Scale and Shift sheet ---
  this->dbcurve->setScaleFactor(curveValues->scaleX, curveValues->scaleY);
  this->dbcurve->setOffset(curveValues->offsetX, curveValues->offsetY);
  this->dbcurve->setZeroAdjust(curveValues->zeroAdjustX, curveValues->zeroAdjustY);

  // --- Fourier Analysis sheet ---
  this->dbcurve->setAnalysisFlag(static_cast<FmCurveSet::Analysis>(curveValues->analysis));
  this->dbcurve->setDftDomain(curveValues->dftDomain);
  this->dbcurve->setDftRemoveComp(curveValues->dftRemoveComp);
  this->dbcurve->setDftEntireDomain(curveValues->dftEntireDomain);
  this->dbcurve->setDftResample(curveValues->dftResample);
  this->dbcurve->setDftResampleRate(curveValues->dftResampleRate);

  // --- Fatigue sheet ---
  this->dbcurve->setFatigueGateValue(curveValues->fatigueGateValue);
  this->dbcurve->setFatigueDomain(curveValues->fatigueDomain);
  this->dbcurve->setFatigueEntireDomain(curveValues->fatigueEntireDomain);
  // Update the Fatigue values for feasable curves only, to avoid
  // destroying the default values set in the FmCurveSet constructor
  if (curveValues->fatigueFeasable) {
    this->dbcurve->setFatigueLifeUnit(static_cast<FmCurveSet::LifeUnit>(curveValues->fatigueLifeUnit));
    this->dbcurve->setFatigueSNCurve(curveValues->fatigueSNCurve);
    this->dbcurve->setFatigueSNStd(curveValues->fatigueSNStd);
  }

  this->dbcurve->onDataChanged();

  // Now update the other curves in the current selection.
  // Only some fields of the currently selected tab will be updated.
  for (FmCurveSet* curve : mySelectedCurves)
    FapUACurveDefine::setMultiselectionDBValues(curve,curveValues);
}

//----------------------------------------------------------------------------

void FapUACurveDefine::setMultiselectionDBValues(FmCurveSet* curve,
						 FuaCurveDefineValues* curveValues)
{
  if (!curve) return;

  // Only some of the fields will be updated, typically those
  // likely to have the same value for a large set of curves.
  // The curve data definition, for instance, is not updated in this method.

  switch (curveValues->selectedTabIdx) {

  case 0: // --- Data sheet ---
    curve->setAutoLegend(curveValues->autoLegend);
    curve->setToBeExportedBatch(curveValues->autoExport);

    // --- Spatial result curves ---
    if (curve->usingInputMode() == FmCurveSet::SPATIAL_RESULT &&
        curveValues->inputMode == FmCurveSet::SPATIAL_RESULT) {
      curve->setTimeRange(curveValues->timeRange);
      curve->setTimeOper(curveValues->timeOper);
      curve->setResultOper(FmCurveSet::XAXIS,curveValues->spaceOper);
    }

    // --- Internal function or preview curve ---
    if (curve->usingInputMode() >= FmCurveSet::INT_FUNCTION) {
      curve->setUseSmartPoints(curveValues->functionDomain.autoInc);
      curve->setFuncDomain(curveValues->functionDomain.X);
      curve->setIncX(curveValues->functionDomain.dX);
    }
    break;

  case 1: // --- Appearance sheet ---
    curve->setColor(curveValues->color);
    curve->setCurveStyle(curveValues->curveType);
    curve->setCurveWidth(curveValues->curveWidth);
    curve->setCurveSymbol(curveValues->curveSymbol);
    curve->setCurveSymbolSize(curveValues->symbolSize);
    curve->setCurveNumSymbols(curveValues->numSymbols);
    break;

  case 3: // --- Scale and Shift sheet ---
    curve->setScaleFactor(curveValues->scaleX, curveValues->scaleY);
    curve->setOffset(curveValues->offsetX, curveValues->offsetY);
    curve->setZeroAdjust(curveValues->zeroAdjustX, curveValues->zeroAdjustY);
    break;

  case 4: // --- Fourier Analysis sheet ---
    curve->setAnalysisFlag(static_cast<FmCurveSet::Analysis>(curveValues->analysis));
    curve->setDftDomain(curveValues->dftDomain);
    curve->setDftRemoveComp(curveValues->dftRemoveComp);
    curve->setDftEntireDomain(curveValues->dftEntireDomain);
    curve->setDftResample(curveValues->dftResample);
    curve->setDftResampleRate(curveValues->dftResampleRate);
    break;

  default: // Nothing to update in the other sheets
    return;
  }

  curve->onDataChanged();
}

//----------------------------------------------------------------------------

void FapUACurveDefine::getDBValues(FFuaUIValues* values)
{
  if (!this->dbcurve) return;

  FuaCurveDefineValues* curveValues = (FuaCurveDefineValues*) values;

  curveValues->legend = this->dbcurve->getLegend();
  curveValues->autoLegend = this->dbcurve->getAutoLegend();

  curveValues->inputMode = this->dbcurve->usingInputMode();
  if (this->dbcurve->getOwnerGraph()->isBeamDiagram())
    curveValues->inputMode *= -1;

  curveValues->axesComplete = this->dbcurve->areAxesComplete();
  curveValues->manualReload = this->dbcurve->needsManualRefresh();
  curveValues->autoExport = this->dbcurve->isAutoExported();

  // --- Appearance sheet ---
  curveValues->color = this->dbcurve->getColor();
  curveValues->curveType = this->dbcurve->getCurveStyle();
  curveValues->curveWidth = this->dbcurve->getCurveWidth();
  curveValues->curveSymbol = this->dbcurve->getCurveSymbol();
  curveValues->symbolSize = this->dbcurve->getCurveSymbolSize();
  curveValues->numSymbols = this->dbcurve->getCurveNumSymbols();

  // --- Spatial result curves ---
  // (temporary until macro objects for beam nodes are realized)
  std::vector<FmIsPlottedBase*> triads;
  this->dbcurve->getSpatialObjs(triads);
  if (triads.size() > 1) {
    curveValues->firstTriad = triads.front();
    curveValues->secondTriad = triads.back();
  }
  curveValues->timeRange = this->dbcurve->getTimeRange();
  curveValues->timeOper = this->dbcurve->getTimeOper();
  curveValues->spaceOper = this->dbcurve->getResultOper(FmCurveSet::XAXIS);

  // --- Combined curves ---
  curveValues->expression = this->dbcurve->getExpression();
  std::vector<FmCurveSet*> curveComps;
  size_t nComp = this->dbcurve->getCurveComps(curveComps,curveValues->activeComps);
  curveValues->curveComps.resize(curveValues->activeComps.size(),NULL);
  for (size_t i = 0; i < nComp; i++)
    curveValues->curveComps[i] = curveComps[i];

  static FapUAQuery cQuery;
  cQuery.clear();
  cQuery.typesToFind[FmCurveSet::getClassTypeID()] = true;
  if (curveValues->inputMode < 0)
    cQuery.verifyCB = FFaDynCB2M(FapUACurveDefine,this,verifySpatialCurveCB,bool&,FmModelMemberBase*);
  else
    cQuery.verifyCB = FFaDynCB2M(FapUACurveDefine,this,verifyTemporalCurveCB,bool&,FmModelMemberBase*);
  curveValues->curveQuery = &cQuery;

  // --- From file ---
  curveValues->filePath = this->dbcurve->getFilePath();
  const std::string& mPath = FmDB::getMechanismObject()->getAbsModelFilePath();
  int fileType = FiDeviceFunctionFactory::identify(curveValues->filePath,mPath);
  curveValues->isMultiChannelFile = fileType == RPC_TH_FILE || fileType == ASC_MC_FILE;
  curveValues->channel = this->dbcurve->getChannelName();
  curveValues->modelFilePath = mPath + FFaFilePath::getPathSeparator();

  // --- Internal function or preview curve ---
  FmMathFuncBase* f = this->dbcurve->getFunctionRef();
  curveValues->functionRef = f;
  curveValues->functionDomain.autoInc = f && f->hasSmartPoints() ? this->dbcurve->getUseSmartPoints() : -1;
  curveValues->functionDomain.X = this->dbcurve->getFuncDomain();
  curveValues->functionDomain.dX = this->dbcurve->getIncX();

  static FapUAQuery fQuery;
  fQuery.clear();
  fQuery.typesToFind[FmMathFuncBase::getClassTypeID()] = true;
  curveValues->functionQuery = &fQuery;

  // --- Scale and Shift sheet ---
  curveValues->scaleX = this->dbcurve->getXScale();
  curveValues->scaleY = this->dbcurve->getYScale();
  curveValues->offsetX = this->dbcurve->getXOffset();
  curveValues->offsetY = this->dbcurve->getYOffset();
  curveValues->zeroAdjustX = this->dbcurve->getXZeroAdjust();
  curveValues->zeroAdjustY = this->dbcurve->getYZeroAdjust();

  // --- Fourier Analysis sheet ---
  curveValues->analysis = this->dbcurve->getAnalysisFlag();
  curveValues->dftFeasable = this->dbcurve->isTimeAxis(FmCurveSet::XAXIS);
  curveValues->dftDomain = this->dbcurve->getDftDomain();
  curveValues->dftRemoveComp = this->dbcurve->getDftRemoveComp();
  curveValues->dftEntireDomain = this->dbcurve->getDftEntireDomain();
  curveValues->dftResample = this->dbcurve->getDftResample();
  curveValues->dftResampleRate = this->dbcurve->getDftResampleRate();

  // --- Fatigue sheet ---
  if (!this->dbcurve->isFatigueCurve())
    curveValues->fatigueFeasable = 0;
  else if (FapSimEventHandler::getActiveEvent() == NULL &&
	   FapSimEventHandler::hasResults(false))
    curveValues->fatigueFeasable = 2;
  else
    curveValues->fatigueFeasable = 1;

  // Change OIH 2012-06-28: Svein and Kristian agreed that
  // fatigue should always be available for all graphs.
  if (!curveValues->fatigueFeasable)
    curveValues->fatigueFeasable = 1;

  curveValues->fatigueDomain = this->dbcurve->getFatigueDomain();
  curveValues->fatigueEntireDomain = this->dbcurve->getFatigueEntireDomain();
  curveValues->fatigueLifeUnit = this->dbcurve->getFatigueLifeUnit();
  curveValues->fatigueGateValue = this->dbcurve->getFatigueGateValue();
  curveValues->fatigueSNCurve = this->dbcurve->getFatigueSNCurve();
  curveValues->fatigueSNStd = this->dbcurve->getFatigueSNStd();
}

//----------------------------------------------------------------------------

void FapUACurveDefine::verifySpatialCurveCB(bool& isOK, FmModelMemberBase* item)
{
  isOK = false;
  FmCurveSet* curve = dynamic_cast<FmCurveSet*>(item);
  if (curve && curve != this->dbcurve)
    isOK = curve->getOwnerGraph()->isBeamDiagram();
}

//----------------------------------------------------------------------------

void FapUACurveDefine::verifyTemporalCurveCB(bool& isOK, FmModelMemberBase* item)
{
  isOK = false;
  FmCurveSet* curve = dynamic_cast<FmCurveSet*>(item);
  if (curve && curve != this->dbcurve)
    isOK = !curve->getOwnerGraph()->isBeamDiagram();
}

//----------------------------------------------------------------------------

void FapUACurveDefine::reloadCurve()
{
  if (!this->dbcurve) return;

  this->dbcurve->reload();
}

//----------------------------------------------------------------------------

void FapUACurveDefine::setAutoLegend(bool autoLegend)
{
  if (!this->dbcurve) return;

  this->dbcurve->setAutoLegend(autoLegend);
  if (autoLegend)
    this->ui->setLegend(this->dbcurve->getLegend());
}

//----------------------------------------------------------------------------

void FapUACurveDefine::getChannelList(const std::string& file)
{
  std::string fName(file);
  FFaFilePath::makeItAbsolute(fName,FmDB::getMechanismObject()->getAbsModelFilePath());

  std::vector<std::string> channels;
  if (!FiDeviceFunctionFactory::getChannelList(fName,channels))
    FFaMsg::dialog("File: " + fName +
		   "\nCould not read channel list. Check file header.");
  else if (!channels.empty())
    this->ui->setChannelList(channels);
}

//----------------------------------------------------------------------------

void FapUACurveDefine::onAxisCompleted()
{
  if (!this->dbcurve) return;

  if (this->dbcurve->areAxesComplete())
  {
    this->ui->setCompleteSign(true);
    if (this->dbcurve->getAutoLegend())
      this->ui->setLegend(this->dbcurve->getLegend());
  }
  else
  {
    this->ui->setCompleteSign(false);
    if (this->dbcurve->getAutoLegend())
      this->ui->setLegend("");
  }

  int d = FuiCurveDefine::X;
  if (this->dbcurve->getOwnerGraph()->isBeamDiagram())
    d = FuiCurveDefine::Y;

  for (; d < FuiCurveDefine::NDIRS; d++) {
    this->dbcurve->setResult(d,this->axesUA[d]->getAxisResult());
    this->dbcurve->setResultOper(d,this->axesUA[d]->getAxisOper());
  }

  this->dbcurve->onDataChanged();

  FapUAExistenceHandler::doUpdateUI(FapUAProperties::getClassTypeID());
}

//----------------------------------------------------------------------------

void FapUACurveDefine::onEditXAxis()
{
  if (!this->dbcurve) return;

  IAmEditingCurveAxis = true;
  FapGraphCmds::editAxis(FuiCurveDefine::X, this->dbcurve);
}

//----------------------------------------------------------------------------

void FapUACurveDefine::onEditYAxis()
{
  if (!this->dbcurve) return;

  IAmEditingCurveAxis = true;
  FapGraphCmds::editAxis(FuiCurveDefine::Y, this->dbcurve);
}

//----------------------------------------------------------------------------

void FapUACurveDefine::onModelMemberChanged(FmModelMemberBase* item)
{
  if (wait) return;

  if (!item->isOfType(FmCurveSet::getClassTypeID())) return;

  FmCurveSet* curve = (FmCurveSet*) item;

  // do we need to check for scale and offset here?
  if (!curve->hasXYDataChanged() || curve != this->dbcurve) return;
  if (!this->isUIPoppedUp()) return;

  this->wait = true;
  this->updateUIValues();
  this->wait = false;

  FapUAExistenceHandler::doUpdateUI(FapUAProperties::getClassTypeID());
}

//----------------------------------------------------------------------------

FapUACurveDefine::SignalConnector::SignalConnector(FapUACurveDefine* ua) : owner(ua)
{
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_CHANGED,
			  FFaSlot1M(SignalConnector,this,
				    onModelMemberChanged,FmModelMemberBase*));
}

//----------------------------------------------------------------------------

FapUACurveDefine::SignalConnector::~SignalConnector()
{
  FFaSwitchBoard::removeAllOwnerConnections(this);
}

//----------------------------------------------------------------------------

void FapUACurveDefine::fetchStatistics(bool scaled, bool entire,
				       double start, double stop)
{
  if (!this->dbcurve) return;

  // If current curve is in a loaded graph, the curve data is already loaded
  // and we are using that graph's cached data. If not, we need to load it here.
  FapGraphDataMap* graphm = NULL;
  if (!this->curveData.hasDataChanged(this->dbcurve))
    graphm = &this->curveData; // use the cached curve data
  else if (!(graphm = FapUAGraphView::getGraphDataMap(this->dbcurve)))
  {
    std::string errMsg;
    this->curveData.findPlottingData(this->dbcurve,errMsg,true);
    if (errMsg.empty())
      graphm = &this->curveData;
    else
    {
      this->curveData.clear();
      FFaMsg::dialog(errMsg,FFaMsg::ERROR);
      return;
    }
  }

  FapCurveStat stat;
  graphm->getCurveStatistics(this->dbcurve, scaled, entire, start, stop, stat);

  this->ui->setCurveStatistics(stat.rms, stat.avg, stat.stdDev,
			       stat.integral, stat.min, stat.max);
}

//----------------------------------------------------------------------------

void FapUACurveDefine::applyVerticalMarkers(double min, double max)
{
  if (!this->dbcurve) return;

  FapUAGraphView::insertVerticalMarkers(this->dbcurve, min, max);
}

//----------------------------------------------------------------------------

void FapUACurveDefine::removeVerticalMarkers()
{
  if (!this->dbcurve) return;

  FapUAGraphView::removeVerticalMarkers(this->dbcurve);
}

//---------------------------------------------------------------------------

void FapUACurveDefine::fetchCurveDamage(bool weighted, bool entire,
					double start, double stop)
{
  if (!this->dbcurve) return;
  if (!FFpSNCurveLib::allocated()) return;

  double gate = this->dbcurve->getFatigueGateValue();
  int snStd = this->dbcurve->getFatigueSNStd();
  int snCurve = this->dbcurve->getFatigueSNCurve();
  FFpSNCurve* snC = FFpSNCurveLib::instance()->getCurve(snStd,snCurve);
  if (!snC)
  {
    ListUI <<"Error in damage calculation - Undefined SN-curve: "
	   <<"StdIndex="<< snStd <<" CurveIndex="<< snCurve <<"\n";
    return;
  }

  double damage = 0.0;
  double interval = 0.0;
  FapGraphDataMap* graphm = NULL;
  if (weighted)
  {
    FmCurveSet::Analysis tmpAF = this->dbcurve->getAnalysisFlag();
    this->dbcurve->setAnalysisFlag(FmCurveSet::NONE,false); // turn off DFT

    // Calculate weighted damage, event by event
    FapGraphDataMap eventData;
    std::vector<FmSimulationEvent*> events;
    FmDB::getAllSimulationEvents(events);
    FmMechanism* mech = FmDB::getMechanismObject();

    FFuProgressDialog* progDlg = FFuProgressDialog::create("Please wait...",
							   "Cancel",
							   "Calculating Damage",
							   events.size());

    for (size_t i = 0; i < events.size(); i++)
    {
      progDlg->setCurrentProgress(i);
      if (progDlg->userCancelled()) break;

      FpModelRDBHandler::RDBRelease();
      FpModelRDBHandler::RDBOpen(events[i]->getResultStatusData(),mech);

      std::string errMsg;
      eventData.findPlottingData(this->dbcurve,errMsg);
      double eDamage = -1.0;
      if (errMsg.empty())
	eDamage = eventData.getDamageFromCurve(this->dbcurve,gate,true,
					       entire,start,stop,*snC);
      if (eDamage >= 0.0)
	damage += eDamage*events[i]->getProbability();
      else
      {
	ListUI <<"===> Damage calculation failed for "
	       << events[i]->getIdString();
	if (!errMsg.empty()) ListUI <<"\n     "<< errMsg;
	FFaMsg::list("\n",true);
      }

      if (entire && i == 0)
      {
	FFpCurve* curve = eventData.getFFpCurve(this->dbcurve,false);
	if (curve && !curve->empty())
	  interval = curve->getXrange();
      }
    }

    progDlg->setCurrentProgress(events.size());
    delete progDlg;

    FpModelRDBHandler::RDBRelease();
    FpModelRDBHandler::RDBOpen(mech->getResultStatusData(),mech);

    this->dbcurve->setAnalysisFlag(tmpAF,false); // reset data analysis flag
  }
  else // No events, or we are doing the currently loaded event only
  {
    // If current curve is in a loaded graph, the curve data is already loaded
    // and we use that graph's cached data. If not, we need to load it here.
    // We also need to load the curve if it has been DFT-transformed, because
    // the curve data in graphm then has been replaced by its DFT-transform.
    std::string errMsg;
    if (!this->curveData.hasDataChanged(this->dbcurve))
      graphm = &this->curveData; // use the cached curve data
    else if (this->dbcurve->doDft() || this->dbcurve->doRainflow() ||
	     !(graphm = FapUAGraphView::getGraphDataMap(this->dbcurve)))
    {
      FmCurveSet::Analysis tmpAF = this->dbcurve->getAnalysisFlag();
      this->dbcurve->setAnalysisFlag(FmCurveSet::NONE,false); // turn off DFT
      this->curveData.findPlottingData(this->dbcurve,errMsg);
      this->dbcurve->setAnalysisFlag(tmpAF,false); // reset data analysis flag
      if (errMsg.empty())
        graphm = &this->curveData;
      else
        this->curveData.clear();
    }

#ifdef FAP_DEBUG
    std::cout <<"FapUACurveDefine: Calculating damage for "
              << this->dbcurve->getIdString(true) << std::endl;
#endif
    if (graphm)
      damage = graphm->getDamageFromCurve(this->dbcurve,gate,true,
					  entire,start,stop,*snC);
    else
      damage = -1.0;

    if (damage < 0.0)
    {
      ListUI <<"===> Damage calculation failed - no curve data";
      if (!errMsg.empty()) ListUI <<"\n     "<< errMsg;
      FFaMsg::list("\n",true);
    }
  }

  if (entire && graphm)
  {
    FFpCurve* curve = graphm->getFFpCurve(this->dbcurve,false);
    if (curve && !curve->empty()) interval = curve->getXrange();
  }
  else if (interval == 0.0)
    interval = stop - start;

  this->ui->setDamageFromCurve(damage, interval);
}
