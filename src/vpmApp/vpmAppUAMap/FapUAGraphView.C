// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAGraphView.H"
#include "vpmApp/FapEventManager.H"
#include "vpmApp/vpmAppCmds/FapAnimationCmds.H"
#include "vpmUI/vpmUITopLevels/FuiGraphView.H"
#include "vpmUI/Fui.H"

#include "vpmDB/FmGraph.H"
#include "vpmDB/FmCurveSet.H"
#include "vpmDB/FmAnimation.H"
#include "vpmDB/FmMathFuncBase.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmDB.H"

#include "vpmPM/FpRDBExtractorManager.H"
#include "FFrLib/FFrExtractor.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "FFuLib/FFuColor.H"


std::set<FapUAGraphView*> FapUAGraphView::ourSelfSet;

Fmd_SOURCE_INIT(FAPUAGRAPHVIEW, FapUAGraphView, FapUAExistenceHandler);

//------------------------------------------------------------------------------

FapUAGraphView::FapUAGraphView(FuiGraphView* uic)
  : FapUAExistenceHandler(uic), FapUAItemsViewHandler(uic), FapUADataHandler(uic),
    signalConnector(this)
{
  Fmd_CONSTRUCTOR_INIT(FapUAGraphView);

  uic->setAxisScaleChangedCB(FFaDynCB0M(FapUAGraphView,this,onAxisScaleChanged));
  uic->setGraphSelectedCB(FFaDynCB0M(FapUAGraphView,this,onGraphSelected));

  this->ui = uic;
  this->dbgraph = FapEventManager::getLoadingGraph();
  this->myAnimMarkerID.fill(-1);
  this->myMinMarkerID = -1;
  this->myMaxMarkerID = -1;

  ourSelfSet.insert(this);
}

//------------------------------------------------------------------------------

FFuaUIValues* FapUAGraphView::createValuesObject()
{
  return new FuaGraphViewValues();
}

//------------------------------------------------------------------------------

void FapUAGraphView::updateSession()
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAGraphView::updateSession()"<< std::endl;
#endif
  FapUAItemsViewHandler::updateSession();

  if (!this->dbgraph) return;

  std::vector<FmCurveSet*> curves;
  this->dbgraph->getCurveSets(curves,true);

  this->clearSession();
  this->updateUIValues();
  this->loadCurvesInViewer(curves,false);

  // If a time history animation is currently loaded,
  // add time markers to the opened graph (TT #2866).
  FmAnimation* anim = FapAnimationCmds::getCurrentAnimation();
  if (anim && anim->isHistoryAnimation())
    this->initAnimation();

  this->permTotSelectUIItems(FapEventManager::getPermSelection());
}

//------------------------------------------------------------------------------

/*!
  Clears only curves that have results from RDB
*/

void FapUAGraphView::clearSessionSpecial()
{
  if (!this->dbgraph) return;

  std::vector<FmCurveSet*> curves;
  this->dbgraph->getCurveSets(curves);
  for (FmCurveSet* c : curves)
    if (c->isResultDependent()) this->removeUICurve(c);
}

//------------------------------------------------------------------------------

/*!
  Erases one curve from UI, and clears it from internal book-keeping
*/

bool FapUAGraphView::removeUICurve(FmCurveSet* curve)
{
  int item = this->getMapItem(curve);
  if (item < 0) return false; // curve is not in the UI

  this->ui->deleteItem(item);
  this->eraseMapItem(curve);
  this->graphData.erase(curve);
  return true;
}

//------------------------------------------------------------------------------

/*!
  Clears UI and internal book-keeping
*/

void FapUAGraphView::clearSession()
{
  FapUAItemsViewHandler::clearSession();
  this->removeVerticalMarkers();
  this->graphData.clear();
}

//------------------------------------------------------------------------------

/*!
  Erases all instances of the "-character in the string label (if any).
  The model file parser does not cope with their presense (TT #2926).
*/

static const std::string& stripFnutts(const std::string& label)
{
  size_t i = label.find('"');
  if (i < std::string::npos)
  {
    FFaMsg::dialog("\"-characters are not allowed in graph labels"
                   " and will be removed",FFaMsg::WARNING);
    for (; i < label.size(); i = label.find('"',i))
      const_cast<std::string&>(label).erase(i,1);
  }

  return label;
}


