// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_LIST_VIEW_ITEM_H
#define FFU_QT_LIST_VIEW_ITEM_H

#include "FFuLib/FFuListViewItem.H"
#include <QTreeWidgetItem>

class FFuQtListView;


class FFuQtListViewItem : public QTreeWidgetItem, public FFuListViewItem
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
  virtual void copyTexts(FFuListViewItem* original);

  virtual void        setItemText(int col, const char* text);
  virtual std::string getItemText(int col) const;
  virtual void setItemTextBold(bool bold);
  virtual void setItemTextItalic(bool italic);
  virtual void setItemImage(int col, const char** pixmap);
  virtual bool isItemSelected();
  virtual void setItemSelectable(bool enable);

  virtual FFuListView*     getListView() const;
  virtual FFuListViewItem* getParentItem() const;
  virtual FFuListViewItem* getFirstChildItem() const;
  virtual FFuListViewItem* getLastChildItem() const;
  virtual FFuListViewItem* getNextSiblingItem() const;
  virtual int              getNSiblings() const;
  virtual int              getNChildren() const;

  virtual void setItemToggleAble(bool able);
  virtual void setToggleValue(int toggle, bool notify);

  virtual void setItemDropable(bool enable);
  virtual void setItemDragable(bool enable);

private:
  virtual void setSelected(bool selected);
  /* TODO Not yet ported to Qt6
  virtual void paintCell(QPainter* p,const QColorGroup& cg,int column,int width,int align);
  virtual void paintFocus(QPainter* p,const QColorGroup& cg,const QRect& r);
  end TODO */
};

#endif
