// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapEditCmds.H"
#include "vpmApp/vpmAppProcess/FapSimEventHandler.H"
#include "vpmApp/FapEventManager.H"
#include "vpmApp/FapLicenseManager.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFuLib/FFuAuxClasses/FFuaIdentifiers.H"
#include "FFuLib/FFuListViewItem.H"
#include "FFuLib/FFuListView.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "vpmPM/FpRDBExtractorManager.H"
#include "vpmPM/FpPM.H"

#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "vpmUI/vpmUITopLevels/FuiModeller.H"
#include "vpmUI/vpmUITopLevels/FuiCtrlModeller.H"
#include "vpmUI/vpmUITopLevels/FuiSplitBeam.H"
#include "vpmUI/vpmUIComponents/FuiTopologyView.H"
#include "vpmUI/FuiModes.H"
#include "vpmUI/FuiCtrlModes.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmBeam.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmCamJoint.H"
#include "vpmDB/FmElementGroupProxy.H"
#include "vpmDB/FmRefPlane.H"
#include "vpmDB/FmCtrlLine.H"
#include "vpmDB/FmCtrlElementBase.H"
#ifdef FT_HAS_EXTCTRL
#include "vpmDB/FmExternalCtrlSys.H"
#endif
#include "vpmDB/FmSensorBase.H"
#include "vpmDB/FmCurveSet.H"
#include "vpmDB/FmGraph.H"
#include "vpmDB/FmRingStart.H"
#include "vpmDB/FmStrainRosette.H"
#include "vpmDB/FmSimulationEvent.H"
#include "vpmDB/FmSubAssembly.H"


FmModelMemberBase* FapEditCmds::objBeingErased = NULL;

//----------------------------------------------------------------------------

void FapEditCmds::init()
{
  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_edit_undo");
  cmdItem->setSmallIcon(undo_xpm);
  cmdItem->setText("Undo");
  cmdItem->setToolTip("Undo");
  cmdItem->setAccelKey(FFuaKeyCode::CtrlAccel+FFuaKeyCode::Z);
  cmdItem->setActivatedCB(FFaDynCB0S(FpPM::vpmUndo));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FpPM::vpmGetUndoSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_edit_erase");
  cmdItem->setSmallIcon(erase_xpm);
  cmdItem->setText("Delete");
  cmdItem->setToolTip("Delete");
  cmdItem->setAccelKey(FFuaKeyCode::Delete);
  cmdItem->setActivatedCB(FFaDynCB0S(FapEditCmds::erase));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapEditCmds::getEraseSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_edit_eraseStickers");
  cmdItem->setSmallIcon(eraseStickers_xpm);
  cmdItem->setText("Delete All Stickers");
  cmdItem->setToolTip("Delete all stickers");
  cmdItem->setActivatedCB(FFaDynCB0S(FmDB::eraseAllStickers));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModelTouchable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_edit_copyObject");
  cmdItem->setText("Copy Object");
  cmdItem->setToolTip("Make a copy of a property object");
  cmdItem->setActivatedCB(FFaDynCB0S(FapEditCmds::copyObject));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapEditCmds::getCopyObjectSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_edit_copyLink");
  cmdItem->setSmallIcon(duplicateLink_xpm);
  cmdItem->setText("Copy Part");
  cmdItem->setToolTip("Make a copy of a part");
  cmdItem->setActivatedCB(FFaDynCB0S(FapEditCmds::copyLink));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapEditCmds::getCopyLinkSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_edit_copySubassembly");
  cmdItem->setText("Copy Subassembly");
  cmdItem->setToolTip("Make a copy of a subassembly");
  cmdItem->setActivatedCB(FFaDynCB0S(FapEditCmds::copySubassembly));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapEditCmds::getCopySubassemblySensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_edit_mergeParts");
  cmdItem->setText("Merge Parts");
  cmdItem->setToolTip("Merge Generic Parts into one");
  cmdItem->setActivatedCB(FFaDynCB0S(FapEditCmds::mergeParts));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapEditCmds::getMergePartsSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_edit_splitBeam");
  cmdItem->setSmallIcon(splitBeam_xpm);
  cmdItem->setText("Split Beam...");
  cmdItem->setToolTip("Split a beam into several beams");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapEditCmds::splitBeam(0); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapEditCmds::getSplitBeamSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_edit_orientCam");
  cmdItem->setSmallIcon(camJoint_xpm);
  cmdItem->setText("Reorient Cam Joint");
  cmdItem->setToolTip("Reorient all triads of the Cam surface");
  cmdItem->setActivatedCB(FFaDynCB0S(FapEditCmds::reorientCam));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapEditCmds::getReorientCamSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_edit_selectTempSelection");
  cmdItem->setText("Select");
  cmdItem->setToolTip("Select this object");
  cmdItem->setActivatedCB(FFaDynCB0S(FapEditCmds::selectTempSelection));

  cmdItem = new FFuaCmdItem("cmdId_edit_selectTempSelectionAll");
  cmdItem->setText("Select All");
  cmdItem->setToolTip("Select all objects");
  cmdItem->setActivatedCB(FFaDynCB0S(FapEditCmds::selectTempSelectionAll));
}
//----------------------------------------------------------------------------

