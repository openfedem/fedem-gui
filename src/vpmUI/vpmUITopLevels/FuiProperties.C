// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiProperties.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
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
#include "FFuLib/FFuFrame.H"
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
  myTagField->popDown();
  myTopologyView->popDown();

  // Reference Plane

  myRefPlanePosition->popDown();
  myRefPlaneSizeFrame->setLabel("Size");
  myRefPlaneSizeFrame->popDown();
  myRefPlaneHeightField->setLabel("Height");
  myRefPlaneHeightField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myRefPlaneHeightField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myRefPlaneHeightField->popDown();
  myRefPlaneWidthField->setLabel("Width");
  myRefPlaneWidthField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myRefPlaneWidthField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myRefPlaneWidthField->popDown();

  // Higher Pairs

  myHPRatioFrame->setLabel("Transmission output ratio");
  myHPRatioFrame->popDown();
  myHPRatioField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myHPRatioField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myHPRatioField->popDown();

  // Spring characteristics

  mySpringChar->setChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  mySpringChar->popDown();

  // Spring

  mySpringForce->setAsSpring(true);
  mySpringForce->setChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  mySpringForce->popDown();
  mySpringDeflCalc->setChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  mySpringDeflCalc->popDown();

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

  // Beam and Shaft

  myShaftGeneralFrame->setLabel("General");

  myShaftCrossSectionDefLabel->setLabel("Cross-section:");
  myShaftCrossSectionDefField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myShaftCrossSectionDefField->setBehaviour(FuiQueryInputField::REF_NONE);
  myShaftCrossSectionDefField->setButtonMeaning(FuiQueryInputField::EDIT);

  myVisualize3DButton->setLabel("Visualize 3D");
  myVisualize3DButton->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  myVisualize3DButton->popDown();
  myVisualize3DStartAngleField->setLabel("Start");
  myVisualize3DStartAngleField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  myVisualize3DStartAngleField->setLabelMargin(1);
  myVisualize3DStartAngleField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myVisualize3DStartAngleField->popDown();
  myVisualize3DStopAngleField->setLabel("Stop");
  myVisualize3DStopAngleField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  myVisualize3DStopAngleField->setLabelMargin(1);
  myVisualize3DStopAngleField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myVisualize3DStopAngleField->popDown();

  myShaftSDFrame->setLabel("Structural Damping");

  myShaftSDMassField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myShaftSDMassField->setLabel("Mass proportional");
  myShaftSDMassField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myShaftSDMassField->setLabelMargin(0);

  myShaftSDStiffnessField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myShaftSDStiffnessField->setLabel("Stiffness proportional");
  myShaftSDStiffnessField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myShaftSDStiffnessField->setLabelMargin(0);

  myShaftDPFrame->setLabel("Scaling of Dynamic Properties");

  myShaftDPStiffnessField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myShaftDPStiffnessField->setLabel("Stiffness scale");
  myShaftDPStiffnessField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myShaftDPStiffnessField->setLabelMargin(0);

  myShaftDPMassField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myShaftDPMassField->setLabel("Mass scale");
  myShaftDPMassField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myShaftDPMassField->setLabelMargin(0);

  myShaftNoteALabel->setPixMap(info_xpm);
  myShaftNoteBLabel->setLabel("<b>Notes</b>");
  myShaftNoteCLabel->setLabel("Changes applied here will apply to all elements contained in this structure.<br>"
    "Changes to individual elements can be made by editing their respective property fields.");

  myShaftGeneralFrame->popDown();
  myShaftCrossSectionDefLabel->popDown();
  myShaftCrossSectionDefField->popDown();
  myShaftSDFrame->popDown();
  myShaftSDMassField->popDown();
  myShaftSDStiffnessField->popDown();
  myShaftDPFrame->popDown();
  myShaftDPStiffnessField->popDown();
  myShaftDPMassField->popDown();
  myShaftNoteALabel->popDown();
  myShaftNoteBLabel->popDown();
  myShaftNoteCLabel->popDown();

  myBeamOrientationFrame->setLabel("Local Z-axis definition");
  myBeamOrientationFrame->popDown();

  const char* labels[4] = { "X", "Y", "Z", NULL };
  const char** comp = labels;
  for (FFuLabelField* field : myBeamLocalZField) {
    field->setLabel(*(comp++));
    field->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
    field->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
    field->popDown();
  }

  // Turbine, Nacelle, Gearbox, etc. (shared)

  mySubassMassField->setLabel("Mass");
  mySubassMassField->setLabelMargin(0);
  mySubassMassField->setSensitivity(false);
  mySubassMassField->popDown();

  mySubassLengthField->setLabel("Length");
  mySubassLengthField->setLabelMargin(0);
  mySubassLengthField->setSensitivity(false);
  mySubassLengthField->popDown();

  mySubassCoGFrame->setLabel("Center of Gravity");
  mySubassCoGFrame->popDown();
  mySubassCoGField->setRefChangedCB(FFaDynCB1M(FuiProperties,this,onCoGRefChanged,bool));
  mySubassCoGField->setLook(FFuFrame::FLAT);
  mySubassCoGField->setSensitivity(false);
  mySubassCoGField->popDown();

  // Turbine

  myTurbineWindRefFrame->setLabel("Wind reference point");

  myTurbineWindRefTriadDefLabel->setLabel("Reference triad");
  myTurbineWindRefTriadDefField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myTurbineWindRefTriadDefField->setBehaviour(FuiQueryInputField::REF_NONE);
  myTurbineWindRefTriadDefField->setButtonMeaning(FuiQueryInputField::EDIT);

  myTurbineWindVertOffsetLabel->setLabel("Vertical offset");
  myTurbineWindVertOffsetField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myTurbineWindVertOffsetField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  myTurbineAdvTopologyFrame->setLabel("Advanced topology settings");

  myTurbineYawPointTriadDefLabel->setLabel("Yaw point triad");
  myTurbineYawPointTriadDefField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myTurbineYawPointTriadDefField->setBehaviour(FuiQueryInputField::REF_NONE);
  myTurbineYawPointTriadDefField->setButtonMeaning(FuiQueryInputField::EDIT);

  myTurbineHubApexTriadDefLabel->setLabel("Hub apex triad");
  myTurbineHubApexTriadDefField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myTurbineHubApexTriadDefField->setBehaviour(FuiQueryInputField::REF_NONE);
  myTurbineHubApexTriadDefField->setButtonMeaning(FuiQueryInputField::EDIT);

  myTurbineHubPartDefLabel->setLabel("Hub part");
  myTurbineHubPartDefField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myTurbineHubPartDefField->setBehaviour(FuiQueryInputField::REF_NONE);
  myTurbineHubPartDefField->setButtonMeaning(FuiQueryInputField::EDIT);

  myTurbineFirstBearingDefLabel->setLabel("First bearing");
  myTurbineFirstBearingDefField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myTurbineFirstBearingDefField->setBehaviour(FuiQueryInputField::REF_NONE);
  myTurbineFirstBearingDefField->setButtonMeaning(FuiQueryInputField::EDIT);

  myTurbineWindRefFrame->popDown();
  myTurbineWindRefTriadDefLabel->popDown();
  myTurbineWindRefTriadDefField->popDown();
  myTurbineWindVertOffsetLabel->popDown();
  myTurbineWindVertOffsetField->popDown();
  myTurbineAdvTopologyFrame->popDown();
  myTurbineYawPointTriadDefLabel->popDown();
  myTurbineYawPointTriadDefField->popDown();
  myTurbineHubApexTriadDefLabel->popDown();
  myTurbineHubApexTriadDefField->popDown();
  myTurbineHubPartDefLabel->popDown();
  myTurbineHubPartDefField->popDown();
  myTurbineFirstBearingDefLabel->popDown();
  myTurbineFirstBearingDefField->popDown();

  // Generator

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
  myGeneratorTorqueRadioBtn->popDown();
  myGeneratorVelocityRadioBtn->popDown();
  myGeneratorTorqueField->popDown();
  myGeneratorVelocityField->popDown();

  // Blades

  myBladePitchControlLabel->setLabel("Pitch control");

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

  myBladePitchControlLabel->popDown();
  myBladePitchControlField->popDown();
  myBladeFixedPitchToggle->popDown();
  myBladeIceFrame->popDown();
  myBladeIceLayerToggle->popDown();
  myBladeIceThicknessField->popDown();

  // Riser, Jacket and Soil Pile

  myRiserGeneralFrame->setLabel("General");

  myRiserInternalToDefLabel->setLabel("Is internal to:");
  myRiserInternalToDefField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myRiserInternalToDefField->setBehaviour(FuiQueryInputField::REF_NONE);
  myRiserInternalToDefField->setButtonMeaning(FuiQueryInputField::EDIT);
  myRiserInternalToDefField->setSensitivity(false);

  // TODO: For unknown reason it does not work to use the myVisualize3DButton for
  // the riser, jacket and soil pile objects. Find out why and try to fix.
  myRiserVisualize3DButton->setLabel("Visualize 3D");
  myRiserVisualize3DButton->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  myRiserVisualize3DStartAngleField->setLabel("Start");
  myRiserVisualize3DStartAngleField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  myRiserVisualize3DStartAngleField->setLabelMargin(1);
  myRiserVisualize3DStartAngleField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myRiserVisualize3DStopAngleField->setLabel("Stop");
  myRiserVisualize3DStopAngleField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  myRiserVisualize3DStopAngleField->setLabelMargin(1);
  myRiserVisualize3DStopAngleField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));

  myRiserMudFrame->setLabel(" ");

  myRiserMudButton->setLabel("Internal Liquid");
  myRiserMudButton->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));

  myRiserMudDensityField->setLabel("Mud density");
  myRiserMudDensityField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myRiserMudDensityField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myRiserMudDensityField->setLabelMargin(0);

  myRiserMudLevelField->setLabel("Mud level");
  myRiserMudLevelField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myRiserMudLevelField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myRiserMudLevelField->setLabelMargin(0);

  myRiserGeneralFrame->popDown();
  myRiserInternalToDefLabel->popDown();
  myRiserInternalToDefField->popDown();
  myRiserVisualize3DButton->popDown();
  myRiserVisualize3DStartAngleField->popDown();
  myRiserVisualize3DStopAngleField->popDown();
  myRiserMudFrame->popDown();
  myRiserMudButton->popDown();
  myRiserMudDensityField->popDown();
  myRiserMudLevelField->popDown();

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

  // Rotational formulation

  myJointAdvancedTab->popDown();
  myJointAdvancedTab->myRotFormulationFrame->setLabel("Rotation formulation");
  myJointAdvancedTab->myRotFormulationLabel->setLabel("Formulation");
  myJointAdvancedTab->myRotSequenceLabel->setLabel("Sequence");
  myJointAdvancedTab->myRotFormulationMenu->setOptionChangedCB(FFaDynCB1M(FuiProperties,this,
									  onNonConstStringChanged,std::string));
  myJointAdvancedTab->myRotSequenceMenu->setOptionChangedCB(FFaDynCB1M(FuiProperties,this,
								       onNonConstStringChanged,std::string));

  // Spring connectivity

  myJointAdvancedTab->mySpringCplFrame->setLabel("Spring inter-connectivity");
  myJointAdvancedTab->myRotSpringCplLabel->setLabel("Rotation");
  myJointAdvancedTab->myTranSpringCplLabel->setLabel("Translation");
  myJointAdvancedTab->myRotSpringCplMenu->setOptionChangedCB(FFaDynCB1M(FuiProperties,this,
									onNonConstStringChanged,std::string));
  myJointAdvancedTab->myTranSpringCplMenu->setOptionChangedCB(FFaDynCB1M(FuiProperties,this,
									 onNonConstStringChanged,std::string));

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

  myAttackPointFrame->setLabel("Load Target Point");
  myDirectionFrame->setLabel("Direction");
  myFromPointLabel->setLabel("From ");
  myToPointLabel->setLabel("To ");
  myMagnitudeLabel->setLabel("Magnitude");

  myAttackPointEditor->hideOnWhatDisplay(true);
  myAttackPointEditor->setPointChangedCB(FFaDynCB2M(FuiProperties,this,onVecBoolChanged,const FaVec3&,bool));
  myAttackPointEditor->setRefChangedCB(FFaDynCB1M(FuiProperties,this,onBoolTouched,bool));

  myFromPointEditor->hideApplyButton(true);
  myFromPointEditor->setPointChangedCB(FFaDynCB2M(FuiProperties,this,onVecBoolChanged,const FaVec3&,bool));
  myFromPointEditor->setRefChangedCB(FFaDynCB1M(FuiProperties,this,onBoolTouched,bool));

  myToPointEditor->hideApplyButton(true);
  myToPointEditor->setPointChangedCB(FFaDynCB2M(FuiProperties,this,onVecBoolChanged,const FaVec3&,bool));
  myToPointEditor->setRefChangedCB(FFaDynCB1M(FuiProperties,this,onBoolTouched,bool));

  myMagnitudeLabel->popDown();
  myLoadMagnitude->popDown();
  myAttackPointFrame->popDown();
  myDirectionFrame->popDown();
  myAttackPointEditor->popDown();
  myFromPointEditor->popDown();
  myToPointEditor->popDown();
  myFromPointLabel->popDown();
  myToPointLabel->popDown();

  // Generic DB Object

  myGenDBObjTypeLabel->setLabel("Type");
  myGenDBObjDefLabel->setLabel("Definition");

  myGenDBObjTypeField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onStringChanged,const std::string&));
  myGenDBObjTypeField->setToolTip("Enter the keyword identifying the object type for the dynamics solver here");

  myGenDBObjDefField->setTextChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  myGenDBObjDefField->setToolTip("Enter the solver input file data defining the object here.\n"
                                 "Remember to press the Enter key at the end, "
                                 "also when editing existing data.");

  myGenDBObjTypeLabel->popDown();
  myGenDBObjTypeField->popDown();
  myGenDBObjDefLabel->popDown();
  myGenDBObjDefField->popDown();

  // File Reference

  myFileReferenceBrowseField->setAbsToRelPath("yes");
  myFileReferenceBrowseField->setDialogRememberKeyword("FileRefBrowseField");
  myFileReferenceBrowseField->setFileOpenedCB(FFaDynCB2M(FuiProperties,this,onFileRefChanged,const std::string&,int));
  for (const FmFileRefExt& ext : FmFileReference::getExtensions())
    myFileReferenceBrowseField->addDialogFilter(ext.first,ext.second);
  myFileReferenceBrowseField->addAllFilesFilter(true);
  myFileReferenceBrowseField->popDown();

  // Tire

  myTireDataFileLabel->setLabel("Tire file");
  myTireDataFileField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myTireDataFileField->setBehaviour(FuiQueryInputField::REF_TEXT);
  myTireDataFileField->setEditSensitivity(false);

  myBrowseTireFileButton->setLabel("Browse...");
  myRoadLabel->setLabel("Road");
  myRoadField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myRoadField->setBehaviour(FuiQueryInputField::REF_NONE);

  myTireModelTypeLabel->setLabel("Tire model");
  myTireModelMenu->setOptionChangedCB(FFaDynCB1M(FuiProperties,this,onNonConstStringChanged,std::string));

  mySpindelOffsetField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  mySpindelOffsetField->setLabel("Z offset");
  mySpindelOffsetField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  mySpindelOffsetField->setLabelMargin(0);

  myTireDataFileLabel->popDown();
  myTireDataFileField->popDown();
  myBrowseTireFileButton->popDown();
  myRoadLabel->popDown();
  myRoadField->popDown();
  myTireModelTypeLabel->popDown();
  myTireModelMenu->popDown();
  mySpindelOffsetField->popDown();

  // Road

  myUseFuncRoadRadio->setLabel("Road defined by function");
  myRoadFuncLabel->setLabel("Road function");
  myRoadFuncField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myRoadFuncField->setBehaviour(FuiQueryInputField::REF_NONE);

  myRoadZShiftField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myRoadZShiftField->setLabel("Vertical shift");
  myRoadZShiftField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myRoadZShiftField->setLabelMargin(0);
  myRoadXOffsetField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myRoadXOffsetField->setLabel("Horizontal offset");
  myRoadXOffsetField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myRoadXOffsetField->setLabelMargin(0);
  myRoadZRotationField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myRoadZRotationField->setLabel("Rotation about Z axis");
  myRoadZRotationField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myRoadZRotationField->setLabelMargin(0);

  myUseFileRoadRadio->setLabel("Road defined by file");
  myRoadDataFileLabel->setLabel("Road file");
  myRoadDataFileField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myRoadDataFileField->setBehaviour(FuiQueryInputField::REF_TEXT);
  myRoadDataFileField->setEditSensitivity(false);

  myBrowseRoadFileButton->setLabel("Browse...");

  myRoadTypeToggleGroup.insert(myUseFileRoadRadio);
  myRoadTypeToggleGroup.insert(myUseFuncRoadRadio);
  myRoadTypeToggleGroup.setExclusive(true);
  myRoadTypeToggleGroup.setGroupToggleCB(FFaDynCB2M(FuiProperties,this,onIntBoolChanged,int,bool));

  myUseFuncRoadRadio->popDown();
  myUseFileRoadRadio->popDown();
  myRoadDataFileLabel->popDown();
  myRoadDataFileField->popDown();
  myBrowseRoadFileButton->popDown();
  myRoadFuncLabel->popDown();
  myRoadFuncField->popDown();
  myRoadZShiftField->popDown();
  myRoadXOffsetField->popDown();
  myRoadZRotationField->popDown();

  // Material properties

  myMatPropRhoField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myMatPropRhoField->setLabel("<font face='Symbol'><font size='+1'>r</font></font>");
  myMatPropRhoField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myMatPropRhoField->setLabelMargin(0);
  myMatPropEField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myMatPropEField->setLabel("E");
  myMatPropEField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myMatPropEField->setLabelMargin(0);
  myMatPropNuField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myMatPropNuField->setLabel("<font face='Symbol'><font size='+1'>n</font></font>");
  myMatPropNuField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myMatPropNuField->setLabelMargin(0);
  myMatPropGField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myMatPropGField->setLabel("G");
  myMatPropGField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myMatPropGField->setLabelMargin(0);
  myMatPropRhoField->popDown();
  myMatPropEField->popDown();
  myMatPropNuField->popDown();
  myMatPropGField->popDown();

  // Sea state
  mySeaStateSizeFrame->setLabel("Size");
  mySeaStatePositionFrame->setLabel("Position");
  mySeaStateVisualizationFrame->setLabel("Visualization");

  mySeaStateWidthField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  mySeaStateWidthField->setLabel("X-length");
  mySeaStateWidthField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  mySeaStateWidthField->setLabelMargin(0);
  mySeaStateHeightField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  mySeaStateHeightField->setLabel("Y-length");
  mySeaStateHeightField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  mySeaStateHeightField->setLabelMargin(0);

  mySeaStateWidthPosField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  mySeaStateWidthPosField->setLabel("X");
  mySeaStateWidthPosField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  mySeaStateWidthPosField->setLabelMargin(0);

  mySeaStateHeightPosField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  mySeaStateHeightPosField->setLabel("Y");
  mySeaStateHeightPosField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  mySeaStateHeightPosField->setLabelMargin(0);

  mySeaStateNumPoints->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  mySeaStateNumPoints->setLabel("Quantization");
  mySeaStateNumPoints->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  mySeaStateNumPoints->setLabelMargin(0);

  mySeaStateShowGridToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  mySeaStateShowGridToggle->setLabel("Show quantization lines");

  mySeaStateShowSolidToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  mySeaStateShowSolidToggle->setLabel("Show solid");

  mySeaStateSizeFrame->popDown();
  mySeaStatePositionFrame->popDown();
  mySeaStateVisualizationFrame->popDown();
  mySeaStateWidthField->popDown();
  mySeaStateHeightField->popDown();
  mySeaStateWidthPosField->popDown();
  mySeaStateHeightPosField->popDown();
  mySeaStateNumPoints->popDown();
  mySeaStateShowGridToggle->popDown();
  mySeaStateShowSolidToggle->popDown();

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
  myBeamPropHydro->myBeamHydroToggleButton->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  myBeamPropHydro->setAcceptedCBs(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));

  // Strain rosette

  myStrRosTypeLabel->popDown();
  myStrRosTypeLabel->setLabel("Rosette type");
  myStrRosTypeMenu->popDown();
  myStrRosTypeMenu->setOptionSelectedCB(FFaDynCB1M(FuiProperties,this,onIntChanged,int));

  myStrRosNodesField->popDown();
  myStrRosNodesField->setLabel("Nodes");
  myStrRosNodesField->setSensitivity(false);
  myStrRosEditNodesButton->popDown();
  myStrRosEditNodesButton->setLabel("Edit");

  myStrRosAngleField->popDown();
  myStrRosAngleField->setLabel("Angle offset [Deg]");
  myStrRosAngleField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myStrRosAngleField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myStrRosEditDirButton->popDown();
  myStrRosEditDirButton->setLabel("Edit reference direction");

  myStrRosHeightField->popDown();
  myStrRosHeightField->setLabel("Height");
  myStrRosHeightField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myStrRosHeightField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myStrRosUseFEHeightToggle->popDown();
  myStrRosUseFEHeightToggle->setLabel("Use thickness from FE Mesh");
  myStrRosUseFEHeightToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  myStrRosFlipZButton->popDown();
  myStrRosFlipZButton->setLabel("Change side");

  myStrRosEmodField->popDown();
  myStrRosEmodField->setLabel("E-Module");
  myStrRosEmodField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myStrRosEmodField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myStrRosNuField->popDown();
  myStrRosNuField->setLabel("Poissons ratio");
  myStrRosNuField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myStrRosNuField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myStrRosUseFEMatToggle->popDown();
  myStrRosUseFEMatToggle->setLabel("Use material from FE Mesh");
  myStrRosUseFEMatToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));

  myResetStartStrainsToggle->popDown();
  myResetStartStrainsToggle->setLabel("Set start strains to zero");
  myResetStartStrainsToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));

  myMaterialFrame->setLabel("Material");
  myMaterialFrame->toBack();
  myMaterialFrame->popDown();
  myLayerFrame->setLabel("Layer position");
  myLayerFrame->toBack();
  myLayerFrame->popDown();
  myOrientationFrame->setLabel("Orientation");
  myOrientationFrame->toBack();
  myOrientationFrame->popDown();

  // Element group

  myFatigueFrame->setLabel("Fatigue parameters");
  myFatigueFrame->popDown();

  myFatigueToggle->setLabel("Enable fatigue calculation");
  myFatigueToggle->setToolTip("Calculation of damage and life in the Strain Coat Recovery"
			      "\non this element group, based on the selected S-N curve");
  myFatigueToggle->setToggleCB(FFaDynCB1M(FuiProperties,this,onBoolChanged,bool));
  myFatigueToggle->popDown();

