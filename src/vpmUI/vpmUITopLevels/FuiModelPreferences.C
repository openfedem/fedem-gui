// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiModelPreferences.H"
#include "vpmUI/vpmUITopLevels/FuiSeaEnvironment.H"
#include "vpmUI/vpmUIComponents/Fui3DPoint.H"
#include "vpmUI/Fui.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuMemo.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuFileBrowseField.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuDialogButtons.H"
#include "vpmApp/FapLicenseManager.H"


enum {
  SAVE  = FFuDialogButtons::LEFTBUTTON,
  CLOSE = FFuDialogButtons::MIDBUTTON,
  HELP  = FFuDialogButtons::RIGHTBUTTON
};

Fmd_SOURCE_INIT(FUI_MODELPREFERENCES,FuiModelPreferences,FFuTopLevelShell);

//----------------------------------------------------------------------------

FuiModelPreferences::FuiModelPreferences()
{
  Fmd_CONSTRUCTOR_INIT(FuiModelPreferences);

  this->IAmInternalRepository = false;;
}
//----------------------------------------------------------------------------

void FuiModelPreferences::setSensitivity(bool isSensitive)
{
  this->changeButton->setSensitivity(isSensitive);
  this->switchButton->setSensitivity(isSensitive);
  this->extFuncFileButton->setSensitivity(isSensitive);
  this->extFuncFileField->setSensitivity(isSensitive && this->extFuncFileButton->getValue());

  this->dialogButtons->setButtonSensitivity(SAVE,isSensitive);
}
//----------------------------------------------------------------------------

void FuiModelPreferences::setUpdateDescriptionCB(const FFaDynCB1<const std::string&>& dynCB)
{
  this->updateDescriptionCB = dynCB;
}
//----------------------------------------------------------------------------

void FuiModelPreferences::setChangeRepositoryCB(const FFaDynCB1<bool>& dynCB)
{
  this->changeRepositoryCB = dynCB;
}
//----------------------------------------------------------------------------

