// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiProperties.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "vpmUI/vpmUIComponents/FuiDynamicProperties.H"
#include "vpmUI/vpmUIComponents/FuiPointEditor.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmUI/vpmUIComponents/FuiCurveDefine.H"
#include "vpmUI/vpmUIComponents/FuiSNCurveSelector.H"
#else
class FuiSNCurveSelector {};
#endif
#include "vpmUI/vpmUIComponents/FuiTriadSummary.H"
#include "vpmUI/vpmUIComponents/FuiJointSummary.H"
#include "vpmUI/vpmUIComponents/FuiJointTabAdvanced.H"
#include "vpmUI/vpmUIComponents/FuiResultTabs.H"
#include "vpmUI/vpmUIComponents/FuiMotionType.H"
#include "vpmUI/vpmUIComponents/FuiVariableType.H"
#include "vpmUI/vpmUIComponents/FuiPositionData.H"
#include "vpmUI/vpmUIComponents/Fui3DPoint.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "vpmDB/FmFileReference.H"
#include "vpmDB/FmJointBase.H"
#include "vpmDB/FmAssemblyBase.H"

#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuFileBrowseField.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuRadioButton.H"
#include "FFuLib/FFuToolButton.H"
#include "FFuLib/FFuTabbedWidgetStack.H"
#include "FFuLib/FFuMemo.H"
#include "FFuLib/FFuTable.H"
#include "FFuLib/FFuScale.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuFileDialog.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFaLib/FFaCmdLineArg/FFaCmdLineArg.H"

#include "vpmApp/FapLicenseManager.H"


FuiProperties::FuiProperties()
{
  IAmSensitive = true;
  IAmShowingHeading = false;
  IAmShowingFriction = false;
  IAmShowingDOF_TZ_Toggle = false;
  IAmShowingRotFormulation = false;
  IAmShowingTranSpringCpl = false;
  IAmShowingRotSpringCpl = false;
  IAmShowingScrew = false;
  IAmShowingSwapTriadButton = false;
  IAmShowingAddMasterButton = false;
  IAmShowingRevMasterButton = false;
  IAmShowingCamData = false;
  IAmShowingRefPlane = false;
  IAmShowingHPRatio = false;
  IAmShowingAxialDamper = false;
  IAmShowingAxialSpring = false;
  IAmShowingSpringChar = false;
  IAmShowingLinkData = false;
  IAmShowingTriadData = false;
  IAmShowingLoadData = false;
  IAmShowingJointData = false;
  IAmShowingGenDBObjData = false;
  IAmShowingFunctionData = false;
  IAmShowingCtrlData = false;
  IAmShowingCtrlInOut = false;
  IAmShowingExtCtrlSysData = false;
  IAmShowingAnimationData = false;
  IAmShowingGraphData = false;
  IAmShowingCurveData = false;
  IAmShowingFileReference = false;
  IAmShowingTireData = false;
  IAmShowingRoadData = false;
  IAmShowingMatPropData = false;
  IAmShowingSeaStateData = false;
  IAmShowingBeamPropData = false;
  IAmShowingStrainRosetteData = false;
  IAmShowingPipeSurfaceData = false;
  IAmShowingGroupData = false;
  IAmShowingRAOData = false;
  IAmShowingEventData = false;
  IAmShowingSubAssData = false;
  IAmShowingUDEData = false;
  IAmShowingBeamData = false;
  IAmShowingShaftData = false;
  IAmShowingTurbineData = false;
  IAmShowingTowerData = false;
  IAmShowingNacelleData = false;
  IAmShowingGeneratorData = false;
  IAmShowingGearboxData = false;
  IAmShowingRotorData = false;
  IAmShowingBladeData = false;
  IAmShowingRiserData = false;
  IAmShowingJacketData = false;
  IAmShowingSoilPileData = false;
  IAmShowingStartGuide = false;

  myTriadResults = NULL;
  myJointResults = NULL;

  myCtrlElementProperties = myExtCtrlSysProperties = NULL;
  myAnimationDefine = myGraphDefine = myCurveDefine = NULL;
  mySNSelector = NULL;
}


void FuiProperties::initWidgets()
{
  // Heading

  myTypeField->setSensitivity(false);
  myIdField->setSensitivity(false);
  myDescriptionField->setLabel("Description");
  myDescriptionField->myField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onStringChanged,const std::string&));
  myDescriptionField->popDown();
  myTagField->setLabel("Tag");
  myTagField->myField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onStringChanged,const std::string&));
  myTopologyView->popDown();

  // Reference Plane

  myRefPlaneHeightField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myRefPlaneHeightField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myRefPlaneWidthField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myRefPlaneWidthField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myRefPlane->popDown();

  // Higher Pairs

  myHPRatioFrame->setLabel("Transmission output ratio");
  myHPRatioFrame->popDown();
  myHPRatioFrame->setMinWidth(250);
  myHPRatioField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myHPRatioField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));

  // Spring characteristics

  mySpringChar->setChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  mySpringChar->popDown();

  // Spring

  mySpringForce->setAsSpring(true);
  mySpringForce->setChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  mySpringDeflCalc->setChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  mySpring->popDown();

  // Damper

  myDamperForce->setAsSpring(false);
  myDamperForce->setChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  myDamperForce->popDown();

  // Part

  mySelectedLinkTab = "Part";
  myLinkTabs->addTabPage(myLinkModelSheet, "Part");
  myLinkTabs->addTabPage(myLinkOriginSheet, "Origin");
  myLinkTabs->setTabSelectedCB(FFaDynCB1M(FuiProperties,this,onLinkTabSelected,int));
  myLinkTabs->popDown();

  myLinkFEnodeSheet->popDown();
  myLinkRedOptSheet->popDown();
  myLinkLoadSheet->popDown();
  myGenericPartCGSheet->popDown();
  myGenericPartMassSheet->popDown();
  myGenericPartStiffSheet->popDown();
  myHydrodynamicsSheet->popDown();
  myMeshingSheet->popDown();
  myAdvancedLinkOptsSheet->popDown();
  myNonlinearLinkOptsSheet->popDown();

  myLinkModelSheet->setValuesChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  myLinkRedOptSheet->setValuesChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  myLinkLoadSheet->setValuesChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  myGenericPartCGSheet->setValuesChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  myGenericPartMassSheet->setValuesChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  myGenericPartMassSheet->materialField->setChangedCB(FFaDynCB1M(FuiProperties,this,onMaterialChanged,FuiQueryInputField*));
  myGenericPartStiffSheet->setValuesChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  myHydrodynamicsSheet->setValuesChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  myMeshingSheet->setValuesChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  myMeshingSheet->materialField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myAdvancedLinkOptsSheet->setValuesChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  myNonlinearLinkOptsSheet->setValuesChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));

  // Beam

  const char* beamLabels[8] = {
    "Mass", "Length",
    "Start", "Stop",
    "X", "Y", "Z",
    NULL };

  myBeamCrossSectionDefField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myBeamCrossSectionDefField->setBehaviour(FuiQueryInputField::REF_NONE);
  myBeamCrossSectionDefField->setButtonMeaning(FuiQueryInputField::EDIT);
  myBeamCrossSectionDefField->setTextForNoRefSelected("");

  myBeamVisualize3DButton->setLabel("Visualize 3D");
  myBeamVisualize3DButton->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  myBeamVisualize3DButton->useUITristate(false);

  myBeamOrientationFrame->setLabel("Local Z-axis definition");

  const char** label = beamLabels;
  myBeamMassField->setLabel(*(label++));
  myBeamMassField->setSensitivity(false);
  myBeamLengthField->setLabel(*(label++));
  myBeamLengthField->setSensitivity(false);

  for (FFuLabelField* field : myBeamVisualize3DFields)
  {
    field->setLabel(*(label++));
    field->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
    field->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  }
  for (FFuLabelField* field : myBeamLocalZField)
  {
    field->setLabel(*(label++));
    field->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
    field->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  }

  myBeamMassField->setLabelWidth(myBeamLengthField->myLabel->getWidthHint());

  myBeamDynProps->initWidgets(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));

  myBeam->popDown();

  // Turbine, Nacelle, Gearbox, etc. (shared)

  mySubassMassField->setLabel("Mass");
  mySubassMassField->setSensitivity(false);

  mySubassLengthField->setLabel("Length");
  mySubassLengthField->setSensitivity(false);

  mySubassMassField->setLabelWidth(mySubassLengthField->myLabel->getWidthHint());

  mySubassCoGFrame->setLabel("Center of Gravity");
  mySubassCoGField->setRefChangedCB(FFaDynCB1M(FuiProperties,this,onCoGRefChanged,bool));
  mySubassCoGField->setSensitivity(false);

  myVisualize3DButton->setLabel("Visualize 3D");
  myVisualize3DButton->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));

  // Turbine

  myTurbineWindRefFrame->setLabel("Wind reference point");
  myTurbineAdvTopologyFrame->setLabel("Advanced topology settings");

  myTurbineWindVertOffsetField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myTurbineWindVertOffsetField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  for (FuiQueryInputField* fld : myTurbineFields)
  {
    fld->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
    fld->setBehaviour(FuiQueryInputField::REF_NONE);
    fld->setButtonMeaning(FuiQueryInputField::EDIT);
  }

  // Generator

  myGeneratorFrame->setLabel("Generator controller");

  myGeneratorTorqueRadioBtn->setLabel("Torque control");
  myGeneratorVelocityRadioBtn->setLabel("Velocity control");
  myGeneratorRadioGroup.insert(myGeneratorTorqueRadioBtn);
  myGeneratorRadioGroup.insert(myGeneratorVelocityRadioBtn);
  myGeneratorRadioGroup.setExclusive(true);
  myGeneratorRadioGroup.setGroupToggleCB(FFaDynCB2M(FuiProperties,this,onIntBoolChanged,int,bool));
  myGeneratorTorqueField->setBehaviour(FuiQueryInputField::REF_NUMBER);
  myGeneratorTorqueField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myGeneratorVelocityField->setBehaviour(FuiQueryInputField::REF_NUMBER);
  myGeneratorVelocityField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));

  // Shaft

  myShaftCrossSectionDefField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myShaftCrossSectionDefField->setBehaviour(FuiQueryInputField::REF_NONE);
  myShaftCrossSectionDefField->setButtonMeaning(FuiQueryInputField::EDIT);
  myShaftCrossSectionDefField->setTextForNoRefSelected("");

  myShaftDynProps->initWidgets(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));

  myShaftNote->setText("Changes applied to the fields above will apply to all elements contained in this structure. "
    "Changes to individual elements can be made by editing their respective property fields.");

  // Blades

  myBladePitchControlFrame->setLabel("Pitch control");

  myBladePitchControlField->setBehaviour(FuiQueryInputField::REF_NUMBER);
  myBladePitchControlField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));

  myBladeFixedPitchToggle->setLabel("Fixed pitch");
  myBladeFixedPitchToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));

  myBladeIceFrame->setLabel("Ice layer");

  myBladeIceLayerToggle->setLabel("Add ice layer");
  myBladeIceLayerToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));

  myBladeIceThicknessField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myBladeIceThicknessField->setLabel("Thickness");
  myBladeIceThicknessField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  // Riser, Jacket and Soil Pile

  myRiserInternalToDefField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myRiserInternalToDefField->setBehaviour(FuiQueryInputField::REF_NONE);
  myRiserInternalToDefField->setButtonMeaning(FuiQueryInputField::EDIT);
  myRiserInternalToDefField->setSensitivity(false);

  myRiserVisualize3DStartAngleField->setLabel("Start");
  myRiserVisualize3DStartAngleField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  myRiserVisualize3DStartAngleField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myRiserVisualize3DStopAngleField->setLabel("Stop");
  myRiserVisualize3DStopAngleField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  myRiserVisualize3DStopAngleField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));

  myRiserMudButton->setLabel("Internal Liquid");
  myRiserMudButton->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));

  myRiserMudDensityField->setLabel("Mud density");
  myRiserMudDensityField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myRiserMudDensityField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  myRiserMudLevelField->setLabel("Mud level");
  myRiserMudLevelField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myRiserMudLevelField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myRiserMudLevelField->setLabelWidth(myRiserMudDensityField->myLabel->getWidthHint());

  // Joints

  mySelectedJointTab = "Summary";
  myJointTabs->addTabPage(myJointSummary, "Summary");
  myJointTabs->setTabSelectedCB(FFaDynCB1M(FuiProperties,this,onJointTabSelected,int));
  myJointTabs->popDown();

  myJointPosition->popDown();

  for (FuiJointDOF* jdof : myJointDofs) {
    jdof->setChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
    jdof->motionType->setChangedCB(FFaDynCB0M(FuiProperties,this,updateUIValues));
    jdof->motionType->setValuesChangedCB(FFaDynCB1M(FuiProperties,this,onJointDofChanged,int));
    jdof->initialVel->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
#ifdef FT_HAS_FREQDOMAIN
    jdof->freqToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
#endif
  }

  myJointResults->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  myJointResults->popDown();

  mySwapTriadButton->setLabel("Swap Master and Slave Triad");
  mySwapTriadButton->popDown();

  myAddMasterButton->setLabel("Add Master");
  myAddMasterButton->popDown();

  myRevMasterButton->setLabel("Reverse Masters");
  myRevMasterButton->popDown();

  // Pipe Surface

  myPipeRadiusField->setLabel("Radius");
  myPipeRadiusField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myPipeRadiusField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myPipeRadiusField->popDown();

  // Cam

  myJointSummary->myCamThicknessField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myJointSummary->myCamWidthField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myJointSummary->myRadialToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));

  // Screw connection

  myJointSummary->myScrewToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  myJointSummary->myScrewRatioField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));

  // Friction

  myJointSummary->myFriction->setChangedCB(FFaDynCB2M(FuiProperties,this,onIntDoubleChanged,int,double));
  myJointSummary->myFrictionDof->setOptionSelectedCB(FFaDynCB1M(FuiProperties,this,onIntChanged,int));

  // TZ dof toggle

  myJointSummary->myDOF_TZ_Toggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));

  // Rotational formulation and Spring couplings

  myJointAdvancedTab->setOptionChangedCB(FFaDynCB1M(FuiProperties,this,onNonConstStringChanged,std::string));
  myJointAdvancedTab->popDown();

  // Triad

  mySelectedTriadTab = "General";
  myTriadTabs->addTabPage(myTriadSummary, "General");
  myTriadTabs->addTabPage(myTriadPosition, "Origin");
  myTriadTabs->setTabSelectedCB(FFaDynCB1M(FuiProperties,this,onTriadTabSelected,int));
  myTriadTabs->popDown();

  myTriadSummary->mySysDirMenu->setOptionSelectedCB(FFaDynCB1M(FuiProperties,this,onIntChanged,int));
  myTriadSummary->myConnectorMenu->setOptionSelectedCB(FFaDynCB1M(FuiProperties,this,onIntChanged,int));

  myTriadSummary->myMassField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myTriadSummary->myIxField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myTriadSummary->myIyField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myTriadSummary->myIzField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));

  for (FuiTriadDOF* tdof : myTriadDofs) {
    tdof->setChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
    tdof->motionType->setChangedCB(FFaDynCB0M(FuiProperties,this,updateUIValues));
    tdof->motionType->setValuesChangedCB(FFaDynCB1M(FuiProperties,this,onTriadDofChanged,int));
    tdof->initialVel->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
#ifdef FT_HAS_FREQDOMAIN
    tdof->freqToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
#endif
  }

  myTriadResults->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  myTriadResults->popDown();

  // Load

  myLoadMagnitude->setChangedCB(FFaDynCB2M(FuiProperties,this,onIntDoubleChanged,int,double));
  myLoadMagnitude->setBehaviour(FuiQueryInputField::REF_NUMBER);

  myAttackPointEditor->hideOnWhatDisplay(true);
  myAttackPointEditor->setPointChangedCB(FFaDynCB2M(FuiProperties,this,onVecBoolChanged,const FaVec3&,bool));
  myAttackPointEditor->setRefChangedCB(FFaDynCB1M(FuiProperties,this,onBoolTouched,bool));

  myFromPointEditor->hideApplyButton(true);
  myFromPointEditor->setPointChangedCB(FFaDynCB2M(FuiProperties,this,onVecBoolChanged,const FaVec3&,bool));
  myFromPointEditor->setRefChangedCB(FFaDynCB1M(FuiProperties,this,onBoolTouched,bool));

  myToPointEditor->hideApplyButton(true);
  myToPointEditor->setPointChangedCB(FFaDynCB2M(FuiProperties,this,onVecBoolChanged,const FaVec3&,bool));
  myToPointEditor->setRefChangedCB(FFaDynCB1M(FuiProperties,this,onBoolTouched,bool));

  myLoad->popDown();

  // Generic DB Object

  myGenDBObjTypeField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onStringChanged,const std::string&));
  myGenDBObjTypeField->setToolTip("Enter the keyword identifying the object type for the dynamics solver here");

  myGenDBObjDefField->setTextChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  myGenDBObjDefField->setToolTip("Enter the solver input file data defining the object here.\n"
                                 "Remember to press the Enter key at the end, "
                                 "also when editing existing data.");

  myGenDBObject->popDown();

  // File Reference

  myFileReferenceBrowseField->setAbsToRelPath("yes");
  myFileReferenceBrowseField->setDialogRememberKeyword("FileRefBrowseField");
  myFileReferenceBrowseField->setFileOpenedCB(FFaDynCB2M(FuiProperties,this,onFileRefChanged,const std::string&,int));
  for (const FmFileRefExt& ext : FmFileReference::getExtensions())
    myFileReferenceBrowseField->addDialogFilter(ext.first,ext.second);
  myFileReferenceBrowseField->addAllFilesFilter(true);
  myFileReferenceBrowseField->popDown();

  // Tire

  myTireDataFileField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myTireDataFileField->setBehaviour(FuiQueryInputField::REF_TEXT);
  myTireDataFileField->setEditSensitivity(false);

  myBrowseTireFileButton->setLabel("Browse...");
  myRoadField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myRoadField->setBehaviour(FuiQueryInputField::REF_NONE);

  myTireModelMenu->setOptionChangedCB(FFaDynCB1M(FuiProperties,this,onNonConstStringChanged,std::string));

  mySpindelOffset->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  mySpindelOffset->setInputCheckMode(FFuIOField::DOUBLECHECK);

  myTire->popDown();

  // Road

  myUseFuncRoadRadio->setLabel("Road defined by function");
  myRoadFuncField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myRoadFuncField->setBehaviour(FuiQueryInputField::REF_NONE);

  for (FFuIOField* field : myRoadFields)
  {
    field->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
    field->setInputCheckMode(FFuIOField::DOUBLECHECK);
  }

  myUseFileRoadRadio->setLabel("Road defined by file");
  myRoadDataFileField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myRoadDataFileField->setBehaviour(FuiQueryInputField::REF_TEXT);
  myRoadDataFileField->setEditSensitivity(false);

  myBrowseRoadFileButton->setLabel("Browse...");

  myRoadTypeToggleGroup.insert(myUseFileRoadRadio);
  myRoadTypeToggleGroup.insert(myUseFuncRoadRadio);
  myRoadTypeToggleGroup.setExclusive(true);
  myRoadTypeToggleGroup.setGroupToggleCB(FFaDynCB2M(FuiProperties,this,onIntBoolChanged,int,bool));

  myRoad->popDown();

  // Material properties

  myMatPropFrame->setLabel("Material data");
  myMatPropFrame->popDown();

  myMatPropRhoField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myMatPropRhoField->setLabel("<font face='Symbol'><font size='+1'>r</font></font>");
  myMatPropRhoField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myMatPropEField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myMatPropEField->setLabel("E");
  myMatPropEField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myMatPropNuField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myMatPropNuField->setLabel("<font face='Symbol'><font size='+1'>n</font></font>");
  myMatPropNuField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myMatPropGField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myMatPropGField->setLabel("G");
  myMatPropGField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  // Sea state

  mySeaStateWidthField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  mySeaStateWidthField->setLabel("X-length");
  mySeaStateWidthField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  mySeaStateHeightField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  mySeaStateHeightField->setLabel("Y-length");
  mySeaStateHeightField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  mySeaStateWidthPosField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  mySeaStateWidthPosField->setLabel("X");
  mySeaStateWidthPosField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  mySeaStateHeightPosField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  mySeaStateHeightPosField->setLabel("Y");
  mySeaStateHeightPosField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  mySeaStateNumPoints->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  mySeaStateNumPoints->setLabel("Quantization");
  mySeaStateNumPoints->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  mySeaStateShowGridToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  mySeaStateShowGridToggle->setLabel("Show quantization lines");

  mySeaStateShowSolidToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  mySeaStateShowSolidToggle->setLabel("Show solid");

  mySeaState->popDown();

  // Beam properties

  mySelectedBeamPropTab = "Structural";
  myBeamPropTabs->addTabPage(myBeamPropSummary, "Structural");
  myBeamPropTabs->addTabPage(myBeamPropHydro, "Hydrodynamics");
  myBeamPropTabs->setTabSelectedCB(FFaDynCB1M(FuiProperties,this,onBeamPropTabSelected,int));
  myBeamPropTabs->popDown();
  // Change hooks (so that changes get committed in DB) :
  myBeamPropSummary->myCrossSectionTypeMenu->setOptionSelectedCB(FFaDynCB1M(FuiProperties,this,onIntChanged,int));
  myBeamPropSummary->myMaterialDefField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myBeamPropSummary->myDependencyButton->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  myBeamPropSummary->setAcceptedCBs(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myBeamPropHydro->setToggledCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  myBeamPropHydro->setAcceptedCBs(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));

  // Strain rosette

  myStrRosTypeMenu->setOptionSelectedCB(FFaDynCB1M(FuiProperties,this,onIntChanged,int));

  myStrRosNodesField->setSensitivity(false);
  myStrRosEditNodesButton->setLabel("Edit");

  myStrRosAngleField->setLabel("Angle offset [Deg]");
  myStrRosAngleField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myStrRosAngleField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myStrRosEditDirButton->setLabel("Edit reference direction");

  myStrRosHeightField->setLabel("Height");
  myStrRosHeightField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myStrRosHeightField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myStrRosUseFEHeightToggle->setLabel("Use thickness from FE Mesh");
  myStrRosUseFEHeightToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  myStrRosFlipZButton->setLabel("Change side");
  myStrRosFlipZButton->setMaxWidth(150);

  myStrRosEmodField->setLabel("E-Module");
  myStrRosEmodField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myStrRosEmodField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myStrRosNuField->setLabel("Poissons ratio");
  myStrRosNuField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myStrRosNuField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myStrRosEmodField->setLabelWidth(myStrRosNuField->myLabel->getWidthHint());
  myStrRosUseFEMatToggle->setLabel("Use material from FE Mesh");
  myStrRosUseFEMatToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));

  myResetStartStrainsToggle->setLabel("Set start strains to zero");
  myResetStartStrainsToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  myStrRosEditDirButton->setMinWidth(myResetStartStrainsToggle->getWidthHint()-5);

  myStrainRosette->popDown();

  // Element group

  myFatigueFrame->setLabel("Fatigue parameters");
  myFatigueFrame->popDown();

  myFatigueToggle->setLabel("Enable fatigue calculation");
  myFatigueToggle->setToolTip("Calculation of damage and life in the Strain Coat Recovery"
			      "\non this element group, based on the selected S-N curve");
  myFatigueToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));

