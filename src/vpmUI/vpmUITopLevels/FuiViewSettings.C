// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiViewSettings.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuFrame.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuScale.H"
#include "FFuLib/FFuColorDialog.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuDialogButtons.H"

#include <vector>
#include <cmath>


Fmd_SOURCE_INIT(FUI_VIEWSETTINGS, FuiViewSettings, FFuTopLevelShell);


FuiViewSettings::FuiViewSettings() : myToggleButtonGroup(false), myColorEditButtonGroup(false)
{
  Fmd_CONSTRUCTOR_INIT(FuiViewSettings);

  showTireSettings = false;
}


FuiViewSettings::~FuiViewSettings()
{
  for (const std::pair<FFuColorDialog* const,FFuFrame*>& color : myColorDialogMap)
    delete color.first;
}


void FuiViewSettings::setUIValues(const FFuaUIValues* values)
{
  if (!values) return;

  FuaViewSettingsValues* gsValues = (FuaViewSettingsValues*) values;

  FFuaPalette pal;
  for (int i = 0; i < FuiViewSettings::NSYMBOLS; i++)
    {
      mySymbolToggles[i]->setValue(gsValues->isSymbolOn[i]);
      pal.setStdBackground(gsValues->mySymbolColor[i]);
      mySymbolColFrames[i]->setColors(pal);
    }

  myRevoluteJointToggle->setValue(gsValues->isRevoluteJointsOn);
  myRevoluteJointToggle->setSensitivity(gsValues->isSymbolOn[JOINT]);
  myBallJointToggle->setValue(gsValues->isBallJointsOn);
  myBallJointToggle->setSensitivity(gsValues->isSymbolOn[JOINT]);
  myRigidJointToggle->setValue(gsValues->isRigidJointsOn);
  myRigidJointToggle->setSensitivity(gsValues->isSymbolOn[JOINT]);
  myFreeJointToggle->setValue(gsValues->isFreeJointsOn);
  myFreeJointToggle->setSensitivity(gsValues->isSymbolOn[JOINT]);
  myPrismaticJointToggle->setValue(gsValues->isPrismaticJointsOn);
  myPrismaticJointToggle->setSensitivity(gsValues->isSymbolOn[JOINT]);
  myCylindricJointToggle->setValue(gsValues->isCylindricJointsOn);
  myCylindricJointToggle->setSensitivity(gsValues->isSymbolOn[JOINT]);
  myCamJointToggle->setValue(gsValues->isCamJointsOn);
  myCamJointToggle->setSensitivity(gsValues->isSymbolOn[JOINT]);

  myBeamTriadsToggle->setValue(gsValues->isBeamTriadsOn);
  myBeamTriadsToggle->setSensitivity(gsValues->isSymbolOn[TRIADS]);
  myBeamsToggle->setValue(gsValues->isBeamsOn);
  myBeamCSToggle->setValue(gsValues->isBeamCSOn);
  myBeamCSToggle->setSensitivity(gsValues->isBeamsOn);
  myPartsToggle->setValue(gsValues->isPartsOn);
  myPartCSToggle->setValue(gsValues->isPartCSOn);
  myPartCSToggle->setSensitivity(gsValues->isPartsOn);
  myInternalPartCSsToggle->setValue(gsValues->isInternalPartCSsOn);
  myInternalPartCSsToggle->setSensitivity(gsValues->isPartsOn);
  myPartCoGCSToggle->setValue(gsValues->isPartCoGCSOn);
  myPartCoGCSToggle->setSensitivity(gsValues->isPartsOn);

  myRefPlaneToggle->setValue(gsValues->isRefPlaneOn);
  mySeaStateToggle->setValue(gsValues->isSeaStateOn);
  myWavesToggle->setValue(gsValues->isWavesOn);
  myWavesToggle->setSensitivity(gsValues->isSeaStateOn);

  mySymbolSizeField->setValue(gsValues->mySymbolSize);
  myLineWidthScale->setValue(gsValues->myLineWidth);

  pal.setStdBackground(gsValues->myGroundedColor);
  myGroundedColFrame->setColors(pal);

  pal.setStdBackground(gsValues->myUnattachedColor);
  myUnattachedColFrame->setColors(pal);

  pal.setStdBackground(gsValues->myBackgroundColor);
  myBackgroundColFrame->setColors(pal);

  myFogToggle->setValue(gsValues->isFogOn);
  myFogScale->setValue((int)(100.0*sqrt((double)gsValues->myFogVisibility)));

  mySimpleTransparencyToggle->setValue(gsValues->isSimpleTransparencyOn);
  myAntialiazingToggle->setValue(gsValues->isAntialiazingOn);

  showTireSettings = gsValues->showTireSettings;
}


