// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapExportCmds.H"
#ifdef FT_HAS_OWL
#include "vpmApp/vpmAppCmds/FapOilWellCmds.H"
#endif
#include "vpmApp/vpmAppCmds/FapAnimationCmds.H"
#include "vpmApp/vpmAppCmds/FapGraphCmds.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmApp/vpmAppDisplay/FapGraphDataMap.H"
#endif
#include "vpmApp/vpmAppDisplay/FapCGeoFile.H"
#include "vpmApp/FapLicenseManager.H"
#include "vpmApp/vpmAppProcess/FapSimEventHandler.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmBeam.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmSimulationEvent.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmAnimation.H"
#include "vpmDB/FmAnalysis.H"
#include "vpmDB/FmCurveSet.H"
#include "vpmDB/FmGraph.H"
#include "vpmDB/FmModelExpOptions.H"
#include "vpmDB/FmfExternalFunction.H"

#include "vpmUI/vpmUITopLevels/FuiModeller.H"
#include "vpmUI/vpmUITopLevels/FuiCtrlModeller.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmUI/vpmUITopLevels/FuiGraphView.H"
#else
class FuiGraphView {};
#endif
#include "vpmUI/vpmUITopLevels/FuiAnimExportSetup.H"
#include "vpmUI/vpmUITopLevels/FuiModelExport.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "vpmUI/Fui.H"

#ifdef FT_HAS_GRAPHVIEW
#include "FFpLib/FFpFatigue/FFpSNCurveLib.H"
#include "FFpLib/FFpCurveData/FFpGraph.H"
#endif
#include "vpmPM/FpPM.H"
#include "vpmPM/FpFileSys.H"
#include "vpmPM/FpModelRDBHandler.H"
#include "vpmPM/FpRDBExtractorManager.H"
#ifdef USE_INVENTOR
#include "vpmDisplay/FdDB.H"
#endif

#include "FFuLib/FFuProgressDialog.H"
#include "FFuLib/FFuFileDialog.H"
#include "FFaLib/FFaDefinitions/FFaViewItem.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaDefinitions/FFaAppInfo.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"

#include <algorithm>
#include <iterator>
#include <fstream>
#include <cctype>
#include <ctime>

#if defined(win32) || defined(win64)
#include <direct.h>
#define chdir  _chdir
#define getcwd _getcwd
#ifdef FT_HAS_GRAPHVIEW
#define FFpGraph__DAC FFpGraph::DAC_LITTLE_ENDIAN
#define FFpGraph__RPC FFpGraph::RPC_LITTLE_ENDIAN
#endif
#else
#include <unistd.h>
#ifdef FT_HAS_GRAPHVIEW
#define FFpGraph__DAC FFpGraph::DAC_BIG_ENDIAN
#define FFpGraph__RPC FFpGraph::RPC_BIG_ENDIAN
#endif
#endif

namespace Fap {
#ifdef FT_HAS_ZLIB
  //! \brief Utility to create a zip archive from a list of files.
  bool make_zip(const std::string& zipName, const Strings& fileNames);
#else
  bool make_zip(const std::string&, const Strings&) { return false; }
#endif
}


//! map/set sorting
struct idSort
{
  bool operator()(const FmBase* fm1, const FmBase* fm2) const
  {
    return (fm1->getID() < fm2->getID());
  }
};

//------------------------------------------------------------------------------

void FapExportCmds::init()
{
  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_export_exportObject");
  cmdItem->setText("Export Object...");
  cmdItem->setToolTip("Export selected object");
  cmdItem->setActivatedCB(FFaDynCB0S(FapExportCmds::exportObject));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapExportCmds::getExportObjectSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_export_exportLink");
  cmdItem->setText("Export Part...");
  cmdItem->setToolTip("Export selected part");
  cmdItem->setActivatedCB(FFaDynCB0S(FapExportCmds::exportObject));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapExportCmds::getExportLinkSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_export_exportCurves");
  cmdItem->setText("Export Curves...");
  cmdItem->setToolTip("Export one or more curves");
  cmdItem->setActivatedCB(FFaDynCB0S(FapExportCmds::exportCurves));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapExportCmds::getExportCurveSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_export_exportAllCurves");
  cmdItem->setText("Export all Curves...");
  cmdItem->setToolTip("Export all curves that are toggled for auto-export");
  cmdItem->setActivatedCB(FFaDynCB0S(FapExportCmds::exportCurvesAuto));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapExportCmds::getAutoExportCurveSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_export_exportGraphs");
  cmdItem->setText("Export Graphs...");
  cmdItem->setToolTip("Export one or more graphs");
  cmdItem->setActivatedCB(FFaDynCB0S(FapExportCmds::exportGraphs));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapExportCmds::getExportCurveSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_export_exportGraph");
  cmdItem->setText("Export Graph...");
  cmdItem->setToolTip("Export graphs into a single graph file");
  cmdItem->setActivatedCB(FFaDynCB0S(FapExportCmds::exportGraph));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapExportCmds::getExportCurveSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_export_graphStatistics");
  cmdItem->setText("Export Graph statistics...");
  cmdItem->setToolTip("Export statistical data for all curves in selected graph");
  cmdItem->setActivatedCB(FFaDynCB0S(FapExportCmds::exportGraphStatistics));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapExportCmds::getExportStatisticsSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_export_curveFatigue");
  cmdItem->setText("Export Curve fatigue...");
  cmdItem->setToolTip("Export fatigue data for all curves in selected graph");
  cmdItem->setActivatedCB(FFaDynCB0S(FapExportCmds::exportCurveFatigue));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapExportCmds::getExportFatigueSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_export_exportView");
  cmdItem->setText("Export View...");
  cmdItem->setToolTip("Export view");
  cmdItem->setActivatedCB(FFaDynCB0S(FapExportCmds::exportView));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapExportCmds::getExportViewSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_export_exportAnimation");
  cmdItem->setText("Export Animation...");
  cmdItem->setToolTip("Export animation");
  cmdItem->setActivatedCB(FFaDynCB0S(FapExportCmds::exportAnimation));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapExportCmds::getExportAnimationSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_export_exportVTF");
  cmdItem->setSmallIcon(vtf_xpm);
  cmdItem->setText("Export to VTF...");
  cmdItem->setToolTip("Export animation to VTF");
  cmdItem->setActivatedCB(FFaDynCB0S(FapExportCmds::exportVTF));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapExportCmds::getExportVTFSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_export_exportCGeo");
  cmdItem->setText("Export model to CGeo...");
  cmdItem->setToolTip("Export model to CGe");
  cmdItem->setActivatedCB(FFaDynCB0S(FapExportCmds::exportCGeo));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_export_dtsDigitalTwin");
  cmdItem->setText("Export Digital Twin...");
  cmdItem->setToolTip("Export current model to a zip'ed Digital Twin");
  cmdItem->setActivatedCB(FFaDynCB0S(FapExportCmds::exportDigitalTwin));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_export_pipeStringWear");
  cmdItem->setText("Export Pipe String Wear...");
  cmdItem->setToolTip("Export wear data for selected pipe string");
  cmdItem->setActivatedCB(FFaDynCB0S(FapExportCmds::exportPipeWear));
#ifdef FT_HAS_OWL
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapOilWellCmds::getExportPipeWearSensitivity,bool&));
#else
  cmdItem->setGetSensitivityCB(FFaDynCB1S([](bool& s){ s = false; },bool&));
#endif
}

//------------------------------------------------------------------------------

static FFuFileDialog* saveFile(const char* title = NULL)
{
  return FFuFileDialog::create(FmDB::getMechanismObject()->getAbsModelFilePath(),
                               title, FFuFileDialog::FFU_SAVE_FILE, true);
}

//------------------------------------------------------------------------------

/*!
  Export single object to file (graph/curve/part), depending on selection.
*/

void FapExportCmds::exportObject()
{
  FFuFileDialog* aDialog = saveFile();

  FFaViewItem* object = FapCmdsBase::findSelectedSingleObject();
  FmPart*      part   = NULL;
  FmCurveSet*  curve  = NULL;
  FmGraph*     graph  = NULL;

  // filters
  if ((part = dynamic_cast<FmPart*>(object)))
  {
    aDialog->addFilter("Fedem Part File", "ftl");
    aDialog->setTitle("Save part as");
    aDialog->remember("PartExport");
  }
#ifdef FT_HAS_GRAPHVIEW
  else if ((curve = dynamic_cast<FmCurveSet*>(object)))
  {
    aDialog->addFilter("ASCII file", FmGraph::asc(), true, FFpGraph::ASCII);
    aDialog->addFilter("nCode Dac File", FmGraph::dac(), false, FFpGraph__DAC);
    aDialog->addFilter("MTS RPCIII File", FmGraph::rpc(), false, FFpGraph__RPC);
    aDialog->setTitle("Save curve as");
    aDialog->remember("GraphCurve");
  }
  else if ((graph = dynamic_cast<FmGraph*>(object)))
  {
    aDialog->addFilter("Multi Column ASCII File", FmGraph::asc(), true, FFpGraph::ASCII);
    aDialog->addFilter("MTS RPCIII File", FmGraph::rpc(), false, FFpGraph__RPC);
    aDialog->setTitle("Save graph as");
    aDialog->remember("GraphCurve");
  }
#endif
  else {
    delete aDialog;
    return;
  }

  // get selected file name and associated format
  Strings fileNames = aDialog->execute();
  int fileFormat = 30 + aDialog->getSelectedFilter();
  delete aDialog;
  if (fileNames.empty()) return;

  const std::string& fileName = fileNames.front();
  std::string message;
  bool written = false;

  if (part)
    // write part data
    written = part->exportPart(fileName);
#ifdef FT_HAS_GRAPHVIEW
  else if (curve) {
    // write curve data
    FapGraphDataMap graphDataMap({curve},message);
    FFpGraph        graphData(graphDataMap.getFFpCurve(curve));

    std::string curveDescr = curve->getUserDescription();
    std::string curveName = FFaFilePath::distillName(curveDescr);

    written = graphData.writeCurve(fileName, fileFormat, curveDescr, curveName,
				   curve->getOwnerGraph()->getXaxisLabel(),
				   curve->getOwnerGraph()->getYaxisLabel(),
				   FmDB::getMechanismObject()->getModelFileName(),
				   message);
  }
  else if (graph) {
    // write graph data
    std::vector<FmCurveSet*> curves;
    graph->getCurveSets(curves,true);
    written = FapExportCmds::exportGraph(curves, fileName, fileFormat, message);
  }
#endif

  if (written)
    ListUI <<"  -> "<< static_cast<FmModelMemberBase*>(object)->getIdString()
	   <<"] exported to "<< fileName <<"\n";
  else if (!message.empty())
    FFaMsg::dialog(message,FFaMsg::ERROR);
}