#ifdef FT_HAS_GRAPHVIEW
  mySNSelector->setDataChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
#endif

  myScfField->setLabel("Stress concentration factor");
  myScfField->setToolTip("The computed stress is scaled by this value"
			 "\nbefore it enters the damage calculation");
  myScfField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myScfField->myField->setDoubleDisplayMode(FFuIOField::AUTO,6,1);
  myScfField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));

  // RAO vessel motion

  myRAOFileField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myRAOFileField->setBehaviour(FuiQueryInputField::REF_TEXT);
  myRAOFileField->setEditSensitivity(false);
  myBrowseRAOFileButton->setLabel("Browse...");

  myWaveFuncField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myWaveFuncField->setBehaviour(FuiQueryInputField::REF_NONE);

  myWaveDirMenu->setOptionSelectedCB(FFaDynCB1M(FuiProperties,this,onIntChanged,int));
  myWaveDirMenu->setToolTip("Angle (in degrees) between the local X-axis of the"
			    "\nVessel Triad and the propagating wave direction,"
			    "\ni.e., a zero angle means the waves propagate in"
			    "\nthe direction of the positive local X-axis.");

  myMotionScaleField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myMotionScaleField->setBehaviour(FuiQueryInputField::REF_NONE);

  myRAO->popDown();

  // Simulation event

  myEventProbability->setLabel("Event probability");
  myEventProbability->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myEventProbability->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  mySelectEventButton->setLabel("Set as active event");
  mySelectEventButton->setActivateCB(FFaDynCB0M(FuiProperties,this,onEventActivated));
  mySelectEventButton->setMaxWidth(200);
  myActiveEventLabel->setLabel("This is the current active event.\n"
			       "All graph plotting and animations "
			       "will use the results of this event.");

  mySimEvent->popDown();

  // Subassembly

  mySubassFileField->setLabel("Subassembly model file");
  mySubassFileField->setAbsToRelPath("yes");
  mySubassFileField->setDialogType(FFuFileDialog::FFU_SAVE_FILE);
  mySubassFileField->setDialogRememberKeyword("SubassFileField");
  mySubassFileField->addDialogFilter("Fedem model file","fmm");
  mySubassFileField->setFileOpenedCB(FFaDynCB2M(FuiProperties,this,onFileRefChanged,const std::string&,int));
  mySubassFileField->setToolTip("The objects of this Subassembly will be saved"
				"\nin this model file when saving the model");

  mySubassembly->popDown();

  // Function properties

  myFunctionProperties->popDown();

  // Control system properties

  if (myCtrlElementProperties) myCtrlElementProperties->popDown();
  if (myExtCtrlSysProperties)  myExtCtrlSysProperties->popDown();

  // Animation, Graph and Curve properties

  if (myAnimationDefine) myAnimationDefine->popDown();
  if (myGraphDefine) myGraphDefine->popDown();
  if (myCurveDefine) myCurveDefine->popDown();

  // Start guide

  IAmShowingStartGuide = myStartGuide->initWidgets() ? 1 : -1;
  if (IAmShowingStartGuide > 0)
    myStartGuide->popUp();
  else
    myStartGuide->popDown();

  // Create the UA class to communicate with the application

  FFuUAExistenceHandler::invokeCreateUACB(this);
}


void FuiProperties::placeWidgets(int width, int height)
{
  if (IAmShowingStartGuide > 0)
    myStartGuide->placeWidgets(width,height);

  const int border = 2;
  int fieldHeight = myDescriptionField->getHeightHint();
  int xpos = width - fieldHeight - border;
  int ypos = fieldHeight + 2*border;
  helpBtn->setSizeGeometry(xpos, border, fieldHeight, fieldHeight);
  xpos -= fieldHeight + border;
  forwardBtn->setSizeGeometry(xpos, border, fieldHeight, fieldHeight);
  xpos -= fieldHeight + border;
  backBtn->setSizeGeometry(xpos, border, fieldHeight, fieldHeight);

  if (IAmShowingHeading)
  {
    myHeading->setSizeGeometry(0, border, xpos-border, fieldHeight);
    myTopology->setSizeGeometry(0,ypos,width/4,height-ypos);
    myProperty->setSizeGeometry(width/4+border,ypos,width*3/4,height-ypos);
  }
}


