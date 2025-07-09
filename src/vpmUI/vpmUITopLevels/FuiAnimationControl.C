// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiAnimationControl.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuRadioButton.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuDialogButtons.H"


Fmd_SOURCE_INIT(FUI_ANIMATIONCONTROL,FuiAnimationControl,FFuTopLevelShell);


//----------------------------------------------------------------------------

FuiAnimationControl::FuiAnimationControl()
{
  Fmd_CONSTRUCTOR_INIT(FuiAnimationControl);
}
//----------------------------------------------------------------------------

void FuiAnimationControl::setSensitivity(bool isSensitive)
{
  this->cameraMenu->setSensitivity(isSensitive);
  this->linkMotionToggle->setSensitivity(isSensitive);
  this->triadMotionToggle->setSensitivity(isSensitive);
  this->defToggle->setSensitivity(isSensitive);
  this->defScaleField->setSensitivity(isSensitive);
  this->fringeToggle->setSensitivity(isSensitive);
  this->legendToggle->setSensitivity(isSensitive);
  this->legendColorsMenu->setSensitivity(isSensitive);
  this->legendMappingMenu->setSensitivity(isSensitive);
  this->legendLookMenu->setSensitivity(isSensitive);
  this->legendMaxField->setSensitivity(isSensitive);
  this->legendMinField->setSensitivity(isSensitive);
  this->tickMarkCountRadio->setSensitivity(isSensitive);
  this->tickMarkCountField->setSensitivity(isSensitive);
  this->tickMarkSpaceRadio->setSensitivity(isSensitive);
  this->tickMarkSpaceField->setSensitivity(isSensitive);
}
//----------------------------------------------------------------------------