//------------------------------------------------------------------------------

void FapExportCmds::getExportCurveSensitivity(bool& sensitivity)
{
  sensitivity = !FapLicenseManager::isDemoEdition();
  if (!sensitivity) return; // Not available in the demo edition

  bool dummy; // Check for directly selected curves
  std::vector<FmCurveSet*> curves = FapGraphCmds::findSelectedCurves(dummy);
  if (!curves.empty()) return;

  // Check for curves selected implicitly through selected graphs
  std::vector<FmGraph*> graphs = FapGraphCmds::findSelectedGraphs(dummy);
  for (FmGraph* graph : graphs)
    if (graph->hasCurveSets()) return;

  sensitivity = false;
}

//------------------------------------------------------------------------------

void FapExportCmds::findSelectedCurves(std::vector<FmCurveSet*>& curves)
{
  bool dummy; // Find what is selected
  curves = FapGraphCmds::findSelectedCurves(dummy);

  std::vector<FmGraph*> graphs = FapGraphCmds::findSelectedGraphs(dummy);
  if (graphs.empty()) return;

  // A selected graph implicitly means all its curves.
  // So we need to find all those curves not already among
  // the directly selected curves.

  std::set<FmGraph*,idSort> withCurveSelected;
  for (FmCurveSet* curve : curves)
    withCurveSelected.insert(curve->getOwnerGraph());

  std::set<FmGraph*,idSort> selectedGraphs;
  selectedGraphs.insert(graphs.begin(), graphs.end());

  std::set<FmGraph*,idSort> diff;
  std::set_difference(selectedGraphs.begin(), selectedGraphs.end(),
		      withCurveSelected.begin(), withCurveSelected.end(),
		      std::inserter(diff,diff.begin()));

  for (FmGraph* graph : diff)
  {
    std::vector<FmCurveSet*> crvs;
    graph->getCurveSets(crvs);
    curves.insert(curves.end(), crvs.begin(), crvs.end());
  }
}

//------------------------------------------------------------------------------

/*!
  Command slot for exporting one or more curves.
*/

void FapExportCmds::exportCurves()
{
  std::vector<FmCurveSet*> curves;
  FapExportCmds::findSelectedCurves(curves);

  if (curves.size() == 1)
    FapExportCmds::exportObject();
  else if (curves.size() > 1)
    FapExportCmds::exportMultCurves(curves);
}

//------------------------------------------------------------------------------

/*!
  Command slot for exporting one or more graphs.
*/

void FapExportCmds::exportGraphs()
{
  bool dummy; // Find what is selected
  std::vector<FmGraph*>    graphs = FapGraphCmds::findSelectedGraphs(dummy);
  std::vector<FmCurveSet*> curves = FapGraphCmds::findSelectedCurves(dummy);

  FapExportCmds::exportMultGraphs(curves, graphs);
}

//------------------------------------------------------------------------------

/*!
  Command slot for exporting one or more graphs to a single file.
*/

void FapExportCmds::exportGraph()
{
  bool dummy; // Find what is selected
  std::vector<FmGraph*>    graphs = FapGraphCmds::findSelectedGraphs(dummy);
  std::vector<FmCurveSet*> curves = FapGraphCmds::findSelectedCurves(dummy);

  FapExportCmds::exportMultGraphs(curves, graphs, true);
}

//------------------------------------------------------------------------------

/*!
  Command slot for auto-exporting all toggled curves for all events.
*/

void FapExportCmds::exportCurvesAuto()
{
#ifdef FT_HAS_GRAPHVIEW
  FFuFileDialog* aDialog = saveFile("Save graph as");

  aDialog->addFilter("Multi Column ASCII File",
                     FmGraph::asc(), true, FFpGraph::ASCII+1);
  aDialog->addFilter("Multi Column ASCII File, half precision",
                     FmGraph::asc(),false, FFpGraph::ASCII);
  aDialog->addFilter("Multi Column ASCII File, double precision",
                     FmGraph::asc(),false, FFpGraph::ASCII+2);
  aDialog->addFilter("MTS RPCIII File",
                     FmGraph::rpc(),false, FFpGraph__RPC);
  aDialog->remember("GraphCurve");

  Strings fileNames = aDialog->execute();
  int format = aDialog->getSelectedFilter();
  delete aDialog;
  if (fileNames.empty()) return;

  // If the model has events, do the export for all events,
  // unless one event is currently active
  bool doAllEvents = FmDB::hasObjects(FmSimulationEvent::getClassTypeID());
  if (FapSimEventHandler::getActiveEvent()) doAllEvents = false;
  if (doAllEvents) format += 100;

  FapExportCmds::autoExportCurves(fileNames.front(), format);
#endif
}

//------------------------------------------------------------------------------

void FapExportCmds::getAutoExportCurveSensitivity(bool& sensitivity)
{
  sensitivity = !FapLicenseManager::isDemoEdition();
  if (!sensitivity) return; // Not available in the demo edition

  std::vector<FmModelMemberBase*> allCurves;
  FmDB::getAllOfType(allCurves,FmCurveSet::getClassTypeID());
  for (FmModelMemberBase* curve : allCurves)
    if (static_cast<FmCurveSet*>(curve)->isAutoExported())
      return;

  sensitivity = false;
}

//------------------------------------------------------------------------------

/*!
  Export all toggled curves for all events, either as single files
  or all curves in one graph file.
*/

std::string FapExportCmds::autoExportCurves(const std::string& exportPath,
                                            int format, bool exportSingleGraph)
{
  std::string path;
#ifdef FT_HAS_GRAPHVIEW
  // Find all curves which are toggled for auto-export
  std::vector<FmCurveSet*> exportedCurves;
  std::vector<FmModelMemberBase*> allCurves;
  FmDB::getAllOfType(allCurves,FmCurveSet::getClassTypeID());
  for (FmModelMemberBase* curve : allCurves)
    if (static_cast<FmCurveSet*>(curve)->isAutoExported())
      exportedCurves.push_back(static_cast<FmCurveSet*>(curve));
  if (exportedCurves.empty()) return path;

  // We need to open the result database in case we were running batch
  FmMechanism* mech = FmDB::getMechanismObject();
  bool wasOpen = FpRDBExtractorManager::instance()->getModelExtractor() != NULL;
  if (!wasOpen)
  {
    FpRDBExtractorManager::instance()->createModelExtractor();
    if (format < 10) // we are exporting the master event in batch mode
      FpModelRDBHandler::RDBOpen(mech->getResultStatusData(),mech);
  }
  else if (format >= 10) // We are exporting events, so
    FpModelRDBHandler::RDBRelease(true,true); // close the master event RDB first

  std::vector<FmSimulationEvent*> events(1,FapSimEventHandler::getActiveEvent());
  if (format >= 10) FmDB::getAllSimulationEvents(events);
  if (format >= 100) events.push_back(NULL); // the master event

  FFuProgressDialog* progDlg = NULL;
  if (wasOpen && events.size() > 1)
    progDlg = FFuProgressDialog::create("Please wait...", "Cancel",
					"Exporting Curves", events.size());

  // Now do the curve export, event by event
  int numEvent = 0;
  for (FmSimulationEvent* event : events)
  {
    numEvent++;
    if (progDlg)
    {
      progDlg->setCurrentProgress(numEvent);
      if (progDlg->userCancelled()) break;
    }

    FmResultStatusData* eventRsd;
    if (!event)
    {
      // We are doing the master event
      path = exportPath;
      eventRsd = mech->getResultStatusData();
      FFaFilePath::makeItAbsolute(path,mech->getAbsModelFilePath());
      if (numEvent > 0)
	FpModelRDBHandler::RDBOpen(eventRsd,mech,wasOpen);
    }
    else
    {
      path = event->eventName(exportPath);
      eventRsd = event->getResultStatusData();
      if (format >= 10)
      {
	// Open the result database for next event
	FpRDBExtractorManager::instance()->createModelExtractor();
	FpModelRDBHandler::RDBOpen(eventRsd,mech);
      }
    }

    std::string message;
    if (FpModelRDBHandler::hasResults(eventRsd))
    {
      if (exportSingleGraph)
      {
	if (path.rfind('.') == std::string::npos) path += ".asc";
	ListUI <<"\n===> Exporting curves to "<< path <<"\n";
	FapExportCmds::exportGraph(exportedCurves,path,format%10,message);
	ListUI <<"\n";
      }
      else if (FpFileSys::verifyDirectory(path))
      {
	ListUI <<"\n===> Exporting curves to "<< path <<"\n";
	FapExportCmds::exportCurves(exportedCurves,path,format%10,message);
	ListUI <<"\n";
      }
      else
	ListUI <<"\n *** Could not access directory "<< path
	       <<"\n     Curve export NOT performed\n";
    }
    if ((event && format >= 10) || !wasOpen)
      FpModelRDBHandler::RDBRelease(true,true);

    if (!message.empty())
    {
      ListUI << message <<"\n";
      if (event)
        ListUI <<"\nDetected while exporting "<< event->getIdString() <<".\n";
    }

#ifdef FT_STEP_BY_STEP_EXPORT
    static char answer = 'y';
    if (answer != 'Y')
    {
      std::cout <<"Continue? ";
      std::cin >> answer;
      if (answer == '0' || answer == 'n') break;
    }
#endif
  }

  if (progDlg)
  {
    progDlg->setCurrentProgress(events.size());
    delete progDlg;
  }

  if (wasOpen && format >= 10)
  {
    FpModelRDBHandler::RDBRelease(true); // Renewing the possibility extractor as well
    FpModelRDBHandler::RDBOpen(FapSimEventHandler::getActiveRSD(),mech,true);
  }
#endif

  return path;
}

