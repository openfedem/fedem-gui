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
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuDialogButtons.H"

#include <cmath>


Fmd_SOURCE_INIT(FUI_VIEWSETTINGS, FuiViewSettings, FFuTopLevelShell);


FuiViewSettings::FuiViewSettings()
  : myToggleButtonGroup(false), myColorEditButtonGroup(false)
{
  Fmd_CONSTRUCTOR_INIT(FuiViewSettings);
}


FuiViewSettings::~FuiViewSettings()
{
  for (const std::pair<FFuColorDialog* const,FFuFrame*>& col : myColorDialogMap)
    delete col.first;
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
    myColorFrames[i]->setColors(pal);
  }

  myJointSymbolToggles[REVOLUTE]->setValue(gsValues->isRevoluteJointsOn);
  myJointSymbolToggles[BALL]->setValue(gsValues->isBallJointsOn);
  myJointSymbolToggles[RIGID]->setValue(gsValues->isRigidJointsOn);
  myJointSymbolToggles[FREE]->setValue(gsValues->isFreeJointsOn);
  myJointSymbolToggles[PRISMATIC]->setValue(gsValues->isPrismaticJointsOn);
  myJointSymbolToggles[CYLINDRIC]->setValue(gsValues->isCylindricJointsOn);
  myJointSymbolToggles[CAM]->setValue(gsValues->isCamJointsOn);
  for (FFuToggleButton* toggle : myJointSymbolToggles)
    toggle->setSensitivity(gsValues->isSymbolOn[JOINT]);

  myBeamTriadsToggle->setValue(gsValues->isBeamTriadsOn);
  myBeamTriadsToggle->setSensitivity(gsValues->isSymbolOn[TRIADS]);
  myBeamsToggle->setValue(gsValues->isBeamsOn);
  myBeamCSToggle->setValue(gsValues->isBeamCSOn);
  myBeamCSToggle->setSensitivity(gsValues->isBeamsOn);
  myPartsToggle->setValue(gsValues->isPartsOn);
  myPartSymbolToggles[0]->setValue(gsValues->isPartCSOn);
  myPartSymbolToggles[1]->setValue(gsValues->isInternalPartCSsOn);
  myPartSymbolToggles[2]->setValue(gsValues->isPartCoGCSOn);
  for (FFuToggleButton* toggle : myPartSymbolToggles)
    toggle->setSensitivity(gsValues->isPartsOn);

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
  myFogScale->setSensitivity(gsValues->isFogOn);
  myFogScale->setValue((int)(100.0*sqrt((double)gsValues->myFogVisibility)));

  myTransparencyToggle->setValue(gsValues->isSimpleTransparencyOn);
  myAntialiazingToggle->setValue(gsValues->isAntialiazingOn);

  if (gsValues->showTireSettings)
  {
    mySymbolToggles[FuiViewSettings::TIRES]->popUp();
    myColorFrames[FuiViewSettings::TIRES]->popUp();
    myColorEditButtons[FuiViewSettings::TIRES]->popUp();
  }
  else
  {
    mySymbolToggles[FuiViewSettings::TIRES]->popDown();
    myColorFrames[FuiViewSettings::TIRES]->popDown();
    myColorEditButtons[FuiViewSettings::TIRES]->popDown();
  }
}


bool FuiViewSettings::getValues(FuaViewSettingsValues* values) const
{
  if (!values) return false;

  FFuaPalette pal;
  for (int i = 0; i < FuiViewSettings::NSYMBOLS; i++)
  {
    values->isSymbolOn[i] = mySymbolToggles[i]->getValue();
    myColorFrames[i]->getColors(pal);
    values->mySymbolColor[i] = pal.getStdBackground();
  }
  values->isRevoluteJointsOn  = myJointSymbolToggles[REVOLUTE]->getValue();
  values->isBallJointsOn      = myJointSymbolToggles[BALL]->getValue();
  values->isRigidJointsOn     = myJointSymbolToggles[RIGID]->getValue();
  values->isFreeJointsOn      = myJointSymbolToggles[FREE]->getValue();
  values->isPrismaticJointsOn = myJointSymbolToggles[PRISMATIC]->getValue();
  values->isCylindricJointsOn = myJointSymbolToggles[CYLINDRIC]->getValue();
  values->isCamJointsOn       = myJointSymbolToggles[CAM]->getValue();

  values->isBeamTriadsOn = myBeamTriadsToggle->getValue();
  values->isBeamsOn  = myBeamsToggle->getValue();
  values->isBeamCSOn = myBeamCSToggle->getValue();
  values->isPartsOn           = myPartsToggle->getValue();
  values->isPartCSOn          = myPartSymbolToggles[0]->getValue();
  values->isInternalPartCSsOn = myPartSymbolToggles[1]->getValue();
  values->isPartCoGCSOn       = myPartSymbolToggles[2]->getValue();
  values->isRefPlaneOn = myRefPlaneToggle->getValue();
  values->isSeaStateOn = mySeaStateToggle->getValue();
  values->isWavesOn    = myWavesToggle->getValue();

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
  values->isSimpleTransparencyOn = myTransparencyToggle->getValue();

  return true;
}


