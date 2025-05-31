// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiStressOptions.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuFileBrowseField.H"
#include "FFuLib/FFuFileDialog.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"


Fmd_SOURCE_INIT(FUI_STRESSOPTIONS, FuiStressOptions, FFuTopLevelShell);

//----------------------------------------------------------------------------

FuiStressOptions::FuiStressOptions()
{
  Fmd_CONSTRUCTOR_INIT(FuiStressOptions);

  showVTFfield = false;
}
//----------------------------------------------------------------------------

void FuiStressOptions::initWidgets()
{
  this->time->setResetCB(FFaDynCB0M(FFaDynCB0,&resetTimeCB,invoke));

  this->outputFrame->setLabel("Output Options");
  this->stressToggle[0]->setLabel("stress resultant tensor");
  this->stressToggle[1]->setLabel("stress tensor");
  this->stressToggle[2]->setLabel("von Mises stress");
  this->stressToggle[3]->setLabel("max principal stress");
  this->stressToggle[4]->setLabel("min principal stress");
  this->stressToggle[5]->setLabel("max shear stress");
  this->strainToggle[0]->setLabel("deformation");
  this->strainToggle[1]->setLabel("strain tensor");
  this->strainToggle[2]->setLabel("von Mises strain");
  this->strainToggle[3]->setLabel("max principal strain");
  this->strainToggle[4]->setLabel("min principal strain");
  this->strainToggle[5]->setLabel("max shear strain");

  this->vtfFrame->setLabel("VTF Export");
  this->autoVTFToggle->setLabel("Automatic export to GLview VTF file");
  this->autoVTFToggle->setToggleCB(FFaDynCB1M(FuiStressOptions,this,
					      onAutoVTFToggled,bool));
  this->autoVTFField->setLabel("Express File");
  this->autoVTFField->setAbsToRelPath("yes");
  this->autoVTFField->setDialogType(FFuFileDialog::FFU_SAVE_FILE);
  this->autoVTFField->setDialogRememberKeyword("AutoVTFField");

  this->autoVTFField->addDialogFilter("Express VTF file","vtf",true,0);
  this->autoVTFField->addDialogFilter("Binary VTF file","vtf",false,1);
  this->autoVTFField->addDialogFilter("ASCII VTF file","vtf",false,2);
  this->autoVTFField->setFileOpenedCB(FFaDynCB2M(FuiStressOptions,this,
						 onAutoVTFFileChanged,
						 const std::string&,int));

  this->FuiTopLevelDialog::initWidgets();
}
//-----------------------------------------------------------------------------

void FuiStressOptions::setTimeUIValues(const FuaTimeIntervalValues* timeValues)
{
  this->time->setUIValues(timeValues);
}
//-----------------------------------------------------------------------------

void FuiStressOptions::setVTFLabel(int id)
{
  const char* vtf_label[3] = { "Express File",
			       " Binary File",
			       "  ASCII File" };

  if (id >= 0 && id < 3)
    this->autoVTFField->setLabel(vtf_label[id]);
  else
    this->autoVTFField->setLabel("File");

  bool fringeSens = id == 0 && this->autoVTFToggle->getValue();
  this->minFringeField->setSensitivity(fringeSens);
  this->maxFringeField->setSensitivity(fringeSens);
}
//-----------------------------------------------------------------------------

void FuiStressOptions::onAutoVTFFileChanged(const std::string&, int id)
{
  this->setVTFLabel(id);
}
//----------------------------------------------------------------------------

void FuiStressOptions::onAutoVTFToggled(bool value)
{
  this->autoVTFField->setSensitivity(value);
  bool isExpressFile = this->autoVTFField->getFilterID() == 0;
  this->minFringeField->setSensitivity(value && isExpressFile);
  this->maxFringeField->setSensitivity(value && isExpressFile);
}
//----------------------------------------------------------------------------

FFuaUIValues* FuiStressOptions::createValuesObject()
{
  return new FuaStressOptionsValues();
}
//----------------------------------------------------------------------------

void FuiStressOptions::setUIValues(const FFuaUIValues* values)
{
  FuaStressOptionsValues* stressValues = (FuaStressOptionsValues*) values;

  this->time->setUIValues(&stressValues->timeValues);
  this->strainToggle.front()->setValue(stressValues->deformationOutput);
  this->stressToggle.front()->setValue(stressValues->stressResOutput);
  for (int i = 0; i < 5; i++) {
    this->strainToggle[1+i]->setValue(stressValues->strainOutput[i]);
    this->stressToggle[1+i]->setValue(stressValues->stressOutput[i]);
  }
  if ((showVTFfield = stressValues->autoVTFSwitch >= 0)) {
    this->autoVTFToggle->setValue(stressValues->autoVTFSwitch);
    this->autoVTFField->setAbsToRelPath(stressValues->modelFilePath);
    this->autoVTFField->setFileName(stressValues->autoVTFFileName);
    this->autoVTFField->setFilterID(stressValues->autoVTFFileType);
    this->autoVTFField->setSensitivity(stressValues->autoVTFSwitch);
    this->setVTFLabel(stressValues->autoVTFFileType);
    this->minFringeField->setValue(stressValues->vtfFringeMin);
    this->maxFringeField->setValue(stressValues->vtfFringeMax);
    this->vtfFrame->popUp();
  }
  else
    this->vtfFrame->popDown();

  this->setSensitivity(stressValues->isSensitive);
}
//-----------------------------------------------------------------------------

void FuiStressOptions::getUIValues(FFuaUIValues* values)
{
  FuaStressOptionsValues* stressValues = (FuaStressOptionsValues*) values;

  this->time->getUIValues(&stressValues->timeValues);
  stressValues->deformationOutput = this->strainToggle.front()->getValue();
  stressValues->stressResOutput   = this->stressToggle.front()->getValue();
  for (int i = 0; i < 5; i++) {
    stressValues->stressOutput[i] = this->stressToggle[1+i]->getValue();
    stressValues->strainOutput[i] = this->strainToggle[1+i]->getValue();
  }
  if (showVTFfield) {
    stressValues->autoVTFSwitch   = this->autoVTFToggle->getValue();
    stressValues->autoVTFFileName = this->autoVTFField->getFileName();
    stressValues->autoVTFFileType = this->autoVTFField->getFilterID();
    stressValues->vtfFringeMin    = this->minFringeField->getDouble();
    stressValues->vtfFringeMax    = this->maxFringeField->getDouble();
  }
  else
    stressValues->autoVTFSwitch = -1;
}
