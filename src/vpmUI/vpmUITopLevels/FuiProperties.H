// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_PROPERTIES_H
#define FUI_PROPERTIES_H

#include "FFuLib/FFuBase/FFuMultUIComponent.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "FFuLib/FFuBase/FFuUACommandHandler.H"
#include "vpmUI/vpmUIComponents/FuiSprDaForce.H"
#include "vpmUI/vpmUIComponents/FuiSpringDefCalc.H"
#include "vpmUI/vpmUIComponents/FuiSpringChar.H"
#include "vpmUI/vpmUIComponents/FuiTopologyView.H"
#include "vpmUI/vpmUIComponents/FuiJointDOF.H"
#include "vpmUI/vpmUIComponents/FuiTriadDOF.H"
#include "vpmUI/vpmUIComponents/FuiLinkTabs.H"
#include "vpmUI/vpmUIComponents/FuiBeamPropSummary.H"
#include "vpmUI/vpmUIComponents/FuiBeamPropHydro.H"
#include <array>

class FuiQueryInputField;
class FuiPointEditor;
class FuiPositionData;
class FuiTriadSummary;
class FuiTriadResults;
class FuiJointSummary;
class FuiJointTabAdvanced;
class FuiJointResults;
class FuiSNCurveSelector;
class Fui3DPoint;
class FFuTabbedWidgetStack;
class FFuIOField;
class FFuLabel;
class FFuLabelFrame;
class FFuLabelField;
class FFuFileBrowseField;
class FFuToggleButton;
class FFuRadioButton;
class FFuToolButton;
class FFuPushButton;
class FFuMemo;
class FuaQueryBase;
class FmModelMemberBase;


class FuiProperties : virtual public FFuMultUIComponent,
                      public FFuUAExistenceHandler,
                      public FFuUADataHandler,
                      public FFuUACommandHandler
{
public:
  FuiProperties();
  virtual ~FuiProperties() {}

  // from FFuUADataHandler
  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);
  virtual void buildDynamicWidgets(const FFuaUIValues* values);

  void setCBs(const FFuaUIValues* values);

  virtual void setSensitivity(bool makeSensitive);

  static void showUsersGuide();
  static void showTheoryGuide();
  static void showComApiReference();

  void updateSubassCoG();

  const char* getSelectedTab(const char* objType) const;

