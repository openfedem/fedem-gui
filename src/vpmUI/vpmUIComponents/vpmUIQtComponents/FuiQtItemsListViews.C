// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QDrag>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtItemsListViews.H"


FuiQtSimModelListView::FuiQtSimModelListView(QWidget* parent, const char* name)
  : FuiQtItemsListView(parent,name)
{
  this->setSglSelectionMode(false);
  this->initWidgets();
}

//----------------------------------------------------------------------------

FuiQtResultListView::FuiQtResultListView(QWidget* parent, const char* name)
  : FuiQtItemsListView(parent,name)
{
  this->setDragEnabled(true);
  this->setAcceptDrops(true);
  this->viewport()->setAcceptDrops(true);
  this->initWidgets();
}

//----------------------------------------------------------------------------

FuiQtRDBListView::FuiQtRDBListView(QWidget* parent, const char* name)
  : FuiQtItemsListView(parent,name)
{
  this->setDragEnabled(true);
  this->initWidgets();
}

//----------------------------------------------------------------------------
// See comment below for similar FuiQtSimModelRDBListView method.
void FuiQtRDBListView::startDrag(Qt::DropActions)
{
  QDrag* drag = new QDrag(this);
  drag->setMimeData(this->model()->mimeData(this->selectedIndexes()));
  drag->exec(Qt::CopyAction); // Only allow copying, not moving
}

//----------------------------------------------------------------------------

FuiQtSimModelRDBListView::FuiQtSimModelRDBListView(QWidget* parent, const char* name)
  : FuiQtItemsListView(parent,name)
{
  this->setDragEnabled(true);
  this->initWidgets();
}

//----------------------------------------------------------------------------
// This solution was proposed by ChatGPT when asked "How to disable the
// Qt::MoveAction when dragging an item from one QTreeView to another".
// Without this, the list item dragged from the SimModelRDBListView will be
// deleted, which it is not designed for, causing the application to crash.
void FuiQtSimModelRDBListView::startDrag(Qt::DropActions)
{
  QDrag* drag = new QDrag(this);
  drag->setMimeData(this->model()->mimeData(this->selectedIndexes()));
  drag->exec(Qt::CopyAction); // Only allow copying, not moving
}

//----------------------------------------------------------------------------

FuiQtCrossSectionListView::FuiQtCrossSectionListView(QWidget* parent, const char* name)
  : FuiQtItemsListView(parent,name)
{
  this->initWidgets();
}

//----------------------------------------------------------------------------

FuiQtMaterialListView::FuiQtMaterialListView(QWidget* parent, const char* name)
  : FuiQtItemsListView(parent,name)
{
  this->initWidgets();
}
