// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QtGui/QPixmap>
#include <QtGui/QPainter>

#include "FFuLib/FFuQtComponents/FFuQtListView.H"
#include "FFuLib/FFuAuxClasses/FFuQtAuxClasses/FFuaQtPixmapCache.H"
#include "FFuLib/FFuQtComponents/FFuQtListViewItem.H"


//////////////////////////////////////////////////////////////////////////////
// FFuQtListViewItem Methods

FFuQtListViewItem::FFuQtListViewItem(FFuQtListView* parent,FFuQtListViewItem* after,const char* label)
  : QTreeWidgetItem(parent,after)
{
  this->setItemText(0,label);
}
//----------------------------------------------------------------------------

FFuQtListViewItem::FFuQtListViewItem(FFuQtListView* parent,FFuQtListViewItem* after,FFuQtListViewItem* original)
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

FFuQtListViewItem::FFuQtListViewItem(FFuQtListViewItem* parent,FFuQtListViewItem* after, FFuQtListViewItem* original)
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
    this->setText(col,item->text(col));
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemText(int col,const char* text)
{
  this->setText(col,text);
}
//----------------------------------------------------------------------------

std::string FFuQtListViewItem::getItemText(int col) const
{
  return this->text(col).toStdString();
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemTextBold(bool bold)
{
  this->boldtext = bold;
  //this->repaint(); // TODO necessary with Qt6?
}

//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemTextItalic(bool italic)
{
  this->italictext = italic;
  //this->repaint(); // TODO necessary with Qt6?
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemImage(int col, const char** pixmap)
{
  this->setIcon(col, QIcon(FFuaQtPixmapCache::getPixmap(pixmap)));
}
//----------------------------------------------------------------------------

bool FFuQtListViewItem::isItemSelected()
{
  return this->isSelected();
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemSelectable(bool enable)
{
  this->setFlags(enable ? this->flags() | Qt::ItemIsSelectable : this->flags() & ~Qt::ItemIsSelectable);
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
  if (this->parent())
  {
    for (int i = 0; i+1 < this->parent()->childCount(); i++)
      if (this->parent()->child(i) == this)
        return dynamic_cast<FFuListViewItem*>(this->parent()->child(i+1));
  }
  else if (this->treeWidget())
  {
    for (int i = 0; i+1 < this->treeWidget()->topLevelItemCount(); i++)
      if (this->treeWidget()->topLevelItem(i) == this)
        return dynamic_cast<FFuListViewItem*>(this->treeWidget()->topLevelItem(i+1));
  }

  return NULL;
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

int FFuQtListViewItem::getNChildren() const
{
  return this->childCount();
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemToggleAble(bool able)
{
  if (this->col0Pixmap) return;
  if (able == this->toggleAble) return;

  this->toggleAble = able;

  if (this->toggleAble)
    this->setToggleValue(this->toggle,false);
  else {
    this->setIcon(0, QIcon(QPixmap(this->col0Pixmap)));
    this->toggle = 0;
    this->threeStepToggleAble = false;
  }
}
//----------------------------------------------------------------------------

void  FFuQtListViewItem::setToggleValue(int toggleVal,bool notify)
{
  if (this->col0Pixmap) return;
  if (toggleVal >= NTOGGLES) return;

  if (toggleVal == HALFTOGGLED && !this->threeStepToggleAble)
    this->toggle = TOGGLED;
  else
    this->toggle = toggleVal;

  this->setItemToggleAble(true);

  this->setIcon(0,QIcon(QPixmap(FFuListViewItem::togglepx[this->toggle])));

  if (notify)
    this->getListView()->onListItemToggled(this,this->toggle);
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setSelected(bool isSelected)
{
  this->QTreeWidgetItem::setSelected(isSelected);
}
//----------------------------------------------------------------------------
/* TODO Not yet ported to Qt 6
void FFuQtListViewItem::paintCell(QPainter* p,const QColorGroup& cg,int column,int width,int align)
{
  QFont newfont(p->font());
  newfont.setBold(this->boldtext);
  newfont.setItalic(this->italictext);
  p->setFont(newfont);

  this->QTreeWidgetItem::paintCell(p,cg,column,width,align);
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::paintFocus(QPainter* p,const QColorGroup& cg,const QRect& r)
{
  this->QTreeWidgetItem::paintFocus(p,cg,r);
}
end TODO */
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemDropable(bool enable)
{
  this->setFlags(enable ? this->flags() | Qt::ItemIsDropEnabled : this->flags() & ~Qt::ItemIsDropEnabled);
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemDragable(bool enable)
{
  this->setFlags(enable ? this->flags() | Qt::ItemIsDragEnabled : this->flags() & ~Qt::ItemIsDragEnabled);
}