bool FuiViewSettings::getValues(FuaViewSettingsValues* values) const
{
  if (!values) return false;

  FFuaPalette pal;
  for (int i = 0; i < FuiViewSettings::NSYMBOLS; i++)
    {
      values->isSymbolOn[i] = mySymbolToggles[i]->getValue();
      mySymbolColFrames[i]->getColors(pal);
      values->mySymbolColor[i] = pal.getStdBackground();
    }
  values->isRevoluteJointsOn = myRevoluteJointToggle->getValue();
  values->isBallJointsOn = myBallJointToggle->getValue();
  values->isRigidJointsOn = myRigidJointToggle->getValue();
  values->isFreeJointsOn = myFreeJointToggle->getValue();
  values->isPrismaticJointsOn = myPrismaticJointToggle->getValue();
  values->isCylindricJointsOn = myCylindricJointToggle->getValue();
  values->isCamJointsOn = myCamJointToggle->getValue();

  values->isBeamTriadsOn = myBeamTriadsToggle->getValue();
  values->isBeamsOn = myBeamsToggle->getValue();
  values->isBeamCSOn = myBeamCSToggle->getValue();
  values->isPartsOn = myPartsToggle->getValue();
  values->isPartCSOn = myPartCSToggle->getValue();
  values->isInternalPartCSsOn = myInternalPartCSsToggle->getValue();
  values->isPartCoGCSOn = myPartCoGCSToggle->getValue();
  values->isRefPlaneOn = myRefPlaneToggle->getValue();
  values->isSeaStateOn = mySeaStateToggle->getValue();
  values->isWavesOn = myWavesToggle->getValue();

  values->mySymbolSize = mySymbolSizeField->getDouble();
  values->myLineWidth = myLineWidthScale->getValue();

  myGroundedColFrame->getColors(pal);
  values->myGroundedColor = pal.getStdBackground();

  myUnattachedColFrame->getColors(pal);
  values->myUnattachedColor = pal.getStdBackground();

  myBackgroundColFrame->getColors(pal);
  values->myBackgroundColor = pal.getStdBackground();

  values->isFogOn = myFogToggle->getValue();
  values->myFogVisibility = 0.0001 * myFogScale->getValue() * myFogScale->getValue();

  values->isAntialiazingOn = myAntialiazingToggle->getValue();
  values->isSimpleTransparencyOn = mySimpleTransparencyToggle->getValue();

  return true;
}


