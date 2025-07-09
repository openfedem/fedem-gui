// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiPreferences.H"
#include "vpmApp/FapLicenseManager.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuRadioButton.H"
#include "FFuLib/FFuDialogButtons.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <iostream>


Fmd_SOURCE_INIT(FUI_PREFERENCES, FuiPreferences, FFuTopLevelShell);


//----------------------------------------------------------------------------

FuiPreferences::FuiPreferences()
{
  Fmd_CONSTRUCTOR_INIT(FuiPreferences);

  this->optFields.fill(NULL);
  this->labels.fill(NULL);

  this->reducerFrame = NULL;
  this->eqSolverToggle     = this->recMatrixToggle = NULL;
  this->eqSolverAutomatic  = this->eqSolverManual  = NULL;
  this->recMatrixAutomatic = this->recMatrixManual = NULL;

  this->remoteSolveFrame = NULL;
  this->solverPrefixToggle = this->solverPathToggle = NULL;

  this->isRestartActive = false;
  this->isSensitive = true;
}
//----------------------------------------------------------------------------

void FuiPreferences::initWidgets()
{
  for (FFuIOField* field : this->optFields) {
    field->setInputCheckMode(FFuIOField::NOCHECK);
    field->setAcceptPolicy(FFuIOField::ENTERONLY);
  }

  this->optFields[MAX_CONC_PROC]->setInputCheckMode(FFuIOField::INTEGERCHECK);
  this->optFields[EQ_SOLVER_SWAP]->setInputCheckMode(FFuIOField::INTEGERCHECK);
  this->optFields[REC_MATRIX_SWAP]->setInputCheckMode(FFuIOField::INTEGERCHECK);

  this->labels[REDUCER]->setLabel("FE model Reducer");
  this->labels[SOLVER]->setLabel("Dynamics Solver");
  this->labels[STRESS]->setLabel("Stress Recovery");
  this->labels[MODES]->setLabel("Mode Shape Recovery");
  this->labels[GAGE]->setLabel("Strain Rosette Recovery");
  this->labels[FPP]->setLabel("Strain Coat Recovery Summary");
  this->labels[MAX_CONC_PROC]->setLabel("Max concurrent processes");
  this->labels[EQ_SOLVER_SWAP]->setLabel("[MB]");
  this->labels[REC_MATRIX_SWAP]->setLabel("[MB]");
  this->labels[SOLVER_PREFIX]->setLabel("Remote shell command prefix:");
  //this->labels[SOLVER_PATH]->setLabel("Model path on remote system:");

  this->reducerFrame->setLabel("FE model Reducer Out-of-core Options ");
  this->eqSolverToggle->setLabel("Equation solver out-of-core");
  this->eqSolverToggle->setToggleCB(FFaDynCB1M(FuiPreferences,this,
					       onEqSolverToggeled,bool));

  this->eqSolverAutomatic->setLabel("Automatic");
  this->eqSolverManual->setLabel("Manual");
  this->eqSolverBtnGroup.insert(this->eqSolverAutomatic);
  this->eqSolverBtnGroup.insert(this->eqSolverManual);
  this->eqSolverBtnGroup.setValue(this->eqSolverAutomatic,true);
  this->eqSolverBtnGroup.setGroupToggleCB(FFaDynCB2M(FuiPreferences,this,
                                                     onEqSolverGroupToggeled,
                                                     int,bool));

  this->recMatrixToggle->setLabel("Recovery matrix out-of-core");
  this->recMatrixToggle->setToggleCB(FFaDynCB1M(FuiPreferences,this,
						onRecMatrixToggeled,bool));
  this->recMatrixAutomatic->setLabel("Automatic");
  this->recMatrixManual->setLabel("Manual");
  this->recMatrixBtnGroup.insert(this->recMatrixAutomatic);
  this->recMatrixBtnGroup.insert(this->recMatrixManual);
  this->recMatrixBtnGroup.setValue(this->recMatrixAutomatic,true);
  this->recMatrixBtnGroup.setGroupToggleCB(FFaDynCB2M(FuiPreferences,this,
                                                      onRecMatrixGroupToggeled,
                                                      int,bool));

  this->remoteSolveFrame->setLabel("Remote solve options");
  this->solverPrefixToggle->setLabel("Perform remote solve");
  this->solverPrefixToggle->setToggleCB(FFaDynCB1M(FuiPreferences,this,
						   onSolvePrefixToggeled,bool));
  this->solverPathToggle->setLabel("Model path on remote system:");
  this->solverPathToggle->setToggleCB(FFaDynCB1M(FuiPreferences,this,
						 onSolvePathToggeled,bool));

  this->FuiTopLevelDialog::initWidgets();
}

