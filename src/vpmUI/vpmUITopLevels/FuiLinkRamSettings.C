// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <cmath>

#include "vpmUI/vpmUITopLevels/FuiLinkRamSettings.H"
#include "FFuLib/FFuTable.H"


enum Columns {
  ID,
  NAME,
  LEVEL
};


Fmd_SOURCE_INIT(FUI_LINKRAMSETTINGS, FuiLinkRamSettings, FFuTopLevelShell);


FuiLinkRamSettings::FuiLinkRamSettings()
{
  Fmd_CONSTRUCTOR_INIT(FuiLinkRamSettings);
}


void FuiLinkRamSettings::initWidgets()
{
  myTable->showRowHeader(false);
  myTable->showColumnHeader(true);
  myTable->setNumberColumns(3);

  myTable->setColumnLabel(ID,    "Id");
  myTable->setColumnLabel(NAME,  "Part                      ");
  myTable->setColumnLabel(LEVEL, "Status       ");

  myTable->adjustColumnWidth(ID);
  myTable->adjustColumnWidth(NAME);
  myTable->adjustColumnWidth(LEVEL);

  myRelColWidths.push_back( 50*myTable->getColumnWidth(ID));
  myRelColWidths.push_back( 50*myTable->getColumnWidth(NAME));
  myRelColWidths.push_back( 50*myTable->getColumnWidth(LEVEL));

  myTable->setRowHeight(50);

  myTable->setTableColumnReadOnly(ID, true);
  myTable->setTableColumnReadOnly(NAME, true);

  myTable->setSelectionPolicy(FFuTable::NO_SELECTION);

/* Does not work with Qt4 due to unexpected recursion
  myTable->setColumnResizedCB(FFaDynCB3M(FuiLinkRamSettings, this, columnSizeChanged, int, int, int));
*/
  myTable->setCellDoubleClicked(FFaDynCB3M(FuiLinkRamSettings, this, tableDoubleClicked, int, int, int));

  this->FuiTopLevelDialog::initWidgets();
}


void FuiLinkRamSettings::placeWidgets(int width, int height)
{
  myTable->setEdgeGeometry(3, width-3, 3, height-this->getDialogButtonsHeightHint());
  this->FuiTopLevelDialog::placeWidgets(width,height);
  this->updateColumnWidths();
}


FFuaUIValues* FuiLinkRamSettings::createValuesObject()
{
  return new FuaLinkRamSettingsValues();
}


void FuiLinkRamSettings::setUIValues(const FFuaUIValues* values)
{
  const FuaLinkRamSettingsValues* uiv = (const FuaLinkRamSettingsValues*)values;

  int oldRows = myTable->getNumberRows();
  int newRows = oldRows;

  // Lambda function returning the row index for a given partId.
  auto&& getRowIndex = [this,oldRows](const std::string& partId)
  {
    for (int j = 0; j < oldRows; j++)
      if (partId == myTable->getText(j,ID)) return j;
    return -1;
  };

  for (const FuaLinkRamSettingsValues::RamSetting& lrs : uiv->rowData)
    if (getRowIndex(lrs.id) < 0) newRows++;

  if (newRows > oldRows)
    myTable->setNumberRows(newRows);
  std::vector<bool> usedRows(newRows,false);

  newRows = oldRows;
  for (const FuaLinkRamSettingsValues::RamSetting& lrs : uiv->rowData)
  {
    int j = getRowIndex(lrs.id);
    if (j < 0) j = newRows++;

    myTable->insertText(j, ID, lrs.id);
    myTable->insertText(j, NAME, lrs.description);
    if (j >= oldRows)
      myTable->insertComboItem(j, LEVEL, uiv->ramLevelNames);
    myTable->setComboItemCurrent(j, LEVEL, lrs.ramLevel);
    usedRows[j] = true;
  }

  for (int j = newRows-1; j >= 0; --j)
    if (!usedRows[j])
      myTable->deleteRow(j);
}


void FuiLinkRamSettings::getUIValues(FFuaUIValues* values)
{
  FuaLinkRamSettingsValues* uiv = (FuaLinkRamSettingsValues*) values;

  uiv->rowData.clear();
  FuaLinkRamSettingsValues::RamSetting lData;
  for (int i = 0; i < myTable->getNumberRows(); i++)
  {
    lData.id = myTable->getText(i,ID);
    lData.description = myTable->getText(i,NAME);
    lData.ramLevel = myTable->getComboItemSelection(i,LEVEL);
    uiv->rowData.push_back(lData);
  }
}


void FuiLinkRamSettings::updateColumnWidths()
{
  int totColumnWidth = this->getWidth() - myTable->getRowHeaderWidth() - 10 - 18;
  size_t i;
  double totalRelWidth = 0.0;
  for (i = 0; i < myRelColWidths.size(); ++i)
    totalRelWidth += myRelColWidths[i];

  double columnUnitWidth = totColumnWidth/totalRelWidth;
  double leftovers = 0.0; double dummy;
  for (i = 0; i < myRelColWidths.size(); ++i)
    leftovers += modf(columnUnitWidth*myRelColWidths[i], &dummy);

  for (i = 0; i < myRelColWidths.size()-1; ++i)
    {
      double colW  = columnUnitWidth*myRelColWidths[i];
      if ( (modf(colW, &dummy) >= 0.5) && (leftovers > 0))
        {
          myTable->setColumnWidth(i, (int)(colW + 0.5));
          --leftovers;
        }
      else
        myTable->setColumnWidth(i, (int)(colW));
    }
  myTable->setColumnWidth(i, (int)((columnUnitWidth*myRelColWidths[i]) + leftovers));
}


void FuiLinkRamSettings::columnSizeChanged(int col, int oldSize, int newSize)
{
  double totalRelWidth = 0.0;
  size_t i;
  for (i = 0; i < myRelColWidths.size(); ++i)
    totalRelWidth += myRelColWidths[i];
  int totColumnWidth = this->getWidth() - myTable->getRowHeaderWidth() - 10 - 18;

  size_t ucol = col > 0 ? col : 0;
  int columnWidthsBeforeChangedOne = 0;
  for (i = 0; i < ucol; ++i)
    columnWidthsBeforeChangedOne += myTable->getColumnWidth(i);

  if (newSize + columnWidthsBeforeChangedOne >= totColumnWidth - 5)
    newSize = oldSize;

  double newRelColWidth = newSize * myRelColWidths[col]/oldSize;
  double difference = newRelColWidth - myRelColWidths[col];
  myRelColWidths[col] = newRelColWidth;

  double totRelColWidthRest = 0.0;
  for (i = ucol+1; i < myRelColWidths.size(); ++i)
    totRelColWidthRest += myRelColWidths[i];

  for (i = ucol+1; i < myRelColWidths.size(); ++i)
    myRelColWidths[i] -= myRelColWidths[i]*difference/totRelColWidthRest;

  this->updateColumnWidths();
}


void FuiLinkRamSettings::tableDoubleClicked(int, int, int)
{
  this->updateColumnWidths();
}