void FuiViewSettings::initWidgets(void)
{
  // Frame labels:
  mySymbolsFrame->setLabel("Mechanism symbols:");
  myDefaultColorFrame->setLabel("Default colors:");
  myViewerOptionsFrame->setLabel("Viewer options:");

  // dialog buttons
  myCloseButton->setLabel("Close");
  myCloseButton->setActivateCB(FFaDynCB0M(FuiViewSettings, this, onCloseButtonClicked));

  // Symbols
  myOnOffLabel->setLabel("Visible");
  myColorLabel->setLabel("Color");
  mySymbolSizeLabel->setLabel("Size");
  myLineWidthLabel->setLabel("Line width:");

  mySymbolSizeField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  mySymbolSizeField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  mySymbolSizeField->setAcceptedCB(FFaDynCB1M(FuiViewSettings, this,
					      fieldValueChanged, double));

  myLineWidthScale->setMinMax(0,10);
  myLineWidthScale->setDragCB(FFaDynCB1M(FuiViewSettings, this,
					 sliderValueChanged, int));

  mySymbolToggles[FuiViewSettings::JOINT]->setLabel("Joints");
  mySymbolToggles[FuiViewSettings::SPR_DA]->setLabel("Springs/Dampers");
  mySymbolToggles[FuiViewSettings::GEAR]->setLabel("Gears");
  mySymbolToggles[FuiViewSettings::LOAD]->setLabel("Loads");
  mySymbolToggles[FuiViewSettings::SENSOR]->setLabel("Sensors");
  mySymbolToggles[FuiViewSettings::STRAIN_ROSETTE]->setLabel("Strain rosettes");
  mySymbolToggles[FuiViewSettings::TRIADS]->setLabel("Triads");
  mySymbolToggles[FuiViewSettings::STICKER]->setLabel("Stickers");
  mySymbolToggles[FuiViewSettings::TIRES]->setLabel("Tires");

  myRevoluteJointToggle->setLabel("Revolute joints");
  myBallJointToggle->setLabel("Ball joints");
  myRigidJointToggle->setLabel("Rigid joints");
  myFreeJointToggle->setLabel("Free joints");
  myPrismaticJointToggle->setLabel("Prismatic joints");
  myCylindricJointToggle->setLabel("Cylindric joints");
  myCamJointToggle->setLabel("Cam joints");

  myBeamTriadsToggle->setLabel("Triads between beam elements");
  myBeamsToggle->setLabel("Beams");
  myBeamCSToggle->setLabel("Beam coordinate systems");
  myPartsToggle->setLabel("Parts");
  myPartCSToggle->setLabel("Part coordinate systems");
  myInternalPartCSsToggle->setLabel("Local coordinate systems");
  myPartCoGCSToggle->setLabel("Center of Gravity symbol");

  myRefPlaneToggle->setLabel("Reference Plane");
  mySeaStateToggle->setLabel("Sea State");
  myWavesToggle->setLabel("Waves");

  for (FFuPushButton* button : mySymbolColEditButtons)
    button->setLabel("Edit");

  // Default colors
  myGroundedColLabel->setLabel("Grounded triads:");
  myUnattacedColLabel->setLabel("Unattached symbols:");
  myBackgroundColLabel->setLabel("Modeler background:");

  myGroundedColEditButton->setLabel("Edit");
  myUnattachedColEditButton->setLabel("Edit");
  myBackgroundColEditButton->setLabel("Edit");

  // Viewer options
  myFogToggle->setLabel("Fog");
  myFogVisibilityLabel->setLabel("Visibility:");
  myFogScale->setMinMax(1,1000);
  myFogScale->setDragCB(FFaDynCB1M(FuiViewSettings, this,
				   sliderValueChanged, int));

  mySimpleTransparencyToggle->setLabel("Simple transparency");
  myAntialiazingToggle->setLabel("Anti-Aliasing");

  // add toggles to the lgb:
  myToggleButtonGroup.setGroupToggleCB(FFaDynCB2M(FuiViewSettings, this,
                                                  toggleValueChanged,
                                                  int, bool));

  myToggleButtonGroup.insert(myFogToggle);
  myToggleButtonGroup.insert(myAntialiazingToggle);
  myToggleButtonGroup.insert(mySimpleTransparencyToggle);

  for (int i = 0; i < FuiViewSettings::NSYMBOLS; i++)
    myToggleButtonGroup.insert(mySymbolToggles[i]);

  myToggleButtonGroup.insert(myRevoluteJointToggle);
  myToggleButtonGroup.insert(myBallJointToggle);
  myToggleButtonGroup.insert(myRigidJointToggle);
  myToggleButtonGroup.insert(myFreeJointToggle);
  myToggleButtonGroup.insert(myPrismaticJointToggle);
  myToggleButtonGroup.insert(myCylindricJointToggle);
  myToggleButtonGroup.insert(myCamJointToggle);

  myToggleButtonGroup.insert(myBeamTriadsToggle);
  myToggleButtonGroup.insert(myBeamsToggle);
  myToggleButtonGroup.insert(myBeamCSToggle);
  myToggleButtonGroup.insert(myPartsToggle);
  myToggleButtonGroup.insert(myPartCSToggle);
  myToggleButtonGroup.insert(myInternalPartCSsToggle);
  myToggleButtonGroup.insert(myPartCoGCSToggle);
  myToggleButtonGroup.insert(myRefPlaneToggle);
  myToggleButtonGroup.insert(mySeaStateToggle);
  myToggleButtonGroup.insert(myWavesToggle);


  ////////////////////////////////////////////////////////////////////////
  // the edit button group
  myColorEditButtonGroup.setGroupActivateCB(FFaDynCB1M(FuiViewSettings, this,
                                                       onColorEditClicked,
                                                       FFuComponentBase*));

  // symbols
  int iSym = 0;
  for (FFuColorDialog* colorDialog : mySymbolColDialog)
  {
    myColorEditButtonGroup.insert(mySymbolColEditButtons[iSym],colorDialog);
    colorDialog->setColorChangedCB(FFaDynCB2M(FuiViewSettings,this,       onColDlgChanged,FFuColor,FFuComponentBase*));
    colorDialog->setOkButtonClickedCB(FFaDynCB2M(FuiViewSettings,this,    onColDlgClosed, FFuColor,FFuComponentBase*));
    colorDialog->setCancelButtonClickedCB(FFaDynCB2M(FuiViewSettings,this,onColDlgClosed, FFuColor,FFuComponentBase*));
    myColorDialogMap[colorDialog] = mySymbolColFrames[iSym++];
  }

  // grounded
  myColorEditButtonGroup.insert(myGroundedColEditButton,
                                myGroundedColDialog);

  myGroundedColDialog->setColorChangedCB(FFaDynCB2M(FuiViewSettings,this,         onColDlgChanged, FFuColor, FFuComponentBase*));
  myGroundedColDialog->setOkButtonClickedCB(FFaDynCB2M(FuiViewSettings,this,      onColDlgClosed,  FFuColor, FFuComponentBase*));
  myGroundedColDialog->setCancelButtonClickedCB(FFaDynCB2M(FuiViewSettings,this,  onColDlgClosed,  FFuColor, FFuComponentBase*));
  myColorDialogMap[myGroundedColDialog] = myGroundedColFrame;

  // unattached
  myColorEditButtonGroup.insert(myUnattachedColEditButton,
                                myUnattachedColDialog);

  myUnattachedColDialog->setColorChangedCB(FFaDynCB2M(FuiViewSettings,this,        onColDlgChanged, FFuColor, FFuComponentBase*));
  myUnattachedColDialog->setOkButtonClickedCB(FFaDynCB2M(FuiViewSettings,this,     onColDlgClosed,  FFuColor, FFuComponentBase*));
  myUnattachedColDialog->setCancelButtonClickedCB(FFaDynCB2M(FuiViewSettings,this, onColDlgClosed,  FFuColor, FFuComponentBase*));
  myColorDialogMap[myUnattachedColDialog] = myUnattachedColFrame;

  // background
  myColorEditButtonGroup.insert(myBackgroundColEditButton,
                                myBackgroundColDialog);

  myBackgroundColDialog->setColorChangedCB(FFaDynCB2M(FuiViewSettings,this,        onColDlgChanged, FFuColor, FFuComponentBase*));
  myBackgroundColDialog->setOkButtonClickedCB(FFaDynCB2M(FuiViewSettings,this,     onColDlgClosed,  FFuColor, FFuComponentBase*));
  myBackgroundColDialog->setCancelButtonClickedCB(FFaDynCB2M(FuiViewSettings,this, onColDlgClosed,  FFuColor, FFuComponentBase*));
  myColorDialogMap[myBackgroundColDialog] = myBackgroundColFrame;

  // color dialog labels
  myGroundedColDialog->setTitle("Grounded triads");
  myUnattachedColDialog->setTitle("Unattached symbols");
  myBackgroundColDialog->setTitle("Modeler background");

  mySymbolColDialog[FuiViewSettings::JOINT]->setTitle("Joints");
  mySymbolColDialog[FuiViewSettings::SPR_DA]->setTitle("Springs/Dampers");
  mySymbolColDialog[FuiViewSettings::GEAR]->setTitle("Gears");
  mySymbolColDialog[FuiViewSettings::LOAD]->setTitle("Loads");
  mySymbolColDialog[FuiViewSettings::SENSOR]->setTitle("Sensors");
  mySymbolColDialog[FuiViewSettings::TRIADS]->setTitle("Triads");
  mySymbolColDialog[FuiViewSettings::STICKER]->setTitle("Stickers");
  mySymbolColDialog[FuiViewSettings::TIRES]->setTitle("Tires");

  if (showTireSettings) {
    mySymbolToggles[FuiViewSettings::TIRES]->popUp();
    mySymbolColFrames[FuiViewSettings::TIRES]->popUp();
    mySymbolColEditButtons[FuiViewSettings::TIRES]->popUp();
  }
  else {
    mySymbolToggles[FuiViewSettings::TIRES]->popDown();
    mySymbolColFrames[FuiViewSettings::TIRES]->popDown();
    mySymbolColEditButtons[FuiViewSettings::TIRES]->popDown();
  }

  //create ui's UA object
  FFuUAExistenceHandler::invokeCreateUACB(this);
}


