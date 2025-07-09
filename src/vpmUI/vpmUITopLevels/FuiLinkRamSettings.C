// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

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
  myTable->setNumberColumns(3, true);

  myTable->setColumnLabel(ID,    "Id",     10);
  myTable->setColumnLabel(NAME,  "Part",   70);
  myTable->setColumnLabel(LEVEL, "Status", 20);

  myTable->setRowHeight(50);

  myTable->setTableColumnReadOnly(ID, true);
  myTable->setTableColumnReadOnly(NAME, true);

  myTable->setSelectionPolicy(FFuTable::NO_SELECTION);

  this->FuiTopLevelDialog::initWidgets();
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
