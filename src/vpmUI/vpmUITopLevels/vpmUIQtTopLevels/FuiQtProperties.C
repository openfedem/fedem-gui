// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtProperties.H"
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

#if defined(win32) || defined(win64)
#include <windows.h>
#endif


FuiQtProperties::FuiQtProperties(QWidget* parent,
                                 int xpos, int ypos,
                                 int width, int height,
                                 const char* name)
  : FFuQtMultUIComponent(parent,xpos,ypos,width,height,name)
{
  // Heading :

  myTypeField = new FFuQtIOField(this);
  myIdField = new FFuQtIOField(this);
  myDescriptionField = new FFuQtLabelField(this);
  myTagField = new FFuQtLabelField(this);
  myTopologyView = new FuiQtTopologyView(this);
  backBtn = new FFuQtToolButton(this,FFuaCmdItem::getCmdItem("cmdId_backward_select"));
  forwardBtn = new FFuQtToolButton(this,FFuaCmdItem::getCmdItem("cmdId_forward_select"));
  helpBtn = new FFuQtToolButton(this,FFuaCmdItem::getCmdItem("cmdId_contextHelp_select"));

  // Reference plane :

  myRefPlanePosition = new FuiQtPositionData(this);
  myRefPlaneSizeFrame = new FFuQtLabelFrame(this);
  myRefPlaneHeightField = new FFuQtLabelField(this);
  myRefPlaneWidthField = new FFuQtLabelField(this);

  // HPs :

  myHPRatioFrame = new FFuQtLabelFrame(this);
  myHPRatioField = new FFuQtIOField(this);

  // Advanced spring characteristics :

  mySpringChar = new FuiQtSpringChar(this);

  // Spring :

  mySpringForce    = new FuiQtSprDaForce(this);
  mySpringDeflCalc = new FuiQtSpringDefCalc(this);

  // Damper :

  myDamperForce = new FuiQtSprDaForce(this);

  // Part :

  FFuQtTabbedWidgetStack* qTabs;
  myLinkTabs = qTabs = new FFuQtTabbedWidgetStack(this);
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

  // Beam and Shaft :

  myShaftGeneralFrame         = new FFuQtLabelFrame(this);
  myShaftCrossSectionDefLabel = new FFuQtLabel(this);
  myShaftCrossSectionDefField = new FuiQtQueryInputField(this);
  myShaftCrossSectionDefField->setTextForNoRefSelected("");
  myShaftSDFrame              = new FFuQtLabelFrame(this);
  myShaftSDMassField          = new FFuQtLabelField(this);
  myShaftSDStiffnessField     = new FFuQtLabelField(this);
  myShaftDPFrame              = new FFuQtLabelFrame(this);
  myShaftDPStiffnessField     = new FFuQtLabelField(this);
  myShaftDPMassField          = new FFuQtLabelField(this);
  myShaftNoteALabel           = new FFuQtLabel(this);
  myShaftNoteBLabel           = new FFuQtLabel(this);
  myShaftNoteCLabel           = new FFuQtLabel(this);
  myVisualize3DButton         = new FFuQtToggleButton(this);
  myVisualize3DStartAngleField = new FFuQtLabelField(this);
  myVisualize3DStopAngleField = new FFuQtLabelField(this);
  myBeamOrientationFrame      = new FFuQtLabelFrame(this);
  for (FFuLabelField*& field : myBeamLocalZField)
    field = new FFuQtLabelField(this);

  // Turbine :

  myTurbineWindRefFrame               = new FFuQtLabelFrame(this);
  myTurbineWindRefTriadDefLabel       = new FFuQtLabel(this);
  myTurbineWindRefTriadDefField       = new FuiQtQueryInputField(this);
  myTurbineWindVertOffsetLabel        = new FFuQtLabel(this);
  myTurbineWindVertOffsetField        = new FFuQtIOField(this);
  myTurbineAdvTopologyFrame           = new FFuQtLabelFrame(this);
  myTurbineYawPointTriadDefLabel      = new FFuQtLabel(this);
  myTurbineYawPointTriadDefField      = new FuiQtQueryInputField(this);
  myTurbineHubApexTriadDefLabel       = new FFuQtLabel(this);
  myTurbineHubApexTriadDefField       = new FuiQtQueryInputField(this);
  myTurbineHubPartDefLabel            = new FFuQtLabel(this);
  myTurbineHubPartDefField            = new FuiQtQueryInputField(this);
  myTurbineFirstBearingDefLabel       = new FFuQtLabel(this);
  myTurbineFirstBearingDefField       = new FuiQtQueryInputField(this);

  // Generator :

  myGeneratorTorqueRadioBtn   = new FFuQtRadioButton(this);
  myGeneratorVelocityRadioBtn = new FFuQtRadioButton(this);
  myGeneratorTorqueField      = new FuiQtQueryInputField(this);
  myGeneratorVelocityField    = new FuiQtQueryInputField(this);

  // Blades :

  myBladePitchControlLabel    = new FFuQtLabel(this);
  myBladePitchControlField    = new FuiQtQueryInputField(this);
  myBladeFixedPitchToggle     = new FFuQtToggleButton(this);
  myBladeIceFrame             = new FFuQtLabelFrame(this);
  myBladeIceLayerToggle       = new FFuQtToggleButton(this);
  myBladeIceThicknessField    = new FFuQtLabelField(this);

  // Riser :

  myRiserGeneralFrame         = new FFuQtLabelFrame(this);
  myRiserInternalToDefLabel   = new FFuQtLabel(this);
  myRiserInternalToDefField   = new FuiQtQueryInputField(this);
  myRiserVisualize3DButton    = new FFuQtToggleButton(this);
  myRiserVisualize3DStartAngleField = new FFuQtLabelField(this);
  myRiserVisualize3DStopAngleField = new FFuQtLabelField(this);
  myRiserMudFrame             = new FFuQtLabelFrame(this);
  myRiserMudButton            = new FFuQtToggleButton(this);
  myRiserMudDensityField      = new FFuQtLabelField(this);
  myRiserMudLevelField        = new FFuQtLabelField(this);

  // Joints :

  myJointTabs = qTabs = new FFuQtTabbedWidgetStack(this);
  myJointSummary      = new FuiQtJointSummary(qTabs);
  myJointPosition     = new FuiQtPositionData(qTabs);
  myJointAdvancedTab  = new FuiQtJointTabAdvanced(qTabs);
  myJointResults      = new FuiQtJointResults(qTabs);

  for (FuiJointDOF*& dof : myJointDofs)
    dof = new FuiQtJointDOF(NULL);

  mySwapTriadButton = new FFuQtPushButton(this);
  myAddMasterButton = new FFuQtPushButton(this);
  myRevMasterButton = new FFuQtPushButton(this);

  // Pipe surface :

  myPipeRadiusField = new FFuQtLabelField(this);

  // Triad :

  myTriadTabs = qTabs = new FFuQtTabbedWidgetStack(this);
  myTriadSummary      = new FuiQtTriadSummary(qTabs);
  myTriadPosition     = new FuiQtPositionData(qTabs);
  myTriadResults      = new FuiQtTriadResults(qTabs);

  for (FuiTriadDOF*& dof : myTriadDofs)
    dof = new FuiQtTriadDOF(NULL);

  // Load :

  myMagnitudeLabel = new FFuQtLabelFrame(this);
  myAttackPointFrame = new FFuQtLabelFrame(this);
  myDirectionFrame = new FFuQtLabelFrame(this);
  myAttackPointEditor = new FuiQtPointEditor(this);
  myFromPointEditor = new FuiQtPointEditor(this);
  myToPointEditor = new FuiQtPointEditor(this);
  myFromPointLabel = new FFuQtLabel(this);
  myToPointLabel = new FFuQtLabel(this);
  myLoadMagnitude = new FuiQtQueryInputField(this);

  // Generic DB object :

  myGenDBObjTypeLabel = new FFuQtLabel(this);
  myGenDBObjTypeField = new FFuQtIOField(this);
  myGenDBObjDefLabel = new FFuQtLabel(this);
  myGenDBObjDefField = new FFuQtMemo(this);

  // File reference :

  myFileReferenceBrowseField = new FFuQtFileBrowseField(this);

  // Tire :

  myTireDataFileLabel    = new FFuQtLabel(this);
  myTireDataFileField    = new FuiQtQueryInputField(this);
  myBrowseTireFileButton = new FFuQtPushButton(this);
  myRoadLabel            = new FFuQtLabel(this);
  myRoadField            = new FuiQtQueryInputField(this);
  myTireModelTypeLabel   = new FFuQtLabel(this);
  myTireModelMenu        = new FFuQtOptionMenu(this);
  mySpindelOffsetField   = new FFuQtLabelField(this);

  // Road :

  myRoadFuncLabel        = new FFuQtLabel(this);
  myRoadFuncField        = new FuiQtQueryInputField(this);
  myRoadZShiftField      = new FFuQtLabelField(this);
  myRoadXOffsetField     = new FFuQtLabelField(this);
  myRoadZRotationField   = new FFuQtLabelField(this);

  myUseFuncRoadRadio     = new FFuQtRadioButton(this);
  myUseFileRoadRadio     = new FFuQtRadioButton(this);
  myRoadDataFileLabel    = new FFuQtLabel(this);
  myRoadDataFileField    = new FuiQtQueryInputField(this);
  myBrowseRoadFileButton = new FFuQtPushButton(this);

  // Beam material properties :

  myMatPropRhoField      = new FFuQtLabelField(this);
  myMatPropEField        = new FFuQtLabelField(this);
  myMatPropNuField       = new FFuQtLabelField(this);
  myMatPropGField        = new FFuQtLabelField(this);

  // Sea State

  mySeaStateSizeFrame = new FFuQtLabelFrame(this);
  mySeaStatePositionFrame = new FFuQtLabelFrame(this);
  mySeaStateVisualizationFrame = new FFuQtLabelFrame(this);
  mySeaStateWidthField  = new FFuQtLabelField(this);
  mySeaStateHeightField  = new FFuQtLabelField(this);
  mySeaStateWidthPosField = new FFuQtLabelField(this);
  mySeaStateHeightPosField = new FFuQtLabelField(this);
  mySeaStateNumPoints = new FFuQtLabelField(this);
  mySeaStateShowGridToggle = new FFuQtToggleButton(this);
  mySeaStateShowSolidToggle = new FFuQtToggleButton(this);

  // Beam properties :

  myBeamPropTabs = qTabs = new FFuQtTabbedWidgetStack(this);
  myBeamPropSummary      = new FuiQtBeamPropSummary(qTabs);
  myBeamPropHydro        = new FuiQtBeamPropHydro(qTabs);

  // Strain rosette :

  myStrRosTypeLabel      = new FFuQtLabel(this);
  myStrRosTypeMenu       = new FFuQtOptionMenu(this);
  myStrRosNodesField     = new FFuQtLabelField(this);
  myStrRosEditNodesButton = new FFuQtPushButton(this);
  myStrRosAngleField     = new FFuQtLabelField(this);
  myStrRosEditDirButton  = new FFuQtPushButton(this);
  myStrRosHeightField    = new FFuQtLabelField(this);
  myStrRosUseFEHeightToggle = new FFuQtToggleButton(this);
  myStrRosFlipZButton    = new FFuQtPushButton(this);
  myStrRosEmodField      = new FFuQtLabelField(this);
  myStrRosNuField        = new FFuQtLabelField(this);
  myStrRosUseFEMatToggle = new FFuQtToggleButton(this);
  myResetStartStrainsToggle = new FFuQtToggleButton(this);

  myMaterialFrame        = new FFuQtLabelFrame(this);
  myLayerFrame           = new FFuQtLabelFrame(this);
  myOrientationFrame     = new FFuQtLabelFrame(this);

  // Element group :

  myFatigueFrame  = new FFuQtLabelFrame(this);
  myFatigueToggle = new FFuQtToggleButton(this);
#ifdef FT_HAS_GRAPHVIEW
  mySNSelector    = new FuiQtSNCurveSelector(this);
#endif
  myScfField      = new FFuQtLabelField(this);

  // RAO vessel motion :

  myRAOFileLabel        = new FFuQtLabel(this);
  myRAOFileField        = new FuiQtQueryInputField(this);
  myBrowseRAOFileButton = new FFuQtPushButton(this);
  myWaveFuncLabel       = new FFuQtLabel(this);
  myWaveFuncField       = new FuiQtQueryInputField(this);
  myWaveDirLabel        = new FFuQtLabel(this);
  myWaveDirMenu         = new FFuQtOptionMenu(this);
  myMotionScaleLabel    = new FFuQtLabel(this);
  myMotionScaleField    = new FuiQtQueryInputField(this);

  // Simulation event :

  myEventProbability = new FFuQtLabelField(this);
  mySelectEventButton = new FFuQtPushButton(this);
  myActiveEventLabel = new FFuQtLabel(this);

  // Subassembly :

  mySubassFileField = new FFuQtFileBrowseField(this);
  mySubassPosition = new FuiQtPositionData(this);
  mySubassMassField = new FFuQtLabelField(this);
  mySubassCoGFrame = new FFuQtLabelFrame(this);
  mySubassCoGField = new FuiQt3DPoint(this);
  mySubassLengthField = new FFuQtLabelField(this);

  // Function properties

  myFunctionProperties = new FuiQtFunctionProperties(this);

  // Control system properties

  myCtrlElementProperties = new FuiQtCtrlElemProperties(this);
#ifdef FT_HAS_EXTCTRL
  myExtCtrlSysProperties = new FuiQtExtCtrlSysProperties(this);
#endif

  // Animation, graph and curve properties

  myAnimationDefine = new FuiQtAnimationDefine(this);
#ifdef FT_HAS_GRAPHVIEW
  myGraphDefine = new FuiQtGraphDefine(this);
  myCurveDefine = new FuiQtCurveDefine(this);
#endif

  // Start guide

  mySGLogoImage = new FFuQtLabel(this);
  mySGLogoBorderTop = new FFuQtLabel(this);
  mySGLogoBorderRight = new FFuQtLabel(this);
  mySGFillWhite = new FFuQtLabel(this);
  mySGHeading = new FFuQtLabel(this);
  mySGBorderRight = new FFuQtLabel(this);
  mySGBorderBottom = new FFuQtLabel(this);
  mySGBorderTop = new FFuQtLabel(this);

  FFuQtLabel* qlab;
  mySGContentLabel = qlab = new FFuQtLabel(this);
  qlab->setAlignment(Qt::AlignTop|Qt::AlignLeft);

  // Parent class component initiation

  this->FuiProperties::initWidgets();
}


