// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapDBCreateCmds.H"
#include "vpmApp/vpmAppUAMap/FapUAItemsListView.H"
#include "vpmApp/FapEventManager.H"
#include "vpmApp/FapLicenseManager.H"
#ifdef USE_INVENTOR
#include "vpmDisplay/FdDB.H"
#include "FFaLib/FFaAlgebra/FFaMath.H"
#endif
#include "vpmPM/FpPM.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFaLib/FFaDefinitions/FFaListViewItem.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFuLib/FFuFileDialog.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmRingStart.H"
#include "vpmDB/FmAnimation.H"
#include "vpmDB/FmGraph.H"
#include "vpmDB/FmCurveSet.H"
#include "vpmDB/FmBearingFriction.H"
#include "vpmDB/FmPrismaticFriction.H"
#include "vpmDB/FmCamFriction.H"
#ifdef FT_HAS_EXTCTRL
#include "vpmDB/FmExternalCtrlSys.H"
#endif
#include "vpmDB/FmGenericDBObject.H"
#include "vpmDB/FmFileReference.H"
#include "vpmDB/FmVesselMotion.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmSubAssembly.H"
#include "vpmDB/FmTurbine.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmBeam.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmBeamProperty.H"
#include "vpmDB/FmMaterialProperty.H"
#include "vpmDB/FmUserDefinedElement.H"
#include "vpmDB/FmElementGroupProxy.H"
#include "vpmDB/FmRefPlane.H"
#include "vpmDB/FmCreate.H"

#include "vpmUI/Fui.H"
#include "vpmUI/vpmUITopLevels/FuiMainWindow.H"
#include "vpmUI/vpmUITopLevels/FuiModelManager.H"
#include "vpmUI/vpmUITopLevels/FuiMooringLine.H"
#include "vpmUI/vpmUIComponents/FuiItemsListViews.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"

#include "FiUserElmPlugin/FiUserElmPlugin.H"
#include "FiDeviceFunctions/FiDeviceFunctionFactory.H"

#ifdef FT_HAS_WND
#include "assemblyCreators/turbineConverter.H"
#endif


//----------------------------------------------------------------------------

void FapDBCreateCmds::init()
{
  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_animation");
  cmdItem->setSmallIcon(animation_xpm);
  cmdItem->setText("New Animation");
  cmdItem->setToolTip("Create a new Animation");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createAnimation));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelTouchable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_graph");
  cmdItem->setSmallIcon(graph_xpm);
  cmdItem->setText("New Graph");
  cmdItem->setToolTip("Create a new Graph");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createGraph));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelTouchable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_graphGroup");
  cmdItem->setSmallIcon(graphGroup_xpm);
  cmdItem->setText("New Graph group");
  cmdItem->setToolTip("Create a new Graph group");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createGraphGroup));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelTouchable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_beamDiagram");
  cmdItem->setSmallIcon(graph_xpm);
  cmdItem->setText("New Beam diagram");
  cmdItem->setToolTip("Create a new Beam diagram");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createBeamDiagram));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelTouchable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_curve");
  cmdItem->setSmallIcon(curve_xpm);
  cmdItem->setText("New Curve");
  cmdItem->setToolTip("Create a new Curve");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createCurve));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelTouchable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_importedGraph");
  cmdItem->setSmallIcon(open_xpm);
  cmdItem->setText("Import Graphs");
  cmdItem->setToolTip("Create Graphs by importing curve data from file");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createImportedGraphs));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelTouchable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_importedCurve");
  cmdItem->setSmallIcon(open_xpm);
  cmdItem->setText("Import Curves");
  cmdItem->setToolTip("Create Curves by importing curve data from file");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createImportedCurves));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelTouchable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_beamForceGraph");
  cmdItem->setSmallIcon(graph_xpm);
  cmdItem->setText("Create Beam Force Graph");
  cmdItem->setToolTip("Create a Graph plotting Beam sectional forces");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createBeamForceGraph));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelTouchable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_file_reference");
  cmdItem->setSmallIcon(fileref_xpm);
  cmdItem->setText("File Reference...");
  cmdItem->setToolTip("Create a File Reference");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createFileReferenceObject));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_VesselMotion");
  cmdItem->setSmallIcon(vesselMotion_xpm);
  cmdItem->setText("Response Amplitude Operator");
  cmdItem->setToolTip("Create a Response Amplitude Operator");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createVesselMotionObject));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

