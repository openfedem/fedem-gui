// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFaLib/FFaDefinitions/FFaViewItem.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFuLib/FFuAuxClasses/FFuaIdentifiers.H"
#include "vpmApp/FapEventManager.H"
#include "vpmApp/FapLicenseManager.H"
#include "vpmDB/FmGraph.H"
#include "vpmDB/FmCurveSet.H"
#include "vpmDB/FmSubAssembly.H"
#include "vpmDB/FmDB.H"
#include "vpmUI/Fui.H"
#include "vpmUI/vpmUITopLevels/FuiModeller.H"
#include "vpmUI/vpmUITopLevels/FuiRepeatCurve.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmUI/vpmUITopLevels/FuiGraphView.H"
#include "vpmApp/vpmAppUAMap/FapUAGraphViewTLS.H"
#include "vpmApp/vpmAppUAMap/FapUAGraphView.H"
#endif
#include "vpmApp/vpmAppUAMap/FapUARDBSelector.H"
#include "vpmApp/vpmAppCmds/FapGraphCmds.H"

#include "vpmUI/Icons/curvePlot.xpm"
#include "vpmUI/Icons/replicateCurve.xpm"
#include "vpmUI/Icons/showRDBselector.xpm"

extern const char* eventDef_xpm[];

#include <algorithm>


//----------------------------------------------------------------------------

void FapGraphCmds::init()
{
  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_graph_show");
  cmdItem->setSmallIcon(curvePlot_xpm);
  cmdItem->setText("Show Graph");
  cmdItem->setToolTip("Show Graph");
  cmdItem->setAccelKey(FFuaKeyCode::CtrlAccel+FFuaKeyCode::G);
  cmdItem->setActivatedCB(FFaDynCB0S(FapGraphCmds::show));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapGraphCmds::getShowSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_graph_editXAxis");
  cmdItem->setText("Edit X Axis...");
  cmdItem->setToolTip("Edit X Axis");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapGraphCmds::editAxis(FmCurveSet::XAXIS); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapGraphCmds::getEditXAxisSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_graph_editYAxis");
  cmdItem->setText("Edit Y Axis...");
  cmdItem->setToolTip("Edit Y Axis");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapGraphCmds::editAxis(FmCurveSet::YAXIS); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapGraphCmds::getEditYAxisSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_graph_showRDBSelector");
  cmdItem->setSmallIcon(showRDBselector_xpm);
  cmdItem->setText("Result selector...");
  cmdItem->setToolTip("Open result selector tool to drag results into graphs");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapGraphCmds::editAxis(FmCurveSet::YAXIS); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelTouchable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_graph_showRDBMEFatigue");
  cmdItem->setSmallIcon(eventDef_xpm);
  cmdItem->setText("Fatigue Summary...");
  cmdItem->setToolTip("Open multi-event fatigue summary tool");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ Fui::rdbMEFatigueUI(true); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelTouchable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_graph_repeatCurveForAll");
  cmdItem->setSmallIcon(replicateCurve_xpm);
  cmdItem->setText("Repeat curve for all objects");
  cmdItem->setToolTip("Replicate curve for all objects in model");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapGraphCmds::repeatCurve(0,-1); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapGraphCmds::getRepeatCurveSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_graph_repeatCurveForSome");
  cmdItem->setSmallIcon(replicateCurve_xpm);
  cmdItem->setText("Repeat curve for object range...");
  cmdItem->setToolTip("Replicate curve for all objects within specified range");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapGraphCmds::repeatCurve(0,0); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapGraphCmds::getRepeatCurveSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_graph_enableAutoExport");
  cmdItem->setText("Enable Autoexport of Curves");
  cmdItem->setToolTip("Enable autoexport for all curves in this graph group");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapGraphCmds::toggleAutoExport(true); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelTouchable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_graph_disableAutoExport");
  cmdItem->setText("Disable Autoexport of Curves");
  cmdItem->setToolTip("Disable autoexport for all curves in this graph group");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapGraphCmds::toggleAutoExport(false); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelTouchable,bool&));
}

//----------------------------------------------------------------------------

void FapGraphCmds::show()
{
  bool dummy;
  std::vector<FmGraph*> selGraphs = FapGraphCmds::findSelectedGraphs(dummy);
  std::vector<FmCurveSet*> selCurves = FapGraphCmds::findSelectedCurves(dummy);

  // Running through the selected curves to see if any of their
  // owners are not in vector of graphs to show
  for (FmCurveSet* curve : selCurves) {
    FmGraph* owner = curve->getOwnerGraph();
    if (std::find(selGraphs.begin(),selGraphs.end(),owner) == selGraphs.end())
      selGraphs.push_back(owner);
  }

  for (FmGraph* graph : selGraphs)
    FapGraphCmds::show(graph);
}

//----------------------------------------------------------------------------