void FuiStartGuide::placeWidgets(int width, int height)
{
  const int cx2 = width*2/11;

  // Adjust start guide font size (seems not working)
  std::string text = myContentLabel->getLabel();
  size_t ipos = text.find("font-size:");
  if (ipos != std::string::npos)
  {
    int nFontSize, w = width-cx2, h = height-27;
    if (w >= 1105 && h >= 260)
      nFontSize = 12;
    else if (w >= 1015 && h >= 247)
      nFontSize = 11;
    else if (w >= 900 && h >= 220)
      nFontSize = 10;
    else if (w >= 860 && h >= 195)
      nFontSize = 9;
    else
      nFontSize = 8;

    char buf[16];
    sprintf(buf,"font-size:%dpt;",nFontSize);
    if (text.find(buf) != ipos)
    {
      size_t jpos = text.find("pt;",ipos);
      text.replace(ipos,jpos-ipos+3,buf);
      myContentLabel->setLabel(text);
    }
  }

  const float aspectRatio = 8.0f/4.13f;
  const int hwide = cx2/aspectRatio;
  if (hwide > height) // wide container
    myLogoImage->setSizeGeometry(0, 0, cx2, hwide);
  else // tall container
  {
    int w = height*aspectRatio;
    myLogoImage->setSizeGeometry(cx2-w, 0, w, height);
  }

  this->setSizeGeometry(0, 0, width, height);
  myHeading->setSizeGeometry(cx2+8, 3, width-cx2-78, 22);
  myContentLabel->setSizeGeometry(cx2+3, 27, width-cx2-4, height-28);
  myLogoBorderTop->setSizeGeometry(0, 0, cx2, 3);
  myLogoBorderRight->setSizeGeometry(cx2, 0, 4, 27);
  myBorderTop->setSizeGeometry(cx2+1, 24, width-cx2-1, 3);
  myBorderRight->setSizeGeometry(width-3, 27, 3, height);
  myBorderBottom->setSizeGeometry(cx2, height-1, width-cx2-3, 1);
}


///////////////////////////////////////////////
//
// Dynamic Widgets : Setup of widgets on selection
//
///////

void FuiProperties::buildDynamicWidgets(const FFuaUIValues* values)
{
  backBtn->updateSensitivity();
  forwardBtn->updateSensitivity();
  helpBtn->updateSensitivity();

  const FuaPropertiesValues* pv = dynamic_cast<const FuaPropertiesValues*>(values);
  if (!pv) return;

  IAmShowingHeading           = pv->showHeading;
  IAmShowingFriction          = pv->showFriction;
  IAmShowingDOF_TZ_Toggle     = pv->showDOF_TZ_Toggle;
  IAmShowingRotFormulation    = pv->showRotFormulation;
  IAmShowingRotSpringCpl      = pv->showSpringCpl > 0;
  IAmShowingTranSpringCpl     = pv->showSpringCpl == 1;
  IAmShowingRefPlane          = pv->showRefPlane;
  IAmShowingScrew             = pv->showScrew;
  IAmShowingSwapTriadButton   = pv->showSwapTriadButton;
  IAmShowingAddMasterButton   = pv->showAddMasterButton;
  IAmShowingRevMasterButton   = pv->showReverseMasterButton;
  IAmShowingCamData           = pv->showCamData;
  IAmShowingHPRatio           = pv->showHPRatio;
  IAmShowingAxialDamper       = pv->showAxialDamper;
  IAmShowingAxialSpring       = pv->showAxialSpring;
  IAmShowingSpringChar        = pv->showSpringChar;
  IAmShowingLinkData          = pv->showLinkData;
  IAmShowingTriadData         = pv->showTriadData && pv->isSlave ? 2 : pv->showTriadData;
  IAmShowingLoadData          = pv->showLoadData;
  IAmShowingJointData         = pv->showJointData > 0;
  IAmShowingFunctionData      = pv->showFunctionData;
  IAmShowingCtrlData          = pv->showCtrlData;
  IAmShowingCtrlInOut         = pv->showCtrlInOut;
  IAmShowingAnimationData     = pv->showAnimationData;
  IAmShowingGraphData         = pv->showGraphData;
  IAmShowingCurveData         = pv->showCurveData;
  IAmShowingGenDBObjData      = pv->showGenDBObj;
  IAmShowingFileReference     = pv->showFileReference;
  IAmShowingExtCtrlSysData    = pv->showExtCtrlSysData;
  IAmShowingTireData          = pv->showTireData;
  IAmShowingRoadData          = pv->showRoadData;
  IAmShowingMatPropData       = pv->showMatPropData;
  IAmShowingSeaStateData      = pv->showSeaStateData;
  IAmShowingBeamPropData      = pv->showBeamPropData;
  IAmShowingStrainRosetteData = pv->showStrainRosetteData;
  IAmShowingPipeSurfaceData   = pv->showPipeSurfaceData;
  IAmShowingGroupData         = pv->showGroupData;
  IAmShowingRAOData           = pv->showRAOData;
  IAmShowingEventData         = pv->showSimEventData;
  IAmShowingSubAssData        = pv->showSubassemblyData;
  IAmShowingUDEData           = pv->showUDEData;
  IAmShowingBeamData          = pv->showBeamData;
  IAmShowingShaftData         = pv->showShaftData;
  IAmShowingTurbineData       = pv->showTurbineData;
  IAmShowingTowerData         = pv->showTowerData;
  IAmShowingNacelleData       = pv->showNacelleData;
  IAmShowingGeneratorData     = pv->showGeneratorData;
  IAmShowingGearboxData       = pv->showGearboxData;
  IAmShowingRotorData         = pv->showRotorData;
  IAmShowingBladeData         = pv->showBladeData;
  IAmShowingRiserData         = pv->showRiserData;
  IAmShowingJacketData        = pv->showJacketData;
  IAmShowingSoilPileData      = pv->showSoilPileData;
  if (IAmShowingStartGuide >= 0)
    IAmShowingStartGuide      = pv->showStartGuide;

  // Heading

  if (pv->showHeading)
  {
    myIdField->popUp();
    myDescriptionField->popUp();
    myTagField->popUp();
    myTopologyView->popUp();
    myTopologyView->setTree(pv->myTopology);
    myProperty->popUp();
  }
  else
  {
    myIdField->popDown();
    myDescriptionField->popDown();
    myTagField->popDown();
    myTopologyView->popDown();
    myProperty->popDown();
  }

  // Reference Plane

  if (pv->showRefPlane)
  {
    myRefPlanePosition->setEditedObjs(pv->objsToPosition);
    myRefPlane->popUp();
  }
  else
  {
    myRefPlanePosition->setEditedObjs();
    myRefPlane->popDown();
  }

  // Higher Pair

  if (pv->showHPRatio)
    myHPRatioFrame->popUp();
  else
    myHPRatioFrame->popDown();

  // Spring characteristics

  if (pv->showSpringChar)
    mySpringChar->popUp();
  else
    mySpringChar->popDown();

  // Axial Spring

  if (pv->showAxialSpring)
  {
    mySpringForce->buildDynamicWidgets(pv->myAxialSprForceValues);
    mySpringDeflCalc->buildDynamicWidgets(pv->myAxialSpringDefCalcValues);
    mySpring->popUp();
  }
  else
    mySpring->popDown();

  // Axial Damper

  if (pv->showAxialDamper)
  {
    myDamperForce->buildDynamicWidgets(pv->myAxialDaForceValues);
    myDamperForce->popUp();
  }
  else
    myDamperForce->popDown();

  // Link

  if (pv->showLinkData)
  {
    myLinkTabs->setCurrentTab(mySelectedLinkTab);
    myLinkLoadSheet->buildDynamicWidgets(pv->myLinkValues);
    myLinkTabs->popUp();
  }
  else
  {
    myLinkOriginSheet->setEditedObjs();
    myGenericPartCGSheet->setEditedObjs();
    myLinkTabs->popDown();
  }

  // Beam or User-defined element

  if (pv->showBeamData)
  {
    myBeam->popUp();
    if (pv->myHideCrossSection)
      myBeamCrossSectionDefField->popDown();
    else
      myBeamCrossSectionDefField->popUp();
    myBeamLengthField->popUp();
    myBeamVisualize3DButton->popUp();
    myBeamVisualize3DFields[0]->popUp();
    myBeamVisualize3DFields[1]->popUp();
    myBeamOrientationFrame->popUp();
  }
  else if (pv->showUDEData)
  {
    myBeam->popUp();
    myBeamCrossSectionDefField->popDown();
    myBeamLengthField->popDown();
    myBeamVisualize3DButton->popDown();
    myBeamVisualize3DFields[0]->popDown();
    myBeamVisualize3DFields[1]->popDown();
    myBeamOrientationFrame->popDown();
  }
  else
    myBeam->popDown();

  // Shaft

  if (pv->showShaftData)
    myShaftCrossSectionDefField->popUp();
  else
    myShaftCrossSectionDefField->popDown();

  bool showBladeOrShaft = pv->showBladeData || pv->showShaftData;
  bool showBeamstring = pv->showJacketData || pv->showSoilPileData || pv->showRiserData;
  if (showBladeOrShaft || showBeamstring)
    myShaftDynProps->popUp();
  else
    myShaftDynProps->popDown();

  if (showBladeOrShaft)
    myShaftNote->popUp();
  else
    myShaftNote->popDown();

  // Turbine, Nacelle, Gearbox, etc. (shared fields)

  bool showTurbine = (pv->showTurbineData || pv->showTowerData || pv->showNacelleData ||
                      pv->showGeneratorData || pv->showGearboxData || pv->showRotorData);
  if (pv->showSubassemblyData || showTurbine || showBladeOrShaft || showBeamstring)
  {
    mySubassembly->popUp();
    if (!pv->showSubassemblyData || pv->showSubassPos)
      mySubassMassField->popUp();
    else
      mySubassMassField->popDown();
  }
  else
    mySubassembly->popDown();

  if ((pv->showSubassemblyData && pv->showSubassPos) || pv->showJacketData || showTurbine)
    mySubassCoGFrame->popUp();
  else
    mySubassCoGFrame->popDown();

  if (showBladeOrShaft || pv->showRiserData || pv->showSoilPileData)
    mySubassLengthField->popUp();
  else
    mySubassLengthField->popDown();

  if (showBladeOrShaft || showBeamstring || pv->showTowerData)
    myVisualize3DButton->popUp();
  else
    myVisualize3DButton->popDown();

  if (pv->showTurbineData)
  {
    myTurbineWindRefFrame->popUp();
    myTurbineAdvTopologyFrame->popUp();
  }
  else
  {
    myTurbineWindRefFrame->popDown();
    myTurbineAdvTopologyFrame->popDown();
  }

  if (pv->showGeneratorData)
    myGeneratorFrame->popUp();
  else
    myGeneratorFrame->popDown();

  if (pv->showBladeData)
  {
    myBladePitchControlFrame->popUp();
    myBladeIceFrame->popUp();
  }
  else
  {
    myBladePitchControlFrame->popDown();
    myBladeIceFrame->popDown();
  }

  // Riser, Soil Pile or Jacket :

  if (pv->showRiserData)
  {
    myRiserMudButton->setLabel("Internal Liquid");
    myRiserMudDensityField->setLabel("Mud density");

    myRiserInternalToDefField->popUp();
    myRiserVisualize3DStartAngleField->popUp();
    myRiserVisualize3DStopAngleField->popUp();
    myRiserMudFrame->popUp();
    myRiserMudLevelField->popUp();
  }
  else if (pv->showSoilPileData)
  {
    myRiserMudButton->setLabel("Internal Soil");
    myRiserMudDensityField->setLabel("Soil density");

    myRiserInternalToDefField->popDown();
    myRiserVisualize3DStartAngleField->popUp();
    myRiserVisualize3DStopAngleField->popUp();
    myRiserMudFrame->popUp();
    myRiserMudLevelField->popDown();
  }
  else
  {
    myRiserInternalToDefField->popDown();
    myRiserVisualize3DStartAngleField->popDown();
    myRiserVisualize3DStopAngleField->popDown();
    myRiserMudFrame->popDown();
  }

  // Swap Master and Slave :

  if (pv->showSwapTriadButton)
    mySwapTriadButton->popUp();
  else
    mySwapTriadButton->popDown();

  // Add Masters :

  if (pv->showAddMasterButton)
    myAddMasterButton->popUp();
  else
    myAddMasterButton->popDown();

  // Reverse Masters :

  if (pv->showReverseMasterButton)
    myRevMasterButton->popUp();
  else
    myRevMasterButton->popDown();

  // Cam joint data

  myJointSummary->showCamVars(pv->showCamData);

  // Screw data

  myJointSummary->showScrew(pv->showScrew);

  // Friction data

  myJointSummary->showFriction(pv->showFriction);
  myJointSummary->myFriction->setQuery(pv->myFrictionQuery);

  // TZ dof toggle

  myJointSummary->showTzToggle(pv->showDOF_TZ_Toggle);

  // Fix/Free all DOFs

  myJointSummary->showFixFreeAll(pv->showJointData == 2);

  // Joint rotation formulation

  myJointAdvancedTab->showRotFormulation(pv->showRotFormulation);
  myJointAdvancedTab->myRotFormulationMenu->setOptions(pv->myRotFormulationTypes);
  myJointAdvancedTab->myRotSequenceMenu->setOptions(pv->myRotSequenceTypes);

  // Joint spring coupling

  myJointAdvancedTab->showSpringCpl(pv->showSpringCpl);
  myJointAdvancedTab->myRotSpringCplMenu->setOptions(pv->mySpringCplTypes);
  myJointAdvancedTab->myTranSpringCplMenu->setOptions(pv->mySpringCplTypes);

  // Joints :

  if (pv->showJointData)
  {
    myJointSummary->mySummaryTable->setNumberRows(pv->myJointVals.size());
    myJointSummary->myAddBCLabel->popDown();
    myJointSummary->myDefDamperLabel->popDown();

    myJointPosition->setEditedObjs(pv->objsToPosition);

    myJointTabs->popDown();
    if (!pv->objsToPosition.empty()) {
      myJointTabs->addTabPage(myJointPosition, "Origin", NULL, 1);
      IAmShowingJointData = 2;
    }
    else
      myJointTabs->removeTabPage(myJointPosition);

    for (FuiJointDOF* jdof : myJointDofs)
      myJointTabs->removeTabPage(jdof);
    myJointTabs->removeTabPage(myJointAdvancedTab);
    myJointTabs->removeTabPage(myJointResults);

    size_t jv = 0;
    const char* label[] = { "Tx", "Ty", "Tz", "Rx", "Ry", "Rz" };
    for (FuiJointDOF* jdof : myJointDofs)
      if (jv < pv->myJointVals.size()) {
        int idof = pv->myJointVals[jv].myDofNo;
        myJointTabs->addTabPage(jdof, label[idof]);
        myJointSummary->mySummaryTable->setRowLabel(jv, label[idof]);
        jdof->springDC->buildDynamicWidgets(pv->myJointVals[jv].mySpringDCVals);
        jdof->springFS->buildDynamicWidgets(pv->myJointVals[jv].mySpringFSVals);
        jdof->damperFS->buildDynamicWidgets(pv->myJointVals[jv].myDamperFCVals);
        jdof->dofNo = pv->myJointVals[jv++].myDofNo;
      }
      else
        jdof->dofNo = -1;

    if (pv->showRotFormulation || pv->showSpringCpl)
      myJointTabs->addTabPage(myJointAdvancedTab, "Advanced");

    if (!pv->myJointVals.empty())
      myJointTabs->addTabPage(myJointResults, "Results");

    myJointSummary->showSwapJoint(pv->showJointData-2);

    myJointTabs->setCurrentTab(mySelectedJointTab);
    this->onJointTabSelected(0);
    myJointTabs->popUp();
  }
  else
  {
    if (myJointTabs->isPoppedUp())
      mySelectedJointTab = myJointTabs->getCurrentTabName();
    myJointTabs->popDown();
    myJointPosition->setEditedObjs();
  }

  // Pipe Surface

  if (pv->showPipeSurfaceData)
    myPipeRadiusField->popUp();
  else
    myPipeRadiusField->popDown();

  // Triad :

  if (pv->showTriadData)
  {
    myTriadSummary->mySummaryTable->setNumberRows(pv->myTriadVals.size());
    myTriadSummary->myAddBCLabel->popDown();

    myTriadPosition->setEditedObjs(pv->objsToPosition);

    if (pv->myTriadVals.empty()) {
      myTriadSummary->showMass(false);
      myTriadSummary->showSysDir(false);
      myTriadSummary->mySummaryTable->popDown();
    }
    else {
      myTriadSummary->showMass(pv->myTriadVals.size());
      myTriadSummary->showSysDir(true);
      myTriadSummary->mySummaryTable->popUp();
    }
    myTriadSummary->showConnector(pv->myTriadConnector > 1);

    myTriadTabs->popDown();
    const char* label[] = { "Tx", "Ty", "Tz", "Rx", "Ry", "Rz" };
    for (size_t i = 0; i < myTriadDofs.size(); i++)
      if (i < pv->myTriadVals.size()) {
        myTriadTabs->addTabPage(myTriadDofs[i], label[i], NULL, 2+i);
        myTriadSummary->mySummaryTable->setRowLabel(i, label[i]);
      }
      else
        myTriadTabs->removeTabPage(myTriadDofs[i]);

    if (!pv->myTriadVals.empty())
      myTriadTabs->addTabPage(myTriadResults, "Results");
    else
      myTriadTabs->removeTabPage(myTriadResults);

    myTriadTabs->setCurrentTab(mySelectedTriadTab);
    this->onTriadTabSelected(0);
    myTriadTabs->popUp();
  }
  else
  {
    if (myTriadTabs->isPoppedUp())
      this->onTriadTabSelected(0);
    myTriadTabs->popDown();
    myTriadPosition->setEditedObjs();
  }

  // Load

  if (pv->showLoadData)
    myLoad->popUp();
  else
    myLoad->popDown();

  // Generic DB object

  if (pv->showGenDBObj)
    myGenDBObject->popUp();
  else
    myGenDBObject->popDown();

  // File reference

  if (pv->showFileReference)
    myFileReferenceBrowseField->popUp();
  else
    myFileReferenceBrowseField->popDown();

  // Tire

  if (pv->showTireData)
  {
    myTire->popUp();
    myTireDataFileField->setQuery(pv->myTireDataFileRefQuery);
    myRoadField->setQuery(pv->myRoadQuery);
    myTireModelMenu->setOptions(pv->myTireTypes);
  }
  else
    myTire->popDown();

  // Road

  if (pv->showRoadData)
  {
    myRoad->popUp();
    myRoadFuncField->setQuery(pv->myRoadFunctionQuery);
    myRoadDataFileField->setQuery(pv->myRoadDataFileRefQuery);
  }
  else
    myRoad->popDown();

  // Material properties

  if (pv->showMatPropData)
    myMatPropFrame->popUp();
  else
    myMatPropFrame->popDown();

  // Sea state

  if (pv->showSeaStateData)
    mySeaState->popUp();
  else 
    mySeaState->popDown();

  // Beam properties

  if (pv->showBeamPropData)
  {
    myBeamPropTabs->setCurrentTab(mySelectedBeamPropTab);
    this->onBeamPropTabSelected(0);
    myBeamPropTabs->popUp();
  }
  else
    myBeamPropTabs->popDown();

  // Strain rosette

  if (pv->showStrainRosetteData)
  {
    myStrainRosette->popUp();
    myStrRosTypeMenu->setOptions(pv->myStrainRosetteTypes);
    myStrRosTypeMenu->setSensitivity(pv->myStrRosIsEditable);
    myStrRosEditNodesButton->setSensitivity(pv->myStrRosIsEditable);
    myStrRosAngleField->setSensitivity(pv->myStrRosIsEditable);
    myStrRosEditDirButton->setSensitivity(pv->myStrRosIsEditable);
    myStrRosUseFEHeightToggle->setSensitivity(pv->myStrRosIsEditable);
    myStrRosFlipZButton->setSensitivity(pv->myStrRosIsEditable);
    myStrRosUseFEMatToggle->setSensitivity(pv->myStrRosIsEditable);
    myResetStartStrainsToggle->setSensitivity(pv->myStrRosIsEditable);
  }
  else
    myStrainRosette->popDown();

  // Element group

  if (pv->showGroupData)
    myFatigueFrame->popUp();
  else
    myFatigueFrame->popDown();

  // RAO vessel motion

  if (pv->showRAOData)
  {
    myRAO->popUp();
    myRAOFileField->setQuery(pv->myRAOFileRefQuery);
    myWaveFuncField->setQuery(pv->myWaveFunctionQuery);
    myMotionScaleField->setQuery(pv->myMotionScaleQuery);
  }
  else
    myRAO->popDown();

  // Simulation event

  if (pv->showSimEventData)
    mySimEvent->popUp();
  else
    mySimEvent->popDown();

  // Subassembly

  if (pv->showSubassemblyData || pv->showRiserData ||
      pv->showJacketData || pv->showSoilPileData)
    mySubassFileField->popUp();
  else
    mySubassFileField->popDown();

  if (pv->showSubassPos)
  {
    mySubassPosition->setEditedObjs(pv->objsToPosition);
    mySubassPosition->popUp();
  }
  else
  {
    mySubassPosition->setEditedObjs();
    mySubassPosition->popDown();
  }

  if (pv->showFunctionData)
    myFunctionProperties->popUp();
  else
    myFunctionProperties->popDown();

  if (myCtrlElementProperties)
  {
    if (pv->showCtrlData)
      myCtrlElementProperties->popUp();
    else
      myCtrlElementProperties->popDown();
  }
  if (myExtCtrlSysProperties)
  {
    if (pv->showExtCtrlSysData)
      myExtCtrlSysProperties->popUp();
    else
      myExtCtrlSysProperties->popDown();
  }
  if (myAnimationDefine)
  {
    if (pv->showAnimationData)
      myAnimationDefine->popUp();
    else
      myAnimationDefine->popDown();
  }
  if (myGraphDefine)
  {
    if (pv->showGraphData)
      myGraphDefine->popUp();
    else
      myGraphDefine->popDown();
  }
  if (myCurveDefine)
  {
    if (pv->showCurveData)
      myCurveDefine->popUp();
    else
      myCurveDefine->popDown();
  }

  if (IAmShowingStartGuide > 0)
  {
    myHeading->popDown();
    myTopology->popDown();
    myProperty->popDown();
    myStartGuide->popUp();
    myStartGuide->placeWidgets(this->getWidth(),this->getHeight());
  }
  else
  {
    myStartGuide->popDown();
    myHeading->popUp();
    myTopology->popUp();
    myProperty->popUp();
  }
}