void FuiViewSettings::onColorEditClicked(FFuComponentBase* dlgPtr)
{
  FFuColorDialog *aColorDialog = dynamic_cast<FFuColorDialog*>(dlgPtr);
  if (aColorDialog)
    {
      FFuaPalette pal;
      myColorDialogMap[aColorDialog]->getColors(pal);
      aColorDialog->setInitialColor(pal.getStdBackground());
      aColorDialog->popUp();
    }
}


void FuiViewSettings::onColDlgChanged(FFuColor aColor, FFuComponentBase* dlgPtr)
{
  FFuColorDialog *aColorDialog = dynamic_cast<FFuColorDialog*>(dlgPtr);
  if (aColorDialog)
    {
      FFuaPalette pal;
      pal.setStdBackground(aColor);
      myColorDialogMap[aColorDialog]->setColors(pal);
      this->updateDBValues();
    }
}


void FuiViewSettings::onColDlgClosed(FFuColor aColor, FFuComponentBase* dlgPtr)
{
  FFuColorDialog *aColorDialog = dynamic_cast<FFuColorDialog*>(dlgPtr);
  if (aColorDialog)
    {
      FFuaPalette pal;
      pal.setStdBackground(aColor);
      myColorDialogMap[aColorDialog]->setColors(pal);
      aColorDialog->popDown();
      this->updateDBValues();
    }
}