void FapEditCmds::erase()
{
  std::vector<FFaViewItem*> totSelection;
  if (!FapCmdsBase::getCurrentSelection(totSelection))
    return;

  FpPM::vpmSetUndoPoint("Delete");

  FuiCtrlModes::cancel();
  FmModelMemberBase::inInteractiveErase = true;

  for (FFaViewItem* item : totSelection)

    // Check if still selected, each object could be erased from an owner
    if (FapEventManager::isPermSelected(item) ||
	FapEventManager::isTmpSelected(item))
    {
      if ((objBeingErased = dynamic_cast<FmModelMemberBase*>(item)))
      {
	// TT #2943: Don't erase element group if owner link has been selected,
	// such that the group doesn't go away if the link erase is cancelled
	if (objBeingErased->isOfType(FmElementGroupProxy::getClassTypeID()))
	{
	  FmPart* grOwner = ((FmElementGroupProxy*)objBeingErased)->getOwner();
	  if (FapEventManager::isPermSelected(grOwner) ||
	      FapEventManager::isTmpSelected(grOwner))
	    objBeingErased = NULL;
	}
	else if (objBeingErased->isOfType(FmSimulationEvent::getClassTypeID()))
	{
	  if (FapSimEventHandler::getActiveEvent() == objBeingErased)
	    FapSimEventHandler::activate(NULL);
	}

        // Bugfix #101: prevent crash when deleting graphs and the graph window is open
        else if (objBeingErased->isOfType(FmGraph::getClassTypeID()))
          FapEventManager::setActiveWindow(dynamic_cast<FuiModeller*>(FFuTopLevelShell::getInstanceByType(FuiModeller::getClassTypeID())));

	// Do *not* add any other class-specific deletion tasks here.
	// Such customization is added by reimplementing the virtual
	// interactiveErase method for that particular class.

	// Check if object is used in curve axis definition(s)
	if (objBeingErased && FapEditCmds::checkUsedByCurves(item))
	  objBeingErased->interactiveErase();

	objBeingErased = NULL;
      }
      else
	delete item;
    }

  FFaMsg::resetToAllAnswer();
  FmModelMemberBase::inInteractiveErase = false;
}
//----------------------------------------------------------------------------

bool FapEditCmds::checkUsedByCurves(FFaViewItem* item)
{
  // If this is an object that can be plotted, check if it used by a curve
  FmIsPlottedBase* deletedObj = dynamic_cast<FmIsPlottedBase*>(item);

  if (!deletedObj) return true; // not a plot-able object
  if (!deletedObj->hasCurveSets()) return true; // no curves

  std::vector<FmCurveSet*> curves;
  deletedObj->getCurveSets(curves);

  for (FmCurveSet* curve : curves)
    for (int axis = 0; axis < FmCurveSet::NAXES; axis++)
      if (curve->getResultObj(axis) == deletedObj)
      {
	char a = 'X' + axis - FmCurveSet::XAXIS;
	std::string msg = "Deleting " + deletedObj->getIdString(true);
	msg += std::string(".\nThis object is used in the ") + a
	  + "-axis definition of\n" + curve->getIdString(true)
	  + " in " + curve->getOwnerGraph()->getIdString(true);
	msg += ".\nDo you want to delete this axis definition too "
	  "(if not you should modify the curve) ?\n\n"
	  "Select \"Cancel\" to not delete this "
	  + std::string(deletedObj->getUITypeName()) + ".";

	switch (FFaMsg::dialog(msg,FFaMsg::YES_ALL_NO_CANCEL))
	  {
	  case 1: // yes - erase curve axis definition
	    curve->clearResult(axis);
	    curve->onDataChanged();
	    break;

	  case 2: // cancel - don't delete this object
	    // But don't automatically repeat this cancel for all
	    FFaMsg::resetToAllAnswer(FFaMsg::YES_ALL_NO_CANCEL);
	    return false;
	  }
      }

  return true;
}
//----------------------------------------------------------------------------