static void showPDFfile(QString& strUrl)
{
  // Get AcroRd32.exe
  QString strAR;
#if defined(win32) || defined(win64)
  // Open registry key
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
#endif

  if (strAR.isEmpty()) {
    std::string errmsg("Unable to find Adobe Acrobat Reader when opening file:\n");
    strUrl.replace("file://", "");
    errmsg.append(strUrl.toStdString());
    errmsg.append("\n\nWould you like to try open the PDF using an other reader?");
    if (FFaMsg::dialog(errmsg, FFaMsg::YES_NO_CANCEL) == 1) {
      // Try shell open
      if (strUrl.indexOf("?") > 0)
        strUrl = strUrl.left(strUrl.indexOf("?"));
      QUrl objUrl(QApplication::applicationDirPath() + "/" + strUrl);
      if (!QDesktopServices::openUrl(objUrl))
        FFaMsg::dialog("Unable to find any PDF reader!",FFaMsg::OK);
      return;
    }
  }
  QStringList listAR = strAR.split('\"');
  strAR = listAR[strAR.startsWith('\"') ? 1 : 0];
  // Get pdf file path and page number
  QStringList listUrl = strUrl.split('?');
  listUrl[0].replace("file://", "");
  // Show PDF
  QProcess* myProcess = new QProcess();
  QStringList arguments;
  if (listUrl.size() > 1)
    arguments << "/A" << listUrl[1];
  arguments << QApplication::applicationDirPath() + "/" + listUrl[0];
  myProcess->start(strAR, arguments);
}