FFuaUIValues* FuiProperties::createValuesObject()
{
  return new FuaPropertiesValues();
}


///////////////////////////////////////////////
//
// setUIValues : Setting values into the User interface
//               From the values object
//
///////

void FuiProperties::setUIValues(const FFuaUIValues* values)
{
  const FuaPropertiesValues* pv = dynamic_cast<const FuaPropertiesValues*>(values);
  if (!pv)
  {
    std::cerr <<" *** FuiProperties::setUIValues: Wrong values type"<< std::endl;
    return;
  }

  // Heading

  myTypeField->setValue(pv->myType);
  myIdField->setValue(pv->myId);
  if (pv->showHeading)
  {
    myDescriptionField->setValue(pv->myDescription);
    myTagField->setValue(pv->myTag);
  }

  // Reference Plane

  if (pv->showRefPlane)
  {
    myRefPlaneHeightField->setValue(pv->myRefPlaneHeight);
    myRefPlaneWidthField->setValue(pv->myRefPlaneWidth);
  }

  // Higher Pair

  if (pv->showHPRatio)
    myHPRatioField->setValue(pv->myHPRatio);

  // Axial Spring :

  if (pv->showAxialSpring)
  {
    mySpringForce->setValues(pv->myAxialSprForceValues);
    mySpringDeflCalc->setValues(pv->myAxialSpringDefCalcValues);
  }

  // Spring characteristics

  if (pv->showSpringChar)
    mySpringChar->setValues(pv->mySpringCharValues);

  // Axial Damper :

  if (pv->showAxialDamper)
    myDamperForce->setValues(pv->myAxialDaForceValues);

  // Part :

  if (pv->showLinkData)
  {
    myLinkTabs->popDown();
    std::string tmpSel = myLinkTabs->getCurrentTabName();
    myLinkTabs->removeTabPage(myLinkFEnodeSheet);
    myLinkTabs->removeTabPage(myLinkRedOptSheet);
    myLinkTabs->removeTabPage(myLinkLoadSheet);
    myLinkTabs->removeTabPage(myGenericPartCGSheet);
    myLinkTabs->removeTabPage(myGenericPartMassSheet);
    myLinkTabs->removeTabPage(myGenericPartStiffSheet);
    myLinkTabs->removeTabPage(myHydrodynamicsSheet);
    myLinkTabs->removeTabPage(myMeshingSheet);
    myLinkTabs->removeTabPage(myAdvancedLinkOptsSheet);
    myLinkTabs->removeTabPage(myNonlinearLinkOptsSheet);

    if (!pv->myLinkValues.suppressInSolver) {
      if (pv->myLinkValues.useGenericPart) {
	myLinkTabs->addTabPage(myGenericPartMassSheet, "Mass");
	myLinkTabs->addTabPage(myGenericPartStiffSheet, "Stiffness");
	myLinkTabs->addTabPage(myGenericPartCGSheet, "CoG");
	myLinkTabs->addTabPage(myHydrodynamicsSheet, "Hydrodynamics");
	myGenericPartCGSheet->popUp();
	myGenericPartStiffSheet->popUp();
	myGenericPartMassSheet->popUp();
	myHydrodynamicsSheet->popUp();
      }
      else {
	myLinkTabs->addTabPage(myLinkFEnodeSheet, "FE Node");
	myLinkTabs->addTabPage(myLinkRedOptSheet, "Reduction Options");
	myLinkFEnodeSheet->popUp();
	myLinkRedOptSheet->popUp();
	if (pv->myLinkValues.loadCases.size() > 0) {
	  myLinkTabs->addTabPage(myLinkLoadSheet, "Reduced Loads");
	  myLinkLoadSheet->popUp();
	}
	if (FapLicenseManager::checkLicense("FA-NLR")) {
	  myLinkTabs->addTabPage(myNonlinearLinkOptsSheet, "Nonlinear");
	  myNonlinearLinkOptsSheet->popUp();
	}
      }
      if (pv->myLinkValues.enableMeshing) {
	myLinkTabs->addTabPage(myMeshingSheet, "Meshing");
	myMeshingSheet->popUp();
      }
      myLinkTabs->addTabPage(myAdvancedLinkOptsSheet, "Advanced");
      myAdvancedLinkOptsSheet->popUp();
    }

    myLinkTabs->popUp();
    myLinkTabs->setCurrentTab(tmpSel);
    this->onLinkTabSelected(0);
    myLinkModelSheet->setValues(pv->myLinkValues);
    myLinkOriginSheet->setEditedObjs(pv->objsToPosition);
    myLinkFEnodeSheet->setViewedObj(dynamic_cast<FmIsPositionedBase*>(pv->objsToPosition.front()));
    myLinkFEnodeSheet->setValues(pv->myLinkValues);
    myLinkRedOptSheet->setValues(pv->myLinkValues);
    myLinkLoadSheet->setValues(pv->myLinkValues);
    myGenericPartCGSheet->setEditedObjs(pv->objsToPosition);
    myGenericPartCGSheet->setValues(pv->myLinkValues);
    myGenericPartMassSheet->setValues(pv->myLinkValues);
    myGenericPartStiffSheet->setValues(pv->myLinkValues);
    myHydrodynamicsSheet->setValues(pv->myLinkValues);
    myMeshingSheet->setValues(pv->myLinkValues);
    myAdvancedLinkOptsSheet->setValues(pv->myLinkValues);
    myNonlinearLinkOptsSheet->setValues(pv->myLinkValues);
  }

  // Beam and User-defined element :

  if (pv->showBeamData)
  {
    myBeamCrossSectionDefField->setQuery(pv->myCrossSectionQuery);
    myBeamCrossSectionDefField->setSelectedRef(pv->mySelectedCS);
    myBeamVisualize3DButton->setValue(pv->myVisualize3D);
    myBeamVisualize3DFields[0]->myField->setValue(pv->myVisualize3DAngles.first);
    myBeamVisualize3DFields[1]->myField->setValue(pv->myVisualize3DAngles.second);
    for (int i = 0; i < 3; i++)
      myBeamLocalZField[i]->setValue(pv->myOrientation[i]);
  }
  if (pv->showBeamData || pv->showUDEData)
  {
    myBeamMassField->setValue(pv->myTotalMass);
    myBeamLengthField->setValue(pv->myTotalLength);
    myBeamDynProps->setValues(pv->myMassPropDamp,pv->myStifPropDamp,
                              pv->myScaleStiff,pv->myScaleMass);
  }

  // Shaft :

  if (pv->showShaftData)
  {
    mySubassLengthField->setValue(pv->myTotalLength);
    mySubassMassField->setValue(pv->myTotalMass);
    if (pv->myBlankFieldsFlags & 0x0001) {
      myShaftCrossSectionDefField->setQuery(pv->myCrossSectionQuery);
      myShaftCrossSectionDefField->setSelectedRef(NULL);
    }
    else {
      myShaftCrossSectionDefField->setQuery(pv->myCrossSectionQuery);
      myShaftCrossSectionDefField->setSelectedRef(pv->mySelectedCS);
    }
    myShaftDynProps->setValues(pv->myMassPropDamp, pv->myStifPropDamp,
                               pv->myScaleStiff, pv->myScaleMass,
                               pv->myBlankFieldsFlags);
  }

  if (pv->showShaftData || pv->showBladeData || pv->showTowerData) {
    myVisualize3DButton->useUITristate(true);
    myVisualize3DButton->setUITristateValue(pv->myVisualize3Dts);
    myVisualize3DButton->setSensitivity(pv->myVisualize3DEnabled);
  }

  // Lambda function returning the (first) sub-assembly among pv->objsToPosition.
  auto&& selectedSubAss = [pv]()
  {
    FmAssemblyBase* subass = NULL;
    for (FmModelMemberBase* obj : pv->objsToPosition)
      if ((subass = dynamic_cast<FmAssemblyBase*>(obj))) break;
    return subass;
  };

  // Turbine :

  if (pv->showTurbineData)
  {
    mySelectedSubass = selectedSubAss();
    mySubassMassField->setValue(pv->myTotalMass);
    mySubassCoGField->setValue(pv->myCoG);
    mySubassCoGField->setGlobal();

    myTurbineFields[0]->setQuery(pv->myTurbineWindRefTriadDefQuery);
    myTurbineFields[0]->setSelectedRef(pv->myTurbineWindRefTriadDefSelected);
    myTurbineWindVertOffsetField->setValue(pv->myTurbineWindVertOffset);
    myTurbineFields[1]->setQuery(pv->myTurbineYawPointTriadDefQuery);
    myTurbineFields[1]->setSelectedRef(pv->myTurbineYawPointTriadDefSelected);
    myTurbineFields[2]->setQuery(pv->myTurbineHubApexTriadDefQuery);
    myTurbineFields[2]->setSelectedRef(pv->myTurbineHubApexTriadDefSelected);
    myTurbineFields[3]->setQuery(pv->myTurbineHubPartDefQuery);
    myTurbineFields[3]->setSelectedRef(pv->myTurbineHubPartDefSelected);
    myTurbineFields[4]->setQuery(pv->myTurbineFirstBearingDefQuery);
    myTurbineFields[4]->setSelectedRef(pv->myTurbineFirstBearingDefSelected);
  }
  else
    mySelectedSubass = NULL;

  // Tower, Nacelle, Gearbox or Rotor :

  if (pv->showTowerData || pv->showNacelleData || pv->showGearboxData || pv->showRotorData)
  {
    mySelectedSubass = selectedSubAss();
    mySubassMassField->setValue(pv->myTotalMass);
    mySubassCoGField->setValue(pv->myCoG);
    mySubassCoGField->setGlobal();
  }

  // Generator :

  if (pv->showGeneratorData)
  {
    mySelectedSubass = selectedSubAss();
    mySubassMassField->setValue(pv->myTotalMass);
    mySubassCoGField->setValue(pv->myCoG);
    mySubassCoGField->setGlobal();

    if (pv->myGeneratorTorqueControl) {
      myGeneratorTorqueRadioBtn->setValue(true);
      myGeneratorTorqueField->setSensitivity(IAmSensitive);
      myGeneratorVelocityField->setSensitivity(false);
    }
    else {
      myGeneratorVelocityRadioBtn->setValue(true);
      myGeneratorTorqueField->setSensitivity(false);
      myGeneratorVelocityField->setSensitivity(IAmSensitive);
    }

    myGeneratorTorqueField->setValue(pv->myGeneratorTorqueControlConstValue);
    myGeneratorTorqueField->setQuery(pv->myGeneratorTorqueControlEngineQuery);
    myGeneratorTorqueField->setSelectedRef(pv->myGeneratorTorqueControlSelectedEngine);
    myGeneratorTorqueField->setButtonCB(pv->myEditButtonCB);

    myGeneratorVelocityField->setValue(pv->myGeneratorVelocityControlConstValue);
    myGeneratorVelocityField->setQuery(pv->myGeneratorVelocityControlEngineQuery);
    myGeneratorVelocityField->setSelectedRef(pv->myGeneratorVelocityControlSelectedEngine);
    myGeneratorVelocityField->setButtonCB(pv->myEditButtonCB);
  }

  // Blade :

  if (pv->showBladeData)
  {
    mySubassMassField->setValue(pv->myTotalMass);
    mySubassLengthField->setValue(pv->myTotalLength);

    myBladeIceLayerToggle->setValue(pv->myBladeIceLayer);
    myBladeIceThicknessField->setValue(pv->myBladeIceThickness);
    myBladeIceThicknessField->setSensitivity(pv->myBladeIceLayer);
    myBladeFixedPitchToggle->setValue(pv->myBladePitchIsFixed);
    myBladePitchControlField->setSensitivity(IAmSensitive && !pv->myBladePitchIsFixed);

    myBladePitchControlField->setValue(pv->myBladePitchControlConstValue);
    myBladePitchControlField->setQuery(pv->myBladePitchControlEngineQuery);
    myBladePitchControlField->setSelectedRef(pv->myBladePitchControlSelectedEngine);
    myBladePitchControlField->setButtonCB(pv->myEditButtonCB);

    myShaftDynProps->setValues(pv->myMassPropDamp, pv->myStifPropDamp,
                               pv->myScaleStiff, pv->myScaleMass,
                               pv->myBlankFieldsFlags);
  }

  // Riser, Jacket or Soil Pile :

  if (pv->showRiserData || pv->showJacketData || pv->showSoilPileData)
  {
    mySelectedSubass = selectedSubAss();
    mySubassMassField->setValue(pv->myTotalMass);
    if (pv->showJacketData) {
      mySubassCoGField->setValue(pv->myCoG);
      mySubassCoGField->setGlobal();
    }
    else
      mySubassLengthField->setValue(pv->myTotalLength);
    myVisualize3DButton->setUITristateValue(pv->myVisualize3Dts);
    myRiserVisualize3DStartAngleField->myField->setValue(pv->myVisualize3DAngles.first);
    myRiserVisualize3DStopAngleField->myField->setValue(pv->myVisualize3DAngles.second);
    myRiserMudButton->setValue(pv->myIntFluid);
    myRiserMudDensityField->setValue(pv->myIntFluidDensity);
    myRiserMudDensityField->setSensitivity(IAmSensitive && pv->myIntFluid);
    myRiserMudLevelField->setValue(pv->myIntFluidLevel);
    myRiserMudLevelField->setSensitivity(IAmSensitive && pv->myIntFluid);

    myShaftDynProps->setValues(pv->myMassPropDamp, pv->myStifPropDamp,
                               pv->myScaleStiff, pv->myScaleMass,
                               pv->myBlankFieldsFlags);
  }

  // Joints :

  if (pv->showJointData)
  {
    this->buildDynamicWidgets(values);

    for (size_t jv = 0; jv < pv->myJointVals.size(); jv++)
    {
      if (!pv->showCamData)
	myJointDofs[jv]->motionType->setSensitivity(IAmSensitive);
      else if (jv == 0) // local X-dof in Cam joints can only be spring-damper constrained
	myJointDofs[jv]->motionType->setSensitivity(false); // disable all toggles
      else
      {
	// Cam joint dofs can only be free or spring-damper constrained
	myJointDofs[jv]->motionType->setSensitivity(false);
	myJointDofs[jv]->motionType->setSensitivity(0,IAmSensitive);
	myJointDofs[jv]->motionType->setSensitivity(3,IAmSensitive);
      }

      myJointDofs[jv]->setValues(pv->myJointVals[jv],IAmSensitive);

      if (pv->showCamData) // Cam joint dofs can not have loads
	myJointDofs[jv]->simpleLoad->popDown();

      myJointSummary->setSummary(jv,pv->myJointVals[jv]);
    }

    myJointSummary->updateTableGeometry();
    myJointResults->setValues(pv->myResToggles);
  }

  // Cam Data

  if (pv->showCamData)
  {
    myJointSummary->myCamThicknessField->setValue(pv->myCamThickness);
    myJointSummary->myCamWidthField->setValue(pv->myCamWidth);
    myJointSummary->myRadialToggle->setValue(pv->IAmRadialContact);
  }

  // Pipe Surface

  if (pv->showPipeSurfaceData)
    myPipeRadiusField->setValue(pv->pipeSurfaceRadius);

  // Screw Data

  if (pv->showScrew)
  {
    myJointSummary->myScrewToggle->setValue(pv->myIsScrewConnection);
    myJointSummary->myScrewRatioField->setValue(pv->myScrewRatio);
    myJointSummary->myScrewRatioField->setSensitivity(IAmSensitive && pv->myIsScrewConnection);
  }

  // Friction Data

  if (pv->showFriction > 0)
    myJointSummary->myFriction->setSelectedRef(pv->mySelectedFriction);
  if (pv->showFriction > 1)
    myJointSummary->myFrictionDof->selectOption(pv->myFrictionDof);

  // TZ dof Toggle

  if (pv->showDOF_TZ_Toggle)
    myJointSummary->myDOF_TZ_Toggle->setValue(pv->myIsDOF_TZ_legal);

  // Rotation Formulation

  if (pv->showRotFormulation)
  {
    myJointAdvancedTab->myRotFormulationMenu->selectOption(pv->mySelectedRotFormulation);
    myJointAdvancedTab->myRotSequenceMenu->selectOption(pv->mySelectedRotSequence);
    if (pv->mySelectedRotFormulation == FmJointBase::ROT_AXIS)
      myJointAdvancedTab->myRotSequenceMenu->setSensitivity(false);
    else
      myJointAdvancedTab->myRotSequenceMenu->setSensitivity(IAmSensitive);
    myJointAdvancedTab->setRotExplain(FmJointBase::getRotExplain(pv->mySelectedRotFormulation,pv->mySelectedRotSequence));
  }

  // Spring Coupling

  if (pv->showSpringCpl)
    myJointAdvancedTab->myRotSpringCplMenu->selectOption(pv->mySelectedRotSpringCpl);
  if (pv->showSpringCpl == 1)
    myJointAdvancedTab->myTranSpringCplMenu->selectOption(pv->mySelectedTranSpringCpl);

  // Triad

  if (pv->showTriadData)
  {
    this->buildDynamicWidgets(values);

    myTriadSummary->myFENodeField->myField->setValue(pv->myFENodeIdx);
    myTriadSummary->myMassField->setValue(pv->myMass[0]);
    myTriadSummary->myIxField->setValue(pv->myMass[1]);
    myTriadSummary->myIyField->setValue(pv->myMass[2]);
    myTriadSummary->myIzField->setValue(pv->myMass[3]);

    myTriadSummary->mySysDirMenu->selectOption(pv->mySysDir);
    if (pv->myTriadConnector > 1)
      myTriadSummary->myConnectorMenu->selectOption(pv->myTriadConnector-2);

    if (pv->isSlave)
      myTriadSummary->myTriadLabel->setLabel("This is a slave triad");
    else if (pv->isMaster)
    {
      if (pv->myTriadVals.empty())
        myTriadSummary->myTriadLabel->setLabel("This is a grounded master triad");
      else
        myTriadSummary->myTriadLabel->setLabel("This is a master triad");
    }
    else if (pv->myTriadVals.empty())
      myTriadSummary->myTriadLabel->setLabel("This triad is grounded");
    else
      myTriadSummary->myTriadLabel->setLabel("");

    if (pv->myTriadVals.empty() || pv->isSlave)
    {
      myTriadSummary->mySetAllFixedButton->popDown();
      myTriadSummary->mySetAllFreeButton->popDown();
    }
    else
    {
      myTriadSummary->mySetAllFixedButton->popUp();
      myTriadSummary->mySetAllFreeButton->popUp();
    }

    for (size_t i = 0; i < pv->myTriadVals.size(); i++)
    {
      myTriadDofs[i]->setValues(pv->myTriadVals[i],!pv->isSlave,IAmSensitive);
      myTriadSummary->setSummary(i,pv->myTriadVals[i]);
    }

    myTriadSummary->updateTableGeometry();
    myTriadResults->setValues(pv->myResToggles);
  }

  // Load

  if (pv->showLoadData)
  {
    myLoadMagnitude->setValue(pv->myLoadMagnitude);
    myLoadMagnitude->setQuery(pv->myLoadEngineQuery);
    myLoadMagnitude->setSelectedRef(pv->mySelectedLoadMagnitudeEngine);
    myAttackPointEditor->setValue(pv->myAttackPoint);
    myAttackPointEditor->setGlobal(pv->myAttackPointIsGlobal);
    myAttackPointEditor->setOnWhatText(pv->myAttackObjectText);

    myFromPointEditor->setValue(pv->myFromPoint);
    myFromPointEditor->setGlobal(pv->myFromPointIsGlobal);
    myFromPointEditor->setOnWhatText(pv->myFromPointObjectText);

    myToPointEditor->setValue(pv->myToPoint);
    myToPointEditor->setGlobal(pv->myToPointIsGlobal);
    myToPointEditor->setOnWhatText(pv->myToPointObjectText);
  }

  // Generic DB Object

  if (pv->showGenDBObj)
  {
    myGenDBObjTypeField->setValue(pv->myGenDBObjType);
    myGenDBObjDefField->setAllText(pv->myGenDBObjDef.c_str());
  }

  // File reference

  if (pv->showFileReference)
  {
    myFileReferenceBrowseField->setAbsToRelPath(pv->myModelFilePath);
    myFileReferenceBrowseField->setFileName(pv->myFileReferenceName);
  }

  // Tire

  if (pv->showTireData)
  {
    myTireDataFileField->setSelectedRef(pv->mySelectedTireDataFileRef);
    myTireDataFileField->setText(pv->myTireDataFileName);
    myRoadField->setSelectedRef(pv->mySelectedRoad);
    myTireModelMenu->selectOption(pv->mySelectedTireModelType,false);
    mySpindelOffset->setValue(pv->mySpindelTriadOffset);

    // Set read-only if no tire license available
    if (!FapLicenseManager::checkTireLicense()) {
      myTireDataFileField->setSensitivity(false);
      myBrowseTireFileButton->setSensitivity(false);
      myRoadField->setSensitivity(false);
      myTireModelMenu->setSensitivity(false);
      mySpindelOffset->setSensitivity(false);
    }
  }

  // Road

  if (pv->showRoadData)
  {
    myUseFuncRoadRadio->setValue(!pv->iAmUsingExtRoadData);
    myRoadFuncField->setSelectedRef(pv->mySelectedRoadFunc);
    myRoadFields[0]->setValue(pv->myRoadZRotation);
    myRoadFields[1]->setValue(pv->myRoadZShift);
    myRoadFields[2]->setValue(pv->myRoadXOffset);

    myUseFileRoadRadio->setValue(pv->iAmUsingExtRoadData);
    myRoadDataFileField->setSelectedRef(pv->mySelectedRoadDataFileRef);
    myRoadDataFileField->setText(pv->myRoadDataFileName);

    // Set read-only if no tire license available
    if (!FapLicenseManager::checkTireLicense()) {
      myUseFuncRoadRadio->setSensitivity(false);
      myRoadFuncField->setSensitivity(false);
      for (FFuIOField* fld : myRoadFields)
	fld->setSensitivity(false);
      myUseFileRoadRadio->setSensitivity(false);
      myRoadDataFileField->setSensitivity(false);
      myBrowseRoadFileButton->setSensitivity(false);
    }
    else if (myUseFuncRoadRadio->getSensitivity()) {
      myRoadFuncField->setSensitivity(!pv->iAmUsingExtRoadData);
      for (FFuIOField* fld : myRoadFields)
	fld->setSensitivity(!pv->iAmUsingExtRoadData);
      myRoadDataFileField->setSensitivity(pv->iAmUsingExtRoadData);
      myBrowseRoadFileButton->setSensitivity(pv->iAmUsingExtRoadData);
    }
  }

  // Material properties

  if (pv->showMatPropData)
  {
    myMatPropRhoField->setValue(pv->myMatPropRho);
    myMatPropEField->setValue(pv->myMatPropE);
    myMatPropNuField->setValue(pv->myMatPropNu);
    myMatPropGField->setValue(pv->myMatPropG);
  }

  // Beam properties

  if (pv->showBeamPropData)
  {
    myBeamPropSummary->myCrossSectionTypeMenu->selectOption(pv->myBeamCrossSectionType);
    myBeamPropSummary->onCrossSectionTypeChanged(pv->myBeamCrossSectionType);
    myBeamPropSummary->myMaterialDefField->setQuery(pv->myMaterialQuery);
    myBeamPropSummary->myMaterialDefField->setSelectedRef(pv->mySelectedMaterial);
    myBeamPropSummary->myDependencyButton->setValue(pv->myBeamBreakDependence);
    myBeamPropSummary->onBreakDependencyToggled(IAmSensitive && pv->myBeamBreakDependence);
    myBeamPropSummary->setValues(pv->myBeamProp);
    myBeamPropHydro->onBeamHydroToggled(IAmSensitive && pv->myBeamHydroToggle);
    myBeamPropHydro->setValues(pv->myBeamHydroToggle,pv->myHydroProp);
  }

  // Strain rosette

  if (pv->showStrainRosetteData)
  {
    myStrRosTypeMenu->selectOption(pv->mySelectedRosetteType,false);

    std::string nodeIdString;
    if (!pv->myStrRosNodes.empty())
      nodeIdString = FFaNumStr(pv->myStrRosNodes.front());
    for (size_t i = 1; i < pv->myStrRosNodes.size(); i++)
      nodeIdString += FFaNumStr(", %d",pv->myStrRosNodes[i]);

    myStrRosNodesField->setValue(nodeIdString);
    myStrRosAngleField->setValue(pv->myStrRosAngle);
    myStrRosHeightField->setValue(pv->myStrRosHeight);
    myStrRosUseFEHeightToggle->setValue(pv->IAmUsingFEHeight);
    myStrRosEmodField->setValue(pv->myStrRosEMod);
    myStrRosNuField->setValue(pv->myStrRosNu);
    myStrRosUseFEMatToggle->setValue(pv->IAmUsingFEMaterial);
    myResetStartStrainsToggle->setValue(pv->IAmResettingStartStrains);

    myStrRosHeightField->setSensitivity(!pv->IAmUsingFEHeight && myStrRosUseFEHeightToggle->getSensitivity());
    myStrRosEmodField->setSensitivity(!pv->IAmUsingFEMaterial && myStrRosUseFEHeightToggle->getSensitivity());
    myStrRosNuField->setSensitivity(!pv->IAmUsingFEMaterial && myStrRosUseFEHeightToggle->getSensitivity());
  }

  // Element group

  if (pv->showGroupData)
  {
    myFatigueToggle->setValue(pv->doFatigue);
#ifdef FT_HAS_GRAPHVIEW
    mySNSelector->setValues(pv->mySNStd,pv->mySNCurve);
    mySNSelector->setSensitivity(pv->doFatigue);
#endif
    myScfField->setValue(pv->mySCF);
    myScfField->setSensitivity(pv->doFatigue);
  }

  // RAO vessel motion

  if (pv->showRAOData)
  {
    myRAOFileField->setSelectedRef(pv->mySelectedRAOFileRef);
    myRAOFileField->setText(pv->myRAOFileName);
    myWaveFuncField->setSelectedRef(pv->mySelectedWaveFunc);
    myWaveDirMenu->setOptions(pv->myWaveDirections);
    myWaveDirMenu->selectIntOption(pv->mySelectedWaveDir);
    myMotionScaleField->setSelectedRef(pv->mySelectedScale);
  }

  // Sea state

  if (pv->showSeaStateData)
  {
    mySeaStateWidthField->setValue(pv->mySeaStateWidth);
    mySeaStateHeightField->setValue(pv->mySeaStateHeight);
    mySeaStateWidthPosField->setValue(pv->mySeaStateWidthPos);
    mySeaStateHeightPosField->setValue(pv->mySeaStateHeightPos);
    mySeaStateNumPoints->setValue(pv->mySeaStateNumPoints);
    mySeaStateShowGridToggle->setValue(pv->mySeaStateShowGrid);
    mySeaStateShowSolidToggle->setValue(pv->mySeaStateShowSolid);
  }

  // Simulation event

  if (pv->showSimEventData)
  {
    myEventProbability->setValue(pv->mySimEventProbability);
    mySelectEventButton->setSensitivity(pv->allowSimEventChange);
    if (pv->showActiveEvent) {
      mySelectEventButton->setLabel("Revert to master event");
      myActiveEventLabel->popUp();
    }
    else {
      mySelectEventButton->setLabel("Set as active event");
      myActiveEventLabel->popDown();
    }
  }

  // Subassembly

  if (pv->showSubassemblyData || pv->showRiserData ||
      pv->showJacketData || pv->showSoilPileData)
  {
    mySubassFileField->setAbsToRelPath(pv->myModelFilePath);
    mySubassFileField->setFileName(pv->mySubAssemblyFile);
  }
  if (pv->showSubassemblyData && pv->showSubassPos)
  {
    mySelectedSubass = selectedSubAss();
    mySubassMassField->setValue(pv->myTotalMass);
    mySubassCoGField->setValue(pv->myCoG);
    mySubassCoGField->setGlobal();
  }

  this->placeWidgets(this->getWidth(), this->getHeight());
}