//------------------------------------------------------------------------------

/*!
  Export one or more curves to individual files.
*/

void FapExportCmds::exportMultCurves(const std::vector<FmCurveSet*>& curves)
{
  if (curves.empty()) return; // nothing selected

#ifdef FT_HAS_GRAPHVIEW
  FFuFileDialog* aDialog = FFuFileDialog::create(FmDB::getMechanismObject()->getAbsModelFilePath(),
                                                 "Save curves to directory", FFuFileDialog::FFU_DIRECTORY);

  Strings exts    = FmGraph::asc();
  Strings dacExts = FmGraph::dac();
  Strings rpcExts = FmGraph::rpc();
  exts.insert(exts.end(), dacExts.begin(), dacExts.end());
  exts.insert(exts.end(), rpcExts.begin(), rpcExts.end());
  aDialog->addFilter("Curve files", exts);

  // Option menu for selecting file format
  std::string ascii("ASCII File");
  std::string dac  ("nCode Dac File");
  std::string rpc  ("MTS RPCIII File");
  aDialog->addUserOptionMenu("Format", "File format:");
  aDialog->addUserOption("Format", ascii);
  aDialog->addUserOption("Format", dac);
  aDialog->addUserOption("Format", rpc);
  aDialog->remember("GraphCurve");

  Strings dirPaths = aDialog->execute();
  std::string fileType = aDialog->getSelectedUserOption("Format");
  delete aDialog;
  if (dirPaths.empty()) return;

  int format;
  if (fileType == ascii)
    format = FFpGraph::ASCII;
  else if (fileType == dac)
    format = FFpGraph__DAC;
  else
    format = FFpGraph__RPC;

  std::string message;
  FapExportCmds::exportCurves(curves, dirPaths.front(), format, message);

  if (!message.empty())
    FFaMsg::dialog(message,FFaMsg::ERROR);
#endif
}

//------------------------------------------------------------------------------

void FapExportCmds::exportCurves(const std::vector<FmCurveSet*>& curves,
				 const std::string& dirPath, int format,
				 std::string& message)
{
#ifdef FT_HAS_GRAPHVIEW
  // Define file extension depending on export format
  std::string ext;
  switch (format) {
  case FFpGraph::ASCII:
    ext = ".asc";
    break;
  case FFpGraph::DAC_LITTLE_ENDIAN:
  case FFpGraph::DAC_BIG_ENDIAN:
    ext = ".dac";
    break;
  case FFpGraph::RPC_LITTLE_ENDIAN:
  case FFpGraph::RPC_BIG_ENDIAN:
    ext = ".rsp";
  default:
    ext = ".dat";
  }

  // Find data for all the curves
  FapGraphDataMap graphDataMap(curves,message);

  // Now we have the data for all curves in graphDataMap.
  // Loop over all curves and write their data to files.
  for (FmCurveSet* curve : curves)
  {
    FFpGraph graphData(graphDataMap.getFFpCurve(curve));

    // It is time to figure out a good name for the curve.
    // Names will be on the form G_<gid>_C_<id>_<descr>.<ext>
    std::string descr = FFaFilePath::distillName(curve->getUserDescription());
    std::string fName = FFaNumStr("G_%d_",curve->getOwnerGraph()->getID()) +
                   FFaNumStr("C_%d_",curve->getID()) + descr + ext;

    // Write curve data to file
    if (graphData.writeCurve(FFaFilePath::appendFileNameToPath(dirPath,fName),
			     30+format, curve->getUserDescription(), descr,
			     curve->getOwnerGraph()->getXaxisLabel(),
			     curve->getOwnerGraph()->getYaxisLabel(),
			     FmDB::getMechanismObject()->getModelFileName(),
			     message))
      ListUI <<"  -> "<< curve->getIdString() <<" exported to "<< fName <<"\n";
  }
#endif
}

//------------------------------------------------------------------------------

/*!
  Export one or more graphs to multi-channel files.
*/

void FapExportCmds::exportMultGraphs(const std::vector<FmCurveSet*>& curves,
				     const std::vector<FmGraph*>& graphs,
				     bool onlyOneGraph)
{
  if (curves.empty() && graphs.empty()) return; // nothing selected

#ifdef FT_HAS_GRAPHVIEW
  // Check if we need to write multiple graph files
  if (!onlyOneGraph && graphs.size() < 2) {
    onlyOneGraph = true;
    if (!curves.empty()) {
      FmGraph* g = graphs.empty() ? curves.front()->getOwnerGraph() : graphs.front();
      for (FmCurveSet* curve : curves)
	if (curve->getOwnerGraph() != g)
	  onlyOneGraph = false;
    }
  }

  FFuFileDialog* aDialog = NULL;
  Strings ascExts = FmGraph::asc();
  Strings rpcExts = FmGraph::rpc();

  // Option menu for selecting file format
  std::string asc("Multi Column ASCII File");
  std::string rpc("MTS RPCIII File");
  std::string wnd("SESAM Wind File");
  const int GRAPH_SESAM = FFpGraph::RPC_BIG_ENDIAN+1;

  if (onlyOneGraph) {
    aDialog = saveFile("Save graph as");
    aDialog->addFilter(asc, ascExts, true, FFpGraph::ASCII);
    aDialog->addFilter(rpc, rpcExts, false, FFpGraph__RPC);
    aDialog->addFilter(wnd, "wind", false, GRAPH_SESAM);
    aDialog->addUserToggle("noHeader", "Exclude header", false);
  }
  else {
    aDialog = FFuFileDialog::create(FmDB::getMechanismObject()->getAbsModelFilePath(),
                                    "Save graphs to directory", FFuFileDialog::FFU_DIRECTORY);
    ascExts.insert(ascExts.end(), rpcExts.begin(), rpcExts.end());
    ascExts.push_back("wind");
    aDialog->addFilter("Graph files", ascExts);
    aDialog->addUserToggle("noHeader", "Exclude header", false);
    aDialog->addUserOptionMenu("Format", "File format:");
    aDialog->addUserOption("Format", asc);
    aDialog->addUserOption("Format", rpc);
    aDialog->addUserOption("Format", wnd);
  }

  aDialog->remember("GraphCurve");

  Strings dirPath = aDialog->execute();
  if (dirPath.empty()) {
    delete aDialog;
    return;
  }

  std::string& file = dirPath.front();

  int  format   = FFpGraph::ASCII;
  bool noHeader = aDialog->getUserToggleSet("noHeader");
  bool windFile = false;
  if (onlyOneGraph)
    if (aDialog->getSelectedFilter() != GRAPH_SESAM)
      format = aDialog->getSelectedFilter();
    else
      windFile = true;
  else if (aDialog->getSelectedUserOption("Format") == wnd)
    windFile = true;
  else if (aDialog->getSelectedUserOption("Format") == rpc)
    format = FFpGraph__RPC;
  delete aDialog;

  typedef std::map<FmGraph*,std::vector<FmCurveSet*>,idSort> GraphCrvMap;

  GraphCrvMap graphMap;
  for (FmGraph* graph : graphs)
    graph->getCurveSets(graphMap[graph],true);

  // Append selected curves if not already included through the selected graphs
  for (FmCurveSet* curve : curves) {
    std::vector<FmCurveSet*>& expCurves = graphMap[curve->getOwnerGraph()];
    if (std::find(expCurves.begin(),expCurves.end(),curve) == expCurves.end())
      expCurves.push_back(curve);
  }

  std::string message;
  if (onlyOneGraph)
  {
    // Export curves from multiple graphs into a single file
    std::vector<FmCurveSet*> allCurves;
    for (const GraphCrvMap::value_type& gc : graphMap)
      allCurves.insert(allCurves.end(), gc.second.begin(), gc.second.end());

    if (FapExportCmds::exportGraph(allCurves,file,30+format,message,noHeader || windFile))
      ListUI <<"  -> Graph exported to "<< file <<"\n";
  }
  else for (const GraphCrvMap::value_type& gc : graphMap)
  {
    // Export curves into one file for each graph
    FmGraph* graph = gc.first;
    std::string fileName = FFaFilePath::appendFileNameToPath(file,FFaNumStr("G_%d_",graph->getID()))
      + FFaFilePath::distillName(graph->getUserDescription()) + ".";
    if (windFile)
      fileName += "wind";
    else if (format == FFpGraph::ASCII)
      fileName += ascExts.front();
    else
      fileName += rpcExts.front();

    if (FapExportCmds::exportGraph(gc.second,fileName,30+format,message,noHeader || windFile))
      ListUI <<"  -> Graph ["<< graph->getID()
	     <<"] exported to "<< fileName <<"\n";
  }

  if (!message.empty()) {
    ListUI << message <<"\n";
    FFaMsg::dialog(message,FFaMsg::ERROR);
  }
#endif
}

//------------------------------------------------------------------------------

/*!
  Export a set of curves to the specified graph file.
*/

bool FapExportCmds::exportGraph(const std::vector<FmCurveSet*>& curves,
				const std::string& fileName, int format,
				std::string& message, bool noHeader)
{
#ifdef FT_HAS_GRAPHVIEW
  if (format <= 2)
    format = 10*format; // half, single or double precision ASCII
  else if (format < 10)
    format += 30; // use default precision for RPC

  FapGraphDataMap graphDataMap(curves,message);
  FFpGraph        graphData;

  Strings curveDescr, curveName;
  curveDescr.reserve(curves.size());
  curveName.reserve(curves.size());

  for (FmCurveSet* curve : curves) {
    graphData.addCurve(graphDataMap.getFFpCurve(curve));
    curveDescr.push_back(curve->getUserDescription());
    curveName.push_back(FFaFilePath::distillName(curveDescr.back()));
  }
  graphData.setNoHeaderState(noHeader);
  return graphData.writeGraph(fileName, format, curveDescr, curveName,
			      FmDB::getMechanismObject()->getModelFileName(),
			      message);
#else
  return false;
#endif
}