void FuiViewSettings::placeWidgets(int width,int height)
{
  // Grid lines for main frames and frame text

  // vertical
  int glV1  = 7;
  int glV3  = width - 7;

  // horizontal for Mech symbols
  int corrH = height - 7 - 40;

  int glH1  = 7;
  int glH2  = FFuMultUIComponent::getGridLinePos(corrH, 770);

  // Horizontal for Default colors
  int glH3  = glH2 + FFuMultUIComponent::getGridLinePos(corrH, 20);
  int glH4  = FFuMultUIComponent::getGridLinePos(corrH, 880);

  // Horizontal for Viewer options
  int glH5  = glH4 + FFuMultUIComponent::getGridLinePos(corrH, 20);
  int glH6  = height - 7 - myCloseButton->getHeightHint() - 7;

  // Frame geometry
  mySymbolsFrame->setEdgeGeometry(glV1, glV3, glH1, glH2);
  this->placeSymbols(mySymbolsFrame->getWidth(), mySymbolsFrame->getHeight());

  myDefaultColorFrame->setEdgeGeometry(glV1, glV3, glH3, glH4);
  this->placeDefaultColors(myDefaultColorFrame->getWidth(), myDefaultColorFrame->getHeight());

  myViewerOptionsFrame->setEdgeGeometry(glV1, glV3, glH5, glH6);
  this->placeViewerOptions(myViewerOptionsFrame->getWidth(), myViewerOptionsFrame->getHeight());

  myCloseButton->setEdgeGeometry(glV1, 85, glH6+7, height-7);
}


