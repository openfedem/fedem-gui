// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_LIST_VIEW_ITEM_H
#define FFU_QT_LIST_VIEW_ITEM_H

#include "FFuLib/FFuListViewItem.H"
#include <Qt3Support/Q3ListView>


class FFuQtListView;

class FFuQtListViewItem : public Q3ListViewItem, public FFuListViewItem
{
public:
  // If parent=0, the item is created at top level in the list view
  // If after=0 the item is inserted first in parents list
  FFuQtListViewItem(FFuQtListView* parent,FFuQtListViewItem* after,const char* label);
  FFuQtListViewItem(FFuQtListView* parent,FFuQtListViewItem* after,FFuQtListViewItem* original);
  FFuQtListViewItem(FFuQtListViewItem* parent,FFuQtListViewItem* after,const char* label);
  FFuQtListViewItem(FFuQtListViewItem* parent,FFuQtListViewItem* after,FFuQtListViewItem* original);
  virtual ~FFuQtListViewItem() {}
  virtual void copyPixmaps(FFuListViewItem* original);

  virtual void        setItemText(int col, const char* text);
  virtual std::string getItemText(int col) const;
  virtual void setItemTextBold(int col, bool bold);
  virtual void setItemTextItalic(int col, bool italic);
  virtual void setItemImage(int col, const char** pixmap);
  virtual void setItemSelectable(bool enable);
  virtual bool isItemSelected() const;

  virtual FFuListView*     getListView() const;
  virtual FFuListViewItem* getParentItem() const;
  virtual FFuListViewItem* getFirstChildItem() const;
  virtual FFuListViewItem* getLastChildItem() const;
  virtual FFuListViewItem* getNextSiblingItem() const;
  virtual int              getNSiblings() const;
  virtual int              getNChildren() const;

  virtual void setItemToggleAble(unsigned char able);
  virtual void setToggleValue(int toggle, bool notify);

  virtual void setItemDropable(bool enable);
  virtual void setItemDragable(bool enable);

private:
  //reimpl from QListItem
  virtual void setSelected(bool selected);
  virtual void paintCell(QPainter* p,const QColorGroup& cg,int column,int width,int align);
  virtual void paintFocus(QPainter* p,const QColorGroup& cg,const QRect& r);
};

#endif
