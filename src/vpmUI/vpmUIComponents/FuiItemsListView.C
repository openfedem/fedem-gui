// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiItemsListView.H"
#include "FFuLib/FFuListViewItem.H"
#include "FFuLib/FFuPopUpMenu.H"


bool FuiItemsListView::mouseSelected = false;


int FuiItemsListView::createItem(int parent, int after, const char* label)
{
  return this->createListItem(this->getListItem(parent),this->getListItem(after),NULL,label)->getItemId();
}
//----------------------------------------------------------------------------

void FuiItemsListView::deleteItem(int item)
{
  this->deleteListItem(this->getListItem(item));
}
//----------------------------------------------------------------------------

void FuiItemsListView::deleteItems()
{
  this->clearList();
}
//----------------------------------------------------------------------------

void FuiItemsListView::expandItem(int item, bool expand)
{
  this->openListItem(this->getListItem(item),expand,false);
}
//----------------------------------------------------------------------------

void FuiItemsListView::ensureItemVisible(int item)
{
  this->ensureListItemVisible(this->getListItem(item),true);
}
//----------------------------------------------------------------------------

void FuiItemsListView::setItemSelectAble(int item, bool able)
{
  this->getListItem(item)->setItemSelectable(able);
}
//----------------------------------------------------------------------------

void FuiItemsListView::setItemText(int item, const std::vector<std::string>& texts)
{
  FFuListViewItem* ffuitem = this->getListItem(item);
  for (size_t i = 0; i < texts.size(); i++)
    ffuitem->setItemText(i,texts[i].c_str());
}
//----------------------------------------------------------------------------

void FuiItemsListView::setItemTextBold(int item, bool bold)
{
  this->getListItem(item)->setItemTextBold(0,bold);
}
//----------------------------------------------------------------------------

void FuiItemsListView::setItemTextItalic(int item, bool italic)
{
  this->getListItem(item)->setItemTextItalic(0,italic);
}
//----------------------------------------------------------------------------

void FuiItemsListView::setItemImage(int item, const char** pixmap)
{
  this->getListItem(item)->setItemImage(0,pixmap);
}
//----------------------------------------------------------------------------

void FuiItemsListView::setItemToggleAble(int item, bool able)
{
  this->getListItem(item)->setItemToggleAble(able);
}
//----------------------------------------------------------------------------

void FuiItemsListView::setItemThreeStepToggleAble(int item, bool able)
{
  this->getListItem(item)->setItemThreeStepToggleAble(able);
}
//----------------------------------------------------------------------------

void FuiItemsListView::setItemToggleValue(int item, int value)
{
  this->getListItem(item)->setToggleValue(value,false);
}
//----------------------------------------------------------------------------

int FuiItemsListView::getItemToggleValue(int item)
{
  return this->getListItem(item)->getToggleValue();
}
//----------------------------------------------------------------------------

void FuiItemsListView::setItemDropable(int item, bool yesOrNo)
{
  this->getListItem(item)->setItemDropable(yesOrNo);
}
//----------------------------------------------------------------------------

void FuiItemsListView::setItemDragable(int item, bool yesOrNo)
{
  this->getListItem(item)->setItemDragable(yesOrNo);
}
//----------------------------------------------------------------------------

int FuiItemsListView::getItemPosition(int item)
{
  return this->getListItem(item)->getItemPosition();
}
//----------------------------------------------------------------------------

int FuiItemsListView::getItemBefore(int itemsParent, int itemsListPosition)
{
  FFuListViewItem* before = this->getListItemBefore(this->getListItem(itemsParent),
						    itemsListPosition);
  return before ? before->getItemId() : -1;
}
//----------------------------------------------------------------------------

int FuiItemsListView::getParent(int item)
{
  FFuListViewItem* parent = this->getListItem(item)->getParentItem();
  return parent ? parent->getItemId() : -1;
}
//----------------------------------------------------------------------------

