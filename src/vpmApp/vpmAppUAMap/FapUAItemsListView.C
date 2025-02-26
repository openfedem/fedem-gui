// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iterator>
#include <algorithm>

#include "vpmApp/vpmAppUAMap/FapUAItemsListView.H"
#include "vpmApp/FapEventManager.H"
#include "vpmUI/vpmUIComponents/FuiItemsListView.H"

#include "FFuLib/FFuAuxClasses/FFuaIdentifiers.H"
#include "FFaLib/FFaDefinitions/FFaListViewItem.H"
#include "FFaLib/FFaString/FFaStringExt.H"


Fmd_SOURCE_INIT(FAPUAITEMSLISTVIEW, FapUAItemsListView, FapUAExistenceHandler);

//----------------------------------------------------------------------------

FapUAItemsListView::FapUAItemsListView(FuiItemsListView* uic)
  : FapUAExistenceHandler(uic), FapUAItemsViewHandler(uic), FapUACommandHandler(uic)
{
  Fmd_CONSTRUCTOR_INIT(FapUAItemsListView);

  this->ui = uic;

  this->automUpdateParentsPresence = false;
  this->leavesOnlySelectable = false;
  this->childrenVecCap = 200;
  this->topLevelItem = 0;
  this->freezeTopLevelItem = false;
  this->topLevelItemIncludeMyself = false;

  this->maintainSorting = false;
  this->sortMode = SORT_ID;

  this->ui->setTmpSelectItemCB(FFaDynCB1M(FapUAItemsListView,this,
					  tmpSelectItem,int));
  this->ui->setExpandItemCB(FFaDynCB2M(FapUAItemsListView,this,
				       expandItem,int,bool));
}
//----------------------------------------------------------------------------

void FapUAItemsListView::setTopLevelItem(FFaListViewItem* item, bool includeMe)
{
#ifdef LV_DEBUG
  reportItem(item,"FapUAItemsListView::setTopLevelItem: ");
#endif

  this->topLevelItem = item;
  this->topLevelItemIncludeMyself = includeMe;

  this->updateTopLevelItem();
}
//----------------------------------------------------------------------------

FFaListViewItem* FapUAItemsListView::getUIParent(FFaListViewItem* item) const
{
#ifdef LV_DEBUG
  reportItem(item,"FapUAItemsListView::getUIParent: ");
#endif

  return this->getMapLVItem(this->ui->getParent(this->getMapItem(item)));
}
//----------------------------------------------------------------------------

void FapUAItemsListView::createUITopLevelItems(FFaListViewItem* parent)
{
#ifdef LV_DEBUG
  reportItem(parent,"FapUAItemsListView::createUITopLevelItems: ");
#endif

  std::vector<FFaListViewItem*> items;
  items.reserve(this->childrenVecCap);

  this->getVerifiedChildren(parent,items);

  for (size_t i = 0; i < items.size(); i++) {
    items[i]->setPositionInListView(this->ui->getName(),i);
    this->createUIItem(items[i], NULL, i ? items[i-1] : NULL);
  }
}
//----------------------------------------------------------------------------

void FapUAItemsListView::createUIItem(FFaListViewItem* item,
				      FFaListViewItem* parent,
				      FFaListViewItem* after)
{
#ifdef LV_DEBUG
  reportItem(item,"FapUAItemsListView::createUIItem: ");
#endif

  if (!item) return;

  std::vector<FFaListViewItem*> children;
  this->getVerifiedChildren(item,children);

  this->createSingleUIItem(item,parent,after);

  for (size_t i = 0; i < children.size(); i++) {
    children[i]->setPositionInListView(this->ui->getName(),i);
    this->createUIItem(children[i], item, i ? children[i-1] : NULL);
  }
}
//----------------------------------------------------------------------------

void FapUAItemsListView::sortByName()
{
  sortMode = SORT_DESCR;
  this->updateSession();
}
//----------------------------------------------------------------------------

void FapUAItemsListView::sortByID()
{
  sortMode = SORT_ID;
  this->updateSession();
}
//----------------------------------------------------------------------------

void FapUAItemsListView::ensureSelectedVisible()
{
  std::vector<FFuListViewItem*> selected = ui->getSelectedListItems();
  for (FFuListViewItem* item : selected)
    ui->ensureListItemVisible(item,false);
}
//----------------------------------------------------------------------------

void FapUAItemsListView::deleteUIItem(FFaViewItem* item)
{
#ifdef LV_DEBUG
  reportItem(item,"FapUAItemsListView::deleteUIItem: ");
#endif

  int uiitem = this->getMapItem(item);
  if (uiitem < 0) return;

  std::vector<int> children = this->ui->getChildren(uiitem,true);

  this->ui->deleteItem(uiitem);
  this->eraseMapItem(uiitem);

  for (int child : children)
    this->eraseMapItem(child);
}
//----------------------------------------------------------------------------

