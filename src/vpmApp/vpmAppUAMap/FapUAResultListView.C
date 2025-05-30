// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAResultListView.H"
#include "vpmApp/vpmAppUAMap/FapUARDBSelector.H"
#include "vpmApp/FapEventManager.H"
#include "vpmApp/FapLicenseManager.H"
#include "vpmUI/vpmUIComponents/FuiItemsListView.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "vpmUI/Icons/curveSymbols.h"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmGraph.H"
#include "vpmDB/FmCurveSet.H"
#include "vpmDB/FmAnimation.H"
#include "vpmDB/FmRingStart.H"
#include "vpmDB/FmSubAssembly.H"
#include "vpmDB/FmPipeStringDataExporter.H"

#include "FFrLib/FFrEntryBase.H"
#include "FFaLib/FFaOperation/FFaOpUtils.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFpLib/FFpFatigue/FFpSNCurveLib.H"

#include <array>
#include <map>

#include <cstdio>
#include <cstdlib>
#include <cstring>


namespace // Curve symbol/color manipulations
{
  using IconsMap = std::map<int,const char**>;
  std::map<std::string,IconsMap> curveSymbols;
  std::multimap<std::string,int> colorSymbols;

  const char** getCurvePixMap(const std::array<float,3>& color, int symb)
  {
    // Initalize the curve symbol library
    static std::vector<const char**> curveSymbolLibrary = {
      symbol_Line_xpm,
      symbol_Plus_xpm,
      symbol_Cross_xpm,
      symbol_Circle_xpm,
      symbol_Diamond_xpm,
      symbol_Rectangle_xpm,
      symbol_UpTriangle_xpm,
      symbol_DownTriangle_xpm,
      symbol_LeftTriangle_xpm,
      symbol_RightTriangle_xpm
    };

    // Convert color value into a hexidesimal code
    std::string hexColor;
    char hexValue[5];
    for (float c : color)
    {
      int b = static_cast<int>(c*255.0f);
      sprintf(hexValue,"%x",b); // int to hex
      if (b < 16) hexColor.append("0");
      hexColor.append(hexValue);
    }

    // Check if this color exists
    std::map<std::string,IconsMap>::iterator cIt = curveSymbols.find(hexColor);
    std::multimap<std::string,int>::iterator xIt = colorSymbols.find(hexColor);
    if (cIt == curveSymbols.end())
      cIt = curveSymbols.insert({ hexColor, IconsMap() }).first; // New symbol
    else if (xIt != colorSymbols.end())
      for (; xIt != colorSymbols.upper_bound(hexColor); ++xIt)
        if (xIt->second == symb) // does a symbol for this color exist?
        {
          // Return pixmap if already existing
          IconsMap::const_iterator iit = cIt->second.find(symb);
          return iit == cIt->second.end() ? NULL : iit->second;
        }

    if (symb < 0 || symb >= static_cast<int>(curveSymbolLibrary.size()))
      return NULL; // symbol index out of range

    colorSymbols.insert({ hexColor, symb });

    // Create a new color and symbol icon
    const char** oldPixmap = curveSymbolLibrary[symb];

    // Find number of arrays in xpm-file
    std::string head(oldPixmap[0]);
    int i = head.find(" ");
    int j = head.find(" ",i+1) + 1;
    int n = 1 + atoi(head.substr(0,i).c_str()) + atoi(head.substr(j,1).c_str());

    char** newPixmap = new char*[n];
    for (j = 0; j < n; j++)
    {
      int length = strlen(oldPixmap[j]) + 1;
      char* temp = new char[length];
      strcpy(temp,oldPixmap[j]);

      if (*oldPixmap[j] == '$')
        for (int o = 0; o < length; o++)
          if (temp[o] == '#')
            strcpy(&temp[o+1],hexColor.c_str());

      newPixmap[j] = temp;
    }

    return cIt->second[symb] = (const char**)newPixmap;
  }
}
//----------------------------------------------------------------------------

