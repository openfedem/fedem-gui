// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtProperties.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtDynamicProperties.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtTopologyView.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtMemo.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtPointEditor.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtFunctionProperties.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtCtrlElemProperties.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtAnimationDefine.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtGraphDefine.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtCurveDefine.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtSNCurveSelector.H"
#endif
#ifdef FT_HAS_EXTCTRL
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtExtCtrlSysProperties.H"
#endif
#include "FFuLib/FFuQtComponents/FFuQtRadioButton.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtSpringChar.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtSprDaForce.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtSpringDefCalc.H"
#include "FFuLib/FFuQtComponents/FFuQtToolButton.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFuLib/FFuQtComponents/FFuQtTabbedWidgetStack.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtJointSummary.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtJointDOF.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtTriadSummary.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtBeamPropSummary.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtBeamPropHydro.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtTriadDOF.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtResultTabs.H"
#include "FFuLib/FFuQtComponents/FFuQtFileBrowseField.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtLinkTabs.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtPositionData.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtJointTabAdvanced.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQt3DPoint.H"
#include "vpmUI/Pixmaps/startGuideLogo.xpm"
#include "vpmUI/Pixmaps/startGuideBorderRight.xpm"
#include "vpmUI/Pixmaps/startGuideBorderTop.xpm"

#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "Admin/FedemAdmin.H"

#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include <QProcess>
#include <QGridLayout>

#if defined(win32) || defined(win64)
#include <windows.h>
#endif
#include <iostream>


FuiQtStartGuide::FuiQtStartGuide(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  myLogoImage = new FFuQtLabel(this);
  myLogoBorderTop = new FFuQtLabel(this);
  myLogoBorderRight = new FFuQtLabel(this);
  myHeading = new FFuQtLabel(this);
  myBorderRight = new FFuQtLabel(this);
  myBorderBottom = new FFuQtLabel(this);
  myBorderTop = new FFuQtLabel(this);

  FFuQtLabel* qLabel;
  myContentLabel = qLabel = new FFuQtLabel(this);
  qLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
}


class FuiQtProperty : public FFuQtWidget
{
public:
  FuiQtProperty(QWidget* parent, const char* name) : FFuQtWidget(parent,name) {}
};