void FapEditCmds::getEraseSensitivity(bool& isSensitive)
{
  isSensitive = false;
  if (FapEventManager::hasStackedSelections()) return;
  FapCmdsBase::isModelTouchable(isSensitive);
  if (!isSensitive) return;

  std::vector<FFaViewItem*> totSelection;
  if (!FapCmdsBase::getCurrentSelection(totSelection))
    isSensitive = false;
  else for (FFaViewItem* item : totSelection)
    if (!FapEditCmds::isEraseAble(item))
      isSensitive = false;
}
//----------------------------------------------------------------------------

// JL: This is being called all too often. Need to keep track of which
// elements have been checked in this round and filter on them first...

bool FapEditCmds::isEraseAble(FFaViewItem* obj)
{
  FmModelMemberBase* mmb = dynamic_cast<FmModelMemberBase*>(obj);
  if (!mmb) return false;

  // Graphs/curves/animations are always erasable,
  // except for the loaded animation
  if (mmb->isOfType(FmResultBase::getClassTypeID()))
    return ((FmAnimation*)mmb != FapEventManager::getActiveAnimation());

  // Strain rosettes are always erasable,
  // except those having results already
  if (mmb->isOfType(FmStrainRosette::getClassTypeID()))
    return !FpRDBExtractorManager::instance()->hasResults(mmb);

  // The remaining object types are erasable only when there are no results
  bool eraseable;
  FapCmdsBase::isModelEditable(eraseable);
  if (!eraseable) return false;

  // Object check
  if (mmb->isOfType(FmRingStart::getClassTypeID()))
    eraseable = false;
  else if (mmb->isOfType(FmRefPlane::getClassTypeID()))
    eraseable = false;
  else if (mmb->isOfType(FmSensorBase::getClassTypeID()))
    if (((FmSensorBase*)mmb)->isTime() ||
	((FmSensorBase*)mmb)->isControlOutput() ||
	((FmSensorBase*)mmb)->isExternalCtrlSys())
      eraseable = false;

  // License check
  if (mmb->isOfType(FmCtrlLine::getClassTypeID()) ||
      mmb->isOfType(FmCtrlElementBase::getClassTypeID()))
    eraseable = FapLicenseManager::hasCtrlLicense(0,0);
#ifdef FT_HAS_EXTCTRL
  else if (mmb->isOfType(FmExternalCtrlSys::getClassTypeID()))
    eraseable = FapLicenseManager::hasExtCtrlLicense(0);
#endif

  return eraseable;
}
//----------------------------------------------------------------------------

void FapEditCmds::copyObject()
{
  std::vector<FmModelMemberBase*> selection;
  FapCmdsBase::getCurrentSelection(selection);

  FmBase* lastCopy = NULL;
  for (FmModelMemberBase* sel : selection)
    lastCopy = sel->duplicate();

  FapEventManager::permUnselectAll();
  if (lastCopy)
    FapEventManager::permTotalSelect(static_cast<FmModelMemberBase*>(lastCopy));
}
//----------------------------------------------------------------------------

void FapEditCmds::getCopyObjectSensitivity(bool& isSensitive)
{
  FapCmdsBase::isModelEditable(isSensitive);
  if (!isSensitive) return;

  std::vector<FmModelMemberBase*> totSelection;
  if (!FapCmdsBase::getCurrentSelection(totSelection))
    isSensitive = false;
  else for (FmModelMemberBase* obj : totSelection)
    if (!obj->isOfType(FmStructPropertyBase::getClassTypeID()) &&
        !obj->isOfType(FmEngine::getClassTypeID()))
      isSensitive = false;
}
//----------------------------------------------------------------------------