void FapUAItemsListView::ensureItemVisible(FFaViewItem* item)
{
  this->ui->ensureItemVisible(this->getMapItem(item));
}
//----------------------------------------------------------------------------

void FapUAItemsListView::updateItemPositionsInDB(int uiparent)
{
  std::vector<int> children = this->ui->getChildren(uiparent);

  int pos = 0;
  for (int child : children)
    this->getMapLVItem(child)->setPositionInListView(this->ui->getName(),pos++);
}
//----------------------------------------------------------------------------

void FapUAItemsListView::tmpSelectItem(int item)
{
  if (!this->hasApplIndependentSelection)
    FapEventManager::tmpSelect(this->getMapItem(item));
}
//----------------------------------------------------------------------------

void FapUAItemsListView::expandItem(int item, bool open)
{
  FFaListViewItem* dbitem = this->getMapLVItem(item);
  dbitem->setExpandedInListView(this->ui->getName(),open);
}
//----------------------------------------------------------------------------

void FapUAItemsListView::tmpSelectionChangedEvent()
{
  if (this->shouldIUpdateOnChanges())
    this->updateUICommands(true,false);
}
//----------------------------------------------------------------------------

bool FapUAItemsListView::getItemExpanded(FFaListViewItem* item)
{
  return item->getExpandedInListView(this->ui->getName());
}
//----------------------------------------------------------------------------

std::string FapUAItemsListView::getItemText(FFaListViewItem* item)
{
  return item->getItemName() + FFaNumStr(" %d",item->getItemID());
}
//----------------------------------------------------------------------------

const char** FapUAItemsListView::getItemPixmap(FFaListViewItem* item)
{
  return item->getListViewPixmap();
}
//----------------------------------------------------------------------------

void FapUAItemsListView::updateSession()
{
#ifdef LV_DEBUG
  std::cout <<"\nFapUAItemsListView::updateSession()"<< std::endl;
#endif

  FapUAItemsViewHandler::updateSession();

  this->ui->setSglSelectionMode(this->singleSelectionMode());
  this->ui->setDecoratedRoot(this->getDecoratedRoot());
  this->ui->showHeader(this->getShowHeader());
  this->ui->enableHeaderClick(this->getHeaderClickEnabling());

  if (!this->freezeTopLevelItem) {
    this->clearSession();
    this->createUITopLevelItems(NULL);
    if (this->leavesOnlySelectable)
      this->updateLeavesOnlySelectable();

    this->permTotSelectUIItems(FapEventManager::getPermSelection());
  }
  else
    this->updateTopLevelItem();
}
//----------------------------------------------------------------------------

void FapUAItemsListView::updateTopLevelItem()
{
  this->clearSession();

#ifdef LV_DEBUG
  std::cout <<"\nFapUAItemsListView::updateTopLevelItem()"<< std::endl;
#endif

  if (this->topLevelItem) {
    if (this->topLevelItemIncludeMyself) {
      if (this->verifyItem(this->topLevelItem))
        this->createUIItem(this->topLevelItem,NULL,NULL);
    }
    else
      this->createUITopLevelItems(this->topLevelItem);

    if (this->leavesOnlySelectable)
      this->updateLeavesOnlySelectable();

    this->permTotSelectUIItems(FapEventManager::getPermSelection());
  }
}
//----------------------------------------------------------------------------

FFaListViewItem* FapUAItemsListView::getMapLVItem(int item) const
{
  return (FFaListViewItem*)const_cast<FapUAItemsListView*>(this)->getMapItem(item);
}
//----------------------------------------------------------------------------

void FapUAItemsListView::getVerifiedChildren(FFaListViewItem* parent,
					     std::vector<FFaListViewItem*>& items)
{
#ifdef LV_DEBUG
  reportItem(parent,"FapUAItemsListView::getVerifiedChildren: ");
#endif
  this->getChildren(parent,items);

  std::vector<FFaListViewItem*>::iterator it = items.begin();
  while (it != items.end())
    if (this->verifyItem(*it))
      ++it;
    else
      it = items.erase(it);

  this->sortItems(items);

#ifdef LV_DEBUG
  std::cout <<"================================"<< std::endl;
  reportItem(parent,"Sorted children of: ");
  for (FFaListViewItem* item : items) reportItem(item,"\t");
  std::cout <<"================================"<< std::endl;
#endif
}
//----------------------------------------------------------------------------

bool FapUAItemsListView::verifyItem(FFaListViewItem* item)
{
  bool valid = true;
  this->verifyItemCB.invoke(item,valid);
#if LV_DEBUG > 2
  std::cout <<"FapUAItemsListView::verifyItem:"<< (valid ? "T ":"F ");
  reportItem(item);
#endif
  return valid;
}
//----------------------------------------------------------------------------