protected:
  void initWidgets();

  static void showPDF(const char* url);

  virtual bool initStartGuide() = 0;

  virtual void placeWidgets(int width, int height);

  virtual void onPoppedUp() { this->updateUIValues(); }

  virtual void setUICommands(const FFuaUICommands*) {}
  virtual void updateUICommandsToggle(const FFuaUICommands*) {}
  virtual void updateUICommandsSensitivity(const FFuaUICommands*);

  // Component "shadow" members

  // Heading

  FFuIOField*    myTypeField;
  FFuIOField*    myIdField;
  FFuLabelField* myDescriptionField;
  FFuLabelField* myTagField;

  FuiTopologyView* myTopologyView;
  FFuToolButton*   backBtn;
  FFuToolButton*   forwardBtn;
  FFuToolButton*   helpBtn;

  // Reference Plane

  FuiPositionData* myRefPlanePosition;
  FFuLabelFrame*   myRefPlaneSizeFrame;
  FFuLabelField*   myRefPlaneHeightField;
  FFuLabelField*   myRefPlaneWidthField;

  // Higher Pairs

  FFuLabelFrame* myHPRatioFrame;
  FFuIOField*    myHPRatioField;

  // Advanced spring characteristics

  FuiSpringChar* mySpringChar;

  // Spring & Damper

  FuiSprDaForce*    mySpringForce;
  FuiSpringDefCalc* mySpringDeflCalc;
  FuiSprDaForce*    myDamperForce;

  // Part

  FFuTabbedWidgetStack* myLinkTabs;
  std::string           mySelectedLinkTab;
  int                   mySelectedTabIndex;

  FuiLinkModelSheet*         myLinkModelSheet;
  FuiLinkOriginSheet*        myLinkOriginSheet;
  FuiLinkNodeSheet*          myLinkFEnodeSheet;
  FuiLinkRedOptSheet*        myLinkRedOptSheet;
  FuiLinkLoadSheet*          myLinkLoadSheet;
  FuiGenericPartCGSheet*     myGenericPartCGSheet;
  FuiGenericPartMassSheet*   myGenericPartMassSheet;
  FuiGenericPartStiffSheet*  myGenericPartStiffSheet;
  FuiHydrodynamicsSheet*     myHydrodynamicsSheet;
  FuiMeshingSheet*           myMeshingSheet;
  FuiAdvancedLinkOptsSheet*  myAdvancedLinkOptsSheet;
  FuiNonlinearLinkOptsSheet* myNonlinearLinkOptsSheet;

  // Beam and Shaft

  FFuLabelFrame*      myShaftGeneralFrame;
  FFuLabel*           myShaftCrossSectionDefLabel;
  FuiQueryInputField* myShaftCrossSectionDefField;
  FFuLabelFrame*      myShaftSDFrame;
  FFuLabelField*      myShaftSDMassField;
  FFuLabelField*      myShaftSDStiffnessField;
  FFuLabelFrame*      myShaftDPFrame;
  FFuLabelField*      myShaftDPStiffnessField;
  FFuLabelField*      myShaftDPMassField;
  FFuLabel*           myShaftNoteALabel;
  FFuLabel*           myShaftNoteBLabel;
  FFuLabel*           myShaftNoteCLabel;
  FFuToggleButton*    myVisualize3DButton;
  FFuLabelField*      myVisualize3DStartAngleField;
  FFuLabelField*      myVisualize3DStopAngleField;
  FFuLabelFrame*      myBeamOrientationFrame;
  std::array<FFuLabelField*,3> myBeamLocalZField;

  // Turbine

  FFuLabelFrame*      myTurbineWindRefFrame;
  FFuLabel*           myTurbineWindRefTriadDefLabel;
  FuiQueryInputField* myTurbineWindRefTriadDefField;
  FFuLabel*           myTurbineWindVertOffsetLabel;
  FFuIOField*         myTurbineWindVertOffsetField;
  FFuLabelFrame*      myTurbineAdvTopologyFrame;
  FFuLabel*           myTurbineYawPointTriadDefLabel;
  FuiQueryInputField* myTurbineYawPointTriadDefField;
  FFuLabel*           myTurbineHubApexTriadDefLabel;
  FuiQueryInputField* myTurbineHubApexTriadDefField;
  FFuLabel*           myTurbineHubPartDefLabel;
  FuiQueryInputField* myTurbineHubPartDefField;
  FFuLabel*           myTurbineFirstBearingDefLabel;
  FuiQueryInputField* myTurbineFirstBearingDefField;

  // Generator

  FFuRadioButton*     myGeneratorTorqueRadioBtn;
  FFuRadioButton*     myGeneratorVelocityRadioBtn;
  FFuLogicButtonGroup myGeneratorRadioGroup;
  FuiQueryInputField* myGeneratorTorqueField;
  FuiQueryInputField* myGeneratorVelocityField;

  // Blade

  FFuLabel*           myBladePitchControlLabel;
  FuiQueryInputField* myBladePitchControlField;
  FFuToggleButton*    myBladeFixedPitchToggle;
  FFuLabelFrame*      myBladeIceFrame;
  FFuToggleButton*    myBladeIceLayerToggle;
  FFuLabelField*      myBladeIceThicknessField;

  // Riser

  FFuLabelFrame*      myRiserGeneralFrame;
  FFuLabel*           myRiserInternalToDefLabel;
  FuiQueryInputField* myRiserInternalToDefField;
  FFuToggleButton*    myRiserVisualize3DButton;
  FFuLabelField*      myRiserVisualize3DStartAngleField;
  FFuLabelField*      myRiserVisualize3DStopAngleField;
  FFuLabelFrame*      myRiserMudFrame;
  FFuToggleButton*    myRiserMudButton;
  FFuLabelField*      myRiserMudDensityField;
  FFuLabelField*      myRiserMudLevelField;

  // Joints

  FFuPushButton* mySwapTriadButton;
  FFuPushButton* myAddMasterButton;
  FFuPushButton* myRevMasterButton;

  FFuTabbedWidgetStack* myJointTabs;
  std::string           mySelectedJointTab;
  int                   mySelectedJointDof;

  FuiJointSummary*     myJointSummary;
  FuiPositionData*     myJointPosition;
  std::array<FuiJointDOF*,6> myJointDofs;
  FuiJointTabAdvanced* myJointAdvancedTab;
  FuiJointResults*     myJointResults;

  // Pipe Surface

  FFuLabelField* myPipeRadiusField;

  // Triad

  FFuTabbedWidgetStack* myTriadTabs;
  std::string           mySelectedTriadTab;
  int                   mySelectedTriadDof;

  FuiTriadSummary*      myTriadSummary;
  FuiPositionData*      myTriadPosition;
  FuiTriadResults*      myTriadResults;

  std::array<FuiTriadDOF*,6> myTriadDofs;

  // Load

  FuiQueryInputField* myLoadMagnitude;
  FFuLabel*           myFromPointLabel;
  FFuLabel*           myToPointLabel;
  FFuLabelFrame*      myMagnitudeLabel;
  FFuLabelFrame*      myAttackPointFrame;
  FFuLabelFrame*      myDirectionFrame;
  FuiPointEditor*     myAttackPointEditor;
  FuiPointEditor*     myFromPointEditor;
  FuiPointEditor*     myToPointEditor;

  // Generic DB Object

  FFuLabel*   myGenDBObjTypeLabel;
  FFuIOField* myGenDBObjTypeField;
  FFuLabel*   myGenDBObjDefLabel;
  FFuMemo*    myGenDBObjDefField;

  // File reference

  FFuFileBrowseField* myFileReferenceBrowseField;

  // Tire

  FFuLabel*           myTireDataFileLabel;
  FuiQueryInputField* myTireDataFileField;
  FFuPushButton*      myBrowseTireFileButton;
  FFuLabel*           myRoadLabel;
  FuiQueryInputField* myRoadField;
  FFuLabel*           myTireModelTypeLabel;
  FFuOptionMenu*      myTireModelMenu;
  FFuLabelField*      mySpindelOffsetField;

  // Road

  FFuLogicButtonGroup myRoadTypeToggleGroup;

  FFuRadioButton*     myUseFuncRoadRadio;
  FFuLabel*           myRoadFuncLabel;
  FuiQueryInputField* myRoadFuncField;
  FFuLabelField*      myRoadZShiftField;
  FFuLabelField*      myRoadXOffsetField;
  FFuLabelField*      myRoadZRotationField;

  FFuLabel*           myRoadFuncTypeLabel;
  FFuOptionMenu*      myRoadFuncTypeMenu;

  FFuRadioButton*     myUseFileRoadRadio;
  FFuLabel*           myRoadDataFileLabel;
  FuiQueryInputField* myRoadDataFileField;
  FFuPushButton*      myBrowseRoadFileButton;

  // Material properties

  FFuLabelField*      myMatPropRhoField;
  FFuLabelField*      myMatPropEField;
  FFuLabelField*      myMatPropNuField;
  FFuLabelField*      myMatPropGField;

  // Beam properties

  FFuTabbedWidgetStack* myBeamPropTabs;
  std::string           mySelectedBeamPropTab;
  FuiBeamPropSummary*   myBeamPropSummary;
  FuiBeamPropHydro*     myBeamPropHydro;

  // Strain Rosette

  FFuLabel*           myStrRosTypeLabel;
  FFuOptionMenu*      myStrRosTypeMenu;
  FFuLabelField*      myStrRosNodesField;
  FFuPushButton*      myStrRosEditNodesButton;
  FFuLabelField*      myStrRosAngleField;
  FFuPushButton*      myStrRosEditDirButton;
  FFuLabelField*      myStrRosHeightField;
  FFuToggleButton*    myStrRosUseFEHeightToggle;
  FFuPushButton*      myStrRosFlipZButton;
  FFuLabelField*      myStrRosEmodField;
  FFuLabelField*      myStrRosNuField;
  FFuToggleButton*    myStrRosUseFEMatToggle;
  FFuToggleButton*    myResetStartStrainsToggle;

  FFuLabelFrame*      myMaterialFrame;
  FFuLabelFrame*      myLayerFrame;
  FFuLabelFrame*      myOrientationFrame;

  // Element group

  FFuLabelFrame*      myFatigueFrame;
  FFuToggleButton*    myFatigueToggle;
  FuiSNCurveSelector* mySNSelector;
  FFuLabelField*      myScfField;

  // RAO vessel motion

  FFuLabel*           myRAOFileLabel;
  FuiQueryInputField* myRAOFileField;
  FFuPushButton*      myBrowseRAOFileButton;
  FFuLabel*           myWaveFuncLabel;
  FuiQueryInputField* myWaveFuncField;
  FFuLabel*           myWaveDirLabel;
  FFuOptionMenu*      myWaveDirMenu;
  FFuLabel*           myMotionScaleLabel;
  FuiQueryInputField* myMotionScaleField;

  // Sea State

  FFuLabelFrame*      mySeaStateSizeFrame;
  FFuLabelFrame*      mySeaStatePositionFrame;
  FFuLabelFrame*      mySeaStateVisualizationFrame;
  FFuLabelField*      mySeaStateWidthField;
  FFuLabelField*      mySeaStateHeightField;
  FFuLabelField*      mySeaStateWidthPosField;
  FFuLabelField*      mySeaStateHeightPosField;
  FFuLabelField*      mySeaStateNumPoints;
  FFuToggleButton*    mySeaStateShowGridToggle;
  FFuToggleButton*    mySeaStateShowSolidToggle;

  // Simulation event

  FFuLabelField*      myEventProbability;
  FFuPushButton*      mySelectEventButton;
  FFuLabel*           myActiveEventLabel;

  // Subassembly

  FFuFileBrowseField* mySubassFileField;
  FuiPositionData*    mySubassPosition;
  FmModelMemberBase*  mySelectedSubass;
  FFuLabelField*      mySubassMassField;
  FFuLabelFrame*      mySubassCoGFrame;
  Fui3DPoint*         mySubassCoGField;
  FFuLabelField*      mySubassLengthField;

  // Function properties

  FFuComponentBase* myFunctionProperties;

  // Control system properties

  FFuComponentBase* myCtrlElementProperties;
  FFuComponentBase* myExtCtrlSysProperties;

  // Animation, graph and curve properties

  FFuComponentBase* myAnimationDefine;
  FFuComponentBase* myGraphDefine;
  FFuComponentBase* myCurveDefine;

  // Start guide
  FFuLabel* mySGLogoImage;
  FFuLabel* mySGLogoBorderTop;
  FFuLabel* mySGLogoBorderRight;
  FFuLabel* mySGHeading;
  FFuLabel* mySGFillWhite;
  FFuLabel* mySGBorderTop;
  FFuLabel* mySGBorderRight;
  FFuLabel* mySGBorderBottom;
  FFuLabel* mySGContentLabel;

