// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_LIST_VIEW_ITEM_H
#define FFU_LIST_VIEW_ITEM_H

#include <string>

class FFuListView;


/*!
  This class automatically sets an unique id (>-1) for the item.
  The id is meant to serve as an identifier for external bookkeeping.
  Never bookkeep the item pointers as they may change during item manipulation.

  \author Dag R. Christensen
  \date Mar 5 1999
*/

class FFuListViewItem
{
public:
  FFuListViewItem();
  virtual ~FFuListViewItem() {}

  // Unique item identifier
  int  getItemId() const { return itemId; }
  void setItemId(int id) { itemId = id; }

  // Basic item functions
  virtual void        setItemText(int col, const char* text) = 0;
  virtual std::string getItemText(int col) const = 0;

  virtual void setItemTextBold(int col, bool bold = true) = 0;
  virtual void setItemTextItalic(int col, bool italic = true) = 0;

  virtual void setItemImage(int col, const char** pixmap) = 0;

  virtual void setItemSelectable(bool enable) = 0;
  virtual bool isItemSelected() const = 0;

  virtual bool isItemExpanded() const = 0;

  // Relations
  virtual FFuListView*     getListView() const = 0;
  virtual FFuListViewItem* getParentItem() const = 0;
  virtual FFuListViewItem* getFirstChildItem() const = 0;
  virtual FFuListViewItem* getLastChildItem() const = 0;
  virtual FFuListViewItem* getNextSiblingItem() const = 0;
          FFuListViewItem* getPreviousSiblingItem() const;
  virtual int              getNSiblings() const = 0; // with me included
  virtual int              getNChildren() const = 0;
  virtual int              getNColumns() const = 0;

  int getItemPosition() const; // starts with 0
  int getDepth() const;

  virtual void setItemDropable(bool enable) = 0;
  virtual void setItemDragable(bool enable) = 0;

  // Levels
  bool isFirstLevel() const;  // no parent
  bool isSecondLevel() const; // no grandparent but parent
  bool isThirdLevel() const;  // no greatgrandparent but grandparent and parent

  // Toggle, status starts always on UNTOGGLED
  enum ToggleVal { UNTOGGLED, TOGGLED, HALFTOGGLED };
  bool isItemToggleAble() const { return toggleAble > 0; }
  int getToggleValue() const { return toggle; }

  virtual void setItemToggleAble(unsigned char able) = 0;
  virtual void setToggleValue(int toggle, bool notify = false) = 0;
  void toggleItem(bool notify = false);
  void setItemThreeStepToggleAble(bool able);

protected:
  int itemId;
  int toggle;
  unsigned char toggleAble; // 0, 1 or 3=three-step-toggleble
};

#endif