void FapUAGraphView::setDBValues(FFuaUIValues* values)
{
  if (!this->dbgraph) return;

  FuaGraphViewValues* graphvalues = (FuaGraphViewValues*)values;

  this->dbgraph->setTitle(stripFnutts(graphvalues->title));
  this->dbgraph->setSubTitle(stripFnutts(graphvalues->subtitle));
  this->dbgraph->setXaxisLabel(stripFnutts(graphvalues->xAxisLabel));
  this->dbgraph->setYaxisLabel(stripFnutts(graphvalues->yAxisLabel));
  this->dbgraph->setFontSize(graphvalues->fontSize);
  this->dbgraph->setGridType(graphvalues->gridtype);
  this->dbgraph->setShowLegend(graphvalues->showLegend);
  this->dbgraph->setAutoScaleFlag(graphvalues->autoScaleOnLoadCurve);
  this->dbgraph->onChanged();

  for (const FuaGraphViewValues::CurveData& cv : graphvalues->curves)
  {
    FmCurveSet* dbcurve = dynamic_cast<FmCurveSet*>(this->getMapItem(cv.id));
    if (!dbcurve) break;

    dbcurve->setLegend(stripFnutts(cv.legend));
    dbcurve->setColor(cv.color);
    dbcurve->setCurveStyle(cv.style);
    dbcurve->setCurveWidth(cv.width);
    dbcurve->setCurveSymbol(cv.symbols);
    dbcurve->setCurveSymbolSize(cv.symbolssize);
    dbcurve->setCurveNumSymbols(cv.numSymbols);
    dbcurve->onChanged();
  }
}

//------------------------------------------------------------------------------

void FapUAGraphView::getDBValues(FFuaUIValues* values)
{
  if (!this->dbgraph) return;

  FuaGraphViewValues* graphvalues = (FuaGraphViewValues*) values;

  graphvalues->title = this->dbgraph->getTitle();
  graphvalues->subtitle = this->dbgraph->getSubTitle();
  graphvalues->xAxisLabel = this->dbgraph->getXaxisLabel();
  graphvalues->yAxisLabel = this->dbgraph->getYaxisLabel();
  graphvalues->fontSize = this->dbgraph->getFontSize();
  graphvalues->gridtype = this->dbgraph->getGridType();
  graphvalues->showLegend = this->dbgraph->getShowLegend();
  graphvalues->autoScaleOnLoadCurve = this->dbgraph->getAutoScaleFlag();
  this->dbgraph->getXaxisScale(graphvalues->xAxisMin,graphvalues->xAxisMax);
  this->dbgraph->getYaxisScale(graphvalues->yAxisMin,graphvalues->yAxisMax);

  graphvalues->curves.clear();
  FuaGraphViewValues::CurveData curve;

  int useFFTlabels = -1;
  std::vector<FmCurveSet*> curves;
  this->dbgraph->getCurveSets(curves);
  for (FmCurveSet* c : curves)
  {
    curve.id = this->getMapItem(c);
    if (curve.id < 0) break;

    curve.legend = c->getLegend();
    curve.color = FFuColor(c->getColor());
    curve.style = c->getCurveStyle();
    curve.width = c->getCurveWidth();
    curve.symbols = c->getCurveSymbol();
    curve.symbolssize = c->getCurveSymbolSize();
    curve.numSymbols = c->getCurveNumSymbols();
    if (curve.style != 2 && !c->doDft())
      useFFTlabels = 0;
    else if (useFFTlabels < 0)
      useFFTlabels = 1;

    graphvalues->curves.push_back(curve);
  }

  if (useFFTlabels == 1)
  {
    graphvalues->xAxisLabel = "Frequency [Hz]";
    graphvalues->yAxisLabel = "";
  }
}

//------------------------------------------------------------------------------

void FapUAGraphView::onAxisScaleChanged()
{
  if (!this->dbgraph) return;

  this->dbgraph->setXaxisScale(this->ui->getPlotterXAxisMin(),
			       this->ui->getPlotterXAxisMax());

  this->dbgraph->setYaxisScale(this->ui->getPlotterYAxisMin(),
			       this->ui->getPlotterYAxisMax());
}

//------------------------------------------------------------------------------