////////////////////////////////////////////////////////////////////////////////
//
// GetUIValues : Getting values from the User interface
//               and put them into the values object
//
///////

void FuiProperties::getUIValues(FFuaUIValues* values)
{
  FuaPropertiesValues* pv = dynamic_cast<FuaPropertiesValues*>(values);
  if (!pv) return;

  // Heading

  pv->showHeading = IAmShowingHeading;

  if (IAmShowingHeading)
  {
    pv->myDescription = myDescriptionField->getText();
    pv->myTag = myTagField->getText();
  }

  // Reference Plane

  if (IAmShowingRefPlane)
  {
    pv->myRefPlaneHeight = myRefPlaneHeightField->getDouble();
    pv->myRefPlaneWidth  = myRefPlaneWidthField->getDouble();
  }

  // Higher Pair

  if (IAmShowingHPRatio)
    pv->myHPRatio = myHPRatioField->getDouble();

  // Axial Spring :

  if (IAmShowingAxialSpring)
  {
    mySpringForce->getValues(pv->myAxialSprForceValues);
    mySpringDeflCalc->getValues(pv->myAxialSpringDefCalcValues);
  }

  // Spring Characteristics:

  if (IAmShowingSpringChar)
    mySpringChar->getValues(pv->mySpringCharValues);

  // Axial Damper :

  if (IAmShowingAxialDamper)
    myDamperForce->getValues(pv->myAxialDaForceValues);

  // Part :

  if (IAmShowingLinkData)
  {
    pv->selectedTab = mySelectedTabIndex;
    myLinkModelSheet->getValues(pv->myLinkValues);
    myLinkRedOptSheet->getValues(pv->myLinkValues);
    myLinkLoadSheet->getValues(pv->myLinkValues);
    myGenericPartCGSheet->getValues(pv->myLinkValues);
    myGenericPartMassSheet->getValues(pv->myLinkValues);
    myGenericPartStiffSheet->getValues(pv->myLinkValues);
    myHydrodynamicsSheet->getValues(pv->myLinkValues);
    myMeshingSheet->getValues(pv->myLinkValues);
    myAdvancedLinkOptsSheet->getValues(pv->myLinkValues);
    myNonlinearLinkOptsSheet->getValues(pv->myLinkValues);
  }

  // Beam and User-defined element :

  if (IAmShowingBeamData)
  {
    pv->mySelectedCS  = myBeamCrossSectionDefField->getSelectedRef();
    pv->myVisualize3D = myBeamVisualize3DButton->getValue();
    pv->myVisualize3DAngles.first  = myBeamVisualize3DFields[0]->myField->getIntFromText();
    pv->myVisualize3DAngles.second = myBeamVisualize3DFields[1]->myField->getIntFromText();
    for (int i = 0; i < 3; i++)
      pv->myOrientation[i] = myBeamLocalZField[i]->getValue();
  }
  if (IAmShowingBeamData || IAmShowingUDEData)
    myBeamDynProps->getValues(pv->myMassPropDamp,pv->myStifPropDamp,
                              pv->myScaleStiff,pv->myScaleMass);

  // Shaft :

  if (IAmShowingShaftData)
  {
    pv->mySelectedCS = myShaftCrossSectionDefField->getSelectedRef();
    pv->myBlankFieldsFlags = pv->mySelectedCS ? 0x0001 : 0x0000;
    pv->myBlankFieldsFlags |= myShaftDynProps->getValues(pv->myMassPropDamp,
                                                         pv->myStifPropDamp,
                                                         pv->myScaleStiff,
                                                         pv->myScaleMass);
  }

  if (IAmShowingShaftData || IAmShowingBladeData || IAmShowingTowerData)
    pv->myVisualize3Dts = myVisualize3DButton->getTristateValue();

  // Turbine :

  if (IAmShowingTurbineData)
  {
    pv->myTurbineWindRefTriadDefSelected  = myTurbineFields[0]->getSelectedRef();
    pv->myTurbineWindVertOffset           = myTurbineWindVertOffsetField->getDouble();
    pv->myTurbineYawPointTriadDefSelected = myTurbineFields[1]->getSelectedRef();
    pv->myTurbineHubApexTriadDefSelected  = myTurbineFields[2]->getSelectedRef();
    pv->myTurbineHubPartDefSelected       = myTurbineFields[3]->getSelectedRef();
    pv->myTurbineFirstBearingDefSelected  = myTurbineFields[4]->getSelectedRef();
  }

  // Generator :

  if (IAmShowingGeneratorData)
  {
    pv->myGeneratorTorqueControl = myGeneratorTorqueRadioBtn->getValue();
    pv->myGeneratorTorqueControlIsConstant = myGeneratorTorqueField->isAConstant();
    pv->myGeneratorTorqueControlConstValue = myGeneratorTorqueField->getValue();
    pv->myGeneratorTorqueControlSelectedEngine = myGeneratorTorqueField->getSelectedRef();
    pv->myGeneratorVelocityControlIsConstant = myGeneratorVelocityField->isAConstant();
    pv->myGeneratorVelocityControlConstValue = myGeneratorVelocityField->getValue();
    pv->myGeneratorVelocityControlSelectedEngine = myGeneratorVelocityField->getSelectedRef();
  }

  // Blade :

  if (IAmShowingBladeData)
  {
    pv->myBladeIceLayer = myBladeIceLayerToggle->getValue();
    pv->myBladeIceThickness = myBladeIceThicknessField->getValue();
    pv->myBladePitchIsFixed = myBladeFixedPitchToggle->getValue();
    pv->myBladePitchControlIsConstant = myBladePitchControlField->isAConstant();
    pv->myBladePitchControlConstValue = myBladePitchControlField->getValue();
    pv->myBladePitchControlSelectedEngine = myBladePitchControlField->getSelectedRef();

    pv->myBlankFieldsFlags = myShaftDynProps->getValues(pv->myMassPropDamp,
                                                        pv->myStifPropDamp,
                                                        pv->myScaleStiff,
                                                        pv->myScaleMass);
  }

  // Riser, Jacket and Soil Pile :

  if (IAmShowingRiserData || IAmShowingJacketData || IAmShowingSoilPileData)
  {
    pv->myVisualize3Dts = myVisualize3DButton->getTristateValue();
    pv->myVisualize3DAngles.first = myRiserVisualize3DStartAngleField->myField->getIntFromText();
    pv->myVisualize3DAngles.second = myRiserVisualize3DStopAngleField->myField->getIntFromText();

    pv->myIntFluid = myRiserMudButton->getValue();
    pv->myIntFluidDensity = myRiserMudDensityField->getValue();
    pv->myIntFluidLevel = myRiserMudLevelField->getValue();

    pv->myBlankFieldsFlags = myShaftDynProps->getValues(pv->myMassPropDamp,
                                                        pv->myStifPropDamp,
                                                        pv->myScaleStiff,
                                                        pv->myScaleMass);
  }

  // Joints :

  if (IAmShowingJointData)
    {
      pv->selectedTab = mySelectedTabIndex;

      pv->myJointVals.reserve(6);
      for (FuiJointDOF* jdof : myJointDofs)
        if (jdof->dofNo >= 0)
        {
          FuiJointDOFValues jv(jdof->dofNo);
          jdof->getValues(jv);
          pv->myJointVals.push_back(jv);
        }

      myJointResults->getValues(pv->myResToggles);
    }

  // Cam Data

  if (IAmShowingCamData)
    {
      pv->myCamThickness   = myJointSummary->myCamThicknessField->getDouble();
      pv->myCamWidth       = myJointSummary->myCamWidthField->getDouble();
      pv->IAmRadialContact = myJointSummary->myRadialToggle->getValue();
    }

  // Pipe Surface

  if (IAmShowingPipeSurfaceData)
    pv->pipeSurfaceRadius = myPipeRadiusField->getValue();

  // Screw

  if (IAmShowingScrew)
    {
      pv->myIsScrewConnection = myJointSummary->myScrewToggle->getValue();
      pv->myScrewRatio = myJointSummary->myScrewRatioField->getDouble();
    }

  // Friction

  if (IAmShowingFriction > 0)
    pv->mySelectedFriction = myJointSummary->myFriction->getSelectedRef();
  if (IAmShowingFriction > 1)
    pv->myFrictionDof = myJointSummary->myFrictionDof->getSelectedOption();

  if (IAmShowingDOF_TZ_Toggle)
    pv->myIsDOF_TZ_legal = myJointSummary->myDOF_TZ_Toggle->getValue();

  // Rotation formulation

  if (IAmShowingRotFormulation)
    {
      pv->mySelectedRotFormulation = myJointAdvancedTab->myRotFormulationMenu->getSelectedOption();
      pv->mySelectedRotSequence    = myJointAdvancedTab->myRotSequenceMenu->getSelectedOption();
    }

  // Spring coupling

  if (IAmShowingRotSpringCpl)
    pv->mySelectedRotSpringCpl = myJointAdvancedTab->myRotSpringCplMenu->getSelectedOption();
  if (IAmShowingTranSpringCpl)
    pv->mySelectedTranSpringCpl = myJointAdvancedTab->myTranSpringCplMenu->getSelectedOption();

  // Triad :

  if (IAmShowingTriadData)
    {
      pv->selectedTab = mySelectedTabIndex;

      pv->myMass[0] = myTriadSummary->myMassField->getValue();
      pv->myMass[1] = myTriadSummary->myIxField->getValue();
      pv->myMass[2] = myTriadSummary->myIyField->getValue();
      pv->myMass[3] = myTriadSummary->myIzField->getValue();

      pv->mySysDir = myTriadSummary->mySysDirMenu->getSelectedOption();
      if (myTriadSummary->myConnectorMenu->isPoppedUp())
	pv->myTriadConnector = 2+myTriadSummary->myConnectorMenu->getSelectedOption();

      pv->myTriadVals.resize(6);
      for (int i = 0; i < 6; i++)
        myTriadDofs[i]->getValues(pv->myTriadVals[i]);

      myTriadResults->getValues(pv->myResToggles);
    }

  // Load

  if (IAmShowingLoadData)
  {
    pv->myLoadMagnitude = myLoadMagnitude->getValue();
    pv->mySelectedLoadMagnitudeEngine = myLoadMagnitude->getSelectedRef();

    pv->myAttackPoint = myAttackPointEditor->getValue();
    pv->myAttackPointIsGlobal = myAttackPointEditor->isGlobal();

    pv->myFromPoint = myFromPointEditor->getValue();
    pv->myFromPointIsGlobal = myFromPointEditor->isGlobal();

    pv->myToPoint = myToPointEditor->getValue();
    pv->myToPointIsGlobal = myToPointEditor->isGlobal();
  }

  // Generic DB Object

  if (IAmShowingGenDBObjData)
  {
    pv->myGenDBObjType = myGenDBObjTypeField->getValue();
    myGenDBObjDefField->getText(pv->myGenDBObjDef);
  }

  // File reference

  if (IAmShowingFileReference)
    pv->myFileReferenceName = myFileReferenceBrowseField->getFileName();

  // Tire

  if (IAmShowingTireData)
  {
    pv->mySpindelTriadOffset = mySpindelOffset->getDouble();
    pv->mySelectedTireModelType = myTireModelMenu->getSelectedOptionStr();
    pv->mySelectedRoad = myRoadField->getSelectedRef();

    if (myTireDataFileField->isAConstant())
      pv->myTireDataFileName = myTireDataFileField->getText();
    else
      pv->mySelectedTireDataFileRef = myTireDataFileField->getSelectedRef();
  }

  // Road

  if (IAmShowingRoadData)
  {
    pv->iAmUsingExtRoadData = myUseFileRoadRadio->getValue();
    pv->mySelectedRoadFunc  = myRoadFuncField->getSelectedRef();
    pv->myRoadZRotation     = myRoadFields[0]->getDouble();
    pv->myRoadZShift        = myRoadFields[1]->getDouble();
    pv->myRoadXOffset       = myRoadFields[2]->getDouble();

    if (myRoadDataFileField->isAConstant())
      pv->myRoadDataFileName = myRoadDataFileField->getText();
    else
      pv->mySelectedRoadDataFileRef = myRoadDataFileField->getSelectedRef();
  }

  // Material properties

  if (IAmShowingMatPropData)
  {
    pv->myMatPropRho = myMatPropRhoField->getValue();
    pv->myMatPropE   = myMatPropEField->getValue();
    pv->myMatPropNu  = myMatPropNuField->getValue();
    pv->myMatPropG   = myMatPropGField->getValue();
  }

  // Beam properties

  if (IAmShowingBeamPropData)
  {
    pv->selectedTab = mySelectedTabIndex;
    pv->myBeamCrossSectionType = myBeamPropSummary->myCrossSectionTypeMenu->getSelectedOption();
    pv->mySelectedMaterial = myBeamPropSummary->myMaterialDefField->getSelectedRef();
    pv->myBeamBreakDependence = myBeamPropSummary->myDependencyButton->getValue();
    myBeamPropSummary->getValues(pv->myBeamProp);
    pv->myBeamHydroToggle = myBeamPropHydro->getValues(pv->myHydroProp);
  }

  // Strain rosette

  if (IAmShowingStrainRosetteData)
  {
    pv->mySelectedRosetteType = myStrRosTypeMenu->getSelectedOptionStr();

    pv->myStrRosAngle = myStrRosAngleField->getValue();
    pv->IAmResettingStartStrains = myResetStartStrainsToggle->getValue();

    pv->IAmUsingFEHeight = myStrRosUseFEHeightToggle->getValue();
    if (!pv->IAmUsingFEHeight)
      pv->myStrRosHeight = myStrRosHeightField->getValue();

    pv->IAmUsingFEMaterial = myStrRosUseFEMatToggle->getValue();
    if (!pv->IAmUsingFEMaterial) {
      pv->myStrRosEMod = myStrRosEmodField->getValue();
      pv->myStrRosNu = myStrRosNuField->getValue();
    }
  }

  // Element group

  if (IAmShowingGroupData)
  {
    pv->doFatigue = myFatigueToggle->getValue();
#ifdef FT_HAS_GRAPHVIEW
    mySNSelector->getValues(pv->mySNStd,pv->mySNCurve);
#endif
    pv->mySCF = myScfField->getValue();
  }

  // RAO vessel motion

  if (IAmShowingRAOData)
  {
    if (myRAOFileField->isAConstant())
      pv->myRAOFileName = myRAOFileField->getText();
    else
      pv->mySelectedRAOFileRef = myRAOFileField->getSelectedRef();

    pv->mySelectedWaveFunc = myWaveFuncField->getSelectedRef();
    pv->mySelectedWaveDir = myWaveDirMenu->getSelectedInt();
    pv->mySelectedScale = myMotionScaleField->getSelectedRef();
  }

  // Sea state

  if (IAmShowingSeaStateData)
  {
    pv->mySeaStateWidth  = mySeaStateWidthField->getValue();
    pv->mySeaStateHeight = mySeaStateHeightField->getValue();

    pv->mySeaStateWidthPos = mySeaStateWidthPosField->getValue();
    pv->mySeaStateHeightPos = mySeaStateHeightPosField->getValue();

    pv->mySeaStateNumPoints = mySeaStateNumPoints->getValue();
    pv->mySeaStateShowGrid = mySeaStateShowGridToggle->getValue();
    pv->mySeaStateShowSolid = mySeaStateShowSolidToggle->getValue();
  }

  // Simulation event

  if (IAmShowingEventData)
    pv->mySimEventProbability = myEventProbability->getValue();

  // Subassembly

  if (IAmShowingSubAssData || IAmShowingRiserData ||
      IAmShowingJacketData || IAmShowingSoilPileData)
    pv->mySubAssemblyFile = mySubassFileField->getFileName();
}