void FapGraphCmds::show(FmGraph* graph)
{
#ifdef FT_HAS_GRAPHVIEW
  FapUAGraphViewTLS* tls = FapGraphCmds::getTLS(graph);
  if (tls)
    tls->getUI()->popUp();
  else {
    FapEventManager::setLoadingGraph(graph);
    FuiGraphView* gv = Fui::newGraphViewUI(graph->getUserDescription().c_str());
    gv->enablePlotterDemoWarning(FapLicenseManager::isDemoEdition());
  }
#else
  // Dummy statement to avoid compiler warning
  std::cout <<"  ** FapGraphCmds::show(): "<< graph->getIdString(true)
            <<" not shown."<< std::endl;
#endif
}

//----------------------------------------------------------------------------

void FapGraphCmds::getShowSensitivity(bool& sensitivity)
{
  sensitivity = false;

#ifdef FT_HAS_GRAPHVIEW
  std::vector<FFaViewItem*> permSelection;
  FFaViewItem* tmpSelection;
  FapEventManager::getSelection(permSelection,tmpSelection);

  // Check that all selected are either curves or graphs
  if (tmpSelection)
    sensitivity = (dynamic_cast<FmCurveSet*>(tmpSelection) ||
                   dynamic_cast<FmGraph*>(tmpSelection));
  else
    // Running through all perm selected.
    // Returning with sensitivity false if we encounter one selection
    // that is neither curve nor graph.
    for (FFaViewItem* item : permSelection)
      if (!dynamic_cast<FmCurveSet*>(item) && !dynamic_cast<FmGraph*>(item))
        return;

  sensitivity = true;
#endif
}

//----------------------------------------------------------------------------

/*!
  svenev: Note on edit axes:
  There are two ways here, one for editing via right click,
  and one for editing using the old Edit button. When clicking
  the edit button, the curve is sent as a parameter, in order to
  work around a kind of a nasty design flaw (Curve def can be active
  without a curve being selected). It's not nice, but it works. When
  editing by right clicking, the curve is found from the selection.
*/

void FapGraphCmds::editAxis(int axis, FmCurveSet* curve)
{
  if (!curve)
  {
    bool curvesOnly;
    std::vector<FmCurveSet*> curves = FapGraphCmds::findSelectedCurves(curvesOnly);
    if (curves.size() == 1) curve = curves.front();
  }

  Fui::rdbSelectorUI(true);

  FapUAExistenceHandler* uas = FapUAExistenceHandler::getFirstOfType(FapUARDBSelector::getClassTypeID());
  if (uas) static_cast<FapUARDBSelector*>(uas)->edit(curve,axis);
}

//----------------------------------------------------------------------------

void FapGraphCmds::getEditXAxisSensitivity(bool& sensitivity)
{
  FapCmdsBase::isModelTouchable(sensitivity);
  if (!sensitivity) return;

  std::vector<FmCurveSet*> curves = FapGraphCmds::findSelectedCurves(sensitivity);
  if (curves.size() == 1)
    sensitivity = curves.front()->usingInputMode() == FmCurveSet::TEMPORAL_RESULT;
  else
    sensitivity = false;
}

//----------------------------------------------------------------------------

void FapGraphCmds::getEditYAxisSensitivity(bool& sensitivity)
{
  FapCmdsBase::isModelTouchable(sensitivity);
  if (!sensitivity) return;

  std::vector<FmCurveSet*> curves = FapGraphCmds::findSelectedCurves(sensitivity);
  if (curves.size() == 1)
    sensitivity = curves.front()->usingInputMode() <= FmCurveSet::RDB_RESULT;
  else
    sensitivity = false;
}

//----------------------------------------------------------------------------

std::vector<FmCurveSet*> FapGraphCmds::findSelectedCurves(bool& curvesOnly)
{
  std::vector<FmCurveSet*> curves;
  std::vector<FFaViewItem*> permSelection;
  FFaViewItem* tmpSelection;
  FapEventManager::getSelection(permSelection,tmpSelection);

  curvesOnly = true;
  if (!tmpSelection && permSelection.empty())
    curvesOnly = false;
  else if (tmpSelection)
    if (dynamic_cast<FmCurveSet*>(tmpSelection))
      curves.push_back((FmCurveSet*)tmpSelection);
    else
      curvesOnly = false;
  else
    for (FFaViewItem* item : permSelection)
      if (dynamic_cast<FmCurveSet*>(item))
	curves.push_back(static_cast<FmCurveSet*>(item));
      else
	curvesOnly = false;

  return curves;
}

//----------------------------------------------------------------------------

