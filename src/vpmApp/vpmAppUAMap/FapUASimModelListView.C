// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "vpmApp/vpmAppUAMap/FapUASimModelListView.H"
#include "vpmApp/vpmAppCmds/FapFunctionCmds.H"
#include "vpmApp/FapLicenseManager.H"
#include "vpmUI/vpmUIComponents/FuiItemsListView.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmBeam.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmCamJoint.H"
#include "vpmDB/FmRingStart.H"
#include "vpmDB/FmBladeProperty.H"
#include "vpmDB/FmSimulationEvent.H"
#include "vpmDB/FmElementGroupProxy.H"
#include "vpmDB/FmStructAssembly.H"
#include "vpmDB/FmTurbine.H"
#include "vpmDB/FmRiser.H"
#include "vpmDB/FmSoilPile.H"
#include "FFaLib/FFaCmdLineArg/FFaCmdLineArg.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFaLib/FFaDefinitions/FFaAppInfo.H"


//----------------------------------------------------------------------------

FapUASimModelListView::FapUASimModelListView(FuiItemsListView* ui)
  : FapUAItemsListView(ui), FapUAModMemListView(ui)
{
  this->automUpdateParentsPresence = true;
  this->maintainSorting = true;
  this->debugMode = false;
  FFaCmdLineArg::instance()->getValue("debug",this->debugMode);
}
//----------------------------------------------------------------------------

bool FapUASimModelListView::verifyItem(FFaListViewItem* item)
{
  if (!item) return false;

  if (!this->FapUAModMemListView::verifyItem(item))
    return false;

#ifdef LV_DEBUG
  reportItem(item,"FapUASimModelListView::verifyItem: ");
#endif

  FmModelMemberBase* mmb = dynamic_cast<FmModelMemberBase*>(item);
  if (!mmb) return false;

  // Check the most likely leaf type first
  if (mmb->isOfType(FmSimulationModelBase::getClassTypeID()))
    return ((FmSimulationModelBase*)mmb)->isListable();

  else if (mmb->isOfType(FmSubAssembly::getClassTypeID()))
    return ((FmSubAssembly*)mmb)->isListable();

  else if (mmb->isOfType(FmSimulationEvent::getClassTypeID()))
    return true;

  else if (mmb->isOfType(FmRingStart::getClassTypeID()))
  {
    // Headers
    int memberClassType = ((FmRingStart*)mmb)->getRingMemberType();
    if (memberClassType == FmElementGroupProxy::getClassTypeID())
      return false; // don't want the Element Group header

    if (memberClassType == FmMathFuncBase::getClassTypeID()) {
      std::vector<FFaListViewItem*> children;
      this->getChildren(item,children);
      for (FFaListViewItem* child : children)
	if (this->verifyItem(child))
	  return true;
      return false;
    }

    // Check for sub-headers
    const std::vector<FmRingStart*>& subheaders = ((FmRingStart*)mmb)->getChildren();

    if (!subheaders.empty()) {
      for (FmRingStart* head : subheaders)
	if (this->verifyItem(head))
	  return true;
    }

    else if (mmb->getNext()->isOfType(FmSimulationModelBase::getClassTypeID()) ||
	     memberClassType == FmSimulationEvent::getClassTypeID() ||
	     memberClassType == FmSubAssembly::getClassTypeID()) {
      // Check for leafs
      std::vector<FmModelMemberBase*> items;
      ((FmRingStart*)mmb)->getModelMembers(items);
      for (FmModelMemberBase* obj : items)
        if (this->verifyItem(obj))
          return true;
    }
  }

  return false;
}
//----------------------------------------------------------------------------

bool FapUASimModelListView::isHeaderOkAsLeaf(FFaListViewItem* item) const
{
  return dynamic_cast<FmPart*>(item) ? true : false;
}
//----------------------------------------------------------------------------