#ifdef FT_HAS_GRAPHVIEW
  mySNSelector->setDataChangedCB(FFaDynCB0M(FuiProperties,this,onValuesChanged));
  mySNSelector->popDown();
#endif

  myScfField->setLabel("Stress concentration factor");
  myScfField->setToolTip("The computed stress is scaled by this value"
			 "\nbefore it enters the damage calculation");
  myScfField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myScfField->myField->setDoubleDisplayMode(FFuIOField::AUTO,6,1);
  myScfField->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myScfField->popDown();

  // RAO vessel motion

  myRAOFileLabel->setLabel("RAO file");
  myRAOFileLabel->popDown();
  myRAOFileField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myRAOFileField->setBehaviour(FuiQueryInputField::REF_TEXT);
  myRAOFileField->setEditSensitivity(false);
  myRAOFileField->popDown();
  myBrowseRAOFileButton->setLabel("Browse...");
  myBrowseRAOFileButton->popDown();

  myWaveFuncLabel->setLabel("Wave function");
  myWaveFuncLabel->popDown();
  myWaveFuncField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myWaveFuncField->setBehaviour(FuiQueryInputField::REF_NONE);
  myWaveFuncField->popDown();

  myWaveDirLabel->setLabel("Wave direction");
  myWaveDirLabel->popDown();
  myWaveDirMenu->setOptionSelectedCB(FFaDynCB1M(FuiProperties,this,onIntChanged,int));
  myWaveDirMenu->setToolTip("Angle (in degrees) between the local X-axis of the"
			    "\nVessel Triad and the propagating wave direction,"
			    "\ni.e., a zero angle means the waves propagate in"
			    "\nthe direction of the positive local X-axis.");
  myWaveDirMenu->popDown();

  myMotionScaleLabel->setLabel("Motion scale");
  myMotionScaleLabel->popDown();
  myMotionScaleField->setChangedCB(FFaDynCB1M(FuiProperties,this,onQIFieldChanged,FuiQueryInputField*));
  myMotionScaleField->setBehaviour(FuiQueryInputField::REF_NONE);
  myMotionScaleField->popDown();

  // Simulation event

  myEventProbability->setLabel("Event probability");
  myEventProbability->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myEventProbability->setAcceptedCB(FFaDynCB1M(FuiProperties,this,onDoubleChanged,double));
  myEventProbability->popDown();
  mySelectEventButton->setLabel("Set as active event");
  mySelectEventButton->setActivateCB(FFaDynCB0M(FuiProperties,this,onEventActivated));
  mySelectEventButton->popDown();
  myActiveEventLabel->setLabel("This is the current active event.\n"
			       "All graph plotting and animations "
			       "will use the results of this event.");
  myActiveEventLabel->popDown();

  // Subassembly

  mySubassFileField->setLabel("Subassembly model file");
  mySubassFileField->setAbsToRelPath("yes");
  mySubassFileField->setDialogType(FFuFileDialog::FFU_SAVE_FILE);
  mySubassFileField->setDialogRememberKeyword("SubassFileField");
  mySubassFileField->addDialogFilter("Fedem model file","fmm");
  mySubassFileField->setFileOpenedCB(FFaDynCB2M(FuiProperties,this,onFileRefChanged,const std::string&,int));
  mySubassFileField->setToolTip("The objects of this Subassembly will be saved"
				"\nin this model file when saving the model");
  mySubassFileField->popDown();
  mySubassPosition->popDown();

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

  if ((IAmShowingStartGuide = this->initStartGuide()))
  {
    myTypeField->popDown();
    myIdField->popDown();
    mySGLogoImage->popUp();
    mySGLogoBorderTop->popUp();
    mySGLogoBorderRight->popUp();
    mySGHeading->popUp();
    mySGFillWhite->popUp();
    mySGBorderTop->popUp();
    mySGBorderRight->popUp();
    mySGBorderBottom->popUp();
    mySGContentLabel->popUp();
  }
  else
  {
    mySGLogoImage->popDown();
    mySGLogoBorderTop->popDown();
    mySGLogoBorderRight->popDown();
    mySGHeading->popDown();
    mySGFillWhite->popDown();
    mySGBorderTop->popDown();
    mySGBorderRight->popDown();
    mySGBorderBottom->popDown();
    mySGContentLabel->popDown();
    myTypeField->popUp();
    myIdField->popUp();
    IAmShowingStartGuide = -1; // Start guide not available
  }

  // Create the UA class to communicate with the application

  this->invokeCreateUACB(this);
}


///////////////////////////////////////////////
//
// Place widgets : Geometry management
//
///////