void FuiViewSettings::placeSymbols(int width, int height)
{
  // vertical gridlines
  int glV1 = FFuMultUIComponent::getGridLinePos(width, 30);
  int glV1_5 = glV1 + myOnOffLabel->getHeightHint();
  int glV5 = FFuMultUIComponent::getGridLinePos(width, 970);

  int editSiz = this->getFontWidth("MEditM")*width/250;

  int glV4 = glV5 - editSiz;
  int glV3 = glV4 - getGridLinePos(width, 30);
  int glV2 = glV3 - editSiz/2;

  int glV2_5 = FFuMultUIComponent::getGridLinePos(width, glV1 + 10 + myLineWidthLabel->getWidthHint(),
						  FFuMultUIComponent::FROM_START);

  // Horizontal rows (Lines) :

  int nSymb = showTireSettings ? FuiViewSettings::NSYMBOLS-1 : FuiViewSettings::TIRES;
  int rowCount = 1 + nSymb + 13;
  int textHeight = myOnOffLabel->getHeightHint();
  int rowHeight = (height - textHeight - 2)/(rowCount + 7);
  int buttHHint = mySymbolColEditButtons[FuiViewSettings::JOINT]->getHeightHint();
  int buttonHeight = rowHeight-2 < buttHHint ? rowHeight-2 : buttHHint;

  int i; std::vector<int> glH(rowCount);
  glH.front() = textHeight + rowHeight/2;
  for (i = 1; i < rowCount; i++)
    glH[i] = glH[i-1] + rowHeight;

  myOnOffLabel->setEdgeGeometry  (glV1, glV1 + myOnOffLabel->getWidthHint(),
				  glH.front()-(myOnOffLabel->getHeightHint()/2)+2,
				  glH.front()+(myOnOffLabel->getHeightHint()/2)+3);

  myColorLabel->setEdgeGeometry  (glV2, glV2 + myOnOffLabel->getWidthHint(),
				  glH.front()-(myOnOffLabel->getHeightHint()/2)+2,
				  glH.front()+(myOnOffLabel->getHeightHint()/2)+3);

  double offset = 0;
  for (i = 0; i < nSymb; i++)
    {
      mySymbolToggles[i]->setEdgeGeometry (glV1, glV1 + mySymbolToggles[i]->getWidthHint(),
		  glH[i + 1] - rowHeight / 2 + offset, glH[i + 1] + rowHeight / 2 + offset);

      mySymbolColEditButtons[i]->setEdgeGeometry (glV4, glV5,
		  glH[i + 1] - buttonHeight / 2 + offset, glH[i + 1] + buttonHeight / 2 + offset);
	  mySymbolColFrames[i]->setEdgeGeometry(glV2, glV3, glH[i + 1] - buttonHeight / 2 + offset, glH[i + 1] + buttonHeight / 2 + offset);

	  if (mySymbolToggles[i] == mySymbolToggles[FuiViewSettings::JOINT])
	  {
		  offset += rowHeight;
		  myRevoluteJointToggle->setEdgeGeometry(glV1_5, glV1_5 + myRevoluteJointToggle->getWidthHint(),
			  glH[i + 1] - rowHeight / 2 + offset, glH[i + 1] + rowHeight / 2 + offset);
		  offset += rowHeight;

		  myBallJointToggle->setEdgeGeometry(glV1_5, glV1_5 + myBallJointToggle->getWidthHint(),
			  glH[i + 1] - rowHeight / 2 + offset, glH[i + 1] + rowHeight / 2 + offset);
		  offset += rowHeight;

		  myRigidJointToggle->setEdgeGeometry(glV1_5, glV1_5 + myRigidJointToggle->getWidthHint(),
			  glH[i + 1] - rowHeight / 2 + offset, glH[i + 1] + rowHeight / 2 + offset);
		  offset += rowHeight;

		  myFreeJointToggle->setEdgeGeometry(glV1_5, glV1_5 + myFreeJointToggle->getWidthHint(),
			  glH[i + 1] - rowHeight / 2 + offset, glH[i + 1] + rowHeight / 2 + offset);
		  offset += rowHeight;

		  myPrismaticJointToggle->setEdgeGeometry(glV1_5, glV1_5 + myPrismaticJointToggle->getWidthHint(),
			  glH[i + 1] - rowHeight / 2 + offset, glH[i + 1] + rowHeight / 2 + offset);
		  offset += rowHeight;

		  myCylindricJointToggle->setEdgeGeometry(glV1_5, glV1_5 + myCylindricJointToggle->getWidthHint(),
			  glH[i + 1] - rowHeight / 2 + offset, glH[i + 1] + rowHeight / 2 + offset);
		  offset += rowHeight;

		  myCamJointToggle->setEdgeGeometry(glV1_5, glV1_5 + myCamJointToggle->getWidthHint(),
			  glH[i + 1] - rowHeight / 2 + offset, glH[i + 1] + rowHeight / 2 + offset);
	  }
    }

  int tHeight = showTireSettings ? rowHeight : 0;
  mySymbolToggles[FuiViewSettings::TRIADS]->setEdgeGeometry(glV1, glV1 + mySymbolToggles[FuiViewSettings::TRIADS]->getWidthHint(),
	  glH[FuiViewSettings::TRIADS] + tHeight - rowHeight / 2 + offset,
	  glH[FuiViewSettings::TRIADS] + tHeight + rowHeight / 2 + offset);

  mySymbolColEditButtons[FuiViewSettings::TRIADS]->setEdgeGeometry(glV4, glV5,
	  glH[FuiViewSettings::TRIADS] + tHeight - buttonHeight / 2 + offset,
	  glH[FuiViewSettings::TRIADS] + tHeight + buttonHeight / 2 + offset);
  mySymbolColFrames[FuiViewSettings::TRIADS]->setEdgeGeometry(glV2, glV3,
	  glH[FuiViewSettings::TRIADS] + tHeight - buttonHeight / 2 + offset,
	  glH[FuiViewSettings::TRIADS] + tHeight + buttonHeight / 2 + offset);

  int idx = nSymb+2;
  myBeamTriadsToggle->setEdgeGeometry (glV1_5, glV1_5 + myBeamTriadsToggle->getWidthHint(),
                                       glH[idx]- rowHeight/2 + offset, glH[idx]+ rowHeight/2 + offset);
  idx++;
  myBeamsToggle->setEdgeGeometry (glV1, glV1 + myBeamsToggle->getWidthHint(),
	  glH[idx] - rowHeight / 2 + offset, glH[idx] + rowHeight / 2 + offset);
  idx++;
  myBeamCSToggle->setEdgeGeometry (glV1_5, glV1_5 + myBeamCSToggle->getWidthHint(),
	  glH[idx] - rowHeight / 2 + offset, glH[idx] + rowHeight / 2 + offset);
  idx++;
  myPartsToggle->setEdgeGeometry (glV1, glV1 + myPartsToggle->getWidthHint(),
	  glH[idx] - rowHeight / 2 + offset, glH[idx] + rowHeight / 2 + offset);
  idx++;
  myPartCSToggle->setEdgeGeometry (glV1_5, glV1_5 + myPartCSToggle->getWidthHint(),
	  glH[idx] - rowHeight / 2 + offset, glH[idx] + rowHeight / 2 + offset);
  idx++;
  myInternalPartCSsToggle->setEdgeGeometry (glV1_5, glV1_5 + myInternalPartCSsToggle->getWidthHint(),
	  glH[idx] - rowHeight / 2 + offset, glH[idx] + rowHeight / 2 + offset);
  idx++;
  myPartCoGCSToggle->setEdgeGeometry (glV1_5, glV1_5 + myPartCoGCSToggle->getWidthHint(),
	  glH[idx] - rowHeight / 2 + offset, glH[idx] + rowHeight / 2 + offset);
  idx++;
  myRefPlaneToggle->setEdgeGeometry (glV1, glV1 + myRefPlaneToggle->getWidthHint(),
	  glH[idx] - rowHeight / 2 + offset, glH[idx] + rowHeight / 2 + offset);
  idx++;
  mySeaStateToggle->setEdgeGeometry (glV1, glV1 + mySeaStateToggle->getWidthHint(),
	  glH[idx] - rowHeight / 2 + offset, glH[idx] + rowHeight / 2 + offset);
  idx++;
  myWavesToggle->setEdgeGeometry (glV1_5, glV1_5 + myWavesToggle->getWidthHint(),
	  glH[idx] - rowHeight / 2 + offset, glH[idx] + rowHeight / 2 + offset);
  idx++;
  mySymbolSizeLabel->setEdgeGeometry (glV1, glV1 + mySymbolSizeLabel->getWidthHint(),
	  glH[idx] - rowHeight / 2 + offset, glH[idx] + rowHeight / 2 + offset);
  mySymbolSizeField->setCenterYGeometry(glV2_5, glH[idx]+ offset, (glV5 - glV2_5) / 2, buttonHeight );
  idx++;
  myLineWidthLabel->setEdgeGeometry (glV1, glV1 + myLineWidthLabel->getWidthHint(),
	  glH[idx] - rowHeight / 2 + offset, glH[idx] + rowHeight / 2 + offset);

  myLineWidthScale->setEdgeGeometry(glV2_5, glV5, glH[idx] - rowHeight / 2 + offset, glH[idx] + rowHeight / 2 + offset);
}