int FapUAItemsListView::createSingleUIItem(FFaListViewItem* item,
					   FFaListViewItem* parent,
					   FFaListViewItem* after)
{
#ifdef LV_DEBUG
  reportItem(item,"FapUAItemsListView::createSingleUIItem: ");
#endif

  if (this->getMapItem(item) > -1) return -1;

  int uiitem = this->ui->createItem(this->getMapItem(parent),
                                    this->getMapItem(after),
                                    this->getItemText(item));
  if (uiitem < 0) return uiitem;

  this->putMapItem(uiitem,item);

  //settings
  if (!this->getItemSelectAble(item))
    this->ui->setItemSelectAble(uiitem,false);

  if (this->getItemExpanded(item))
    this->ui->expandItem(uiitem,true);

  //text font
  if (this->getItemTextBold(item))
    this->ui->setItemTextBold(uiitem,true);
  if (this->getItemTextItalic(item))
    this->ui->setItemTextItalic(uiitem,true);

  //pxmap
  const char** px = this->getItemPixmap(item);
  if (px) this->ui->setItemImage(uiitem,px);

  //toggle
  if (this->getItemToggleAble(item)) {
    this->ui->setItemToggleAble(uiitem,true);
    if (this->getItemThreeStepToggleAble(item))
      this->ui->setItemThreeStepToggleAble(uiitem,true);
    this->ui->setItemToggleValue(uiitem,this->getItemToggleValue(item));
  }

#ifdef LV_DEBUG
  std::cout <<"FapUAItemsListView::createSingleUIItem: Created item "
            << uiitem <<" for "<< this->getItemText(item) << std::endl;
#endif

  return uiitem;
}
//----------------------------------------------------------------------------

FFaListViewItem* FapUAItemsListView::getItemBefore(FFaListViewItem* item,
						   int itemsUIParent) const
{
#ifdef LV_DEBUG
  reportItem(item,"FapUAItemsListView::getItemBefore: ");
#endif

  std::vector<int> children = this->ui->getChildren(itemsUIParent);
  if (children.empty()) return NULL;

  // If not maintaining sorting, return last of siblings
  FFaListViewItem* itemBefore = NULL;
  if (!this->maintainSorting)
  {
    itemBefore = this->getMapLVItem(children.back());
#ifdef LV_DEBUG
    reportItem(itemBefore,"                     (unsorted) -> ");
#endif
    return itemBefore;
  }

  size_t i;
  std::vector<FFaListViewItem*> lvChildren;
  for (i = 0; i < children.size(); i++)
    lvChildren.push_back(this->getMapLVItem(children[i]));

  // If item not already in vector, insert it
  if (find(lvChildren.begin(), lvChildren.end(), item) == lvChildren.end())
    lvChildren.push_back(item);

  // Sorting item and its siblings
  this->sortItems(lvChildren);

  // Looping over siblings to find item
  for (i = 1; i < lvChildren.size() && !itemBefore; i++)
    if (item == lvChildren[i])
      itemBefore = lvChildren[i-1];

#ifdef LV_DEBUG
  reportItem(itemBefore,"                       (sorted) -> ");
#endif
  return itemBefore;
}
//----------------------------------------------------------------------------

void FapUAItemsListView::updateLeavesOnlySelectable()
{
#ifdef LV_DEBUG
  std::cout <<"FapUAItemsListView::updateLeavesOnlySelectable()"<< std::endl;
#endif

  std::map<int,FFaViewItem*>::iterator it;
  for (it = this->intMap.begin(); it != this->intMap.end(); ++it)
    if (!this->leavesOnlySelectable)
      this->ui->setItemSelectAble(it->first,true);
    else if (this->ui->getNChildren(it->first))
      this->ui->setItemSelectAble(it->first,false); // not leaf node
    else
      this->ui->setItemSelectAble(it->first,true); // leaf node
}
//----------------------------------------------------------------------------