//------------------------------------------------------------------------------

static void writeMetaData(std::ostream& os, const std::string& fileName)
{
  FFaAppInfo current;
  os <<"# Fedem version: "<< current.version <<"\n";
  os <<"# Model file: "<< FmDB::getMechanismObject()->getModelFileName() <<"\n";
  os <<"# This file: "<< fileName <<"\n";
  os <<"# Exported by: "<< current.user <<", "<< current.date <<"\n#\n";
}

//------------------------------------------------------------------------------

/*!
  Export curve statistics for all curves in the selected graph.
*/

void FapExportCmds::exportGraphStatistics()
{
#ifdef FT_HAS_GRAPHVIEW
  FmGraph* graph = dynamic_cast<FmGraph*>(FapCmdsBase::findSelectedSingleObject());
  if (!graph) return;

  std::vector<FmCurveSet*> curves;
  graph->getCurveSets(curves,true);
  if (curves.empty()) return;

  FFuFileDialog* aDialog = saveFile("Export curve statistics");
  aDialog->addFilter("Statistics file",FmGraph::asc());
  aDialog->remember("CurveStatistics");

  Strings fileNames = aDialog->execute();
  delete aDialog;
  if (fileNames.empty()) return;

  std::string message;
  FapGraphDataMap graphDataMap(curves,message);
  if (!message.empty())
  {
    FFaMsg::dialog(message,FFaMsg::ERROR);
    return;
  }

  std::vector<FapCurveSet> curveStat;
  if (!graphDataMap.getCurveStatistics(curveStat)) return;

  // Beta feature: Extraction of measured depth from Cam joint description
  bool isWearData = FFaString(graph->getUserDescription()).hasSubString("#Wear");

  std::ofstream outputFile(fileNames.front().c_str());
  writeMetaData(outputFile,fileNames.front());
  outputFile <<"#CurveID";
  if (isWearData) outputFile <<"\tMD";
  outputFile <<"\tRMS\tAVG\tStdDev\tIntegral\tMin\tMax\n";
  for (const FapCurveSet& stat : curveStat)
  {
    outputFile << stat.first->getID() << '\t';
    if (isWearData)
    {
      double MD = 0.0; // Get measured depth from Cam joint description
      FmModelMemberBase* obj = stat.first->getResultObj(FmCurveSet::YAXIS);
      if (obj)
	MD = FFaString(obj->getUserDescription()).getDoubleAfter("MD:");
      outputFile << MD <<'\t';
    }
    outputFile << stat.second.rms << '\t'
	       << stat.second.avg << '\t'
	       << stat.second.stdDev << '\t'
	       << stat.second.integral << '\t'
	       << stat.second.min << '\t'
	       << stat.second.max << '\n';
  }

  ListUI <<"  -> Curve statistics for Graph ["<< graph->getID()
	 <<"] exported to "<< fileNames.front() <<"\n";
#endif
}

//----------------------------------------------------------------------------

void FapExportCmds::getExportStatisticsSensitivity(bool& sensitivity)
{
  FFaViewItem* selected = FapCmdsBase::findSelectedSingleObject();
  sensitivity = dynamic_cast<FmGraph*>(selected) ? true : false;
}

//------------------------------------------------------------------------------

/*!
  Export fatigue results for all selected curves.
  Only for simulation events + weighted fatigue.
*/

void FapExportCmds::exportCurveFatigue()
{
#ifdef FT_HAS_GRAPHVIEW
  std::vector<FmCurveSet*> curves;
  FapExportCmds::findSelectedCurves(curves);
  for (std::vector<FmCurveSet*>::iterator cit = curves.begin(); cit != curves.end();)
    if ((*cit)->isFatigueCurve())
      ++cit;
    else
      cit = curves.erase(cit);
  if (curves.empty()) return; // none of the selected curves had proper results

  FFuFileDialog* aDialog = saveFile("Export curve fatigue");
  aDialog->addFilter("Fatigue file",FmGraph::asc());
  aDialog->remember("CurveStatistics");

  Strings fileNames = aDialog->execute();
  delete aDialog;
  if (fileNames.empty()) return;

  std::vector<FmSimulationEvent*> events;
  FmDB::getAllSimulationEvents(events);
  if (events.empty()) return;

  // Use the time domain specification for the first curve only
  bool wholeDomain = curves.front()->getFatigueEntireDomain();
  double startT = curves.front()->getFatigueDomain().first;
  double stopT = curves.front()->getFatigueDomain().second;

  // Find the S-N curves to base the fatigue analysis on
  size_t i, nE = events.size();
  size_t j, nC = curves.size();
  std::vector<FFpSNCurve*> snC(nC);
  std::vector<DoubleVec>   damage(nC);
  for (j = 0; j < nC; j++)
  {
    int snStd = curves[j]->getFatigueSNStd();
    int snCurve = curves[j]->getFatigueSNCurve();
    snC[j] = FFpSNCurveLib::instance()->getCurve(snStd,snCurve);
    if (!snC[j])
    {
      ListUI <<"===> Error in damage calculation - Undefined SN-curve: "
	     <<"StdIndex="<< snStd <<" CurveIndex="<< snCurve <<"\n";
      return;
    }
    damage[j].reserve(1+nE);
    damage[j].push_back(0.0);
  }

  bool cancelled = false;
  FFuProgressDialog* progDlg = FFuProgressDialog::create("Please wait...", "Cancel",
							 "Exporting Curve Damage",
							 nE*(1+nC));

  // Now do the fatigue analysis, event by event
  for (i = 0; i < events.size() && !cancelled; i++)
  {
    progDlg->setCurrentProgress(i*(1+nC));
    cancelled = progDlg->userCancelled();
    if (cancelled) break;

    // Open result database for next event
    FmResultStatusData* rsd = events[i]->getResultStatusData();
    if (i > 0 || events[i] != FapSimEventHandler::getActiveEvent())
    {
      FpModelRDBHandler::RDBRelease(true,true);
      FpModelRDBHandler::RDBOpen(rsd,FmDB::getMechanismObject());
    }
    if (FpModelRDBHandler::hasResults(rsd))
    {
      std::string message;
      FapGraphDataMap graphDataMap(curves,message);
      if (!message.empty())
	ListUI <<"===> Damage calculation failed for "
	       << events[i]->getIdString() <<"\n     "<< message <<"\n";
      else for (j = 0; j < nC; j++)
      {
	progDlg->setCurrentProgress(i*(1+nC)+1+j);
	cancelled = progDlg->userCancelled();
	if (cancelled) break;

	damage[j].push_back(graphDataMap.getDamageFromCurve(curves[j],
							    curves[j]->getFatigueGateValue(),true,
							    wholeDomain,startT,stopT,*snC[j]));
	damage[j].front() += damage[j].back()*events[i]->getProbability();
      }
    }
    else
      events.erase(events.begin()+(i--));
  }

  if (!cancelled)
  {
    progDlg->setCurrentProgress(nE*(1+nC));
    cancelled = progDlg->userCancelled();
  }
  delete progDlg;

  if (!cancelled)
  {
    std::ofstream outputFile(fileNames.front().c_str());
    writeMetaData(outputFile,fileNames.front());
    outputFile <<"#CurveID\tWeighted\t";
    for (j = 0; j < events.size(); j++)
      outputFile <<"\tEvent_"<< events[j]->getID();
    for (i = 0; i < nC; i++)
    {
      outputFile <<'\n'<< curves[i]->getID() <<'\t';
      for (j = 0; j <= events.size(); j++)
	outputFile <<'\t'<< damage[i][j];
    }
    outputFile <<'\n';
    ListUI <<"  -> Curve fatigue exported to "<< fileNames.front() <<"\n";
  }

  FpModelRDBHandler::RDBRelease(true);
  FpModelRDBHandler::RDBOpen(FapSimEventHandler::getActiveRSD(),
			     FmDB::getMechanismObject(),true);
#endif
}

//------------------------------------------------------------------------------

void FapExportCmds::getExportFatigueSensitivity(bool& sensitivity)
{
  sensitivity = !FapLicenseManager::isDemoEdition();
  if (!sensitivity) return; // Not available in the demo edition

  sensitivity = FmDB::hasObjects(FmSimulationEvent::getClassTypeID());
  if (!sensitivity) return; // Only for event results

  std::vector<FmCurveSet*> curves;
  FapExportCmds::findSelectedCurves(curves);
  for (FmCurveSet* curve : curves)
    if (curve->isFatigueCurve()) return;

  sensitivity = false;
}

//----------------------------------------------------------------------------

/*!
  Exports the view, 3D or graph (ctrl modeller not yet) to an image file.
*/

