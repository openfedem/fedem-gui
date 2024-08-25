// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiCreateTurbineAssembly.H"
#include "vpmUI/vpmUITopLevels/FuiCreateTurbineTower.H"
#include "vpmUI/Fui.H"
#include "vpmUI/Pixmaps/turbines.xpm"
#include "vpmUI/Pixmaps/turbineModel1.xpm"
#include "vpmUI/Pixmaps/turbineModel2.xpm"
#include "vpmUI/Pixmaps/turbineModel3.xpm"
#include "vpmUI/Pixmaps/turbineModel4.xpm"
#include "vpmUI/Pixmaps/turbineModel5.xpm"
#include "vpmUI/Pixmaps/turbineModel6.xpm"

extern const char* info_xpm[];

#include "FFuLib/FFuAuxClasses/FFuaPalette.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuDialogButtons.H"
#include "FFuLib/FFuFileDialog.H"
#include "FFuLib/FFuSpinBox.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuFileBrowseField.H"
#include "FFaLib/FFaOS/FFaFilePath.H"

#include "vpmPM/FpPM.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmFileSys.H"
#include "vpmDB/FmBladeProperty.H"
#include "vpmDB/FmMechanism.H"

#include "vpmApp/vpmAppCmds/FapDBCreateCmds.H"

using namespace FWP;

enum {
  APPLY  = FFuDialogButtons::LEFTBUTTON,
  CANCEL = FFuDialogButtons::MIDBUTTON,
  HELP   = FFuDialogButtons::RIGHTBUTTON
};

Fmd_SOURCE_INIT(FUI_CREATETURBINEASSEMBLY,FuiCreateTurbineAssembly,FFuTopLevelShell);

//----------------------------------------------------------------------------

FuiCreateTurbineAssembly::FuiCreateTurbineAssembly() : myFields{}
{
  Fmd_CONSTRUCTOR_INIT(FuiCreateTurbineAssembly);

  nameField = NULL;
  haveTurbine = false;
  currentBladeDesign = NULL;
}
//----------------------------------------------------------------------------

void FuiCreateTurbineAssembly::setSensitivity(bool isSensitive)
{
  this->dialogButtons->setButtonSensitivity(APPLY,isSensitive);
}
//----------------------------------------------------------------------------

static void getPropertiesPath(std::string& dir)
{
  dir = FpPM::getFullFedemPath("Properties");
}