///////////////////////////////////////////////
//
// Setting some callbacks on init
//
///////

void FuiProperties::setCBs(const FFuaUIValues* values)
{
  const FuaPropertiesValues* pv = dynamic_cast<const FuaPropertiesValues*>(values);
  if (!pv) return;

  myTopologyView->setHighlightedCB(pv->myTopologyHighlightCB);
  myTopologyView->setActivatedCB  (pv->myTopologyActivatedCB);
  myTopologyView->setBuildPopUpCB (pv->myTopologyRightClickedCB);

  myDamperForce->   setCBs(pv->myAxialDaForceValues);
  mySpringForce->   setCBs(pv->myAxialSprForceValues);
  mySpringDeflCalc->setCBs(pv->myAxialSpringDefCalcValues);
  mySpringChar->    setCBs(pv->mySpringCharValues);

  myLinkModelSheet->setChangeLinkCB(pv->myLinkValues.linkChangeCB);
  myLinkModelSheet->setChangeGPVizCB(pv->myLinkValues.linkChangeVizCB);
  myMeshingSheet->setMeshLinkCB(pv->myLinkValues.linkMeshCB);
  myMeshingSheet->materialField->setButtonCB(pv->myEditButtonCB);
  myGenericPartMassSheet->materialField->setButtonCB(pv->myEditButtonCB);

  mySwapTriadButton->setActivateCB(pv->mySwapTriadCB);
  myAddMasterButton->setActivateCB(pv->myAddMasterCB);
  myRevMasterButton->setActivateCB(pv->myRevMasterCB);

  myTriadSummary->mySetAllFixedButton->setActivateCB(pv->myDofSetAllFixedCB);
  myTriadSummary->mySetAllFreeButton->setActivateCB(pv->myDofSetAllFreeCB);

  myJointSummary->mySetAllFixedButton->setActivateCB(pv->myDofSetAllFixedCB);
  myJointSummary->mySetAllFreeButton->setActivateCB(pv->myDofSetAllFreeCB);
  myJointSummary->mySwapJointButton->setActivateCB(pv->mySwapJointCB);

  myDofStatusCB = pv->myDofStatusCB;

  for (int tv = 0; tv < 6; tv++)
    myTriadDofs[tv]->loadField->setCBs(pv->myTriadVals[tv].myLoadVals);

  for (int jv = 0; jv < 6; jv++)
  {
    myJointDofs[jv]->simpleLoad->setCBs(pv->myJointVals[jv].myLoadVals);
    myJointDofs[jv]->springDC->setCBs(pv->myJointVals[jv].mySpringDCVals);
    myJointDofs[jv]->springFS->setCBs(pv->myJointVals[jv].mySpringFSVals);
    myJointDofs[jv]->damperFS->setCBs(pv->myJointVals[jv].myDamperFCVals);
  }

  myJointSummary->myFriction->setButtonCB(pv->myEditButtonCB);

  myBeamPropSummary->myMaterialDefField->setButtonCB(pv->myEditButtonCB);

  myBeamCrossSectionDefField->setButtonCB(pv->myEditButtonCB);
  myShaftCrossSectionDefField->setButtonCB(pv->myEditButtonCB);

  for (FuiQueryInputField* fld : myTurbineFields)
    fld->setButtonCB(pv->myEditButtonCB);

  myLoadMagnitude->setButtonCB(pv->myEditButtonCB);

  myAttackPointEditor->setViewPointCB(pv->myLoadViewAttackPointCB);
  myAttackPointEditor->setViewWhatCB(pv->myLoadViewAttackWhatCB);
  myAttackPointEditor->setPickCB(pv->myLoadPickAttackPointCB);
  myAttackPointEditor->setApplyCB(pv->myLoadApplyCB);

  myFromPointEditor->setViewPointCB(pv->myLoadViewFromPointCB);
  myFromPointEditor->setViewWhatCB(pv->myLoadViewFromWhatCB);
  myFromPointEditor->setPickCB(pv->myLoadPickFromPointCB);

  myToPointEditor->setViewPointCB(pv->myLoadViewToPointCB);
  myToPointEditor->setViewWhatCB(pv->myLoadViewToWhatCB);
  myToPointEditor->setPickCB(pv->myLoadPickToPointCB);

  myBrowseTireFileButton->setActivateCB(pv->myBrowseTireFileCB);
  myTireDataFileField->setButtonCB(pv->myEditButtonCB);
  myRoadField->setButtonCB(pv->myEditButtonCB);

  myRoadFuncField->setButtonCB(pv->myEditButtonCB);
  myRoadDataFileField->setButtonCB(pv->myEditButtonCB);
  myBrowseRoadFileButton->setActivateCB(pv->myBrowseRoadFileCB);

  myStrRosEditNodesButton->setActivateCB(pv->myEditNodesCB);
  myStrRosEditDirButton->setActivateCB(pv->myEditDirectionCB);
  myStrRosFlipZButton->setActivateCB(pv->myFlipZCB);

  myBrowseRAOFileButton->setActivateCB(pv->myBrowseRAOFileCB);
  myRAOFileField->setButtonCB(pv->myEditButtonCB);
  myWaveFuncField->setButtonCB(pv->myEditButtonCB);
  myMotionScaleField->setButtonCB(pv->myEditButtonCB);

  mySimEventCB = pv->mySimEventCB;
}


