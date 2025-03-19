// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

/*!
  \file FuiQtItemsListViews.H

  \brief Header file for the Qt Implementation of FuiItemsListViews.

  You must give all list view objects a unique name when you use them
  because of the list view variable management in FFaListViewItem.

  \author Dag R. Christensen
*/

#ifndef FUI_QT_ITEMS_LIST_VIEWS_H
#define FUI_QT_ITEMS_LIST_VIEWS_H

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtItemsListView.H"
#include "vpmUI/vpmUIComponents/FuiItemsListViews.H"


class FuiQtSimModelListView : public FuiQtItemsListView,
                              public FuiSimModelListView
{
public:
  FuiQtSimModelListView(QWidget* parent, const char* name = "SimModel");
};


class FuiQtResultListView : public FuiQtItemsListView,
                            public FuiResultListView
{
public:
  FuiQtResultListView(QWidget* parent, const char* name = "Result");
};


class FuiQtRDBListView : public FuiQtItemsListView,
                         public FuiRDBListView
{
public:
  FuiQtRDBListView(QWidget* parent, const char* name = "RDB");
protected:
  // Need to override this method in order to disable the Move drop action
  void startDrag(Qt::DropActions) override;
};


class FuiQtSimModelRDBListView : public FuiQtItemsListView,
                                 public FuiSimModelRDBListView
{
public:
  FuiQtSimModelRDBListView(QWidget* parent, const char* name = "SimModelRDB");
protected:
  // Need to override this method in order to disable the Move drop action
  void startDrag(Qt::DropActions) override;
};


class FuiQtCrossSectionListView : public FuiQtItemsListView,
                                  public FuiCrossSectionListView
{
public:
  FuiQtCrossSectionListView(QWidget* parent, const char* name = "CrossSection");
};


class FuiQtMaterialListView : public FuiQtItemsListView,
                              public FuiMaterialListView
{
public:
  FuiQtMaterialListView(QWidget* parent, const char* name = "Material");
};

#endif