void FapEditCmds::copySubassembly()
{
  std::vector<FmModelMemberBase*> selection;
  FapCmdsBase::getCurrentSelection(selection);

  FmBase* lastCopy = NULL;
  for (FmModelMemberBase* sel : selection)
    if (sel->isOfType(FmSubAssembly::getClassTypeID()))
      lastCopy = sel->duplicate();

  FapEventManager::permUnselectAll();
  if (lastCopy)
    FapEventManager::permTotalSelect(static_cast<FmModelMemberBase*>(lastCopy));
}
//----------------------------------------------------------------------------

void FapEditCmds::getCopySubassemblySensitivity(bool& isSensitive)
{
  FapCmdsBase::isModelEditable(isSensitive);
  if (!isSensitive) return;

  std::vector<FmModelMemberBase*> totSelection;
  if (!FapCmdsBase::getCurrentSelection(totSelection))
    isSensitive = false;
  else for (FmModelMemberBase* obj : totSelection)
    if (!obj->isOfType(FmSubAssembly::getClassTypeID()) ||
        !static_cast<FmSubAssembly*>(obj)->isCopyable())
      isSensitive = false;
}
//----------------------------------------------------------------------------

void FapEditCmds::copyLink()
{
  std::vector<FmPart*> selection;
  FapCmdsBase::getSelectedParts(selection);

  FmBase* lastCopy = NULL;
  for (FmPart* part : selection)
    lastCopy = part->duplicate();

  FapEventManager::permUnselectAll();
  if (lastCopy)
    FapEventManager::permTotalSelect(static_cast<FmModelMemberBase*>(lastCopy));
}
//----------------------------------------------------------------------------

void FapEditCmds::getCopyLinkSensitivity(bool& isSensitive)
{
  FapCmdsBase::isModelEditable(isSensitive);
  if (!isSensitive) return;

  std::vector<FmModelMemberBase*> totSelection;
  if (!FapCmdsBase::getCurrentSelection(totSelection))
    isSensitive = false;
  else for (FmModelMemberBase* obj : totSelection)
    if (!obj->isOfType(FmPart::getClassTypeID()))
      isSensitive = false;
}
//----------------------------------------------------------------------------

void FapEditCmds::mergeParts()
{
  std::vector<FmPart*> selection;
  FapCmdsBase::getSelectedParts(selection);

  for (size_t i = 1; i < selection.size(); i++)
    selection.front()->mergeGenericParts(selection[i]);
}
//----------------------------------------------------------------------------

void FapEditCmds::getMergePartsSensitivity(bool& isSensitive)
{
  FapCmdsBase::isModelEditable(isSensitive);
  if (!isSensitive) return;

  size_t nParts = 0;
  std::vector<FmModelMemberBase*> totSelection;
  if (FapCmdsBase::getCurrentSelection(totSelection))
    for (FmModelMemberBase* obj : totSelection)
      if (obj->isOfType(FmPart::getClassTypeID()))
        nParts++;

  isSensitive = nParts > 1;
}
//----------------------------------------------------------------------------

void FapEditCmds::splitBeam(int nseg)
{
  FmBeam* beam = dynamic_cast<FmBeam*>(FapEventManager::getTmpMMBSelection());
  if (!beam && FapEventManager::getPermMMBSelection().size() == 1)
    beam = dynamic_cast<FmBeam*>(FapEventManager::getPermMMBSelection().front());

  if (!beam)
    return;
  else if (nseg > 1)
    beam->split(nseg);
  else
  {
    // Launch dialog to prompt for user input
    FuiSplitBeam* dialog = FuiSplitBeam::getUI(true);
    dialog->setDialogButtonClickedCB(FFaDynCB1S(FapEditCmds::onSplitBeamDone,int));
    dialog->execute();
  }
}
//----------------------------------------------------------------------------

void FapEditCmds::onSplitBeamDone(int button)
{
  FuiSplitBeam* dialog = FuiSplitBeam::getUI(false);
  if (button == 0)
    FapEditCmds::splitBeam(dialog->getValue());
}
//----------------------------------------------------------------------------