FapUAResultListView::FapUAResultListView(FuiItemsListView* ui)
  : FapUAItemsListView(ui), FapUAModMemListView(ui)
{
  this->automUpdateParentsPresence = true;
  this->maintainSorting = true;

  this->importItemHeader.setText("Import");
  this->exportItemHeader.setText("Export");

  this->ui->setStartDragCB(FFaDynCB1M(FapUAResultListView,this,
                                      startDrag,bool&));
  this->ui->setDroppedCB(FFaDynCB2M(FapUAResultListView,this,
                                    dropItems,int,int&));

  // Header items
#ifdef FT_HAS_PREVIEW
  funcPreviews = new FmRingStart("Function previews", graph_xpm);
#else
  funcPreviews = NULL;
#endif
  beamDiagrams = new FmRingStart("Beam diagrams", graph_xpm);
}
//----------------------------------------------------------------------------

FFaListViewItem* FapUAResultListView::getParent(FFaListViewItem* item,
                                                const IntVec& assID) const
{
#ifdef LV_DEBUG
  reportItem(item,"FapUAResultListView::getParent: ");
#endif

  FFaListViewItem* parent = NULL;
  FmModelMemberBase* mmb = NULL;

  if (item == funcPreviews || item == beamDiagrams)
    parent = FmDB::getHead(FmGraph::getClassTypeID());
  else if ((mmb = dynamic_cast<FmModelMemberBase*>(item)))
  {
    if (mmb->isOfType(FmRingStart::getClassTypeID()))
    {
      parent = static_cast<FmRingStart*>(mmb)->getParent();
      if (!parent && !assID.empty())
	if ((parent = static_cast<FmModelMemberBase*>(mmb->getParentAssembly())))
	  if (assID.back() != parent->getItemID())
	  {
	    std::cerr <<"ERROR: Assembly topology inconsistency detected!"<< std::endl;
	    parent = NULL;
	  }
    }
    else if (mmb->isOfType(FmCurveSet::getClassTypeID()))
      parent = static_cast<FmCurveSet*>(mmb)->getOwnerGraph();
    else if (mmb->isOfType(FmGraph::getClassTypeID()))
    {
      if (static_cast<FmGraph*>(mmb)->isFuncPreview())
	parent = funcPreviews;
      else if (static_cast<FmGraph*>(mmb)->isBeamDiagram())
	parent = beamDiagrams;
      else
	parent = FmDB::getHead(mmb->getTypeID(),assID);
    }
    else
      parent = FmDB::getHead(mmb->getTypeID(),assID);
  }

  FmRingStart* rs = dynamic_cast<FmRingStart*>(parent);
  if (rs && rs->getRingMemberType() == FmSubAssembly::getClassTypeID())
    // Skip the Assemblies header, moving one level up
    parent = this->getParent(rs,assID);

#ifdef LV_DEBUG
  if (parent) reportItem(parent,"--> Parent is ");
#endif
  return parent;
}
//----------------------------------------------------------------------------