void FapUAGraphView::onGraphSelected()
{
  if (!this->dbgraph) return;

  FapEventManager::permTotalSelect(this->dbgraph);
}

//------------------------------------------------------------------------------

void FapUAGraphView::onModelMemberConnected(FmModelMemberBase* item)
{
  if (!this->shouldIUpdateOnChanges()) return;

  // Only interested in curves belonging to this graph
  if (!item->isOfType(FmCurveSet::getClassTypeID())) return;
  if (((FmCurveSet*)item)->getOwnerGraph() != this->dbgraph) return;
  if (this->getMapItem(item) > -1) return;

  this->loadCurveInViewer((FmCurveSet*)item);
  this->updateUIValues();
}

//------------------------------------------------------------------------------

void FapUAGraphView::onModelMemberDisconnected(FmModelMemberBase* item)
{
  if (!this->shouldIUpdateOnChanges()) return;

  // Only interested in curves belonging to this graph
  if (!item->isOfType(FmCurveSet::getClassTypeID())) return;
  if (((FmCurveSet*)item)->getOwnerGraph() != this->dbgraph) return;

  this->deleteUIItem(item);
  this->graphData.erase((const FmCurveSet*)item);
}

//------------------------------------------------------------------------------

void FapUAGraphView::onModelMemberChanged(FmModelMemberBase* item)
{
  if (!this->shouldIUpdateOnChanges()) return;

  if (item->isOfType(FmGraph::getClassTypeID()))
  {
    // my graph has changed
    if (item == this->dbgraph)
      this->updateUIValues();
  }
  else if (item->isOfType(FmMathFuncBase::getClassTypeID()))
  {
    // one of the functions my graph is plotting have changed
    bool oneOfMine = false;
    std::vector<FmCurveSet*> referredCurves;
    item->getReferringObjs(referredCurves,"myFunction");
    for (FmCurveSet* curve : referredCurves)
      if (curve->getOwnerGraph() == this->dbgraph)
      {
	oneOfMine = true;
	curve->reload();
      }

    if (oneOfMine)
      this->updateUIValues();
  }
  else if (item->isOfType(FmCurveSet::getClassTypeID()))
  {
    // one of the curves my graph is plotting has changed
    FmCurveSet* curve = (FmCurveSet*)item;
    if (curve->getOwnerGraph() == this->dbgraph)
    {
      // check if curve definition or analysis options have changed
      if (curve->hasXYDataChanged() || curve->hasDFTOptionsChanged())
	this->loadCurveInViewer(curve);
      else if (curve->hasScaleOrOffsetChanged())
	this->ui->setPlotterScaleAndOffset(this->getMapItem(curve),
					   curve->getXScale(),
					   curve->getXOffset(),
					   curve->getXZeroAdjust(),
					   curve->getYScale(),
					   curve->getYOffset(),
					   curve->getYZeroAdjust(), true);
      this->updateUIValues();
    }
  }
}

//------------------------------------------------------------------------------

void FapUAGraphView::onNewModelExtr(FFrExtractor*)
{
}

//------------------------------------------------------------------------------

void FapUAGraphView::onModelExtrDeleted(FFrExtractor*)
{
  this->clearSessionSpecial();
}

//------------------------------------------------------------------------------

void FapUAGraphView::onModelExtrDataChanged(FFrExtractor*)
{
  if (!this->dbgraph) return;
  if (this->dbgraph->isBeamDiagram()) return;

  std::vector<FmCurveSet*> curves, tempCurves;
  this->dbgraph->getCurveSets(curves);
  tempCurves.reserve(curves.size());

  // Only bother for curves with data from RDB
  for (FmCurveSet* c : curves)
    if (c->isResultDependent())
      tempCurves.push_back(c);

#if FAP_DEBUG > 1
  std::cout <<"\nFapUAGraphView::onModelExtrDataChanged: "
            << tempCurves.size() <<" curves to reload." <<std::endl;
#endif
  if (!tempCurves.empty())
    this->loadCurvesInViewer(tempCurves,true);

  this->permTotSelectUIItems(FapEventManager::getPermSelection());
}

//------------------------------------------------------------------------------

/*!
  Implemented to catch any frs file delete.
  If results for one of the curves have been deleted, clear that curve.
*/

