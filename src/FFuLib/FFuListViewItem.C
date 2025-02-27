// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuListViewItem.H"
#include "FFuLib/FFuListView.H"


FFuListViewItem::FFuListViewItem()
{
  static int itemCount = 0;

  this->itemId = itemCount++;

  this->toggle = UNTOGGLED;
  this->toggleAble = false;
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuListViewItem::getPreviousSiblingItem() const
{
  FFuListViewItem* item = this->getParentItem();
  if (item)
    item = item->getFirstChildItem();
  else
    item = this->getListView()->getFirstChildItem();

  FFuListViewItem* previous = NULL;
  while (item)
    if (item == this)
      return previous;
    else if (item->getNextSiblingItem()) {
      previous = item;
      item = item->getNextSiblingItem();
    }

  return NULL;
}
//----------------------------------------------------------------------------

int FFuListViewItem::getItemPosition() const
{
  FFuListViewItem* sibling = this->getParentItem();
  if (sibling)
    sibling = sibling->getFirstChildItem();
  else
    sibling = this->getListView()->getFirstChildItem();

  for (int i = 0; sibling; i++)
    if (sibling == this)
      return i;
    else
      sibling = sibling->getNextSiblingItem();

  return -1;
}
//----------------------------------------------------------------------------

int FFuListViewItem::getDepth() const
{
  FFuListViewItem* parent = this->getParentItem();
  return parent ? parent->getDepth()+1 : 0;
}
//----------------------------------------------------------------------------

bool FFuListViewItem::isFirstLevel() const
{
  return this->getParentItem() ? false : true;
}
//----------------------------------------------------------------------------

bool FFuListViewItem::isSecondLevel() const
{
  FFuListViewItem* parent = this->getParentItem();
  return parent ? parent->isFirstLevel() : false;
}
//----------------------------------------------------------------------------

bool FFuListViewItem::isThirdLevel() const
{
  FFuListViewItem* parent = this->getParentItem();
  return parent ? parent->isSecondLevel() : false;
}
//----------------------------------------------------------------------------

void FFuListViewItem::toggleItem(bool notify)
{
  switch (this->toggle) {
  case UNTOGGLED:
    this->setToggleValue(TOGGLED,notify);
    break;
  case TOGGLED:
    if (this->toggleAble > 1)
      this->setToggleValue(HALFTOGGLED,notify);
    else
      this->setToggleValue(UNTOGGLED,notify);
    break;
  case HALFTOGGLED:
    this->setToggleValue(UNTOGGLED,notify);
    break;
  }
}
//----------------------------------------------------------------------------

void FFuListViewItem::setItemThreeStepToggleAble(bool able)
{
  if (able)
    this->setItemToggleAble(3);
  else if (this->toggleAble > 1)
    this->toggleAble = 1;
}
//----------------------------------------------------------------------------
