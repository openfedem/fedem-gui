// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiTopologyView.H"
#include "FFuLib/FFuListView.H"
#include "FFuLib/FFuListViewItem.H"
#include "FFuLib/FFuPopUpMenu.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"


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


void FuiTopologyView::setTree(const FuiTopologyItems& topology)
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


void FuiTopologyView::setHighlightedCB(const FFaDynCB2<int,bool>& aDynCB)
{
  myHighlightedCB = aDynCB;
}

void FuiTopologyView::setActivatedCB(const FFaDynCB1<int>& aDynCB)
{
  myActivatedCB = aDynCB;
}

void FuiTopologyView::setBuildPopUpCB(const FFaDynCB2<int,FFuCmdItems&>& aDynCB)
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
  if (!items.empty()) {
    // Remember to dehighlight if user is dragselecting
    // Then there comes no selection changed on item deselected.
    if (previousSelected)
      myHighlightedCB.invoke(previousSelected->getItemId(),false);

    previousSelected = items.front();
    myHighlightedCB.invoke(previousSelected->getItemId(),true);
  }
  else if (previousSelected) {
    myHighlightedCB.invoke(previousSelected->getItemId(),false);
    previousSelected = NULL;
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

void FuiTopologyView::onMyViewActivated(FFuListViewItem* item)
{
  if (item)
    myActivatedCB.invoke(item->getItemId());
}


void FuiTopologyView::onMyViewRMBPressed(FFuListViewItem* item)
{
  FFuCmdItems commands;
  if (item)
    myBuildPopUpCB.invoke(item->getItemId(),commands);

  FFuPopUpMenu* menu = myView->getPopUpMenu();
  menu->deleteItems();
  for (FFuaCmdItem* cmd : commands)
    menu->insertCmdItem(cmd);
}


void FuiTopologyView::onMyViewPopUpSelected(const std::vector<FFuListViewItem*>& selection,
                                            FFuaCmdItem* command)
{
  std::vector<int> selectedIDs;
  selectedIDs.reserve(selection.size());
  for (FFuListViewItem* item : selection)
    selectedIDs.push_back(item->getItemId());

  myPopUpSelectedCB.invoke(selectedIDs,command);
}