void FapExportCmds::exportView()
{
  FFuFileDialog* aDialog = saveFile("Save view as");

  FuiModeller* mod;
  FuiCtrlModeller* ctrl;
  FuiGraphView* graphview;

  // filters
  if ((mod = FapCmdsBase::getActiveModeller())) {
    aDialog->remember("modExport");
    aDialog->addFilter("2D RGB Snapshot","rgb");
    aDialog->addFilter("2D JPEG Snapshot","jpeg");
    aDialog->addFilter("2D BMP Snapshot","bmp");
    aDialog->addFilter("2D PNG Snapshot","png");
    aDialog->addFilter("3D Inventor Snapshot","iv");
  }
  else if ((ctrl = FapCmdsBase::getActiveCtrlModeller())) {
    aDialog->remember("ctrlExport");
    aDialog->addFilter("2D RGB Snapshot","rgb");
    aDialog->addFilter("3D Inventor Snapshot","iv");
  }
  else if ((graphview = FapCmdsBase::getActiveGraphView())) {
    aDialog->remember("graphExport");
    aDialog->addFilter("PNG Image","png");
    aDialog->addFilter("BMP Image","bmp");
    aDialog->addFilter("JPEG Image","jpeg");
  }

  // get file
  Strings fileNames = aDialog->execute();
  delete aDialog;
  if (fileNames.empty()) return;

  const std::string& fileName = fileNames.front();

  // write
  bool written = false;
  if (mod) {
#ifdef USE_INVENTOR
    if (FFaFilePath::isExtension(fileName,"rgb"))
      written = FdDB::exportRGB(fileName.c_str());
    else if (FFaFilePath::isExtension(fileName,"jpeg"))
      written = FdDB::exportJPEG(fileName.c_str());
    else if (FFaFilePath::isExtension(fileName,"png"))
      written = FdDB::exportPNG(fileName.c_str());
    else if (FFaFilePath::isExtension(fileName,"bmp"))
      written = FdDB::exportBMP(fileName.c_str());
    else if (FFaFilePath::isExtension(fileName,"iv"))
      written = FdDB::exportIV(fileName.c_str());
#endif
  }
  else if (ctrl) {
    ListUI <<"  -> Export of Control System view is not implemented yet.\n";
  }
  else if (graphview) {
#ifdef FT_HAS_GRAPHVIEW
    if (FFaFilePath::isExtension(fileName,"png"))
      written = graphview->saveAsImage(fileName,"PNG");
    else if (FFaFilePath::isExtension(fileName,"bmp"))
      written = graphview->saveAsImage(fileName,"BMP");
    else if (FFaFilePath::isExtension(fileName,"jpeg"))
      written = graphview->saveAsImage(fileName,"JPEG");
#endif
  }

  if (written)
    ListUI <<"  -> View exported to "<< fileName <<"\n";
  else
    FFaMsg::dialog("Could not export view to " + fileName, FFaMsg::ERROR);
}

//----------------------------------------------------------------------------

/*!
  Exports a loaded animation to mpeg or avi.
*/

void FapExportCmds::exportAnimation()
{
  static FmAnimation* lastAnim = NULL;
  FuiAnimExportSetup* setup = FuiAnimExportSetup::getUI(false);
  setup->setDialogButtonClickedCB(FFaDynCB1S(FapExportCmds::onAnimationExportSetupDone,int));

  // Set default file name, unless it is the same animation as last time
  if (FapAnimationCmds::getCurrentAnimation() != lastAnim)
  {
    lastAnim = FapAnimationCmds::getCurrentAnimation();
    std::string path = FmDB::getMechanismObject()->getAbsModelFilePath();
    std::string name = FFaFilePath::distillName(lastAnim->getUserDescription());
    std::string defaultFile = FFaFilePath::appendFileNameToPath(path,name) + ".mpeg";
    setup->setDefaultFileName(defaultFile);
  }

  setup->execute();
}

//----------------------------------------------------------------------------

/*!
  CB from animation setup dialog.
*/

void FapExportCmds::onAnimationExportSetupDone(int button)
{
  FuiAnimExportSetup* dialog = FuiAnimExportSetup::getUI(false);
  if (button > 0) return; // Cancel

  bool allFrames, realTime, omitFrames, includeFrames;
  int framesToOmit, framesToInclude;
  dialog->getSetupValues(allFrames, realTime, omitFrames, includeFrames,
			 framesToOmit, framesToInclude);

  std::string fileName; int fileFormat;
  dialog->getFileValues(fileName, fileFormat);

  if (FapAnimationCmds::exportAnim(allFrames, realTime,
				   omitFrames, includeFrames,
				   framesToOmit, framesToInclude,
				   fileName, fileFormat))
    ListUI <<"  -> Animation ["<< FapAnimationCmds::getCurrentAnimation()->getID()
	   <<"] exported to "<< fileName <<"\n";
}

//----------------------------------------------------------------------------

/*!
  Loads the selected animation and exports it to VTF.
*/

void FapExportCmds::exportVTF()
{
  // Do nothing if there was no animation
  FmAnimation* anim = FapAnimationCmds::findSelectedAnimation();
  if (!anim) return;

  FFuFileDialog* aDialog = saveFile("Save animation to GLview VTF file");
  aDialog->addFilter("Express VTF-file","vtf",true,0);
  aDialog->addFilter("Binary VTF-file","vtf",false,1);
  aDialog->addFilter("ASCII VTF-file","vtf",false,2);
  aDialog->addUserField("Time increment:",0.0);
  aDialog->addUserToggle("firstOrder","Export as first-order elements",false);
  aDialog->remember("VTFExport");

  Strings fileNames = aDialog->execute();
  int fileType = aDialog->getSelectedFilter();
  bool firstOrder = aDialog->getUserToggleSet("firstOrder");
  double xTimeInc = aDialog->getUserFieldValue("Time increment:");
  delete aDialog;
  if (fileNames.empty()) return;

  if (FapAnimationCmds::exportVTF(anim,fileNames.front(),fileType,firstOrder,xTimeInc))
    ListUI <<"  -> Animation ["<< anim->getID()
	   <<"] exported to "<< fileNames.front() <<"\n";
}

//------------------------------------------------------------------------------

/*!
  Export to VTF all toggled animations.
*/

void FapExportCmds::autoExportToVTF(const std::string& exportDir,
                                    int vtfFormat, bool asFirstOrder)
{
  bool batchOpen = false;
  FmMechanism* mech = FmDB::getMechanismObject();

  // Export all animations which are toggled for auto-export
  std::vector<FmModelMemberBase*> allAnims;
  FmDB::getAllOfType(allAnims,FmAnimation::getClassTypeID());
  for (FmModelMemberBase* anim : allAnims)
    if (static_cast<FmAnimation*>(anim)->autoExport.getValue())
    {
      // We need to open the result database in case we were running batch
      if (FpRDBExtractorManager::instance()->getModelExtractor() == NULL)
      {
	batchOpen = true;
	FpRDBExtractorManager::instance()->createModelExtractor();
	FpModelRDBHandler::RDBOpen(mech->getResultStatusData(),mech);
      }
      if (!FpModelRDBHandler::hasResults(mech->getResultStatusData()))
	break;

      std::string fName = FFaFilePath::distillName(anim->getUserDescription()) + ".vtf";
      if (FapAnimationCmds::exportVTF(static_cast<FmAnimation*>(anim),
				      FFaFilePath::appendFileNameToPath(exportDir,fName),
				      vtfFormat,asFirstOrder))
	ListUI <<"  -> Animation ["<< anim->getID() <<"] exported to "<< fName <<"\n";
    }

  if (batchOpen)
    FpModelRDBHandler::RDBRelease(true,true);
}

//----------------------------------------------------------------------------

/*!
  Exports the model to ceetron CGeo binary format
*/

void FapExportCmds::exportCGeo()
{
  if (!FapLicenseManager::checkLicense("FA-SAP"))
    return;

  FFuFileDialog* aDialog = saveFile("Save model to Ceetron CGeo file");
  aDialog->addFilter("Ceetron CGeo-file", "cgeo", true, 0);
  aDialog->remember("CGeoExport");

  Strings fileNames = aDialog->execute();
  delete aDialog;

  // Create the CGeo file
  if (!fileNames.empty() && FapCGeo::writeGeometry(fileNames.front()))
    ListUI <<"  -> Model exported to "<< fileNames.front() <<"\n";
}

//------------------------------------------------------------------------------

/*!
  Static helper to write the resource configuration file
  for an EPD Connected Products simulation app.
*/

static bool write_res_config(const std::string& fName)
{
  // TODO: Maybe replace by some configurable options in the export dialog?
  const char* config[9] = {
    "containers:",
    "  app-container-fedempy:",
    "    requests:",
    "      cpu: 2",
    "      memory: 1Gi",
    "    limits:",
    "      cpu: 4",
    "      memory: 2Gi",
    NULL };

  std::ofstream os(fName.c_str());
  if (!os) return false;

  for (const char** p = config; *p; ++p)
    os << *p <<"\n";

  return true;
}


/*!
  Static helper to write the app.json file
  for an EPD Connected Products simulation app.
*/

