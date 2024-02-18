// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "FFuLib/FFuListView.H"
#include "FFuLib/FFuListViewItem.H"
#include "FFuLib/FFuPopUpMenu.H"
#include "FFuLib/FFuAuxClasses/FFuaIdentifiers.H"

//----------------------------------------------------------------------------

FFuListView::FFuListView()
{
  this->popUpMenu = NULL;
  this->tmpSelected = NULL;
  this->ensureVisOnExpansion = true;
}
//----------------------------------------------------------------------------

void FFuListView::permTotSelectListItems(const std::vector<FFuListViewItem*>& totalSel, bool notify)
{
  this->clearListSelection(notify);
  
  if (this->isSglSelectionMode() && totalSel.size() > 1)
    std::cerr <<"WARNING - FFuListView::permTotSelectListItems: You are trying to select "
              << totalSel.size() <<" items in single-selection mode"<< std::endl;

  if (this->tmpSelected)
    this->permSelectedDuringTmpSelection = totalSel;

  for (FFuListViewItem* item : totalSel)
    this->permSelectListItem(item,true,notify);
}
//----------------------------------------------------------------------------

std::vector<FFuListViewItem*> FFuListView::getSelectedListItems()
{
  std::vector<FFuListViewItem*> items;
  for (const std::pair<const int,FFuListViewItem*>& lvi : this->lviMap)
    if (lvi.second->isItemSelected())
      items.push_back(lvi.second);

  return items;
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuListView::getListItemBefore(FFuListViewItem* itemsParent,int itemsListPosition)
{
  if (itemsListPosition < 1) return NULL;
    
  FFuListViewItem* item;
  FFuListViewItem* last = NULL;

  if (itemsParent)
    item = itemsParent->getFirstChildItem();
  else 
    item = this->getFirstChildItem();

  for (int pos = 0; item; pos++)
    if (pos+1 == itemsListPosition)
      return item;
    else {
      last = item;
      item = item->getNextSiblingItem();
    }

  return last;
}
//----------------------------------------------------------------------------

std::vector<FFuListViewItem*> FFuListView::getListChildren(FFuListViewItem* parent)
{
  std::vector<FFuListViewItem*> items;
  FFuListViewItem* item;

  if (parent)
    item = parent->getFirstChildItem();
  else 
    item = this->getFirstChildItem();

  while (item) {
    items.push_back(item);
    item = item->getNextSiblingItem();
  }
  
  return items;
}
//----------------------------------------------------------------------------

std::vector<FFuListViewItem*> FFuListView::getAllListChildren(FFuListViewItem* parent)
{
  std::vector<FFuListViewItem*> items;

  if (!parent)
    for (const std::pair<const int,FFuListViewItem*>& lvi : this->lviMap)
      items.push_back(lvi.second);
  else // traversing the tree
    for (FFuListViewItem* item = parent->getFirstChildItem(); item;) {
      items.push_back(item);
      if (item->getFirstChildItem())
	item = item->getFirstChildItem();
      else if (item->getNextSiblingItem())
	item = item->getNextSiblingItem();
      else { 
	while ((item=item->getParentItem()) &&
	       item != parent &&
	       !item->getNextSiblingItem());
	if (item && item != parent)
	  item = item->getNextSiblingItem();
	else
	  item = NULL;
      }
    }

  return items;
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuListView::getListItem(int itemId)
{
  if (itemId < 0) return NULL;

  std::map<int,FFuListViewItem*>::const_iterator it = this->lviMap.find(itemId);
  return it == this->lviMap.end() ? NULL : it->second;
}
//----------------------------------------------------------------------------

std::vector<FFuListViewItem*> FFuListView::arePresent(const std::vector<FFuListViewItem*>& in)
{
  std::vector<FFuListViewItem*> present;

  for (FFuListViewItem* item : in)
    for (const std::pair<const int,FFuListViewItem*>& lvi : this->lviMap)
      if (item == lvi.second) {
	present.push_back(item);
	break;
      }

  return present;
}
//----------------------------------------------------------------------------

void FFuListView::deleteListItem(FFuListViewItem* item)
{
  if (!item) return;

  bool wasblocked = this->areLibSignalsBlocked();
  this->blockLibSignals(true);

  // Erase item and all item's children from the map
  for (FFuListViewItem* child : this->getAllListChildren(item))
    this->lviMap.erase(child->getItemId());
  this->lviMap.erase(item->getItemId());

  delete item; // takes also all children 

  this->blockLibSignals(wasblocked);
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuListView::copyListItem(FFuListViewItem* itemToCopy,FFuListViewItem* copyToParent,
					   FFuListViewItem* copyToAfter,bool copyIndices)
{
  FFuListViewItem *item, *newTopItem, *newItem, *newParent, *newAfter;

  newItem = this->createListItem(copyToParent,copyToAfter,itemToCopy);
  // copy index
  if (copyIndices)
    newItem->setItemId(itemToCopy->getItemId());

  newTopItem = newItem;
  newParent = newItem;
  newAfter = NULL;
  
  //traversing the tree
  item = itemToCopy->getFirstChildItem();
  while (item) {
    // creating new item
    newItem = this->createListItem(newParent,newAfter,item);
    // copy index
    if (copyIndices)
      newItem->setItemId(item->getItemId());

    if (item->getFirstChildItem()) {
      newParent = newItem;
      newAfter = NULL;
      item = item->getFirstChildItem();
    }
    else if (item->getNextSiblingItem()) {
      newAfter = newItem;
      item = item->getNextSiblingItem();
    }
    else { 
      //rewind upwards till we find a sibling or we are itemToCopy
      do {
	item = item->getParentItem();
	newItem = newItem->getParentItem();
      } while (item && item != itemToCopy && !item->getNextSiblingItem());
      if (item == itemToCopy)
        break; // we are itemToCopy
      else if (item) {
	//first sibling on the way up
	newParent = newItem->getParentItem();
	newAfter = newItem;
	item = item->getNextSiblingItem();
      }
    }
  }
  return newTopItem;
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuListView::moveListItem(FFuListViewItem* itemToMove,FFuListViewItem* moveToParent,
					   FFuListViewItem* moveToAfter)
{
  FFuListViewItem* newTopItem = this->copyListItem(itemToMove,moveToParent,moveToAfter,true);
  if (!newTopItem) return NULL;

  this->deleteListItem(itemToMove);
  return newTopItem;
}
//----------------------------------------------------------------------------

void FFuListView::onPermSelectionChanged()
{ 
  this->permSelectionChangedEvent();
  this->invokePermSelectionChangedCB();
}
//----------------------------------------------------------------------------

void FFuListView::onTmpSelectionChanged(FFuListViewItem* listItem)
{ 
  this->tmpSelectionChangedEvent(listItem);
  this->invokeTmpSelectionChangedCB(listItem);
}
//----------------------------------------------------------------------------

void FFuListView::onMenuItemSelected(const std::vector<FFuListViewItem*>& listItems,
				     FFuaCmdItem* menuItem)
{
  this->menuItemSelectedEvent(listItems,menuItem);
  this->invokeMenuItemSelectedCB(listItems,menuItem);
}
//----------------------------------------------------------------------------

void FFuListView::onListItemOpened(FFuListViewItem* listItem,bool open)
{
  if (this->ensureVisOnExpansion && open) {
    //her kommer kode naar qt klarer dette bedre
  }
  
  this->listItemOpenedEvent(listItem,open); 
  this->invokeItemExpandedCB(listItem,open); 
}
//----------------------------------------------------------------------------

void FFuListView::onListItemToggled(FFuListViewItem* listItem,int toggle)
{
  this->listItemToggledEvent(listItem,toggle);
  this->invokeItemToggledCB(listItem,toggle); 
}
//----------------------------------------------------------------------------

void FFuListView::executePopUp(FFuListViewItem* rMBItem)
{
  FFuaCmdItem* id = NULL;
  std::vector<FFuListViewItem*> realSelected = this->getSelectedListItems();
  if (rMBItem) {
    if (rMBItem->isItemSelected()) {
      this->rightMousePressed(rMBItem);
      if (!this->popUpMenu->isMenuEmpty())
	id = this->popUpMenu->executeAtCursorPos();
      if (id && !this->menuItemSelectedCB.empty())
	this->onMenuItemSelected(this->arePresent(realSelected),id);
    }
    else
      this->tmpSelectListItem(rMBItem,false);
  }
  else { // nonitem pick gives an empty permsel change from this lv
    this->clearListSelection(false);
    this->onPermSelectionChanged();
    // Build menu
    this->rightMousePressed(rMBItem);
    if (!this->popUpMenu->isMenuEmpty())
      id = this->popUpMenu->executeAtCursorPos();
    if (id && !this->menuItemSelectedCB.empty())
      this->onMenuItemSelected(std::vector<FFuListViewItem*>(),id);
  }
}
//----------------------------------------------------------------------------

void FFuListView::tmpSelectListItem(FFuListViewItem* item, bool)
{
  if (!item || item == this->tmpSelected)
    return;

  std::vector<FFuListViewItem*> realSelected = this->getSelectedListItems();

  this->onTmpSelectionChanged(item);
  this->permTotSelectListItems({item});
  //TODO mark realSelected in the viewport

  this->tmpSelected = item;

  // Build menu :  
  this->rightMousePressed(item);

  FFuaCmdItem* id = NULL;
  if (!this->popUpMenu->isMenuEmpty())
    id = this->popUpMenu->executeAtCursorPos();// may give a realselected change
  
  this->tmpSelected = NULL;
  this->onTmpSelectionChanged(NULL);

  if (!permSelectedDuringTmpSelection.empty()) {
    this->permTotSelectListItems(this->arePresent(permSelectedDuringTmpSelection));
    this->permSelectedDuringTmpSelection.clear();
  }
  else
    this->permTotSelectListItems(this->arePresent(realSelected));

  if (id && !this->menuItemSelectedCB.empty())
    this->onMenuItemSelected(this->arePresent({item}),id);
}
//----------------------------------------------------------------------------

//NOTES

//qtbug right buttom pressed: qt gives a selection if right m buttom is pressed 
//at the same time as the cursor is moved

//QListViewItem:setOpen is reimpl in order to notify expansions since qt 
//doesnt have a signal for such an event. But one should be careful about what
//one do on that event since qt might not be prepared to handle everything in
//the state it is under during setOpen. 
