// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuListViewItem.H"
#include "FFuLib/FFuListView.H"

#include "FFuLib/FFuAuxClasses/FFuAuxPixmaps/toggled.xpm"
#include "FFuLib/FFuAuxClasses/FFuAuxPixmaps/untoggled.xpm"
#include "FFuLib/FFuAuxClasses/FFuAuxPixmaps/halftoggled.xpm"


int FFuListViewItem::itemCount = 0;
const char** FFuListViewItem::togglepx[] = { untoggled_xpm,
                                             toggled_xpm,
                                             halftoggled_xpm };


FFuListViewItem::FFuListViewItem()
{
  this->itemId = itemCount++;

  this->toggleAble = false;
  this->threeStepToggleAble = false;
  this->toggle = 0;
  this->boldtext = false;
  this->italictext = false;
  this->col0Pixmap = NULL;
}
//----------------------------------------------------------------------------

void FFuListViewItem::copyData(FFuListViewItem* original)
{
  this->col0Pixmap = original->col0Pixmap;
  this->copyPixmaps(original);
  this->copyTexts(original);
  this->setItemTextBold(original->boldtext);
  this->setItemTextItalic(original->italictext);

  if (original->toggleAble) {
    this->setItemToggleAble(original->toggleAble);
    this->setItemThreeStepToggleAble(original->threeStepToggleAble);
    this->setToggleValue(original->toggle);
  }
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
    if (this->threeStepToggleAble)
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