void FuiAnimationControl::initWidgets()
{
  this->cameraMenu->setRefSelectedCB(FFaDynCB1M(FuiAnimationControl,this,onMenuSelected,int));
  this->cameraMenu->setBehaviour(FuiQueryInputField::REF_NONE,true);

  this->linkMotionToggle->setToggleCB(FFaDynCB1M(FuiAnimationControl,this,onButtonToggled,bool));
  this->triadMotionToggle->setToggleCB(FFaDynCB1M(FuiAnimationControl,this,onButtonToggled,bool));
  this->defToggle->setToggleCB(FFaDynCB1M(FuiAnimationControl,this,onButtonToggled,bool));
  this->fringeToggle->setToggleCB(FFaDynCB1M(FuiAnimationControl,this,onButtonToggled,bool));
  this->legendToggle->setToggleCB(FFaDynCB1M(FuiAnimationControl,this,onButtonToggled,bool));

  this->legendColorsMenu->setOptionSelectedCB(FFaDynCB1M(FuiAnimationControl,this,onMenuSelected,int));
  this->legendMappingMenu->setOptionSelectedCB(FFaDynCB1M(FuiAnimationControl,this,onMenuSelected,int));
  this->legendLookMenu->setOptionSelectedCB(FFaDynCB1M(FuiAnimationControl,this,onMenuSelected,int));

  this->defScaleField->setAcceptedCB(FFaDynCB1M(FuiAnimationControl,this,onFieldValueChanged,char*));
  this->legendMaxField->setAcceptedCB(FFaDynCB1M(FuiAnimationControl,this,onFieldValueChanged,char*));
  this->legendMinField->setAcceptedCB(FFaDynCB1M(FuiAnimationControl,this,onFieldValueChanged,char*));
  this->tickMarkCountField->setAcceptedCB(FFaDynCB1M(FuiAnimationControl,this,onFieldValueChanged,char*));
  this->tickMarkSpaceField->setAcceptedCB(FFaDynCB1M(FuiAnimationControl,this,onFieldValueChanged,char*));

  this->tickMarkCountRadio->setToggleCB(FFaDynCB1M(FuiAnimationControl,this,onTickMarkCountToggled,bool));
  this->tickMarkSpaceRadio->setToggleCB(FFaDynCB1M(FuiAnimationControl,this,onTickMarkSpaceToggled,bool));

  this->dialogButtons->setButtonClickedCB(FFaDynCB1M(FuiAnimationControl,this,onDialogButtonClicked,int));

  this->legendMaxField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->legendMaxField->setDoubleDisplayMode(FFuIOField::AUTO);
  this->legendMaxField->setDoubleDisplayPrecision(3);
  this->legendMaxField->setZeroDisplayPrecision(1);

  this->legendMinField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->legendMinField->setDoubleDisplayMode(FFuIOField::AUTO);
  this->legendMinField->setDoubleDisplayPrecision(3);
  this->legendMinField->setZeroDisplayPrecision(1);

  this->defScaleField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->defScaleField->setDoubleDisplayMode(FFuIOField::DECIMAL);
  this->defScaleField->setDoubleDisplayPrecision(2);
  this->defScaleField->setZeroDisplayPrecision(1);

  this->tickMarkCountField->setInputCheckMode(FFuIOField::INTEGERCHECK);

  this->tickMarkSpaceField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->tickMarkSpaceField->setDoubleDisplayMode(FFuIOField::DECIMAL);
  this->tickMarkSpaceField->setDoubleDisplayPrecision(3);
  this->tickMarkSpaceField->setZeroDisplayPrecision(1);

  this->dialogButtons->setButtonLabel(FFuDialogButtons::LEFTBUTTON, "Close");

  this->cameraFrame->setLabel("Camera");
  this->motionFrame->setLabel("Motion");
  this->linkMotionToggle->setLabel("Show Part Motion");
  this->triadMotionToggle->setLabel("Show Triad Motion");
  this->triadMotionToggle->popDown();
  this->defFrame->setLabel("Deformations");
  this->defToggle->setLabel("Show Part Deformation");
  this->legendFrame->setLabel("Contour Legend");
  this->fringeToggle->setLabel("Show Contour");
  this->legendToggle->setLabel("Show Legend");
  this->tickMarkFrame->setLabel("Tick Marks");
  this->tickMarkCountRadio->setLabel("Count");
  this->tickMarkSpaceRadio->setLabel("Spacing");

  // create ui's UA object
  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//----------------------------------------------------------------------------

bool FuiAnimationControl::onClose()
{
  this->onDialogButtonClicked(0);
  return false;
}
//-----------------------------------------------------------------------------

void FuiAnimationControl::onPoppedUp()
{
  this->updateUIValues();
}
//----------------------------------------------------------------------------

void FuiAnimationControl::onTickMarkCountToggled(bool toggle)
{
  tickMarkSpaceRadio->setValue(!toggle);
  this->updateDBValues();
}
//----------------------------------------------------------------------------

void FuiAnimationControl::onTickMarkSpaceToggled(bool toggle)
{
  tickMarkCountRadio->setValue(!toggle);
  this->updateDBValues();
}
//----------------------------------------------------------------------------

void FuiAnimationControl::onDialogButtonClicked(int)
{
  this->invokeFinishedCB();
}
//----------------------------------------------------------------------------

FFuaUIValues* FuiAnimationControl::createValuesObject()
{
  return new FuaAnimationControlValues();
}
//----------------------------------------------------------------------------

void FuiAnimationControl::setUIValues(const FFuaUIValues* values)
{
  FuaAnimationControlValues* animValues = (FuaAnimationControlValues*) values;

  this->cameraMenu->setQuery(animValues->myLinkQuery);
  this->cameraMenu->setSelectedRef(animValues->myLinkToFollow);

  this->linkMotionToggle->setValue(animValues->linkMotion);
  this->triadMotionToggle->setValue(animValues->triadMotion);

  this->defToggle->setValue(animValues->def);
  this->defScaleField->setValue(animValues->defScale);
  this->defScaleField->setSensitivity(animValues->defScalePossible && animValues->def);

  this->fringeToggle->setValue(animValues->fringe);
  this->legendToggle->setValue(animValues->legend);

  this->legendColorsMenu->setOptions(animValues->legendColors);
  this->legendColorsMenu->selectOption(animValues->selLegendColor,false);
  this->legendMappingMenu->setOptions(animValues->legendMappings);
  this->legendMappingMenu->selectOption(animValues->selLegendMapping,false);
  this->legendLookMenu->setOptions(animValues->legendLooks);
  this->legendLookMenu->selectOption(animValues->selLegendLook,false);

  this->legendMinField->setValue(animValues->legendRange.first);
  this->legendMaxField->setValue(animValues->legendRange.second);

  this->tickMarkCountRadio->setValue(animValues->tickMarkCount);
  this->tickMarkSpaceRadio->setValue(!animValues->tickMarkCount);

  this->tickMarkCountField->setSensitivity(animValues->tickMarkCount);
  this->tickMarkSpaceField->setSensitivity(!animValues->tickMarkCount);

  this->tickMarkCountField->setValue(animValues->tickMarkCountVal);
  this->tickMarkSpaceField->setValue(animValues->tickMarkSpacingVal);
}
//-----------------------------------------------------------------------------

void FuiAnimationControl::getUIValues(FFuaUIValues* values)
{
  FuaAnimationControlValues* animValues = (FuaAnimationControlValues*) values;

  animValues->myLinkToFollow = this->cameraMenu->getSelectedRef();

  animValues->linkMotion = this->linkMotionToggle->getValue();
  animValues->triadMotion = this->triadMotionToggle->getValue();

  animValues->def = this->defToggle->getValue();
  animValues->defScale = this->defScaleField->getDouble();

  animValues->fringe = this->fringeToggle->getValue();
  animValues->legend = this->legendToggle->getValue();

  animValues->selLegendColor = this->legendColorsMenu->getSelectedOptionStr();
  animValues->selLegendMapping = this->legendMappingMenu->getSelectedOptionStr();
  animValues->selLegendLook = this->legendLookMenu->getSelectedOptionStr();

  animValues->legendRange.first  = this->legendMinField->getDouble();
  animValues->legendRange.second = this->legendMaxField->getDouble();

  animValues->tickMarkCount = this->tickMarkCountRadio->getValue();
  animValues->tickMarkCountVal = this->tickMarkCountField->getInt();
  animValues->tickMarkSpacingVal = this->tickMarkSpaceField->getDouble();
}
