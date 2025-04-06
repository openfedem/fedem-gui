// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_TABLE_H
#define FFU_TABLE_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <vector>
#include <string>


/*!
  \brief Wrapper around QTableWidget

  Looks and behaves like a spreadsheet (sort of). Note that the table takes
  ownership of the inserted items, and deletes them whenever it feels necessary.

  You have the choice of inserting either a table item or a widget into the
  cells. Note that QTable behaves strange sometimes, and you will have to make
  sure yourself that you don't insert both a widget and a table item into the
  same cell. Note also that clearTable() and clearTableItems() only work on
  table items, leaving widgets and possibly creating memory leaks.
  If you want to clear widgets also, you should use other methods.
  Or do some work on this class to improve it :-

  The table doesn't remember settings for cells that have been removed.
  Therefore if you set, e.g., height to a row, remove it and inserts it again,
  the height setting will be lost.

  \author Sven-Kaare Evenseth
*/

class FFuTable : virtual public FFuComponentBase
{
public:
  FFuTable() { resizingColumn = -1; }
  virtual ~FFuTable() {}

  //! Adds row of components. Append if row is negative or out of bounds.
  virtual void addRow(const std::vector<FFuComponentBase*>& cells, int row = -1) = 0;

  //! Inserts item at the position.
  virtual void insertWidget(int row, int col, FFuComponentBase* item) = 0;
  //! Gets item at the position.
  virtual FFuComponentBase* getWidget(int row, int col) = 0;
  //! Inserts a text at the position.
  virtual void insertText(int row, int col, const std::string& text) = 0;
  //! Fetches the text at the position. Returns empty string if not text.
  virtual std::string getText(int row, int col) = 0;

  // Interfaces to QTableWidgetItem objects instead of implementing FFu wrapper

  //! Inserts a combo item into the cell. CB on changed is accessed through FFuTable.
  virtual void insertComboItem(int row, int col, const std::vector<std::string>& items) = 0;
  //! Gets current selection.
  virtual int getComboItemSelection(int row, int col) = 0;
  //! Sets new selection.
  virtual void setComboItemCurrent(int row, int col, int i) = 0;
  //! Inserts a check box item to the cell. CB on changed accessed through FFuTable.
  virtual void insertCheckBoxItem(int row, int col, const std::string& text, bool setChecked) = 0;
  //! Retrieves current toggle.
  virtual bool getCheckBoxItemToggle(int row, int col) = 0;
  //! Sets a toggle value.
  virtual void setCheckBoxItemToggle(int row, int col, bool toggle) = 0;

  /*! Deletes widgets and sets sizes to 0. \sa clearTableItems(). */
  void clearTable();
  /*! Deletes items (not widgets?) contained in table, but leaves dimensions */
  void clearTableItems();
  /*! Deletes the widget at position */
  virtual void clearCellContents(int row, int cell) = 0;
  /*! Deletes the widgets in row */
  virtual void deleteRow(int row) = 0;
  /*! Deletes the widgets in col */
  virtual void deleteColumn(int col) = 0;

  /*! Sets column header label */
  virtual void setColumnLabel(int col, const std::string& text) = 0;
  /*! Sets column header label and associated initial relative width */
  void setColumnLabel(size_t col, const std::string& text, double relWidth);
  /*! Gets column header label */
  virtual std::string getColumnLabel(int col) = 0;
  /*! Sets row header label */
  virtual void setRowLabel(int row, const std::string& text) = 0;
  /*! Show/hide column header */
  virtual void showColumnHeader(bool show) = 0;
  /*! Show/hide row header */
  virtual void showRowHeader(bool show) = 0;
  /*! Set the hight of the column header row */
  virtual void setColumnHeaderHeight(int height) = 0;
  /*! Set the width of the row header column */
  virtual void setRowHeaderWidth(int width) = 0;
  /*! Get the width of the row header column */
  virtual int getColumnHeaderHeight() = 0;
  /*! Get the width of the row header column */
  virtual int getRowHeaderWidth() = 0;

  /*! Show/hide grid */
  virtual void showTableGrid(bool show) = 0;

  /*! Selection policies */
  enum {SINGLE,
	MULTI,
	SINGLE_ROW,
	MULTI_ROW,
	NO_SELECTION};

  /*! Set selection policy according to enum */
  virtual void setSelectionPolicy(int policy) = 0;
  /*! Retrieve selection policy */
  virtual int  getSelectionPolicy() = 0;

  /*! Retrieve row count */
  virtual int getNumberRows() = 0;
  /*! Retrieve column count */
  virtual int getNumberColumns() = 0;

  /*! Set number of rows */
  virtual void setNumberRows(int num) = 0;
  /*! Set number of columns */
  virtual void setNumberColumns(int cols) = 0;
  /*! Set number of columns and with relative column widths */
  void setNumberColumns(size_t cols, bool withRelativeWidths);

  virtual int getRowHeight(int row) = 0;
  virtual int getColumnWidth(int col) = 0;

  virtual void setColumnWidth(int col, int width) = 0;
  virtual void setRowHeight(int row, int height) = 0;

  void setColumnWidth(int width);
  void setRowHeight(int height);

  /*! Allow row to be stretchable */
  virtual void stretchRowHeight(int row, bool stretchable) = 0;
  /*! Allow col to be stretchable */
  virtual void stretchColWidth(int col, bool stretchable) = 0;

  void stretchRowsHeight(bool stretchable);
  void stretchColsWidth(bool stretchable);

  /*! Adjust column width so that the column is wide enough to display the widest cell */
  virtual void adjustColumnWidth(int col) = 0;
  /*! Adjust the row height so that the row is high enough to display the highest cell */
  virtual void adjustRowHeight(int row) = 0;

  virtual int getCurrentRow() = 0;
  virtual int getCurrentCol() = 0;

  virtual void setSensitivity(bool isSensitive) = 0;
  virtual void setCellSensitivity(int row, int col, bool isSensitive) = 0;

  void setRowSensitivity(int row, bool isSensitive);
  void setColumnSensitivity(int col, bool isSensitive);

  virtual void setTableCellReadOnly(int row, int col, bool ro) = 0;

  void setTableReadOnly(bool ro);
  void setTableRowReadOnly(int row, bool ro);
  void setTableColumnReadOnly(int col, bool ro);

  void setSelectionChangedCB  (const FFaDynCB0&          cb) { mySelectionChangedCB = cb; }
  void setCurrentCellChangedCB(const FFaDynCB2<int,int>& cb) { myCurrentCellChangedCB = cb; }
  void setCellValueChangedCB  (const FFaDynCB2<int,int>& cb) { myCellValueChangedCB = cb; }
  void setCellPressedCB       (const FFaDynCB2<int,int>& cb) { myCellPressedCB = cb; }
  void setCellClickedCB       (const FFaDynCB2<int,int>& cb) { myCellClickedCB = cb; }

protected:
  FFaDynCB0          mySelectionChangedCB;
  FFaDynCB2<int,int> myCurrentCellChangedCB;
  FFaDynCB2<int,int> myCellValueChangedCB;
  FFaDynCB2<int,int> myCellPressedCB;
  FFaDynCB2<int,int> myCellClickedCB;

  void columnSizeChanged(int col, int oldSize, int newSize);

public:
  void updateColumnWidths();

private:
  std::vector<double> myRelColWidths;
  int                 resizingColumn;
};

#endif