FuiQtProperties::FuiQtProperties(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  myStartGuide = new FuiQtStartGuide(this);
  myProperty = new FuiQtProperty(this,"Properties");

  myTopology = new FuiQtProperty(this,"Topology");
  myTopologyView = new FuiQtTopologyView(NULL);

  // Heading :

  myHeading = new FuiQtProperty(this,"Heading");
  myTypeField = new FFuQtIOField();
  myIdField = new FFuQtIOField();
  myDescriptionField = new FFuQtLabelField();
  myTagField = new FFuQtLabelField();

  backBtn = new FFuQtToolButton(this,FFuaCmdItem::getCmdItem("cmdId_backward_select"));
  forwardBtn = new FFuQtToolButton(this,FFuaCmdItem::getCmdItem("cmdId_forward_select"));
  helpBtn = new FFuQtToolButton(this,FFuaCmdItem::getCmdItem("cmdId_contextHelp_select"));

  // Reference plane :

  myRefPlane = new FuiQtProperty(NULL,"Reference plane");
  myRefPlanePosition = new FuiQtPositionData();
  myRefPlaneHeightField = new FFuQtIOField();
  myRefPlaneWidthField  = new FFuQtIOField();

  // HPs :

  myHPRatioFrame = new FFuQtLabelFrame();
  myHPRatioField = new FFuQtIOField();

  // Advanced spring characteristics :

  mySpringChar = new FuiQtSpringChar(NULL);

  // Spring :

  mySpring = new FuiQtProperty(NULL,"Axial spring");
  mySpringForce = new FuiQtSprDaForce(NULL);
  mySpringDeflCalc = new FuiQtSpringDefCalc(NULL);

  // Damper :

  myDamperForce = new FuiQtSprDaForce(NULL);

  // Part :

  FFuQtTabbedWidgetStack* qTabs;
  myLinkTabs = qTabs = new FFuQtTabbedWidgetStack(NULL);
  myLinkModelSheet   = new FuiQtLinkModelSheet(qTabs);
  myLinkOriginSheet  = new FuiQtLinkOriginSheet(qTabs);
  myLinkFEnodeSheet  = new FuiQtLinkNodeSheet(qTabs);
  myLinkRedOptSheet  = new FuiQtLinkRedOptSheet(qTabs);
  myLinkLoadSheet    = new FuiQtLinkLoadSheet(qTabs);
  myGenericPartCGSheet = new FuiQtGenericPartCGSheet(qTabs);
  myGenericPartMassSheet = new FuiQtGenericPartMassSheet(qTabs);
  myGenericPartStiffSheet = new FuiQtGenericPartStiffSheet(qTabs);
  myHydrodynamicsSheet    = new FuiQtHydrodynamicsSheet(qTabs);
  myMeshingSheet          = new FuiQtMeshingSheet(qTabs);
  myAdvancedLinkOptsSheet = new FuiQtAdvancedLinkOptsSheet(qTabs);
  myNonlinearLinkOptsSheet = new FuiQtNonlinearLinkOptsSheet(qTabs);

  // Beam :

  myBeam = new FuiQtProperty(NULL,"Beam");
  myBeamCrossSectionDefField = new FuiQtQueryInputField(NULL,"Cross section:",
                                                        true);
  myBeamMassField   = new FFuQtLabelField();
  myBeamLengthField = new FFuQtLabelField();
  myBeamVisualize3DButton = new FFuQtToggleButton();
  for (FFuLabelField*& field : myBeamVisualize3DFields)
    field = new FFuQtLabelField();
  myBeamOrientationFrame = new FFuQtLabelFrame();
  for (FFuLabelField*& field : myBeamLocalZField)
    field = new FFuQtLabelField();
  myBeamDynProps = new FuiQtDynamicProperties();

  // Turbine :

  myTurbineWindRefFrame     = new FFuQtLabelFrame();
  myTurbineAdvTopologyFrame = new FFuQtLabelFrame();
  myTurbineWindVertOffsetField = new FFuQtIOField();
  myTurbineFields[0] = new FuiQtQueryInputField(NULL,"Reference triad",true);
  myTurbineFields[1] = new FuiQtQueryInputField(NULL,"Yaw point triad",true);
  myTurbineFields[2] = new FuiQtQueryInputField(NULL,"Hub apex triad",true);
  myTurbineFields[3] = new FuiQtQueryInputField(NULL,"Hub part triad",true);
  myTurbineFields[4] = new FuiQtQueryInputField(NULL,"First bearing",true);

  // Generator :

  myGeneratorFrame            = new FFuQtLabelFrame();
  myGeneratorTorqueRadioBtn   = new FFuQtRadioButton();
  myGeneratorVelocityRadioBtn = new FFuQtRadioButton();
  myGeneratorTorqueField      = new FuiQtQueryInputField(NULL);
  myGeneratorVelocityField    = new FuiQtQueryInputField(NULL);

  // Shaft :

  myShaftCrossSectionDefField = new FuiQtQueryInputField(NULL,"Cross section:",
                                                         true);
  myShaftDynProps             = new FuiQtDynamicProperties();
  myShaftNote                 = new FFuQtNotes();

  // Blades :

  myBladePitchControlFrame    = new FFuQtLabelFrame();
  myBladePitchControlField    = new FuiQtQueryInputField(NULL);
  myBladeFixedPitchToggle     = new FFuQtToggleButton();
  myBladeIceFrame             = new FFuQtLabelFrame();
  myBladeIceLayerToggle       = new FFuQtToggleButton();
  myBladeIceThicknessField    = new FFuQtLabelField();

  // Riser :

  myRiserInternalToDefField = new FuiQtQueryInputField(NULL,
                                                       "Is internal to:",true);
  myRiserVisualize3DStartAngleField = new FFuQtLabelField();
  myRiserVisualize3DStopAngleField  = new FFuQtLabelField();
  myRiserMudFrame             = new FFuQtLabelFrame();
  myRiserMudButton            = new FFuQtToggleButton();
  myRiserMudDensityField      = new FFuQtLabelField();
  myRiserMudLevelField        = new FFuQtLabelField();

  // Joints :

  myJointTabs = qTabs = new FFuQtTabbedWidgetStack(NULL);
  myJointSummary      = new FuiQtJointSummary(qTabs);
  myJointPosition     = new FuiQtPositionData(qTabs);
  myJointAdvancedTab  = new FuiQtJointTabAdvanced(qTabs);
  myJointResults      = new FuiQtJointResults(qTabs);

  for (FuiJointDOF*& dof : myJointDofs)
    dof = new FuiQtJointDOF(NULL);

  mySwapTriadButton = new FFuQtPushButton();
  myAddMasterButton = new FFuQtPushButton();
  myRevMasterButton = new FFuQtPushButton();

  // Pipe surface :

  myPipeRadiusField = new FFuQtLabelField();

  // Triad :

  myTriadTabs = qTabs = new FFuQtTabbedWidgetStack(NULL);
  myTriadSummary      = new FuiQtTriadSummary(qTabs);
  myTriadPosition     = new FuiQtPositionData(qTabs);
  myTriadResults      = new FuiQtTriadResults(qTabs);

  for (FuiTriadDOF*& dof : myTriadDofs)
    dof = new FuiQtTriadDOF(NULL);

  // Load :

  myLoad = new FuiQtProperty(NULL,"Load");
  myLoadMagnitude = new FuiQtQueryInputField(NULL);
  myAttackPointEditor = new FuiQtPointEditor(NULL);
  myFromPointEditor = new FuiQtPointEditor(NULL);
  myToPointEditor = new FuiQtPointEditor(NULL);

  // Generic DB object :

  myGenDBObject = new FuiQtProperty(NULL,"Generic DB object");
  myGenDBObjTypeField = new FFuQtIOField();
  myGenDBObjDefField = new FFuQtMemo();

  // File reference :

  myFileReferenceBrowseField = new FFuQtFileBrowseField(NULL);

  // Tire :

  myTire = new FuiQtProperty(NULL,"Tire");
  myTireDataFileField    = new FuiQtQueryInputField(NULL);
  myBrowseTireFileButton = new FFuQtPushButton();
  myRoadField            = new FuiQtQueryInputField(NULL);
  myTireModelMenu        = new FFuQtOptionMenu();
  mySpindelOffset        = new FFuQtIOField();

  // Road :

  myRoad = new FuiQtProperty(NULL,"Road");
  myRoadFuncField        = new FuiQtQueryInputField(NULL);
  for (FFuIOField*& fld : myRoadFields) fld = new FFuQtIOField();
  myUseFuncRoadRadio     = new FFuQtRadioButton();
  myUseFileRoadRadio     = new FFuQtRadioButton();
  myRoadDataFileField    = new FuiQtQueryInputField(NULL);
  myBrowseRoadFileButton = new FFuQtPushButton();

  // Beam material properties :

  myMatPropFrame    = new FFuQtLabelFrame();
  myMatPropRhoField = new FFuQtLabelField();
  myMatPropEField   = new FFuQtLabelField();
  myMatPropNuField  = new FFuQtLabelField();
  myMatPropGField   = new FFuQtLabelField();

  // Beam properties :

  myBeamPropTabs = qTabs = new FFuQtTabbedWidgetStack(NULL);
  myBeamPropSummary      = new FuiQtBeamPropSummary(qTabs);
  myBeamPropHydro        = new FuiQtBeamPropHydro(qTabs);

  // Strain rosette :

  myStrainRosette = new FuiQtProperty(NULL,"Strain rosette");
  myStrRosTypeMenu = new FFuQtOptionMenu();
  myResetStartStrainsToggle = new FFuQtToggleButton();
  myStrRosNodesField = new FFuQtIOField();
  myStrRosEditNodesButton = new FFuQtPushButton();
  myStrRosAngleField = new FFuQtLabelField();
  myStrRosEditDirButton = new FFuQtPushButton();
  myStrRosUseFEHeightToggle = new FFuQtToggleButton();
  myStrRosHeightField = new FFuQtLabelField();
  myStrRosFlipZButton = new FFuQtPushButton();
  myStrRosUseFEMatToggle = new FFuQtToggleButton();
  myStrRosEmodField = new FFuQtLabelField();
  myStrRosNuField = new FFuQtLabelField();

  // Element group :

  myFatigueFrame = new FFuQtLabelFrame();
  myFatigueToggle = new FFuQtToggleButton();
#ifdef FT_HAS_GRAPHVIEW
  mySNSelector = new FuiQtSNCurveSelector(NULL);
#endif
  myScfField = new FFuQtLabelField();

  // RAO vessel motion :

  myRAO = new FuiQtProperty(NULL,"RAO");
  myRAOFileField        = new FuiQtQueryInputField(NULL);
  myBrowseRAOFileButton = new FFuQtPushButton();
  myWaveFuncField       = new FuiQtQueryInputField(NULL);
  myWaveDirMenu         = new FFuQtOptionMenu();
  myMotionScaleField    = new FuiQtQueryInputField(NULL);

  // Sea state :

  mySeaState = new FuiQtProperty(NULL,"Sea state");
  mySeaStateWidthField  = new FFuQtLabelField();
  mySeaStateHeightField  = new FFuQtLabelField();
  mySeaStateWidthPosField = new FFuQtLabelField();
  mySeaStateHeightPosField = new FFuQtLabelField();
  mySeaStateNumPoints = new FFuQtLabelField();
  mySeaStateShowGridToggle = new FFuQtToggleButton();
  mySeaStateShowSolidToggle = new FFuQtToggleButton();

  // Simulation event :

  mySimEvent = new FuiQtProperty(NULL,"Simulation event");
  myEventProbability = new FFuQtLabelField();
  mySelectEventButton = new FFuQtPushButton();
  myActiveEventLabel = new FFuQtLabel();

  // Subassembly :

  mySubassembly = new FuiQtProperty(NULL,"Subassembly");
  mySubassFileField = new FFuQtFileBrowseField(NULL);
  mySubassPosition = new FuiQtPositionData(NULL);
  mySubassMassField = new FFuQtLabelField();
  mySubassCoGFrame = new FFuQtLabelFrame();
  mySubassCoGField = new FuiQt3DPoint(NULL);
  mySubassLengthField = new FFuQtLabelField();
  myVisualize3DButton = new FFuQtToggleButton();

  // Function properties

  myFunctionProperties = new FuiQtFunctionProperties(NULL);

  // Control system properties

  myCtrlElementProperties = new FuiQtCtrlElemProperties(NULL);
#ifdef FT_HAS_EXTCTRL
  myExtCtrlSysProperties = new FuiQtExtCtrlSysProperties(NULL);
#endif

  // Animation, graph and curve properties

  myAnimationDefine = new FuiQtAnimationDefine(NULL);
#ifdef FT_HAS_GRAPHVIEW
  myGraphDefine = new FuiQtGraphDefine(NULL);
  myCurveDefine = new FuiQtCurveDefine(NULL);
#endif

  // Parent class component initiation

  this->initWidgets();

  ////////////////////////////////////////////////////////
  // Positioning of property widgets using layout managers
  ////////////////////////////////////////////////////////

  QBoxLayout* layout;
  QBoxLayout* layout2;
  QGridLayout* gLayout;

  // Heading

  layout = new QHBoxLayout(myHeading->getQtWidget());
  layout->setContentsMargins(2,0,0,0);
  layout->addWidget(myTypeField->getQtWidget(),5);
  layout->addWidget(myIdField->getQtWidget(),1);
  layout->addWidget(myDescriptionField->getQtWidget(),10);
  layout->addWidget(myTagField->getQtWidget(),5);

  // Reference plane

  QGroupBox* qRefPlaneSizeFrame = new QGroupBox("Size");
  qRefPlaneSizeFrame->setMaximumWidth(100);
  gLayout = new QGridLayout(qRefPlaneSizeFrame);
  gLayout->setContentsMargins(5,0,5,5);
  gLayout->addWidget(new QLabel("Height"), 0,0);
  gLayout->addWidget(new QLabel("Width"),  1,0);
  gLayout->addWidget(myRefPlaneHeightField->getQtWidget(), 0,1);
  gLayout->addWidget(myRefPlaneWidthField->getQtWidget(),  1,1);

  layout = new QVBoxLayout(myRefPlane->getQtWidget());
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(2);
  layout->addWidget(myRefPlanePosition->getQtWidget(),3);
  layout->addWidget(qRefPlaneSizeFrame,2);

  // Higher Pairs

  layout = new QVBoxLayout(myHPRatioFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myHPRatioField->getQtWidget());

  // Axial spring

  layout = new QHBoxLayout(mySpring->getQtWidget());
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(mySpringDeflCalc->getQtWidget(),1,Qt::AlignTop);
  layout->addWidget(mySpringForce->getQtWidget(),1,Qt::AlignTop);

  // Beam

  QLayout* layout3 = new QHBoxLayout();
  layout3->setContentsMargins(0,0,0,0);
  for (FFuLabelField* field : myBeamVisualize3DFields)
    layout3->addWidget(field->getQtWidget());

  QGroupBox* qGeneral = new QGroupBox("General");
  layout = new QVBoxLayout(qGeneral);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myBeamCrossSectionDefField->getQtWidget());
  layout->addStretch(1);
  layout->addWidget(myBeamMassField->getQtWidget());
  layout->addWidget(myBeamLengthField->getQtWidget());
  layout->addStretch(1);
  layout->addWidget(myBeamVisualize3DButton->getQtWidget());
  layout->addLayout(layout3);

  layout = new QHBoxLayout(myBeamOrientationFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  for (FFuLabelField* field : myBeamLocalZField)
    layout->addWidget(field->getQtWidget());

  layout2 = new QVBoxLayout();
  layout2->setContentsMargins(0,0,0,0);
  layout2->addWidget(myBeamDynProps->getQtWidget());
  layout2->addWidget(myBeamOrientationFrame->getQtWidget());
  layout2->addStretch(1);

  layout = new QHBoxLayout(myBeam->getQtWidget());
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qGeneral,1);
  layout->addLayout(layout2,2);

  // Turbine

  layout = new QVBoxLayout(myTurbineWindRefFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myTurbineFields.front()->getQtWidget());
  layout->addWidget(new QLabel("Vertical offset"));
  layout->addWidget(myTurbineWindVertOffsetField->getQtWidget());

  gLayout = new QGridLayout(myTurbineAdvTopologyFrame->getQtWidget());
  gLayout->setContentsMargins(5,0,5,5);
  for (int r = 0; r < 2; r++)
    for (int c = 0; c < 2; c++)
      gLayout->addWidget(myTurbineFields[1+r+2*c]->getQtWidget(), r,c);

  QWidget* qTurbine = new QWidget();
  layout = new QHBoxLayout(qTurbine);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(myTurbineWindRefFrame->getQtWidget(),1);
  layout->addWidget(myTurbineAdvTopologyFrame->getQtWidget(),2);

  // Generator

  gLayout = new QGridLayout(myGeneratorFrame->getQtWidget());
  gLayout->setContentsMargins(5,0,5,5);
  gLayout->addWidget(myGeneratorTorqueRadioBtn->getQtWidget(), 0,0);
  gLayout->addWidget(myGeneratorVelocityRadioBtn->getQtWidget(), 1,0);
  gLayout->addWidget(myGeneratorTorqueField->getQtWidget(), 0,1);
  gLayout->addWidget(myGeneratorVelocityField->getQtWidget(), 1,1);

  // Shaft

  layout2 = new QHBoxLayout();
  layout2->setContentsMargins(0,0,0,0);
  layout2->addWidget(myRiserMudFrame->getQtWidget());
  layout2->addWidget(myShaftDynProps->getQtWidget());

  QWidget* qShaftRight = new QWidget();
  layout = new QVBoxLayout(qShaftRight);
  layout->setContentsMargins(0,0,0,0);
  layout->addLayout(layout2);
  layout->addWidget(myShaftNote->getQtWidget());
  layout->addStretch(1);

  // Blades

  layout = new QVBoxLayout(myBladePitchControlFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myBladePitchControlField->getQtWidget());
  layout->addWidget(myBladeFixedPitchToggle->getQtWidget());

  layout = new QVBoxLayout(myBladeIceFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myBladeIceLayerToggle->getQtWidget());
  layout->addWidget(myBladeIceThicknessField->getQtWidget());

  // Riser

  layout = new QVBoxLayout(myRiserMudFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myRiserMudButton->getQtWidget());
  layout->addWidget(myRiserMudDensityField->getQtWidget());
  layout->addWidget(myRiserMudLevelField->getQtWidget());

  // Load

  QGroupBox* qMagnitude = new QGroupBox("Magnitude");
  layout = new QVBoxLayout(qMagnitude);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myLoadMagnitude->getQtWidget());

  QGroupBox* qLoadTarget = new QGroupBox("Load target point");
  layout = new QVBoxLayout(qLoadTarget);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myAttackPointEditor->getQtWidget());

  QGroupBox* qDirection = new QGroupBox("Direction");
  gLayout = new QGridLayout(qDirection);
  gLayout->setContentsMargins(5,0,5,5);
  gLayout->setRowStretch(1,1);
  gLayout->addWidget(new QLabel("From"), 0,0);
  gLayout->addWidget(new QLabel("To"), 0,1);
  gLayout->addWidget(myFromPointEditor->getQtWidget(), 1,0);
  gLayout->addWidget(myToPointEditor->getQtWidget(), 1,1);

  layout2 = new QVBoxLayout();
  layout2->setContentsMargins(0,0,0,0);
  layout2->addWidget(qMagnitude);
  layout2->addWidget(qLoadTarget);

  layout = new QHBoxLayout(myLoad->getQtWidget());
  layout->setContentsMargins(0,0,0,0);
  layout->addLayout(layout2);
  layout->addWidget(qDirection);

  // Generic DB object

  gLayout = new QGridLayout(myGenDBObject->getQtWidget());
  gLayout->setContentsMargins(0,0,0,0);
  gLayout->setVerticalSpacing(2);
  gLayout->setColumnStretch(1,1);
  gLayout->setRowStretch(1,1);
  gLayout->addWidget(new QLabel("Type"), 0,0);
  gLayout->addWidget(new QLabel("Definition"), 1,0, Qt::AlignTop);
  gLayout->addWidget(myGenDBObjTypeField->getQtWidget(), 0,1);
  gLayout->addWidget(myGenDBObjDefField->getQtWidget(), 1,1);

  // Tire

  gLayout = new QGridLayout(myTire->getQtWidget());
  gLayout->setContentsMargins(0,0,0,0);
  gLayout->setColumnStretch(1,1);
  gLayout->addWidget(new QLabel("Tire file"), 0,0);
  gLayout->addWidget(new QLabel("Road"), 1,0);
  gLayout->addWidget(new QLabel("Tire model"), 2,0, Qt::AlignVCenter);
  gLayout->addWidget(new QLabel("Z offset"), 3,0, Qt::AlignVCenter);
  gLayout->addWidget(myTireDataFileField->getQtWidget(), 0,1);
  gLayout->addWidget(myRoadField->getQtWidget(), 1,1);
  gLayout->addWidget(myTireModelMenu->getQtWidget(), 2,1, Qt::AlignVCenter);
  gLayout->addWidget(mySpindelOffset->getQtWidget(), 3,1, Qt::AlignVCenter);
  gLayout->addWidget(myBrowseTireFileButton->getQtWidget(), 0,2);

  // Road

  gLayout = new QGridLayout(myRoad->getQtWidget());
  gLayout->setContentsMargins(0,0,0,0);
  gLayout->setVerticalSpacing(2);
  gLayout->setColumnStretch(2,1);
  gLayout->addWidget(myUseFuncRoadRadio->getQtWidget(), 0,0,1,2);
  gLayout->addWidget(myUseFileRoadRadio->getQtWidget(), 5,0,1,2, Qt::AlignVCenter);
  gLayout->addWidget(new QLabel("      Road function"), 1,0);
  gLayout->addWidget(new QLabel("      Road file"), 6,0, Qt::AlignVCenter);
  gLayout->addWidget(myRoadFuncField->getQtWidget(), 1,1,1,3);
  gLayout->addWidget(new QLabel("Rotation about Z-axis"), 2,1);
  gLayout->addWidget(new QLabel("Vertical shift"), 3,1);
  gLayout->addWidget(new QLabel("Horizontal offset"), 4,1);
  for (size_t i = 0; i < 3; i++)
    gLayout->addWidget(myRoadFields[i]->getQtWidget(), 2+i,2);
  gLayout->addWidget(myRoadDataFileField->getQtWidget(), 6,1,1,2, Qt::AlignVCenter);
  gLayout->addWidget(myBrowseRoadFileButton->getQtWidget(), 6,3, Qt::AlignVCenter);

  // Material properties

  layout = new QVBoxLayout(myMatPropFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myMatPropRhoField->getQtWidget());
  layout->addWidget(myMatPropEField->getQtWidget());
  layout->addWidget(myMatPropNuField->getQtWidget());
  layout->addWidget(myMatPropGField->getQtWidget());

  // Strain rosette

  QWidget* qRosette = new QWidget();
  gLayout = new QGridLayout(qRosette);
  gLayout->setContentsMargins(0,0,0,0);
  gLayout->addWidget(new QLabel("Rosette type"), 0,0);
  gLayout->addWidget(new QLabel("Nodes"), 1,0);
  gLayout->addWidget(myStrRosTypeMenu->getQtWidget(), 0,1);
  gLayout->addWidget(myStrRosNodesField->getQtWidget(), 1,1);
  gLayout->addWidget(myResetStartStrainsToggle->getQtWidget(), 0,2);
  gLayout->addWidget(myStrRosEditNodesButton->getQtWidget(), 1,2);

  QGroupBox* qOrient = new QGroupBox("Orientation");
  layout = new QHBoxLayout(qOrient);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myStrRosAngleField->getQtWidget());
  layout->addWidget(myStrRosEditDirButton->getQtWidget());

  QGroupBox* qLayerPos = new QGroupBox("Layer position");
  layout = new QVBoxLayout(qLayerPos);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myStrRosUseFEHeightToggle->getQtWidget());
  layout->addWidget(myStrRosHeightField->getQtWidget());
  layout->addWidget(myStrRosFlipZButton->getQtWidget());

  QGroupBox* qMaterial = new QGroupBox("Material");
  layout = new QVBoxLayout(qMaterial);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myStrRosUseFEMatToggle->getQtWidget());
  layout->addWidget(myStrRosEmodField->getQtWidget());
  layout->addWidget(myStrRosNuField->getQtWidget());

  gLayout = new QGridLayout(myStrainRosette->getQtWidget());
  gLayout->setContentsMargins(0,0,0,0);
  gLayout->addWidget(qRosette, 0,0,1,2);
  gLayout->addWidget(qOrient, 1,0,1,2);
  gLayout->addWidget(qLayerPos, 2,0);
  gLayout->addWidget(qMaterial, 2,1);

  gLayout = new QGridLayout(myFatigueFrame->getQtWidget());
  gLayout->setContentsMargins(5,0,5,5);
  gLayout->setHorizontalSpacing(20);
  gLayout->setColumnStretch(0,3);
  gLayout->setColumnStretch(1,2);
  gLayout->addWidget(myFatigueToggle->getQtWidget(), 0,0,1,2);