void FuiModelPreferences::initWidgets()
{
  this->dialogButtons->setButtonClickedCB(FFaDynCB1M(FuiModelPreferences,this,
						     onDialogButtonClicked,int));
  this->changeButton->setActivateCB(FFaDynCB0M(FuiModelPreferences,this,
					       onChangeButtonPushed));
  this->switchButton->setActivateCB(FFaDynCB0M(FuiModelPreferences,this,
					       onSwitchButtonPushed));
  this->extFuncFileButton->setToggleCB(FFaDynCB1M(FFuComponentBase,extFuncFileField,
						  setSensitivity,bool));

  this->dialogButtons->setButtonLabel(SAVE,"Save");
  this->dialogButtons->setButtonLabel(CLOSE,"Close");
  this->dialogButtons->setButtonLabel(HELP,"Help");

  this->descriptionFrame->setLabel("Model description");
  this->descriptionMemo->setEditable(true);
  this->descriptionMemo->setToolTip("Enter any text describing the model in this field.\n"
				    "Note: Changes in this field are saved also when the dialog is Canceled or Closed");

  this->repositoryFrame->setLabel("FE model repository");
  this->repositoryField->setSensitivity(false);
  this->changeButton->setLabel("Change...");
  this->changeButton->setToolTip("Select or create a new external FE model repository");
  this->overwriteButton->setLabel("Overwrite existing reduced FE data");
  this->overwriteButton->setToolTip("If enabled, a new reduction of an FE part will overwrite\n"
                                    "the current existing and invalid data, instead of creating a\n"
                                    "new directory. Use this to save disk space for larger FE parts.");

  this->unitsFrame->setLabel("Units");

  this->modelingTolFrame->setLabel("Modeling tolerance");

  this->modelingTolField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->modelingTolField->setDoubleDisplayMode(FFuIOField::AUTO);
  this->modelingTolField->setDoubleDisplayPrecision(3);
  this->modelingTolField->setZeroDisplayPrecision(1);

  this->gravitationFrame->setLabel("Gravitation");
  this->initialVelFrame->setLabel("Initial translational velocity");

  this->extFuncFileFrame->setLabel("File for external function evaluation");
  this->extFuncFileButton->setLabel("Use external function file");
  this->extFuncFileField->setAbsToRelPath("yes");
  this->extFuncFileField->setDialogType(FFuFileDialog::FFU_OPEN_FILE);
  std::vector<std::string> ascii({"asc","txt"});
  this->extFuncFileField->addDialogFilter("ASCII curve data file",ascii,true);
  this->extFuncFileField->addDialogFilter("Comma-separated values file","csv");
  this->extFuncFileField->addAllFilesFilter(true);
  this->extFuncFileField->setDialogRememberKeyword("ExternalFuncFileField");

  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//----------------------------------------------------------------------------

void FuiModelPreferences::onPoppedUp()
{
  this->updateUIValues();
}
//----------------------------------------------------------------------------

bool FuiModelPreferences::onClose()
{
  this->invokeFinishedCB();
  this->updateDescriptionOnly();
  return false;
}
//----------------------------------------------------------------------------

void FuiModelPreferences::onDialogButtonClicked(int button)
{
  switch (button)
  {
    case SAVE:
      this->updateDBValues();
      break;

    case CLOSE:
      this->updateDescriptionOnly();
      this->invokeFinishedCB();
      return;

    case HELP:
      // Display the topic in the help file
      Fui::showCHM("dialogbox/model-preferences.htm");
      return;
  }

  // Also update the sea environment UI,
  // since the Gravitation vector are in both UI's
  FFuTopLevelShell* env = FFuTopLevelShell::getInstanceByType(FuiSeaEnvironment::getClassTypeID());
  if (env) dynamic_cast<FuiSeaEnvironment*>(env)->updateUIValues();
}
//----------------------------------------------------------------------------

void FuiModelPreferences::onChangeButtonPushed()
{
  this->changeRepositoryCB.invoke(false);
  this->updateUIValues();
}
//----------------------------------------------------------------------------

void FuiModelPreferences::onSwitchButtonPushed()
{
  this->changeRepositoryCB.invoke(!this->IAmInternalRepository);
  this->updateUIValues();
}
//----------------------------------------------------------------------------

FFuaUIValues* FuiModelPreferences::createValuesObject()
{
  return new FuaModelPreferencesValues();
}
//----------------------------------------------------------------------------

void FuiModelPreferences::setUIValues(const FFuaUIValues* values)
{
  FuaModelPreferencesValues* prefValues = (FuaModelPreferencesValues*) values;

  this->descriptionMemo->setAllText(prefValues->description.c_str());
  this->modelingTolField->setValue(prefValues->modelingTol);
  this->unitsMenu->setOptions(prefValues->units);
  this->unitsMenu->selectOption(prefValues->selectedUnits,false);

  this->gravitationVector->setValue(prefValues->gravitation);
  this->initialVelVector->setValue(prefValues->velocity);
  this->overwriteButton->setValue(prefValues->overwriteFE);

  this->setSensitivity(prefValues->isSensitive);

  if (prefValues->useFuncFile >= 0) {
    this->extFuncFileButton->setValue(prefValues->useFuncFile);
    this->extFuncFileField->setAbsToRelPath(prefValues->modelFilePath);
    this->extFuncFileField->setFileName(prefValues->extFuncFileName);
    this->extFuncFileField->setSensitivity(true);
  }
  else {
    this->extFuncFileButton->setSensitivity(false);
    this->extFuncFileField->setSensitivity(false);
  }

  if (FapLicenseManager::isProEdition())
    this->repositoryFrame->popUp();
  else
    this->repositoryFrame->popDown();

  this->IAmInternalRepository = prefValues->repository.empty();
  if (this->IAmInternalRepository) {
    this->repositoryField->setValue("(internal)");
    this->changeButton->setSensitivity(false);
    this->switchButton->setLabel("Switch to external repository...");
    this->switchButton->setToolTip("Select or create an external FE model repository\n"
				   "that is independent of the current model");
  }
  else {
    this->repositoryField->setValue(prefValues->repository);
    this->switchButton->setLabel("Switch to internal repository...");
    this->switchButton->setToolTip("Use an internal FE model repository\n"
				   "that is specific for current model");
  }
}
//-----------------------------------------------------------------------------

void FuiModelPreferences::getUIValues(FFuaUIValues* values)
{
  FuaModelPreferencesValues* prefValues = (FuaModelPreferencesValues*) values;

  prefValues->description = this->descriptionMemo->getText();
  prefValues->repository = this->repositoryField->getValue();
  if (prefValues->repository == "(internal)") prefValues->repository = "";
  prefValues->modelingTol = this->modelingTolField->getDouble();
  prefValues->selectedUnits = this->unitsMenu->getSelectedOptionStr();
  prefValues->gravitation = this->gravitationVector->getValue();
  prefValues->velocity = this->initialVelVector->getValue();
  prefValues->overwriteFE = this->overwriteButton->getValue();
  if (this->extFuncFileButton->getSensitivity()) {
    prefValues->useFuncFile = this->extFuncFileButton->getValue();
    prefValues->extFuncFileName = this->extFuncFileField->getFileName();
  }
  else
    prefValues->useFuncFile = -1;
}
//-----------------------------------------------------------------------------

void FuiModelPreferences::updateDescriptionOnly()
{
  this->updateDescriptionCB.invoke(this->descriptionMemo->getText());
}