///////////////////////////////////////////////
//
// Setting the sensivity when blocking due to results
//
///////

void FuiProperties::setSensitivity(bool makeSensitive)
{
  IAmSensitive = makeSensitive;

  // Higher Pairs

  myHPRatioField->setSensitivity(makeSensitive);

  // Spring characteristics

  mySpringChar->setSensitivity(makeSensitive);

  // Spring

  mySpringForce->setSensitivity(makeSensitive);
  mySpringDeflCalc->setSensitivity(makeSensitive);

  // Damper

  myDamperForce->setSensitivity(makeSensitive);

  // Link

  myLinkModelSheet->setSensitivity(makeSensitive);
  myLinkOriginSheet->setSensitivity(makeSensitive);
  myLinkRedOptSheet->setSensitivity(makeSensitive);
  myLinkLoadSheet->setSensitivity(makeSensitive);
  myGenericPartCGSheet->setSensitivity(makeSensitive);
  myGenericPartMassSheet->setSensitivity(makeSensitive);
  myGenericPartStiffSheet->setSensitivity(makeSensitive);
  myHydrodynamicsSheet->setSensitivity(makeSensitive);
  myMeshingSheet->setSensitivity(makeSensitive);
  myAdvancedLinkOptsSheet->setSensitivity(makeSensitive);
  myNonlinearLinkOptsSheet->setSensitivity(makeSensitive);

  // Joints

  mySwapTriadButton->setSensitivity(makeSensitive);
  myAddMasterButton->setSensitivity(makeSensitive);
  myRevMasterButton->setSensitivity(makeSensitive);
  myJointPosition->setSensitivity(makeSensitive);
  myJointSummary->mySetAllFreeButton->setSensitivity(makeSensitive);
  myJointSummary->mySetAllFixedButton->setSensitivity(makeSensitive);

  for (FuiJointDOF* jdof : myJointDofs)
  {
    if (!IAmShowingCamData)
      jdof->motionType->setSensitivity(makeSensitive);
    else if (jdof->dofNo == 0) // local X-dof in Cam joints can only be spring-damper constrained
      jdof->motionType->setSensitivity(false); // disable all toggles
    else
    {
      // Cam joint dofs can only be free or spring-damper constrained
      jdof->motionType->setSensitivity(false);
      jdof->motionType->setSensitivity(0,IAmSensitive);
      jdof->motionType->setSensitivity(3,IAmSensitive);
    }

    jdof->variableType->setSensitivity(makeSensitive);
    jdof->simpleLoad->setSensitivity(makeSensitive);
    jdof->initialVel->setSensitivity(makeSensitive);
    jdof->springDC->setSensitivity(makeSensitive);
    jdof->springFS->setSensitivity(makeSensitive);
    jdof->damperFS->setSensitivity(makeSensitive);
  }

  // Cam

  myJointSummary->myCamThicknessField->setSensitivity(makeSensitive);
  myJointSummary->myCamWidthField->setSensitivity(makeSensitive);
  myJointSummary->myRadialToggle->setSensitivity(makeSensitive);

  // Screw connection

  myJointSummary->myScrewToggle->setSensitivity(makeSensitive);
  if (myJointSummary->myScrewToggle->getValue())
    myJointSummary->myScrewRatioField->setSensitivity(makeSensitive);
  else
    myJointSummary->myScrewRatioField->setSensitivity(false);

  // Friction

  myJointSummary->myFriction->setSensitivity(makeSensitive);
  myJointSummary->myFrictionDof->setSensitivity(makeSensitive);

  // Dof TZ toggle

  myJointSummary->myDOF_TZ_Toggle->setSensitivity(makeSensitive);

  // Rotational formulation and Spring couplings

  myJointAdvancedTab->setSensitivity(makeSensitive);

  // Triad

  myTriadSummary->mySetAllFreeButton->setSensitivity(makeSensitive);
  myTriadSummary->mySetAllFixedButton->setSensitivity(makeSensitive);
  myTriadSummary->myMassField->setSensitivity(makeSensitive);
  myTriadSummary->myIxField->setSensitivity(makeSensitive);
  myTriadSummary->myIyField->setSensitivity(makeSensitive);
  myTriadSummary->myIzField->setSensitivity(makeSensitive);
  myTriadSummary->mySysDirMenu->setSensitivity(makeSensitive);
  myTriadSummary->myConnectorMenu->setSensitivity(makeSensitive);
  myTriadPosition->setSensitivity(makeSensitive);

  for (FuiTriadDOF* tdof : myTriadDofs)
  {
    if (IAmShowingTriadData == 2) // slave triad
    {
      tdof->motionType->setSensitivity(false);
      tdof->variableType->setSensitivity(false);
    }
    else
    {
      tdof->motionType->setSensitivity(makeSensitive);
      tdof->variableType->setSensitivity(makeSensitive);
    }
    tdof->loadField->setSensitivity(makeSensitive);
    tdof->initialVel->setSensitivity(makeSensitive);
  }

  // Load

  myLoadMagnitude->setSensitivity(makeSensitive);
  myAttackPointEditor->setSensitivity(makeSensitive);
  myFromPointEditor->setSensitivity(makeSensitive);
  myToPointEditor->setSensitivity(makeSensitive);

  // Generic DB Object

  myGenDBObjTypeField->setSensitivity(makeSensitive);
  myGenDBObjDefField->setSensitivity(makeSensitive);

  // File reference

  myFileReferenceBrowseField->setSensitivity(makeSensitive);

  // Tire
  bool tireSensitivity = FapLicenseManager::checkTireLicense(false) && makeSensitive;

  myTireDataFileField->setSensitivity(tireSensitivity);
  myBrowseTireFileButton->setSensitivity(tireSensitivity);
  myRoadField->setSensitivity(tireSensitivity);
  myTireModelMenu->setSensitivity(tireSensitivity);
  mySpindelOffset->setSensitivity(tireSensitivity);

  // Road

  myRoadFuncField->setSensitivity(tireSensitivity);
  for (FFuIOField* fld : myRoadFields)
    fld->setSensitivity(tireSensitivity);

  myUseFuncRoadRadio->setSensitivity(tireSensitivity);
  myUseFileRoadRadio->setSensitivity(tireSensitivity);
  myRoadDataFileField->setSensitivity(tireSensitivity);
  myBrowseRoadFileButton->setSensitivity(tireSensitivity);

  // Material properties

  myMatPropRhoField->setSensitivity(makeSensitive);
  myMatPropEField->setSensitivity(makeSensitive);
  myMatPropNuField->setSensitivity(makeSensitive);
  myMatPropGField->setSensitivity(makeSensitive);

  // Beam properties

  myBeamPropSummary->setPropSensitivity(makeSensitive);
  myBeamPropHydro->setPropSensitivity(makeSensitive);

  // Beam

  myBeamCrossSectionDefField->setSensitivity(makeSensitive);
  myBeamDynProps->setSensitivity(makeSensitive);
  for (FFuLabelField* field : myBeamLocalZField)
    field->setSensitivity(makeSensitive);

  // Shaft & Blade

  myShaftCrossSectionDefField->setSensitivity(makeSensitive);
  myShaftDynProps->setSensitivity(makeSensitive);

  // Riser

  myRiserMudButton->setSensitivity(makeSensitive);
  myRiserMudDensityField->setSensitivity(makeSensitive);
  myRiserMudLevelField->setSensitivity(makeSensitive);

  // Turbine

  myTurbineWindVertOffsetField->setSensitivity(makeSensitive);
  for (FuiQueryInputField* fld : myTurbineFields)
    fld->setSensitivity(makeSensitive);

  // Generator

  bool bTorqueSet = myGeneratorTorqueRadioBtn->getValue();
  myGeneratorRadioGroup.setSensitivity(makeSensitive);
  myGeneratorTorqueField->setSensitivity(makeSensitive && bTorqueSet);
  myGeneratorVelocityField->setSensitivity(makeSensitive && !bTorqueSet);

  // Blade

  bool bMTFixed = myBladeFixedPitchToggle->getValue();
  myBladeFixedPitchToggle->setSensitivity(makeSensitive);
  myBladePitchControlField->setSensitivity(makeSensitive && !bMTFixed);
  bool bIceLayer = myBladeIceLayerToggle->getValue();
  myBladeIceLayerToggle->setSensitivity(makeSensitive);
  myBladeIceThicknessField->setSensitivity(makeSensitive && bIceLayer);

  // RAO vessel motion

  myRAOFileField->setSensitivity(makeSensitive);
  myBrowseRAOFileButton->setSensitivity(makeSensitive);
  myWaveFuncField->setSensitivity(makeSensitive);
  myWaveDirMenu->setSensitivity(makeSensitive);
  myMotionScaleField->setSensitivity(makeSensitive);

  // Subassembly

  mySubassPosition->setSensitivity(makeSensitive);

  // Function properties

  bool ctrlSensitivity = FapLicenseManager::checkCtrlLicense(false) && makeSensitive;

  if (IAmShowingCtrlInOut)
    myFunctionProperties->setSensitivity(ctrlSensitivity);
  else
    myFunctionProperties->setSensitivity(makeSensitive);

  // Control system properties

  if (myCtrlElementProperties)
    myCtrlElementProperties->setSensitivity(ctrlSensitivity);

  // External control element

  if (myExtCtrlSysProperties)
    myExtCtrlSysProperties->setSensitivity(FapLicenseManager::checkExtCtrlLicense(false) && makeSensitive);
}