#ifdef FT_HAS_EXTCTRL
  cmdItem = new FFuaCmdItem("cmdId_dBCreate_external_ctrl_sys");
  cmdItem->setSmallIcon(extCtrlSys_xpm);
  cmdItem->setText("External Control System");
  cmdItem->setToolTip("Create an External Control System");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createExternalCtrlSys));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));
#endif

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_GenericDBObject");
  cmdItem->setSmallIcon(generic_xpm);
  cmdItem->setText("Generic DB Object");
  cmdItem->setToolTip("Create a Generic DB Object");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createGenericDBObject));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_generic_part");
  cmdItem->setSmallIcon(createGenericPart_xpm);
  cmdItem->setText("Generic Part");
  cmdItem->setToolTip("Create a Generic Part connected to the selected Triads");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createGenericPart));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_beams");
  cmdItem->setSmallIcon(beam_xpm);
  cmdItem->setText("Beam");
  cmdItem->setToolTip("Create a Beam or Beams between the selected Triads");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createBeams));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_mooring");
  cmdItem->setSmallIcon(beam_xpm);
  cmdItem->setText("Mooring Line...");
  cmdItem->setToolTip("Create a Mooring line of beam elements between the selected Triads");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createMooringLine));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_RotFriction");
  cmdItem->setSmallIcon(gearFriction_xpm);
  cmdItem->setText("Rotational Friction");
  cmdItem->setToolTip("Create a general rotational Friction");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createRotFriction));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_TransFriction");
  cmdItem->setSmallIcon(camJointFriction_xpm);
  cmdItem->setText("Translational Friction");
  cmdItem->setToolTip("Create a general translational Friction");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createTransFriction));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_BearingFriction");
  cmdItem->setSmallIcon(revJointFriction_xpm);
  cmdItem->setText("Bearing Friction");
  cmdItem->setToolTip("Create a Bearing Friction");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createBearingFriction));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_PrismFriction");
  cmdItem->setSmallIcon(prismJointFriction_xpm);
  cmdItem->setText("Prismatic Friction");
  cmdItem->setToolTip("Create a Prismatic Friction");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createPrismaticFriction));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_CamFriction");
  cmdItem->setSmallIcon(camJointFriction_xpm);
  cmdItem->setText("Cam Friction");
  cmdItem->setToolTip("Create a Cam Friction");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createCamFriction));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_MaterialProperty");
  cmdItem->setSmallIcon(material_xpm);
  cmdItem->setText("Material Property");
  cmdItem->setToolTip("Create a Material Property");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createMaterialProperty));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_BeamProperty");
  cmdItem->setSmallIcon(beamProp_xpm);
  cmdItem->setText("Beam Cross Section");
  cmdItem->setToolTip("Create a Beam Cross Section");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createBeamProperty));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_dBCreate_Assembly");
  cmdItem->setText("Subassembly...");
  cmdItem->setToolTip("Create a Subassembly populated with the selected objects");
  cmdItem->setActivatedCB(FFaDynCB0S(FapDBCreateCmds::createAssembly));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));

  for (int i = 0; i < 10; i++)
  {
    cmdItem = new FFuaCmdItem("cmdId_dBCreate_UE" + std::to_string(i));
    cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelEditable,bool&));
    switch (i) {
    case 0: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapDBCreateCmds::createUserElm(0); })); break;
    case 1: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapDBCreateCmds::createUserElm(1); })); break;
    case 2: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapDBCreateCmds::createUserElm(2); })); break;
    case 3: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapDBCreateCmds::createUserElm(3); })); break;
    case 4: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapDBCreateCmds::createUserElm(4); })); break;
    case 5: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapDBCreateCmds::createUserElm(5); })); break;
    case 6: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapDBCreateCmds::createUserElm(6); })); break;
    case 7: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapDBCreateCmds::createUserElm(7); })); break;
    case 8: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapDBCreateCmds::createUserElm(8); })); break;
    case 9: cmdItem->setActivatedCB(FFaDynCB0S([](){ FapDBCreateCmds::createUserElm(9); })); break;
    }
    // The text and tooltip with element names must be set later,
    // after the user-defined element plugin has been loaded
  }
}
//----------------------------------------------------------------------------

