// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiExtCtrlSysProperties.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "FFuLib/FFuFileBrowseField.H"
#include "FFuLib/FFuTable.H"
#include "FFuLib/FFuLabel.H"


//----------------------------------------------------------------------------

void FuiExtCtrlSysProperties::initWidgets()
{
  // Setting up file browse field
  this->fileBrowseField->setLabel("Simulink file:");
  this->fileBrowseField->setAbsToRelPath("yes");
  this->fileBrowseField->addDialogFilter("Simulink file", "mdl", true);
  this->fileBrowseField->setDialogRememberKeyword("ExternalCtrlSys");

  // Callback for opened file
  this->fileBrowseField->setFileOpenedCB(FFaDynCB2M(FuiExtCtrlSysProperties, this,
						    onFileOpened, const std::string&, int));

  // Set up table
  myTable->showColumnHeader(false);
  myTable->showRowHeader(false);
  myTable->setSelectionPolicy(FFuTable::NO_SELECTION);
  myTable->showTableGrid(true);
  myTable->setTableReadOnly(true);

  // create ui's UA object
  FFuUAExistenceHandler::invokeCreateUACB(this);
}

//----------------------------------------------------------------------------

void FuiExtCtrlSysProperties::setEngineSelectedCB(const FFaDynCB1<FuiQueryInputFieldValues&>& aDynCB)
{
  myButtonPressedCB = aDynCB;
}

//----------------------------------------------------------------------------

/*!
  Called when a, possibly, new file has been selected
  Will update DB values, as well as building the UI again with new engines
*/
void FuiExtCtrlSysProperties::onFileOpened(const std::string&, int)
{
  this->updateDBValues(false);
}

//-----------------------------------------------------------------------------

FFuaUIValues* FuiExtCtrlSysProperties::createValuesObject()
{
  return new FuaExtCtrlSysPropertiesValues();
}

//----------------------------------------------------------------------------

void FuiExtCtrlSysProperties::getUIValues(FFuaUIValues* values)
{
  // Updating filepath and the value of readFile in externalElementValues.
  FuaExtCtrlSysPropertiesValues* vals = dynamic_cast<FuaExtCtrlSysPropertiesValues*>(values);
  if (!vals) return;

  // Updating path
  vals->filePath = this->fileBrowseField->getFileName();

  // Updating selected engines
  vals->mySelectedQueries.clear();

  for (FuiQueryInputField* field : myInpEngineFields)
    vals->mySelectedQueries.push_back(field->getSelectedRef());
}

//----------------------------------------------------------------------------

/*!
  When an engine button is pressed, invoke cb from ua
*/
void FuiExtCtrlSysProperties::onEngineButtonPressed(FuiQueryInputFieldValues& query)
{
  myButtonPressedCB.invoke(query);
}

//----------------------------------------------------------------------------

/*!
  When field selection has changed, update db values
*/
void FuiExtCtrlSysProperties::onFieldSelectionChanged(int)
{
  this->updateDBValues();
}

//----------------------------------------------------------------------------

/*!
  Sets values from the values object into the UI
*/
void FuiExtCtrlSysProperties::setUIValues(const FFuaUIValues* values)
{
  // Updates the GUI with the values retrieved from the "values" parameter
  const FuaExtCtrlSysPropertiesValues* externalValues = (FuaExtCtrlSysPropertiesValues*) values;

  this->fileBrowseField->setAbsToRelPath(externalValues->modelFilePath);
  this->fileBrowseField->setFileName(externalValues->filePath);

  // Setting selected
  for (size_t i = 0; i < externalValues->mySelectedQueries.size(); i++) {
    this->myInpEngineFields[i]->setSelectedRef(externalValues->mySelectedQueries[i]);
    this->myInpEngineFields[i]->setSensitivity(IAmSensitive);
  }
}

//----------------------------------------------------------------------------

/*!
  Build dynamic widgets, using the values object
*/
void FuiExtCtrlSysProperties::buildDynamicWidgets(const FFuaUIValues* values)
{
  const FuaExtCtrlSysPropertiesValues* externalValues = (FuaExtCtrlSysPropertiesValues*) values;
  if (!externalValues) return;

  // Rebuild table
  int numCols = myTable->getNumberColumns();
  int numRows = myTable->getNumberRows();
  int newRows = externalValues->fromWorkspace.size();
  for (int row = newRows; row < numRows; row++)
    for (int col = 0; col < numCols; col++)
      myTable->clearCellContents(row, col);

  if (newRows < numRows) {
    myNameLabels.resize(newRows);
    myInpEngineFields.resize(newRows);
  }
  else for (int i = numRows; i < newRows; i++)
    this->makeNewFields();

  // Adjusting table
  myTable->setNumberRows(newRows);
  myTable->setNumberColumns(2);

  // Bulding contents of scroll window
  for (size_t x = 0; x < myInpEngineFields.size(); x++) {
    myNameLabels[x]->setLabel(externalValues->fromWorkspace[x]);
    myInpEngineFields[x]->setQuery(externalValues->myEngineQuery);
    myInpEngineFields[x]->setBehaviour(FuiQueryInputField::REF_NONE);
    myInpEngineFields[x]->setButtonCB(FFaDynCB1M(FuiExtCtrlSysProperties, this,
						 onEngineButtonPressed, FuiQueryInputFieldValues&));
    myInpEngineFields[x]->setRefSelectedCB(FFaDynCB1M(FuiExtCtrlSysProperties, this,
						      onFieldSelectionChanged, int));
    if ((int)x >= numRows) {
      myTable->insertWidget(x, 0, myNameLabels[x]);
      myTable->insertWidget(x, 1, myInpEngineFields[x]);
    }
  }

  myTable->stretchColsWidth(true);
}

//----------------------------------------------------------------------------

void FuiExtCtrlSysProperties::setSensitivity(bool isSensitive)
{
  IAmSensitive = isSensitive;

  fileBrowseField->setSensitivity(isSensitive);
  for (FuiQueryInputField* field : myInpEngineFields)
    field->setSensitivity(isSensitive);
}

//----------------------------------------------------------------------------
