// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuQtComponents/FFuQtTable.H"

#include <QHeaderView>
#include <QComboBox>


FFuQtTable::FFuQtTable(QWidget* parent, bool hs, bool vs) : QTableWidget(parent)
{
  this->setWidget(this);

  QObject::connect(this, SIGNAL(itemSelectionChanged()),
                   this, SLOT(fwdSelectionChanged()));
  QObject::connect(this, SIGNAL(cellChanged(int,int)),
                   this, SLOT(fwdCurrentCellChanged(int,int)));
  QObject::connect(this, SIGNAL(cellClicked(int,int)),
                   this, SLOT(fwdCellClicked(int,int)));
  QObject::connect(this, SIGNAL(cellDoubleClicked(int,int)),
                   this, SLOT(fwdCellDoubleClicked(int,int)));
  QObject::connect(this, SIGNAL(cellPressed(int,int)),
                   this, SLOT(fwdCellPressed(int,int)));
  QObject::connect(this->horizontalHeader(), SIGNAL(sectionResized(int,int,int)),
                   this, SLOT(fwdColumnResized(int,int,int)));
  QObject::connect(this, SIGNAL(cellChanged(int,int)),
                   this, SLOT(fwdCellValueChanged(int,int)));

  this->setHorizontalScrollBarPolicy(hs ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
  this->setVerticalScrollBarPolicy(vs ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);

  this->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
}


void FFuQtTable::addRow(const std::vector<FFuComponentBase*>& cells, int row)
{
  int rowCount = this->rowCount();
  int colCount = this->columnCount();

  if (colCount > (int)cells.size())
    this->setColumnCount(cells.size());

  if (row < 0 || row >= rowCount)
    this->setRowCount((row = rowCount) + 1);

  QWidget* cell;
  for (size_t col = 0; col < cells.size(); col++)
    if ((cell = dynamic_cast<QWidget*>(cells[col])))
      this->setCellWidget(row, col, cell);
}

void FFuQtTable::deleteRow(int row)
{
  this->removeRow(row);
}

void FFuQtTable::deleteColumn(int col)
{
  this->removeColumn(col);
}

void FFuQtTable::setColumnLabel(int col, const std::string& text)
{
  this->setHorizontalHeaderItem(col, new QTableWidgetItem(text.c_str()));
}

std::string FFuQtTable::getColumnLabel(int col)
{
  return this->horizontalHeaderItem(col)->text().toStdString();
}

void FFuQtTable::setRowLabel(int row, const std::string& text)
{
  this->setVerticalHeaderItem(row, new QTableWidgetItem(text.c_str()));
}

void FFuQtTable::showColumnHeader(bool show)
{
  if (show)
    this->horizontalHeader()->show();
  else
    this->horizontalHeader()->hide();
}

void FFuQtTable::showRowHeader(bool show)
{
  if (show)
    this->verticalHeader()->show();
  else
    this->verticalHeader()->hide();
}

void FFuQtTable::setColumnHeaderHeight(int height)
{
  int width = this->horizontalHeader()->width();
  this->horizontalHeader()->resize(width, height);
}

void FFuQtTable::setRowHeaderWidth(int width)
{
  int height = this->verticalHeader()->height();
  this->verticalHeader()->resize(width, height);
}

int FFuQtTable::getColumnHeaderHeight()
{
  return this->horizontalHeader()->height();
}

int FFuQtTable::getRowHeaderWidth()
{
  return this->verticalHeader()->width();
}

void FFuQtTable::showTableGrid(bool show)
{
  this->setShowGrid(show);
}

void FFuQtTable::setSelectionPolicy(int policy)
{
  switch (policy) {
  case SINGLE:
    this->setSelectionMode(SingleSelection);
    this->setSelectionBehavior(SelectItems);
    break;

  case MULTI:
    this->setSelectionMode(MultiSelection);
    this->setSelectionBehavior(SelectItems);
    break;

  case SINGLE_ROW:
    this->setSelectionMode(SingleSelection);
    this->setSelectionBehavior(SelectRows);
    break;

  case MULTI_ROW:
    this->setSelectionMode(MultiSelection);
    this->setSelectionBehavior(SelectRows);
    break;

  case NO_SELECTION:
    this->setSelectionMode(NoSelection);
    this->setSelectionBehavior(SelectItems);
    break;

  default:
    this->setSelectionMode(SingleSelection);
    this->setSelectionBehavior(SelectItems);
    break;
  }
}

int FFuQtTable::getSelectionPolicy()
{
  switch (this->selectionMode()) {
  case MultiSelection:
    return this->selectionBehavior() == SelectRows ? MULTI_ROW : MULTI;
  case SingleSelection:
    return this->selectionBehavior() == SelectRows ? SINGLE_ROW : SINGLE;
  case NoSelection:
    return NO_SELECTION;
  default:
    return -1;
  }
}

void FFuQtTable::insertWidget(int row, int col, FFuComponentBase* item)
{
  this->removeCellWidget(row, col);

  QWidget* cell = dynamic_cast<QWidget*>(item);
  if (cell) this->setCellWidget(row, col, cell);
}

FFuComponentBase* FFuQtTable::getWidget(int row, int col)
{
  return dynamic_cast<FFuComponentBase*>(this->cellWidget(row,col));
}

void FFuQtTable::insertText(int row, int col, const std::string& text)
{
  QTableWidgetItem* item = this->item(row, col);
  if (item)
    item->setText(text.c_str());
  else
    this->setItem(row, col, new QTableWidgetItem(text.c_str()));
}

std::string FFuQtTable::getText(int row, int col)
{
  QTableWidgetItem* item = this->item(row, col);
  return item ? item->text().toStdString() : "";
}

void FFuQtTable::insertComboItem(int row, int col, const std::vector<std::string>& items)
{
  QComboBox* combo = dynamic_cast<QComboBox*>(this->cellWidget(row,col));
  if (combo)
    combo->clear();
  else
  {
    this->removeCellWidget(row, col);
    this->setCellWidget(row, col, combo = new QComboBox());
  }

  for (const std::string& item : items)
    combo->addItem(item.c_str());
}

void FFuQtTable::setComboItemCurrent(int row, int col, int i)
{
  if (!this->isInsideTable(row, col)) return;

  QComboBox* combo = dynamic_cast<QComboBox*>(this->cellWidget(row,col));
  if (!combo) return;

  combo->setCurrentIndex(i);
}

int FFuQtTable::getComboItemSelection(int row, int col)
{
  if (!this->isInsideTable(row, col)) return -1;

  QComboBox* combo = dynamic_cast<QComboBox*>(this->cellWidget(row,col));
  if (!combo) return -2;

  return combo->currentIndex();
}

void FFuQtTable::insertCheckBoxItem(int row, int col, const std::string& text, bool checked)
{
  QTableWidgetItem* item = this->item(row, col);
  if (item)
    item->setText(text.c_str());
  else
    this->setItem(row, col, item = new QTableWidgetItem(text.c_str()));

  item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
  item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
}

bool FFuQtTable::getCheckBoxItemToggle(int row, int col)
{
  if (!this->isInsideTable(row, col)) return false;

  QTableWidgetItem* item = this->item(row, col);
  if (item && (item->flags() & Qt::ItemIsUserCheckable))
    return item->checkState() == Qt::Checked;

  return false;
}

void FFuQtTable::setCheckBoxItemToggle(int row, int col, bool toggle)
{
  if (!this->isInsideTable(row, col)) return;

  QTableWidgetItem* item = this->item(row, col);
  if (item && (item->flags() & Qt::ItemIsUserCheckable))
    item->setCheckState(toggle ? Qt::Checked : Qt::Unchecked);
}

void FFuQtTable::clearCellContents(int row, int col)
{
  if (!this->isInsideTable(row, col)) return;

  this->removeCellWidget(row, col);
}

int FFuQtTable::getNumberRows()
{
  return this->rowCount();
}

int FFuQtTable::getNumberColumns()
{
  return this->columnCount();
}

void FFuQtTable::setNumberRows(int num)
{
  this->setRowCount(num);
}

void FFuQtTable::setNumberColumns(int num)
{
  this->setColumnCount(num);
}

int FFuQtTable::getRowHeight(int row)
{
  return this->rowHeight(row);
}

int FFuQtTable::getColumnWidth(int col)
{
  return this->columnWidth(col);
}

void FFuQtTable::setColumnWidth(int col, int width)
{
  this->QTableWidget::setColumnWidth(col, width);
}

void FFuQtTable::setRowHeight(int row, int height)
{
  this->QTableWidget::setRowHeight(row, height);
}

void FFuQtTable::stretchRowHeight(int row, bool stretchable)
{
  if (row < 0 || row > this->rowCount()) // Only possible for last row with Qt4
    this->horizontalHeader()->setStretchLastSection(stretchable);
}

void FFuQtTable::stretchColWidth(int col, bool stretchable)
{
  if (col < 0 || col > this->columnCount()) // Only possible for last column with Qt4
    this->horizontalHeader()->setStretchLastSection(stretchable);
}

void FFuQtTable::adjustColumnWidth(int col)
{
  this->resizeColumnToContents(col);
}

void FFuQtTable::adjustRowHeight(int row)
{
  this->resizeRowToContents(row);
}

int FFuQtTable::getCurrentRow()
{
  return this->currentRow();
}

int FFuQtTable::getCurrentCol()
{
  return this->currentColumn();
}

void FFuQtTable::setSensitivity(bool isSensitive)
{
  this->setEnabled(isSensitive);

  for (int row = 0; row < this->rowCount(); row++)
    this->setRowSensitivity(row, isSensitive);
}

void FFuQtTable::setCellSensitivity(int row, int col, bool isSensitive)
{
  if (!this->isInsideTable(row,col)) return;

  FFuComponentBase* ffuCell = this->getWidget(row,col);
  QComboBox* combo = dynamic_cast<QComboBox*>(this->cellWidget(row,col));
  if (ffuCell)
    ffuCell->setSensitivity(isSensitive);
  else if (combo)
    combo->setEnabled(isSensitive);

  QTableWidgetItem* item = this->item(row, col);
  if (item && (item->flags() & Qt::ItemIsUserCheckable))
    item->setFlags(item->flags() & (isSensitive ? Qt::ItemIsEnabled : ~Qt::ItemIsEnabled));
}

void FFuQtTable::setTableCellReadOnly(int row, int col, bool ro)
{
  if (!this->isInsideTable(row,col)) return;

  QTableWidgetItem* item = this->item(row, col);
  if (item)
    item->setFlags(item->flags() & (ro ? ~Qt::ItemIsEditable : Qt::ItemIsEditable));
}

bool FFuQtTable::isInsideTable(int row, int col) const
{
  return row >= 0 && col >= 0 && row <= this->rowCount() && col <= columnCount();
}