static void selectResultItem(FFaViewItem* item)
{
  // Selecting the newly created item
  FapEventManager::permTotalSelect(item);

  // Ensuring it is visible in the Results manager view
  FuiResultListView* rlv = Fui::getMainWindow()->getModelManager()->getResultManager();
  static_cast<FapUAItemsListView*>(rlv->getUA())->ensureItemVisible(item);
}
//----------------------------------------------------------------------------

FmBase* FapDBCreateCmds::getSelectedAssembly(int classType)
{
  std::vector<FFaListViewItem*> permSelection;
  FFaListViewItem* selection;
  FapEventManager::getLVSelection(permSelection,selection);
  if (!selection && !permSelection.empty())
    selection = permSelection.front();

  FmBase* subass = dynamic_cast<FmSubAssembly*>(selection);
  if (!subass)
  {
    FmModelMemberBase* mmb = dynamic_cast<FmModelMemberBase*>(selection);
    if (mmb)
    {
      if (classType < 0 || mmb->isOfType(classType))
        subass = mmb->getParentAssembly();
      else
      {
        FmRingStart* rs = dynamic_cast<FmRingStart*>(selection);
        if (rs && rs->getRingMemberType() == classType)
          subass = rs->getParentAssembly();
      }
    }
  }

  if (subass && classType < 0) // We don't want the graph groups here...
    if (static_cast<FmSubAssembly*>(subass)->hasObjects(FmResultBase::getClassTypeID()))
      return NULL;

  return subass;
}
//----------------------------------------------------------------------------

