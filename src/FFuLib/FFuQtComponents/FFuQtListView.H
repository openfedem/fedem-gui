// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_LIST_VIEW_H
#define FFU_QT_LIST_VIEW_H

#include <QTreeWidget>

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "FFuLib/FFuListView.H"

class QMouseEvent;


/*!
  \brief Qt-implementation of Fedem listview widgets based on QTreeWidget.
  \details Do not trust the MouseButton press/release signal because they
  are not given at the right time. Instead use the mousePressEvent() and
  mouseReleaseEvent() signals.
*/

class FFuQtListView : public QTreeWidget, virtual public FFuListView,
                      public FFuQtComponentBase
{
  Q_OBJECT

public:
  FFuQtListView(QWidget* parent = NULL, int nColumns = 0,
                const char* name = "FFuQtListView");
  virtual ~FFuQtListView() {}

protected slots:
  void fwdSelectionChanged();
  void fwdExpanded(QTreeWidgetItem* item);
  void fwdCollapsed(QTreeWidgetItem* item);
  void fwdReturnPressed(QTreeWidgetItem* item, int);
  void fwdDoubleClicked(QTreeWidgetItem* item, int);

public:
  virtual void setListSorting(int column, bool ascending);
  virtual void setHeaderClickEnabled(int column, bool enable);
  virtual void setListRootIsDecorated(bool enable);
  virtual void setAllListColumnsShowSelection(bool enable);
  virtual void setHeaderOff(bool Off);

  virtual void clearList();

  virtual void setListColumns(const std::vector<const char*>& labels);
  virtual void setListColumnWidth(int column, int width);
  virtual void resizeListColumnsToContents();
  virtual void permSelectListItem(FFuListViewItem* item, bool select,
                                  bool notify);
  virtual void clearListSelection(bool notify);
  virtual void openListItem(FFuListViewItem* item, bool open, bool notify);
  virtual void ensureListItemVisible(FFuListViewItem* item, bool notify);

  virtual void setSglSelectionMode(bool single);
  virtual bool isSglSelectionMode() const;

  virtual const char* getName() const;

  virtual FFuListViewItem* getSelectedListItemSglMode() const;
  virtual FFuListViewItem* getCurrentListItem() const;
  virtual FFuListViewItem* getFirstChildItem() const;

  virtual FFuListViewItem* createListItem(const char* label,
                                          FFuListViewItem* parent,
                                          FFuListViewItem* after);

private:
  // FFuComponentBase Re-impl
  virtual void setFonts (FFuaFontSet aFontSet);
  virtual void setColors(FFuaPalette aPalette);

  // QTreeWidget re-implementations
  virtual void mousePressEvent(QMouseEvent* e);
  virtual void mouseReleaseEvent(QMouseEvent* e);
};

#endif
