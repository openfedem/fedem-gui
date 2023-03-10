// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QEvent>
#include <QDropEvent>
#include <QStyleFactory>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtItemsListView.H"
#include "FFuLib/FFuQtComponents/FFuQtListViewItem.H"

//----------------------------------------------------------------------------

FuiQtItemsListView::FuiQtItemsListView(QWidget* parent, const char* name)
  : FFuQtListView(parent,1,name)
{
  this->setStyle(QStyleFactory::create("windows")); // enable connector lines
  this->setStyleSheet("selection-color: white;"
                      "selection-background-color: #3399ff;");
}
//----------------------------------------------------------------------------

void FuiQtItemsListView::setVisible(bool visible)
{
  bool wasvisible = this->isVisible();

  this->FFuQtListView::setVisible(visible);

  if (!wasvisible && visible)
    this->onPoppedUpFromMem();
  else if (wasvisible && !visible)
    this->onPoppedDownToMem();
}
//----------------------------------------------------------------------------

bool FuiQtItemsListView::event(QEvent* e)
{
  bool ret = this->FFuQtListView::event(e);

  switch (e->type())
  {
  case QEvent::Show:
    this->onPoppedUp();
    return true;
  case QEvent::Hide:
    this->onPoppedDown();
    return true;
  default:
    break;
  }

  return ret;
}
//----------------------------------------------------------------------------

void FuiQtItemsListView::dropEvent(QDropEvent* e)
{
  QTreeWidgetItem* qtDropItem = this->itemAt(e->pos());
  FFuQtListViewItem* dropItem = dynamic_cast<FFuQtListViewItem*>(qtDropItem);
  this->droppedCB.invoke(dropItem ? dropItem->getItemId() : -1,
                         e->dropAction() & Qt::CopyAction, e);
}