std::vector<int> FuiItemsListView::getChildren(int parent, bool all)
{
  std::vector<int> ids;
  FFuListViewItem* pItem = this->getListItem(parent);
  if (parent > -1 && !pItem)
    return ids;

  std::vector<FFuListViewItem*> items = all ? this->getAllListChildren(pItem) : this->getListChildren(pItem);
  ids.reserve(items.size());

  for (FFuListViewItem* item : items)
    ids.push_back(item->getItemId());

  return ids;
}
//----------------------------------------------------------------------------

int FuiItemsListView::getNSiblings(int item)
{
  return this->getListItem(item)->getNSiblings();
}
//----------------------------------------------------------------------------

int FuiItemsListView::getNChildren(int item)
{
  return this->getListItem(item)->getNChildren();
}
//----------------------------------------------------------------------------

void FuiItemsListView::showHeader(bool show)
{
  this->setHeaderOff(!show);
}
//----------------------------------------------------------------------------

void FuiItemsListView::enableHeaderClick(bool enable)
{
  this->setHeaderClickEnabled(-1,enable);
}
//----------------------------------------------------------------------------

void FuiItemsListView::setDecoratedRoot(bool decorated)
{
  this->setListRootIsDecorated(decorated);
}
//----------------------------------------------------------------------------

void FuiItemsListView::permTotSelectItems(const std::vector<int>& totalSelection)
{
  std::vector<FFuListViewItem*> items;
  items.reserve(totalSelection.size());

  for (int id : totalSelection)
    items.push_back(this->getListItem(id));

  this->permTotSelectListItems(items); // no notify
}
//----------------------------------------------------------------------------

std::vector<int> FuiItemsListView::getSelectedItems()
{
  std::vector<FFuListViewItem*> items = this->getSelectedListItems();

  std::vector<int> ids;
  ids.reserve(items.size());

  for (FFuListViewItem* item : items)
    ids.push_back(item->getItemId());

  return ids;
}
//----------------------------------------------------------------------------

bool FuiItemsListView::isItemSelected(int item)
{
  return this->getListItem(item)->isItemSelected();
}
//----------------------------------------------------------------------------

void FuiItemsListView::setUICommands(const FFuaUICommands* commands)
{
  this->popUpMenu->deleteItems();
  for (FFuaCmdItem* cmd : static_cast<const FuaItemsLVCommands*>(commands)->popUpMenu)
    this->popUpMenu->insertCmdItem(cmd);
}
//----------------------------------------------------------------------------

void FuiItemsListView::updateUICommandsSensitivity(const FFuaUICommands* commands)
{
  for (FFuaCmdItem* cmd : static_cast<const FuaItemsLVCommands*>(commands)->popUpMenu)
    this->popUpMenu->updateCmdItem(cmd,true);
}
//----------------------------------------------------------------------------

void FuiItemsListView::updateUICommandsToggle(const FFuaUICommands* commands)
{
  for (FFuaCmdItem* cmd : static_cast<const FuaItemsLVCommands*>(commands)->popUpMenu)
    this->popUpMenu->updateCmdItem(cmd,false);
}
//----------------------------------------------------------------------------

void FuiItemsListView::permSelectionChangedEvent()
{
  this->invokePermTotSelectItemsCB(this->getSelectedItems());
}
//----------------------------------------------------------------------------

void FuiItemsListView::tmpSelectionChangedEvent(FFuListViewItem* item)
{
  this->tmpSelectItemCB.invoke(item ? item->getItemId() : -1);
}
//----------------------------------------------------------------------------

void FuiItemsListView::listItemOpenedEvent(FFuListViewItem* item, bool open)
{
  if (item)
    this->expandItemCB.invoke(item->getItemId(),open);
}
//----------------------------------------------------------------------------

void FuiItemsListView::listItemToggledEvent(FFuListViewItem* item, int toggle)
{
  if (item)
    this->toggleItemCB.invoke(item->getItemId(),toggle);
}
//----------------------------------------------------------------------------

void FuiItemsListView::rightMousePressed(FFuListViewItem*)
{
  this->updateUICommands();
}