void FuiProperties::placeWidgets(int width, int height)
{
  int fontHeigth = this->getFontHeigth();
  int fieldHeight = getGridLinePos(height,100);
  if (fieldHeight > fontHeigth+5) fieldHeight = fontHeigth+5;
  int hBorder = getGridLinePos(width,4);
  int vBorder = getGridLinePos(height,10);

  int headingBottom = vBorder + fieldHeight;
  int leftPartSeparator = 2*hBorder + getGridLinePos(width,270);
  int aThreePart = (width - leftPartSeparator)/3;

  int textHeight = myDescriptionField->myLabel->getHeightHint();

  // Heading

  int glv1 = hBorder;
  int glv2 = glv1 + getGridLinePos(width,230);
  int glv3 = glv2 + hBorder;
  int glv4 = glv3 + getGridLinePos(width,40);
  int glh1 = vBorder;
  int glh2 = glh1 + fieldHeight;
  if (IAmShowingStartGuide < 1)
    {
      myTypeField->setEdgeGeometry(glv1, glv2, glh1, glh2);
      myIdField->  setEdgeGeometry(glv3, glv4, glh1, glh2);
    }

  if (IAmShowingHeading)
    {
      int glv5 = glv4 + hBorder;
      int glv8 = width - 2 * hBorder - 3 * fieldHeight;
      int glv6 = glv5 + (glv8 - glv5)*2/3 - hBorder;
      int glv7 = glv6 + 2*hBorder;
      int glh3 = height;
      if (IAmShowingSwapTriadButton || IAmShowingAddMasterButton || IAmShowingRevMasterButton)
        glh3 -= 3*fieldHeight/2 + 2*vBorder;

      myDescriptionField->setEdgeGeometry(glv5, glv6, glh1, glh2);
      myTagField->        setEdgeGeometry(glv7, glv8, glh1, glh2);
      myTopologyView->    setEdgeGeometry(0,leftPartSeparator,glh2,glh3);
    }

  backBtn->setEdgeGeometry(width-3*fieldHeight, width-2*fieldHeight, vBorder, vBorder+fieldHeight);
  forwardBtn->setEdgeGeometry(width-2*fieldHeight, width-fieldHeight, vBorder, vBorder+fieldHeight);
  helpBtn->setEdgeGeometry(width-fieldHeight, width, vBorder, vBorder+fieldHeight);

  // Reference Plane

  if (IAmShowingRefPlane)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = v1 + 2*hBorder;
      int v4 = leftPartSeparator + aThreePart - hBorder;
      int v3 = v4 - 2*hBorder;
      int v5 = width - 2*hBorder;
      int h0 = headingBottom + 2*vBorder;
      myRefPlanePosition->setEdgeGeometry(v1,v5,h0,height);

      int labelWidth = myRefPlaneHeightField->myLabel->getWidthHint();
      int fieldHeigt = myRefPlanePosition->myFields.front()->getHeightHint();

      int h1 = h0 + 3*textHeight + 2*fieldHeigt + 10*vBorder;
      int h2 = h1 + textHeight + 2*vBorder;
      int h3 = h2 + fieldHeight;
      int h4 = h3 + 2*vBorder;
      int h5 = h4 + fieldHeight;
      int h6 = h5 + 2*vBorder;

      myRefPlaneHeightField->setLabelWidth(labelWidth);
      myRefPlaneWidthField->setLabelWidth(labelWidth);

      myRefPlaneSizeFrame->setEdgeGeometry(v1,v4,h1,h6);
      myRefPlaneHeightField->setEdgeGeometry(v2,v3,h2,h3);
      myRefPlaneWidthField->setEdgeGeometry(v2,v3,h4,h5);
    }

  // Higher Pair

  if (IAmShowingHPRatio)
    {
      int glv1 = leftPartSeparator + 2*hBorder;
      int glv2 = leftPartSeparator + aThreePart - hBorder;
      int glh1 = headingBottom + 5*vBorder;
      int glh2 = glh1+textHeight+ 2*vBorder;
      int glh3 = glh2 + fieldHeight;
      int glh4 = glh3 + 2*vBorder;

      myHPRatioFrame->setEdgeGeometry(glv1,glv2,glh1,glh4);
      myHPRatioField->setEdgeGeometry(glv1 + 2*hBorder,glv2 - 2*hBorder,glh2 , glh3);
    }

  // Axial Damper

  if(IAmShowingAxialDamper)
    {
      int glv1 = leftPartSeparator + hBorder;
      int glv2 = width;
      int glh1 = headingBottom + vBorder;
      int glh2 = height;

      myDamperForce->setEdgeGeometry(glv1, glv2, glh1, glh2);
    }

  // Axial Spring

  if(IAmShowingAxialSpring)
    {
      int glv1 = leftPartSeparator + hBorder;
      int glv2 = glv1 + (width - glv1)/2;
      int glv3 = glv2 + hBorder;
      int glv4 = width;
      int glh1 = headingBottom + vBorder;
      int glh2 = height;

      mySpringForce   ->setEdgeGeometry(glv3, glv4, glh1, glh2);
      mySpringDeflCalc->setEdgeGeometry(glv1, glv2, glh1, glh2);
    }

  // Advanced Spring Characteristics

  if(IAmShowingSpringChar)
    {
      int glv1 = leftPartSeparator + hBorder;
      int glv2 = width;
      int glh1 = headingBottom + vBorder;
      int glh2 = height;

      mySpringChar->setEdgeGeometry(glv1, glv2, glh1, glh2);
    }

  // Part

  if(IAmShowingLinkData)
    {
      int glv1 = leftPartSeparator + hBorder;
      int glv2 = width;
      int glh1 = headingBottom + vBorder;
      int glh2 = height;

      myLinkTabs->setEdgeGeometry(glv1, glv2, glh1, glh2);
    }

  // Beam and Shaft

  if (IAmShowingBeamData || IAmShowingShaftData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int hDelta = (width-v1)/3;
      int h1 = headingBottom + 2*vBorder;
      int y = fieldHeight+vBorder;
      int rEdge = v1+hDelta-10;

      myShaftGeneralFrame->setEdgeGeometry(v1, rEdge, h1, h1+130+fieldHeight+vBorder);
      if (myShaftCrossSectionDefLabel->isPoppedUp()) {
        myShaftCrossSectionDefLabel->setEdgeGeometry(v1+10, rEdge-10, h1+y, h1+fieldHeight+y);
        y += fieldHeight+vBorder;
        myShaftCrossSectionDefField->setEdgeGeometry(v1+10, rEdge-10, h1+y, h1+fieldHeight+y);
        y += fieldHeight+vBorder+5;
      }
      mySubassMassField->setEdgeGeometry(v1+10, rEdge-10, h1+y, h1+fieldHeight+y);
      y += fieldHeight+vBorder;
      mySubassLengthField->setEdgeGeometry(v1+10, rEdge-10, h1+y, h1+fieldHeight+y);
      y += fieldHeight+vBorder;
      myVisualize3DButton->setEdgeGeometry(v1+10, rEdge-10, h1+y, h1+fieldHeight+y);
      y += fieldHeight+vBorder;
      myVisualize3DStartAngleField->setEdgeGeometry(v1+10, v1+10+60, h1+y, h1+fieldHeight+y);
      myVisualize3DStopAngleField->setEdgeGeometry(v1+10+70, v1+10+70+60, h1+y, h1+fieldHeight+y);
    }

  if (IAmShowingUDEData || IAmShowingBeamData || IAmShowingShaftData || IAmShowingBladeData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int hDelta = (width-v1)/(IAmShowingUDEData ? 2 : 3);
      int v2     = v1 + (IAmShowingUDEData ? 0 : hDelta);
      int v3     = v2 + hDelta;
      int rEdge  = v2 + hDelta-10;

      int y = headingBottom + 2*vBorder;
      int yy = y+3*fieldHeight+4*vBorder;
      myShaftSDFrame->setEdgeGeometry(v2, rEdge, y, yy);
      y += fieldHeight+vBorder;
      myShaftSDMassField->setEdgeGeometry(v2+10, rEdge-10, y, y+fieldHeight);
      y += fieldHeight+vBorder;
      myShaftSDStiffnessField->setEdgeGeometry(v2+10, rEdge-10, y, y+fieldHeight);

      y = headingBottom + 2*vBorder;
      rEdge = v3+hDelta-10;
      myShaftDPFrame->setEdgeGeometry(v3, rEdge, y, yy);
      y += fieldHeight+vBorder;
      myShaftDPStiffnessField->setEdgeGeometry(v3+10, rEdge-10, y, y+fieldHeight);
      y += fieldHeight+vBorder;
      myShaftDPMassField->setEdgeGeometry(v3+10, rEdge-10, y, y+fieldHeight);

      myShaftSDMassField->setLabelWidth(110);
      myShaftSDStiffnessField->setLabelWidth(110);
      myShaftDPStiffnessField->setLabelWidth(80);
      myShaftDPMassField->setLabelWidth(80);

      if (IAmShowingUDEData) {
	yy += 2*vBorder;
	mySubassMassField->setEdgeGeometry(v2+10, v3-20, yy, yy+fieldHeight);
      }
    }

  if (IAmShowingBeamData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = v1+(width-v1)/3;
      int y = headingBottom + 3*fieldHeight + 10*vBorder;
      int hDelta = (width-10-v2)/3;
      myBeamOrientationFrame->setEdgeGeometry(v2, width-10, y, y+2*fieldHeight+3*vBorder);
      y += fieldHeight+vBorder;
      for (FFuLabelField* field : myBeamLocalZField) {
	field->setEdgeGeometry(v2+10, v2+hDelta-10, y, y+fieldHeight);
	v2 += hDelta;
      }
    }

  if (IAmShowingShaftData || IAmShowingBladeData)
    {
      int h1 = height - vBorder - 3*fieldHeight;
      int h2 = h1 + fieldHeight;
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = v1 + (width-v1)/3;
      myShaftNoteALabel->setEdgeGeometry(v2,v2+16,h1,h1+fieldHeight);
      myShaftNoteBLabel->setEdgeGeometry(v2+20,width,h1,h2);
      myShaftNoteCLabel->setEdgeGeometry(v2,width,h2,h2+2*fieldHeight);
    }

  // Turbine, Nacelle, Gearbox, etc. (shared)

  if (IAmShowingTowerData ||
      IAmShowingNacelleData ||
      IAmShowingGearboxData ||
      IAmShowingRotorData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = v1 + 140;
      int y = headingBottom + 2*vBorder;
      mySubassMassField->setEdgeGeometry(v1, v2, y, y+fieldHeight);
      y += fieldHeight + 2*vBorder;
      mySubassCoGFrame->setEdgeGeometry(v1-hBorder, v2, y, y+fontHeigth+4*fieldHeight+vBorder);
      y += fontHeigth;
      mySubassCoGField->setEdgeGeometry(v1, v2-hBorder, y, y+4*fieldHeight);
    }
  if (IAmShowingTowerData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int y = headingBottom + 6*vBorder + 5*fieldHeight + fontHeigth;
      myVisualize3DButton->setEdgeGeometry(v1, v1+140, y, y+fieldHeight);
    }
  if (IAmShowingTurbineData)
    {
      int v1 = leftPartSeparator + hBorder;
      int v2 = width - hBorder;
      int v3 = v1 + 140;

      // mass field
      int y = headingBottom + 2*vBorder;
      mySubassMassField->setEdgeGeometry(v1+hBorder, v3+hBorder, y, y+fieldHeight);
      // center of gravity field
      y += fieldHeight + 2*vBorder;
      mySubassCoGFrame->setEdgeGeometry(v1, v3+hBorder, y, y+fontHeigth+4*fieldHeight+vBorder);
      y += fontHeigth;
      mySubassCoGField->setEdgeGeometry(v1+hBorder, v3, y, y+4*fieldHeight);

      // position fields
      int spHeight = mySubassPosition->getHeightHint();
      mySubassPosition->setEdgeGeometry(v3+3*hBorder, v2, headingBottom, headingBottom+spHeight);

      // adv topology settings
      int x1 = v3+3*hBorder;
      y = headingBottom + spHeight;
      int cmbWidth = (v2-x1-20)/3;
      myTurbineWindRefFrame->setEdgeGeometry(x1, x1+cmbWidth+20, y, height);
      y += 14;
      x1 += 10;
      int x2 = x1 + cmbWidth;
      myTurbineWindRefTriadDefLabel->setEdgeGeometry(x1, x2, y, y+fieldHeight);
      y += fieldHeight;
      myTurbineWindRefTriadDefField->setEdgeGeometry(x1, x2, y, y+fieldHeight);
      y += fieldHeight;
      myTurbineWindVertOffsetLabel->setEdgeGeometry(x1, x2, y, y+fieldHeight);
      y += fieldHeight;
      myTurbineWindVertOffsetField->setEdgeGeometry(x1, x2, y, y+fieldHeight);

      x1 += cmbWidth + 4 + 20;
      x2 += cmbWidth + 4;
      y = headingBottom + spHeight + 14;
      myTurbineAdvTopologyFrame->setEdgeGeometry(x1-10, v2, y-14, height);
      myTurbineYawPointTriadDefLabel->setEdgeGeometry(x1, x2, y, y+fieldHeight);
      y += fieldHeight;
      myTurbineYawPointTriadDefField->setEdgeGeometry(x1, x2, y, y+fieldHeight);
      y += fieldHeight;
      myTurbineHubApexTriadDefLabel->setEdgeGeometry(x1, x2, y, y+fieldHeight);
      y += fieldHeight;
      myTurbineHubApexTriadDefField->setEdgeGeometry(x1, x2, y, y+fieldHeight);

      x1 += cmbWidth + 4;
      x2 += cmbWidth + 4;
      y = headingBottom + spHeight + 14;
      myTurbineHubPartDefLabel->setEdgeGeometry(x1, x2, y, y+fieldHeight);
      y += fieldHeight;
      myTurbineHubPartDefField->setEdgeGeometry(x1, x2, y, y+fieldHeight);
      y += fieldHeight;
      myTurbineFirstBearingDefLabel->setEdgeGeometry(x1, x2, y, y+fieldHeight);
      y += fieldHeight;
      myTurbineFirstBearingDefField->setEdgeGeometry(x1, x2, y, y+fieldHeight);
    }
  if (IAmShowingGeneratorData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = v1 + 140;
      int y = headingBottom + 2*vBorder;
      mySubassMassField->setEdgeGeometry(v1, v2, y, y+fieldHeight);
      y += fieldHeight + 2*vBorder;
      mySubassCoGFrame->setEdgeGeometry(v1-hBorder, v2, y, y+fontHeigth+4*fieldHeight+vBorder);
      y += fontHeigth;
      mySubassCoGField->setEdgeGeometry(v1, v2-hBorder, y, y+4*fieldHeight);
      y = headingBottom + 2*vBorder;
      v1 = v2 + 2*hBorder;
      myGeneratorTorqueRadioBtn->setEdgeGeometry(v1, v1+100, y, y+fieldHeight);
      myGeneratorTorqueField->setEdgeGeometry(v1+110, v1+310, y, y+fieldHeight);
      y += fieldHeight+10;
      myGeneratorVelocityRadioBtn->setEdgeGeometry(v1, v1+100, y, y+fieldHeight);
      myGeneratorVelocityField->setEdgeGeometry(v1+110, v1+310, y, y+fieldHeight);
      y += fieldHeight+10;
    }
  if (IAmShowingBladeData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int hDelta = (width-v1)/3;
      int v2 = v1+hDelta;
      int y = headingBottom + 2*vBorder;
      int rEdge = v1+hDelta-10;
      myBladePitchControlLabel->setEdgeGeometry(v1, v1+100, y, y+fieldHeight);
      y += fieldHeight+0;
      myBladePitchControlField->setEdgeGeometry(v1, rEdge, y, y+fieldHeight);
      y += fieldHeight+4;
      myBladeFixedPitchToggle->setEdgeGeometry(v1, rEdge, y, y+fieldHeight);
      y += fieldHeight+14;
      mySubassMassField->setEdgeGeometry(v1, v1+(hDelta/2)-10, y, y+fieldHeight);
      mySubassLengthField->setEdgeGeometry(v1+(hDelta/2), v1+hDelta-10, y, y+fieldHeight);
      y += fieldHeight+14;
      myBladeIceFrame->setEdgeGeometry(v1, rEdge, y, y+75);
      y += fieldHeight+4;
      myBladeIceLayerToggle->setEdgeGeometry(v1+10, v1+110, y, y+fieldHeight);
      y += fieldHeight+4;
      myBladeIceThicknessField->setEdgeGeometry(v1+10, v1+140, y, y+fieldHeight);

      rEdge = v2+hDelta-10;
      y = headingBottom + 3*fieldHeight+5*vBorder+14;
      myVisualize3DButton->setEdgeGeometry(v2, rEdge, y, y+fieldHeight);
    }
  if (IAmShowingRiserData || IAmShowingJacketData || IAmShowingSoilPileData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int hDelta = (width-v1)/4;
      int v2 = v1 + hDelta-10;
      int v3 = v2 + hDelta-20;
      int v4 = v3 + hDelta+30;
      int rEdge1 = v2-10;
      int rEdge2 = v3-10;
      int rEdge3 = v4-10;

      int h1 = headingBottom + 2*vBorder;
      int h3 = h1 + fieldHeight + 3*vBorder;
      int h2 = h3 + fontHeigth + 4*fieldHeight + 6*vBorder;
      if (IAmShowingRiserData)
	h2 += 2*fieldHeight + 2*vBorder;
      else if (IAmShowingJacketData)
	h2 += fontHeigth + 2*fieldHeight - vBorder;
      int h4 = h3 + 3*fieldHeight + 4*vBorder;

      myRiserGeneralFrame->setEdgeGeometry(v1, rEdge1, h3, h2);

      int y = h3 + fontHeigth+vBorder;
      if (IAmShowingRiserData) {
        myRiserInternalToDefLabel->setEdgeGeometry(v1+10, rEdge1-10, y, y+fieldHeight);
        y += fieldHeight+vBorder;
        myRiserInternalToDefField->setEdgeGeometry(v1+10, rEdge1-10, y, y+fieldHeight);
        y += fieldHeight+vBorder;
      }
      mySubassMassField->setEdgeGeometry(v1+10, rEdge1-10, y, y+fieldHeight);
      y += fieldHeight+vBorder;
      if (IAmShowingJacketData) {
	int h5 = y + fontHeigth;
	mySubassCoGFrame->setEdgeGeometry(v1+hBorder, rEdge1-hBorder, y, h5+4*fieldHeight+vBorder);
	mySubassCoGField->setEdgeGeometry(v1+2*hBorder, rEdge1-2*hBorder, h5, h5+4*fieldHeight);
	y = h5 + 4*fieldHeight + 2*vBorder;
      }
      else {
        mySubassLengthField->setEdgeGeometry(v1+10, rEdge1-10, y, y+fieldHeight);
        y += fieldHeight+vBorder;
      }
      myRiserVisualize3DButton->setEdgeGeometry(v1+10, rEdge1-10, y, y+fieldHeight);
      y += fieldHeight+vBorder;
      myRiserVisualize3DStartAngleField->setEdgeGeometry(v1+10, v1+10+60, y, y+fieldHeight);
      myRiserVisualize3DStopAngleField->setEdgeGeometry(v1+10+70, v1+10+70+60, y, y+fieldHeight);

      myRiserMudFrame->setEdgeGeometry(v2, rEdge2, h3, h4);

      myRiserMudButton->setEdgeGeometry(v2+10, v2+100, h3, h3+fieldHeight);
      y = h3 + fieldHeight+vBorder;
      myRiserMudDensityField->setEdgeGeometry(v2+10, rEdge2-10, y, y+fieldHeight);
      y += fieldHeight+vBorder;
      myRiserMudLevelField->setEdgeGeometry(v2+10, rEdge2-10, y, y+fieldHeight);

      mySubassFileField->setEdgeGeometry(v1,width,h1,h1+fieldHeight);

      int nSDDPOffsetX = IAmShowingJacketData ? v3 - v2 : 0;
      myShaftSDFrame->setEdgeGeometry(v3-nSDDPOffsetX, rEdge3-nSDDPOffsetX, h3, h4);

      y = fieldHeight+vBorder;
      myShaftSDMassField->setEdgeGeometry(v3+10-nSDDPOffsetX, rEdge3-10-nSDDPOffsetX, h3+y, h3+fieldHeight+y);
      y += fieldHeight+vBorder;
      myShaftSDStiffnessField->setEdgeGeometry(v3+10-nSDDPOffsetX, rEdge3-10-nSDDPOffsetX, h3+y, h3+fieldHeight+y);

      myShaftDPFrame->setEdgeGeometry(v4-nSDDPOffsetX, width-nSDDPOffsetX, h3, h4);

      y = fieldHeight+vBorder;
      myShaftDPStiffnessField->setEdgeGeometry(v4+10-nSDDPOffsetX, width-10-nSDDPOffsetX, h3+y, h3+fieldHeight+y);
      y += fieldHeight+vBorder;
      myShaftDPMassField->setEdgeGeometry(v4+10-nSDDPOffsetX, width-10-nSDDPOffsetX, h3+y, h3+fieldHeight+y);
      y += fieldHeight+vBorder+20;

      // position fields
      int spHeight = mySubassPosition->getHeightHint();
      mySubassPosition->setEdgeGeometry(v2, width, h4+vBorder, h4+vBorder+spHeight);

      myShaftSDMassField->setLabelWidth(110);
      myShaftSDStiffnessField->setLabelWidth(110);
      myShaftDPStiffnessField->setLabelWidth(80);
      myShaftDPMassField->setLabelWidth(80);
      myRiserMudDensityField->setLabelWidth(60);
      myRiserMudLevelField->setLabelWidth(60);
    }

  if (IAmShowingRiserData || IAmShowingSoilPileData ||
      IAmShowingShaftData || IAmShowingBeamData || IAmShowingUDEData)
  {
    mySubassMassField->setLabelWidth(60);
    mySubassLengthField->setLabelWidth(60);
  }
  else
  {
    mySubassMassField->setLabelWidth(30);
    mySubassLengthField->setLabelWidth(34);
  }

  // Joints

  if(IAmShowingJointData)
    {
      int glv1 = leftPartSeparator + hBorder;
      int glv2 = width;
      int glh1 = headingBottom + vBorder;
      int glh2 = height;

      // "Touch" the geometry first
      myJointTabs->setEdgeGeometry(glv1, glv2-1, glh1, glh2);
      myJointTabs->setEdgeGeometry(glv1, glv2, glh1, glh2);
    }

  if(IAmShowingPipeSurfaceData)
    {
      int glv1 = leftPartSeparator + hBorder;
      int glv2 = width/2;
      int glh1 = headingBottom + vBorder;
      int glh2 = glh1 + fieldHeight;

      myPipeRadiusField->setEdgeGeometry(glv1, glv2, glh1, glh2);
    }

  if(IAmShowingSwapTriadButton)
    {
      int glv1 = hBorder;
      int glv2 = getGridLinePos(width,200);
      int glh2 = height;
      int glh1 = glh2 - 3*fieldHeight/2;

      mySwapTriadButton->setEdgeGeometry(glv1,glv2,glh1,glh2);
    }

  if(IAmShowingAddMasterButton)
    {
      int glv1 = hBorder;
      int glv2 = getGridLinePos(width, IAmShowingRevMasterButton ? 135 : 150);
      int glh2 = height;
      int glh1 = glh2 - 3*fieldHeight/2;

      myAddMasterButton->setEdgeGeometry(glv1,glv2,glh1,glh2);

      if (IAmShowingRevMasterButton)
      {
        glv1 += hBorder + glv2;
        glv2 = hBorder + getGridLinePos(width,270);
        myRevMasterButton->setEdgeGeometry(glv1,glv2,glh1,glh2);
      }
    }
  else if (IAmShowingRevMasterButton)
    {
      int glv1 = hBorder;
      int glv2 = getGridLinePos(width,150);
      int glh2 = height;
      int glh1 = glh2 - 3*fieldHeight/2;

      myRevMasterButton->setEdgeGeometry(glv1,glv2,glh1,glh2);
    }

  // Triad

  if(IAmShowingTriadData)
    {
      int glv1 = leftPartSeparator + hBorder;
      int glv2 = width;
      int glh1 = headingBottom + vBorder;
      int glh2 = height;

      // "Touch" the geometry first
      myTriadTabs->setEdgeGeometry(glv1, glv2-1, glh1, glh2);
      myTriadTabs->setEdgeGeometry(glv1, glv2, glh1, glh2);
    }

  // Load

  if(IAmShowingLoadData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = v1 + 2*hBorder;
      int v3 = leftPartSeparator + aThreePart - 2*hBorder;
      int v4 = v3 + 2*hBorder;
      int v5 = v4 + 2*hBorder;
      int v6 = v5 + 2*hBorder;
      int v7 = leftPartSeparator + 2*aThreePart - hBorder;
      int v8 = v7 + 2*hBorder;
      int v10 = width;
      int v9 = v10 - hBorder;

      int h1 = headingBottom + 2*vBorder;
      int h2 = h1 + textHeight + vBorder;
      int h3 = h2 + fieldHeight;
      int h31 = h3 + 2*vBorder;
      int h32 = h31 + vBorder;
      int h4 = h32 + textHeight + vBorder;
      int h6 = height;
      int h5 = h6 - vBorder;

      myLoadMagnitude->    setEdgeGeometry(v2, v3, h2, h3);
      myMagnitudeLabel->   setEdgeGeometry(v1,v4, h1, h31);

      myAttackPointFrame-> setEdgeGeometry(v1, v4, h32, h6);
      myAttackPointEditor->setEdgeGeometry(v2, v3, h4, h5);

      myDirectionFrame->   setEdgeGeometry(v5, v10,h1, h6);

      myFromPointLabel->   setEdgeGeometry(v6, v7, h2, h3);
      myToPointLabel->     setEdgeGeometry(v8, v9, h2, h3);

      myFromPointEditor->  setEdgeGeometry(v6, v7, h3, h5);
      myToPointEditor->    setEdgeGeometry(v8, v9, h3, h5);
    }

  // Generic DB Object

  if (IAmShowingGenDBObjData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = v1 + 2*hBorder +  this->myGenDBObjDefLabel->getWidthHint();
      int v3 = width;
      int h1 = headingBottom + 2*hBorder;
      int h2 = h1 + fieldHeight;
      int h3 = h2 + vBorder;
      int h35 = h3 + textHeight;
      int h4 = height - vBorder;

      myGenDBObjTypeLabel->setEdgeGeometry(v1, v2, h1, h2);
      myGenDBObjTypeField->setEdgeGeometry(v2, v3, h1, h2);
      myGenDBObjDefLabel->setEdgeGeometry(v1, v2, h3, h35);
      myGenDBObjDefField->setEdgeGeometry(v2, v3, h3, h4);
    }

  // File Reference

  if (IAmShowingFileReference)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = width - hBorder;
      int h1 = headingBottom + 2*vBorder;
      int h2 = h1 + fieldHeight;

      myFileReferenceBrowseField->setEdgeGeometry(v1, v2, h1, h2);
    }

  // Tire

  if(IAmShowingTireData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = v1 +  this->getFontWidth("Tire model   ");
      int v4 = width - this->getFontWidth(" Browse...   ");
      int v5 = width;

      int h1 = headingBottom + 2*vBorder;
      int h2 = h1 + fieldHeight;
      int h3 = h2 + vBorder;
      int h4 = h3 + fieldHeight;
      int h5 = h4 + vBorder;
      int h6 = h5 + fieldHeight;
      int h7 = h6 + vBorder;
      int h8 = h7 + fieldHeight;

      myTireDataFileLabel->   setEdgeGeometry(v1, v2, h1, h2);
      myTireDataFileField->   setEdgeGeometry(v2, v4, h1, h2);
      myBrowseTireFileButton->setEdgeGeometry(v4, v5, h1, h2);
      myRoadLabel->           setEdgeGeometry(v1, v2, h3, h4);
      myRoadField->           setEdgeGeometry(v2, v4, h3, h4);
      myTireModelTypeLabel->  setEdgeGeometry(v1, v2, h5, h6);
      myTireModelMenu->       setEdgeGeometry(v2, v4, h5, h6);
      mySpindelOffsetField->  setEdgeGeometry(v1, v4, h7, h8);
      mySpindelOffsetField->setLabelWidth(v2-v1);
    }

  // Road

  if(IAmShowingRoadData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v1_5 = v1 + 5*hBorder;
      int v2 = v1_5 + this->getFontWidth(" Function argument ");
      int v4 = width - this->getFontWidth(" Browse... ");
      int v5 = width;

      int h1 = headingBottom + 2*vBorder;
      int h2 = h1 + fieldHeight;
      int h3 = h2 + vBorder;
      int h4 = h3 + fieldHeight;
      int h5 = h4 + vBorder;
      int h5_1 = h5 + fieldHeight;
      int h5_2 = h5_1 + vBorder;
      int h5_3 = h5_2 + fieldHeight;
      int h5_4 = h5_3 + vBorder;
      int h5_5 = h5_4 + fieldHeight;
      int h6 = h5_5;
      int h7 = h6 + 3*vBorder;
      int h8 = h7 + fieldHeight;
      int h9 = h8 + vBorder;
      int h10 = h9 + fieldHeight;

      myUseFuncRoadRadio->  setEdgeGeometry(v1,   v5, h1, h2);
      myRoadFuncLabel->     setEdgeGeometry(v1_5, v2, h3, h4);
      myRoadFuncField->     setEdgeGeometry(v2,   v5, h3, h4);

      myRoadZShiftField->setLabelWidth(myRoadZRotationField->getLabelWidth());
      myRoadXOffsetField->setLabelWidth(myRoadZRotationField->getLabelWidth());
      myRoadZRotationField->setLabelWidth(myRoadZRotationField->getLabelWidth());

      myRoadZRotationField-> setEdgeGeometry(v2, v5, h5, h5_1);
      myRoadZShiftField->    setEdgeGeometry(v2, v5, h5_2, h5_3);
      myRoadXOffsetField->   setEdgeGeometry(v2, v5 ,h5_4, h5_5);

      myUseFileRoadRadio->     setEdgeGeometry(v1,   v5, h7, h8);
      myRoadDataFileLabel->    setEdgeGeometry(v1_5, v2, h9, h10);
      myRoadDataFileField->    setEdgeGeometry(v2,   v4, h9, h10);
      myBrowseRoadFileButton-> setEdgeGeometry(v4,   v5, h9, h10);
    }

  // Material properties

  if(IAmShowingMatPropData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = v1+200;
      int h1 = headingBottom + 2*vBorder;
      int h2 = h1 + fieldHeight;
      int y = 0;

      myMatPropRhoField->setLabelWidth(20);
      myMatPropEField->setLabelWidth(20);
      myMatPropNuField->setLabelWidth(20);
      myMatPropGField->setLabelWidth(20);

      myMatPropRhoField->setEdgeGeometry(v1, v2, h1+y, h2+y);
      y += fieldHeight+vBorder;
      myMatPropEField->setEdgeGeometry(v1, v2, h1+y, h2+y);
      y += fieldHeight+vBorder;
      myMatPropNuField->setEdgeGeometry(v1, v2, h1+y, h2+y);
      y += fieldHeight+vBorder;
      myMatPropGField->setEdgeGeometry(v1, v2, h1+y, h2+y);
    }

  // Sea state

  if(IAmShowingSeaStateData)
  {
    int v1 = leftPartSeparator + 2*hBorder;
    int v2 = v1+200;
    int v3 = v1+vBorder;
    int v4 = v2-vBorder;
    int h1 = headingBottom + 2*vBorder;
    int h2 = h1 + fieldHeight;
    int y = 0;

    mySeaStateWidthField->setLabelWidth(55);
    mySeaStateHeightField->setLabelWidth(55);

    mySeaStateWidthPosField->setLabelWidth(10);
    mySeaStateHeightPosField->setLabelWidth(10);

    mySeaStateNumPoints->setLabelWidth(90);

    mySeaStatePositionFrame->setEdgeGeometry(v1,v2,h1+y,h1+3*(fieldHeight+vBorder)+y);
    y += fieldHeight+vBorder;
    mySeaStateWidthPosField->setEdgeGeometry(v3, v4, h1+y, h2+y);
    y += fieldHeight+vBorder;
    mySeaStateHeightPosField->setEdgeGeometry(v3, v4, h1+y, h2+y);
    y += fieldHeight+vBorder;
    mySeaStateSizeFrame->setEdgeGeometry(v1,v2,h1+y,h1+3*(fieldHeight+vBorder)+y);
    y += fieldHeight+vBorder;
    mySeaStateWidthField->setEdgeGeometry(v3, v4, h1+y, h2+y);
    y += fieldHeight+vBorder;
    mySeaStateHeightField->setEdgeGeometry(v3, v4, h1+y, h2+y);

    y=0;

    mySeaStateVisualizationFrame->setEdgeGeometry(v2+hBorder,v2+300+2*hBorder,h1+y,h1+5*(fieldHeight+vBorder)+y);
    y += fieldHeight+vBorder;
    mySeaStateNumPoints->setEdgeGeometry(v2+2*hBorder, v2+300, h1+y, h2+y);
    y += fieldHeight+vBorder;
    mySeaStateShowSolidToggle->setEdgeGeometry(v2+2*hBorder, v2+300, h1+y, h2+y);
    y += fieldHeight+vBorder;
    mySeaStateShowGridToggle->setEdgeGeometry(v2+2*hBorder, v2+300, h1+y, h2+y);
  }

  // Beam properties

  if(IAmShowingBeamPropData)
    {
      int glv1 = leftPartSeparator + hBorder;
      int glv2 = width;
      int glh1 = headingBottom + vBorder;
      int glh2 = height;

      myBeamPropTabs->setEdgeGeometry(glv1, glv2, glh1, glh2);
    }

  // Strain rosette

  if(IAmShowingStrainRosetteData)
    {
      int h1 = headingBottom + 2*vBorder;
      int h2 = h1 + fieldHeight;
      int h3 = h2 + vBorder;
      int h4 = h3 + fieldHeight;
      int h5 = h4 + vBorder;
      int h6 = h5 + 4*fieldHeight/5;
      int h7 = h6 + fieldHeight;
      int h8 = h7 + fieldHeight/5;
      int h9 = h8 + vBorder;
      int h10 = h9 + 4*fieldHeight/5;
      int h11 = h10 + fieldHeight;
      int h12 = h11 + vBorder;
      int h13 = h12 + fieldHeight;
      int h14 = h13 + vBorder;
      int h15 = h14 + fieldHeight;
      int h16 = h15 + fieldHeight/5;

      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = v1 + 2*hBorder;
      int v3 = v2 + this->getFontWidth("Angle offset [Deg]M ");
      int v4 = v2 + this->getFontWidth("xMChange SideMx");
      int v5 = v4 + myStrRosHeightField->myField->getFontWidth("0.12345678901e-200");
      int v6 = v5 + 2*hBorder;
      int v7 = v6 + 2*hBorder;
      int v8 = v7 + 2*hBorder;
      int v11 = v3 + myStrRosNodesField->myField->getFontWidth(" 5000000, 5000000, 5000000, 5000000 ");
      int v12 = v11 + 2*hBorder;
      int v14 = v12 + myStrRosEditNodesButton->getWidthHint();
      int v13 = v8 + this->getFontWidth("Poissons ratio ");

      int v16 = width - 2*hBorder;
      int v15 = v16 - 2*hBorder;
      int v10  = v15 - myStrRosEditDirButton->getWidthHint();
      int v9  = v10 - 2*hBorder;

      int bAdd = vBorder/2;

      myStrRosTypeLabel        ->setEdgeGeometry(v2, v3, h1, h2);
      myStrRosTypeMenu         ->setEdgeGeometry(v3, v11, h1, h2);
      myResetStartStrainsToggle->setEdgeGeometry(v12, v15, h1, h2);

      myStrRosNodesField       ->setEdgeGeometry(v2, v11, h3, h4);
      myStrRosNodesField->setLabelWidth(v3-v2);
      myStrRosNodesField->setLabelMargin(0);
      myStrRosEditNodesButton  ->setEdgeGeometry(v12, v14, h3-bAdd, h4+bAdd);

      myOrientationFrame       ->setEdgeGeometry(v1, v16, h5, h8);
      myStrRosAngleField       ->setEdgeGeometry(v2, v9, h6, h7);
      myStrRosAngleField->setLabelWidth(v3-v2);
      myStrRosAngleField->setLabelMargin(0);
      myStrRosEditDirButton    ->setEdgeGeometry(v10, v15, h6-bAdd, h7+bAdd);

      myLayerFrame->setEdgeGeometry(v1, v6, h9, h16);
      myStrRosUseFEHeightToggle->setEdgeGeometry(v2, v5, h10, h11);
      myStrRosHeightField      ->setEdgeGeometry(v2, v5, h12, h13);
      myStrRosHeightField->setLabelWidth(this->getFontWidth("HeightM"));
      myStrRosHeightField->setLabelMargin(0);
      myStrRosFlipZButton      ->setEdgeGeometry(v2, v4, h14-bAdd, h15+bAdd);

      myMaterialFrame->setEdgeGeometry(v7, v16, h9, h16);
      myStrRosUseFEMatToggle   ->setEdgeGeometry(v8, v15, h10, h11);
      myStrRosEmodField        ->setEdgeGeometry(v8, v15, h12, h13);
      myStrRosEmodField->setLabelWidth(v13-v8);
      myStrRosEmodField->setLabelMargin(0);
      myStrRosNuField          ->setEdgeGeometry(v8, v15, h14,h15);
      myStrRosNuField->setLabelWidth(v13-v8);
      myStrRosNuField->setLabelMargin(0);
    }

  // Element group

  if (IAmShowingGroupData)
    {
      int left = leftPartSeparator + 2*hBorder;
      int right = width;
      int snLeft = left + 2*hBorder;
      int snRight = snLeft + 3*(right-left)/5;
      int scfLeft = snRight + 3*hBorder;
      int scfRight = right - 2*hBorder;
      int top = headingBottom + 2*vBorder;
      int fatTop = top + fieldHeight;
      int fatBottom = fatTop + fieldHeight;
      int snTop = fatBottom + vBorder;
      int snBottom = snTop + fieldHeight;
      int bottom = snBottom + 3*vBorder;

      this->myFatigueFrame->setEdgeGeometry(left, right, top, bottom);
      this->myFatigueToggle->setEdgeGeometry(snLeft, snRight, fatTop, fatBottom);
#ifdef FT_HAS_GRAPHVIEW
      this->mySNSelector->setEdgeGeometry(snLeft, snRight, snTop, snBottom);
#endif
      this->myScfField->setEdgeGeometry(scfLeft, scfRight, snTop, snBottom);
    }

  // RAO vessel motion

  if (IAmShowingRAOData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = v1 + this->getFontWidth("Wave function  ");
      int v3 = width - this->myBrowseRAOFileButton->getWidthHint();
      int v4 = v3 - this->getFontWidth("Wave direction ");
      int v5 = v4 - 2*hBorder;
      int v6 = width;

      int h1 = headingBottom + 2*vBorder;
      int h2 = h1 + fieldHeight;
      int h3 = h2 + 2*vBorder;
      int h4 = h3 + fieldHeight;
      int h5 = h4 + 2*vBorder;
      int h6 = h5 + fieldHeight;

      myRAOFileLabel->       setEdgeGeometry(v1, v2, h1, h2);
      myRAOFileField->       setEdgeGeometry(v2, v3, h1, h2);
      myBrowseRAOFileButton->setEdgeGeometry(v3, v6, h1, h2);
      myWaveFuncLabel->      setEdgeGeometry(v1, v2, h3, h4);
      myWaveFuncField->      setEdgeGeometry(v2, v5, h3, h4);
      myWaveDirLabel->       setEdgeGeometry(v4, v3, h3, h4);
      myWaveDirMenu->        setEdgeGeometry(v3, v6, h3, h4);
      myMotionScaleLabel->   setEdgeGeometry(v1, v2, h5, h6);
      myMotionScaleField->   setEdgeGeometry(v2, v6, h5, h6);
    }

  // Simulation event

  if (IAmShowingEventData)
    {
      int left = leftPartSeparator + 2*hBorder;
      int right = width;
      int fieldTop = headingBottom + 2*vBorder;
      int fieldWidth = (right-left)/2;
      int fieldHeight = 25;
      int buttonWidth = this->mySelectEventButton->getWidthHint();
      this->myEventProbability->setEdgeGeometry(left, left+fieldWidth, fieldTop, fieldTop+fieldHeight);
      fieldTop += fieldHeight + 5*vBorder;
      this->mySelectEventButton->setEdgeGeometry(left, left+buttonWidth, fieldTop, fieldTop+fieldHeight);
      fieldTop += fieldHeight + 5*vBorder;
      this->myActiveEventLabel->setEdgeGeometry(left, right, fieldTop, fieldTop+2*fieldHeight);
    }

  // Subassembly

  if (IAmShowingSubAssData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = width;
      int v3 = v1 + 140;
      int h1 = headingBottom + 2*vBorder;
      int h2 = h1 + fieldHeight;
      int h3 = h2 + 3*vBorder;
      int h4 = h3 + fieldHeight + 2*vBorder;
      mySubassFileField->setEdgeGeometry(v1,v2,h1,h2);
      mySubassPosition->setEdgeGeometry(v3+hBorder,v2,h3,height);
      mySubassMassField->setEdgeGeometry(v1,v3,h3,h3+fieldHeight);
      mySubassCoGFrame->setEdgeGeometry(v1-hBorder,v3,h4,height);
      mySubassCoGField->setEdgeGeometry(v1,v3-hBorder,h4+fontHeigth,height-vBorder);
    }

  // Function properties

  if (IAmShowingFunctionData)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = width;
      int h1 = headingBottom + 2*vBorder;
      int h2 = height;

      myFunctionProperties->setEdgeGeometry(v1,v2,h1,h2);
    }

  // Control system properties

  if (IAmShowingCtrlData && myCtrlElementProperties)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = width;
      int h1 = headingBottom + 2*vBorder;
      int h2 = height;

      myCtrlElementProperties->setEdgeGeometry(v1,v2,h1,h2);
    }

  if (IAmShowingAnimationData && myAnimationDefine)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = width;
      int h1 = headingBottom + 2*vBorder;
      int h2 = height;

      myAnimationDefine->setEdgeGeometry(v1,v2,h1,h2);
    }

  if (IAmShowingGraphData && myGraphDefine)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = width;
      int h1 = headingBottom + 2*vBorder;
      int h2 = height;

      myGraphDefine->setEdgeGeometry(v1,v2,h1,h2);
    }

  if (IAmShowingCurveData && myCurveDefine)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = width;
      int h1 = headingBottom + 2*vBorder;
      int h2 = height;

      myCurveDefine->setEdgeGeometry(v1,v2,h1,h2);
    }

  if (IAmShowingExtCtrlSysData && myExtCtrlSysProperties)
    {
      int v1 = leftPartSeparator + 2*hBorder;
      int v2 = width;
      int h1 = headingBottom + 2*vBorder;
      int h2 = height;

      myExtCtrlSysProperties->setEdgeGeometry(v1,v2,h1,h2);
    }

  if (IAmShowingStartGuide > 0)
    {
      const float logoAspectRatio = 8.0f/4.13f;
      int cx2 = width*2/11;
      if (cx2 > height*logoAspectRatio) // wide container
        mySGLogoImage->setEdgeGeometry(0, cx2, 0, cx2/logoAspectRatio);
      else // tall container
        mySGLogoImage->setEdgeGeometry(cx2-height*logoAspectRatio, cx2, 0, height);

      mySGLogoBorderTop->setEdgeGeometry(0, cx2, 0, 3); // blue border above logo image
      mySGLogoBorderRight->setEdgeGeometry(cx2, cx2+4, 0, 27); // blue short-border on the right of the logo image
      mySGHeading->setEdgeGeometry(cx2+8, width-70, 3, 25);
      mySGFillWhite->setEdgeGeometry(cx2, width, 27, height);
      mySGBorderTop->setEdgeGeometry(cx2+1, width, 24, 27);
      mySGBorderRight->setEdgeGeometry(width-1, width, 27, height);
      mySGBorderBottom->setEdgeGeometry(cx2, width-1, height-1, height);
      mySGContentLabel->setEdgeGeometry(cx2+3, width, 27, height);

      // font size
      std::string text = mySGContentLabel->getLabel();
      size_t ipos = text.find("font-size:");
      if (ipos != std::string::npos) {
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
        if (text.find(buf) == ipos) {
          size_t jpos = text.find("pt;",ipos);
          text.replace(ipos,jpos-ipos+3,buf);
          mySGContentLabel->setLabel(text);
        }
      }

      // Note: We should enable resizing of this area when we have more content to show..
    }
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
  IAmShowingRotSpringCpl      = pv->showRotSpringCpl;
  IAmShowingTranSpringCpl     = pv->showTranSpringCpl;
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
    myDescriptionField->popUp();
    myTagField->popUp();
    myTopologyView->popUp();
    myTopologyView->setTree(pv->myTopology);
  }
  else
  {
    myDescriptionField->popDown();
    myTagField->popDown();
    myTopologyView->popDown();
  }

  // Reference Plane

  if (pv->showRefPlane)
  {
    myRefPlanePosition->setEditedObj(pv->myObjToPosition);
    myRefPlanePosition->popUp();
    myRefPlaneSizeFrame->popUp();
    myRefPlaneHeightField->popUp();
    myRefPlaneWidthField->popUp();
  }
  else
  {
    myRefPlanePosition->setEditedObj(NULL);
    myRefPlanePosition->popDown();
    myRefPlaneSizeFrame->popDown();
    myRefPlaneHeightField->popDown();
    myRefPlaneWidthField->popDown();
  }

  // Higher Pair

  if (pv->showHPRatio)
  {
    myHPRatioFrame->popUp();
    myHPRatioField->popUp();
  }
  else
  {
    myHPRatioFrame->popDown();
    myHPRatioField->popDown();
  }

  // Spring characteristics

  if (pv->showSpringChar)
    mySpringChar->popUp();
  else
    mySpringChar->popDown();

  // Axial Spring

  if (pv->showAxialSpring)
  {
    mySpringForce->buildDynamicWidgets(pv->myAxialSprForceValues);
    mySpringForce->popUp();
    mySpringDeflCalc->buildDynamicWidgets(pv->myAxialSpringDefCalcValues);
    mySpringDeflCalc->popUp();
  }
  else
  {
    mySpringForce->popDown();
    mySpringDeflCalc->popDown();
  }

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
    myLinkOriginSheet->setEditedObj(NULL);
    myGenericPartCGSheet->setEditedObj(NULL);
    myLinkTabs->popDown();
  }

  // Beam and Shaft

  if (pv->showBeamData || pv->showShaftData)
  {
    myShaftGeneralFrame->popUp();
    if (pv->myHideCrossSection) {
      myShaftCrossSectionDefLabel->popDown();
      myShaftCrossSectionDefField->popDown();
    }
    else {
      myShaftCrossSectionDefLabel->popUp();
      myShaftCrossSectionDefField->popUp();
    }
  }
  else
  {
    myShaftGeneralFrame->popDown();
    myShaftCrossSectionDefLabel->popDown();
    myShaftCrossSectionDefField->popDown();
  }

  bool showBladeOrShaft = pv->showBladeData || pv->showShaftData;
  bool showBeamstring = pv->showJacketData || pv->showSoilPileData || pv->showRiserData;
  if (showBladeOrShaft || showBeamstring || pv->showBeamData || pv->showUDEData)
  {
    myShaftSDFrame->popUp();
    myShaftSDMassField->popUp();
    myShaftSDStiffnessField->popUp();
    myShaftDPFrame->popUp();
    myShaftDPStiffnessField->popUp();
    myShaftDPMassField->popUp();
  }
  else
  {
    myShaftSDFrame->popDown();
    myShaftSDMassField->popDown();
    myShaftSDStiffnessField->popDown();
    myShaftDPFrame->popDown();
    myShaftDPStiffnessField->popDown();
    myShaftDPMassField->popDown();
  }

  if (pv->showBeamData)
  {
    myBeamOrientationFrame->popUp();
    for (FFuLabelField* f : myBeamLocalZField) f->popUp();
  }
  else
  {
    myBeamOrientationFrame->popDown();
    for (FFuLabelField* f : myBeamLocalZField) f->popDown();
  }

  if (showBladeOrShaft)
  {
    myShaftNoteALabel->popUp();
    myShaftNoteBLabel->popUp();
    myShaftNoteCLabel->popUp();
  }
  else
  {
    myShaftNoteALabel->popDown();
    myShaftNoteBLabel->popDown();
    myShaftNoteCLabel->popDown();
  }

  // Turbine, Nacelle, Gearbox, etc. (shared fields) :

  bool showSubassMassCog = pv->showSubassemblyData && pv->showSubassPos;
  if (showSubassMassCog ||
      pv->showTurbineData ||
      pv->showTowerData ||
      pv->showNacelleData ||
      pv->showGeneratorData ||
      pv->showGearboxData ||
      pv->showRotorData ||
      pv->showShaftData ||
      pv->showBladeData ||
      pv->showRiserData ||
      pv->showJacketData ||
      pv->showSoilPileData ||
      pv->showBeamData ||
      pv->showUDEData)
    mySubassMassField->popUp();
  else
    mySubassMassField->popDown();

  if (showSubassMassCog ||
      pv->showTurbineData ||
      pv->showTowerData ||
      pv->showNacelleData ||
      pv->showGeneratorData ||
      pv->showGearboxData ||
      pv->showRotorData ||
      pv->showJacketData)
  {
    mySubassCoGFrame->popUp();
    mySubassCoGField->popUp();
  }
  else
  {
    mySubassCoGFrame->popDown();
    mySubassCoGField->popDown();
  }

  if (pv->showShaftData ||
      pv->showBladeData ||
      pv->showRiserData ||
      pv->showSoilPileData ||
      pv->showBeamData)
    mySubassLengthField->popUp();
  else
    mySubassLengthField->popDown();

  if (showBladeOrShaft || pv->showTowerData || pv->showBeamData)
    myVisualize3DButton->popUp();
  else
    myVisualize3DButton->popDown();

  if (pv->showBeamData)
  {
    myVisualize3DStartAngleField->popUp();
    myVisualize3DStopAngleField->popUp();
  }
  else
  {
    myVisualize3DStartAngleField->popDown();
    myVisualize3DStopAngleField->popDown();
  }

  if (pv->showTurbineData)
  {
    myTurbineWindRefFrame->popUp();
    myTurbineWindRefTriadDefLabel->popUp();
    myTurbineWindRefTriadDefField->popUp();
    myTurbineWindVertOffsetLabel->popUp();
    myTurbineWindVertOffsetField->popUp();
    myTurbineAdvTopologyFrame->popUp();
    myTurbineYawPointTriadDefLabel->popUp();
    myTurbineYawPointTriadDefField->popUp();
    myTurbineHubApexTriadDefLabel->popUp();
    myTurbineHubApexTriadDefField->popUp();
    myTurbineHubPartDefLabel->popUp();
    myTurbineHubPartDefField->popUp();
    myTurbineFirstBearingDefLabel->popUp();
    myTurbineFirstBearingDefField->popUp();
  }
  else
  {
    myTurbineWindRefFrame->popDown();
    myTurbineWindRefTriadDefLabel->popDown();
    myTurbineWindRefTriadDefField->popDown();
    myTurbineWindVertOffsetLabel->popDown();
    myTurbineWindVertOffsetField->popDown();
    myTurbineAdvTopologyFrame->popDown();
    myTurbineYawPointTriadDefLabel->popDown();
    myTurbineYawPointTriadDefField->popDown();
    myTurbineHubApexTriadDefLabel->popDown();
    myTurbineHubApexTriadDefField->popDown();
    myTurbineHubPartDefLabel->popDown();
    myTurbineHubPartDefField->popDown();
    myTurbineFirstBearingDefLabel->popDown();
    myTurbineFirstBearingDefField->popDown();
  }

  if (pv->showGeneratorData)
  {
    myGeneratorTorqueRadioBtn->popUp();
    myGeneratorVelocityRadioBtn->popUp();
    myGeneratorTorqueField->popUp();
    myGeneratorVelocityField->popUp();
  }
  else
  {
    myGeneratorTorqueRadioBtn->popDown();
    myGeneratorVelocityRadioBtn->popDown();
    myGeneratorTorqueField->popDown();
    myGeneratorVelocityField->popDown();
  }

  if (pv->showBladeData)
  {
    myBladePitchControlLabel->popUp();
    myBladePitchControlField->popUp();
    myBladeFixedPitchToggle->popUp();
    myBladeIceFrame->popUp();
    myBladeIceLayerToggle->popUp();
    myBladeIceThicknessField->popUp();
  }
  else
  {
    myBladePitchControlLabel->popDown();
    myBladePitchControlField->popDown();
    myBladeFixedPitchToggle->popDown();
    myBladeIceFrame->popDown();
    myBladeIceLayerToggle->popDown();
    myBladeIceThicknessField->popDown();
  }

  // Riser, Soil Pile or Jacket :

  if (pv->showRiserData)
  {
    myRiserMudButton->setLabel("Internal Liquid");
    myRiserMudDensityField->setLabel("Mud density");

    myRiserGeneralFrame->popUp();
    myRiserInternalToDefLabel->popUp();
    myRiserInternalToDefField->popUp();
    myRiserVisualize3DButton->popUp();
    myRiserVisualize3DStartAngleField->popUp();
    myRiserVisualize3DStopAngleField->popUp();
    myRiserMudFrame->popUp();
    myRiserMudButton->popUp();
    myRiserMudDensityField->popUp();
    myRiserMudLevelField->popUp();
  }
  else if (pv->showSoilPileData)
  {
    myRiserMudButton->setLabel("Internal Soil");
    myRiserMudDensityField->setLabel("Soil density");

    myRiserGeneralFrame->popUp();
    myRiserVisualize3DButton->popUp();
    myRiserVisualize3DStartAngleField->popUp();
    myRiserVisualize3DStopAngleField->popUp();
    myRiserInternalToDefLabel->popDown();
    myRiserInternalToDefField->popDown();
    myRiserMudFrame->popUp();
    myRiserMudButton->popUp();
    myRiserMudDensityField->popUp();
    myRiserMudLevelField->popDown();
  }
  else if (pv->showJacketData)
  {
    myRiserGeneralFrame->popUp();
    myRiserVisualize3DButton->popUp();
    myRiserVisualize3DStartAngleField->popDown();
    myRiserVisualize3DStopAngleField->popDown();
    myRiserInternalToDefLabel->popDown();
    myRiserInternalToDefField->popDown();
    myRiserMudFrame->popDown();
    myRiserMudButton->popDown();
    myRiserMudDensityField->popDown();
    myRiserMudLevelField->popDown();
  }
  else
  {
    myRiserGeneralFrame->popDown();
    myRiserInternalToDefLabel->popDown();
    myRiserInternalToDefField->popDown();
    myRiserVisualize3DButton->popDown();
    myRiserVisualize3DStartAngleField->popDown();
    myRiserVisualize3DStopAngleField->popDown();
    myRiserMudFrame->popDown();
    myRiserMudButton->popDown();
    myRiserMudDensityField->popDown();
    myRiserMudLevelField->popDown();
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
  if (pv->showFriction)
    myJointSummary->myFriction->setQuery(pv->myFrictionQuery);

  // TZ dof toggle

  myJointSummary->showTzToggle(pv->showDOF_TZ_Toggle);

  // Fix/Free all DOFs

  myJointSummary->showFixFreeAll(pv->showJointData == 2);

  // Joint rotation formulation

  myJointAdvancedTab->showRotFormulation(pv->showRotFormulation);
  if (pv->showRotFormulation)
  {
    myJointAdvancedTab->myRotFormulationMenu->setOptions(pv->myRotFormulationTypes);
    myJointAdvancedTab->myRotSequenceMenu->setOptions(pv->myRotSequenceTypes);
  }

  // Joint spring coupling

  if (pv->showTranSpringCpl)
    myJointAdvancedTab->showSpringCpl(true);
  else if (pv->showRotSpringCpl)
    myJointAdvancedTab->showRotSpringCpl();
  else
    myJointAdvancedTab->showSpringCpl(false);

  if (pv->showRotSpringCpl)
    myJointAdvancedTab->myRotSpringCplMenu->setOptions(pv->mySpringCplTypes);
  if (pv->showTranSpringCpl)
    myJointAdvancedTab->myTranSpringCplMenu->setOptions(pv->mySpringCplTypes);

  // Joints :

  if (pv->showJointData)
  {
    myJointSummary->mySummaryTable->setNumberRows(pv->myJointVals.size());
    myJointSummary->myAddBCLabel->popDown();
    myJointSummary->myDefDamperLabel->popDown();

    myJointPosition->setEditedObj(pv->myObjToPosition);

    myJointTabs->popDown();
    if (pv->myObjToPosition) {
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
        myJointTabs->addTabPage(jdof, label[pv->myJointVals[jv].myDofNo]);
        myJointSummary->setRowLabel(jv, pv->myJointVals[jv], label);
        jdof->springDC->buildDynamicWidgets(pv->myJointVals[jv].mySpringDCVals);
        jdof->springFS->buildDynamicWidgets(pv->myJointVals[jv].mySpringFSVals);
        jdof->damperFS->buildDynamicWidgets(pv->myJointVals[jv].myDamperFCVals);
        jdof->dofNo = pv->myJointVals[jv++].myDofNo;
      }
      else
        jdof->dofNo = -1;

    if (pv->showRotSpringCpl || pv->showRotFormulation)
      myJointTabs->addTabPage(myJointAdvancedTab, "Advanced");

    if (!pv->myJointVals.empty())
      myJointTabs->addTabPage(myJointResults, "Results");

    myJointTabs->setCurrentTab(mySelectedJointTab);
    this->onJointTabSelected(0);
    myJointTabs->popUp();
  }
  else
  {
    if (myJointTabs->isPoppedUp())
      mySelectedJointTab = myJointTabs->getCurrentTabName();
    myJointTabs->popDown();
    myJointPosition->setEditedObj(NULL);
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

    myTriadPosition->setEditedObj(pv->myObjToPosition);

    if (pv->myTriadVals.empty()) {
      myTriadSummary->myMassFrame->popDown();
      myTriadSummary->myMassField->popDown();
      myTriadSummary->myIxField->popDown();
      myTriadSummary->myIyField->popDown();
      myTriadSummary->myIzField->popDown();
      myTriadSummary->mySysDirFrame->popDown();
      myTriadSummary->mySysDirMenu->popDown();
      myTriadSummary->mySummaryTable->popDown();
    }
    else {
      myTriadSummary->myMassFrame->popUp();
      myTriadSummary->myMassField->popUp();
      if (pv->myTriadVals.size() > 3) {
	myTriadSummary->myIxField->popUp();
	myTriadSummary->myIyField->popUp();
	myTriadSummary->myIzField->popUp();
      }
      else {
	myTriadSummary->myIxField->popDown();
	myTriadSummary->myIyField->popDown();
	myTriadSummary->myIzField->popDown();
      }
      myTriadSummary->mySysDirFrame->popUp();
      myTriadSummary->mySysDirMenu->popUp();
      myTriadSummary->mySummaryTable->popUp();
    }
    if (pv->myTriadConnector > 1) {
      myTriadSummary->myConnectorFrame->popUp();
      myTriadSummary->myConnectorMenu->popUp();
    }
    else {
      myTriadSummary->myConnectorFrame->popDown();
      myTriadSummary->myConnectorMenu->popDown();
    }

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
    myTriadPosition->setEditedObj(NULL);
  }

  // Load

  if (pv->showLoadData)
  {
    myMagnitudeLabel->popUp();
    myLoadMagnitude->popUp();
    myAttackPointFrame->popUp();
    myDirectionFrame->popUp();
    myAttackPointEditor->popUp();
    myFromPointEditor->popUp();
    myToPointEditor->popUp();
    myFromPointLabel->popUp();
    myToPointLabel->popUp();
  }
  else
  {
    myMagnitudeLabel->popDown();
    myLoadMagnitude->popDown();
    myAttackPointFrame->popDown();
    myDirectionFrame->popDown();
    myAttackPointEditor->popDown();
    myFromPointEditor->popDown();
    myToPointEditor->popDown();
    myFromPointLabel->popDown();
    myToPointLabel->popDown();
  }

  // Generic DB object

  if (pv->showGenDBObj)
  {
    myGenDBObjTypeLabel->popUp();
    myGenDBObjTypeField->popUp();
    myGenDBObjDefLabel->popUp();
    myGenDBObjDefField->popUp();
  }
  else
  {
    myGenDBObjTypeLabel->popDown();
    myGenDBObjTypeField->popDown();
    myGenDBObjDefLabel->popDown();
    myGenDBObjDefField->popDown();
  }

  // File reference

  if (pv->showFileReference)
    myFileReferenceBrowseField->popUp();
  else
    myFileReferenceBrowseField->popDown();

  // Tire

  if (pv->showTireData)
  {
    myTireDataFileLabel->popUp();
    myTireDataFileField->popUp();
    myTireDataFileField->setQuery(pv->myTireDataFileRefQuery);
    myBrowseTireFileButton->popUp();
    myRoadLabel->popUp();
    myRoadField->popUp();
    myRoadField->setQuery(pv->myRoadQuery);
    myTireModelTypeLabel->popUp();
    myTireModelMenu->popUp();
    myTireModelMenu->setOptions(pv->myTireTypes);
    mySpindelOffsetField->popUp();
  }
  else
  {
    myTireDataFileLabel->popDown();
    myTireDataFileField->popDown();
    myBrowseTireFileButton->popDown();
    myRoadLabel->popDown();
    myRoadField->popDown();
    myTireModelTypeLabel->popDown();
    myTireModelMenu->popDown();
    mySpindelOffsetField->popDown();
  }

  // Road

  if (pv->showRoadData)
  {
    myUseFuncRoadRadio->popUp();
    myRoadFuncLabel->popUp();
    myRoadFuncField->popUp();
    myRoadFuncField->setQuery(pv->myRoadFunctionQuery);
    myRoadZShiftField->popUp();
    myRoadXOffsetField->popUp();
    myRoadZRotationField->popUp();
    myUseFileRoadRadio->popUp();
    myRoadDataFileLabel->popUp();
    myRoadDataFileField->popUp();
    myRoadDataFileField->setQuery(pv->myRoadDataFileRefQuery);
    myBrowseRoadFileButton->popUp();
  }
  else
  {
    myUseFuncRoadRadio->popDown();
    myRoadFuncLabel->popDown();
    myRoadFuncField->popDown();
    myRoadZShiftField->popDown();
    myRoadXOffsetField->popDown();
    myRoadZRotationField->popDown();
    myUseFileRoadRadio->popDown();
    myRoadDataFileLabel->popDown();
    myRoadDataFileField->popDown();
    myUseFileRoadRadio->popDown();
    myRoadDataFileLabel->popDown();
    myRoadDataFileField->popDown();
    myBrowseRoadFileButton->popDown();
  }

  // Material properties

  if (pv->showMatPropData)
  {
    myMatPropRhoField->popUp();
    myMatPropEField->popUp();
    myMatPropNuField->popUp();
    myMatPropGField->popUp();
  }
  else
  {
    myMatPropRhoField->popDown();
    myMatPropEField->popDown();
    myMatPropNuField->popDown();
    myMatPropGField->popDown();
  }

  // Sea State
  if (pv->showSeaStateData)
  {
    mySeaStateSizeFrame->popUp();
    mySeaStatePositionFrame->popUp();
    mySeaStateVisualizationFrame->popUp();
    mySeaStateWidthField->popUp();
    mySeaStateHeightField->popUp();
    mySeaStateWidthPosField->popUp();
    mySeaStateHeightPosField->popUp();
    mySeaStateNumPoints->popUp();
    mySeaStateShowGridToggle->popUp();
    mySeaStateShowSolidToggle->popUp();
  }
  else {
    mySeaStateSizeFrame->popDown();
    mySeaStatePositionFrame->popDown();
    mySeaStateVisualizationFrame->popDown();
    mySeaStateWidthField->popDown();
    mySeaStateHeightField->popDown();
    mySeaStateWidthPosField->popDown();
    mySeaStateHeightPosField->popDown();
    mySeaStateNumPoints->popDown();
    mySeaStateShowGridToggle->popDown();
    mySeaStateShowSolidToggle->popDown();
  }

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
    myStrRosTypeLabel->popUp();
    myStrRosTypeMenu->popUp();
    myStrRosTypeMenu->setOptions(pv->myStrainRosetteTypes);
    myStrRosNodesField->popUp();
    myStrRosEditNodesButton->popUp();
    myStrRosAngleField->popUp();
    myStrRosEditDirButton->popUp();
    myStrRosHeightField->popUp();
    myStrRosUseFEHeightToggle->popUp();
    myStrRosFlipZButton->popUp();
    myStrRosEmodField->popUp();
    myStrRosNuField->popUp();
    myStrRosUseFEMatToggle->popUp();
    myResetStartStrainsToggle->popUp();
    myMaterialFrame->popUp();
    myLayerFrame->popUp();
    myOrientationFrame->popUp();

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
  {
    myStrRosTypeLabel->popDown();
    myStrRosTypeMenu->popDown();
    myStrRosNodesField->popDown();
    myStrRosEditNodesButton->popDown();
    myStrRosAngleField->popDown();
    myStrRosEditDirButton->popDown();
    myStrRosHeightField->popDown();
    myStrRosUseFEHeightToggle->popDown();
    myStrRosFlipZButton->popDown();
    myStrRosEmodField->popDown();
    myStrRosNuField->popDown();
    myStrRosUseFEMatToggle->popDown();
    myResetStartStrainsToggle->popDown();
    myMaterialFrame->popDown();
    myLayerFrame->popDown();
    myOrientationFrame->popDown();
  }

  // Element group

  if (pv->showGroupData)
  {
    myFatigueFrame->popUp();
    myFatigueToggle->popUp();
#ifdef FT_HAS_GRAPHVIEW
    mySNSelector->popUp();
#endif
    myScfField->popUp();
  }
  else
  {
    myFatigueFrame->popDown();
    myFatigueToggle->popDown();
#ifdef FT_HAS_GRAPHVIEW
    mySNSelector->popDown();
#endif
    myScfField->popDown();
  }

  // RAO vessel motion

  if (pv->showRAOData)
  {
    myRAOFileLabel->popUp();
    myRAOFileField->popUp();
    myRAOFileField->setQuery(pv->myRAOFileRefQuery);
    myBrowseRAOFileButton->popUp();
    myWaveFuncLabel->popUp();
    myWaveFuncField->popUp();
    myWaveFuncField->setQuery(pv->myWaveFunctionQuery);
    myWaveDirLabel->popUp();
    myWaveDirMenu->popUp();
    myMotionScaleLabel->popUp();
    myMotionScaleField->popUp();
    myMotionScaleField->setQuery(pv->myMotionScaleQuery);
  }
  else
  {
    myRAOFileLabel->popDown();
    myRAOFileField->popDown();
    myBrowseRAOFileButton->popDown();
    myWaveFuncLabel->popDown();
    myWaveFuncField->popDown();
    myWaveDirLabel->popDown();
    myWaveDirMenu->popDown();
    myMotionScaleLabel->popDown();
    myMotionScaleField->popDown();
  }

  // Simulation event

  if (pv->showSimEventData)
  {
    myEventProbability->popUp();
    mySelectEventButton->popUp();
  }
  else
  {
    myEventProbability->popDown();
    mySelectEventButton->popDown();
    myActiveEventLabel->popDown();
  }

  // Subassembly

  if (pv->showSubassemblyData || pv->showRiserData ||
      pv->showJacketData || pv->showSoilPileData)
    mySubassFileField->popUp();
  else
    mySubassFileField->popDown();

  if (pv->showSubassPos)
  {
    mySubassPosition->setEditedObj(pv->myObjToPosition);
    mySubassPosition->popUp();
  }
  else
  {
    mySubassPosition->setEditedObj(NULL);
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
    myTypeField->popDown();
    myIdField->popDown();
    mySGLogoImage->popUp();
    mySGLogoBorderTop->popUp();
    mySGLogoBorderRight->popUp();
    mySGHeading->popUp();
    mySGFillWhite->popUp();
    mySGBorderTop->popUp();
    mySGBorderRight->popUp();
    mySGBorderBottom->popUp();
    mySGContentLabel->popUp();
  }
  else
  {
    mySGLogoImage->popDown();
    mySGLogoBorderTop->popDown();
    mySGLogoBorderRight->popDown();
    mySGHeading->popDown();
    mySGFillWhite->popDown();
    mySGBorderTop->popDown();
    mySGBorderRight->popDown();
    mySGBorderBottom->popDown();
    mySGContentLabel->popDown();
    myTypeField->popUp();
    myIdField->popUp();
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
    myLinkOriginSheet->setEditedObj(dynamic_cast<FmIsPositionedBase*>(pv->myObjToPosition));
    myLinkFEnodeSheet->setViewedObj(dynamic_cast<FmIsPositionedBase*>(pv->myObjToPosition));
    myLinkFEnodeSheet->setValues(pv->myLinkValues);
    myLinkRedOptSheet->setValues(pv->myLinkValues);
    myLinkLoadSheet->setValues(pv->myLinkValues);
    myGenericPartCGSheet->setEditedObj(dynamic_cast<FmIsPositionedBase*>(pv->myObjToPosition));
    myGenericPartCGSheet->setValues(pv->myLinkValues);
    myGenericPartMassSheet->setValues(pv->myLinkValues);
    myGenericPartStiffSheet->setValues(pv->myLinkValues);
    myHydrodynamicsSheet->setValues(pv->myLinkValues);
    myMeshingSheet->setValues(pv->myLinkValues);
    myAdvancedLinkOptsSheet->setValues(pv->myLinkValues);
    myNonlinearLinkOptsSheet->setValues(pv->myLinkValues);
  }

  // Beam and Shaft :

  if (pv->showBeamData || pv->showShaftData)
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
    if (pv->myBlankFieldsFlags & 0x0002)
      myShaftSDMassField->setValue("");
    else
      myShaftSDMassField->setValue(pv->myMassPropDamp);
    if (pv->myBlankFieldsFlags & 0x0004)
      myShaftSDStiffnessField->setValue("");
    else
      myShaftSDStiffnessField->setValue(pv->myStifPropDamp);
    if (pv->myBlankFieldsFlags & 0x0008)
      myShaftDPMassField->setValue("");
    else
      myShaftDPMassField->setValue(pv->myScaleMass);
    if (pv->myBlankFieldsFlags & 0x0010)
      myShaftDPStiffnessField->setValue("");
    else
      myShaftDPStiffnessField->setValue(pv->myScaleStiff);
  }
  else if (pv->showUDEData)
  {
    mySubassMassField->setValue(pv->myTotalMass);
    myShaftSDMassField->setValue(pv->myMassPropDamp);
    myShaftSDStiffnessField->setValue(pv->myStifPropDamp);
    myShaftDPMassField->setValue(pv->myScaleMass);
    myShaftDPStiffnessField->setValue(pv->myScaleStiff);
 }

  if (pv->showBeamData) {
    for (int i = 0; i < 3; i++)
      myBeamLocalZField[i]->setValue(pv->myOrientation[i]);
    myVisualize3DButton->useUITristate(false);
    myVisualize3DButton->setValue(pv->myVisualize3D);
    myVisualize3DButton->setSensitivity(true);
    myVisualize3DStartAngleField->myField->setValue(pv->myVisualize3DAngles.first);
    myVisualize3DStopAngleField->myField->setValue(pv->myVisualize3DAngles.second);
  }
  else if (pv->showShaftData || pv->showBladeData || pv->showTowerData) {
    myVisualize3DButton->useUITristate(true);
    myVisualize3DButton->setUITristateValue(pv->myVisualize3Dts);
    myVisualize3DButton->setSensitivity(pv->myVisualize3DEnabled);
  }

  // Turbine :

  if (pv->showTurbineData)
  {
    mySubassMassField->setValue(pv->myTotalMass);
    mySubassCoGField->setValue(pv->myCoG);
    mySubassCoGField->setGlobal();
    mySelectedSubass = pv->myObjToPosition;

    myTurbineWindRefTriadDefField->setQuery(pv->myTurbineWindRefTriadDefQuery);
    myTurbineWindRefTriadDefField->setSelectedRef(pv->myTurbineWindRefTriadDefSelected);
    myTurbineWindVertOffsetField->setValue(pv->myTurbineWindVertOffset);
    myTurbineYawPointTriadDefField->setQuery(pv->myTurbineYawPointTriadDefQuery);
    myTurbineYawPointTriadDefField->setSelectedRef(pv->myTurbineYawPointTriadDefSelected);
    myTurbineHubApexTriadDefField->setQuery(pv->myTurbineHubApexTriadDefQuery);
    myTurbineHubApexTriadDefField->setSelectedRef(pv->myTurbineHubApexTriadDefSelected);
    myTurbineHubPartDefField->setQuery(pv->myTurbineHubPartDefQuery);
    myTurbineHubPartDefField->setSelectedRef(pv->myTurbineHubPartDefSelected);
    myTurbineFirstBearingDefField->setQuery(pv->myTurbineFirstBearingDefQuery);
    myTurbineFirstBearingDefField->setSelectedRef(pv->myTurbineFirstBearingDefSelected);
  }
  else
    mySelectedSubass = NULL;

  // Tower, Nacelle, Gearbox or Rotor :

  if (pv->showTowerData || pv->showNacelleData || pv->showGearboxData || pv->showRotorData)
  {
    mySubassMassField->setValue(pv->myTotalMass);
    mySubassCoGField->setValue(pv->myCoG);
    mySubassCoGField->setGlobal();
    mySelectedSubass = pv->myObjToPosition;
  }

  // Generator :

  if (pv->showGeneratorData)
  {
    mySubassMassField->setValue(pv->myTotalMass);
    mySubassCoGField->setValue(pv->myCoG);
    mySubassCoGField->setGlobal();
    mySelectedSubass = pv->myObjToPosition;

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

    // These fields are reused from Shaft
    if (pv->myBlankFieldsFlags & 0x0002)
      myShaftSDMassField->setValue("");
    else
      myShaftSDMassField->setValue(pv->myMassPropDamp);
    if (pv->myBlankFieldsFlags & 0x0004)
      myShaftSDStiffnessField->setValue("");
    else
      myShaftSDStiffnessField->setValue(pv->myStifPropDamp);
    if (pv->myBlankFieldsFlags & 0x0008)
      myShaftDPMassField->setValue("");
    else
      myShaftDPMassField->setValue(pv->myScaleMass);
    if (pv->myBlankFieldsFlags & 0x0010)
      myShaftDPStiffnessField->setValue("");
    else
      myShaftDPStiffnessField->setValue(pv->myScaleStiff);
  }

  // Riser, Jacket or Soil Pile :

  if (pv->showRiserData || pv->showJacketData || pv->showSoilPileData)
  {
    mySelectedSubass = pv->myObjToPosition;
    mySubassMassField->setValue(pv->myTotalMass);
    if (pv->showJacketData) {
      mySubassCoGField->setValue(pv->myCoG);
      mySubassCoGField->setGlobal();
    }
    else
      mySubassLengthField->setValue(pv->myTotalLength);
    myRiserVisualize3DButton->setUITristateValue(pv->myVisualize3Dts);
    myRiserVisualize3DStartAngleField->myField->setValue(pv->myVisualize3DAngles.first);
    myRiserVisualize3DStopAngleField->myField->setValue(pv->myVisualize3DAngles.second);
    myRiserMudButton->setValue(pv->myIntFluid);
    myRiserMudDensityField->setValue(pv->myIntFluidDensity);
    myRiserMudDensityField->setSensitivity(IAmSensitive && pv->myIntFluid);
    myRiserMudLevelField->setValue(pv->myIntFluidLevel);
    myRiserMudLevelField->setSensitivity(IAmSensitive && pv->myIntFluid);

    // These fields are reused from Shaft
    if (pv->myBlankFieldsFlags & 0x0002)
      myShaftSDMassField->setValue("");
    else
      myShaftSDMassField->setValue(pv->myMassPropDamp);
    if (pv->myBlankFieldsFlags & 0x0004)
      myShaftSDStiffnessField->setValue("");
    else
      myShaftSDStiffnessField->setValue(pv->myStifPropDamp);
    if (pv->myBlankFieldsFlags & 0x0008)
      myShaftDPMassField->setValue("");
    else
      myShaftDPMassField->setValue(pv->myScaleMass);
    if (pv->myBlankFieldsFlags & 0x0010)
      myShaftDPStiffnessField->setValue("");
    else
      myShaftDPStiffnessField->setValue(pv->myScaleStiff);
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

    myJointSummary->mySummaryTable->stretchColWidth(-1,true);
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
    myJointAdvancedTab->myRotExplainLabel->setLabel(FmJointBase::getRotExplain(pv->mySelectedRotFormulation,
									       pv->mySelectedRotSequence));
  }

  // Spring Coupling

  if (pv->showRotSpringCpl)
    myJointAdvancedTab->myRotSpringCplMenu->selectOption(pv->mySelectedRotSpringCpl);
  if (pv->showTranSpringCpl)
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
      if (pv->myTriadVals.empty())
	myTriadSummary->myTriadLabel->setLabel("This is a grounded master triad");
      else
	myTriadSummary->myTriadLabel->setLabel("This is a master triad");
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

    myJointSummary->mySummaryTable->stretchColWidth(-1,true);
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
    mySpindelOffsetField->setValue(pv->mySpindelTriadOffset);

    // Set read-only if no tire license available
    if (!FapLicenseManager::checkTireLicense()) {
      myTireDataFileField->setSensitivity(false);
      myBrowseTireFileButton->setSensitivity(false);
      myRoadField->setSensitivity(false);
      myTireModelMenu->setSensitivity(false);
      mySpindelOffsetField->setSensitivity(false);
    }
  }

  // Road

  if (pv->showRoadData)
  {
    myUseFuncRoadRadio->setValue(!pv->iAmUsingExtRoadData);
    myRoadFuncField->setSelectedRef(pv->mySelectedRoadFunc);
    myRoadZShiftField->setValue(pv->myRoadZShift);
    myRoadXOffsetField->setValue(pv->myRoadXOffset);
    myRoadZRotationField->setValue(pv->myRoadZRotation);

    myUseFileRoadRadio->setValue(pv->iAmUsingExtRoadData);
    myRoadDataFileField->setSelectedRef(pv->mySelectedRoadDataFileRef);
    myRoadDataFileField->setText(pv->myRoadDataFileName);

    // Set read-only if no tire license available
    if (!FapLicenseManager::checkTireLicense()) {
      myUseFuncRoadRadio->setSensitivity(false);
      myRoadFuncField->setSensitivity(false);
      myRoadZShiftField->setSensitivity(false);
      myRoadXOffsetField->setSensitivity(false);
      myRoadZRotationField->setSensitivity(false);

      myUseFileRoadRadio->setSensitivity(false);
      myRoadDataFileField->setSensitivity(false);
      myBrowseRoadFileButton->setSensitivity(false);
    }
    else if (myUseFuncRoadRadio->getSensitivity()) {
      myRoadFuncField->setSensitivity(!pv->iAmUsingExtRoadData);
      myRoadZShiftField->setSensitivity(!pv->iAmUsingExtRoadData);
      myRoadXOffsetField->setSensitivity(!pv->iAmUsingExtRoadData);
      myRoadZRotationField->setSensitivity(!pv->iAmUsingExtRoadData);

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

  // Sea State

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
    myBeamPropHydro->myBeamHydroToggleButton->setValue(pv->myBeamHydroToggle);
    myBeamPropHydro->onBeamHydroToggled(IAmSensitive && pv->myBeamHydroToggle);
    myBeamPropHydro->setValues(pv->myHydroProp);
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
    mySubassMassField->setValue(pv->myTotalMass);
    mySubassCoGField->setValue(pv->myCoG);
    mySubassCoGField->setGlobal();
    mySelectedSubass = pv->myObjToPosition;
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
      pv->myRefPlaneHeight = myRefPlaneHeightField->getValue();
      pv->myRefPlaneWidth  = myRefPlaneWidthField->getValue();
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

  // User-defined element, Beam and Shaft

  if (IAmShowingUDEData || IAmShowingBeamData || IAmShowingShaftData)
    {
      pv->myMassPropDamp = myShaftSDMassField->getValue();
      pv->myStifPropDamp = myShaftSDStiffnessField->getValue();
      pv->myScaleMass    = myShaftDPMassField->getValue();
      pv->myScaleStiff   = myShaftDPStiffnessField->getValue();
    }

  if (IAmShowingBeamData || IAmShowingShaftData)
    pv->mySelectedCS = myShaftCrossSectionDefField->getSelectedRef();

  if (IAmShowingShaftData)
    {
      // Get blank fields
      pv->myBlankFieldsFlags = 0x0000;
      if (myShaftCrossSectionDefField->getSelectedRef() == NULL)
        pv->myBlankFieldsFlags |= 0x0001;
      if (myShaftSDMassField->getText().length() == 0)
        pv->myBlankFieldsFlags |= 0x0002;
      if (myShaftSDStiffnessField->getText().length() == 0)
        pv->myBlankFieldsFlags |= 0x0004;
      if (myShaftDPMassField->getText().length() == 0)
        pv->myBlankFieldsFlags |= 0x0008;
      if (myShaftDPStiffnessField->getText().length() == 0)
        pv->myBlankFieldsFlags |= 0x0010;
    }

  if (IAmShowingBeamData) {
    for (int i = 0; i < 3; i++)
      pv->myOrientation[i] = myBeamLocalZField[i]->getValue();
    pv->myVisualize3D = myVisualize3DButton->getValue();
    pv->myVisualize3DAngles.first = myVisualize3DStartAngleField->myField->getIntFromText();
    pv->myVisualize3DAngles.second = myVisualize3DStopAngleField->myField->getIntFromText();
  }
  else if (IAmShowingShaftData || IAmShowingBladeData || IAmShowingTowerData)
    pv->myVisualize3Dts = myVisualize3DButton->getTristateValue();

  // Turbine :

  if (IAmShowingTurbineData)
  {
    pv->myTurbineWindRefTriadDefSelected   = myTurbineWindRefTriadDefField->getSelectedRef();
    pv->myTurbineWindVertOffset            = myTurbineWindVertOffsetField->getDouble();
    pv->myTurbineYawPointTriadDefSelected  = myTurbineYawPointTriadDefField->getSelectedRef();
    pv->myTurbineHubApexTriadDefSelected   = myTurbineHubApexTriadDefField->getSelectedRef();
    pv->myTurbineHubPartDefSelected        = myTurbineHubPartDefField->getSelectedRef();
    pv->myTurbineFirstBearingDefSelected   = myTurbineFirstBearingDefField->getSelectedRef();
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

    // These fields are reused from Shaft
    pv->myMassPropDamp = myShaftSDMassField->getValue();
    pv->myStifPropDamp = myShaftSDStiffnessField->getValue();
    pv->myScaleMass = myShaftDPMassField->getValue();
    pv->myScaleStiff = myShaftDPStiffnessField->getValue();
    // flags
    pv->myBlankFieldsFlags = 0x0000;
    if (myShaftSDMassField->getText().length() == 0)
      pv->myBlankFieldsFlags |= 0x0002;
    if (myShaftSDStiffnessField->getText().length() == 0)
      pv->myBlankFieldsFlags |= 0x0004;
    if (myShaftDPMassField->getText().length() == 0)
      pv->myBlankFieldsFlags |= 0x0008;
    if (myShaftDPStiffnessField->getText().length() == 0)
      pv->myBlankFieldsFlags |= 0x0010;
  }

  // Riser, Jacket and Soil Pile :

  if (IAmShowingRiserData || IAmShowingJacketData || IAmShowingSoilPileData)
  {
    pv->myVisualize3Dts = myRiserVisualize3DButton->getTristateValue();
    pv->myVisualize3DAngles.first = myRiserVisualize3DStartAngleField->myField->getIntFromText();
    pv->myVisualize3DAngles.second = myRiserVisualize3DStopAngleField->myField->getIntFromText();

    pv->myIntFluid = myRiserMudButton->getValue();
    pv->myIntFluidDensity = myRiserMudDensityField->getValue();
    pv->myIntFluidLevel = myRiserMudLevelField->getValue();

    // These fields are reused from Shaft
    pv->myMassPropDamp = myShaftSDMassField->getValue();
    pv->myStifPropDamp = myShaftSDStiffnessField->getValue();
    pv->myScaleMass = myShaftDPMassField->getValue();
    pv->myScaleStiff = myShaftDPStiffnessField->getValue();
    // flags
    pv->myBlankFieldsFlags = 0x0000;
    if (myShaftSDMassField->getText().length() == 0)
      pv->myBlankFieldsFlags |= 0x0002;
    if (myShaftSDStiffnessField->getText().length() == 0)
      pv->myBlankFieldsFlags |= 0x0004;
    if (myShaftDPMassField->getText().length() == 0)
      pv->myBlankFieldsFlags |= 0x0008;
    if (myShaftDPStiffnessField->getText().length() == 0)
      pv->myBlankFieldsFlags |= 0x0010;
  }

  // Joints :

  if (IAmShowingJointData)
    {
      pv->selectedTab = mySelectedTabIndex;
      pv->myObjToPosition = myJointPosition->getEditedObj();

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
      pv->myCamThickness = myJointSummary->myCamThicknessField->getValue();
      pv->myCamWidth     = myJointSummary->myCamWidthField->getValue();
      pv->IAmRadialContact = myJointSummary->myRadialToggle->getValue();
    }

  // Pipe Surface

  if (IAmShowingPipeSurfaceData)
    pv->pipeSurfaceRadius = myPipeRadiusField->getValue();

  // Screw

  if (IAmShowingScrew)
    {
      pv->myIsScrewConnection = myJointSummary->myScrewToggle->getValue();
      pv->myScrewRatio = myJointSummary->myScrewRatioField->getValue();
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
      pv->myLoadMagnitude= myLoadMagnitude->getValue();
      pv->mySelectedLoadMagnitudeEngine= myLoadMagnitude->getSelectedRef();

      pv->myAttackPoint= myAttackPointEditor->getValue();
      pv->myAttackPointIsGlobal= myAttackPointEditor->isGlobal();

      pv->myFromPoint= myFromPointEditor->getValue();
      pv->myFromPointIsGlobal= myFromPointEditor->isGlobal();

      pv->myToPoint= myToPointEditor->getValue();
      pv->myToPointIsGlobal= myToPointEditor->isGlobal();
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
      pv->mySpindelTriadOffset = mySpindelOffsetField->getValue();
      pv->mySelectedTireModelType = myTireModelMenu->getSelectedOptionStr();
      pv->mySelectedRoad = myRoadField->getSelectedRef();

      if (myTireDataFileField->isAConstant())
        {
          pv->mySelectedTireDataFileRef = NULL;
          pv->myTireDataFileName = myTireDataFileField->getText();
        }
      else
        {
          pv->myTireDataFileName = "";
          pv->mySelectedTireDataFileRef = myTireDataFileField->getSelectedRef();
        }
    }

  // Road

  if (IAmShowingRoadData)
    {
      pv->iAmUsingExtRoadData = myUseFileRoadRadio->getValue();
      pv->mySelectedRoadFunc  = myRoadFuncField->getSelectedRef();
      pv->myRoadZShift        = myRoadZShiftField->getValue();
      pv->myRoadXOffset       = myRoadXOffsetField->getValue();
      pv->myRoadZRotation     = myRoadZRotationField->getValue();

      if (myRoadDataFileField->isAConstant())
        {
          pv->mySelectedRoadDataFileRef = NULL;
          pv->myRoadDataFileName = myRoadDataFileField->getText();
        }
      else
        {
          pv->myRoadDataFileName = "";
          pv->mySelectedRoadDataFileRef = myRoadDataFileField->getSelectedRef();
        }
    }

  // Material properties

  if (IAmShowingMatPropData)
    {
      pv->myMatPropRho        = myMatPropRhoField->getValue();
      pv->myMatPropE          = myMatPropEField->getValue();
      pv->myMatPropNu         = myMatPropNuField->getValue();
      pv->myMatPropG          = myMatPropGField->getValue();
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

  // Beam properties

  if (IAmShowingBeamPropData)
    {
      pv->selectedTab = mySelectedTabIndex;
      pv->myBeamCrossSectionType = myBeamPropSummary->myCrossSectionTypeMenu->getSelectedOption();
      pv->mySelectedMaterial = myBeamPropSummary->myMaterialDefField->getSelectedRef();
      pv->myBeamBreakDependence = myBeamPropSummary->myDependencyButton->getValue();
      myBeamPropSummary->getValues(pv->myBeamProp);
      pv->myBeamHydroToggle = myBeamPropHydro->myBeamHydroToggleButton->getValue();
      myBeamPropHydro->getValues(pv->myHydroProp);
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
        {
          pv->mySelectedRAOFileRef = NULL;
          pv->myRAOFileName = myRAOFileField->getText();
        }
      else
        {
          pv->mySelectedRAOFileRef = myRAOFileField->getSelectedRef();
          pv->myRAOFileName = "";
	}

      pv->mySelectedWaveFunc = myWaveFuncField->getSelectedRef();
      pv->mySelectedWaveDir = myWaveDirMenu->getSelectedInt();
      pv->mySelectedScale = myMotionScaleField->getSelectedRef();
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

  myShaftCrossSectionDefField->setButtonCB(pv->myEditButtonCB);

  myTurbineWindRefTriadDefField->setButtonCB(pv->myEditButtonCB);
  myTurbineYawPointTriadDefField->setButtonCB(pv->myEditButtonCB);
  myTurbineHubApexTriadDefField->setButtonCB(pv->myEditButtonCB);
  myTurbineHubPartDefField->setButtonCB(pv->myEditButtonCB);
  myTurbineFirstBearingDefField->setButtonCB(pv->myEditButtonCB);

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

  // Rotational formulation

  myJointAdvancedTab->myRotFormulationMenu->setSensitivity(makeSensitive);
  myJointAdvancedTab->myRotSequenceMenu->setSensitivity(makeSensitive);

  // Spring couplings

  myJointAdvancedTab->myTranSpringCplMenu->setSensitivity(makeSensitive);
  myJointAdvancedTab->myRotSpringCplMenu->setSensitivity(makeSensitive);

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
  mySpindelOffsetField->setSensitivity(tireSensitivity);

  // Road

  myRoadFuncField->setSensitivity(tireSensitivity);
  myRoadZShiftField->setSensitivity(tireSensitivity);
  myRoadXOffsetField->setSensitivity(tireSensitivity);
  myRoadZRotationField->setSensitivity(tireSensitivity);

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

  // RAO vessel motion

  myRAOFileField->setSensitivity(makeSensitive);
  myBrowseRAOFileButton->setSensitivity(makeSensitive);
  myWaveFuncField->setSensitivity(makeSensitive);
  myWaveDirMenu->setSensitivity(makeSensitive);
  myMotionScaleField->setSensitivity(makeSensitive);

  // Shaft, Beam & Blade

  myShaftCrossSectionDefField->setSensitivity(makeSensitive);
  myShaftSDMassField->setSensitivity(makeSensitive);
  myShaftSDStiffnessField->setSensitivity(makeSensitive);
  myShaftDPStiffnessField->setSensitivity(makeSensitive);
  myShaftDPMassField->setSensitivity(makeSensitive);
  for (FFuLabelField* field : myBeamLocalZField)
    field->setSensitivity(makeSensitive);

  // Riser

  myRiserMudButton->setSensitivity(makeSensitive);
  myRiserMudDensityField->setSensitivity(makeSensitive);
  myRiserMudLevelField->setSensitivity(makeSensitive);

  // Turbine

  myTurbineWindRefTriadDefField->setSensitivity(makeSensitive);
  myTurbineWindVertOffsetField->setSensitivity(makeSensitive);
  myTurbineYawPointTriadDefField->setSensitivity(makeSensitive);
  myTurbineHubApexTriadDefField->setSensitivity(makeSensitive);
  myTurbineHubPartDefField->setSensitivity(makeSensitive);
  myTurbineFirstBearingDefField->setSensitivity(makeSensitive);

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
  FmAssemblyBase* subass = dynamic_cast<FmAssemblyBase*>(mySelectedSubass);
  if (subass)
  {
    FaVec3 CoG(subass->getGlobalCoG());
    if (toGlobal)
      mySubassCoGField->setValue(CoG);
    else
      mySubassCoGField->setValue(subass->toLocal(CoG));
  }
}


void FuiProperties::updateSubassCoG()
{
  this->onCoGRefChanged(mySubassCoGField->isGlobal());
}

void FuiProperties::showUsersGuide()
{
  showPDF("file://Doc/FedemUsersGuide.pdf?page=1");
}

void FuiProperties::showTheoryGuide()
{
  showPDF("file://Doc/FedemTheoryGuide.pdf?page=1");
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