void FuiProperties::updateUICommandsSensitivity(const FFuaUICommands*)
{
  backBtn->updateSensitivity();
  forwardBtn->updateSensitivity();
  helpBtn->updateSensitivity();
}


void FuiProperties::onValuesChanged()
{
  this->updateDBValues(true);
}


void FuiProperties::onMaterialChanged(FuiQueryInputField* matField)
{
  // Need to update the same field in the Meshing tab also,
  // since that is the field that passes the selected material to DB
  myMeshingSheet->materialField->setSelectedRef(matField->getSelectedRef());

  this->onValuesChanged();
}

void FuiProperties::onLinkTabSelected(int)
{
  mySelectedLinkTab  = myLinkTabs->getCurrentTabName();
  mySelectedTabIndex = myLinkTabs->getCurrentTabPosIdx();
}

void FuiProperties::onJointTabSelected(int)
{
  mySelectedJointTab = myJointTabs->getCurrentTabName();
  mySelectedTabIndex = myJointTabs->getCurrentTabPosIdx();
  if (mySelectedJointTab == "Advanced")
    mySelectedJointDof = -1;
  else
    mySelectedJointDof = mySelectedTabIndex - IAmShowingJointData;
}

void FuiProperties::onJointDofChanged(int status)
{
  myDofStatusCB.invoke(mySelectedJointDof,status);
}

void FuiProperties::onTriadTabSelected(int)
{
  mySelectedTriadTab = myTriadTabs->getCurrentTabName();
  mySelectedTabIndex = myTriadTabs->getCurrentTabPosIdx();
  mySelectedTriadDof = mySelectedTabIndex - 2;
}

void FuiProperties::onTriadDofChanged(int status)
{
  myDofStatusCB.invoke(mySelectedTriadDof,status);
}

void FuiProperties::onBeamPropTabSelected(int)
{
  mySelectedBeamPropTab = myBeamPropTabs->getCurrentTabName();
  mySelectedTabIndex    = myBeamPropTabs->getCurrentTabPosIdx();
}

void FuiProperties::onEventActivated()
{
  mySimEventCB.invoke();
  this->updateUIValues();
}


void FuiProperties::onCoGRefChanged(bool toGlobal)
{
  if (mySelectedSubass)
  {
    FaVec3 CoG(mySelectedSubass->getGlobalCoG());
    if (toGlobal)
      mySubassCoGField->setValue(CoG);
    else
      mySubassCoGField->setValue(mySelectedSubass->toLocal(CoG));
  }
}


void FuiProperties::updateSubassCoG()
{
  this->onCoGRefChanged(mySubassCoGField->isGlobal());
}

void FuiProperties::showUsersGuide()
{
  showPDF("file://Doc/FedemUsersGuide.pdf");
}

void FuiProperties::showTheoryGuide()
{
  showPDF("file://Doc/FedemTheoryGuide.pdf");
}


const char* FuiProperties::getSelectedTab(const char* objType) const
{
  if (!strcmp(objType,"Part"))
    return mySelectedLinkTab.c_str();
  else if (!strcmp(objType,"Triad"))
    return mySelectedTriadDof >= 0 ? "DOF" : mySelectedTriadTab.c_str();
  else if (strstr(objType,"joint"))
    return mySelectedJointDof >= 0 ? "DOF" : mySelectedJointTab.c_str();
  else if (!strcmp(objType,"Beam cross section"))
    return mySelectedBeamPropTab.c_str();
#ifdef FT_HAS_GRAPHVIEW
  else if (!strcmp(objType,"Curve") && myCurveDefine)
    return dynamic_cast<FuiCurveDefine*>(myCurveDefine)->getCurrentTabName();
#endif
  else
    return NULL;
}
