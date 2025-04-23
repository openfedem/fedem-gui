// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_TABLE_H
#define FFU_QT_TABLE_H

#include <QTableWidget>

#include "FFuLib/FFuTable.H"
#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"


class FFuQtTable : public QTableWidget, public FFuTable,
                   virtual public FFuQtComponentBase
{
  Q_OBJECT

public:
  FFuQtTable(QWidget* parent, bool hs = false, bool vs = false);
  virtual ~FFuQtTable() {}

  virtual void addRow(const std::vector<FFuComponentBase*>& cells, int row);

  virtual void insertWidget(int row, int col, FFuComponentBase* item);
  virtual FFuComponentBase* getWidget(int row, int col);
  virtual void insertText(int row, int col, const std::string& text);
  virtual std::string getText(int row, int col);

  virtual void insertComboItem(int row, int col,
                               const std::vector<std::string>& items);
  virtual int  getComboItemSelection(int row, int col);
  virtual void setComboItemCurrent(int row, int col, int i);

  virtual void insertCheckBoxItem(int row, int col,
                                  const std::string& text, bool setChecked);
  virtual bool getCheckBoxItemToggle(int row, int col);
  virtual void setCheckBoxItemToggle(int row, int col, bool toggle);

  virtual void clearCellContents(int row, int cell);
  virtual void deleteRow(int row);
  virtual void deleteColumn(int col);

  virtual void setColumnLabel(int col, const std::string& text);
  virtual std::string getColumnLabel(int col);
  virtual void setRowLabel(int row, const std::string& text);
  virtual void showColumnHeader(bool show);
  virtual void showRowHeader(bool show);

  virtual void setColumnHeaderHeight(int height);
  virtual void setRowHeaderWidth(int width);
  virtual int getColumnHeaderHeight();
  virtual int getRowHeaderWidth();

  virtual void showTableGrid(bool show);

  virtual void setSelectionPolicy(int policy);
  virtual int  getSelectionPolicy();

  virtual int getNumberRows();
  virtual int getNumberColumns();

  virtual void setNumberRows(int num);
  virtual void setNumberColumns(int cols);

  virtual int getRowHeight(int row);
  virtual int getColumnWidth(int col);

  virtual void setRowHeight(int row, int height);
  virtual void setColumnWidth(int col, int width);

  virtual void stretchRowHeight(int row, bool stretchable);
  virtual void stretchColWidth(int col, bool stretchable);
  virtual void adjustRowHeight(int row);
  virtual void adjustColumnWidth(int col);

  virtual int getCurrentRow();
  virtual int getCurrentCol();

  virtual void setSensitivity(bool isSensitive);
  virtual void setCellSensitivity(int row, int col, bool isSensitive);
  virtual void setTableCellReadOnly(int row, int col, bool ro);

protected slots:
  void fwdSelectionChanged()               { mySelectionChangedCB.invoke(); }
  void fwdCurrentCellChanged(int r, int c) { myCurrentCellChangedCB.invoke(r,c); }
  void fwdCellValueChanged(int r, int c)   { myCellValueChangedCB.invoke(r,c); }
  void fwdCellPressed(int r, int c)        { myCellPressedCB.invoke(r,c); }
  void fwdCellClicked(int r, int c)        { myCellClickedCB.invoke(r,c); }
  void fwdCellDoubleClicked(int, int)      { this->updateColumnWidths(); }
  void fwdColumnResized(int c,int o,int n) { this->columnSizeChanged(c,o,n); }

private:
  bool isInsideTable(int row, int col) const;
};

#endif