void FuiViewSettings::placeDefaultColors(int width, int height)
{
  // Vertical gridlines
  int editSiz = this->getFontWidth("MEditM")*width/250;

  int glV5 = getGridLinePos(width, 970);
  int glV4 = glV5 - editSiz;
  int glV3 = glV4 - getGridLinePos(width, 30);
  int glV2 = glV3 - editSiz/2;
  int glV1 = getGridLinePos(width, 30);

  // Horizontal lines for Default colors
  int textHeight = myGroundedColLabel->getHeightHint();
  int rowHeight = (height - textHeight - 2)/3;
  int buttonHHint = myGroundedColEditButton->getHeightHint();
  int buttonHeight = rowHeight-2 < buttonHHint ? rowHeight-2 : buttonHHint;

  int glH1 = textHeight + rowHeight/2;
  int glH2 = glH1 + rowHeight;
  int glH3 = glH2 + rowHeight;

  myGroundedColLabel->setEdgeGeometry        (glV1, glV1 + myGroundedColLabel->getWidthHint(),
					      glH1-buttonHeight/2, glH1+buttonHeight/2 + 1);
  myGroundedColEditButton->setEdgeGeometry   (glV4, glV5,
					      glH1-buttonHeight/2, glH1+buttonHeight/2+1);
  myGroundedColFrame->setEdgeGeometry        (glV2, glV3,
					      glH1-buttonHeight/2, glH1+buttonHeight/2+1);

  myUnattacedColLabel->setEdgeGeometry       (glV1, glV1 + myUnattacedColLabel->getWidthHint(),
					      glH2-buttonHeight/2, glH2+ buttonHeight/2+1);
  myUnattachedColEditButton->setEdgeGeometry (glV4, glV5,
					      glH2-buttonHeight/2, glH2+buttonHeight/2+1);
  myUnattachedColFrame->setEdgeGeometry      (glV2, glV3,
					      glH2-buttonHeight/2, glH2+buttonHeight/2+1);

  myBackgroundColLabel->setEdgeGeometry      (glV1, glV1 + myBackgroundColLabel->getWidthHint(),
					      glH3-buttonHeight/2, glH3+buttonHeight/2+1);
  myBackgroundColEditButton->setEdgeGeometry (glV4, glV5,
					      glH3-buttonHeight/2, glH3+buttonHeight/2+1);
  myBackgroundColFrame->setEdgeGeometry      (glV2, glV3,
				              glH3-buttonHeight/2, glH3+buttonHeight/2+1);
}


