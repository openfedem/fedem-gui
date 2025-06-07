// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiAppearance.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuFrame.H"
#include "FFuLib/FFuScale.H"
#include "FFuLib/FFuDialogButtons.H"
#include "FFuLib/FFuColorSelector.H"
#include "FFuLib/FFuOptionMenu.H"


Fmd_SOURCE_INIT(FUI_APPEARANCE, FuiAppearance, FFuTopLevelShell);


FuiAppearance::FuiAppearance()
{
  Fmd_CONSTRUCTOR_INIT(FuiAppearance);

  isShininessChanged    = false;
  isTransparencyChanged = false;
  isColorChanged        = false;
  isPolygonsChanged     = false;
  isLinesChanged        = false;
}


void FuiAppearance::setUIValues(const FFuaUIValues* values)
{
  if (!values) return;
  
  FuaAppearanceValues* gsValues = (FuaAppearanceValues*) values;
    
  myShininessScale->setValue(gsValues->myShininess);
  myTransparencyScale->setValue(gsValues->myTransparency);
  myColorSelector->setColor(gsValues->myColor, false);
  myPolygonOptions->selectOption(gsValues->myPolygonsValue);
  myLinesOptions->selectOption(gsValues->myLinesValue);
}


bool FuiAppearance::getValues(FuaAppearanceValues* values) const
{
  if (!values) return false;
  
  values->isShininessChanged    = this->isShininessChanged;
  values->isTransparencyChanged = this->isTransparencyChanged;
  values->isColorChanged        = this->isColorChanged;
  values->isPolygonsChanged     = this->isPolygonsChanged;
  values->isLinesChanged        = this->isLinesChanged;
  values->myShininess = myShininessScale->getValue();
  values->myTransparency = myTransparencyScale->getValue();
  values->myColor = myColorSelector->getColor();
  values->myPolygonsValue = myPolygonOptions->getSelectedOption();
  values->myLinesValue = myLinesOptions->getSelectedOption();
  return true;
}


void FuiAppearance::setPolygonLODText(const std::vector<std::string>& options)
{
  myPolygonOptions->setOptions(options);
}

void FuiAppearance::setLinesLODText(const std::vector<std::string>& options)
{
  myLinesOptions->setOptions(options);
}


void FuiAppearance::initWidgets(void)
{
  myDetailsFrame->setLabel("Level Of Detail");
  myColorFrame->setLabel("Color");
  myMaterialFrame->setLabel("Material");

  myDialogButtons->setButtonClickedCB(FFaDynCB1M(FuiAppearance,this,
						 onDialogButtonClicked,int));
  myDialogButtons->setButtonLabel(FFuDialogButtons::LEFTBUTTON,"Close");

  myTransparencyScale->setMinMax(0,100);
  myShininessScale->setMinMax(0,100);

  myTransparencyScale->setDragCB(FFaDynCB1M(FuiAppearance,this,
					    onTranspSliderChanged,int));
  myShininessScale->setDragCB(FFaDynCB1M(FuiAppearance,this,
					 onShininessSliderChanged,int));
  myColorSelector->setColorChangedCB(FFaDynCB1M(FuiAppearance,this,
                                                colorValueChanged,const FFuColor&));

  myPolygonOptions->setOptionSelectedCB(FFaDynCB1M(FuiAppearance,this, 
						   onPolygonOptionChanged,int));
  myLinesOptions->setOptionSelectedCB(FFaDynCB1M(FuiAppearance,this,
						 onLinesOptionChanged,int));
  
  FFuUAExistenceHandler::invokeCreateUACB(this);
}


void FuiAppearance::disableAdvancedOptions(bool disable)
{
  if (disable)
    {
      myShininessScale->setSensitivity(false);
      myPolygonOptions->setSensitivity(false);
      myLinesOptions->setSensitivity(false);
    }
  else
   {
      myShininessScale->setSensitivity(true);
      myPolygonOptions->setSensitivity(true);
      myLinesOptions->setSensitivity(true);
    }    
}


bool FuiAppearance::onClose()
{
  this->invokeFinishedCB();
  return true;
}


void FuiAppearance::onDialogButtonClicked(int button)
{
  if (button==FFuDialogButtons::LEFTBUTTON) 
    this->invokeFinishedCB();
}


bool FuiAppearance::updateDBValues(bool)
{
  // optain values from UI
  FuaAppearanceValues values; 
  this->getValues(&values);
  
  // do something elsewhere...
  this->invokeSetAndGetDBValuesCB(&values);
  
  //Re-set values to ensure value correspondance between ui and db
  this->setUIValues(&values);
  return true;
} 
