// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QtGui/QPixmap>

#include "FFuLib/FFuQtComponents/FFuQtListView.H"
#include "FFuLib/FFuQtComponents/FFuQtListViewItem.H"
#include "FFuLib/FFuAuxClasses/FFuQtAuxClasses/FFuaQtPixmapCache.H"
#include "FFuLib/FFuAuxClasses/FFuAuxPixmaps/toggled.xpm"
#include "FFuLib/FFuAuxClasses/FFuAuxPixmaps/untoggled.xpm"
#include "FFuLib/FFuAuxClasses/FFuAuxPixmaps/halftoggled.xpm"


////////////////////////////////////////////////////////////////////////////////

FFuQtListViewItem::FFuQtListViewItem(FFuQtListView* parent,FFuQtListViewItem* after,const char* label)
  : QTreeWidgetItem(parent,after)
{
  this->setItemText(0,label);
}
//----------------------------------------------------------------------------

FFuQtListViewItem::FFuQtListViewItem(FFuQtListView* parent,FFuQtListViewItem* after,
				     FFuQtListViewItem* original)
  : QTreeWidgetItem(parent,after)
{
  this->copyData(original);
}
//----------------------------------------------------------------------------

FFuQtListViewItem::FFuQtListViewItem(FFuQtListViewItem* parent,FFuQtListViewItem* after,const char* label)
  : QTreeWidgetItem(parent,after)
{
  this->setItemText(0,label);
}
//----------------------------------------------------------------------------

FFuQtListViewItem::FFuQtListViewItem(FFuQtListViewItem* parent,FFuQtListViewItem* after,
				     FFuQtListViewItem* original)
  : QTreeWidgetItem(parent,after)
{
  this->copyData(original);
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::copyPixmaps(FFuListViewItem* original)
{
  QTreeWidgetItem* item = dynamic_cast<QTreeWidgetItem*>(original);
  if (!item) return;

  for (int col = 0; col < this->columnCount(); col++)
  {
    QIcon ic = item->icon(col);
    if (!ic.isNull()) this->setIcon(col,ic);
  }
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::copyTexts(FFuListViewItem* original)
{
  QTreeWidgetItem* item = dynamic_cast<QTreeWidgetItem*>(original);
  if (!item) return;

  for (int col = 0; col < item->columnCount(); col++)
  {
    this->setText(col,item->text(col));
    this->setFont(col,item->font(col));
  }
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemText(int col, const char* text)
{
  this->setText(col,text);
}
//----------------------------------------------------------------------------

std::string FFuQtListViewItem::getItemText(int col) const
{
  return this->text(col).toStdString();
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemTextBold(int col, bool bold)
{
  QFont myFont(this->font(col));
  myFont.setBold(bold);
  this->setFont(col,myFont);
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemTextItalic(int col, bool italic)
{
  QFont myFont(this->font(col));
  myFont.setItalic(italic);
  this->setFont(col,myFont);
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemImage(int col, const char** pixmap)
{
  this->setIcon(col, QIcon(FFuaQtPixmapCache::getPixmap(pixmap)));
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemSelectable(bool enable)
{
  this->toggleFlag(enable,Qt::ItemIsSelectable);
}
//----------------------------------------------------------------------------

FFuListView* FFuQtListViewItem::getListView() const
{
  return dynamic_cast<FFuListView*>(this->treeWidget());
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListViewItem::getParentItem() const
{
  return dynamic_cast<FFuListViewItem*>(this->parent());
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListViewItem::getFirstChildItem() const
{
  return dynamic_cast<FFuListViewItem*>(this->child(0));
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListViewItem::getNextSiblingItem() const
{
  QTreeWidgetItem* nSI = NULL;
  if (this->parent())
  {
    for (int i = 0; i+1 < this->parent()->childCount() && !nSI; i++)
      if (this->parent()->child(i) == this)
        nSI = this->parent()->child(i+1);
  }
  else if (this->treeWidget())
  {
    for (int i = 0; i+1 < this->treeWidget()->topLevelItemCount() && !nSI; i++)
      if (this->treeWidget()->topLevelItem(i) == this)
        nSI = this->treeWidget()->topLevelItem(i+1);
  }

  return dynamic_cast<FFuListViewItem*>(nSI);
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListViewItem::getLastChildItem() const
{
  return dynamic_cast<FFuListViewItem*>(this->child(this->childCount()-1));
}
//----------------------------------------------------------------------------

int FFuQtListViewItem::getNSiblings() const
{
  if (this->parent())
    return this->parent()->childCount();
  else
    return this->treeWidget()->topLevelItemCount();
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemToggleAble(unsigned char able)
{
  if (able == this->toggleAble) return;

  this->toggleAble = able;

  if (this->toggleAble)
    this->setToggleValue(this->toggle,false);
  else {
    this->setIcon(0,QIcon(QPixmap(untoggled_xpm)));
    this->toggle = UNTOGGLED;
  }
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setToggleValue(int toggleVal, bool notify)
{
  if (toggleVal == HALFTOGGLED && this->toggleAble <= 1)
    this->toggle = TOGGLED;
  else if (toggleVal <= HALFTOGGLED)
    this->toggle = toggleVal;
  else
    return;

  this->setItemToggleAble(true);

  switch (this->toggle) {
  case UNTOGGLED:   this->setIcon(0,QIcon(QPixmap(untoggled_xpm))); break;
  case TOGGLED:     this->setIcon(0,QIcon(QPixmap(toggled_xpm))); break;
  case HALFTOGGLED: this->setIcon(0,QIcon(QPixmap(halftoggled_xpm))); break;
  }

  if (notify)
    this->getListView()->onListItemToggled(this,this->toggle);
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemDropable(bool enable)
{
  this->toggleFlag(enable,Qt::ItemIsDropEnabled);
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemDragable(bool enable)
{
  this->toggleFlag(enable,Qt::ItemIsDragEnabled);
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::toggleFlag(bool enable, Qt::ItemFlags flag)
{
  this->setFlags(enable ? this->flags() | flag : this->flags() & ~flag);
}