void FapUAGraphView::onModelExtrHeaderChanged(FFrExtractor* extr)
{
  if (!this->dbgraph) return;

  std::vector<FmCurveSet*> curves;
  this->dbgraph->getCurveSets(curves);

  // Only bother for curves with temporal data from RDB and currently in viewer
  int nReload = 0;
  int nRemove = 0;
  for (FmCurveSet* c : curves)
    if (c->usingInputMode() == FmCurveSet::TEMPORAL_RESULT)
    {
      FFrEntryBase* xItem = extr->search(c->getResult(FmCurveSet::XAXIS));
      FFrEntryBase* yItem = extr->search(c->getResult(FmCurveSet::YAXIS));
      bool haveData = xItem && yItem && !xItem->isEmpty() && !yItem->isEmpty();
      bool isInView = this->getMapItem(c) >= 0;
      if (isInView && !haveData)
        nRemove += this->removeUICurve(c);
      else if (!isInView && haveData)
        nReload += this->graphData.setDataChanged(c);
#ifdef FAP_DEBUG
      if (haveData)
        std::cout <<"\nFapUAGraphView::onModelExtrHeaderChanged(): Plotting "
                  << c->getIdString(true) <<" with\n\t X-axis data: "
                  << xItem->getEntryDescription() <<"\n\t Y-axis data: "
                  << yItem->getEntryDescription() << std::endl;
#endif
    }

#ifdef FAP_DEBUG
  std::cout <<"\nFapUAGraphView::onModelExtrHeaderChanged()";
  if (nRemove > 0) std::cout <<" "<< nRemove <<" RDB curves to unload";
  if (nReload > 0) std::cout <<" "<< nReload <<" RDB curves to reload";
  std::cout << std::endl;
#endif

  ui->updateSession();
}

//------------------------------------------------------------------------------

void FapUAGraphView::loadCurveInViewer(FmCurveSet* curve)
{
  Fui::noUserInputPlease();
  FFaMsg::pushStatus("Loading curve data");

  // find data
  this->graphData.findPlottingData({curve});

  // load curve (also if without result or extractor)
  this->loadCurveDataInViewer(curve,false);

  FFaMsg::popStatus();
  Fui::okToGetUserInput();
}

//------------------------------------------------------------------------------

void FapUAGraphView::loadCurvesInViewer(const std::vector<FmCurveSet*>& curves,
					bool append)
{
  Fui::noUserInputPlease();
  FFaMsg::pushStatus("Loading curve data");

  // find data
  this->graphData.findPlottingData(curves,NULL,append);

  // load all curves (also those without result or extractor)
  for (FmCurveSet* curve : curves)
    this->loadCurveDataInViewer(curve,append);

  FFaMsg::popStatus();
  Fui::okToGetUserInput();
}

//------------------------------------------------------------------------------

void FapUAGraphView::loadCurveDataInViewer(FmCurveSet* curve, bool append)
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAGraphView::loadCurveDataInViewer "
            << curve->getIdString(true);
#endif

  // Check if this is an already loaded curve with no data change
  int uiItem = this->getMapItem(curve);
  if (uiItem >= 0 && !graphData.hasDataChanged(curve))
  {
#ifdef FAP_DEBUG
    std::cout <<" (unchanged data)"<< std::endl;
#endif
    return;
  }

  FFpCurve* myCurve = graphData.getFFpCurve(curve,false,true);
  if (!myCurve->checkAxesSize())
  {
#ifdef FAP_DEBUG
    std::cout <<" (failed)"<< std::endl;
#endif
    ListUI <<" ===> Error loading "<< curve->getIdString(true)
	   <<".\nThe axes do not have the same size.\n";
    return;
  }

  // Appearance for the efficiency of settings
  FFuColor color(curve->getColor());
  int style = curve->getCurveStyle();
  int width = curve->getCurveWidth();
  int symb = curve->getCurveSymbol();
  int symbsize = curve->getCurveSymbolSize();
  int numSymbols = curve->getCurveNumSymbols();
  std::string legend = curve->getLegend();
  // For DFT-transformed curves scale/shift/zeroAdjust have already been applied
  double scaleX = curve->doAnalysis() ? 1.0 : curve->getXScale();
  double offsetX = curve->doAnalysis() ? 0.0 : curve->getXOffset();
  bool zeroAdjustX = curve->doAnalysis() ? false : curve->getXZeroAdjust();
  double scaleY = curve->doAnalysis() ? 1.0 : curve->getYScale();
  double offsetY = curve->doAnalysis() ? 0.0 : curve->getYOffset();
  bool zeroAdjustY = curve->doAnalysis() ? false : curve->getYZeroAdjust();