std::vector<FmGraph*> FapGraphCmds::findSelectedGraphs(bool& graphsOnly)
{
  std::vector<FmGraph*> graphs;
  std::vector<FFaViewItem*> permSelection;
  FFaViewItem* tmpSelection;
  FapEventManager::getSelection(permSelection,tmpSelection);

  graphsOnly = true;
  if (!tmpSelection && permSelection.empty())
    graphsOnly = false;
  else if (tmpSelection)
    if (dynamic_cast<FmGraph*>(tmpSelection))
      graphs.push_back((FmGraph*)tmpSelection);
    else
      graphsOnly = false;
  else
    for (FFaViewItem* item : permSelection)
      if (dynamic_cast<FmGraph*>(item))
	graphs.push_back(static_cast<FmGraph*>(item));
      else
	graphsOnly = false;

  return graphs;
}

//----------------------------------------------------------------------------

FapUAGraphViewTLS* FapGraphCmds::getTLS(FmGraph* graph)
{
#ifdef FT_HAS_GRAPHVIEW
  std::vector<FapUAExistenceHandler*> allTLS;
  FapUAExistenceHandler::getAllOfType(FapUAGraphViewTLS::getClassTypeID(), allTLS);

  for (FapUAExistenceHandler* tls : allTLS) {
    FuiGraphViewTLS* tlsui = (FuiGraphViewTLS*)tls->getUI();
    if (((FapUAGraphView*)tlsui->getGraphViewComp()->getUA())->getDBPointer() == graph)
      return (FapUAGraphViewTLS*)tls;
  }
#else
  // Dummy statement to avoid compiler warning
  std::cout <<"  ** FapGraphCmds::getTLS(): No graph view for "
            << graph->getIdString(true) << std::endl;
#endif
  return NULL;
}

//----------------------------------------------------------------------------

void FapGraphCmds::killAllGraphViews()
{
  // RHR: Quick fix that hopefully fixes the issue with the program crashing,
  // when opening or undoing a file when a graph is open.
  // Also fixes that the program does not close properly with a graph view open
  FapEventManager::setActiveWindow(dynamic_cast<FuiModeller*>(FFuTopLevelShell::getInstanceByType(FuiModeller::getClassTypeID())));

#ifdef FT_HAS_GRAPHVIEW
  std::vector<FapUAExistenceHandler*> allTLS;
  FapUAExistenceHandler::getAllOfType(FapUAGraphViewTLS::getClassTypeID(), allTLS);

  for (FapUAExistenceHandler* tls : allTLS)
    ((FuiGraphViewTLS*)tls->getUI())->onClose();
#endif
}

//----------------------------------------------------------------------------

void FapGraphCmds::enableGraphDemoWarning(bool enable)
{
#ifdef FT_HAS_GRAPHVIEW
  std::vector<FapUAExistenceHandler*> allTLS;
  FapUAExistenceHandler::getAllOfType(FapUAGraphViewTLS::getClassTypeID(), allTLS);

  for (FapUAExistenceHandler* tls : allTLS)
    ((FuiGraphViewTLS*)tls->getUI())->getGraphViewComp()->enablePlotterDemoWarning(enable);
#else
  // Dummy statement to avoid compiler warning on unused variable
  std::cout <<"  ** FapGraphCmds::enableGraphDemoWarning() "<< enable << std::endl;
#endif
}

//----------------------------------------------------------------------------

void FapGraphCmds::getRepeatCurveSensitivity(bool& sensitivity)
{
  FapCmdsBase::isModelTouchable(sensitivity);
  if (!sensitivity) return;

  bool dummy = sensitivity = false;
  std::vector<FmCurveSet*> curves = FapGraphCmds::findSelectedCurves(dummy);
  if (curves.size() != 1) return;

  FmCurveSet* curveToRepeat = curves.front();
  if (curveToRepeat->usingInputMode() != FmCurveSet::TEMPORAL_RESULT) return;

  FFaResultDescription res = curveToRepeat->getResult(FmCurveSet::YAXIS);
  FmModelMemberBase* obj = res.baseId > 0 ? FmDB::findObject(res.baseId) : NULL;
  if (!obj) {
    res = curveToRepeat->getResult(FmCurveSet::YAXIS);
    obj = res.baseId > 0 ? FmDB::findObject(res.baseId) : NULL;
  }
  if (obj)
    sensitivity = FmDB::getObjectCount(obj->getTypeID()) > 1;
}

//----------------------------------------------------------------------------

/*!
  Creates a copy of the selected curve for each instance of the object type
  being plotted by the selected curve. It is the Y-axis object that is used,
  unless it does not exist. In the latter case the X-axis object is used.
  If the X- and Y-axis are referring to the same object, both axes are changed
  as looping over all objects of the actual type in the model.
*/

