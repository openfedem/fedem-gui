// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiFppOptions.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"


enum {
  STRESS,
  STRAIN
};

Fmd_SOURCE_INIT(FUI_FPPOPTIONS,FuiFppOptions,FFuTopLevelShell);

//----------------------------------------------------------------------------

FuiFppOptions::FuiFppOptions()
{
  Fmd_CONSTRUCTOR_INIT(FuiFppOptions);

  this->showNCodeFields = false;
}
//----------------------------------------------------------------------------

void FuiFppOptions::initWidgets()
{
  time->setResetCB(FFaDynCB0M(FFaDynCB0,&resetTimeCB,invoke));

  this->pvxGateField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->pvxGateField->setDoubleDisplayMode(FFuIOField::AUTO,6,1);
  this->biaxGateField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->biaxGateField->setDoubleDisplayMode(FFuIOField::AUTO,6,1);

  this->maxField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->maxField->setDoubleDisplayMode(FFuIOField::AUTO,6,1);
  this->minField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->minField->setDoubleDisplayMode(FFuIOField::AUTO,6,1);

  this->nBinsField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  this->maxElemsField->setInputCheckMode(FFuIOField::INTEGERCHECK);

  this->typeLabel->setLabel("Analysis type");
  this->typeMenu->addOption("Signed abs max stress (S-N)", STRESS);
  this->typeMenu->addOption("Signed abs max strain (E-N)", STRAIN);
  this->typeMenu->setOptionSelectedCB(FFaDynCB1M(FuiFppOptions,this,
						 onAnalysisTypeChanged,int));

  this->rainflowFrame->setLabel("Rainflow/Fatigue Analysis");
  this->rainflowButton->setLabel("Perform rainflow and fatigue analysis");
  this->rainflowButton->setToggleCB(FFaDynCB1M(FuiFppOptions,this,
					       onRainflowButtonToggled,bool));

  this->biaxGateLabel->setLabel("Biaxiality stress threshold");
  this->biaxGateLabel->setToolTip("Principal stresses below this value are ignored"
				  "\nin the biaxiality calculations");

  this->onAnalysisTypeChanged(STRESS);

  this->histFrame->setLabel("Histogram");
  this->maxElemsLabel->setLabel("Max number of elements processed together");

  this->FuiTopLevelDialog::initWidgets();
}
//----------------------------------------------------------------------------

void FuiFppOptions::setTimeUIValues(const FuaTimeIntervalValues* timeValues)
{
  this->time->setUIValues(timeValues);
}
//-----------------------------------------------------------------------------

void FuiFppOptions::onRainflowButtonToggled(bool toggle)
{
  this->typeMenu->setSensitivity(showNCodeFields && toggle);
  this->pvxGateField->setSensitivity(toggle);
  this->maxField->setSensitivity(toggle);
  this->minField->setSensitivity(toggle);
  this->nBinsField->setSensitivity(toggle);
}
//----------------------------------------------------------------------------

void FuiFppOptions::onAnalysisTypeChanged(int type)
{
  std::string atype(type == STRESS ? "Stress" : "Strain");
  this->pvxGateLabel->setLabel(atype + " range threshold");
  this->pvxGateLabel->setToolTip((atype + " ranges below this value are ignored"
				  "\nin the rainflow analysis (peak valley extraction)").c_str());
}
//----------------------------------------------------------------------------

FFuaUIValues* FuiFppOptions::createValuesObject()
{
  return new FuaFppOptionsValues();
}
//----------------------------------------------------------------------------

void FuiFppOptions::setUIValues(const FFuaUIValues* values)
{
  FuaFppOptionsValues* fppValues = (FuaFppOptionsValues*) values;

  this->time->setUIValues(&fppValues->timeValues);
  this->maxElemsField->setValue(fppValues->nElemsTogether);
  this->rainflowButton->setValue(fppValues->performRainflow);
  this->pvxGateField->setValue(fppValues->pvxGate);
  this->biaxGateField->setValue(fppValues->biaxGate);

  if ((showNCodeFields = fppValues->histType >= 0)) {
    // The histogram fields are relevant for nCode only
    typeMenu->selectOption(fppValues->histType);
    typeMenu->setSensitivity(true);
    this->onAnalysisTypeChanged(fppValues->histType);
    maxField->setValue(fppValues->max);
    minField->setValue(fppValues->min);
    nBinsField->setValue(fppValues->nBins);
    histFrame->popUp();
  }
  else {
    typeMenu->selectOption(STRESS);
    typeMenu->setSensitivity(false);
    histFrame->popDown();
  }

  this->onRainflowButtonToggled(fppValues->performRainflow);
}
//-----------------------------------------------------------------------------

void FuiFppOptions::getUIValues(FFuaUIValues* values)
{
  FuaFppOptionsValues* fppValues = (FuaFppOptionsValues*) values;

  this->time->getUIValues(&fppValues->timeValues);
  fppValues->nElemsTogether = this->maxElemsField->getInt();
  fppValues->performRainflow = this->rainflowButton->getValue();
  fppValues->pvxGate = this->pvxGateField->getDouble();
  fppValues->biaxGate = this->biaxGateField->getDouble();
  if (this->showNCodeFields) {
    fppValues->histType = this->typeMenu->getSelectedOption();
    fppValues->max = this->maxField->getDouble();
    fppValues->min = this->minField->getDouble();
    fppValues->nBins = this->nBinsField->getInt();
  }
  else
    fppValues->histType = -1;
}
//-----------------------------------------------------------------------------