private:
  bool IAmSensitive;
  bool IAmShowingHeading;
  char IAmShowingFriction;
  bool IAmShowingDOF_TZ_Toggle;
  bool IAmShowingRotFormulation;
  bool IAmShowingTranSpringCpl;
  bool IAmShowingRotSpringCpl;
  bool IAmShowingScrew;
  bool IAmShowingSwapTriadButton;
  bool IAmShowingAddMasterButton;
  bool IAmShowingRevMasterButton;
  bool IAmShowingCamData;
  bool IAmShowingRefPlane;
  bool IAmShowingHPRatio;
  bool IAmShowingAxialDamper;
  bool IAmShowingAxialSpring;
  bool IAmShowingSpringChar;
  bool IAmShowingLinkData;
  char IAmShowingTriadData;
  bool IAmShowingLoadData;
  char IAmShowingJointData;
  bool IAmShowingGenDBObjData;
  bool IAmShowingFunctionData;
  bool IAmShowingCtrlData;
  bool IAmShowingCtrlInOut;
  bool IAmShowingExtCtrlSysData;
  bool IAmShowingAnimationData;
  bool IAmShowingGraphData;
  bool IAmShowingCurveData;
  bool IAmShowingFileReference;
  bool IAmShowingTireData;
  bool IAmShowingRoadData;
  bool IAmShowingMatPropData;
  bool IAmShowingSeaStateData;
  bool IAmShowingBeamPropData;
  bool IAmShowingStrainRosetteData;
  bool IAmShowingPipeSurfaceData;
  bool IAmShowingGroupData;
  bool IAmShowingRAOData;
  bool IAmShowingEventData;
  bool IAmShowingSubAssData;
  bool IAmShowingUDEData;
  bool IAmShowingBeamData;
  bool IAmShowingShaftData;
  bool IAmShowingTurbineData;
  bool IAmShowingTowerData;
  bool IAmShowingNacelleData;
  bool IAmShowingGeneratorData;
  bool IAmShowingGearboxData;
  bool IAmShowingRotorData;
  bool IAmShowingBladeData;
  bool IAmShowingRiserData;
  bool IAmShowingJacketData;
  bool IAmShowingSoilPileData;
  char IAmShowingStartGuide;

  // Methods for rerouting of CB's :

  void onValuesChanged();
  void onStringChanged(const std::string&)      { this->onValuesChanged(); }
  void onNonConstStringChanged(std::string)     { this->onValuesChanged(); }
  void onIntDoubleChanged(int,double)           { this->onValuesChanged(); }
  void onBoolChanged(bool)                      { this->onValuesChanged(); }
  void onBoolTouched(bool)                      { this->updateUIValues(); }
  void onIntBoolChanged(int,bool)               { this->onValuesChanged(); }
  void onDoubleChanged(double)                  { this->onValuesChanged(); }
  void onIntChanged(int)                        { this->onValuesChanged(); }
  void onVecBoolChanged(const FaVec3&,bool)     { this->onValuesChanged(); }
  void onFileRefChanged(const std::string&,int) { this->onValuesChanged(); }
  void onQIFieldChanged(FuiQueryInputField*)    { this->onValuesChanged(); }

  void onMaterialChanged(FuiQueryInputField* matField);
  void onLinkTabSelected(int);
  void onJointTabSelected(int);
  void onJointDofChanged(int status);
  void onTriadTabSelected(int);
  void onTriadDofChanged(int status);
  void onBeamPropTabSelected(int);
  void onEventActivated();
  void onCoGRefChanged(bool toGlobal);

  // Callback objects

  FFaDynCB2<int,int> myDofStatusCB;
  FFaDynCB0          mySimEventCB;
};