#ifdef FAP_DEBUG
  std::cout <<" ("<< (*myCurve)[FmCurveSet::XAXIS].size() <<" points): ";
  if (uiItem < 0)
    std::cout <<"new curve"<< std::endl;
  else
    std::cout <<"curve ID "<< uiItem << std::endl;
#endif

  if (uiItem >= 0) // Curve is already in viewer
  {
    this->ui->loadPlotterCurveData(uiItem,
				   &(*myCurve)[FmCurveSet::XAXIS],
				   &(*myCurve)[FmCurveSet::YAXIS],
				   append, color, style, width, symb,
				   symbsize, numSymbols, legend,
				   scaleX, offsetX, zeroAdjustX,
				   scaleY, offsetY, zeroAdjustY);
  }
  else
  {
    uiItem = this->ui->loadNewPlotterCurve(&(*myCurve)[FmCurveSet::XAXIS],
					   &(*myCurve)[FmCurveSet::YAXIS],
					   color, style, width, symb,
					   symbsize, numSymbols, legend,
					   scaleX, offsetX, zeroAdjustX,
					   scaleY, offsetY, zeroAdjustY);
    if (uiItem < 0)
      ListUI <<" ===> Error loading "<< curve->getIdString(true) <<"\n";
    else
      this->putMapItem(uiItem,curve);
  }

  myCurve->onDataPlotted();
  this->permTotSelectUIItems(FapEventManager::getPermSelection());
}

//------------------------------------------------------------------------------

FapUAGraphView* FapUAGraphView::getUAGraphView(FmGraph* graph)
{
  std::vector<FapUAExistenceHandler*> uas;
  FapUAExistenceHandler::getAllOfType(FapUAGraphView::getClassTypeID(),uas);

  for (FapUAExistenceHandler* ua : uas)
    if (((FapUAGraphView*)ua)->getDBPointer() == graph)
      return (FapUAGraphView*)ua;

  return NULL;
}

//------------------------------------------------------------------------------

FapUAGraphView* FapUAGraphView::getUAGraphView(FmCurveSet* curve)
{
  std::vector<FapUAExistenceHandler*> uas;
  FapUAExistenceHandler::getAllOfType(FapUAGraphView::getClassTypeID(),uas);

  for (FapUAExistenceHandler* ua : uas)
    if (((FapUAGraphView*)ua)->getDBPointer()->hasCurve(curve))
      return (FapUAGraphView*)ua;

  return NULL;
}

//------------------------------------------------------------------------------

FapGraphDataMap* FapUAGraphView::getGraphDataMap(FmCurveSet* curve)
{
  FapUAGraphView* uagv = FapUAGraphView::getUAGraphView(curve);
  return uagv ? &uagv->graphData : NULL;
}

//------------------------------------------------------------------------------

void FapUAGraphView::insertVerticalMarkers(FmCurveSet* curve,
					   double min, double max)
{
  FapUAGraphView* graphView = FapUAGraphView::getUAGraphView(curve);
  if (!graphView) return;

  graphView->insertVerticalMarkers(min,max);
}

//------------------------------------------------------------------------------

void FapUAGraphView::insertVerticalMarkers(double min, double max)
{
  this->myMinMarkerID = this->ui->insertPlotterLineMarker("", FmCurveSet::XAXIS, min);
  this->myMaxMarkerID = this->ui->insertPlotterLineMarker("", FmCurveSet::XAXIS, max);
}

//------------------------------------------------------------------------------

void FapUAGraphView::removeVerticalMarkers(FmCurveSet* curve)
{
  FapUAGraphView* graphView = FapUAGraphView::getUAGraphView(curve);
  if (!graphView) return;

  graphView->removeVerticalMarkers();
}

//------------------------------------------------------------------------------