static void onURLActivated(const std::string& url)
{
  QString strUrl(url.c_str());
  QUrl    objUrl(strUrl);
  if (!objUrl.isValid())
    FFaMsg::dialog("Invalid URL:\n" + url, FFaMsg::WARNING);
  else if (strUrl.left(7) == "file://")
    showPDFfile(strUrl);
  else if (strUrl.left(8) == "https://" || strUrl.left(7) == "http://")
    if (!QDesktopServices::openUrl(objUrl))
      FFaMsg::dialog("Unable to open URL:\n" + url, FFaMsg::WARNING);
}


bool FuiQtProperties::initStartGuide()
{
  // Load HTML file
  QString htmFile = "StartGuide.htm";
  QString appPath = QApplication::applicationDirPath() + "/Doc/";
  QFile file(appPath + htmFile);
  bool fileOk = file.open(QIODevice::ReadOnly | QIODevice::Text);
#if defined(win32) || defined(win64)
  if (!fileOk) {
    // Get path of the startguide folder from Windows registry.
    // The path must use forward slashes, and must end with a forward slash.
    QString regkey = QString("FMM-file\\internal\\") + FedemAdmin::getVersion();
    HKEY hk;
    LONG err = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, regkey.toStdWString().c_str(), 0, KEY_QUERY_VALUE, &hk);
    if (err == ERROR_SUCCESS) {
      // Get registry value
      char path[2048];
      DWORD cbData = 2048;
      memset(path,0,cbData);
      err = ::RegQueryValueEx(hk, L"startGuidePath", NULL, NULL, (LPBYTE)path, &cbData);
      ::RegCloseKey(hk);
      if (err == ERROR_SUCCESS) {
        appPath = path; // internal url
        file.setFileName(appPath + htmFile);
        fileOk = file.open(QIODevice::ReadOnly | QIODevice::Text);
      }
    }
  }