static bool write_app_json(const std::string& fName,
                           const std::string& appName, double window_size,
                           const std::string& IG_inputs, const Strings& inputs,
                           const std::string& IG_outputs = "",
                           const std::map<std::string,FmThreshold>* outputs = NULL)
{
  if (inputs.empty())
  {
    FFaMsg::dialog("No input functions in the model.\nCan't export "+
                   std::string(window_size > 0.0 ? "streaming" : "batch") +
                   " app.",FFaMsg::ERROR);
    return false;
  }

  if (outputs && outputs->empty())
    outputs = NULL;

  std::ofstream os(fName.c_str());
  if (!os) return false;

  // Lambda function for writing a string to the file, inserting ("") around
  // each alphanumeric string, and assuming the syntax $<n> to indicate a
  // newline followed by an indent of length <n>.
  auto&& writeWithFnutts=[&os](const std::string& str)
  {
    bool wasNewLine = false;
    bool wasAlphaNum = false;
    for (char c : str)
      if (wasNewLine)
      {
        for (char d = '0'; d < c; d++) os << ' ';
        wasNewLine = false;
      }
      else
      {
        bool isAlphaNum = isalnum(c) || c == '-' || c == '_' || c == '.';
        if (isAlphaNum != wasAlphaNum) os << '"';
        wasAlphaNum = isAlphaNum;
        wasNewLine = c == '$';
        if (wasNewLine)
          os << '\n';
        else
          os << c;
      }
    if (wasAlphaNum) os << '"';
  };

  writeWithFnutts("{$2version: 3,$2name: ");
  writeWithFnutts(appName);
  if (window_size > 0.0)
  {
    // Streaming app with specified window length
    std::string window;
    double wind_int = 0;
    if (modf(window_size,&wind_int) == 0.0)
      window = std::to_string((int)wind_int) + "sec,";
    else if (modf(1.0e3*window_size,&wind_int) == 0.0)
      window = std::to_string((int)wind_int) + "ms,";
    else
      window = std::to_string((long long int)floor(1.0e6*window_size)) + "mcs,";
    writeWithFnutts(",$2windowing: {$4window: " + window);
    writeWithFnutts("$4overlap: 0sec,$4horizon: 0sec$2},");
  }
  else // Batch app
    writeWithFnutts(",$2type: batch,");

  writeWithFnutts("$2types: {$4" + IG_inputs + ": {");
  for (size_t i = 0; i < inputs.size(); i++)
  {
    writeWithFnutts("$6" + inputs[i] + ": {$8type: float,$8unit: -$6}");
    if (i < inputs.size()-1) os << ',';
  }

  int nthres = 0;
  if (outputs)
  {
    int nout = outputs->size();
    writeWithFnutts("$4},$4" + IG_outputs + ": {");
    for (const std::pair<const std::string,FmThreshold>& output : *outputs)
    {
      writeWithFnutts("$6" + output.first + ": {$8type: float,$8unit: -$6}");
      if (--nout > 0) os << ',';
      if (output.second.isActive()) ++nthres;
    }
  }

  writeWithFnutts("$4}$2},$2operators: {");
  writeWithFnutts("$4in: {$6type: input,$6out: [{");
  writeWithFnutts("$8name: " + IG_inputs + ",$8type: " + IG_inputs + "$6}]$4},");
  writeWithFnutts(window_size > 0 ? "$4fmmfedem: {" : "$4visualization: {");
  writeWithFnutts("$6type: python,$6image: fedempy,$6library: ");
  if (window_size > 0)
    writeWithFnutts("fedempy.dts_operators.window");
  else
    writeWithFnutts("fedempy.dts_operators.stress_visualization");
  writeWithFnutts(",$6function: run,");
  writeWithFnutts("$6in: [{$8source: " + IG_inputs + "$6}],$6out: [");
  if (outputs)
  {
    writeWithFnutts("{$8name: " + IG_outputs + ",$8type: " + IG_outputs + ",$8storage: ");
    os <<"true\n      }";
  }
  else if (!window_size)
    writeWithFnutts("{$8name: cug,$8kind: file,$8format: cug$6}");
  if (nthres > 0)
  {
    writeWithFnutts("],$6events: [");
    for (const std::pair<const std::string,FmThreshold>& output : *outputs)
      if (output.second.isActive())
      {
        output.second.writeAppJson(os,output.first,IG_outputs,6);
        if (--nthres > 0) os << ',';
      }
  }
  else if (!window_size)
  {
    writeWithFnutts("],$6events: [{");
    writeWithFnutts("$8severity: {$9 code: C_HIGH$8},");
    writeWithFnutts("$8condition: "); os <<"true,";
    writeWithFnutts("$8code: {$9 code: HIGH$8},");
    writeWithFnutts("$8type: com.sap.newton.vtfx.SituationReplay$6}");
  }
  writeWithFnutts("]$4}$2}\n}\n");

  return true;
}


/*!
  Static helper that creates a zipped archive of specified folder.
*/

static bool make_zip (const std::string& folderPath, bool eraseIt = true,
                      const std::string& suffix = "zip")
{
  // Collect all file names recursively
  Strings fileNames;
  if (!FmFileSys::getFiles(fileNames, folderPath, NULL, true))
    return false;

  // Remove the path making all file names relative to folderPath
  size_t npath = folderPath.size();
  for (std::string& fName : fileNames)
    fName.erase(0,npath+1);

  // Temporarily change working directory such that zip works
  // using relative path names
  char* oldwd = getcwd(NULL,128);
  if (chdir(folderPath.c_str()) < 0)
  {
    perror(folderPath.c_str());
    free(oldwd);
    return false;
  }

  // Create the zip file
  std::string zipFile = folderPath + "."+ suffix;
  bool ok = Fap::make_zip(zipFile, fileNames);
  if (ok)
  {
    ListUI <<"  -> Model exported to "<< zipFile <<" with content:";
    for (const std::string& file : fileNames)
      ListUI <<"\n     "<< file;
    ListUI <<"\n";
  }

  // Clean up
  if (oldwd && chdir(oldwd) < 0)
    perror(oldwd);
  if (ok && eraseIt)
    FmFileSys::removeDir(folderPath);
  free(oldwd);
  return ok;
}


/*!
  Static helper that checks the validity of an app path.
*/

static std::string get_app_path (const std::string& fileName, const std::string& prg)
{
  if (fileName.empty())
  {
    std::cerr <<" *** FapExportCmds::"<< prg <<"(): Empty file name"<< std::endl;
    return fileName;
  }

  std::string appPath = FFaFilePath::getBaseName(fileName);
  FFaFilePath::makeItAbsolute(appPath, FmDB::getMechanismObject()->getAbsModelFilePath());
  if (FpFileSys::verifyDirectory(FFaFilePath::checkName(appPath), false))
  {
    // Directory exists, make sure it is empty
    Strings deadFiles;
    if (FmFileSys::getFiles(deadFiles, appPath, NULL, true))
    {
      std::cerr <<" *** FapExportCmds::"<< prg <<"(): Directory "<< appPath
                <<" already contain the files:";
      for (const std::string& file : deadFiles)
        std::cerr <<"\n     "<< FFaFilePath::getRelativeFilename(appPath,file);
      std::cerr << std::endl;
      appPath.clear();
    }
  }
  else if (!FpFileSys::verifyDirectory(appPath, true))
  {
    std::cerr <<" *** FapExportCmds::"<< prg <<"(): Failed to create directory "
              << appPath << std::endl;
    appPath.clear();
  }

  return appPath;
}


/*!
  static helper removing all frs-output options.
*/

static void remove_all_frs_output (std::string& options,
                                   std::map<FmIsPlottedBase*,BoolVec>& saveVars)
{
  if (!options.empty())
  {
    size_t ipos = options.find("-all");
    while (ipos < options.size())
    {
      size_t ip = ipos;
      while (ip > 0 && isspace(options[ip-1])) --ip;
      options.erase(ip,options.find_first_of(' ',ipos));
      ipos = options.find("-all");
    }
  }

  if (options.empty())
    options.assign("-allPrimaryVars-");
  else if (options.find("-allPrimaryVars-") == std::string::npos)
    options.insert(0,"-allPrimaryVars- ");

  if (options.find("-noBeamForces") == std::string::npos)
    if (FmDB::getObjectCount(FmBeam::getClassTypeID()) > 0)
      options.append(" -noBeamForces");

  // Switch off all output toggles
  std::vector<FmModelMemberBase*> allObjs;
  FmDB::getAllOfType(allObjs,FmIsPlottedBase::getClassTypeID());
  for (FmModelMemberBase* obj : allObjs)
  {
    FmIsPlottedBase* plb = static_cast<FmIsPlottedBase*>(obj);
    if (!plb->mySaveVar.getValue().empty())
    {
      saveVars[plb] = plb->mySaveVar.getValue();
      plb->mySaveVar.setValue({});
    }
  }
}

//------------------------------------------------------------------------------

/*!
  Exports the model to a zipped DTS streaming app.
*/

void FapExportCmds::exportDTSApp(FmModelExpOptions* options)
{
  std::string appPath = get_app_path(options->streamFilename.getValue(), "exportDTSApp");
  if (appPath.empty()) return; // Invalid path

  const std::string& IGinp = options->streamInputIndGroup.getValue();
  const std::string& IGout = options->streamOutputIndGroup.getValue();
  double windowSize = options->streamWindowSize.getValue();
  bool use_state = options->streamTransferState.getValue();

  // Get inputs and outputs
  Strings inputs;
  std::map<std::string,FmThreshold> outputs;
  std::vector<FmEngine*> engines;
  FmDB::getAllEngines(engines);
  for (FmEngine* e : engines)
    if (e->isExternalFunc())
    {
      std::string name = e->getTag();
      if (name.empty())
        name = "dt_input_" + std::to_string(inputs.size()+1);
      inputs.push_back(name);
    }
    else if (e->myOutput.getValue())
    {
      std::string name = e->getTag();
      if (name.empty())
        name = "dt_output_" + std::to_string(outputs.size()+1);
      outputs[name] = e->myThreshold.getValue();
    }

  // Create the fedem configuration file
  std::string confPath = FFaFilePath::appendFileNameToPath(appPath, "config");
  if (!FpFileSys::verifyDirectory(confPath, true)) return;
  std::ofstream os(FFaFilePath::appendFileNameToPath(confPath, "fmmfedem.json").c_str());
  if (!os) return;

  FmMechanism* mech = FmDB::getMechanismObject();
  os <<"{\n  \"fmm_file\": \""<< mech->getModelName(true) <<"\",\n";
  if (!outputs.empty())
  {
    // Store the output function ids
    int n_out = 0;
    os <<"  \"output_ids\": [";
    for (FmEngine* e : engines)
      if (e->myOutput.getValue() && !e->isExternalFunc())
      {
        if (++n_out > 1) os <<", ";
        os << e->getID();
      }
    os <<"],\n";
  }
  os <<"  \"use_state\": "<< (use_state ? "true" : "false") <<"\n}\n";
  os.close();

  // Create the resource configuration file
  if (!write_res_config(FFaFilePath::appendFileNameToPath(appPath, "resource-config.yml")))
  {
    FmFileSys::removeDir(appPath);
    return;
  }

  // Create the app.json file
  std::string appId = FFaLowerCaseString(mech->getModelName()) + std::to_string(time(NULL));
  std::string fName = FFaFilePath::appendFileNameToPath(appPath, "app.json");
  if (!write_app_json(fName, appId, windowSize, IGinp, inputs, IGout, &outputs))
  {
    FmFileSys::removeDir(appPath);
    return;
  }

  std::string libPath = FFaFilePath::appendFileNameToPath(appPath, "lib");
  if (!FpFileSys::verifyDirectory(libPath, true))
    return;

  // Adjust some solver options
  FmAnalysis* analysis = FmDB::getActiveAnalysis();
  bool stopChanged = analysis->stopTimeEnable.setValue(false);
  bool freqChanged = analysis->solveEigenvalues.setValue(false);
  bool aexpChanged = analysis->autoCurveExportSwitch.setValue(false);
  bool uexfChanged = analysis->useExternalFuncFile.setValue(false);
  std::string exff = analysis->externalFuncFileName.getValue();
  bool exffChanged = analysis->externalFuncFileName.setValue("");

  // Switch off all frs-file output
  std::string& addOpts = analysis->solverAddOpts.getValue();
  std::string oldOpts(addOpts);
  std::map<FmIsPlottedBase*,BoolVec> saveVars;
  remove_all_frs_output(addOpts,saveVars);

  // Save model file with dependencies to app folder
  bool ok = FpPM::vpmModelExport(FFaFilePath::appendFileNameToPath(libPath, mech->getModelName(true)));

  // Restore solver options
  analysis->stopTimeEnable.setValue(stopChanged);
  analysis->solveEigenvalues.setValue(freqChanged);
  analysis->autoCurveExportSwitch.setValue(aexpChanged);
  analysis->useExternalFuncFile.setValue(uexfChanged);
  if (exffChanged)
    analysis->externalFuncFileName.setValue(exff);

  if (addOpts != oldOpts) addOpts = oldOpts;

  for (std::pair<FmIsPlottedBase* const,BoolVec>& plb : saveVars)
    plb.first->mySaveVar.setValue(plb.second);

  // Create a zipped archive for the DTS app
  if (ok) make_zip(appPath);
}