void FapUAGraphView::removeVerticalMarkers()
{
  this->ui->removePlotterMarker(this->myMinMarkerID);
  this->ui->removePlotterMarker(this->myMaxMarkerID);
  this->myMinMarkerID = -1;
  this->myMaxMarkerID = -1;
}

//------------------------------------------------------------------------------

/*!
  Inits all the graphs for doing animation things,
  basically allocates a marker, with ID.
*/

void FapUAGraphView::initAnimationAllGraphs()
{
  for (FapUAGraphView* ua : ourSelfSet)
    ua->initAnimation();
}


/*!
  Sets the animation time on all graphs to move the marker to
  the correct animation time.
*/

void FapUAGraphView::setAnimationTimeAllGraphs(double time)
{
  for (FapUAGraphView* ua : ourSelfSet)
    ua->setAnimationTime(time);
}


/*!
  Removes everything regarding animation from all the graphs,
  Basically remove the allocated markers.
*/

void FapUAGraphView::resetAnimationAllGraphs()
{
  for (FapUAGraphView* ua : ourSelfSet)
    ua->resetAnimation();
}


/*!
  Adds a line marker to the axis that has time, if any.
*/

void FapUAGraphView::initAnimation()
{
  if (!this->dbgraph) return;

  if (this->dbgraph->isBeamDiagram() || this->dbgraph->isFuncPreview())
    return;

  std::vector<FmCurveSet*> curves;
  this->dbgraph->getCurveSets(curves);
  for (FmCurveSet* c : curves)
    for (int a = 0; a < FmCurveSet::NAXES; a++)
      if (this->myAnimMarkerID[a] < 0 && c->isTimeAxis(a))
        this->myAnimMarkerID[a] = this->ui->insertPlotterLineMarker("",a,0.0);
}


/*!
  Updates the animation time marker position, if any.
*/

void FapUAGraphView::setAnimationTime(double time)
{
  for (int a = 0; a < FmCurveSet::NAXES; a++)
    if (this->myAnimMarkerID[a] > -1)
    {
      if (a == FmCurveSet::XAXIS)
        this->ui->setPlotterMarkerXPos(this->myAnimMarkerID[a],time);
      else if (a == FmCurveSet::YAXIS)
        this->ui->setPlotterMarkerYPos(this->myAnimMarkerID[a],time);
    }
}


/*!
  Removes the animation time markers.
*/

void FapUAGraphView::resetAnimation()
{
  for (int a = 0; a < FmCurveSet::NAXES; a++)
    if (this->myAnimMarkerID[a] > -1)
      this->ui->removePlotterMarker(this->myAnimMarkerID[a]);

  this->myAnimMarkerID.fill(-1);
}

//------------------------------------------------------------------------------

FapUAGraphView::SignalConnector::SignalConnector(FapUAGraphView* ua) : owner(ua)
{
  // DB signals
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_CONNECTED,
			  FFaSlot1M(SignalConnector,this,
				    onModelMemberConnected,FmModelMemberBase*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_DISCONNECTED,
			  FFaSlot1M(SignalConnector,this,
				    onModelMemberDisconnected,
				    FmModelMemberBase*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_CHANGED,
			  FFaSlot1M(SignalConnector,this,
				    onModelMemberChanged,FmModelMemberBase*));
  // RDB signals
  FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
			  FpRDBExtractorManager::NEW_MODELEXTRACTOR,
			  FFaSlot1M(SignalConnector,this,
				    onNewModelExtr,FFrExtractor*));
  FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
			  FpRDBExtractorManager::MODELEXTRACTOR_ABOUT_TO_DELETE,
			  FFaSlot1M(SignalConnector,this,
				    onModelExtrDeleted,FFrExtractor*));
  FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
			  FpRDBExtractorManager::MODELEXTRACTOR_DATA_CHANGED,
			  FFaSlot1M(SignalConnector,this,
				    onModelExtrDataChanged,FFrExtractor*));
  FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
			  FpRDBExtractorManager::MODELEXTRACTOR_HEADER_CHANGED,
			  FFaSlot1M(SignalConnector,this,
				    onModelExtrHeaderChanged,FFrExtractor*));
}
//------------------------------------------------------------------------------

FapUAGraphView::SignalConnector::~SignalConnector()
{
  FFaSwitchBoard::removeAllOwnerConnections(this);
}