void FapUAResultListView::getChildren(FFaListViewItem* parent,
				      std::vector<FFaListViewItem*>& children) const
{
#ifdef LV_DEBUG
  reportItem(parent,"FapUAResultListView::getChildren: ");
#endif

  FmModelMemberBase* mmparent = NULL;
  if (!parent)
  {
    // Top headers
    const std::map<int,FmRingStart*>& topHeadMap = *FmDB::getHeadMap();
    for (const std::pair<const int,FmRingStart*>& head : topHeadMap)
      if (!head.second->getParent() && head.second->hasRingMembers())
      {
        if (head.second->getRingMemberType() == FmSubAssembly::getClassTypeID())
          // Skip the Assemblies header, get its children instead
          this->getChildren(head.second,children);
        else
          children.push_back(head.second);
      }
  }

  else if (parent == funcPreviews)
  {
    FmBase* gh = FmDB::getHead(FmGraph::getClassTypeID());
    for (FmBase* g = gh->getNext(); g != gh; g = g->getNext())
      if (static_cast<FmGraph*>(g)->isFuncPreview())
	children.push_back(static_cast<FmModelMemberBase*>(g));
  }

  else if (parent == beamDiagrams)
  {
    FmBase* gh = FmDB::getHead(FmGraph::getClassTypeID());
    for (FmBase* g = gh->getNext(); g != gh; g = g->getNext())
      if (static_cast<FmGraph*>(g)->isBeamDiagram())
	children.push_back(static_cast<FmModelMemberBase*>(g));
  }

  else
    mmparent = dynamic_cast<FmModelMemberBase*>(parent);

  if (!mmparent)
    return;

  else if (mmparent->isOfType(FmRingStart::getClassTypeID()))
  {
    // parent is header
    FmRingStart* rs = static_cast<FmRingStart*>(mmparent);
    std::vector<FmRingStart*> subheaders = rs->getChildren();
    if (subheaders.empty())
    {
      // parent is header with no subheaders (ie with leafs)
      std::vector<FmModelMemberBase*> items;
      rs->getModelMembers(items);
      for (FmModelMemberBase* item : items)
        children.push_back(item);
    }
    else
      // parent is header with subheaders
      for (FmRingStart* header : subheaders)
        if (header->hasRingMembers())
          children.push_back(header);

    if (rs->getRingMemberType() == FmGraph::getClassTypeID())
      if (!rs->getParentAssembly())
      {
	std::vector<FFaListViewItem*>::iterator it = children.begin();
	while (it != children.end())
	  if (static_cast<FmGraph*>(*it)->isBeamDiagram() || static_cast<FmGraph*>(*it)->isFuncPreview())
	    it = children.erase(it);
	  else
	    ++it;
        if (funcPreviews)
          children.push_back(funcPreviews);
        if (beamDiagrams)
          children.push_back(beamDiagrams);
      }
  }

  else if (mmparent->isOfType(FmGraph::getClassTypeID()))
  {
    // parent is a Graph, check for curves in it
    std::vector<FmCurveSet*> curves;
    static_cast<FmGraph*>(mmparent)->getCurveSets(curves);
    for (FmCurveSet* curve : curves)
    {
#ifndef FT_HAS_PREVIEW
      // Hide curves plotting internal functions
      if (curve->usingInputMode() >= FmCurveSet::INT_FUNCTION)
        continue;
#endif
      children.push_back(curve);
    }
  }

  else if (mmparent->isOfType(FmSubAssembly::getClassTypeID()))
  {
    // parent is a sub-assembly
    const std::map<int,FmRingStart*>& subHeadMap = *static_cast<FmSubAssembly*>(mmparent)->getHeadMap();
    for (const std::pair<const int,FmRingStart*>& head : subHeadMap)
      if (!head.second->getParent() && head.second->hasRingMembers())
      {
        if (head.second->getRingMemberType() == FmSubAssembly::getClassTypeID())
          // Skip the Assemblies header, get its children instead
          this->getChildren(head.second,children);
        else
          children.push_back(head.second);
      }
  }
}
//----------------------------------------------------------------------------

bool FapUAResultListView::verifyItem(FFaListViewItem* item)
{
  if (!item) return false;

  // "own" headers
  if (item == funcPreviews || item == beamDiagrams) {
    std::vector<FFaListViewItem*> subs;
    this->getChildren(item,subs);
    for (FFaListViewItem* subitem : subs)
      if (this->verifyItem(subitem))
        return true;
    return false;
  }

  if (!this->FapUAModMemListView::verifyItem(item)) return false;

#ifdef LV_DEBUG
  reportItem(item,"FapUAResultListView::verifyItem: ");
#endif

  FmModelMemberBase* mmb = dynamic_cast<FmModelMemberBase*>(item);
  if (!mmb) return false;

  // Check the most likely leaf type first
  if (mmb->isOfType(FmResultBase::getClassTypeID()))
    return true;

  else if (mmb->isOfType(FmSubAssembly::getClassTypeID()))
    return static_cast<FmSubAssembly*>(mmb)->isListable(true);

  else if (mmb->isOfType(FmRingStart::getClassTypeID()))
  {
    // Headers
    FmRingStart* rs = static_cast<FmRingStart*>(mmb);
    if (rs->getRingMemberType() == FmCurveSet::getClassTypeID())
      return false; // Don't want Curve header

    // Check for sub-headers
    const std::vector<FmRingStart*>& subheaders = rs->getChildren();
    if (!subheaders.empty()) {
      for (FmRingStart* header : subheaders)
        if (this->verifyItem(header))
          return true;
    }
    else if (mmb->getNext()->isOfType(FmResultBase::getClassTypeID()) ||
	     mmb->getNext()->isOfType(FmSubAssembly::getClassTypeID())) {
      // Check for leafs
      std::vector<FmModelMemberBase*> items;
      rs->getModelMembers(items);
      for (FmModelMemberBase* obj : items)
        if (this->verifyItem(obj))
          return true;
    }
  }

  return false;
}
//----------------------------------------------------------------------------

