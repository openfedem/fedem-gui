// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiTopologyView.H"
#include "FFuLib/FFuListView.H"
#include "FFuLib/FFuListViewItem.H"
#include "FFuLib/FFuFrame.H"
#include "FFuLib/FFuPopUpMenu.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"

// Global variable
FuiTopologyView* FuiTopologyView::topologyView = NULL;


FuiTopologyView::FuiTopologyView()
{
  IAmAutoUnHighlighting = true;
  topologyView = this;
}

FuiTopologyView::~FuiTopologyView()
{
  topologyView = NULL;
}

void FuiTopologyView::initWidgets()
{
  myView->setListColumns({"Topology","Id","Description"});
  myView->setListRootIsDecorated(true);
  myView->setAllListColumnsShowSelection(true);
  myView->setHeaderClickEnabled(-1,true);
  myView->setHeaderOff(true);

  myView->setPermSelectionChangedCB(FFaDynCB0M(FuiTopologyView,this,
                                               onMyViewSelectionChanged));
  myView->setLeftMouseBReleaseCB(FFaDynCB0M(FuiTopologyView,this,
                                            onMyViewMouseReleased));
  myView->setReturnPressedCB(FFaDynCB1M(FuiTopologyView,this,
                                        onMyViewActivated,
                                        FFuListViewItem*));
  myView->setDoubleClickedCB(FFaDynCB1M(FuiTopologyView,this,
                                        onMyViewActivated,
                                        FFuListViewItem*));
  myView->setRightMouseBPressCB(FFaDynCB1M(FuiTopologyView,this,
                                           onMyViewRMBPressed,
                                           FFuListViewItem*));
  myView->setMenuItemSelectedCB(FFaDynCB2M(FuiTopologyView,this,
                                           onMyViewPopUpSelected,
                                           const std::vector<FFuListViewItem*>&,
                                           FFuaCmdItem*));
}

void FuiTopologyView::placeWidgets(int width, int height)
{
  myFrame->setEdgeGeometry(0,width,0,height);
  myView->setEdgeGeometry(0,width,0,height);
}

void FuiTopologyView::setUIValues(const FFuaUIValues* values)
{
  const FuaTopologyValues* topValues = dynamic_cast<const FuaTopologyValues*>(values);
  if (topValues)
    this->setTree(topValues->topology);
}


void FuiTopologyView::setTree(const std::vector<FuiTopologyItem> &topology)
{
  FFuListViewItem* parent;
  FFuListViewItem* after;
  FFuListViewItem* newItem = NULL;

  myView->clearList();

  int newDepth = 0, id = 0;
  for (const FuiTopologyItem& top : topology)
  {
    parent = after = newItem;
    for (int depth = newDepth; depth >= top.depth; depth--)
      if (parent)
      {
        after  = parent;
        parent = parent->getParentItem();
      }

    newItem = myView->createListItem(top.type.c_str(),parent,after);
    newItem->setItemId(id++);
    newItem->setItemText(1,top.id.c_str());
    newItem->setItemText(2,top.description.c_str());
    myView->openListItem(newItem,true);

    newDepth = newItem->getDepth();
  }

  myView->resizeListColumnsToContents();
}

FuiTopologyView* FuiTopologyView::getTopologyView()
{
  return topologyView;
}

FFuListView* FuiTopologyView::getListView()
{
  return myView;
}

void FuiTopologyView::setHighlightedCB(const FFaDynCB2<int, bool> &aDynCB)
{
  myHighlightedCB = aDynCB;
}

void FuiTopologyView::setActivatedCB(const FFaDynCB1<int> &aDynCB)
{
  myActivatedCB = aDynCB;
}

void FuiTopologyView::setBuildPopUpCB(const FFaDynCB2<const std::vector<int>&,std::vector<FFuaCmdItem*>&>& aDynCB)
{
  myBuildPopUpCB = aDynCB;
}

void FuiTopologyView::setPopUpSelectedCB(const FFaDynCB2<const std::vector<int>&,FFuaCmdItem*>& aDynCB)
{
  myPopUpSelectedCB = aDynCB;
}


void FuiTopologyView::onMyViewSelectionChanged()
{
  std::vector<FFuListViewItem*> items = myView->getSelectedListItems();

  static FFuListViewItem* previousItemSelected = NULL;
  if (!items.empty()) {
    // Remember to dehighlight if user is dragselecting
    // Then there comes no selection changed on item deselected.
    if (previousItemSelected)
      myHighlightedCB.invoke(previousItemSelected->getItemId(),false);

    previousItemSelected = items.front();
    myHighlightedCB.invoke(previousItemSelected->getItemId(),true);
  }
  else if (previousItemSelected) {
    myHighlightedCB.invoke(previousItemSelected->getItemId(),false);
    previousItemSelected = NULL;
  }
}


void FuiTopologyView::onMyViewMouseReleased()
{
  if (IAmAutoUnHighlighting) {
    std::vector<FFuListViewItem*> selection = myView->getSelectedListItems();
    if (!selection.empty())
      myView->permSelectListItem(selection.front(),false,true);
  }
}

void FuiTopologyView::onMyViewActivated(FFuListViewItem * item)
{
  if (item)
    myActivatedCB.invoke(item->getItemId());
}


void FuiTopologyView::onMyViewRMBPressed(FFuListViewItem * item)
{
  //vector<FFuListViewItem *> selection = myView->getSelectedListItems();
  std::vector<int> selectedIDs;
  std::vector<FFuaCmdItem*> commands;

  FFuPopUpMenu * Popup = myView->getPopUpMenu();

  if(item)
    selectedIDs.push_back(item->getItemId());

  //selectedIDs.reserve(selection.size());
  //for (FFuListViewItem* selitem : selection)
  //  selectedIDs.push_back(selitem->getItemId());

  myBuildPopUpCB.invoke(selectedIDs,commands);

  Popup->deleteItems();
  for (FFuaCmdItem* cmd : commands)
    Popup->insertCmdItem(cmd);

  //Popup->insertCmdItem(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_zoomTo"));
  //myView->tmpSelectListItem(item,false);

  //Popup->deleteItems();
  //FapEventManager::tmpSelect(0);
}

void FuiTopologyView::onMyViewPopUpSelected(const std::vector<FFuListViewItem*> &selection,
                                            FFuaCmdItem* command)
{
  std::vector<int> selectedIDs;
  selectedIDs.reserve(selection.size());
  for (FFuListViewItem* item : selection)
    selectedIDs.push_back(item->getItemId());

  myPopUpSelectedCB.invoke(selectedIDs,command);
}

void FuiTopologyView::onPoppedUp()
{
  // myPoppedUpCD.invoke();
  this->updateUIValues();
}
