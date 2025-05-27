// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiAnimationControl.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"
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
  this->cameraMenu->turnButtonOff(true);
  this->cameraMenu->setBehaviour(FuiQueryInputField::REF_NONE);

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
  this->cameraLabel->setLabel("Reference Part");
  this->motionFrame->setLabel("Motion");
  this->linkMotionToggle->setLabel("Show Part Motion");
  this->triadMotionToggle->setLabel("Show Triad Motion");
  this->triadMotionToggle->popDown();
  this->defFrame->setLabel("Deformations");
  this->defToggle->setLabel("Show Part Deformation");
  this->defScaleLabel->setLabel("Scale");
  this->legendFrame->setLabel("Contour Legend");
  this->fringeToggle->setLabel("Show Contour");
  this->legendToggle->setLabel("Show Legend");
  this->legendColorsLabel->setLabel("Colors");
  this->legendMappingLabel->setLabel("Mapping");
  this->legendLookLabel->setLabel("Look");
  this->legendMaxLabel->setLabel("Max");
  this->legendMinLabel->setLabel("Min");
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
//----------------------------------------------------------------------------

void FuiAnimationControl::placeWidgets(int width,int height)
{
  // Some variables
  int border        = this->getBorder();                  // Border around the window  = 10.
  int frameFontH    = this->motionFrame->getFontHeigth(); // Font height
  int spacing       = (2000-8*border)/93;
  int availHeight   = height - this->dialogButtons->getHeightHint() - 2*border; // this is the height that is re-sizeable.
  int fieldl        = 0;
  int fieldw        = 0;
  int menuH         = getGridLinePos(availHeight, 13*frameFontH/4);
  //
  // Horisontal grid line definition
  //
  // Follow-me camera
  int cameraFrameTop     = border;                                     
  int cameraRefCenter    = cameraFrameTop     + getGridLinePos(availHeight, 3*spacing); 
  int cameraFrameBottom  = cameraRefCenter    + getGridLinePos(availHeight, 2*spacing); 

  // Motion
  int motionFrameTop     = cameraFrameBottom  + getGridLinePos(availHeight, border); 
  int linkCenter         = motionFrameTop     + getGridLinePos(availHeight, 3*spacing);
  int triadeCenter       = linkCenter;                                         
  int motionFrameBottom  = triadeCenter       + getGridLinePos(availHeight, 2*spacing); 

  // Deformations
  int defFrameTop        = motionFrameBottom  + getGridLinePos(availHeight, border); 
  int defCenter          = defFrameTop        + getGridLinePos(availHeight, 3*spacing); 
  int scaleCenter        = defCenter          + getGridLinePos(availHeight, 3*spacing); 
  int defFrameBottom     = scaleCenter        + getGridLinePos(availHeight, 2*spacing); 
  
  // Legend
  int legendFrameTop     = defFrameBottom     + getGridLinePos(availHeight, border); 
  int fringeCenter       = legendFrameTop     + getGridLinePos(availHeight, 3*spacing); 
  int legendCenter       = fringeCenter       + getGridLinePos(availHeight, 2*spacing); 
  int colorCenter        = legendCenter       + getGridLinePos(availHeight, 3*spacing); 
  int mappingCenter      = colorCenter        + getGridLinePos(availHeight, 3*spacing);
  int lookCenter         = mappingCenter      + getGridLinePos(availHeight, 3*spacing);
  int maxCenter          = lookCenter         + getGridLinePos(availHeight, 7*spacing/2);
  int minCenter          = maxCenter          + getGridLinePos(availHeight, 3*spacing); 
  
  // Tick marks
  int tickFrameTop       = minCenter          + getGridLinePos(availHeight, 2*spacing + border); 
  int countCenter        = tickFrameTop       + getGridLinePos(availHeight, 3*spacing); 
  int spaceCenter        = countCenter        + getGridLinePos(availHeight, 3*spacing); 
  int tickFrameBottom    = spaceCenter        + getGridLinePos(availHeight, 2*spacing);
  int legendFrameBottom  = tickFrameBottom    + border;                              

  // Close dialog 
  int dialogTop = height - this->dialogButtons->getHeightHint();
  
  //
  // Vertical grid line definition
  //
  int cameraMenuW      = width - 5*border - this->cameraLabel->getWidthHint();//this->cameraMenu->getWidthHint();
  int cameraAvailMenuW = width - 5*border - this->cameraLabel->getWidthHint();
  
  if (cameraMenuW > cameraAvailMenuW) cameraMenuW = cameraAvailMenuW;

  int cameraMenuStart = width - 2*border - cameraMenuW;
  
  // Adjust to common left border
  int menuW = 0;
  if (this->legendColorsMenu->getWidthHint() > menuW)
    menuW = this->legendColorsMenu->getWidthHint();
  if (this->legendMappingMenu->getWidthHint() > menuW)
    menuW = this->legendMappingMenu->getWidthHint();
  if (this->legendLookMenu->getWidthHint() > menuW)
    menuW = this->legendLookMenu->getWidthHint();

  int labelW = 0;
  if (this->legendColorsLabel->getWidthHint() > labelW)
    labelW = this->legendColorsLabel->getWidthHint();
  if (this->legendMappingLabel->getWidthHint() > labelW)
    labelW = this->legendMappingLabel->getWidthHint();
  if (this->legendLookLabel->getWidthHint() > labelW)
    labelW = this->legendLookLabel->getWidthHint();

  int availMenuW = width - 5*border - labelW;
  if (menuW > availMenuW) menuW = availMenuW;

  // Some common variables
  int menuStart  = width - 2*border - menuW;
  int frameStart = border;
  int frameEnd   = width - border;
  int labelStart = 2*border;

  //
  // Create geometry
  //
  this->cameraFrame->setEdgeGeometry(frameStart, frameEnd , cameraFrameTop, cameraFrameBottom);
  this->cameraLabel->setCenterYGeometryWidthHint(labelStart,cameraRefCenter, menuH);
  this->cameraMenu->setCenterYGeometry(cameraMenuStart, cameraRefCenter, cameraMenuW, menuH);

  this->motionFrame->setEdgeGeometry(frameStart, frameEnd, motionFrameTop, motionFrameBottom);
  this->linkMotionToggle->setCenterYGeometryWidthHint(labelStart, linkCenter, menuH);
  this->triadMotionToggle->setCenterYGeometryWidthHint(labelStart, triadeCenter, menuH);

  this->defFrame->setEdgeGeometry(frameStart, frameEnd, defFrameTop, defFrameBottom);
  this->defToggle->setCenterYGeometryWidthHint(labelStart, defCenter, menuH);
  this->defScaleLabel->setCenterYGeometryWidthHint(labelStart, scaleCenter, menuH);
  fieldl = this->defScaleLabel->getXPos() + this->defScaleLabel->getWidth() + border;
  fieldw = width - 2*border - fieldl;
  this->defScaleField->setCenterYGeometry(fieldl, scaleCenter, fieldw, menuH);

  this->legendFrame->setEdgeGeometry(frameStart, frameEnd, legendFrameTop, legendFrameBottom);
  this->fringeToggle->setCenterYGeometryWidthHint(labelStart, fringeCenter, menuH);
  this->legendToggle->setCenterYGeometryWidthHint(labelStart, legendCenter, menuH);

  this->legendColorsLabel->setCenterYGeometryWidthHint(labelStart, colorCenter, menuH);
  this->legendColorsMenu->setCenterYGeometry(menuStart, colorCenter, menuW, menuH);
  this->legendMappingLabel->setCenterYGeometryWidthHint(labelStart, mappingCenter, menuH);
  this->legendMappingMenu->setCenterYGeometry(menuStart, mappingCenter, menuW, menuH);
  this->legendLookLabel->setCenterYGeometryWidthHint(labelStart, lookCenter, menuH);
  this->legendLookMenu->setCenterYGeometry(menuStart, lookCenter, menuW, menuH);

  this->legendMaxLabel->setCenterYGeometryWidthHint(labelStart, maxCenter, menuH);
  this->legendMinLabel->setCenterYGeometryWidthHint(labelStart, minCenter, menuH);

  fieldl = this->legendMaxLabel->getXPos() + this->legendMaxLabel->getWidth() + border;
  fieldw = width - 2*border - fieldl;
  this->legendMaxField->setCenterYGeometry(fieldl, maxCenter, fieldw, menuH);
  this->legendMinField->setCenterYGeometry(fieldl, minCenter, fieldw, menuH);

  this->tickMarkFrame->setEdgeGeometry(labelStart, width - labelStart, tickFrameTop, tickFrameBottom);
  this->tickMarkCountRadio->setCenterYGeometryWidthHint(3*border, countCenter, menuH);
  this->tickMarkSpaceRadio->setCenterYGeometryWidthHint(3*border, spaceCenter, menuH);

  fieldl = this->tickMarkSpaceRadio->getXPos( )+ this->tickMarkSpaceRadio->getWidth() + border;
  fieldw = width - 3*border - fieldl;
  this->tickMarkCountField->setCenterYGeometry(fieldl, countCenter, fieldw, menuH);
  this->tickMarkSpaceField->setCenterYGeometry(fieldl, spaceCenter, fieldw, menuH);

  this->dialogButtons->setEdgeGeometry(0, width, dialogTop, height);
}
//-----------------------------------------------------------------------------

void FuiAnimationControl::onPoppedUp()
{
  this->updateUIValues();
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