FFaListViewItem* FapUASimModelListView::getParent(FFaListViewItem* item,
						  const std::vector<int>& assID) const
{
#ifdef LV_DEBUG
  reportItem(item,"FapUASimModelListView::getParent: ");
#endif

  FmModelMemberBase* mmb = dynamic_cast<FmModelMemberBase*>(item);
  if (!mmb) return 0;

  FFaListViewItem* parent = 0;
  if (mmb->isOfType(FmRingStart::getClassTypeID()))
  {
    parent = ((FmRingStart*)mmb)->getParent();
    if (!parent && !assID.empty())
      if ((parent = (FmModelMemberBase*)mmb->getParentAssembly()))
        if (assID.back() != parent->getItemID())
        {
          std::cerr <<"ERROR: Assembly topology inconsistency detected!"<< std::endl;
          parent = 0;
        }
  }
  else if (mmb->isOfType(FmElementGroupProxy::getClassTypeID()))
    parent = ((FmElementGroupProxy*)mmb)->getOwner();
  else if (mmb->isOfType(FmMathFuncBase::getClassTypeID()))
  {
    FmRingStart* head = FmDB::getHead(FmMathFuncBase::getClassTypeID(),assID);
    if (head) parent = head->searchFuncHead(((FmMathFuncBase*)mmb)->getFunctionUse());
  }
  else
  {
    FmRingStart* head = FmDB::getHead(mmb->getTypeID(),assID);
    if (head && head->getRingMemberType() == FmSubAssembly::getClassTypeID())
      // Skip the Assemblies header, moving one level up
      parent = this->getParent(head,assID);
    else
      parent = head;
  }

#ifdef LV_DEBUG
  reportItem(parent,"                               -> ");
#endif
  return parent;
}
//----------------------------------------------------------------------------

void FapUASimModelListView::getChildren(FFaListViewItem* parent,
					std::vector<FFaListViewItem*>& children) const
{
#ifdef LV_DEBUG
  reportItem(parent,"FapUASimModelListView::getChildren: ");
#endif

  if (!parent) {

    // Top headers
    for (const std::pair<const int,FmRingStart*>& head : *FmDB::getHeadMap())
      if (!head.second->getParent() && head.second->hasRingMembers())
      {
	if (head.second->getRingMemberType() == FmSubAssembly::getClassTypeID())
	  // Skip the Assemblies header, get its children instead
	  this->getChildren(head.second,children);
	else
	  children.push_back(head.second);
      }

    return;
  }

  FmModelMemberBase* mmb = static_cast<FmModelMemberBase*>(parent);
  if (mmb->isOfType(FmRingStart::getClassTypeID())) {

    // Check for sub-headers
    const std::vector<FmRingStart*>& subheaders = ((FmRingStart*)mmb)->getChildren();
    for (FmRingStart* head : subheaders)
      if (head->hasRingMembers())
	children.push_back(head);

    // Check for leafs
    std::vector<FmModelMemberBase*> items;
    ((FmRingStart*)mmb)->getModelMembers(items);
    for (FmModelMemberBase* item : items)
      children.push_back(item);
  }

  else if (mmb->isOfType(FmPart::getClassTypeID())) {

    // parent is a Part, check for element groups
    std::vector<FmElementGroupProxy*> groups;
    ((FmPart*)mmb)->getElementGroups(groups);
    for (FmElementGroupProxy* group : groups)
      children.push_back(group);
  }

  else if (mmb->isOfType(FmSubAssembly::getClassTypeID())) {

    // parent is a Subassembly
    for (const std::pair<const int,FmRingStart*>& head : *((FmSubAssembly*)mmb)->getHeadMap())
      if (!head.second->getParent() && head.second->hasRingMembers())
      {
	if (head.second->getRingMemberType() == FmSubAssembly::getClassTypeID())
	  // Skip the Assemblies header, get its children instead
	  this->getChildren(head.second,children);
	else
	  children.push_back(head.second);
      }
  }

#if LV_DEBUG > 1
  if (children.empty()) return;
  std::cout <<"================================\n";
  reportItem(parent,"Unverified children of: ");
  for (FFaListViewItem* ch : children) reportItem(ch,"\t");
  std::cout <<"================================"<< std::endl;
#endif
}
//----------------------------------------------------------------------------

std::vector<std::string> FapUASimModelListView::getItemText(FFaListViewItem* item)
{
  if (dynamic_cast<FmRingStart*>(item))
    return std::vector<std::string>(1,item->getItemName());

#ifndef LV_DEBUG
  if (dynamic_cast<FmAssemblyBase*>(item) && !dynamic_cast<FmStructAssembly*>(item))
    // Try leave out the user ID for positioned assembly objects
    // (they are normally very few and of different sub-classes
    // which is sufficient for proper identification)
    return std::vector<std::string>(1,item->getItemDescr());
#endif

  FFaNumStr txt("[%d]",item->getItemID());
  if (debugMode)
    txt += FFaNumStr("{%d}",item->getItemBaseID());
  txt += " " + item->getItemDescr();
  return std::vector<std::string>(1,txt);
}
//----------------------------------------------------------------------------

/*!
  Use this method instead of FapEventManager::isObjectOfTypeSelected when
  the class to type check does not have a working getClassTypeID method,
  such as all the child classes of FmSubAssembly, etc.
*/

