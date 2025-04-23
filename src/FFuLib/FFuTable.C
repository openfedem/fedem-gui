// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <numeric>

#include "FFuLib/FFuTable.H"


void FFuTable::clearTable()
{
  this->clearTableItems();
  this->setNumberRows(0);
  this->setNumberColumns(0);
}


void FFuTable::clearTableItems()
{
  for (int i = 0; i < this->getNumberRows(); i++)
    this->deleteRow(i);
}


void FFuTable::setColumnWidth(int width)
{
  for (int col = 0; col < this->getNumberColumns(); col++)
    this->setColumnWidth(col, width);
}

void FFuTable::setRowHeight(int height)
{
  for (int row = 0; row < this->getNumberRows(); row++)
    this->setRowHeight(row, height);
}


void FFuTable::stretchRowsHeight(bool stretchable)
{
  for (int row = 0; row < this->getNumberRows(); row++)
    this->stretchRowHeight(row, stretchable);
}

void FFuTable::stretchColsWidth(bool stretchable)
{
  for (int col = 0; col < this->getNumberColumns(); col++)
    this->stretchColWidth(col, stretchable);
}


void FFuTable::setRowSensitivity(int row, bool isSensitive)
{
  for (int col = 0; col < this->getNumberColumns(); col++)
  {
    this->setCellSensitivity(row, col, isSensitive);
    this->setTableCellReadOnly(row, col, !isSensitive);
  }
}

void FFuTable::setColumnSensitivity(int col, bool isSensitive)
{
  for (int row = 0; row < this->getNumberRows(); row++)
  {
    this->setCellSensitivity(row, col, isSensitive);
    this->setTableCellReadOnly(row, col, !isSensitive);
  }
}


void FFuTable::setTableReadOnly(bool ro)
{
  for (int row = 0; row < this->getNumberRows(); row++)
    this->setTableRowReadOnly(row, ro);
}


void FFuTable::setTableRowReadOnly(int row, bool ro)
{
  for (int col = 0; col < this->getNumberColumns(); col++)
    this->setTableCellReadOnly(row, col, ro);
}

void FFuTable::setTableColumnReadOnly(int col, bool ro)
{
  for (int row = 0; row < this->getNumberRows(); row++)
    this->setTableCellReadOnly(row, col, ro);
}


void FFuTable::setNumberColumns(size_t cols, bool withRelativeWidths)
{
  this->setNumberColumns(cols);

  if (withRelativeWidths && cols > 0)
    myRelColWidths.resize(cols,100.0/static_cast<double>(cols));
  else
    myRelColWidths.clear();
}


void FFuTable::setColumnLabel(size_t col, const std::string& text, double relWidth)
{
  this->setColumnLabel(col,text);

  if (col < myRelColWidths.size())
    myRelColWidths[col] = relWidth;
}


void FFuTable::updateColumnWidths()
{
  if (myRelColWidths.empty())
    return;

  double unitWidth = (this->getWidth() - this->getRowHeaderWidth() - 4) /
    std::accumulate(myRelColWidths.begin(), myRelColWidths.end(), 0.0);

  double dummy; // Sum up the remainders when truncating the column widths
  double leftovers = std::accumulate(myRelColWidths.begin(),
                                     myRelColWidths.end(), 0.0,
                                     [&dummy,unitWidth](double x, double y)
                                     { return x + modf(unitWidth*y,&dummy); });

  size_t lastCol = myRelColWidths.size() - 1;
  for (size_t ic = 0; ic < lastCol; ic++)
  {
    // Find the new actual column width
    double colW = unitWidth*myRelColWidths[ic];
    if (leftovers > 0.0 && modf(colW,&dummy) >= 0.5)
    {
      colW += 0.5;
      --leftovers;
    }
    resizingColumn = ic;
    this->setColumnWidth(ic, static_cast<int>(colW));
  }

  // The leftovers (if any) are added to the last column
  resizingColumn = lastCol;
  double colWend = unitWidth*myRelColWidths.back() + leftovers;
  this->setColumnWidth(lastCol, static_cast<int>(colWend));
  resizingColumn = -1;
}


void FFuTable::columnSizeChanged(int col, int oldSize, int newSize)
{
  if (myRelColWidths.empty() || col == resizingColumn)
    return; // avoid invalid recursion by QHeaderView::sectionResized() signal

  int maxSize = this->getWidth() - this->getRowHeaderWidth() - 9;
  for (int j = 0; j < col; j++)
    maxSize -= this->getColumnWidth(j);
  if (newSize >= maxSize)
    newSize = oldSize;

  // Find new relative width of this column
  double newRelColWidth = newSize * myRelColWidths[col]/oldSize;
  double difference = newRelColWidth - myRelColWidths[col];
  myRelColWidths[col] = newRelColWidth;

  if (col+1 < static_cast<int>(myRelColWidths.size()))
  {
    // Adjust the relative widths of the columns after this one
    double totRelColWidthRest = std::accumulate(myRelColWidths.begin()+col+1,
                                                myRelColWidths.end(), 0.0);
    for (size_t i = col+1; i < myRelColWidths.size(); i++)
      myRelColWidths[i] -= myRelColWidths[i]*difference/totRelColWidthRest;
  }

  this->updateColumnWidths();
}