std::string FapUAResultListView::getItemText(FFaListViewItem* item)
{
  if (dynamic_cast<FmRingStart*>(item))
    return item->getItemName();

#ifndef LV_DEBUG
  if (dynamic_cast<FmResultBase*>(item))
    // Try leave out the user ID to clean up the result list view a bit...
    return item->getItemDescr();
#endif

  return FFaNumStr("[%d] ",item->getItemID()) + item->getItemDescr();
}
//----------------------------------------------------------------------------

bool FapUAResultListView::getItemTextBold(FFaListViewItem* item)
{
  FmModelMemberBase* mmb = dynamic_cast<FmModelMemberBase*>(item);
  return mmb ? mmb->isOfType(FmGraph::getClassTypeID()) : false;
}
//----------------------------------------------------------------------------

const char** FapUAResultListView::getItemPixmap(FFaListViewItem* item)
{
  if (item == beamDiagrams || item == funcPreviews)
    return graph_xpm;

  FmModelMemberBase* mmb = dynamic_cast<FmModelMemberBase*>(item);
  if (!mmb)
    return (const char**)NULL;
  else if (mmb->isOfType(FmGraph::getClassTypeID()))
    this->ui->setItemDropable(this->getMapItem(mmb),
                              !static_cast<FmGraph*>(mmb)->isBeamDiagram());
  else if (mmb->isOfType(FmCurveSet::getClassTypeID()))
  {
    this->ui->setItemDragable(this->getMapItem(mmb),true);
    FmCurveSet* curve = static_cast<FmCurveSet*>(mmb);
    return getCurvePixMap(curve->getColor(),curve->getCurveSymbol());
  }
  else if (mmb->isOfType(FmRingStart::getClassTypeID()))
  {
    FmRingStart* rs = static_cast<FmRingStart*>(mmb);
    if (rs->getRingMemberType() == FmAnimation::getClassTypeID())
      return animation_xpm;
    else if (rs->getRingMemberType() == FmGraph::getClassTypeID())
      return graph_xpm;
  }

  return (const char**)NULL;
}
//----------------------------------------------------------------------------

FFuaUICommands* FapUAResultListView::getCommands()
{
  FuaItemsLVCommands* cmds = new FuaItemsLVCommands();

  this->importItemHeader.clear();
  this->exportItemHeader.clear();

  bool areCurvesSelected = FapEventManager::isObjectOfTypeSelected(FmCurveSet::getClassTypeID());
  bool areGraphsSelected = FapEventManager::isObjectOfTypeSelected(FmGraph::getClassTypeID());
  bool areAnimsSelected  = FapEventManager::isObjectOfTypeSelected(FmAnimation::getClassTypeID());

  cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_graph_showRDBSelector"));
  cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_curve"));
  cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_graph"));
  cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_beamDiagram"));
  cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_animation"));
  cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_graphGroup"));

  if (FapEventManager::isObjectOfTypeSelected(FmSubAssembly::getClassTypeID())) {
    cmds->popUpMenu.push_back(&this->separator);
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_graph_enableAutoExport"));
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_graph_disableAutoExport"));
  }

  if (areCurvesSelected || areGraphsSelected || areAnimsSelected)
    cmds->popUpMenu.push_back(&this->separator);

  if (areCurvesSelected || areGraphsSelected) {
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_graph_show"));
    if (FFpSNCurveLib::allocated())
      cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_graph_showRDBMEFatigue"));
  }

  if (areAnimsSelected) {
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_animation_show"));
    if (FapLicenseManager::hasFeature("FA-VTF"))
      cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_export_exportVTF"));
  }

  if (areCurvesSelected) {
    cmds->popUpMenu.push_back(&this->separator);
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_graph_editXAxis"));
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_graph_editYAxis"));
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_graph_repeatCurveForAll"));
    cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_graph_repeatCurveForSome"));
  }

  cmds->popUpMenu.push_back(&this->separator);

  cmds->popUpMenu.push_back(&this->importItemHeader);
  this->importItemHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_importedCurve"));
  this->importItemHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_importedGraph"));

  if (areGraphsSelected || areCurvesSelected) {
    cmds->popUpMenu.push_back(&this->exportItemHeader);
    this->exportItemHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_export_exportCurves"));
    this->exportItemHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_export_exportGraphs"));
    this->exportItemHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_export_exportGraph"));
  }
  if (areGraphsSelected)
    this->exportItemHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_export_graphStatistics"));

  if (areGraphsSelected || areCurvesSelected)
    if (FFpSNCurveLib::allocated() && FapLicenseManager::hasFeature("FA-SEV"))
      this->exportItemHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_export_curveFatigue"));

  if (FapEventManager::isObjectOfTypeSelected(FmPipeStringDataExporter::getClassTypeID())) {
    cmds->popUpMenu.push_back(&this->exportItemHeader);
    this->exportItemHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_export_pipeStringWear"));
  }

  cmds->popUpMenu.push_back(&this->separator);
  cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_listView_sortListViewByID"));
  cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_listView_sortListViewByName"));

  cmds->popUpMenu.push_back(&this->separator);
  cmds->popUpMenu.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_erase"));

  return cmds;
}
//----------------------------------------------------------------------------