class FuaPropertiesValues : public FFuaUIValues
{
public:
  FuaPropertiesValues()
  {
    showHeading           = false;
    showFriction          = false;
    showRotFormulation    = false;
    showDOF_TZ_Toggle     = false;
    showRotSpringCpl      = false;
    showTranSpringCpl     = false;
    showRefPlane          = false;
    showScrew             = false;
    showSwapTriadButton   = false;
    showAddMasterButton   = false;
    showReverseMasterButton = false;
    showCamData           = false;
    showHPRatio           = false;
    showAxialDamper       = false;
    showAxialSpring       = false;
    showSpringChar        = false;
    showLinkData          = false;
    showUDEData           = false;
    showBeamData          = false;
    showShaftData         = false;
    showTurbineData       = false;
    showTowerData         = false;
    showNacelleData       = false;
    showGeneratorData     = false;
    showGearboxData       = false;
    showRotorData         = false;
    showBladeData         = false;
    showRiserData         = false;
    showJacketData        = false;
    showSoilPileData      = false;
    showTriadData         = false;
    showLoadData          = false;
    showJointData         = false;
    showGenDBObj          = false;
    showFileReference     = false;
    showTireData          = false;
    showRoadData          = false;
    showMatPropData       = false;
    showSeaStateData      = false;
    showBeamPropData      = false;
    showStrainRosetteData = false;
    showPipeSurfaceData   = false;
    showGroupData         = false;
    showRAOData           = false;
    showSimEventData      = false;
    showActiveEvent       = false;
    showSubassemblyData   = false;
    showSubassPos         = false;

    showFunctionData      = false;
    showCtrlData          = false;
    showCtrlInOut         = false;
    showExtCtrlSysData    = false;
    showAnimationData     = false;
    showGraphData         = false;
    showCurveData         = false;

    showStartGuide        = false;

    myId                  = 0;
    myRefPlaneWidth       = 0.0;
    myRefPlaneHeight      = 0.0;
    myHPRatio             = 0.0;
    myTotalLength         = 0.0;
    myTotalMass           = 0.0;
    myCrossSectionQuery   = NULL;
    mySelectedCS          = NULL;
    myHideCrossSection    = true;
    myVisualize3DEnabled  = true;
    myVisualize3D         = true;
    myVisualize3Dts       = 0;
    myVisualize3DAngles   = { 0, 0 };
    myMassPropDamp        = 0.0;
    myStifPropDamp        = 0.0;
    myScaleMass           = 0.0;
    myScaleStiff          = 0.0;
    myBlankFieldsFlags    = 0;

    isSlave = isMaster    = false;
    myTriadIsAttached     = false;
    myTriadConnector      = 0;
    mySysDir              = 0;

    myAttackPointIsGlobal = false;
    myFromPointIsGlobal   = false;
    myToPointIsGlobal     = false;
    myObjToPosition       = NULL;
    selectedTab           = 0;
  }

