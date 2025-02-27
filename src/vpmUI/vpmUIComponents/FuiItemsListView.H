// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_ITEMS_LIST_VIEW_H
#define FUI_ITEMS_LIST_VIEW_H

#include "FFuLib/FFuListView.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUAItemsViewHandler.H"
#include "FFuLib/FFuBase/FFuUACommandHandler.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <vector>
#include <string>

class FFuaCmdItem;


class FuiItemsListView : virtual public FFuListView,
                         public FFuUAExistenceHandler,
                         public FFuUAItemsViewHandler,
                         public FFuUACommandHandler
{
public:
  FuiItemsListView() {}
  virtual ~FuiItemsListView() {}

  // These callbacks are set by FapUAItemsListView
  void setTmpSelectItemCB(const FFaDynCB1<int>& cb) { tmpSelectItemCB = cb; }
  void setExpandItemCB(const FFaDynCB2<int,bool>& cb) { expandItemCB = cb; }
  void setToggleItemCB(const FFaDynCB2<int,int>& cb) { toggleItemCB = cb; }
  void setStartDragCB(const FFaDynCB1<bool&>& cb) { startDragCB = cb; }
  void setDroppedCB(const FFaDynCB2<int,int&>& cb) { droppedCB = cb; }

  int createItem(int parent, int after, const std::string& label);

  // item settings
  void setItemSelectAble(int item, bool able);
  void expandItem(int item, bool expand);// no notify
  void ensureItemVisible(int item);//expands, notify

  void setItemText(int item, const std::string& texts);
  void setItemTextBold(int item, bool bold);
  void setItemTextItalic(int item, bool italic);
  void setItemImage(int item, const char** pixmap);

  void setItemToggleAble(int item, bool able);
  void setItemThreeStepToggleAble(int item, bool able);
  void setItemToggleValue(int item, int value);// no notify
  int  getItemToggleValue(int item) const;// no notify

  void setItemDropable(int item, bool yesOrNo);
  void setItemDragable(int item, bool yesOrNo);

  // item relations
  int getItemPosition(int item) const;
  int getItemBefore(int itemsParent, int itemsListPosition);
  // -1 if not found
  int getParent(int item) const;
  // if parent = -1, top level children
  std::vector<int> getChildren(int parent, bool all = false);
  int getNSiblings(int item) const;
  int getNChildren(int item) const;

  // listview settings
  void setDecoratedRoot(bool decorated);
  void showHeader(bool show);
  void enableHeaderClick(bool enable);

  // selection (no notify)
  void tmpSelectItem(int item) { this->tmpSelectListItem(this->getListItem(item)); }
  void permSelectItem(int item) { this->permSelectListItem(this->getListItem(item)); }

  // from FFuUAItemsViewHandler, view methods
  virtual void deleteItem(int item);

private:
  virtual void deleteItems();
  virtual void permTotSelectItems(const std::vector<int>& totalSelection);//no notify
  virtual std::vector<int> getSelectedItems();
  virtual bool isItemSelected(int item);

  // from FFuUACommandHandler
  virtual void setUICommands(const FFuaUICommands* commands);
  virtual void updateUICommandsSensitivity(const FFuaUICommands* commands);
  virtual void updateUICommandsToggle(const FFuaUICommands* commands);

  // inside callback functions from FFuListView
  virtual void permSelectionChangedEvent();
  virtual void tmpSelectionChangedEvent(FFuListViewItem* listItem);
  virtual void listItemOpenedEvent(FFuListViewItem* item, bool open);
  virtual void listItemToggledEvent(FFuListViewItem* item, int toggle);

protected:
  // These methods are called every time a listview pops up/down,
  // also from an iconized position
  virtual void onPoppedUp() {}
  virtual void onPoppedDown() {}

  // to/from memory only
  // called every time a ui appears on the screen (not from icon)
  virtual void onPoppedUpFromMem() { this->onPoppedUpFromMemory(); }
  // called when a ui is on its way to the memory (hide)
  virtual void onPoppedDownToMem() { this->onPoppedDownToMemory(); }

  // Builds popup commands when invoked
  virtual void rightMousePressed(FFuListViewItem* listItem);

public:
  static bool mouseSelected;

protected:
  FFaDynCB1<int>      tmpSelectItemCB;
  FFaDynCB2<int,bool> expandItemCB;
  FFaDynCB2<int,int>  toggleItemCB;
  FFaDynCB1<bool&>    startDragCB;
  FFaDynCB2<int,int&> droppedCB;
};

/////////////////////////////////////////////////////////////////////////////

class FuaItemsLVCommands : public FFuaUICommands
{
public:
  FuaItemsLVCommands() {}
  virtual ~FuaItemsLVCommands() {}

public:
  std::vector<FFuaCmdItem*> popUpMenu;
};
/////////////////////////////////////////////////////////////////////////////

#endif
