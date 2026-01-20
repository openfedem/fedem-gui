// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QDropEvent>
#include <QDragEnterEvent>
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

void FuiQtItemsListView::showEvent(QShowEvent* e)
{
  this->FFuQtListView::showEvent(e);
  this->onPoppedUp();
}
//----------------------------------------------------------------------------

void FuiQtItemsListView::hideEvent(QHideEvent* e)
{
  this->FFuQtListView::hideEvent(e);
  this->onPoppedDown();
}
//----------------------------------------------------------------------------

void FuiQtItemsListView::dropEvent(QDropEvent* e)
{
  QTreeWidgetItem* qtDropItem = this->itemAt(e->position().toPoint());
  FFuQtListViewItem* dropItem = dynamic_cast<FFuQtListViewItem*>(qtDropItem);
  int dropAction = e->dropAction() & Qt::CopyAction;
  droppedCB.invoke(dropItem ? dropItem->getItemId() : -1, dropAction);
  if (dropAction < 0) // ignore illegal drop event
  {
    // The ignore() call seems insufficient in this case - the action will still
    // be performed resulting in an invalid tree view and a crash. However,
    // adding the setDropAction() call seems to resolve the matter, as suggested
    // in the post https://forum.qt.io/topic/27876/handle-rejected-dropevent/2
    e->setDropAction(Qt::IgnoreAction);
    e->ignore();
  }
}
//----------------------------------------------------------------------------

void FuiQtItemsListView::dragEnterEvent(QDragEnterEvent* e)
{
  bool accepted = true;
  startDragCB.invoke(accepted);
  if (accepted)
    this->FFuQtListView::dragEnterEvent(e);
  else
    e->ignore(); // ignore illegal drag event
}