  // General Callbacks :

  FFaDynCB1<FuiQueryInputFieldValues&> myEditButtonCB;

  // Heading :

  bool        showHeading;
  std::string myType;
  int         myId;
  std::string myDescription;
  std::string myTag;

  std::vector<FuiTopologyItem> myTopology;

  FFaDynCB2<int,bool> myTopologyHighlightCB;
  FFaDynCB1<int>      myTopologyActivatedCB;
  FFaDynCB2<const std::vector<int>&,std::vector<FFuaCmdItem*>&> myTopologyRightClickedCB;

  // Reference Plane :

  bool   showRefPlane;
  double myRefPlaneWidth;
  double myRefPlaneHeight;

  // Higher Pair :

  bool   showHPRatio;
  double myHPRatio;

  // Axial Damper :

  bool                 showAxialDamper;
  FuiSprDaForceValues  myAxialDaForceValues;

  // Axial Spring :

  bool                   showAxialSpring;
  FuiSprDaForceValues    myAxialSprForceValues;
  FuiSpringDefCalcValues myAxialSpringDefCalcValues;

  // Spring characteristics :

  bool                showSpringChar;
  FuiSpringCharValues mySpringCharValues;

  // Part :

  bool          showLinkData;
  unsigned char selectedTab;
  FuiLinkValues myLinkValues;