#ifdef FT_HAS_GRAPHVIEW
  gLayout->addWidget(mySNSelector->getQtWidget(), 1,0);
#endif
  gLayout->addWidget(myScfField->getQtWidget(), 1,1);

  // RAO vessel motion

  gLayout = new QGridLayout(myRAO->getQtWidget());
  gLayout->setContentsMargins(0,0,0,0);
  gLayout->setColumnStretch(1,1);
  gLayout->addWidget(new QLabel("RAO file"), 0,0);
  gLayout->addWidget(new QLabel("Wave function"), 1,0);
  gLayout->addWidget(new QLabel("Wave direction"), 1,2);
  gLayout->addWidget(new QLabel("Motion scale"), 2,0);
  gLayout->addWidget(myRAOFileField->getQtWidget(), 0,1,1,2);
  gLayout->addWidget(myBrowseRAOFileButton->getQtWidget(), 0,3);
  gLayout->addWidget(myWaveFuncField->getQtWidget(), 1,1);
  gLayout->addWidget(myWaveDirMenu->getQtWidget(), 1,3);
  gLayout->addWidget(myMotionScaleField->getQtWidget(), 2,1,1,3);

  // Sea state

  QGroupBox* qSeaPos = new QGroupBox("Position");
  layout = new QVBoxLayout(qSeaPos);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(mySeaStateWidthPosField->getQtWidget());
  layout->addWidget(mySeaStateHeightPosField->getQtWidget());

  QGroupBox* qSeaSize = new QGroupBox("Size");
  layout = new QVBoxLayout(qSeaSize);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(mySeaStateWidthField->getQtWidget());
  layout->addWidget(mySeaStateHeightField->getQtWidget());

  QGroupBox* qSeaViz = new QGroupBox("Visualization");
  layout = new QVBoxLayout(qSeaViz);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(mySeaStateNumPoints->getQtWidget());
  layout->addWidget(mySeaStateShowSolidToggle->getQtWidget(),0,Qt::AlignTop);
  layout->addWidget(mySeaStateShowGridToggle->getQtWidget(),0,Qt::AlignTop);

  gLayout = new QGridLayout(mySeaState->getQtWidget());
  gLayout->setContentsMargins(0,0,0,0);
  gLayout->addWidget(qSeaPos, 0,0);
  gLayout->addWidget(qSeaSize, 1,0);
  gLayout->addWidget(qSeaViz, 0,1,2,1);

  layout2 = new QHBoxLayout();
  layout2->setContentsMargins(0,0,0,0);
  layout2->addWidget(myEventProbability->getQtWidget(),1);
  layout2->addStretch(1);
  layout = new QVBoxLayout(mySimEvent->getQtWidget());
  layout->setContentsMargins(0,0,0,0);
  layout->addLayout(layout2);
  layout->addWidget(mySelectEventButton->getQtWidget());
  layout->addWidget(myActiveEventLabel->getQtWidget());

  // Subassembly

  layout = new QVBoxLayout(mySubassCoGFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(mySubassCoGField->getQtWidget());

  layout3 = new QHBoxLayout();
  layout3->setContentsMargins(0,0,0,0);
  layout3->addWidget(myRiserVisualize3DStartAngleField->getQtWidget());
  layout3->addWidget(myRiserVisualize3DStopAngleField->getQtWidget());

  QWidget* qLeft = new QWidget();
  layout = new QVBoxLayout(qLeft);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(myBladePitchControlFrame->getQtWidget());
  layout->addWidget(myShaftCrossSectionDefField->getQtWidget());
  layout->addWidget(myRiserInternalToDefField->getQtWidget());
  layout->addStretch(1);
  layout->addWidget(mySubassMassField->getQtWidget());
  layout->addWidget(mySubassLengthField->getQtWidget());
  layout->addStretch(1);
  layout->addWidget(mySubassCoGFrame->getQtWidget());
  layout->addWidget(myBladeIceFrame->getQtWidget());
  layout->addStretch(1);
  layout->addWidget(myVisualize3DButton->getQtWidget());
  layout->addLayout(layout3);

  QWidget* qRight = new QWidget();
  layout = new QVBoxLayout(qRight);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qShaftRight);
  layout->addWidget(mySubassPosition->getQtWidget());
  layout->addWidget(qTurbine);
  layout->addWidget(myGeneratorFrame->getQtWidget());
  layout->addStretch(1);

  layout2 = new QHBoxLayout();
  layout2->setContentsMargins(0,0,0,0);
  layout2->setSpacing(15);
  layout2->addWidget(qLeft);
  layout2->addWidget(qRight);
  layout = new QVBoxLayout(mySubassembly->getQtWidget());
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(mySubassFileField->getQtWidget());
  layout->addLayout(layout2);

  // Topology view

  layout2 = new QHBoxLayout();
  layout2->setContentsMargins(0,0,0,0);
  layout2->addWidget(mySwapTriadButton->getQtWidget());
  layout2->addWidget(myAddMasterButton->getQtWidget());
  layout2->addWidget(myRevMasterButton->getQtWidget());
  layout = new QVBoxLayout(myTopology->getQtWidget());
  layout->setContentsMargins(2,0,0,0);
  layout->addWidget(myTopologyView->getQtWidget());
  layout->addLayout(layout2);

  // Property panel

  layout = new QHBoxLayout(myProperty->getQtWidget());
  layout->setContentsMargins(2,2,4,0);
  layout->addWidget(myRefPlane->getQtWidget());
  layout->addWidget(myHPRatioFrame->getQtWidget(),0,Qt::AlignLeft|Qt::AlignTop);
  layout->addWidget(mySpringChar->getQtWidget());
  layout->addWidget(mySpring->getQtWidget());
  layout->addWidget(myDamperForce->getQtWidget(),0,Qt::AlignTop);
  layout->addWidget(myLinkTabs->getQtWidget());
  layout->addWidget(myTriadTabs->getQtWidget());
  layout->addWidget(myJointTabs->getQtWidget());
  layout->addWidget(myPipeRadiusField->getQtWidget(),0,Qt::AlignLeft|Qt::AlignTop);
  layout->addWidget(myBeam->getQtWidget());
  layout->addWidget(myLoad->getQtWidget());
  layout->addWidget(myGenDBObject->getQtWidget());
  layout->addWidget(myFileReferenceBrowseField->getQtWidget(),0,Qt::AlignTop);
  layout->addWidget(myTire->getQtWidget());
  layout->addWidget(myRoad->getQtWidget());
  layout->addWidget(myMatPropFrame->getQtWidget(),0,Qt::AlignLeft|Qt::AlignTop);
  layout->addWidget(myBeamPropTabs->getQtWidget());
  layout->addWidget(myStrainRosette->getQtWidget());
  layout->addWidget(myFatigueFrame->getQtWidget(),0,Qt::AlignTop);
  layout->addWidget(myRAO->getQtWidget(),0,Qt::AlignTop);
  layout->addWidget(mySeaState->getQtWidget());
  layout->addWidget(mySimEvent->getQtWidget());
  layout->addWidget(mySubassembly->getQtWidget());

  layout->addWidget(myFunctionProperties->getQtWidget());
  layout->addWidget(myCtrlElementProperties->getQtWidget());