#endif

  QString strData;
  if (!fileOk)
    strData = "Welcome to <i>FEDEM 8.0</i>";
  else while (!file.atEnd())
    strData.append(file.readLine().data());
  file.close();
  // Change all relative local paths
  strData.replace(" src='", " src='" + appPath);
  // Set fields
  mySGLogoImage->setPixMap(startGuideLogo_xpm,true);
  mySGHeading->setLabel("<font color='#008cff' size='5'><i><b>Welcome to FEDEM 8.0</b></i></font>");
  mySGContentLabel->setLabel(strData.toStdString().c_str());
  mySGContentLabel->setLinkActivatedCB(FFaDynCB1S(onURLActivated,const std::string&));
  mySGLogoBorderRight->setPixMap(startGuideBorderRight_xpm,true);
  FFuaPalette pal;
  pal.setStdBackground(255,255,255);
  mySGFillWhite->setColors(pal);
  mySGContentLabel->setColors(pal);
  mySGBorderTop->setPixMap(startGuideBorderTop_xpm,true);
  pal.setStdBackground(130,135,144);
  mySGBorderRight->setColors(pal);
  mySGBorderBottom->setColors(pal);
  pal.setStdBackground(113,178,254);
  mySGLogoBorderTop->setColors(pal);
  return fileOk;
}


void FuiProperties::showPDF(const char* url)
{
  QString strUrl(url);
  showPDFfile(strUrl);
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