  // User-defined element and Beam :

  bool                  showUDEData;
  bool                  showBeamData;
  double                myTotalLength; // read-only
  double                myTotalMass; // read-only
  FFuaQueryBase*        myCrossSectionQuery;
  FmModelMemberBase*    mySelectedCS;
  bool                  myHideCrossSection;
  bool                  myVisualize3DEnabled;
  bool                  myVisualize3D;
  int                   myVisualize3Dts;
  std::pair<int,int>    myVisualize3DAngles;
  double                myMassPropDamp;
  double                myStifPropDamp;
  double                myScaleMass;
  double                myScaleStiff;
  FaVec3                myOrientation;
  int                   myBlankFieldsFlags; // specifies what fields are blank

  // Wind turbine :

  bool showTurbineData;
  bool showTowerData;
  bool showNacelleData;
  bool showShaftData;
  bool showGearboxData;
  bool showGeneratorData;
  bool showRotorData;
  bool showBladeData;

  FFuaQueryBase*      myTurbineWindRefTriadDefQuery;
  FmModelMemberBase*  myTurbineWindRefTriadDefSelected;
  double              myTurbineWindVertOffset;
  FFuaQueryBase*      myTurbineYawPointTriadDefQuery;
  FmModelMemberBase*  myTurbineYawPointTriadDefSelected;
  FFuaQueryBase*      myTurbineHubApexTriadDefQuery;
  FmModelMemberBase*  myTurbineHubApexTriadDefSelected;
  FFuaQueryBase*      myTurbineHubPartDefQuery;
  FmModelMemberBase*  myTurbineHubPartDefSelected;
  FFuaQueryBase*      myTurbineFirstBearingDefQuery;
  FmModelMemberBase*  myTurbineFirstBearingDefSelected;

  // Generator :

  bool                myGeneratorTorqueControl;
  bool                myGeneratorTorqueControlIsConstant;
  double              myGeneratorTorqueControlConstValue;
  FFuaQueryBase*      myGeneratorTorqueControlEngineQuery;
  FmModelMemberBase*  myGeneratorTorqueControlSelectedEngine;
  bool                myGeneratorVelocityControlIsConstant;
  double              myGeneratorVelocityControlConstValue;
  FFuaQueryBase*      myGeneratorVelocityControlEngineQuery;
  FmModelMemberBase*  myGeneratorVelocityControlSelectedEngine;

