// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

//#include <Qt3Support/Q3DragObject> Find Qt6 replacement?

#include "FuiQtItemsListViews.H"


FuiQtSimModelListView::FuiQtSimModelListView(QWidget* parent, const char* name)
  : FuiQtItemsListView(parent,name)
{
  this->initWidgets();
}

//----------------------------------------------------------------------------

FuiQtResultListView::FuiQtResultListView(QWidget* parent, const char* name)
  : FuiQtItemsListView(parent,name)
{
  this->initWidgets();
  this->setAcceptDrops(true);
  this->viewport()->setAcceptDrops(true);
}

/* TODO Qt6 port
Q3DragObject* FuiQtResultListView::dragObject()
{
  return new Q3TextDrag(this,"Hei");
}
*/
//----------------------------------------------------------------------------

FuiQtRDBListView::FuiQtRDBListView(QWidget* parent, const char* name)
  : FuiQtItemsListView(parent,name)
{
  this->initWidgets();
}

/* TODO Qt6 port
Q3DragObject* FuiQtRDBListView::dragObject()
{
  return new Q3TextDrag(this,"Hei");
}
*/

//----------------------------------------------------------------------------

FuiQtSimModelRDBListView::FuiQtSimModelRDBListView(QWidget* parent, const char* name)
  : FuiQtItemsListView(parent,name)
{
  this->initWidgets();
}

/* TODO Qt6 port
Q3DragObject* FuiQtSimModelRDBListView::dragObject()
{
  return new Q3TextDrag(this,"Hei");
}
*/

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