//-----------------------------------------------------------------------------

bool FuiPreferences::updateDBValues(bool)
{
  //unhighligh fields since qt enables mult field highlighting (qt bug ?)
  for (FFuIOField* field : this->optFields)
    field->unHighlight();

  //get ui values
  FuaPreferencesValues values;
  for (int iField = 0; iField < NFIELDS; iField++)
    values.options[iField] = this->optFields[iField]->getValue();

  values.useEqSolverBuffer = this->eqSolverToggle->getValue();
  values.autoEqSolverBuffer = this->eqSolverAutomatic->getValue();

  values.useRecMatrixBuffer = this->recMatrixToggle->getValue();
  values.autoRecMatrixBuffer = this->recMatrixAutomatic->getValue();

  values.useSolverPrefix = this->solverPrefixToggle->getValue();
  values.useSolverPath = this->solverPathToggle->getValue();

  // Lambda function checking that an integer field has a valid value
  auto&& isValid = [this,&values](int iField, int minVal = 0)
  {
    bool& status = values.optionStatus[iField];
    if (!this->optFields[iField]->isInt())
      status = false;
    else if (this->optFields[iField]->getInt() < minVal)
      status = false;
    else
      status = true;

    return status;
  };

  if (isValid(MAX_CONC_PROC,1) & isValid(EQ_SOLVER_SWAP) & isValid(REC_MATRIX_SWAP)) {
    //all values are ok for the ui
    this->invokeSetAndGetDBValuesCB(&values);
    //Reset values to ensure value correspondance between ui and db
    for (int iField = 0; iField < NFIELDS; iField++)
      if (values.optionStatus.find(iField) == values.optionStatus.end())
        this->optFields[iField]->setValue(values.options[iField]);
    for (const std::pair<const int,bool>& status : values.optionStatus)
      if (status.second)
        this->optFields[status.first]->setValue(values.options[status.first]);
  }

  //check the values
  for (const std::pair<const int,bool>& status : values.optionStatus)
    if (!status.second) {
      this->optFields[status.first]->highlight();
      std::cout << '\a' << std::flush; //bleep
      return false;
    }

  return true;
}
//----------------------------------------------------------------------------

