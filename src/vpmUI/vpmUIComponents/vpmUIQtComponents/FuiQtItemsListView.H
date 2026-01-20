// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_ITEMS_LISTVIEW_H
#define FUI_QT_ITEMS_LISTVIEW_H

#include "FFuLib/FFuQtComponents/FFuQtListView.H"
#include "vpmUI/vpmUIComponents/FuiItemsListView.H"


class FuiQtItemsListView : public FFuQtListView, virtual public FuiItemsListView
{
protected:
  FuiQtItemsListView(QWidget* parent, const char* name);

public:
  virtual void setVisible(bool visible);

protected:
  virtual void showEvent(QShowEvent* e);
  virtual void hideEvent(QHideEvent* e);
  virtual void dragEnterEvent(QDragEnterEvent* e);
  virtual void dropEvent(QDropEvent* e);
};

#endif