static FmGraph* getGraphOfSelection()
{
  std::vector<FFaListViewItem*> permSelection;
  FFaListViewItem* selection;
  FapEventManager::getLVSelection(permSelection,selection);
  if (!selection && !permSelection.empty())
    selection = permSelection.front();

  FmGraph* graph = dynamic_cast<FmGraph*>(selection);
  if (graph) return graph; // graph selected

  FmCurveSet* curve = dynamic_cast<FmCurveSet*>(selection);
  if (curve) return curve->getOwnerGraph(); // curve selected

  FmBase* subass = dynamic_cast<FmSubAssembly*>(selection);
  if (!subass) {
    FmRingStart* rs = dynamic_cast<FmRingStart*>(selection);
    if (rs && rs->getRingMemberType() == FmGraph::getClassTypeID())
      subass = rs->getParentAssembly(); // subassembly selected
  }

  graph = new FmGraph(); // create a new graph
  graph->setParentAssembly(subass);
  graph->connect();

  return graph;
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createAnimation()
{
  FpPM::vpmSetUndoPoint("Animation");

  FmAnimation* animation = new FmAnimation();
  animation->setParentAssembly(getSelectedAssembly(FmAnimation::getClassTypeID()));
  animation->connect();

  selectResultItem(animation);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createGraph()
{
  FpPM::vpmSetUndoPoint("Graph");

  FmGraph* graph = new FmGraph();
  graph->setParentAssembly(getSelectedAssembly(FmGraph::getClassTypeID()));
  graph->connect();

  selectResultItem(graph);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createGraphGroup()
{
  FpPM::vpmSetUndoPoint("Graph group");

  FmSubAssembly* subAss = new FmSubAssembly();
  subAss->setUserDescription("New Graph group");
  subAss->connect();

  FmGraph* graph = new FmGraph();
  graph->setParentAssembly(subAss);
  graph->connect();

  selectResultItem(subAss);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createBeamDiagram()
{
  FpPM::vpmSetUndoPoint("Beam diagram");

  FmGraph* graph = new FmGraph(true);
  graph->setParentAssembly(getSelectedAssembly(FmGraph::getClassTypeID()));
  graph->connect();

  selectResultItem(graph);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createCurve()
{
  FpPM::vpmSetUndoPoint("Curve");

  FmCurveSet* curve;
  FmGraph* graph = getGraphOfSelection();

  if (graph->isBeamDiagram())
    curve = new FmCurveSet(FmCurveSet::SPATIAL_RESULT);
  else
    curve = new FmCurveSet(FmCurveSet::TEMPORAL_RESULT);

  curve->setColor(graph->getCurveDefaultColor());
  graph->addCurveSet(curve);

  selectResultItem(curve);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createImportedGraphs()
{
  std::vector<std::string> ascExts = FmGraph::asc();
  std::vector<std::string> rpcExts = FmGraph::rpc();
  std::vector<std::string> allExts = ascExts;
  allExts.insert(allExts.end(), rpcExts.begin(), rpcExts.end());

  const std::string& absModFilePath = FmDB::getMechanismObject()->getAbsModelFilePath();
  FFuFileDialog* dialog = FFuFileDialog::create(absModFilePath, "Dialog",
                                                FFuFileDialog::FFU_OPEN_FILES,
                                                true);
  dialog->setTitle("Import Graphs");
  dialog->addFilter("Multi Channel Files", allExts);
  dialog->addFilter("MTS RPC Time History File", rpcExts);
  dialog->addFilter("ASCII file", ascExts);
  dialog->addUserToggle("relToggle", "Use path relative to model-file location", true);
  dialog->remember("GraphCurve");

  std::vector<std::string> files = dialog->execute();
  bool relative = dialog->getUserToggleSet("relToggle");
  delete dialog;

  if (files.empty()) return;

  FpPM::vpmSetUndoPoint("Imported graphs");

  FmBase* subass = getSelectedAssembly(FmGraph::getClassTypeID());
  FmGraph* graph = NULL;
  std::vector<std::string> errorFiles;

  // Loop over selected curve data files
  for (const std::string& file : files)
  {
    std::vector<std::string> channels;
    if (!FiDeviceFunctionFactory::getChannelList(file,channels)) {
      errorFiles.push_back(file);
      continue;
    }

    std::string gDesc = FFaFilePath::getFileName(file);
    std::string fName = relative ? FFaFilePath::getRelativeFilename(absModFilePath,file) : file;

    graph = new FmGraph();
    graph->setUserDescription(gDesc);
    graph->setParentAssembly(subass);
    graph->connect();

    // Loop over channels
    for (const std::string& channel : channels)
    {
      FmCurveSet* curve = new FmCurveSet(FmCurveSet::EXT_CURVE);
      curve->setUserDescription(gDesc + " - " + channel);
      curve->setColor(graph->getCurveDefaultColor());
      curve->setFilePath(fName);
      curve->setChannelName(channel);
      graph->addCurveSet(curve);
    }
  }

  // Selecting the last created graph
  selectResultItem(graph);

  if (errorFiles.empty()) return;

  std::string msg("Could not read channel list from the following files:\n");
  for (const std::string& file : errorFiles) msg += file + "\n";
  FFaMsg::dialog(msg,FFaMsg::WARNING);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createImportedCurves()
{
  std::vector<std::string> ascExts = FmGraph::asc();
  std::vector<std::string> dacExts = FmGraph::dac();
  std::vector<std::string> rpcExts = FmGraph::rpc();
  std::vector<std::string> allExts = ascExts;
  allExts.insert(allExts.end(), rpcExts.begin(), rpcExts.end());
  allExts.insert(allExts.end(), dacExts.begin(), dacExts.end());

  const std::string& absModFilePath = FmDB::getMechanismObject()->getAbsModelFilePath();
  FFuFileDialog* dialog = FFuFileDialog::create(absModFilePath, "Dialog",
                                                FFuFileDialog::FFU_OPEN_FILES,
                                                true);
  dialog->setTitle("Import curve(s)");
  dialog->addFilter("All Curve Files", allExts);
  dialog->addFilter("ASCII File", ascExts);
  dialog->addFilter("MTS RPC Time History File", rpcExts);
  dialog->addFilter("nCode DAC File", dacExts);
  dialog->addAllFilesFilter(true);
  dialog->addUserToggle("relToggle", "Use path relative to model-file location", true);
  dialog->remember("GraphCurve");

  std::vector<std::string> files = dialog->execute();
  bool relative = dialog->getUserToggleSet("relToggle");
  delete dialog;

  if (files.empty()) return;

  FpPM::vpmSetUndoPoint("Imported curves");

  FmGraph* graph = getGraphOfSelection();
  FmCurveSet* curve = NULL;

  // Loop over selected curve definition files
  for (const std::string& file : files)
  {
    curve = new FmCurveSet(FmCurveSet::EXT_CURVE);
    curve->setUserDescription(FFaFilePath::getFileName(file));
    curve->setColor(graph->getCurveDefaultColor());
    curve->setFilePath(relative ? FFaFilePath::getRelativeFilename(absModFilePath,file) : file);
    graph->addCurveSet(curve);
  }

  // Selecting the last created curve
  selectResultItem(curve);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createBeamForceGraph()
{
#ifdef FT_HAS_WND
  std::vector<FmModelMemberBase*> selection;
  if (!FapCmdsBase::getCurrentSelection(selection))
    return;

  FpPM::vpmSetUndoPoint("Beam force graph");

  for (FmModelMemberBase* obj : selection)
    if (obj->isOfType(FmTriad::getClassTypeID()))
      FWP::createTriadForceGraph(obj);
    else if (obj->isOfType(FmBeam::getClassTypeID()))
    {
      static const char* buttons[3] = { "End 1", "End 2", "Cancel" };
      int iend = FFaMsg::dialog("Choose either End 1 or End 2 for " +
                                obj->getIdString(true),FFaMsg::GENERIC,buttons);
      if (iend < 2)
        FWP::createBeamForceGraph(obj,iend+1);
    }
#endif
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createFileReferenceObject()
{
  const std::string& absModFilePath = FmDB::getMechanismObject()->getAbsModelFilePath();
  FFuFileDialog* dialog = FFuFileDialog::create(absModFilePath, "Dialog",
                                                FFuFileDialog::FFU_OPEN_FILES,
                                                true);
  dialog->setTitle("Create file reference");
  for (const FmFileRefExt& filter : FmFileReference::getExtensions())
    dialog->addFilter(filter.first,filter.second);
  dialog->addAllFilesFilter(true);
  dialog->addUserToggle("relToggle", "Use path relative to model-file location", true);
  dialog->remember("FileRefBrowseField");

  std::vector<std::string> files = dialog->execute();
  bool relative = dialog->getUserToggleSet("relToggle");
  delete dialog;

  if (files.empty()) return;

  FpPM::vpmSetUndoPoint("File reference object");

  FmBase* subass = getSelectedAssembly();
  FmFileReference* obj = NULL;
  for (const std::string& file : files)
  {
    obj = new FmFileReference();
    obj->setUserDescription(FFaFilePath::getFileName(file));
    obj->fileName.setValue(relative ? FFaFilePath::getRelativeFilename(absModFilePath,file) : file);
    obj->setParentAssembly(subass);
    obj->connect();
  }

  // Selecting the last created file reference
  FapEventManager::permTotalSelect(obj);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createVesselMotionObject()
{
  FpPM::vpmSetUndoPoint("Response amplitude operator");

  FmVesselMotion* obj = new FmVesselMotion();
  obj->setParentAssembly(getSelectedAssembly());
  obj->connect();
  obj->createMotions();

  FapEventManager::permTotalSelect(obj);
}
//----------------------------------------------------------------------------

#ifdef FT_HAS_EXTCTRL
void FapDBCreateCmds::createExternalCtrlSys()
{
  if (!FapLicenseManager::hasExtCtrlLicense()) return;

  FpPM::vpmSetUndoPoint("External control system");

  FmExternalCtrlSys* obj = new FmExternalCtrlSys();
  obj->setParentAssembly(getSelectedAssembly());
  obj->connect();

  FapEventManager::permTotalSelect(obj);
}
//----------------------------------------------------------------------------
#endif

void FapDBCreateCmds::createGenericDBObject()
{
  FpPM::vpmSetUndoPoint("Generic DB object");

  FmGenericDBObject* obj = new FmGenericDBObject();
  obj->setParentAssembly(getSelectedAssembly());
  obj->connect();

  FapEventManager::permTotalSelect(obj);
}
//----------------------------------------------------------------------------

static int getSelectedTriads(std::vector<FmTriad*>& triads, FmBase*& parent)
{
  FmTriad* triad = NULL;
  for (FmModelMemberBase* selection : FapEventManager::getPermMMBSelection())
    if ((triad = dynamic_cast<FmTriad*>(selection)))
    {
      if (triads.empty())
        parent = triad->getParentAssembly();
      else if (triad->getParentAssembly() != parent)
        parent = NULL;
      triads.push_back(triad);
    }

  FapEventManager::permUnselectAll();
  return triads.size();
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createGenericPart()
{
  FmBase* parent = NULL;
  std::vector<FmTriad*> triads;
  if (getSelectedTriads(triads,parent) < 1) {
    FFaMsg::dialog("Please select the triads to attach to the generic part first.",
                   FFaMsg::OK);
    return;
  }

  FpPM::vpmSetUndoPoint("Generic part");

  FmPart* genPart = new FmPart();
  genPart->setParentAssembly(parent);
  genPart->connect();
  genPart->useGenericProperties.setValue(true);

  // Connect the selected triads

  FaVec3 cg;
  for (FmTriad* triad : triads)
  {
    // Use connect instead of FmPart::attach since we now allow triads to be
    // connected to more than one part (attach only allows single-connection)
    triad->connect(genPart);
    cg += triad->getGlobalTranslation();
  }
  cg /= triads.size();
  // Must refer the CoG position to origin of parent assembly
  FmAssemblyBase* pAss = dynamic_cast<FmAssemblyBase*>(parent);
  genPart->setPositionCG(pAss ? pAss->toLocal(cg) : cg);

  genPart->draw();

  FapEventManager::permTotalSelect(genPart);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createBeams()
{
  FmBase* parent = NULL;
  std::vector<FmTriad*> triads;
  if (getSelectedTriads(triads,parent) < 2) {
    FFaMsg::dialog("Please select two (or more) triads to attach to the beam(s) first.",
                   FFaMsg::OK);
    return;
  }

  FpPM::vpmSetUndoPoint("Beams");

  FmBeam* beam = NULL;
  for (size_t i = 1; i < triads.size(); i++) {
    beam = new FmBeam();
    beam->setParentAssembly(parent);
    beam->connect(triads[i-1],triads[i]);
    beam->draw();
    triads[i-1]->draw();
  }
  triads.back()->draw();

  FapEventManager::permTotalSelect(beam);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createUserElm(int typeIdx)
{
  int eTypes[10];
  int nTypes = FiUserElmPlugin::instance()->getElementTypes(10,eTypes);
  if (nTypes < typeIdx-1) return;

  char typeName[64];
  int nelnod = FiUserElmPlugin::instance()->getTypeName(eTypes[typeIdx],64,typeName);
  if (nelnod < 1) return;

  FmBase* parent = NULL;
  std::vector<FmTriad*> triads;
  if (getSelectedTriads(triads,parent) < nelnod) {
    std::string msg("Please select ");
    if (nelnod == 1)
      msg += "a triad (or a set of triads) ";
    else if (nelnod == 2)
      msg += "two (or more) triads ";
    else
      msg += FFaNumStr("%d triads ",nelnod);
    msg += "to attach to the " + std::string(typeName) + " first.";
    FFaMsg::dialog(msg,FFaMsg::OK);
    return;
  }

  FpPM::vpmSetUndoPoint("Userdefined element");

  FmUserDefinedElement* uelm = NULL;
  for (size_t i = 0; i+nelnod-1 < triads.size(); i++) {
    uelm = new FmUserDefinedElement();
    uelm->setParentAssembly(parent);
    uelm->connect();
    uelm->init(eTypes[typeIdx],typeName,
               std::vector<FmTriad*>(triads.begin()+i,
                                     triads.begin()+i+nelnod));
    uelm->draw();
    triads[i]->draw();
    if (nelnod > 2) break;
  }
  if (nelnod == 2)
    triads.back()->draw();
  else if (nelnod > 2)
    for (int i = 1; i < nelnod; i++)
      triads[i]->draw();

  FapEventManager::permTotalSelect(uelm);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createMooringLine()
{
  FmBase* parent = NULL;
  std::vector<FmTriad*> triads;
  if (getSelectedTriads(triads,parent) != 2) {
    FFaMsg::dialog("Please select two triads to use as the end points first.",FFaMsg::OK);
    return;
  }

  // Launch dialog for selecting the element type, number of segments and total length
  FFuTopLevelShell* uicl = FFuTopLevelShell::getInstanceByType(FuiMooringLine::getClassTypeID());
  FuiMooringLine* dialog = uicl ? dynamic_cast<FuiMooringLine*>(uicl) : FuiMooringLine::create();
  dialog->manage(true,true);
  dialog->addElmType("Beam",true);

  // Include also all two-noded user-defined element types as possible choice
  char typeName[64];
  int i, j, eTypes[10];
  int nTypes = FiUserElmPlugin::instance()->getElementTypes(10,eTypes);
  for (i = j = 0; i < nTypes; i++)
    if (FiUserElmPlugin::instance()->getTypeName(eTypes[i],64,typeName) == 2)
    {
      dialog->addElmType(typeName);
      if (i > j) eTypes[j] = eTypes[i];
      j++;
    }

  dialog->addElmType("Generic Part");
  if (!dialog->execute()) return; // Cancelled

  // Determine which element type to use (Beam is default)
  int elmType = dialog->getElmType();
  if (elmType > j)
    elmType = -1; // use generic part
  else if (elmType > 0)
    elmType = eTypes[elmType-1];

  // Generate the mooring line elements and associated Triads
  Fedem::createMooringLine(triads.front(),triads.back(),
                           dialog->getLength(),dialog->getNumSeg(),elmType);
}

//----------------------------------------------------------------------------

void FapDBCreateCmds::createRotFriction()
{
  FpPM::vpmSetUndoPoint("Rotational friction");

  FmRotFriction* obj = new FmRotFriction();
  obj->setParentAssembly(getSelectedAssembly());
  obj->connect();

  FapEventManager::permTotalSelect(obj);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createTransFriction()
{
  FpPM::vpmSetUndoPoint("Transitional friction");

  FmTransFriction* obj = new FmTransFriction();
  obj->setParentAssembly(getSelectedAssembly());
  obj->connect();

  FapEventManager::permTotalSelect(obj);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createBearingFriction()
{
  FpPM::vpmSetUndoPoint("Bearing friction");

  FmBearingFriction* obj = new FmBearingFriction();
  obj->setParentAssembly(getSelectedAssembly());
  obj->connect();

  FapEventManager::permTotalSelect(obj);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createPrismaticFriction()
{
  FpPM::vpmSetUndoPoint("Prismatic friction");

  FmPrismaticFriction* obj = new FmPrismaticFriction();
  obj->setParentAssembly(getSelectedAssembly());
  obj->connect();

  FapEventManager::permTotalSelect(obj);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createCamFriction()
{
  FpPM::vpmSetUndoPoint("Cam friction");

  FmCamFriction* obj = new FmCamFriction();
  obj->setParentAssembly(getSelectedAssembly());
  obj->connect();

  FapEventManager::permTotalSelect(obj);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createMaterialProperty()
{
  FpPM::vpmSetUndoPoint("Material property");

  FmMaterialProperty* mp = new FmMaterialProperty();
  mp->setParentAssembly(getSelectedAssembly());
  mp->connect();

  FapEventManager::permTotalSelect(mp);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createBeamProperty()
{
  FpPM::vpmSetUndoPoint("Beam cross section");

  FmBeamProperty* bp = new FmBeamProperty();
  bp->setParentAssembly(getSelectedAssembly());
  bp->connect();

  FapEventManager::permTotalSelect(bp);
}
//----------------------------------------------------------------------------

void FapDBCreateCmds::createAssembly()
{
  std::vector<FmModelMemberBase*> selection;
  FapCmdsBase::getCurrentSelection(selection,true);

  // Filter out element groups, if any.
  // They shall automatically follow the Part they are associated with.
  // Also filter out Reference planes, should exist on top level only.
  for (size_t i = 0; i < selection.size();)
    if (selection[i]->isOfType(FmElementGroupProxy::getClassTypeID()) ||
        selection[i]->isOfType(FmRefPlane::getClassTypeID()))
      selection.erase(selection.begin()+i);
    else
      i++;

  FmBase* parent = NULL;
  if (selection.size() == 1 &&
      selection.front()->isOfType(FmSubAssembly::getClassTypeID()))
  {
    const char* buttons[4] = { "A", "B", "Cancel", NULL };
    std::string msg("Do you want to:\n");
    msg.append("\nA) Create a new and empty Subassembly with\n     " +
	       selection.front()->getIdString(true) + " as parent, or\n");
    msg.append("\nB) Move " + selection.front()->getIdString(true) +
               " into a new Subassembly.");
    switch (FFaMsg::dialog(msg,FFaMsg::GENERIC,buttons)) {
    case 0: // option A is chosen
      parent = selection.front();
      selection.clear();
      break;
    case 1: // option B is chosen
      break;
    default: // cancelled operation
      FapEventManager::permUnselectAll();
      return;
    }
  }
  else if (!selection.empty())
  {
    std::string msg("The following object(s) will be moved to a new Subassembly:");
    for (FmModelMemberBase* obj : selection)
      msg.append("\n\t" + obj->getIdString(true));
    msg.append("\n\nContinue ?");
    if (!FFaMsg::dialog(msg,FFaMsg::OK_CANCEL))
    {
      FapEventManager::permUnselectAll();
      return;
    }
  }

  FpPM::vpmSetUndoPoint("Subassembly");

  FmSubAssembly* subAss = Fedem::createSubAssembly(selection,parent);
  subAss->setUserDescription("New Assembly");

  FapEventManager::permUnselectAll();
  FapEventManager::permTotalSelect(subAss);
}
//----------------------------------------------------------------------------

bool FapDBCreateCmds::readBladeDesign(const std::string& bdFile,
                                      FmBladeDesign*& currentDesign)
{
#ifdef FT_HAS_WND
  FmBladeDesign* pDesign = FWP::readBladeDesign(bdFile,currentDesign);
  if (pDesign)
  {
    currentDesign = pDesign;
    return true;
  }
#else
  // Dummy statement to suppress compiler warning
  if (bdFile.empty() || !currentDesign) return false;
#endif
  return false;
}
//----------------------------------------------------------------------------

bool FapDBCreateCmds::updateWindTurbine(bool haveTurbine)
{
  bool status = false;
#ifdef FT_HAS_WND
  Fui::noUserInputPlease();
  FFaMsg::pushStatus("Updating wind turbine model");

  if ((status = FWP::updateTurbine()))
    FmDB::getTurbineObject()->draw();

  if (!haveTurbine && status)
  {
#ifdef USE_INVENTOR
    FdDB::XZnYpZView(); // front view
    FdDB::RotateView(0.0,-M_PI/12.0); // rotate right (one tick)
    FdDB::zoomAll(); // zoom to all
#endif
  }

  FFaMsg::popStatus();
  Fui::okToGetUserInput();
#else
  // Dummy statement to suppress compiler warning
  if (haveTurbine) status = false;
#endif
  return status;
}
//----------------------------------------------------------------------------

bool FapDBCreateCmds::updateWindTurbineTower()
{
  bool status = false;
#ifdef FT_HAS_WND
  Fui::noUserInputPlease();
  FFaMsg::pushStatus("Updating wind turbine model");

  if ((status = FWP::updateTurbineTower()))
    FmDB::getTurbineObject()->draw();

  FFaMsg::popStatus();
  Fui::okToGetUserInput();
#endif
  return status;
}
