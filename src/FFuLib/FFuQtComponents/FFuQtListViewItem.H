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
  FFuQtListViewItem(FFuQtListView* parent,
                    FFuQtListViewItem* after,
                    const char* label);
  FFuQtListViewItem(FFuQtListViewItem* parent,
                    FFuQtListViewItem* after,
                    const char* label);
  virtual ~FFuQtListViewItem() {}

  virtual void        setItemText(int col, const char* text);
  virtual std::string getItemText(int col) const;
  virtual void setItemTextBold(int col, bool bold);
  virtual void setItemTextItalic(int col, bool italic);
  virtual void setItemImage(int col, const char** pixmap);
  virtual void setItemSelectable(bool enable);
  virtual bool isItemSelected() const { return this->isSelected(); }
  virtual bool isItemExpanded() const { return this->isExpanded(); }

  virtual FFuListView*     getListView() const;
  virtual FFuListViewItem* getParentItem() const;
  virtual FFuListViewItem* getFirstChildItem() const;
  virtual FFuListViewItem* getLastChildItem() const;
  virtual FFuListViewItem* getNextSiblingItem() const;
  virtual int              getNSiblings() const;
  virtual int              getNChildren() const { return this->childCount(); }
  virtual int              getNColumns() const { return this->columnCount(); }

  virtual void setItemToggleAble(unsigned char able);
  virtual void setToggleValue(int toggle, bool notify);

  virtual void setItemDropable(bool enable);
  virtual void setItemDragable(bool enable);

private:
  void toggleFlag(bool enable, Qt::ItemFlags flag);
};

#endif