void FuiPreferences::setUIValues(const FFuaUIValues* values)
{
  FuaPreferencesValues* prefValues = (FuaPreferencesValues*) values;

  for (int iField = 0; iField < NFIELDS; iField++)
    this->optFields[iField]->setValue(prefValues->options[iField]);

  this->eqSolverToggle->setValue(prefValues->useEqSolverBuffer);
  this->eqSolverAutomatic->setValue(prefValues->autoEqSolverBuffer);
  this->eqSolverManual->setValue(!prefValues->autoEqSolverBuffer);

  this->recMatrixToggle->setValue(prefValues->useRecMatrixBuffer);
  this->recMatrixAutomatic->setValue(prefValues->autoRecMatrixBuffer);
  this->recMatrixManual->setValue(!prefValues->autoRecMatrixBuffer);

  this->solverPrefixToggle->setValue(prefValues->useSolverPrefix);
  this->solverPathToggle->setValue(prefValues->useSolverPath);
  this->onSolvePrefixToggeled(prefValues->useSolverPrefix);

  this->isRestartActive = prefValues->isRestarted;
  this->setSensitivity(prefValues->isSensitive);

  this->dialogButtons->setButtonSensitivity(FFuDialogButtons::LEFTBUTTON,prefValues->isTouchable);

  if (FapLicenseManager::isProEdition()) {
    // Show fields that are available in the pro-edition only
    this->optFields[FPP]->setSensitivity(true);
    for (int iField = EQ_SOLVER_SWAP; iField < SOLVER_PATH; iField++) {
      this->labels[iField]->popUp();
      this->optFields[iField]->popUp();
    }
    this->optFields[SOLVER_PATH]->popUp();
    this->reducerFrame->popUp();
    this->remoteSolveFrame->popUp();
  }
  else {
    // Hide fields that are available in the pro-edition only
    this->optFields[FPP]->setSensitivity(false);
    for (int iField = EQ_SOLVER_SWAP; iField < SOLVER_PATH; iField++) {
      this->labels[iField]->popDown();
      this->optFields[iField]->popDown();
    }
    this->optFields[SOLVER_PATH]->popDown();
    this->reducerFrame->popDown();
    this->remoteSolveFrame->popDown();
  }
}
//----------------------------------------------------------------------------

void FuiPreferences::setSensitivity(bool sens)
{
  // The reducer and solver additional options should be locked on results
  this->optFields[REDUCER]->setSensitivity(sens);
  this->optFields[SOLVER]->setSensitivity(sens || this->isRestartActive);
  this->eqSolverToggle->setSensitivity(sens);
  this->onEqSolverToggeled(sens && this->eqSolverToggle->getValue());
  this->recMatrixToggle->setSensitivity(sens);
  this->onRecMatrixToggeled(sens && this->recMatrixToggle->getValue());
  this->isSensitive = sens;
}
//----------------------------------------------------------------------------

void FuiPreferences::onEqSolverToggeled(bool isOn)
{
  this->eqSolverAutomatic->setSensitivity(isOn);
  this->eqSolverManual->setSensitivity(isOn);
  this->optFields[EQ_SOLVER_SWAP]->setSensitivity(isOn && this->eqSolverManual->getValue());
}
//----------------------------------------------------------------------------

void FuiPreferences::onEqSolverGroupToggeled(int button, bool isOn)
{
  if (button == 1)
    this->optFields[EQ_SOLVER_SWAP]->setSensitivity(isOn);
  else if (isOn)
    this->optFields[EQ_SOLVER_SWAP]->setSensitivity(false);
}
//----------------------------------------------------------------------------

void FuiPreferences::onRecMatrixToggeled(bool isOn)
{
  this->recMatrixAutomatic->setSensitivity(isOn);
  this->recMatrixManual->setSensitivity(isOn);
  this->optFields[REC_MATRIX_SWAP]->setSensitivity(isOn && this->recMatrixManual->getValue());
}
//----------------------------------------------------------------------------

void FuiPreferences::onRecMatrixGroupToggeled(int button, bool isOn)
{
  if (button == 1)
    this->optFields[REC_MATRIX_SWAP]->setSensitivity(isOn);
  else if (isOn)
    this->optFields[REC_MATRIX_SWAP]->setSensitivity(false);
}
//----------------------------------------------------------------------------

void FuiPreferences::onSolvePrefixToggeled(bool isOn)
{
  this->optFields[SOLVER_PREFIX]->setSensitivity(isOn);
  this->solverPathToggle->setSensitivity(isOn);
  this->optFields[SOLVER_PATH]->setSensitivity(isOn && this->solverPathToggle->getValue());
}
//----------------------------------------------------------------------------

void FuiPreferences::onSolvePathToggeled(bool isOn)
{
  this->optFields[SOLVER_PATH]->setSensitivity(isOn);
}
//----------------------------------------------------------------------------

void FuiPreferences::onRestartToggeled(bool isOn)
{
  this->isRestartActive = isOn;
  this->optFields[SOLVER]->setSensitivity(isSensitive || isRestartActive);
}
//----------------------------------------------------------------------------