void FuiViewSettings::placeViewerOptions(int width, int height)
{
  int textHeight = myFogVisibilityLabel->getHeightHint();

  int glV1 = FFuMultUIComponent::getGridLinePos(width, 30);
  int glV2 = FFuMultUIComponent::getGridLinePos(width, 970);
  
  int rowHeight = (height-3 - textHeight) / 3;
  
  int glH1 =  textHeight + rowHeight/2;
  int glH2 =  glH1 + rowHeight;
  int glH3 =  glH2 + rowHeight;
  
  myFogToggle->setEdgeGeometry  (glV1, glV1 + myFogToggle->getWidthHint() ,
				 glH1 - rowHeight/2, //(myFogToggle->getHeightHint()/2) ,
				 glH1 + rowHeight/2);//(myFogToggle->getHeightHint()/2)+1 ); 
  
  mySimpleTransparencyToggle->setEdgeGeometry  (glV1, glV1 + mySimpleTransparencyToggle->getWidthHint() ,
						glH2 - rowHeight/2, //-(mySimpleTransparencyToggle->getHeightHint()/2) ,
						glH2 + rowHeight/2);//+(mySimpleTransparencyToggle->getHeightHint()/2)+1 ); 

  myAntialiazingToggle->setEdgeGeometry  (glV1, glV1 + myAntialiazingToggle->getWidthHint() ,
					  glH3 - rowHeight/2, //-(myAntialiazingToggle->getHeightHint()/2) ,
					  glH3 + rowHeight/2);//+(myAntialiazingToggle->getHeightHint()/2)+1 ); 
  
  myFogVisibilityLabel->setEdgeGeometry (glV1 + myFogToggle->getWidthHint() + 10 , 
					 glV1 + myFogToggle->getWidthHint() + 10 + myFogVisibilityLabel->getWidthHint() ,  
					 glH1 - rowHeight/2, //-(myFogVisibilityLabel->getHeightHint()/2) ,
					 glH1 + rowHeight/2);//+(myFogVisibilityLabel->getHeightHint()/2)+1 ); 

  myFogScale->setEdgeGeometry (  glV1 + myFogToggle->getWidthHint()  + 10 + myFogVisibilityLabel->getWidthHint() + 5, 
				 glV2,  
				 glH1 - rowHeight/2, //-(myFogScale->getHeightHint()/2) ,
				 glH1 + rowHeight/2);//+(myFogScale->getHeightHint()/2)+1 ); 
}


void FuiViewSettings::onPoppedUp()
{
  this->updateUIValues();
}

void FuiViewSettings::onPoppedDown()
{
  for (const std::pair<FFuColorDialog* const,FFuFrame*>& color : myColorDialogMap)
    color.first->popDown();
}


bool FuiViewSettings::onClose()
{
  this->onPoppedDown();
  this->popDown();
  return true;
}


bool FuiViewSettings::updateDBValues(bool)
{
  // obtain values from UI
  FuaViewSettingsValues values; 
  this->getValues(&values);

  // do something elsewhere...
  this->invokeSetAndGetDBValuesCB(&values);

  // Re-set values to ensure value correspondance between ui and db
  this->setUIValues(&values);

  return true;
}