template<class T> static bool isObjectOfTypeSelected(const T&)
{
  if (dynamic_cast<T*>(FapEventManager::getTmpSelection()))
    return true;
  else if (dynamic_cast<FmModelMemberBase*>(FapEventManager::getTmpSelection()))
    return false;
  else if (dynamic_cast<T*>(FapEventManager::getFirstPermSelectedObject()))
    return true;
  else
    return false;
}

//----------------------------------------------------------------------------

FFuaUICommands* FapUASimModelListView::getCommands()
{
  FuaItemsLVCommands* cmds = new FuaItemsLVCommands();

  // Clearing sub menu entries

  this->createHeader.clear();
  this->createSpringFunctionHeader.clear();
  this->createDamperFunctionHeader.clear();
  this->createFrictionHeader.clear();

  // Setting up sub menu command headers :

  this->createHeader.setText("Create");
  this->createSpringFunctionHeader.setText("Spring characteristics");
  this->createSpringFunctionHeader.setSmallIcon(spring_xpm);
  this->createDamperFunctionHeader.setText("Damper characteristics");
  this->createDamperFunctionHeader.setSmallIcon(damper_xpm);
  this->createFrictionHeader.setText("Friction");
  this->createFrictionHeader.setSmallIcon(friction_xpm);

  this->convertHeader.setText("Convert function");
  this->convertToSpringHeader.setText("Spring characteristics");
  this->convertToSpringHeader.setSmallIcon(spring_xpm);
  this->convertToDamperHeader.setText("Damper characteristics");
  this->convertToDamperHeader.setSmallIcon(damper_xpm);

  this->solveHeader.setText("Solve");
  this->resultHeader.setText("Result");

  // Build menus

  cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_zoomTo"));
  cmds->popUpMenu.push_back(&this->separator);

  cmds->popUpMenu.push_back(&this->createHeader);

  this->createHeader.push_back(&this->createSpringFunctionHeader);
  this->createSpringFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_SprTransStiff"));
  this->createSpringFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_SprTransForce"));
  this->createSpringFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_SprRotStiff"));
  this->createSpringFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_SprRotTorque"));
  this->createSpringFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_SpringCharTrans"));
  this->createSpringFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_SpringCharRot"));

  this->createHeader.push_back(&this->createDamperFunctionHeader);
  this->createDamperFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_DaTransCoeff"));
  this->createDamperFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_DaTransForce"));
  this->createDamperFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_DaRotCoeff"));
  this->createDamperFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_DaRotTorque"));

  this->createHeader.push_back(&this->separator);
  this->createHeader.push_back(&this->createFrictionHeader);
  this->createFrictionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_RotFriction"));
  this->createFrictionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_TransFriction"));
  this->createFrictionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_BearingFriction"));
  this->createFrictionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_PrismFriction"));
  this->createFrictionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_CamFriction"));

  this->createHeader.push_back(&this->separator);
  this->createHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_GeneralFunction"));
  this->createHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_DriveFile"));
  if (FapLicenseManager::hasFeature("FA-TII"))
    this->createHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_RoadFunction"));
  if (FapLicenseManager::hasFeature("FA-WND") || FapLicenseManager::hasFeature("FA-RIS")) {
    this->createHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_WaveFunction"));
    this->createHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_CurrFunction"));
  }
#ifdef FT_HAS_EXTCTRL
  this->createHeader.push_back(&this->separator);
  this->createHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_external_ctrl_sys"));
#endif
  this->createHeader.push_back(&this->separator);
  this->createHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_file_reference"));
  this->createHeader.push_back(&this->separator);
  this->createHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_BeamProperty"));
  this->createHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_MaterialProperty"));
  this->createHeader.push_back(&this->separator);
  this->createHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_Assembly"));

  bool convertable = false, convertableSprDa = false, convertDriveFile = false;
  FapFunctionCmds::getConvertFuncSensitivity(convertable, convertableSprDa, convertDriveFile);

  if (convertable) {
    this->convertHeader.clear();
    this->convertToSpringHeader.clear();
    this->convertToDamperHeader.clear();

    cmds->popUpMenu.push_back(&this->separator);
    cmds->popUpMenu.push_back(&this->convertHeader);

    if (convertableSprDa) {
      this->convertHeader.push_back(&this->convertToSpringHeader);
      this->convertHeader.push_back(&this->convertToDamperHeader);

      this->convertToSpringHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_ConvertSprTransStiff"));
      this->convertToSpringHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_ConvertSprTransForce"));
      this->convertToSpringHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_ConvertSprRotStiff"));
      this->convertToSpringHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_ConvertSprRotTorque"));

      this->convertToDamperHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_ConvertDaTransCoeff"));
      this->convertToDamperHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_ConvertDaTransForce"));
      this->convertToDamperHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_ConvertDaRotCoeff"));
      this->convertToDamperHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_ConvertDaRotTorque"));
    }
    this->convertHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_ConvertGeneralFunction"));
    if (convertDriveFile)
      this->convertHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_ConvertDriveFile"));
    if (FapLicenseManager::hasFeature("FA-TII"))
      this->convertHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_ConvertRoadFunction"));
  }

  if (FapEventManager::isObjectOfTypeSelected(FmSubAssembly::getClassTypeID()))
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_file_loadLink"));

  if (FapEventManager::isObjectOfTypeSelected(FmLink::getClassTypeID()) ||
      FapEventManager::isObjectOfTypeSelected(FmElementGroupProxy::getClassTypeID())) {
    cmds->popUpMenu.push_back(&this->separator);
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_LinkSelection_hide"));
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_LinkSelection_show"));
  }
  if (FapEventManager::isObjectOfTypeSelected(FmStructPropertyBase::getClassTypeID()) ||
      FapEventManager::isObjectOfTypeSelected(FmEngine::getClassTypeID()))
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_copyObject"));
  if (FapEventManager::isObjectOfTypeSelected(FmSubAssembly::getClassTypeID()))
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_copySubassembly"));
  if (FapEventManager::isObjectOfTypeSelected(FmPart::getClassTypeID())) {
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_copyLink"));
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_mergeParts"));
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_file_changeLink"));
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_export_exportLink"));
  }
  if (FapEventManager::isObjectOfTypeSelected(FmBeam::getClassTypeID()))
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_splitBeam"));
  if (FapEventManager::isObjectOfTypeSelected(FmCamJoint::getClassTypeID()))
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_orientCam"));
  if (FapEventManager::isObjectOfTypeSelected(FmPart::getClassTypeID()) ||
      FapEventManager::isObjectOfTypeSelected(FmElementGroupProxy::getClassTypeID())) {

    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_StrainCoat_selection"));

    cmds->popUpMenu.push_back(&this->separator);

    cmds->popUpMenu.push_back(&this->solveHeader);

    this->solveHeader.clear();
    if (!FFaAppInfo::checkProgramPath("fedem_partsol").empty())
      this->solveHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveLink"));
    this->solveHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_reduceLink"));
    this->solveHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveStressOnLink"));
    this->solveHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveRosetteOnLink"));
    this->solveHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveStrainCoatOnLink"));
#ifdef FT_HAS_NCODE
    this->solveHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveFEFatigueOnLink"));
#endif
  }

  if (FapLicenseManager::hasFeature("FA-WND")) {

    if (FapEventManager::isObjectOfTypeSelected(FmBeam::getClassTypeID()) ||
	FapEventManager::isObjectOfTypeSelected(FmTriad::getClassTypeID())) {

      cmds->popUpMenu.push_back(&this->separator);
      cmds->popUpMenu.push_back(&this->resultHeader);

      this->resultHeader.clear();
      this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_beamForceGraph"));
    }
    if (isObjectOfTypeSelected(FmBladeDesign(true)))
      cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_bladeDefinition"));

    if (isObjectOfTypeSelected(FmTurbine(false,true)))
      cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_createTurbineAssembly"));

    if (isObjectOfTypeSelected(FmTower(true)))
      cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_createTurbineTower"));
  }

  if (FapLicenseManager::hasFeature("FA-RIS"))
    if (isObjectOfTypeSelected(FmRiser(true)) || isObjectOfTypeSelected(FmSoilPile(true)))
      cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_createBeamstringPair"));

  FmRingStart* seHead = FmDB::getHead(FmSimulationEvent::getClassTypeID());
  if (FapEventManager::isTmpSelected(seHead) || FapEventManager::isPermSelected(seHead)) {
    cmds->popUpMenu.push_back(&this->separator);
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_eventDefinition"));
  }

  if (FapEventManager::isObjectOfTypeSelected(FmSimulationModelBase::getClassTypeID()) ||
      FapEventManager::isObjectOfTypeSelected(FmSubAssembly::getClassTypeID())) {
    cmds->popUpMenu.push_back(&this->separator);
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_objectBrowser"));
  }

  cmds->popUpMenu.push_back(&this->separator);

  cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_listView_sortListViewByID"));
  cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_listView_sortListViewByName"));

  if (FapEventManager::isObjectOfTypeSelected(FmSubAssembly::getClassTypeID())) {
    cmds->popUpMenu.push_back(&this->separator);
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_SubassemblySelection_show"));
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_SubassemblySelection_hide"));
  }

  if (FapEventManager::hasTmpSelection() || FapEventManager::hasPermSelection()) {
    cmds->popUpMenu.push_back(&this->separator);
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_erase"));
  }

  return cmds;
}