  // Blade :

  bool                myBladeIceLayer;
  double              myBladeIceThickness;
  bool                myBladePitchIsFixed;
  bool                myBladePitchControlIsConstant;
  double              myBladePitchControlConstValue;
  FFuaQueryBase*      myBladePitchControlEngineQuery;
  FmModelMemberBase*  myBladePitchControlSelectedEngine;

  // Riser, Jacket and Soil Pile :

  bool                showRiserData;
  bool                showJacketData;
  bool                showSoilPileData;
  bool                myIntFluid;
  double              myIntFluidDensity;
  double              myIntFluidLevel;

  // Triad :

  bool   showTriadData;
  bool   isSlave;
  bool   isMaster;
  bool   myTriadIsAttached;
  char   myTriadConnector;
  char   mySysDir;
  std::string myFENodeIdx;
  std::array<double,4> myMass;

  FFaDynCB2<int,int> myDofStatusCB;

  std::vector<FuiTriadDOFValues> myTriadVals;
  std::vector<bool>              myResToggles;

  // Joint :

  char   showJointData;
  std::vector<FuiJointDOFValues> myJointVals;

  bool   showSwapTriadButton;
  bool   showAddMasterButton;
  bool   showReverseMasterButton;
  FFaDynCB0 mySwapTriadCB;
  FFaDynCB0 myAddMasterCB;
  FFaDynCB0 myRevMasterCB;
  FmModelMemberBase* myObjToPosition;

  FFaDynCB0 myDofSetAllFreeCB;
  FFaDynCB0 myDofSetAllFixedCB;
  FFaDynCB0 mySwapJointCB;

  // Pipe Surface
  bool   showPipeSurfaceData;
  double pipeSurfaceRadius;

  // Cam
  bool   showCamData;
  double myCamThickness;
  double myCamWidth;
  bool   IAmRadialContact;

  // Screw
  bool   showScrew;
  bool   myIsScrewConnection;
  double myScrewRatio;

  // Rotational formulation
  bool showRotFormulation;
  int  mySelectedRotFormulation;
  int  mySelectedRotSequence;
  std::vector<std::string> myRotFormulationTypes;
  std::vector<std::string> myRotSequenceTypes;

  // Spring coupling
  bool showRotSpringCpl;
  bool showTranSpringCpl;
  int  mySelectedRotSpringCpl;
  int  mySelectedTranSpringCpl;
  std::vector<std::string> mySpringCplTypes;

  // Rev joint optional TZ DOF
  bool showDOF_TZ_Toggle;
  bool myIsDOF_TZ_legal;

  // Friction :

  char               showFriction;
  FFuaQueryBase*     myFrictionQuery;
  FmModelMemberBase* mySelectedFriction;
  int                myFrictionDof;

  // Load :

  bool               showLoadData;
  double             myLoadMagnitude;
  FFuaQueryBase*     myLoadEngineQuery;
  FmModelMemberBase* mySelectedLoadMagnitudeEngine;

  FaVec3      myAttackPoint;
  bool        myAttackPointIsGlobal;
  std::string myAttackObjectText;

  FFaDynCB1<bool>        myLoadViewAttackPointCB;
  FFaDynCB1<bool>        myLoadViewAttackWhatCB;
  FFaDynCB0              myLoadPickAttackPointCB;
  FFaDynCB2<bool,FaVec3> myLoadApplyCB;

  FaVec3      myFromPoint;
  bool        myFromPointIsGlobal;
  std::string myFromPointObjectText;

  FFaDynCB1<bool> myLoadViewFromPointCB;
  FFaDynCB1<bool> myLoadViewFromWhatCB;
  FFaDynCB0       myLoadPickFromPointCB;

  FaVec3      myToPoint;
  bool        myToPointIsGlobal;
  std::string myToPointObjectText;

  FFaDynCB1<bool> myLoadViewToPointCB;
  FFaDynCB1<bool> myLoadViewToWhatCB;
  FFaDynCB0       myLoadPickToPointCB;

  // Generic DB Object

  bool           showGenDBObj;
  std::string    myGenDBObjType;
  std::string    myGenDBObjDef;

  // File Reference

  bool           showFileReference;
  std::string    myFileReferenceName;
  std::string    myModelFilePath;