void FapEditCmds::getSplitBeamSensitivity(bool& isSensitive)
{
  FapCmdsBase::isModelEditable(isSensitive);
  if (!isSensitive) return;

  std::vector<FmModelMemberBase*> selection;
  if (!FapCmdsBase::getCurrentSelection(selection) || selection.size() != 1)
    isSensitive = false;
  else if (selection.front()->isOfType(FmBeam::getClassTypeID()))
    // We do not allow to split wind turbine blade elements
    isSensitive = !static_cast<FmBeam*>(selection.front())->getBladeProperty();
  else
    isSensitive = false;
}
//----------------------------------------------------------------------------

void FapEditCmds::reorientCam()
{
  FmCamJoint* cam = dynamic_cast<FmCamJoint*>(FapEventManager::getTmpMMBSelection());
  if (!cam && !FapEventManager::getPermMMBSelection().empty())
    cam = dynamic_cast<FmCamJoint*>(FapEventManager::getPermMMBSelection().front());

  if (cam)
    cam->setDefaultRotationOnMasters();
}
//----------------------------------------------------------------------------

void FapEditCmds::getReorientCamSensitivity(bool& isSensitive)
{
  FapCmdsBase::isModelEditable(isSensitive);
  if (!isSensitive) return;

  std::vector<FmModelMemberBase*> selection;
  if (!FapCmdsBase::getCurrentSelection(selection) || selection.empty())
    isSensitive = false;
  else
    isSensitive = selection.front()->isOfType(FmCamJoint::getClassTypeID());
}
//----------------------------------------------------------------------------

void FapEditCmds::selectTempSelection()
{
  FmModelMemberBase* tmpSelection = FapEventManager::getTmpMMBSelection();
  if (!tmpSelection) return;

  FapEventManager::permUnselectAll();
  FapEventManager::permSelect(tmpSelection);
  FapEventManager::tmpSelect(NULL);
}
//----------------------------------------------------------------------------

void FapEditCmds::selectTempSelectionAll()
{
  // Get the topology view
  FuiTopologyView* topologyView = FuiTopologyView::getTopologyView();
  if (!topologyView) return;

  // Get a list of all top-level items
  // Note: The top-level child(ren) might not have ids, which means that
  //       we need to get the children (of that group).
  FFuListView* listView = topologyView->getListView();
  FFuListViewItem* firstItem = listView->getFirstChildItem();
  if (!firstItem) return;

  if (!firstItem->getItemText(1).empty()) firstItem = NULL;
  std::vector<FFuListViewItem*> items = listView->getListChildren(firstItem);

  // All possible object type prefices in the Topology view.
  // See also FapUAProperties::getDBValues().
  static std::vector<const char*> pfxs = {
    "First ", "Second ",
    "Master ", "Slave ",
    "Bearing ", "Element property ",
    "Input: ", "Output: ",
    "Tx: ", "Ty: ", "Tz: ", "Rx: ", "Ry: ", "Rz: ",
  };

  // Select the top-level items
  std::vector<FFaViewItem*> itemsToSel;
  for (FFuListViewItem* item : items)
  {
    std::string strType = item->getItemText(0);
    if (strType.empty()) continue;
    std::string strUIds = item->getItemText(1);
    if (strUIds.empty()) continue;

    // Remove possible object type prefix
    for (const char* pfx : pfxs)
      if (strType.find(pfx) == 0)
      {
        strType.erase(0,strlen(pfx));
        break;
      }

    // Get id of item
    const char* pszIds = strUIds.c_str();
    // The first number is the id of the element
    int id = atol(pszIds);
    // The other numbers are the ids of the parent assemblies
    std::vector<int> assemblyIds;
    while ((pszIds = strchr(pszIds,',')))
    {
      ++pszIds; // skip the comma
      if (assemblyIds.empty())
        assemblyIds.push_back(atol(pszIds));
      else
        assemblyIds.insert(assemblyIds.begin(),atol(pszIds));
    }

    FmModelMemberBase* mmb = dynamic_cast<FmModelMemberBase*>(FmDB::findID(strType,id,assemblyIds));
    if (mmb) itemsToSel.push_back(mmb);
  }

  // Assign to selection if we found something
  if (!itemsToSel.empty())
    FapEventManager::permTotalSelect(itemsToSel);
}
