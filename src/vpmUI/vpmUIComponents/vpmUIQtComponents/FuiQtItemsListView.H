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

class QEvent;
class QDropEvent;

class FuiQtItemsListView : public FFuQtListView,
			   virtual public FuiItemsListView
{
  Q_OBJECT

public:
  FuiQtItemsListView(QWidget* parent = NULL,
		     const char* name="FuiQtItemsListView");
  virtual ~FuiQtItemsListView() {}

private:
  virtual bool event(QEvent* e);
  virtual void setVisible(bool visible);

protected:
  void dropEvent(QDropEvent *e);
};

#endif