void FapGraphCmds::repeatCurve(int fromID, int toID)
{
  bool dummy;
  std::vector<FmCurveSet*> selected = FapGraphCmds::findSelectedCurves(dummy);
  if (selected.size() != 1) return;

  FmCurveSet* curveToRepeat = selected.front();
  if (curveToRepeat->usingInputMode() != FmCurveSet::TEMPORAL_RESULT) return;

  FFaResultDescription xRes = curveToRepeat->getResult(FmCurveSet::XAXIS);
  FFaResultDescription yRes = curveToRepeat->getResult(FmCurveSet::YAXIS);
  FmModelMemberBase* xObj = xRes.baseId > 0 ? FmDB::findObject(xRes.baseId) : NULL;
  FmModelMemberBase* yObj = yRes.baseId > 0 ? FmDB::findObject(yRes.baseId) : NULL;
  if (!xObj && !yObj) return; // selected curve does not plot result quantity

  FmModelMemberBase* obj = yObj ? yObj : xObj;
  std::vector<FmModelMemberBase*> objects;
  if (toID < 0)
    FmDB::getAllOfType(objects,obj->getTypeID());
  else
    FmDB::getAllOfType(objects,-obj->getTypeID(),
                       dynamic_cast<FmSubAssembly*>(obj->getParentAssembly()));

  if (toID == 0 && objects.size() > 2)
  {
    // Launch dialog to prompt for userID range
    FuiRepeatCurve* dialog = FuiRepeatCurve::getUI(true);
    dialog->getValues(fromID,toID);
    if (fromID >= toID)
    {
      FmDB::findIDrange(obj,fromID,toID);
      dialog->setValues(fromID,toID);
    }
    dialog->setLabel("Enter " + std::string(obj->getUITypeName()) +
		     " ID range\nto generate curves for");
    dialog->setDialogButtonClickedCB(FFaDynCB1S(FapGraphCmds::onRepeatCurveDone,int));
    dialog->execute();
    return;
  }

#ifdef FT_HAS_GRAPHVIEW
  // If the owner graph is visible, pop it down while adding the curves
  FapUAGraphViewTLS* tls = NULL;
  FmGraph* graph = curveToRepeat->getOwnerGraph();
  if (graph && (tls = FapGraphCmds::getTLS(graph)))
  {
    if (tls->getUI()->isPoppedUp())
      tls->getUI()->popDown();
    else
      tls = NULL;
  }
#endif

  FmCurveSet* newCurve;
  size_t nObjs = objects.size();
  for (size_t i = 0; i < nObjs; i++)
  {
    if (objects[i] == obj)
      newCurve = curveToRepeat;
    else if (toID < 1 || (objects[i]->getID() >= fromID && objects[i]->getID() <= toID)) {
      newCurve = new FmCurveSet;
      newCurve->clone(curveToRepeat, FmBase::DEEP_APPEND);
      if (xObj && (!yObj || xObj == yObj)) {
	xRes = newCurve->getResult(FmCurveSet::XAXIS);
	xRes.baseId = objects[i]->getBaseID();
	xRes.userId = objects[i]->getID();
	newCurve->setResult(FmCurveSet::XAXIS, xRes);
      }
      if (yObj) {
	yRes = newCurve->getResult(FmCurveSet::YAXIS);
	yRes.baseId = objects[i]->getBaseID();
	yRes.userId = objects[i]->getID();
	newCurve->setResult(FmCurveSet::YAXIS, yRes);
      }
    }
    else
      continue; // object is outside specified user ID range

    float r = 0.0f;
    float g = i <= nObjs/2 ? 0.0f : (i-nObjs/2)/(float)(nObjs/2);
    float b = i >= nObjs/2 ? 1.0f : i/(float)(nObjs/2);
    newCurve->setColor(r,g,b);
    newCurve->onDataChanged();
  }

#ifdef FT_HAS_GRAPHVIEW
  if (tls) {
    tls->getUI()->popUp();
    ((FapUAGraphView*)tls->getUI()->getGraphViewComp()->getUA())->updateSession();
  }
#endif
}

//----------------------------------------------------------------------------

void FapGraphCmds::onRepeatCurveDone(int button)
{
  FuiRepeatCurve* dialog = FuiRepeatCurve::getUI(false);
  if (button > 0) return; // Cancel

  int fromID, toID;
  dialog->getValues(fromID,toID);
  FapGraphCmds::repeatCurve(fromID,toID);
}

//----------------------------------------------------------------------------

void FapGraphCmds::toggleAutoExport(bool enable)
{
  FmSubAssembly* group = NULL;
  std::vector<FFaViewItem*> selection;
  if (FapCmdsBase::getCurrentSelection(selection))
    for (FFaViewItem* item : selection)
      if ((group = dynamic_cast<FmSubAssembly*>(item)))
      {
        std::vector<FmModelMemberBase*> curves;
        FmDB::getAllOfType(curves,FmCurveSet::getClassTypeID(),group);
        for (FmModelMemberBase* curve : curves)
          static_cast<FmCurveSet*>(curve)->setToBeExportedBatch(enable);
      }
}
