// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QtGui/QPixmap>
#include <QtGui/QPainter>

#include "FFuLib/FFuQtComponents/FFuQtListView.H"
#include "FFuLib/FFuQtComponents/FFuQtListViewItem.H"
#include "FFuLib/FFuAuxClasses/FFuQtAuxClasses/FFuaQtPixmapCache.H"
#include "FFuLib/FFuAuxClasses/FFuAuxPixmaps/toggled.xpm"
#include "FFuLib/FFuAuxClasses/FFuAuxPixmaps/untoggled.xpm"
#include "FFuLib/FFuAuxClasses/FFuAuxPixmaps/halftoggled.xpm"


////////////////////////////////////////////////////////////////////////////////

FFuQtListViewItem::FFuQtListViewItem(FFuQtListView* parent,FFuQtListViewItem* after,const char* label)
  : Q3ListViewItem(parent,after)
{
  this->setItemText(0,label);
}
//----------------------------------------------------------------------------

FFuQtListViewItem::FFuQtListViewItem(FFuQtListView* parent,FFuQtListViewItem* after,
				     FFuQtListViewItem* original)
  : Q3ListViewItem(parent,after)
{
  this->copyData(original);
}
//----------------------------------------------------------------------------

FFuQtListViewItem::FFuQtListViewItem(FFuQtListViewItem* parent,FFuQtListViewItem* after,const char* label)
  : Q3ListViewItem(parent,after)
{
  this->setItemText(0,label);
}
//----------------------------------------------------------------------------

FFuQtListViewItem::FFuQtListViewItem(FFuQtListViewItem* parent,FFuQtListViewItem* after,
				     FFuQtListViewItem* original)
  : Q3ListViewItem(parent,after)
{
  this->copyData(original);
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::copyPixmaps(FFuListViewItem* original)
{
  for (int i = 0; i < this->getListView()->getNColumns(); i++)
  {
    Q3ListViewItem* qitem = dynamic_cast<Q3ListViewItem*>(original);
    if (qitem)
    {
      const QPixmap* pm = qitem->pixmap(i);
      if (pm) this->setPixmap(i,QPixmap(*pm));
    }
  }
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

void FFuQtListViewItem::setItemTextBold(int col, bool bold)
{
  this->boldtext = bold;
  this->repaint();
}

//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemTextItalic(int col, bool italic)
{
  this->italictext = italic;
  this->repaint();
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemImage(int col,const char **pixmap)
{
  this->setPixmap(col, FFuaQtPixmapCache::getPixmap(pixmap));
}
//----------------------------------------------------------------------------

bool FFuQtListViewItem::isItemSelected() const
{
  return this->isSelected();
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemSelectable(bool enable)
{
  this->setSelectable(enable);
}
//----------------------------------------------------------------------------

FFuListView* FFuQtListViewItem::getListView() const
{
  return dynamic_cast<FFuListView*>(this->listView());
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListViewItem::getParentItem() const
{
  return dynamic_cast<FFuListViewItem*>(this->parent());
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListViewItem::getFirstChildItem() const
{
  return dynamic_cast<FFuListViewItem*>(this->firstChild());
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListViewItem::getNextSiblingItem() const
{
  return dynamic_cast<FFuListViewItem*>(this->nextSibling());
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListViewItem::getLastChildItem() const
{
  Q3ListViewItem* item = this->firstChild();
  while (item && item->nextSibling())
    item = item->nextSibling();

  return dynamic_cast<FFuListViewItem*>(item);
}
//----------------------------------------------------------------------------

int FFuQtListViewItem::getNSiblings() const
{
  if (this->parent())
    return this->parent()->childCount();
  else
    return this->listView()->childCount();
}
//----------------------------------------------------------------------------

int FFuQtListViewItem::getNChildren() const
{
  return this->childCount();
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemToggleAble(unsigned char able)
{
  if (able == this->toggleAble) return;

  this->toggleAble = able;

  if (this->toggleAble)
    this->setToggleValue(this->toggle,false);
  else {
    this->setPixmap(0,QPixmap(untoggled_xpm));
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
  case UNTOGGLED:   this->setPixmap(0,QPixmap(untoggled_xpm)); break;
  case TOGGLED:     this->setPixmap(0,QPixmap(toggled_xpm)); break;
  case HALFTOGGLED: this->setPixmap(0,QPixmap(halftoggled_xpm)); break;
  }

  if (notify)
    this->getListView()->onListItemToggled(this,this->toggle);
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setSelected(bool isSelected)
{
  this->Q3ListViewItem::setSelected(isSelected);
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::paintCell(QPainter* p,const QColorGroup& cg,int column,int width,int align)
{
  QFont newfont(p->font());
  newfont.setBold(this->boldtext);
  newfont.setItalic(this->italictext);
  p->setFont(newfont);

  this->Q3ListViewItem::paintCell(p,cg,column,width,align);
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::paintFocus(QPainter* p,const QColorGroup& cg,const QRect& r)
{
  this->Q3ListViewItem::paintFocus(p,cg,r);
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemDropable(bool enable)
{
  this->setDropEnabled(enable);
}
//----------------------------------------------------------------------------

void FFuQtListViewItem::setItemDragable(bool enable)
{
  this->setDragEnabled(enable);
}