/*!
  Exports the model to a zipped DTS batch app.
*/

void FapExportCmds::exportDTSBatchApp(FmModelExpOptions* options)
{
  std::string appPath = get_app_path(options->batchFilename.getValue(), "exportDTSBatchApp");
  if (appPath.empty()) return; // Invalid path

  const std::string& IGinp = options->batchInputIndGroup.getValue();
  bool surface_only = options->batchSurfaceOnly.getValue();
  bool stress_recovery = options->batchStressRecovery.getValue();
  bool all_parts = options->batchAllFEParts.getValue();

  // Get inputs
  Strings inputs;
  std::vector<FmEngine*> engines;
  FmDB::getAllEngines(engines);
  for (FmEngine* e : engines)
    if (e->isExternalFunc())
    {
      std::string name = e->getTag();
      if (name.empty())
        name = "dt_input_" + std::to_string(inputs.size()+1);
      inputs.push_back(name);
    }

  // Create config file with visualization setup
  std::string confPath = FFaFilePath::appendFileNameToPath(appPath, "config");
  if (!FpFileSys::verifyDirectory(confPath, true)) return;
  std::ofstream os(FFaFilePath::appendFileNameToPath(confPath, "visualization.json").c_str());
  if (!os) return;

  FmMechanism* mech = FmDB::getMechanismObject();
  std::string linkDB = mech->getModelName() + "_RDB/link_DB/";
  std::vector<FmPart*> parts;
  FmDB::getAllParts(parts);
  os <<"{\n  \"fmm_file\": \""<< mech->getModelName(true)
     <<"\",\n  \"fe_parts\": [";
  int count = 0;
  bool do_recover = false;
  for (FmPart* part : parts)
    if (part->isFEPart() &&
        ((do_recover = part->recoveryDuringSolve.getValue()%2 > 0) || all_parts))
      os << (++count > 1 ? ", {\n" : "{\n")
         <<"    \"path\": \""<< linkDB << part->getBaseFTLName(true) <<"\",\n"
         <<"    \"name\": \""<< part->getTag() <<"\",\n"
         <<"    \"base_id\": "<< part->getBaseID() <<",\n"
         <<"    \"surface_only\": "<< (surface_only ? "true" : "false") <<",\n"
         <<"    \"recovery\": "<< (stress_recovery && do_recover ? "true" : "false")
         <<"\n  }";
  do_recover = stress_recovery && count > 0;

  os <<"],\n  \"visualization_parts\": [";
  count = 0;
  for (FmPart* part : parts)
    if (all_parts && part->isGenericPart() && !part->visDataFile.getValue().empty())
      os << (++count > 1 ? ", {\n" : "{\n")
         <<"    \"path\": \""<< FFaFilePath::getFileName(part->visDataFile.getValue()) <<"\",\n"
         <<"    \"name\": \""<< part->getTag() <<"\",\n"
         <<"    \"base_id\": "<< part->getBaseID()
         <<"\n  }";
  os <<"]\n}\n";
  os.close();

  // Create the resource configuration file
  if (!write_res_config(FFaFilePath::appendFileNameToPath(appPath, "resource-config.yml")))
  {
    FmFileSys::removeDir(appPath);
    return;
  }

  // Create the app.json file
  std::string appId = FFaLowerCaseString(mech->getModelName()) + std::to_string(time(NULL));
  std::string fName = FFaFilePath::appendFileNameToPath(appPath, "app.json");
  if (!write_app_json(fName, appId, 0.0, IGinp, inputs))
  {
    FmFileSys::removeDir(appPath);
    return;
  }

  std::string libPath = FFaFilePath::appendFileNameToPath(appPath, "lib");
  if (!FpFileSys::verifyDirectory(libPath, true))
    return;

  // Adjust some solver options
  FmAnalysis* analysis = FmDB::getActiveAnalysis();
  bool stopChanged = analysis->stopTimeEnable.setValue(false);
  bool freqChanged = analysis->solveEigenvalues.setValue(false);
  bool aexpChanged = analysis->autoCurveExportSwitch.setValue(false);
  bool uexfChanged = analysis->useExternalFuncFile.setValue(false);
  std::string exff = analysis->externalFuncFileName.getValue();
  bool exffChanged = analysis->externalFuncFileName.setValue("");

  // Switch off all frs-file output
  std::string& addOpts = analysis->solverAddOpts.getValue();
  std::string oldOpts(addOpts);
  std::map<FmIsPlottedBase*,BoolVec> saveVars;
  remove_all_frs_output(addOpts,saveVars);

  if (do_recover)
  {
    // Switch off part deformation output
    size_t ipos = addOpts.find("-partDeformation");
    if (ipos == std::string::npos)
      addOpts.append(" -partDeformation=0");
    else
      addOpts[ipos+17] = '0';

    // Enable von Mises stress output through the state array
    ipos = addOpts.find("-partVMStress");
    if (ipos == std::string::npos)
      addOpts.append(" -partVMStress=2");
    else
      addOpts[ipos+14] = '2';
  }

  // Save model file with dependencies to app folder
  bool ok = FpPM::vpmModelExport(FFaFilePath::appendFileNameToPath(libPath, mech->getModelName(true)));

  // Restore solver options
  analysis->stopTimeEnable.setValue(stopChanged);
  analysis->solveEigenvalues.setValue(freqChanged);
  analysis->autoCurveExportSwitch.setValue(aexpChanged);
  analysis->useExternalFuncFile.setValue(uexfChanged);
  if (exffChanged)
    analysis->externalFuncFileName.setValue(exff);

  if (addOpts != oldOpts) addOpts = oldOpts;

  for (std::pair<FmIsPlottedBase* const,BoolVec>& plb : saveVars)
    plb.first->mySaveVar.setValue(plb.second);

  // Create a zipped archive for the DTS app
  if (ok) make_zip(appPath);
}


/*!
  Exports the model as an FMU simulation app.
*/