#ifdef FT_HAS_EXTCTRL
  layout->addWidget(myExtCtrlSysProperties->getQtWidget());
#endif
  layout->addWidget(myAnimationDefine->getQtWidget());
#ifdef FT_HAS_GRAPHVIEW
  layout->addWidget(myGraphDefine->getQtWidget());
  layout->addWidget(myCurveDefine->getQtWidget());
#endif

  myHeading->popDown();
  myProperty->popDown();
}


void FuiQtProperties::showEvent(QShowEvent* e)
{
  this->QWidget::showEvent(e);
  this->updateUIValues();
}


void FuiQtProperties::resizeEvent(QResizeEvent* e)
{
  this->QWidget::resizeEvent(e);
  this->placeWidgets(this->width(),this->height());
}


#if defined(win32) || defined(win64)
// This does not seem to work any longer with Qt 6.
// Anyway, it is currently unused, so consider remove.
static bool showPDFfileAtPage(QString& strUrl)
{
  // Get AcroRd32.exe
  QString strAR;
  // Open registry key to find the registered PDF-file reader
  const wchar_t* regkey = L"AcroExch.Document\\Shell\\Open\\Command";
  HKEY hk;
  LONG err = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, regkey, 0, KEY_QUERY_VALUE, &hk);
  if (err == ERROR_SUCCESS) {
    // Get registry value
    char buf[2048];
    DWORD cbData = 2048;
    memset(buf,0,cbData);
    err = ::RegQueryValueEx(hk, NULL, NULL, NULL, (LPBYTE)buf, &cbData);
    ::RegCloseKey(hk);
    if (err == ERROR_SUCCESS)
      strAR = buf;
  }
  if (strAR.isEmpty()) {
    std::string errmsg("Unable to find Adobe Acrobat Reader "
                       "when opening PDF-file:\n");
    errmsg.append(strUrl.replace("file://","").toStdString());
    errmsg.append("\n\nWould you like to try to open it using another reader?");
    return FFaMsg::dialog(errmsg,FFaMsg::YES_NO) == 0;
  }

  QStringList listAR = strAR.split('\"');
  strAR = listAR[strAR.startsWith('\"') ? 1 : 0];
  // Get pdf file path and page number
  QStringList listUrl = strUrl.split('?');
  listUrl[0].replace("file://","");
  // Show pdf file at the specified page
  QProcess* myProcess = new QProcess();
  QStringList arguments;
  if (listUrl.size() > 1)
    arguments << "/A" << listUrl[1];
  arguments << QApplication::applicationDirPath() + "/" + listUrl[0];
  myProcess->start(strAR,arguments);
  return true;
}
#endif