  // Tire

  bool                  showTireData;
  FFaDynCB0             myBrowseTireFileCB;
  FFuaQueryBase       * myTireDataFileRefQuery;
  FFuaQueryBase       * myRoadQuery;
  std::vector<std::string> myTireTypes;
  double                mySpindelTriadOffset;
  std::string           mySelectedTireModelType;
  FmModelMemberBase   * mySelectedRoad;
  FmModelMemberBase   * mySelectedTireDataFileRef;
  std::string           myTireDataFileName;

  // Road

  bool                  showRoadData;
  bool                  iAmUsingExtRoadData;
  FFuaQueryBase       * myRoadFunctionQuery;
  FmModelMemberBase   * mySelectedRoadFunc;
  double                myRoadZShift;
  double                myRoadXOffset;
  double                myRoadZRotation;
  FFaDynCB0             myBrowseRoadFileCB;
  FFuaQueryBase       * myRoadDataFileRefQuery;
  FmModelMemberBase   * mySelectedRoadDataFileRef;
  std::string           myRoadDataFileName;

  // Material properties

  bool                  showMatPropData;
  double                myMatPropRho;
  double                myMatPropE;
  double                myMatPropNu;
  double                myMatPropG;

  // Beam properties

  bool                  showBeamPropData;
  int                   myBeamCrossSectionType;
  FFuaQueryBase*        myMaterialQuery;
  FmModelMemberBase*    mySelectedMaterial;
  bool                  myBeamBreakDependence;
  bool                  myBeamHydroToggle;
  std::array<double,18> myBeamProp;
  std::array<double,10> myHydroProp;

  // Strain rosette

  bool                   showStrainRosetteData;
  bool                   myStrRosIsEditable;
  std::vector<int>       myStrRosNodes;
  FFaDynCB0              myEditNodesCB;
  double                 myStrRosAngle;
  FFaDynCB0              myEditDirectionCB;
  FFaDynCB0              myFlipZCB;
  std::vector<std::string> myStrainRosetteTypes;
  std::string            mySelectedRosetteType;
  double                 myStrRosHeight;
  bool                   IAmUsingFEHeight;
  double                 myStrRosEMod;
  double                 myStrRosNu;
  bool                   IAmUsingFEMaterial;
  bool                   IAmResettingStartStrains;

  // Element group

  bool   showGroupData;
  bool   doFatigue;
  int    mySNCurve;
  int    mySNStd;
  double mySCF;

  // RAO vessel motion

  bool               showRAOData;
  FFaDynCB0          myBrowseRAOFileCB;
  FFuaQueryBase*     myRAOFileRefQuery;
  FmModelMemberBase* mySelectedRAOFileRef;
  std::string        myRAOFileName;
  FFuaQueryBase*     myWaveFunctionQuery;
  FmModelMemberBase* mySelectedWaveFunc;
  std::vector<int>   myWaveDirections;
  int                mySelectedWaveDir;
  FFuaQueryBase*     myMotionScaleQuery;
  FmModelMemberBase* mySelectedScale;

  // Sea state

  bool   showSeaStateData;
  double mySeaStateWidth;
  double mySeaStateHeight;
  double mySeaStateWidthPos;
  double mySeaStateHeightPos;
  int    mySeaStateNumPoints;
  bool   mySeaStateShowGrid;
  bool   mySeaStateShowSolid;

  // Simulation event

  bool      showSimEventData;
  bool      showActiveEvent;
  bool      allowSimEventChange;
  double    mySimEventProbability;
  FFaDynCB0 mySimEventCB;

  // Subassembly

  bool        showSubassemblyData;
  bool        showSubassPos;
  FaVec3      myCoG;
  std::string mySubAssemblyFile;

  // Function properties
  // (The FunctionProperties ui communicates with db by itself)

  bool showFunctionData;

  // Control system properties
  // (The CtrlElementProperties ui communicates with db by itself)

  bool showCtrlData;
  bool showCtrlInOut;
  bool showExtCtrlSysData;

  // Animation and graph properties
  // (The animationdefine, graphdefine & curvedefine ui's communicate
  // with db by themselves)

  bool showAnimationData;
  bool showGraphData;
  bool showCurveData;

  // Start guide

  bool showStartGuide;
};

#endif