void FuiViewSettings::initWidgets()
{
  // Frame labels:
  mySymbolsFrame->setLabel("Mechanism symbols:");
  myDefaultColorFrame->setLabel("Default colors:");
  myViewerOptionsFrame->setLabel("Viewer options:");

  // dialog buttons
  myCloseButton->setLabel("Close");
  myCloseButton->setActivateCB(FFaDynCB0M(FuiViewSettings, this, onCloseButtonClicked));

  // Symbols
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

  myJointSymbolToggles[REVOLUTE]->setLabel("Revolute joints");
  myJointSymbolToggles[BALL]->setLabel("Ball joints");
  myJointSymbolToggles[RIGID]->setLabel("Rigid joints");
  myJointSymbolToggles[FREE]->setLabel("Free joints");
  myJointSymbolToggles[PRISMATIC]->setLabel("Prismatic joints");
  myJointSymbolToggles[CYLINDRIC]->setLabel("Cylindric joints");
  myJointSymbolToggles[CAM]->setLabel("Cam joints");

  myBeamTriadsToggle->setLabel("Triads between beam elements");
  myBeamsToggle->setLabel("Beams");
  myBeamCSToggle->setLabel("Beam coordinate systems");
  myPartsToggle->setLabel("Parts");
  myPartSymbolToggles[0]->setLabel("Part coordinate systems");
  myPartSymbolToggles[1]->setLabel("Local coordinate systems");
  myPartSymbolToggles[2]->setLabel("Center of Gravity symbol");

  myRefPlaneToggle->setLabel("Reference Plane");
  mySeaStateToggle->setLabel("Sea State");
  myWavesToggle->setLabel("Waves");

  for (FFuPushButton* button : myColorEditButtons)
    button->setLabel("Edit");

  // Default colors
  myGroundedColEditButton->setLabel("Edit");
  myUnattachedColEditButton->setLabel("Edit");
  myBackgroundColEditButton->setLabel("Edit");

  // Viewer options
  myFogToggle->setLabel("Fog");
  myFogScale->setMinMax(1,1000);
  myFogScale->setDragCB(FFaDynCB1M(FuiViewSettings, this,
				   sliderValueChanged, int));

  myTransparencyToggle->setLabel("Simple transparency");
  myAntialiazingToggle->setLabel("Anti-Aliasing");

  // add toggles to the lgb:
  myToggleButtonGroup.setGroupToggleCB(FFaDynCB2M(FuiViewSettings, this,
                                                  toggleValueChanged,
                                                  int, bool));

  for (FFuToggleButton* toggle : mySymbolToggles)
    myToggleButtonGroup.insert(toggle);

  for (FFuToggleButton* toggle : myJointSymbolToggles)
    myToggleButtonGroup.insert(toggle);

  for (FFuToggleButton* toggle : myPartSymbolToggles)
    myToggleButtonGroup.insert(toggle);

  myToggleButtonGroup.insert(myBeamTriadsToggle);
  myToggleButtonGroup.insert(myBeamsToggle);
  myToggleButtonGroup.insert(myBeamCSToggle);
  myToggleButtonGroup.insert(myPartsToggle);
  myToggleButtonGroup.insert(myRefPlaneToggle);
  myToggleButtonGroup.insert(mySeaStateToggle);
  myToggleButtonGroup.insert(myWavesToggle);

  myToggleButtonGroup.insert(myFogToggle);
  myToggleButtonGroup.insert(myAntialiazingToggle);
  myToggleButtonGroup.insert(myTransparencyToggle);

  // Lambda function for assigning color dialog call-backs.
  auto&& setColorDialogCBs = [this](FFuColorDialog* cDialog)
  {
    cDialog->setColorChangedCB(FFaDynCB2M(FuiViewSettings,this,onColDlgChanged,
                                          const FFuColor&,FFuColorDialog*));
    cDialog->setOkButtonClickedCB(FFaDynCB2M(FuiViewSettings,this,onColDlgClosed,
                                             const FFuColor&,FFuColorDialog*));
    cDialog->setCancelButtonClickedCB(FFaDynCB2M(FuiViewSettings,this,onColDlgClosed,
                                                 const FFuColor&,FFuColorDialog*));
  };

  ////////////////////////////////////////////////////////////////////////
  // the edit button group
  myColorEditButtonGroup.setGroupActivateCB(FFaDynCB1M(FuiViewSettings,this,
                                                       onColorEditClicked,
                                                       FFuComponentBase*));

  // symbols
  int iSym = 0;
  for (FFuColorDialog* colorDialog : myColorDialog)
  {
    myColorEditButtonGroup.insert(myColorEditButtons[iSym],colorDialog);
    myColorDialogMap[colorDialog] = myColorFrames[iSym++];
    setColorDialogCBs(colorDialog);
  }

  // grounded
  myColorEditButtonGroup.insert(myGroundedColEditButton,myGroundedColDialog);
  myColorDialogMap[myGroundedColDialog] = myGroundedColFrame;
  setColorDialogCBs(myGroundedColDialog);

  // unattached
  myColorEditButtonGroup.insert(myUnattachedColEditButton,myUnattachedColDialog);
  myColorDialogMap[myUnattachedColDialog] = myUnattachedColFrame;
  setColorDialogCBs(myUnattachedColDialog);

  // background
  myColorEditButtonGroup.insert(myBackgroundColEditButton,myBackgroundColDialog);
  myColorDialogMap[myBackgroundColDialog] = myBackgroundColFrame;
  setColorDialogCBs(myBackgroundColDialog);

  // color dialog labels
  myGroundedColDialog->setTitle("Grounded triads");
  myUnattachedColDialog->setTitle("Unattached symbols");
  myBackgroundColDialog->setTitle("Modeler background");

  myColorDialog[FuiViewSettings::JOINT]->setTitle("Joints");
  myColorDialog[FuiViewSettings::SPR_DA]->setTitle("Springs/Dampers");
  myColorDialog[FuiViewSettings::GEAR]->setTitle("Gears");
  myColorDialog[FuiViewSettings::LOAD]->setTitle("Loads");
  myColorDialog[FuiViewSettings::SENSOR]->setTitle("Sensors");
  myColorDialog[FuiViewSettings::TRIADS]->setTitle("Triads");
  myColorDialog[FuiViewSettings::STICKER]->setTitle("Stickers");
  myColorDialog[FuiViewSettings::TIRES]->setTitle("Tires");

  //create ui's UA object
  FFuUAExistenceHandler::invokeCreateUACB(this);
}


void FuiViewSettings::onColorEditClicked(FFuComponentBase* dlg)
{
  FFuColorDialog* colDlg = dynamic_cast<FFuColorDialog*>(dlg);
  if (!colDlg) return;

  FFuaPalette pal;
  myColorDialogMap[colDlg]->getColors(pal);
  colDlg->setInitialColor(pal.getStdBackground());
  colDlg->popUp();
}


void FuiViewSettings::onColDlgChanged(const FFuColor& col, FFuColorDialog* dlg)
{
  FFuaPalette pal;
  pal.setStdBackground(col);
  myColorDialogMap[dlg]->setColors(pal);

  this->updateDBValues();
}


void FuiViewSettings::onColDlgClosed(const FFuColor& col, FFuColorDialog* dlg)
{
  FFuaPalette pal;
  pal.setStdBackground(col);
  myColorDialogMap[dlg]->setColors(pal);
  dlg->popDown();

  this->updateDBValues();
}


void FuiViewSettings::onPoppedUp()
{
  this->updateUIValues();
}

void FuiViewSettings::onPoppedDown()
{
  for (const std::pair<FFuColorDialog* const,FFuFrame*>& col : myColorDialogMap)
    col.first->popDown();
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