static void onURLActivated(const std::string& url)
{
  QString strUrl(url.c_str());
  if (strUrl.left(7) == "file://")
  {
    if (strUrl.indexOf("?") > 0)
    {
#if defined(win32) || defined(win64)
      if (showPDFfileAtPage(strUrl)) return;
#endif
      strUrl = strUrl.split('?').first();
    }
    strUrl.insert(7, "/" + QApplication::applicationDirPath() + "/").replace(" ", "%20");
  }

  std::string msg;
  QUrl objUrl(strUrl,QUrl::StrictMode);
  if (!objUrl.isValid())
  {
    msg = "Invalid URL:\n";
    std::cerr <<"QUrl: "<< objUrl.errorString().toStdString() << std::endl;
  }
  else if (!QDesktopServices::openUrl(objUrl))
    msg = "Unable to open URL:\n";
  else
    return;

  FFaMsg::dialog(msg + strUrl.toStdString(), FFaMsg::WARNING);
}


bool FuiQtStartGuide::initWidgets()
{
  // Load HTML file
  QString appPath = QApplication::applicationDirPath() + "/Doc/";
  QString htmFile = appPath + "StartGuide.htm";
  QString strData;
  QFile file(htmFile);
  bool fileOk = file.open(QIODevice::ReadOnly | QIODevice::Text);
  if (!fileOk)
  {
    std::cerr <<" *** Can't open file "<< htmFile.toStdString() << std::endl;
    strData = "Welcome to <i>FEDEM 8.1</i>";
  }
  else while (!file.atEnd())
    strData.append(file.readLine().data());
  file.close();

  // Change all relative local paths
  strData.replace(" src='", " src='" + appPath);
  // Set fields
  myLogoImage->setPixMap(startGuideLogo_xpm,true);
  myHeading->setLabel("<font color='#008cff' size='5'><i><b>Welcome to FEDEM 8.1</b></i></font>");
  myContentLabel->setLabel(strData.toStdString());
  myContentLabel->setLinkActivatedCB(FFaDynCB1S(onURLActivated,const std::string&));
  myLogoBorderRight->setPixMap(startGuideBorderRight_xpm,true);
  FFuaPalette pal;
  pal.setStdBackground(255,255,255);
  myContentLabel->setColors(pal);
  myBorderTop->setPixMap(startGuideBorderTop_xpm,true);
  pal.setStdBackground(130,135,144);
  myBorderRight->setColors(pal);
  myBorderBottom->setColors(pal);
  pal.setStdBackground(113,178,254);
  myLogoBorderTop->setColors(pal);
  return fileOk;
}


void FuiProperties::showPDF(const char* url)
{
  onURLActivated(url);
}


void FuiProperties::showComApiReference()
{
  // Show CHM.
  // Example: "HH.EXE ms-its:C:/path/ComAPI.chm::/Introduction.html"
  QProcess* myProcess = new QProcess();
  QStringList arguments;
  arguments << "ms-its:" + QApplication::applicationDirPath() + "/Doc/ComAPI.chm::/Introduction.html";
  myProcess->start("hh.exe", arguments);
}