void FuiCreateTurbineAssembly::initWidgets()
{
  this->headerImage->setPixMap(turbines_xpm);
  this->setMainImage(0,2);

  this->dialogButtons->setButtonClickedCB(FFaDynCB1M(FuiCreateTurbineAssembly,this,
						     onDialogButtonClicked,int));

  this->dialogButtons->setButtonLabel(APPLY,"Generate turbine");
  this->dialogButtons->setButtonLabel(CANCEL,"Close");
  this->dialogButtons->setButtonLabel(HELP,"Help");

  this->drivelineTypeLabel->setLabel("Driveline type");
  this->drivelineTypeMenu->addOption("Gearbox");
  this->drivelineTypeMenu->addOption("Direct");
  this->drivelineTypeMenu->setOptionSelectedCB(FFaDynCB1M(FuiCreateTurbineAssembly,this,onDrivelineTypeChanged,int));

  this->bearingsLabel->setLabel("Bearings");
  this->bearingsMenu->addOption("None");
  this->bearingsMenu->addOption("One bearing");
  this->bearingsMenu->addOption("Two bearings");
  this->bearingsMenu->setOptionSelectedCB(FFaDynCB1M(FuiCreateTurbineAssembly,this,onBearingsChanged,int));

  this->nameLabel->setLabel("Name");
  this->nameField->setLabelMargin(0);
  this->nameField->setLabelWidth(0);

  this->towerBaseFrame->setLabel("Tower base");
  this->myFields[TOWER_X]->setLabel("X");
  this->myFields[TOWER_Y]->setLabel("Y");
  this->myFields[TOWER_Z]->setLabel("Z");
  this->myFields[TOWER_T]->setLabel("T");

  this->hubFrame->setLabel("Hub");
  this->myFields[H1]->setLabel("H1");
  this->myFields[HR]->setLabel("Hr");
  this->myFields[ALPHA]->setLabel("<font face='Symbol'><font size='+1'>a</font></font>");
  this->myFields[BETA]->setLabel("<font face='Symbol'><font size='+1'>b</font></font>");

  this->drivelineFrame->setLabel("Driveline");
  this->myFields[D1]->setLabel("D1");
  this->myFields[D2]->setLabel("D2");
  this->myFields[D3]->setLabel("D3");
  this->myFields[D4]->setLabel("D4");
  this->myFields[D5]->setLabel("D5");
  this->myFields[D6]->setLabel("D6");

  this->myFields[B1]->setLabel("B1");
  this->myFields[B2]->setLabel("B2");

  this->myFields[S]->setLabel("S");
  this->myFields[THETA]->setLabel("<font face='Symbol'><font size='+1'>q</font></font>");

  this->nacelleFrame->setLabel("Nacelle");

  this->myFields[COG_X]->setLabel("CogX");
  this->myFields[COG_Y]->setLabel("CogY");
  this->myFields[COG_Z]->setLabel("CogZ");

  for (FFuLabelField* field : this->myFields) {
    field->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
    field->myField->setDoubleDisplayPrecision(6);
  }

  this->copyButton->setLabel("Copy data");
  this->pasteButton->setLabel("Paste data");
  this->copyButton->setActivateCB(FFaDynCB0M(FuiCreateTurbineAssembly,this,onCopyButtonClicked));
  this->pasteButton->setActivateCB(FFaDynCB0M(FuiCreateTurbineAssembly,this,onPasteButtonClicked));

  this->bladesNumLabel->setLabel("Number of blades");
  this->bladesNumField->setMinMax(2,4);

  this->bladesDesignLabel->setLabel("Blade design");

  this->bladesDesignField->setLabel("");
  this->bladesDesignField->setButtonLabel("...", 20);
  this->bladesDesignField->setGetDefaultDirCB(FFaDynCB1S(getPropertiesPath,std::string&));
  this->bladesDesignField->setFileOpenedCB(FFaDynCB2M(FuiCreateTurbineAssembly,this,
						      onBladeDesignFileSelected,const std::string&,int));
  this->bladesDesignField->addDialogFilter("Blade Definition File","fmm",true);
  this->bladesDesignField->setDialogRememberKeyword("ExternalBladeDefinition");
  this->bladesDesignField->getIOField()->setUseCustomBgColor(true);
  this->bladesDesignField->setToolTip("You must specify a blade design file (see Help)");

  FFuaPalette aPalette;
  aPalette.setFieldBackground(255,155,155); // red
  this->bladesDesignField->getIOField()->setColors(aPalette);

  this->incCtrlSysToggle->setLabel("Include control system");

  this->notesImage->setPixMap(info_xpm);
  this->notesLabel->setLabel("<b>Notes</b>");

  this->placeWidgets(this->getWidth(),this->getHeight());

  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//----------------------------------------------------------------------------

void FuiCreateTurbineAssembly::placeWidgets(int, int height)
{
  int border = this->getBorder();
  int buttonsTop = height - this->dialogButtons->getHeightHint();

  int fontHeight = this->getFontHeigth();
  int fieldHeight = 20; // hard coded field height?!

  int v1 = border;
  int v2 = v1 + 120;
  int y  = border + 170;

  // images

  this->headerImage->setEdgeGeometry(0, 827, 0, 169);

  this->modelImage->setEdgeGeometry(370, 827, 170, 620);

  // driveline and bearings combos

  this->drivelineTypeLabel->setEdgeGeometry(v1, v1+110, y, y+fieldHeight);
  this->bearingsLabel->setEdgeGeometry(v2, v2+100, y, y+fieldHeight);
  this->nameLabel->setEdgeGeometry(v2+110, v2+210, y, y+fieldHeight);
  y += fieldHeight;

  this->drivelineTypeMenu->setEdgeGeometry(v1, v1+110, y, y+fieldHeight);
  this->bearingsMenu->setEdgeGeometry(v2, v2+100, y, y+fieldHeight);
  this->nameField->setEdgeGeometry(v2+110, v2+210, y, y+fieldHeight);
  y += fieldHeight+border;

  // tower base

  this->towerBaseFrame->setEdgeGeometry(v1, v1+110, y, y+110);
  y += fontHeight+7;

  this->myFields[TOWER_X]->setEdgeGeometry(v1+10, v1+90, y, y+fieldHeight);
  this->myFields[TOWER_X]->setLabelWidth(20);
  y += fieldHeight+2;

  this->myFields[TOWER_Y]->setEdgeGeometry(v1+10, v1+90, y, y+fieldHeight);
  this->myFields[TOWER_Y]->setLabelWidth(20);
  y += fieldHeight+2;

  this->myFields[TOWER_Z]->setEdgeGeometry(v1+10, v1+90, y, y+fieldHeight);
  this->myFields[TOWER_Z]->setLabelWidth(20);
  y += fieldHeight+2;

  this->myFields[TOWER_T]->setEdgeGeometry(v1+10, v1+90, y, y+fieldHeight);
  this->myFields[TOWER_T]->setLabelWidth(20);
  y += fieldHeight+10;

  // hub

  this->hubFrame->setEdgeGeometry(v1, v1+110, y, y+110);
  y += fontHeight+7;

  this->myFields[H1]->setEdgeGeometry(v1+10, v1+90, y, y+fieldHeight);
  this->myFields[H1]->setLabelWidth(20);
  y += fieldHeight+2;

  this->myFields[HR]->setEdgeGeometry(v1+10, v1+90, y, y+fieldHeight);
  this->myFields[HR]->setLabelWidth(20);
  y += fieldHeight+2;

  this->myFields[ALPHA]->setEdgeGeometry(v1+10, v1+90, y, y+fieldHeight);
  this->myFields[ALPHA]->setLabelWidth(20);
  y += fieldHeight+2;

  this->myFields[BETA]->setEdgeGeometry(v1+10, v1+90, y, y+fieldHeight);
  this->myFields[BETA]->setLabelWidth(20);

  // driveline

  y = 230;

  this->drivelineFrame->setEdgeGeometry(v2, v2+210, y, 362);
  y += fontHeight+7;

  this->myFields[D1]->setEdgeGeometry(v2+10, v2+90, y, y+fieldHeight);
  this->myFields[D1]->setLabelWidth(20);
  y += fieldHeight+2;

  this->myFields[D2]->setEdgeGeometry(v2+10, v2+90, y, y+fieldHeight);
  this->myFields[D2]->setLabelWidth(20);
  y += fieldHeight+2;

  this->myFields[D3]->setEdgeGeometry(v2+10, v2+90, y, y+fieldHeight);
  this->myFields[D3]->setLabelWidth(20);
  y += fieldHeight+2;

  this->myFields[D4]->setEdgeGeometry(v2+10, v2+90, y, y+fieldHeight);
  this->myFields[D4]->setLabelWidth(20);
  y += fieldHeight+2;

  this->myFields[D5]->setEdgeGeometry(v2+10, v2+90, y, y+fieldHeight);
  this->myFields[D5]->setLabelWidth(20);
  y += fieldHeight+2;

  y = 230+fontHeight+7;

  this->myFields[D6]->setEdgeGeometry(v2+100, v2+185, y, y+fieldHeight);
  this->myFields[D6]->setLabelWidth(20);
  y += fieldHeight+2;

  this->myFields[B1]->setEdgeGeometry(v2+100, v2+185, y, y+fieldHeight);
  this->myFields[B1]->setLabelWidth(20);
  y += fieldHeight+2;

  this->myFields[B2]->setEdgeGeometry(v2+100, v2+185, y, y+fieldHeight);
  this->myFields[B2]->setLabelWidth(20);
  y += fieldHeight+2;

  this->myFields[S]->setEdgeGeometry(v2+100, v2+185, y, y+fieldHeight);
  this->myFields[S]->setLabelWidth(20);
  y += fieldHeight+2;

  this->myFields[THETA]->setEdgeGeometry(v2+100, v2+185, y, y+fieldHeight);
  this->myFields[THETA]->setLabelWidth(20);
  y += fieldHeight+2;

  // nacelle

  y = 368;

  this->nacelleFrame->setEdgeGeometry(v2, v2+115, y, y+90);
  y += fontHeight+7;

  this->myFields[COG_X]->setEdgeGeometry(v2+10, v2+90, y, y+fieldHeight);
  this->myFields[COG_X]->setLabelWidth(25);
  y += fieldHeight+2;

  this->myFields[COG_Y]->setEdgeGeometry(v2+10, v2+90, y, y+fieldHeight);
  this->myFields[COG_Y]->setLabelWidth(25);
  y += fieldHeight+2;

  this->myFields[COG_Z]->setEdgeGeometry(v2+10, v2+90, y, y+fieldHeight);
  this->myFields[COG_Z]->setLabelWidth(25);
  y += fieldHeight+2;

  // copy and paste buttons

  y = 374+40;

  this->copyButton->setEdgeGeometry(v2+130, v2+211, y, y+fieldHeight);
  y += fieldHeight+2;
  this->pasteButton->setEdgeGeometry(v2+130, v2+211, y, y+fieldHeight);
  y += fieldHeight+2;

  // blades

  y = 457;

  this->bladesDesignLabel->setEdgeGeometry(v1, v1+60, y, y+fieldHeight);
  y += fontHeight+7;
  this->bladesDesignField->setEdgeGeometry(v1-5, v1+387, y, y+fieldHeight);
  y += fieldHeight-1;

  this->bladesNumLabel->setEdgeGeometry(v1, v1+84, y, y+fieldHeight);
  y += fontHeight+4;

  this->bladesNumField->setEdgeGeometry(v1, v1+84, y, y+fieldHeight);
  this->incCtrlSysToggle->setEdgeGeometry(v1+110, v1+344, y, y+fieldHeight);
  y += fieldHeight+12;

  // notes

  this->notesImage->setEdgeGeometry(v1, v1+16, y-5, y+11);
  this->notesSeparator->setEdgeGeometry(v1+20, v1+330, y+10, y+13);
  this->notesLabel->setEdgeGeometry(v1+20, v1+330, y-5, y+11);
  this->notesText->setEdgeGeometry(v1, v1+335, y+14, y+66);

  // dialog buttons

  this->dialogButtons->setEdgeGeometry(0,350,buttonsTop,height);
}
//-----------------------------------------------------------------------------

void FuiCreateTurbineAssembly::onPoppedUp()
{
  this->placeWidgets(this->getWidth(),this->getHeight());
  this->updateUIValues();
}
//----------------------------------------------------------------------------

bool FuiCreateTurbineAssembly::onClose()
{
  this->invokeFinishedCB();
  return false;
}
//----------------------------------------------------------------------------

void FuiCreateTurbineAssembly::setMainImage(int drivelineType, int bearings)
{
  switch (drivelineType*10 + bearings)
    {
    case 12: this->modelImage->setPixMap(turbineModel1_xpm); break;
    case 11: this->modelImage->setPixMap(turbineModel2_xpm); break;
    case 10: this->modelImage->setPixMap(turbineModel3_xpm); break;
    case  2: this->modelImage->setPixMap(turbineModel4_xpm); break;
    case  1: this->modelImage->setPixMap(turbineModel5_xpm); break;
    case  0: this->modelImage->setPixMap(turbineModel6_xpm); break;
  }
}
//----------------------------------------------------------------------------

void FuiCreateTurbineAssembly::onDialogButtonClicked(int button)
{
  switch (button)
  {
  case APPLY:
    this->createOrUpdateTurbine();
    break;

  case CANCEL:
    this->invokeFinishedCB();
    break;

  case HELP:
    Fui::showCHM("dialogbox/windpower/turbine-definition.htm");
    break;
  }
}
//----------------------------------------------------------------------------

void FuiCreateTurbineAssembly::onDrivelineTypeChanged(int value)
{
  this->setMainImage(value,bearingsMenu->getSelectedOption());

  switch (value) {
  case 0:
    this->myFields[D4]->setSensitivity(true);
    this->myFields[D5]->setSensitivity(true);
    this->myFields[S]->setSensitivity(true);
    this->myFields[THETA]->setSensitivity(true);
    break;
  case 1:
    this->myFields[D4]->setSensitivity(false);
    this->myFields[D5]->setSensitivity(false);
    this->myFields[S]->setSensitivity(false);
    this->myFields[THETA]->setSensitivity(false);
    break;
  }
}
//----------------------------------------------------------------------------

void FuiCreateTurbineAssembly::onBearingsChanged(int value)
{
  this->setMainImage(drivelineTypeMenu->getSelectedOption(),value);

  switch (value) {
  case 0:
    this->myFields[B1]->setSensitivity(false);
    this->myFields[B2]->setSensitivity(false);
    break;
  case 1:
    this->myFields[B1]->setSensitivity(true);
    this->myFields[B2]->setSensitivity(false);
    break;
  case 2:
    this->myFields[B1]->setSensitivity(true);
    this->myFields[B2]->setSensitivity(true);
    break;
  }
}
//----------------------------------------------------------------------------

void FuiCreateTurbineAssembly::onBladeDesignFileSelected(const std::string& fName, int)
{
  this->bladesDesignField->setFileName(fName);

  FFuaPalette aPalette;
  if (!FmFileSys::isFile(fName))
    aPalette.setFieldBackground(255,155,155); // RED
  else if (FFaFilePath::getPath(fName,false) == FmDB::getMechanismObject()->getAbsBladeFolderPath())
    aPalette.setFieldBackground(215,255,215); // GREEN
  else
    aPalette.setFieldBackground(255,255,208); // ORANGE

  this->bladesDesignField->getIOField()->setColors(aPalette);
}
//----------------------------------------------------------------------------

FFuaUIValues* FuiCreateTurbineAssembly::createValuesObject()
{
  return new FuaCreateTurbineAssemblyValues();
}
//----------------------------------------------------------------------------

void FuiCreateTurbineAssembly::setUIValues(const FFuaUIValues* values)
{
  FuaCreateTurbineAssemblyValues* turbValues = (FuaCreateTurbineAssemblyValues*) values;

  const char* msg1 = "The turbine model must be detached from ground\n"
    "or other structural parts before it can be moved.";
  const char* msg2 = "The nacelle centre of gravity can only be updated\n"
    "from the CoG tab of the Nacelle Part property panel.";

  this->drivelineTypeMenu->selectOption(turbValues->drivelineType);
  this->drivelineTypeMenu->setSensitivity(!turbValues->haveTurbine);
  this->onDrivelineTypeChanged(turbValues->drivelineType);
  this->bearingsMenu->selectOption(turbValues->bearings);
  this->bearingsMenu->setSensitivity(!turbValues->haveTurbine);
  this->onBearingsChanged(turbValues->bearings);
  this->nameField->setValue(turbValues->name);
  for (size_t i = 0; i < turbValues->geom.size() && i < this->myFields.size(); i++)
    this->myFields[i]->setValue(turbValues->geom[i]);

  this->myFields[TOWER_X]->setSensitivity(turbValues->haveTurbine < 2);
  this->myFields[TOWER_Y]->setSensitivity(turbValues->haveTurbine < 2);
  this->myFields[TOWER_Z]->setSensitivity(turbValues->haveTurbine < 2);
  if (turbValues->haveTurbine < 2) {
    this->myFields[TOWER_X]->setToolTip("");
    this->myFields[TOWER_Y]->setToolTip("");
    this->myFields[TOWER_Z]->setToolTip("");
  }
  else {
    this->myFields[TOWER_X]->setToolTip(msg1);
    this->myFields[TOWER_Y]->setToolTip(msg1);
    this->myFields[TOWER_Z]->setToolTip(msg1);
  }

  this->myFields[COG_X]->setSensitivity(!turbValues->haveTurbine);
  this->myFields[COG_Y]->setSensitivity(!turbValues->haveTurbine);
  this->myFields[COG_Z]->setSensitivity(!turbValues->haveTurbine);
  if (!turbValues->haveTurbine) {
    this->myFields[COG_X]->setToolTip("");
    this->myFields[COG_Y]->setToolTip("");
    this->myFields[COG_Z]->setToolTip("");
  }
  else {
    this->myFields[COG_X]->setToolTip(msg2);
    this->myFields[COG_Y]->setToolTip(msg2);
    this->myFields[COG_Z]->setToolTip(msg2);
  }

  this->bladesNumField->setIntValue(turbValues->bladesNum);
  this->bladesNumField->setSensitivity(!turbValues->haveTurbine);
  this->currentBladeDesign = dynamic_cast<FmBladeDesign*>(turbValues->bladesDesign);

  if (currentBladeDesign)
    this->onBladeDesignFileSelected(currentBladeDesign->myModelFile.getValue(),1);

  this->incCtrlSysToggle->setValue(turbValues->incCtrlSys);
  this->setSensitivity(turbValues->isSensitive);

  haveTurbine = turbValues->haveTurbine;

  if (turbValues->haveTurbine) {
    this->notesText->setLabel("You can provide high-level wind turbine model data here.\n"
			      "Click 'Update turbine' to update the existing mechanism model.\n"
			      "All fields use metric and degree values.\n"
			      "The selected blade will be copied to the folder [modelfile name]_blade.");
    this->dialogButtons->setButtonLabel(APPLY,"Update turbine");
  }
  else {
    this->notesText->setLabel("You can provide high-level wind turbine model data here. Click\n"
			      "'Generate turbine' to generate a mechanism model of the turbine.\n"
			      "All fields use metric and degree values.\n"
			      "The selected blade will be copied to the folder [modelfile name]_blade.");
    this->dialogButtons->setButtonLabel(APPLY,"Generate turbine");
  }
}
//-----------------------------------------------------------------------------

void FuiCreateTurbineAssembly::getUIValues(FFuaUIValues* values)
{
  FuaCreateTurbineAssemblyValues* turbValues = (FuaCreateTurbineAssemblyValues*)values;

  turbValues->drivelineType = this->drivelineTypeMenu->getSelectedOption();
  turbValues->bearings      = this->bearingsMenu->getSelectedOption();
  turbValues->name          = this->nameField->getText();
  for (size_t i = 0; i < turbValues->geom.size() && i < this->myFields.size(); i++)
    turbValues->geom[i]     = this->myFields[i]->getValue();
  turbValues->bladesNum     = this->bladesNumField->getIntValue();
  turbValues->bladesDesign  = this->currentBladeDesign;
  turbValues->incCtrlSys    = this->incCtrlSysToggle->getValue();
}
//-----------------------------------------------------------------------------

void FuiCreateTurbineAssembly::createOrUpdateTurbine()
{
  // Read turbine blade design from selected file
  FapDBCreateCmds::readBladeDesign(bladesDesignField->getFileName(),currentBladeDesign);

  bool hadTurbine = haveTurbine; // changes after updateDBValues()
  FpPM::vpmSetUndoPoint("Turbine definition");
  this->updateDBValues();

  // Also update the Tower definition UI and DB,
  // since it depends on the Height field of this UI
  FFuTopLevelShell* twrdef = FFuTopLevelShell::getInstanceByType(FuiCreateTurbineTower::getClassTypeID());
  if (twrdef) dynamic_cast<FuiCreateTurbineTower*>(twrdef)->updateDBValues();

  if (FapDBCreateCmds::updateWindTurbine(hadTurbine))
  {
    this->notesText->setLabel("You can provide high-level wind turbine model data here.\n"
                              "Click 'Update turbine' to update the existing mechanism model.\n"
                              "All fields use metric and degree values.\n"
                              "The selected blade will be copied to the folder [modelfile name]_blade.");
    this->dialogButtons->setButtonLabel(APPLY,"Update turbine");
    Fui::okDialog("Wind turbine mechanism successfully created/updated.");
  }
  else
    Fui::okDialog("Failed to create/update turbine mechanism.");
}