void FapUAItemsListView::onListViewItemConnected(FFaListViewItem* item, bool doVerify)
{
  if (!this->shouldIUpdateOnChanges()) return;

#ifdef LV_DEBUG
  reportItem(item,"\nFapUAItemsListView::onListViewItemConnected: ");
#endif

  if (this->getMapItem(item) > -1) {
    std::cout <<"*** WARNING: "<< this->ui->getName()
	      <<" FapUAItemsListView::onListViewItemConnected\n";
    reportItem(item,"             Item not created, it's already there: ");
    return;
  }

  if (doVerify)
    if (!this->verifyItem(item)) return;

  std::vector<int> assID;
  item->getItemAssemblyPath(assID);
  FFaListViewItem* itemsparent = this->getParent(item,assID);

  if (this->freezeTopLevelItem) {
    // See if its top level parent already is in lv, if not forget it
    FFaListViewItem* parent = itemsparent;
    for (; parent; parent = this->getParent(parent,assID))
      if (this->getMapItem(parent) > -1)
	break;
      else
	parent->getItemAssemblyPath(assID);

    if (!parent) return;
  }

  // itemsparent is not yet in LV
  int itemsuiparent = this->getMapItem(itemsparent);
  if (this->automUpdateParentsPresence && itemsparent && itemsuiparent == -1) {
    this->onListViewItemConnected(itemsparent,false);
    itemsuiparent = this->getMapItem(itemsparent);
  }

  FFaListViewItem* after = this->getItemBefore(item,itemsuiparent);
  this->createSingleUIItem(item,itemsparent,after);

  if (item->isSubjectToLVPositioning(this->ui->getName()))
    this->updateItemPositionsInDB(itemsuiparent);
}
//----------------------------------------------------------------------------

void FapUAItemsListView::onListViewItemDisconnected(FFaListViewItem* item)
{
  if (!this->shouldIUpdateOnChanges()) return;

#ifdef LV_DEBUG
  reportItem(item,"FapUAItemsListView::onListViewItemDisconnected: ");
#endif

  int uiitem = this->getMapItem(item);
  if (uiitem < 0) return;

  // collapse before disconnecting, since thats default
  item->setExpandedInListView(this->ui->getName(),false);

  int uiparent = this->ui->getParent(uiitem);
  bool hasPosition = (item->getPositionInListView(this->ui->getName()) > -1);

  // About to delete last sibling, remove parent also if it is ok

  if (this->automUpdateParentsPresence)
    if (uiparent >= 0 && this->ui->getNSiblings(uiitem) == 1 &&
	!this->isHeaderOkAsLeaf(this->getMapLVItem(uiparent)))
      this->onListViewItemDisconnected(this->getMapLVItem(uiparent));

  this->deleteUIItem(item);

  if (item->isSubjectToLVPositioning(this->ui->getName()) && hasPosition)
    this->updateItemPositionsInDB(uiparent);
}
//----------------------------------------------------------------------------

void FapUAItemsListView::onListViewItemChanged(FFaListViewItem* item)
{
  if (!this->shouldIUpdateOnChanges()) return;

#ifdef LV_DEBUG
  reportItem(item,"FapUAItemsListView::onListViewItemChanged: ");
#endif

  int uiitem = this->getMapItem(item);
  if (uiitem < 0) return;

  this->ui->setItemText(uiitem,this->getItemText(item));

  if (!this->maintainSorting) return;

  std::vector<int> assID;
  item->getItemAssemblyPath(assID);
  FFaListViewItem* parent = this->getParent(item,assID);
  FFaListViewItem* after = this->getItemBefore(item,this->getMapItem(parent));

  // SKE: Doing a brute force move (delete and create)
  // because Fui.. move method is buggy and causes crash
  // TODO: Don't move unless it really should move

  this->deleteUIItem(item);
  this->createUIItem(item, parent, after);

  // Handle selection, since this will now be removed
  std::vector<FFaViewItem*> selection = FapEventManager::getPermSelection();
  for (FFaViewItem* permSel : selection)
    this->ui->permSelectItem(this->getMapItem(permSel));
}
//----------------------------------------------------------------------------

void FapUAItemsListView::sortItems(std::vector<FFaListViewItem*>& v) const
{
  if (this->sortMode == SORT_ID)
    std::sort(v.begin(), v.end(), FFaViewItem::compareID);
  else if (this->sortMode == SORT_DESCR)
    std::sort(v.begin(), v.end(), FFaViewItem::compareDescr);
}
//----------------------------------------------------------------------------

void FapUAItemsListView::reportItem(FFaViewItem* item, const char* prefix)
{
  if (prefix) std::cout << prefix;

  if (item) {
    std::cout << item->getItemName();
    if (item->getItemID() > 0)
      std::cout <<" ["<< item->getItemID() <<"]";
    else
      std::cout <<" {"<< item->getItemBaseID() <<"}";
    std::vector<int> assID;
    item->getItemAssemblyPath(assID);
    if (!assID.empty())
    {
      std::cout <<" AssemblyID=";
      if (assID.size() == 1)
	std::cout << assID.front();
      else
	std::copy(assID.begin(),assID.end(),std::ostream_iterator<int>(std::cout,","));
    }
    std::string descr = item->getItemDescr();
    if (!descr.empty())
      std::cout <<" \""<< descr <<"\"";
  }
  else
    std::cout <<"NULL";

  std::cout << std::endl;
}
