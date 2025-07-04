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
class FmAssemblyBase;
class FmModelMemberBase;


class FuiProperties : virtual public FFuMultUIComponent,
                      public FFuUAExistenceHandler,
                      public FFuUADataHandler,
                      public FFuUACommandHandler
{
public:
  FuiProperties();

  FuiTopologyView* getTopologyView() { return myTopologyView; }

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
  FmAssemblyBase*     mySelectedSubass;
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

  // General Callbacks :

  FFaDynCB1<FuiQueryInputFieldValues&> myEditButtonCB;

  // Heading :

  bool        showHeading = false;
  std::string myType;
  int         myId = 0;
  std::string myDescription;
  std::string myTag;

  std::vector<FuiTopologyItem> myTopology;

  FFaDynCB2<int,bool> myTopologyHighlightCB;
  FFaDynCB1<int>      myTopologyActivatedCB;
  FFaDynCB2<const std::vector<int>&,std::vector<FFuaCmdItem*>&> myTopologyRightClickedCB;

  // Reference Plane :

  bool   showRefPlane = false;
  double myRefPlaneWidth = 0.0;
  double myRefPlaneHeight = 0.0;

  // Higher Pair :

  bool   showHPRatio = false;
  double myHPRatio = 0.0;

  // Axial Damper :

  bool                showAxialDamper = false;
  FuiSprDaForceValues myAxialDaForceValues;

  // Axial Spring :

  bool                   showAxialSpring = false;
  FuiSprDaForceValues    myAxialSprForceValues;
  FuiSpringDefCalcValues myAxialSpringDefCalcValues;

  // Spring characteristics :

  bool                showSpringChar = false;
  FuiSpringCharValues mySpringCharValues;

  // Part :

  bool          showLinkData = false;
  unsigned char selectedTab = 0;
  FuiLinkValues myLinkValues;

  // User-defined element and Beam :

  bool               showUDEData = false;
  bool               showBeamData = false;
  double             myTotalLength = 0.0;
  double             myTotalMass = 0.0;
  FFuaQueryBase*     myCrossSectionQuery = NULL;
  FmModelMemberBase* mySelectedCS = NULL;
  bool               myHideCrossSection = false;
  bool               myVisualize3DEnabled = false;
  bool               myVisualize3D = false;
  int                myVisualize3Dts = 0;
  std::pair<int,int> myVisualize3DAngles = { 0, 0 };
  double             myMassPropDamp = 0.0;
  double             myStifPropDamp = 0.0;
  double             myScaleMass = 0.0;
  double             myScaleStiff = 0.0;
  FaVec3             myOrientation;
  int                myBlankFieldsFlags = 0;

  // Wind turbine :

  bool showTurbineData = false;
  bool showTowerData = false;
  bool showNacelleData = false;
  bool showShaftData = false;
  bool showGearboxData = false;
  bool showGeneratorData = false;
  bool showRotorData = false;
  bool showBladeData = false;

  FFuaQueryBase*     myTurbineWindRefTriadDefQuery = NULL;
  FmModelMemberBase* myTurbineWindRefTriadDefSelected = NULL;
  double             myTurbineWindVertOffset = 0.0;
  FFuaQueryBase*     myTurbineYawPointTriadDefQuery = NULL;
  FmModelMemberBase* myTurbineYawPointTriadDefSelected = NULL;
  FFuaQueryBase*     myTurbineHubApexTriadDefQuery = NULL;
  FmModelMemberBase* myTurbineHubApexTriadDefSelected = NULL;
  FFuaQueryBase*     myTurbineHubPartDefQuery = NULL;
  FmModelMemberBase* myTurbineHubPartDefSelected = NULL;
  FFuaQueryBase*     myTurbineFirstBearingDefQuery = NULL;
  FmModelMemberBase* myTurbineFirstBearingDefSelected = NULL;

  // Generator :

  bool               myGeneratorTorqueControl = false;
  bool               myGeneratorTorqueControlIsConstant = false;
  double             myGeneratorTorqueControlConstValue = 0.0;
  FFuaQueryBase*     myGeneratorTorqueControlEngineQuery = NULL;
  FmModelMemberBase* myGeneratorTorqueControlSelectedEngine = NULL;
  bool               myGeneratorVelocityControlIsConstant = false;
  double             myGeneratorVelocityControlConstValue = 0.0;
  FFuaQueryBase*     myGeneratorVelocityControlEngineQuery = NULL;
  FmModelMemberBase* myGeneratorVelocityControlSelectedEngine = NULL;

  // Blade :

  bool               myBladeIceLayer = false;
  double             myBladeIceThickness = 0.0;
  bool               myBladePitchIsFixed = false;
  bool               myBladePitchControlIsConstant = false;
  double             myBladePitchControlConstValue = 0.0;
  FFuaQueryBase*     myBladePitchControlEngineQuery = NULL;
  FmModelMemberBase* myBladePitchControlSelectedEngine = NULL;

  // Riser, Jacket and Soil Pile :

  bool   showRiserData = false;
  bool   showJacketData = false;
  bool   showSoilPileData = false;
  bool   myIntFluid = false;
  double myIntFluidDensity = 0.0;
  double myIntFluidLevel = 0.0;

  // Triad :

  bool showTriadData = false;
  bool isSlave = false;
  bool isMaster = false;
  bool myTriadIsAttached = false;
  char myTriadConnector = 0;
  char mySysDir = 0;
  std::string myFENodeIdx;
  std::array<double,4> myMass{};

  FFaDynCB2<int,int> myDofStatusCB;

  std::vector<FuiTriadDOFValues> myTriadVals;
  std::vector<bool>              myResToggles;

  // Joint :

  char showJointData = false;
  std::vector<FuiJointDOFValues> myJointVals;

  bool showSwapTriadButton = false;
  bool showAddMasterButton = false;
  bool showReverseMasterButton = false;
  FFaDynCB0 mySwapTriadCB;
  FFaDynCB0 myAddMasterCB;
  FFaDynCB0 myRevMasterCB;

  std::vector<FmModelMemberBase*> objsToPosition;

  FFaDynCB0 myDofSetAllFreeCB;
  FFaDynCB0 myDofSetAllFixedCB;
  FFaDynCB0 mySwapJointCB;

  // Pipe Surface
  bool   showPipeSurfaceData = false;
  double pipeSurfaceRadius = 0.0;

  // Cam
  bool   showCamData = false;
  double myCamThickness = 0.0;
  double myCamWidth = 0.0;
  bool   IAmRadialContact = false;

  // Screw
  bool   showScrew = false;
  bool   myIsScrewConnection = false;
  double myScrewRatio = 0.0;

  // Rotational formulation
  bool showRotFormulation = false;
  int  mySelectedRotFormulation = 0;
  int  mySelectedRotSequence = 0;
  std::vector<std::string> myRotFormulationTypes;
  std::vector<std::string> myRotSequenceTypes;

  // Spring coupling
  bool showRotSpringCpl = false;
  bool showTranSpringCpl = false;
  int  mySelectedRotSpringCpl = 0;
  int  mySelectedTranSpringCpl = 0;
  std::vector<std::string> mySpringCplTypes;

  // Rev joint optional TZ DOF
  bool showDOF_TZ_Toggle = false;
  bool myIsDOF_TZ_legal = false;

  // Friction :

  char               showFriction = false;
  FFuaQueryBase*     myFrictionQuery = NULL;
  FmModelMemberBase* mySelectedFriction = NULL;
  int                myFrictionDof = 0;

  // Load :

  bool               showLoadData = false;
  double             myLoadMagnitude = 0.0;
  FFuaQueryBase*     myLoadEngineQuery = NULL;
  FmModelMemberBase* mySelectedLoadMagnitudeEngine = NULL;

  FaVec3      myAttackPoint;
  bool        myAttackPointIsGlobal = false;
  std::string myAttackObjectText;

  FFaDynCB1<bool>        myLoadViewAttackPointCB;
  FFaDynCB1<bool>        myLoadViewAttackWhatCB;
  FFaDynCB0              myLoadPickAttackPointCB;
  FFaDynCB2<bool,FaVec3> myLoadApplyCB;

  FaVec3      myFromPoint;
  bool        myFromPointIsGlobal = false;
  std::string myFromPointObjectText;

  FFaDynCB1<bool> myLoadViewFromPointCB;
  FFaDynCB1<bool> myLoadViewFromWhatCB;
  FFaDynCB0       myLoadPickFromPointCB;

  FaVec3      myToPoint;
  bool        myToPointIsGlobal = false;
  std::string myToPointObjectText;

  FFaDynCB1<bool> myLoadViewToPointCB;
  FFaDynCB1<bool> myLoadViewToWhatCB;
  FFaDynCB0       myLoadPickToPointCB;

  // Generic DB Object

  bool        showGenDBObj = false;
  std::string myGenDBObjType;
  std::string myGenDBObjDef;

  // File Reference

  bool        showFileReference = false;
  std::string myFileReferenceName;
  std::string myModelFilePath;

  // Tire

  bool                     showTireData = false;
  FFaDynCB0                myBrowseTireFileCB;
  FFuaQueryBase*           myTireDataFileRefQuery = NULL;
  FFuaQueryBase*           myRoadQuery = NULL;
  std::vector<std::string> myTireTypes;
  double                   mySpindelTriadOffset;
  std::string              mySelectedTireModelType;
  FmModelMemberBase*       mySelectedRoad = NULL;
  FmModelMemberBase*       mySelectedTireDataFileRef = NULL;
  std::string              myTireDataFileName;

  // Road

  bool               showRoadData = false;
  bool               iAmUsingExtRoadData;
  FFuaQueryBase*     myRoadFunctionQuery = NULL;
  FmModelMemberBase* mySelectedRoadFunc = NULL;
  double             myRoadZShift = 0.0;
  double             myRoadXOffset = 0.0;
  double             myRoadZRotation = 0.0;
  FFaDynCB0          myBrowseRoadFileCB;
  FFuaQueryBase*     myRoadDataFileRefQuery = NULL;
  FmModelMemberBase* mySelectedRoadDataFileRef = NULL;
  std::string        myRoadDataFileName;

  // Material properties

  bool   showMatPropData = false;
  double myMatPropRho = 0.0;
  double myMatPropE = 0.0;
  double myMatPropNu = 0.0;
  double myMatPropG = 0.0;

  // Beam properties

  bool                  showBeamPropData = false;
  int                   myBeamCrossSectionType = 0;
  FFuaQueryBase*        myMaterialQuery = NULL;
  FmModelMemberBase*    mySelectedMaterial = NULL;
  bool                  myBeamBreakDependence = false;
  bool                  myBeamHydroToggle = false;
  std::array<double,18> myBeamProp{};
  std::array<double,10> myHydroProp{};

  // Strain rosette

  bool                     showStrainRosetteData = false;
  bool                     myStrRosIsEditable = false;
  std::vector<int>         myStrRosNodes;
  FFaDynCB0                myEditNodesCB;
  double                   myStrRosAngle = 0.0;
  FFaDynCB0                myEditDirectionCB;
  FFaDynCB0                myFlipZCB;
  std::vector<std::string> myStrainRosetteTypes;
  std::string              mySelectedRosetteType;
  double                   myStrRosHeight = 0.0;
  bool                     IAmUsingFEHeight = false;
  double                   myStrRosEMod = 0.0;
  double                   myStrRosNu = 0.0;
  bool                     IAmUsingFEMaterial = false;
  bool                     IAmResettingStartStrains = false;

  // Element group

  bool   showGroupData = false;
  bool   doFatigue = false;
  int    mySNCurve = 0;
  int    mySNStd = 0;
  double mySCF = 0.0;

  // RAO vessel motion

  bool               showRAOData = false;
  FFaDynCB0          myBrowseRAOFileCB;
  FFuaQueryBase*     myRAOFileRefQuery = NULL;
  FmModelMemberBase* mySelectedRAOFileRef = NULL;
  std::string        myRAOFileName;
  FFuaQueryBase*     myWaveFunctionQuery = NULL;
  FmModelMemberBase* mySelectedWaveFunc = NULL;
  std::vector<int>   myWaveDirections;
  int                mySelectedWaveDir = 0;
  FFuaQueryBase*     myMotionScaleQuery = NULL;
  FmModelMemberBase* mySelectedScale = NULL;

  // Sea state

  bool   showSeaStateData = false;
  double mySeaStateWidth = 0.0;
  double mySeaStateHeight = 0.0;
  double mySeaStateWidthPos = 0.0;
  double mySeaStateHeightPos = 0.0;
  int    mySeaStateNumPoints = 0;
  bool   mySeaStateShowGrid = false;
  bool   mySeaStateShowSolid = false;

  // Simulation event

  bool      showSimEventData = false;
  bool      showActiveEvent = false;
  bool      allowSimEventChange = false;
  double    mySimEventProbability = 0.0;
  FFaDynCB0 mySimEventCB;

  // Subassembly

  bool        showSubassemblyData = false;
  bool        showSubassPos = false;
  FaVec3      myCoG;
  std::string mySubAssemblyFile;

  // Function properties
  // (The FunctionProperties ui communicates with db by itself)

  bool showFunctionData = false;

  // Control system properties
  // (The CtrlElementProperties ui communicates with db by itself)

  bool showCtrlData = false;
  bool showCtrlInOut = false;
  bool showExtCtrlSysData = false;

  // Animation and graph properties
  // (The animationdefine, graphdefine & curvedefine ui's communicate
  // with db by themselves)

  bool showAnimationData = false;
  bool showGraphData = false;
  bool showCurveData = false;

  // Start guide

  bool showStartGuide = false;
};

#endif