void FapExportCmds::exportFMUApp(FmModelExpOptions* options)
{
  std::string appPath = get_app_path(options->fmuFilename.getValue(), "exportFMUApp");
  if (appPath.empty()) return; // Invalid path

  std::string resourcesPath = FFaFilePath::appendFileNameToPath(appPath, "resources");
  if (!FpFileSys::verifyDirectory(resourcesPath, true)) return;
  std::string binariesPath = FFaFilePath::appendFileNameToPath(appPath, "binaries");
  if (!FpFileSys::verifyDirectory(binariesPath, true)) return;
  std::string binariesPathWin = FFaFilePath::appendFileNameToPath(binariesPath, "win64");
  if (!FpFileSys::verifyDirectory(binariesPathWin, true)) return;
  std::string binariesPathLin = FFaFilePath::appendFileNameToPath(binariesPath, "linux64");
  if (!FpFileSys::verifyDirectory(binariesPathLin, true)) return;
  std::string modelPath = FFaFilePath::appendFileNameToPath(resourcesPath, "model.tmp");
  if (!FpFileSys::verifyDirectory(modelPath, true)) return;

  // Copy template files
  FmMechanism* mech = FmDB::getMechanismObject();
  std::string modelIdentifier = mech->getModelName();
  std::string templPath = FpPM::getFullFedemPath("Templates/cloudsim");

  // Copy shared library. Windows
  if (!FpFileSys::copyFile(FFaFilePath::appendFileNameToPath(templPath, "fedem_fmu.dll"),
                           FFaFilePath::appendFileNameToPath(binariesPathWin, modelIdentifier + ".dll")))
  {
    std::cerr <<" *** FapExportCmds::exportFMUApp(): Failed to copy "
              << FFaFilePath::appendFileNameToPath(templPath, "fedem_fmu.dll") << std::endl;
    perror("                                  ");
  }

  // Copy shared library. Linux
  if (!FpFileSys::copyFile(FFaFilePath::appendFileNameToPath(templPath, "libfedem_fmu.so"),
                           FFaFilePath::appendFileNameToPath(binariesPathLin, modelIdentifier + ".so")))
  {
    std::cerr <<" *** FapExportCmds::exportFMUApp(): Failed to copy "
              << FFaFilePath::appendFileNameToPath(templPath, "libfedem_fmu.so") << std::endl;
    perror("                                  ");
  }

  // Collecting info on input and output functions. Vectors inputs and outputs will contain:
  // <name, description, ExternalFuncId/funcId> for each input and output
  bool useExtFile = options->inclExtFuncFile.getValue();
  using Indicator = std::tuple<std::string,std::string,int>;
  std::vector<Indicator> inputs, outputs;
  std::vector<FmEngine*> engines;
  FmDB::getAllEngines(engines);
  for (FmEngine* e : engines)
    if (e->isExternalFunc() && !useExtFile)
    {
      std::string name = e->getTag();
      if (name.empty())
        name = "dt_input_" + std::to_string(inputs.size() + 1);

      FmfExternalFunction* xf = dynamic_cast<FmfExternalFunction*>(e->getFunction());
      inputs.push_back(std::make_tuple(name, e->getItemDescr(), xf->channel.getValue()));
    }
    else if (e->myOutput.getValue())
    {
      std::string name = e->getTag();
      if (name.empty())
        name = "dt_output_" + std::to_string(outputs.size() + 1);

      outputs.push_back(std::make_tuple(name, e->getItemDescr(), e->getID()));
    }

  // Write modelDescription.xml
  std::ofstream os(FFaFilePath::appendFileNameToPath(appPath,"modelDescription.xml"));
  if (!os) return;

  char generateTime[64];
  time_t currentTime = time(NULL);
  strftime(generateTime,64,"%FT%T",localtime(&currentTime));
  std::string id = FpPM::createUuid();

  os <<"<?xml version=\"1.0\"?>\n"
     <<"<fmiModelDescription fmiVersion=\"2.0\" generationDateAndTime=\""<< generateTime
     <<"\" generationTool=\"FEDEM FMU Exporter\" guid=\""<< id
     <<"\" modelName=\""<< modelIdentifier
     <<"\" description=\""<< mech->getUserDescription(64) <<"\">\n";

  os <<"\t<CoSimulation canGetAndSetFMUstate=\"false\" canHandleVariableCommunicationStepSize=\"false\""
     <<" canInterpolateInputs=\"false\" modelIdentifier=\""<< modelIdentifier <<"\"/>\n";
  os <<"\t<LogCategories>\n"
     <<"\t\t<Category name=\"logAll\"/>\n"
     <<"\t</LogCategories>\n";

  int valRef = 0;
  os <<"\t<ModelVariables>\n";
  for (const Indicator& input : inputs)
    os <<"\t\t<ScalarVariable causality=\"input\" description=\""<< std::get<1>(input)
       <<"\" name=\""<< std::get<0>(input)
       <<"\" valueReference=\""<< valRef++ <<"\">\n"
       <<"\t\t\t<Real start=\"0.0\"/>\n"
       <<"\t\t</ScalarVariable>\n";
  for (const Indicator& output : outputs)
    os <<"\t\t<ScalarVariable causality=\"output\" description=\""<< std::get<1>(output)
       <<"\" name=\""<< std::get<0>(output)
       <<"\" valueReference=\""<< valRef++ <<"\">\n"
       <<"\t\t\t<Real/>\n"
       <<"\t\t</ScalarVariable>\n";
  os <<"\t</ModelVariables>\n";

  os <<"\t<ModelStructure>\n";
  valRef = inputs.size()+1;
  os <<"\t\t<Outputs>\n";
  for (size_t i = 0; i < outputs.size(); i++)
    os <<"\t\t\t<Unknown index=\""<< valRef++ <<"\"/>\n";
  os <<"\t\t</Outputs>\n";
  valRef = inputs.size()+1;
  os <<"\t\t<InitialUnknowns>\n";
  for (size_t i = 0; i < outputs.size(); i++)
    os <<"\t\t\t<Unknown index=\""<< valRef++ <<"\"/>\n";
  os <<"\t\t</InitialUnknowns>\n";
  os <<"\t</ModelStructure>\n";

  os <<"</fmiModelDescription>\n";
  os.close();

  // Write the config.txt file.
  // This file will be read at runtime by the FMU to set up model-specific parameters and memory.
  std::ofstream os2(FFaFilePath::appendFileNameToPath(resourcesPath,"config.txt"));
  if (os2) {
    os2 << modelIdentifier << "\n" << id << "\n";
    os2 << inputs.size() + outputs.size() << "\n";
    os2 << inputs.size() << "\n" << outputs.size() << "\n";
    os2 << 0 << "\n"; // numParams. Not currently supported in export
    os2 << 0 << "\n"; // NumTransforms. Not currently supported in export
    for (const Indicator& inp : inputs)  os2 << std::get<2>(inp) <<"\n";
    for (const Indicator& out : outputs) os2 << std::get<2>(out) <<"\n";
    os2.close();
  }

  // Adjust some solver options
  FmAnalysis* analysis = FmDB::getActiveAnalysis();
  bool freqChanged = analysis->solveEigenvalues.setValue(false);
  bool aexpChanged = analysis->autoCurveExportSwitch.setValue(false);
  bool uexfChanged = !useExtFile && analysis->useExternalFuncFile.setValue(false);
  std::string exff = analysis->externalFuncFileName.getValue();
  bool exffChanged = !useExtFile && analysis->externalFuncFileName.setValue("");

  std::string& addOpts = analysis->solverAddOpts.getValue();
  std::string oldOpts(addOpts);
  std::map<FmIsPlottedBase*,BoolVec> saveVars;
  remove_all_frs_output(addOpts,saveVars);

  // Save model file with dependencies to model folder
  // and create solver input files for batch execution
  std::string newModel = FFaFilePath::appendFileNameToPath(modelPath, mech->getModelName(true));
  bool ok = FpPM::vpmModelExport(newModel, analysis, "model");

  // Restore solver options
  analysis->solveEigenvalues.setValue(freqChanged);
  analysis->autoCurveExportSwitch.setValue(aexpChanged);
  if (!useExtFile)
    analysis->useExternalFuncFile.setValue(uexfChanged);
  if (exffChanged)
    analysis->externalFuncFileName.setValue(exff);

  if (addOpts != oldOpts) addOpts = oldOpts;

  for (std::pair<FmIsPlottedBase* const,BoolVec>& plb : saveVars)
    plb.first->mySaveVar.setValue(plb.second);

  // Create a zipped archive for the FMU
  if (ok) make_zip(appPath, true, "fmu");
}

//----------------------------------------------------------------------------

/*!
  Launch dialog to export the model to zipped DTS apps
*/

void FapExportCmds::exportDigitalTwin()
{
  if (!FapLicenseManager::checkLicense("FA-SAP"))
    return;

  Fui::modelExportUI(true, false);
}

//----------------------------------------------------------------------------

/*!
  Export simulation apps.
*/

void FapExportCmds::exportApps()
{
  if (!FapLicenseManager::checkLicense("FA-SAP"))
    return;

  FmModelExpOptions* options = FmDB::getModelExportOptions();
  if (options->streamAppExport.getValue()) exportDTSApp(options);
  if (options->batchAppExport.getValue()) exportDTSBatchApp(options);
  if (options->fmuExport.getValue()) exportFMUApp(options);
}

//----------------------------------------------------------------------------

void FapExportCmds::getExportObjectSensitivity(bool& sensitivity)
{
  sensitivity = !FapLicenseManager::isDemoEdition();
  if (!sensitivity) return; // Not available in the demo edition

  FFaViewItem* selected = FapCmdsBase::findSelectedSingleObject();
  if (dynamic_cast<FmCurveSet*>(selected) || dynamic_cast<FmGraph*>(selected))
    return;

  FmPart* part = dynamic_cast<FmPart*>(selected);
  if (part)
    sensitivity = part->getLinkHandler() != NULL;
  else
    sensitivity = false;
}

//----------------------------------------------------------------------------

void FapExportCmds::getExportLinkSensitivity(bool& sensitivity)
{
  sensitivity = !FapLicenseManager::isDemoEdition();
  if (!sensitivity) return; // Not available in the demo edition

  FmPart* part = dynamic_cast<FmPart*>(FapCmdsBase::findSelectedSingleObject());
  if (part)
    sensitivity = part->getLinkHandler() != NULL;
  else
    sensitivity = false;
}

//----------------------------------------------------------------------------

void FapExportCmds::getExportViewSensitivity(bool& sensitivity)
{
  sensitivity = (FapCmdsBase::getActiveModeller() ||
                 FapCmdsBase::getActiveGraphView());
}

//----------------------------------------------------------------------------

void FapExportCmds::getExportAnimationSensitivity(bool& sensitivity)
{
  sensitivity = !FapLicenseManager::isDemoEdition();
  if (!sensitivity) return; // Not available in the demo edition

  sensitivity = FapAnimationCmds::getCurrentAnimation() != NULL;
}

//----------------------------------------------------------------------------

void FapExportCmds::getExportVTFSensitivity(bool& sensitivity)
{
  sensitivity = FapLicenseManager::checkVTFExportLicense(false);
  if (!sensitivity) return; // Separately licensed feature

  sensitivity = FapAnimationCmds::findSelectedAnimation() != NULL;
}

//----------------------------------------------------------------------------

void FapExportCmds::exportPipeWear()
{
#ifdef FT_HAS_OWL
  FFuFileDialog* aDialog = saveFile("Export Pipe String Wear Data");
  aDialog->addFilter("Wear data files","asc");
  aDialog->addUserField("Start Period",1.0);
  aDialog->addUserField("End Period",1.0);
  aDialog->remember("exportPipeWear");

  Strings dirPath = aDialog->execute();
  double startPeriod = aDialog->getUserFieldValue("Start Period");
  double endPeriod = aDialog->getUserFieldValue("End Period");
  if (startPeriod < 1.0) startPeriod = 1.0;
  if (endPeriod < startPeriod) endPeriod = startPeriod;
  delete aDialog;

  if (!dirPath.empty())
    FapOilWellCmds::exportPipeWearData(dirPath.front(),startPeriod,endPeriod);
#endif
}