bool FapUAResultListView::isHeaderOkAsLeaf(FFaListViewItem* item) const
{
  return dynamic_cast<FmGraph*>(item) ? true : false;
}
//----------------------------------------------------------------------------

void FapUAResultListView::startDrag(bool& accepted)
{
  // We only allow to drag curve objects from this UI
  accepted = true;
  std::vector<FFaViewItem*> items = this->getUISelectedItems();
  for (size_t i = 0; i < items.size() && accepted; i++)
    if (!dynamic_cast<FmCurveSet*>(items[i]))
      accepted = false;
}

//----------------------------------------------------------------------------

void FapUAResultListView::dropItems(int droppedOnItemIdx, int& dropAction)
{
  FFaViewItem* droppedOnItem = this->getMapItem(droppedOnItemIdx);

  // Get Graph or Subassembly (graph group) as dropped on item

  FmBase* subass = NULL;
  FmCurveSet* curve = NULL;
  FmGraph* graph = dynamic_cast<FmGraph*>(droppedOnItem);
  if (!graph)
  {
    if ((curve = dynamic_cast<FmCurveSet*>(droppedOnItem)))
      graph = curve->getOwnerGraph();
    else if (!(subass = dynamic_cast<FmSubAssembly*>(droppedOnItem)))
    {
      FmRingStart* rs = dynamic_cast<FmRingStart*>(droppedOnItem);
      if (rs && rs->getRingMemberType() == FmGraph::getClassTypeID())
        subass = rs->getParentAssembly();
    }
  }

  std::vector<FFaViewItem*> items = this->getUISelectedItems();
  if (!items.empty())
  {
    // Dropping selected curves from this UI (moving or copying them):

    bool isCopy = dropAction > 0;
    for (size_t i = 0; i < items.size() && dropAction >= 0; i++)
      if ((curve = dynamic_cast<FmCurveSet*>(items[i])))
      {
        FmGraph* oGraph = curve->getOwnerGraph();
        if (!graph) // Curves must be dropped on a graph
          dropAction = -4;
        else if (curve->usingInputMode() >= FmCurveSet::EXT_CURVE ||
                 graph->isBeamDiagram() == oGraph->isBeamDiagram())
        {
          if (isCopy)
          {
            FmCurveSet* newCurve = new FmCurveSet();
            newCurve->clone(curve,FmBase::DEEP_APPEND);
            newCurve->moveTo(graph);
            newCurve->reload();
            this->ensureItemVisible(newCurve);
          }
          else if (graph != oGraph)
            curve->moveTo(graph);
          else // Illegal move - onto same graph
            dropAction = -1;
        }
        else // Illegal copy or move - result curve onto different graph type
          dropAction = -2;
      }
      else if (!isCopy) // Only curves can be moved
        dropAction = -3;

    return;
  }

  // Dropping a result description from the RDB Selector UI:

  if (graph && graph->isBeamDiagram()) return;

  // There should only be one
  FapUARDBSelector* rdbSel = getUAInstance(FapUARDBSelector);
  if (!rdbSel || !rdbSel->isUIPoppedUp()) return;

  FFaResultDescription result = rdbSel->getSelectedResultDescr();
  int axis = rdbSel->getCurrentAxis();

  curve = new FmCurveSet();
  if (!graph)
  {
    // Dropped on empty space or on subassembly
    graph = new FmGraph();
    graph->setParentAssembly(subass);
    graph->connect();
  }
  graph->addCurveSet(curve);
  curve->setColor(graph->getCurveDefaultColor());
  curve->setResult(axis,result);
  curve->setResultOper(axis,FFaOpUtils::getDefaultOper(result.varRefType));
  curve->onDataChanged();

  this->ensureItemVisible(curve);
}
