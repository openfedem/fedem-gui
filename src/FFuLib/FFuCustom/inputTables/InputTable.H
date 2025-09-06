/* SPDX-FileCopyrightText: 2023 SAP SE
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file is part of FEDEM - https://openfedem.org
 */
/*
 * InputTable.h
 *
 *  Created on: Jan 16, 2012
 *      Author: runarhr
 */

#ifndef INPUT_TABLE_H
#define INPUT_TABLE_H

#include <QWidget>

#include "FFuLib/FFuCustom/mvcModels/TableModel.H"

class QAction;
class QTableView;


/*!
  Class that defines a complete spreadsheet/table, with a model and a view.
*/

class InputTable : public QWidget
{
  Q_OBJECT

public:
  InputTable(int rows, int cols, TableOrdering oType, QWidget* parent = NULL);
  virtual ~InputTable() {}

  TableModel* GetModel() { return apTableModel; }
  QTableView* GetView() { return apTableView; }

  void ResizeToContents();

  QAction* copyAction;
  QAction* pasteAction;
  QAction* deleteAction;
  QAction* insertRowAction;
  QAction* removeRowAction;
  QAction* insertColumnAction;
  QAction* removeColumnAction;

public slots:
  void Copy();
  void Paste();
  void Delete();

  void InsertRow();
  void RemoveRow();
  void InsertColumn();
  void RemoveColumn();

  virtual void contextMenuTable(const QPoint& pos);
  virtual void contextMenuVerticalHeader(const QPoint& pos);
  virtual void contextMenuHorizontalHeader(const QPoint& pos);

protected:
  TableModel* apTableModel;
  QTableView* apTableView;
};

#endif
