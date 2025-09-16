/* SPDX-FileCopyrightText: 2023 SAP SE
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file is part of FEDEM - https://openfedem.org
 */
/*
 * InputTable.cpp
 *
 *  Created on: Jan 16, 2012
 *      Author: runarhr
 */

#include <QTableView>
#include <QVBoxLayout>
#include <QAction>
#include <QKeySequence>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QMenu>
#include <QHeaderView>
#include <algorithm>

#include "FFuLib/FFuCustom/inputTables/InputTable.H"


InputTable::InputTable(int rows, int cols, TableOrdering oType, QWidget* parent)
  : QWidget(parent)
{
  apTableView = new QTableView();
  apTableView->setAcceptDrops(true);

  QLayout* layout = new QVBoxLayout(this);
  layout->addWidget(apTableView);

  // ******* Actions *******

  // Copy
  copyAction = new QAction("&Copy",this);
  copyAction->setShortcut(QKeySequence("Ctrl+c"));
  QObject::connect(copyAction, SIGNAL(triggered()), this, SLOT(Copy()));
  apTableView->addAction(copyAction);

  // Paste
  pasteAction = new QAction("&Paste",this);
  pasteAction->setShortcut(QKeySequence("Ctrl+v"));
  QObject::connect(pasteAction, SIGNAL(triggered()), this, SLOT(Paste()));
  apTableView->addAction(pasteAction);

  // Delete
  deleteAction = new QAction("&Delete", this);
  deleteAction->setShortcut(QKeySequence("delete"));
  QObject::connect(deleteAction, SIGNAL(triggered()), this, SLOT(Delete()));
  apTableView->addAction(deleteAction);

  // Insert row
  insertRowAction = new QAction("&Insert Row", this);
  insertRowAction->setShortcut(QKeySequence("Ctrl+i"));
  QObject::connect(insertRowAction, SIGNAL(triggered()), this, SLOT(InsertRow()));
  apTableView->addAction(insertRowAction);

  // Remove row
  removeRowAction = new QAction("&Remove Row", this);
  removeRowAction->setShortcut(QKeySequence("Ctrl+r"));
  QObject::connect(removeRowAction, SIGNAL(triggered()), this, SLOT(RemoveRow()) );
  apTableView->addAction(removeRowAction);

  // Insert column
  insertColumnAction = new QAction("&Insert Column", this);
  insertColumnAction->setShortcut(QKeySequence("Ctrl+k"));
  QObject::connect(insertColumnAction, SIGNAL(triggered()), this, SLOT(InsertColumn()) );
  apTableView->addAction(insertRowAction);

  // Remove column
  removeColumnAction = new QAction("&Remove Column", this);
  removeColumnAction->setShortcut(QKeySequence("Ctrl+l"));
  QObject::connect(removeColumnAction, SIGNAL(triggered()), this, SLOT(RemoveColumn()));
  apTableView->addAction(removeColumnAction);

  /***************/
  // Create model
  apTableModel = new TableModel(rows, cols, oType);
  apTableView->setModel(apTableModel);

  // Context menu for table
  apTableView->setContextMenuPolicy(Qt::CustomContextMenu);
  QObject::connect(apTableView, SIGNAL(customContextMenuRequested(const QPoint&)),
                   this, SLOT(contextMenuTable(const QPoint&)));

  switch(oType) {
  case COLUMN_DOMINANT:
    // Context menu for horizontal header
    apTableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(apTableView->horizontalHeader(), SIGNAL(customContextMenuRequested(const QPoint&)),
                     this, SLOT(contextMenuHorizontalHeader(const QPoint&)));

    apTableView->verticalHeader()->setSectionsClickable(false);
    apTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    break;
  case ROW_DOMINANT:
    // Context menu for vertical header
    apTableView->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(apTableView->verticalHeader(), SIGNAL(customContextMenuRequested(const QPoint&)),
                     this, SLOT(contextMenuVerticalHeader(const QPoint&)));

    apTableView->horizontalHeader()->setSectionsClickable(false);
    apTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    break;
  }

  apTableView->setAlternatingRowColors(true);
  apTableView->setLocale(QLocale::C);
  apTableView->resizeColumnsToContents();
}

void InputTable::ResizeToContents(){
	apTableView->resizeColumnsToContents();
}

void InputTable::Paste(){
	const QMimeData* mimeData = QApplication::clipboard()->mimeData();
	apTableModel->dropMimeData(mimeData, Qt::CopyAction, apTableView->currentIndex().row(), apTableView->currentIndex().column(),apTableView->currentIndex() );
}

void InputTable::Copy()
{
  QItemSelectionModel* pSelectionModel = apTableView->selectionModel();
  if (!pSelectionModel->hasSelection()) return;

  QString text = "";
  QModelIndexList indexList = pSelectionModel->selectedIndexes();
  std::stable_sort(indexList.begin(),indexList.end());
  QModelIndex previousIndex = indexList.first();
  for (const QModelIndex& idx : indexList) {
    if (previousIndex.row() != idx.row())
      text.append("\n");
    else if (previousIndex.column() != idx.column())
      text.append("\t");

    text.append(apTableModel->data(idx,Qt::DisplayRole).toString());
    previousIndex = idx;
  }
  text.append("\n");
  QApplication::clipboard()->setText(text);
}

void InputTable::Delete()
{
  QModelIndexList indexList = apTableView->selectionModel()->selectedIndexes();
  for (const QModelIndex& idx : indexList)
    apTableModel->setData(idx, QVariant(), Qt::EditRole);
}

void InputTable::InsertRow(){
	apTableModel->insertRow(apTableView->verticalHeader()->currentIndex().row(), apTableView->rootIndex() );
}

void InputTable::RemoveRow(){
	apTableModel->removeRow(apTableView->verticalHeader()->currentIndex().row(), apTableView->rootIndex() );
}

void InputTable::InsertColumn(){
	apTableModel->insertColumn(apTableView->horizontalHeader()->currentIndex().column(), apTableView->rootIndex() );
}

void InputTable::RemoveColumn(){
	apTableModel->removeColumn(apTableView->horizontalHeader()->currentIndex().column(), apTableView->rootIndex() );
}

void InputTable::contextMenuTable(const QPoint &pos){
    QMenu menu;
    menu.addAction(copyAction);
    menu.addAction(pasteAction);
    menu.addAction(deleteAction);
    menu.exec(apTableView->mapToGlobal(pos+QPoint(30,0)));
}

void InputTable::contextMenuVerticalHeader(const QPoint &pos){
	int row = apTableView->indexAt(pos).row();
	apTableView->selectRow(row);
    QMenu menu;
    //TODO make insert column work for multiple tables
    menu.addAction(insertRowAction);
    menu.addAction(removeRowAction);
    menu.exec(apTableView->horizontalHeader()->mapToGlobal(pos));
}

void InputTable::contextMenuHorizontalHeader(const QPoint &pos){
	int column = apTableView->indexAt(pos).column();
	apTableView->selectColumn(column);
    QMenu menu;
    //TODO make insert column work for multiple tables
//    menu.addAction(insertColumnAction);
//    menu.addAction(removeColumnAction);
    menu.exec(apTableView->horizontalHeader()->mapToGlobal(pos));
}
