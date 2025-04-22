// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAMainWindow.H"
#include "vpmApp/vpmAppCmds/FapFileCmds.H"
#include "vpmApp/vpmAppCmds/FapToolsCmds.H"
#include "vpmApp/FapLicenseManager.H"
#include "vpmUI/vpmUITopLevels/FuiMainWindow.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "vpmUI/Fui.H"
#include "vpmPM/FpPM.H"
#include "FiUserElmPlugin/FiUserElmPlugin.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFaLib/FFaCmdLineArg/FFaCmdLineArg.H"


Fmd_SOURCE_INIT(FAPUAMAINWINDOW, FapUAMainWindow, FapUAExistenceHandler);

//----------------------------------------------------------------------------

FapUAMainWindow::FapUAMainWindow(FuiMainWindow* uic)
 : FapUAExistenceHandler(uic), FapUACommandHandler(uic), FapUAFinishHandler(uic)
{
  Fmd_CONSTRUCTOR_INIT(FapUAMainWindow);

  this->ui = uic;

  this->fileHeader.setText("File");
  this->fileExportHeader.setText("Export");
  this->fileRecentHeader.setText("Recent models");
  this->editHeader.setText("Edit");
  this->viewHeader.setText("View");
  this->viewZoomHeader.setText("Zoom");
  this->viewZoomHeader.setSmallIcon(zoom_xpm);
  this->viewShiftHeader.setText("Pan");
  this->viewShiftHeader.setSmallIcon(pan_xpm);
  this->viewPredefHeader.setText("Predefined Views");
  this->toolsHeader.setText("Tools");
  this->windpowerHeader.setText("Windpower");
  this->marineHeader.setText("Marine");
  this->mechHeader.setText("Mechanism");
  this->mechCreateSingleJointHeader.setMenuButtonPopupMode(true);
  this->mechCreateMultiJointHeader.setMenuButtonPopupMode(true);
  this->mechCreateGearHeader.setMenuButtonPopupMode(true);
  this->mechCreateSpringHeader.setMenuButtonPopupMode(true);
  this->mechCreateTireHeader.setMenuButtonPopupMode(true);
  this->mechCreateForceHeader.setMenuButtonPopupMode(true);
  this->mechCreateSensorHeader.setMenuButtonPopupMode(true);
  this->threedSymbolSizeHeader.setMenuButtonPopupMode(false);
  this->ctrlHeader.setText("Control");
  this->solveMainHeader.setText("Solve");
  this->resultHeader.setText("Result");
  this->addonsHeader.setText("Addons");
  this->windowsHeader.setText("Windows");
  this->helpHeader.setText("Help");
}
//----------------------------------------------------------------------------

FFuaUICommands* FapUAMainWindow::getCommands()
{
  FuaMainWindowCommands* cmds = new FuaMainWindowCommands();

  //menubar
  cmds->menuBar.clear();

  cmds->menuBar.push_back(&this->fileHeader);
  cmds->menuBar.push_back(&this->editHeader);
  cmds->menuBar.push_back(&this->viewHeader);
  cmds->menuBar.push_back(&this->toolsHeader);
  if (FapLicenseManager::hasFeature("FA-WND"))
    cmds->menuBar.push_back(&this->windpowerHeader);
  if (FapLicenseManager::hasFeature("FA-WND") ||
      FapLicenseManager::hasFeature("FA-RIS"))
    cmds->menuBar.push_back(&this->marineHeader);
  cmds->menuBar.push_back(&this->mechHeader);
  if (FapLicenseManager::hasFeature("FA-CTR"))
    cmds->menuBar.push_back(&this->ctrlHeader);
  cmds->menuBar.push_back(&this->solveMainHeader);
  cmds->menuBar.push_back(&this->resultHeader);

  bool haveAddOns = FapToolsCmds::haveAddons();
  if (haveAddOns)
    cmds->menuBar.push_back(&this->addonsHeader);

  cmds->menuBar.push_back(&this->windowsHeader);
  cmds->menuBar.push_back(&this->helpHeader);

  //file
  this->fileHeader.clear();
  this->fileExportHeader.clear();
  this->fileRecentHeader.clear();

  this->fileHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_nu"));
  this->fileHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_open"));
  this->fileHeader.push_back(&this->fileRecentHeader);

  this->fileHeader.push_back(&this->separator);
  this->fileHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_save"));
  this->fileHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_saveAs"));

  this->fileHeader.push_back(&this->separator);
  this->fileHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_loadLink"));
  this->fileHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_setLinkRepository"));
  this->fileHeader.push_back(&this->separator);
  this->fileHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_importBeamCS"));
  this->fileHeader.push_back(&this->separator);
  this->fileHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_importAssembly"));

  this->fileHeader.push_back(&this->separator);
  this->fileHeader.push_back(&this->fileExportHeader);
  this->fileExportHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_export_exportObject"));
  this->fileExportHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_export_exportView"));
  this->fileExportHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_export_exportAnimation"));
  this->fileExportHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_export_exportAllCurves"));
  if (FapLicenseManager::hasFeature("FA-SAP")) {
    this->fileExportHeader.push_back(&this->separator);
    this->fileExportHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_export_exportCGeo"));
    this->fileExportHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_export_dtsDigitalTwin"));
  }

  //recent files
  FFuaCmdItem* pCmdItem; int i = 0;
  for (const std::string& model : FpPM::recentModels())
    if ((pCmdItem = FFuaCmdItem::getCmdItem("cmdId_file_recent" + std::to_string(i++))))
    {
      pCmdItem->setToolTip("Open the model " + model);
      pCmdItem->setText(std::to_string(i) + " " + model);
      this->fileRecentHeader.push_back(pCmdItem);
    }

  this->fileHeader.push_back(&this->separator);
  this->fileHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_exit"));

  //edit
  this->editHeader.clear();

  this->editHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_copyObject"));
  this->editHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_copySubassembly"));
  this->editHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_copyLink"));
  this->editHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_splitBeam"));
  this->editHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_orientCam"));

  this->editHeader.push_back(&this->separator);
  this->editHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_erase"));

  /* doesn't work properly yet
  this->editHeader.push_back(&this->separator);
  this->editHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_undo"));
  */

  this->editHeader.push_back(&this->separator);
  this->editHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_listView_ensureListViewSelectedVisible"));
  this->editHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_backward_select"));
  this->editHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_forward_select"));

  this->editHeader.push_back(&this->separator);
  this->editHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_modelPreferences"));

  //view
  this->viewHeader.clear();
  this->viewZoomHeader.clear();
  this->viewShiftHeader.clear();
  this->viewPredefHeader.clear();

  this->viewHeader.push_back(&this->viewZoomHeader);
  this->viewZoomHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_zoomAll"));
  this->viewZoomHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_zoomTo"));
  this->viewZoomHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_zoomWindow"));
  /* currently not available with Qwt 6.1.2
  this->viewZoomHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_zoomAllWindow"));
  */
  this->viewZoomHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_zoomIn"));
  this->viewZoomHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_zoomOut"));

  this->viewHeader.push_back(&this->separator);
  this->viewHeader.push_back(&this->viewShiftHeader);
  this->viewShiftHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_shiftLeft"));
  this->viewShiftHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_shiftRight"));
  this->viewShiftHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_shiftUp"));
  this->viewShiftHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_shiftDown"));

  this->viewHeader.push_back(&this->separator);
  this->viewHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_solidView"));
  this->viewHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_solidViewNoEdge"));
  this->viewHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_lineView"));
  this->viewHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_flatShadedView"));
  this->viewHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_topLightOnly"));

  this->viewHeader.push_back(&this->separator);
  this->viewHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_perspectiveView"));
  this->viewHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_parallellView"));

  this->viewHeader.push_back(&this->separator);
  this->viewHeader.push_back(&this->viewPredefHeader);
  this->viewPredefHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_isometricView"));
  this->viewPredefHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_XYpZpY"));
  this->viewPredefHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_XYnZpY"));
  this->viewPredefHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_XZnYpZ"));
  this->viewPredefHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_XZpYpZ"));
  this->viewPredefHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_YZpXpZ"));
  this->viewPredefHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_YZnXpZ"));

  this->viewHeader.push_back(&this->separator);
  this->viewHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mainWin_showModelManager"));
  this->viewHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mainWin_showProperties"));

  //tools
  this->toolsHeader.clear();

  this->toolsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_viewFilter"));
  this->toolsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_setObjAppearance"));

  this->toolsHeader.push_back(&this->separator);
  this->toolsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_objectBrowser"));

  this->toolsHeader.push_back(&this->separator);
  this->toolsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_distance"));
  this->toolsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_angle"));

  this->toolsHeader.push_back(&this->separator);
  if (FapLicenseManager::hasFeature("FA-CTR"))
    this->toolsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl_gridSnap"));
  this->toolsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_linkRamSettings"));
#if defined(win32) || defined(win64)
  this->toolsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_setFileAssociations"));
#endif
  this->toolsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_plugins"));

  //windpower
  this->windpowerHeader.clear();

  this->windpowerHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_airfoilDefinition"));
  this->windpowerHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_bladeDefinition"));
  this->windpowerHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_createTurbineAssembly"));
  this->windpowerHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_createTurbineTower"));

  this->windpowerHeader.push_back(&this->separator);
  this->windpowerHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_airEnvironment"));

  if (FapLicenseManager::hasFeature("FA-SEV")) {
    this->windpowerHeader.push_back(&this->separator);
    this->windpowerHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_createEvents"));
  }

  //addons
  this->addonsHeader.clear();
  if (haveAddOns) {
    std::string addon;
    for (i = 0; FapToolsCmds::getAddonExe(i,addon); i++)
      if ((pCmdItem = FFuaCmdItem::getCmdItem("cmdId_tools_addon" + std::to_string(i))))
      {
        pCmdItem->setToolTip(addon);
        pCmdItem->setText(addon + "...");
        this->addonsHeader.push_back(pCmdItem);
      }
  }

  //marine
  this->marineHeader.clear();

  this->marineHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_WaveFunction"));
  this->marineHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_CurrFunction"));
  this->marineHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_VesselMotion"));

  this->marineHeader.push_back(&this->separator);
  this->marineHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_seaEnvironment"));

  this->marineHeader.push_back(&this->separator);
  this->marineHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_createRiser"));
  this->marineHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_createJacket"));
  this->marineHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_createPile"));
  if (FapLicenseManager::hasFeature("FA-SEV"))
    this->marineHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_createEvents"));

  if (FapLicenseManager::hasFeature("FA-RIS")) {
    this->marineHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_mooring"));
    this->marineHeader.push_back(&this->separator);
    this->marineHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_createBeamstringPair"));
  }

  //mech
  this->mechHeader.clear();

  this->createSpiderHeader.clear();
  this->createSpiderHeader.setText("Surface Connector");
  this->createSpiderHeader.setSmallIcon(spider_xpm);
  this->createSpiderHeader.setToolTip("Create a surface connector to connect a Triad to an FE mesh");

  this->createRigidSpiderHeader.clear();
  this->createRigidSpiderHeader.setText("Rigid surface");
  this->createRigidSpiderHeader.setSmallIcon(spider_gen_xpm);

  this->createFlexibleSpiderHeader.clear();
  this->createFlexibleSpiderHeader.setText("Flexible surface");
  this->createFlexibleSpiderHeader.setSmallIcon(spider_geom_xpm);

  this->createJointHeader.clear();
  this->createJointHeader.setText("Joints");
  this->createJointHeader.setSmallIcon(revJoint_xpm);

  this->createFrictionHeader.clear();
  this->createFrictionHeader.setText("Friction");
  this->createFrictionHeader.setSmallIcon(friction_xpm);

  this->createSpringFunctionHeader.clear();
  this->createSpringFunctionHeader.setText("Spring characteristics");
  this->createSpringFunctionHeader.setSmallIcon(spring_xpm);

  this->createDamperFunctionHeader.clear();
  this->createDamperFunctionHeader.setText("Damper characteristics");
  this->createDamperFunctionHeader.setSmallIcon(damper_xpm);

  this->createWellHeader.clear();
  this->createWellHeader.setText("Oil Well");

  this->createUDEHeader.clear();
  this->createUDEHeader.setText("User-defined Elements");

  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_attach"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_detach"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_pointToPointMove"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_allignCS"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_allignRotations"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_moveToCenter"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createSticker"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_eraseStickers"));

  this->mechHeader.push_back(&this->separator);
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_loadLink"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_generic_part"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_beams"));

  this->mechHeader.push_back(&this->separator);
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createTriad"));

  this->mechHeader.push_back(&this->separator);
  this->mechHeader.push_back(&this->createSpiderHeader);
  this->createSpiderHeader.push_back(&this->createFlexibleSpiderHeader);
  this->createFlexibleSpiderHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_createRBE3GeomSpider"));
  this->createFlexibleSpiderHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_createRBE3GeneralSpider"));
  this->createSpiderHeader.push_back(&this->createRigidSpiderHeader);
  this->createRigidSpiderHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_createRBE2GeomSpider"));
  this->createRigidSpiderHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_createRBE2GeneralSpider"));

  this->mechHeader.push_back(&this->separator);
  this->mechHeader.push_back(&this->createJointHeader);
  this->createJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createRevJoint"));
  this->createJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createBallJoint"));
  this->createJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createRigidJoint"));
  this->createJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createFreeJoint"));
  this->createJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createFreeJoint2"));

  this->createJointHeader.push_back(&this->separator);
  this->createJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createPrismJoint"));
  this->createJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createPrismJoint2"));
  this->createJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createCylJoint"));
  this->createJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createCylJoint2"));
  this->createJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createCamJoint"));

  this->mechHeader.push_back(&this->separator);
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createGear"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createRackPinon"));

  this->mechHeader.push_back(&this->separator);
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createSpring"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createDamper"));

  this->mechHeader.push_back(&this->createSpringFunctionHeader);
  this->createSpringFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_SprTransStiff"));
  this->createSpringFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_SprTransForce"));
  this->createSpringFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_SprRotStiff"));
  this->createSpringFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_SprRotTorque"));
  this->createSpringFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_SpringCharTrans"));
  this->createSpringFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_SpringCharRot"));

  this->createDamperFunctionHeader.clear();
  this->mechHeader.push_back(&this->createDamperFunctionHeader);
  this->createDamperFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_DaTransCoeff"));
  this->createDamperFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_DaTransForce"));
  this->createDamperFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_DaRotCoeff"));
  this->createDamperFunctionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_DaRotTorque"));

  this->mechHeader.push_back(&this->separator);
  this->mechHeader.push_back(&this->createFrictionHeader);
  this->createFrictionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_RotFriction"));
  this->createFrictionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_TransFriction"));
  this->createFrictionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_BearingFriction"));
  this->createFrictionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_PrismFriction"));
  this->createFrictionHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_CamFriction"));

  if (FapLicenseManager::hasFeature("FA-TII")) {
    this->mechHeader.push_back(&this->separator);
    this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createTire"));
    this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createRoad"));
    this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_RoadFunction"));
  }

  this->mechHeader.push_back(&this->separator);
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createForce"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createTorque"));

  this->mechHeader.push_back(&this->separator);
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createSimpleSensor"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createRelativeSensor"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_createStrainRosette"));

  this->mechHeader.push_back(&this->separator);
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_GeneralFunction"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_function_DriveFile"));

#ifdef FT_HAS_EXTCTRL
  this->mechHeader.push_back(&this->separator);
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_external_ctrl_sys"));
#endif

  this->mechHeader.push_back(&this->separator);
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_file_reference"));

  this->mechHeader.push_back(&this->separator);
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_GenericDBObject"));

  this->mechHeader.push_back(&this->separator);
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_MaterialProperty"));
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_BeamProperty"));

  int eTypes[10];
  int nTypes = FiUserElmPlugin::instance()->getElementTypes(10,eTypes);
  if (nTypes > 0) {
    this->mechHeader.push_back(&this->separator);
    this->mechHeader.push_back(&this->createUDEHeader);
    char typeName[64];
    for (i = 0; i < nTypes; i++)
      if ((pCmdItem = FFuaCmdItem::getCmdItem("cmdId_dBCreate_UE" + std::to_string(i))))
        if (FiUserElmPlugin::instance()->getTypeName(eTypes[i],64,typeName) > 0)
        {
          pCmdItem->setText(typeName);
          pCmdItem->setToolTip(typeName);
          this->createUDEHeader.push_back(pCmdItem);
        }
  }

  if (FapLicenseManager::hasFeature("FA-OWL")) {
    this->mechHeader.push_back(&this->separator);
    this->mechHeader.push_back(&this->createWellHeader);
    this->createWellHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_createPipeSurface"));
    this->createWellHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_createPipeString"));
    this->createWellHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_createDrillString"));
  }

  this->mechHeader.push_back(&this->separator);
  this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_Assembly"));

  if (FapLicenseManager::hasFeature("FA-SEV")) {
    this->mechHeader.push_back(&this->separator);
    this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_file_createEvents"));
    this->mechHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_eventDefinition"));
  }

  //ctrl
  this->ctrlHeader.clear();

  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_rotate"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_addPoint"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_removePoint"));

  this->ctrlHeader.push_back(&this->separator);
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createInput"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createOutput"));

  this->ctrlHeader.push_back(&this->separator);
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createAmplifier"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPower"));

  this->ctrlHeader.push_back(&this->separator);
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createComparator"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createAdder"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createMultiplyer"));

  this->ctrlHeader.push_back(&this->separator);
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createIntegrator"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createLimDerivator"));

  this->ctrlHeader.push_back(&this->separator);
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createTimeDelay"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createSampleHold"));

  this->ctrlHeader.push_back(&this->separator);
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createLogicalSwitch"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createLimitation"));

  this->ctrlHeader.push_back(&this->separator);
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createDeadZone"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createHysteresis"));

  this->ctrlHeader.push_back(&this->separator);
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPid"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPi"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPd"));

  this->ctrlHeader.push_back(&this->separator);
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPlimIlimD"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPIlimD"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPlimI"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPlimD"));

  this->ctrlHeader.push_back(&this->separator);
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createRealPole"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createCompConjPole"));

  this->ctrlHeader.push_back(&this->separator);
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_create1ordTF"));
  this->ctrlHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_create2ordTF"));

  //solve
  this->solveMainHeader.clear();

#ifdef FT_HAS_SOLVERS
  this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_reduceAllLinks"));
#endif
  this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveDynamicsBasic"));
  this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveDynamicsAdvanced"));
#ifdef FT_HAS_SOLVERS
  this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveStress"));
  this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveModes"));
  this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveRosette"));
  if (FapLicenseManager::hasFeature("FA-DRB")) {
    this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveStrainCoat"));
#ifdef FT_HAS_NCODE
    this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveDutyCycle"));
#endif
  }
  this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveAll"));
  if (FapLicenseManager::hasFeature("FA-SEV"))
    this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveEvents"));
  this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_prepareBatch"));

  this->solveMainHeader.push_back(&this->separator);
  this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_manageSolveStress"));
  this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_manageSolveModes"));
  this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_manageSolveRosette"));
  if (FapLicenseManager::hasFeature("FA-DRB")) {
    this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_manageSolveStrainCoat"));
#ifdef FT_HAS_NCODE
    this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_manageSolveDutyCycle"));
#endif
  }
  this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_preferences"));

  this->solveMainHeader.push_back(&this->separator);
  this->solveMainHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_stopSolver"));
#endif

  //result
  this->resultHeader.clear();

  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_graph_showRDBSelector"));
  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_graph"));
  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_curve"));
  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_animation"));
  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_graphGroup"));

  this->resultHeader.push_back(&this->separator);
  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_graph_show"));
  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_animation_show"));
  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_animation_hide"));
  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_animation_showControls"));

  this->resultHeader.push_back(&this->separator);
  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_miniFileBrowser"));
  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_refreshRDB"));
  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_eraseSimuleRes"));
  if (FapLicenseManager::hasFeature("FA-SEV"))
    this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_eraseEventRes"));

#ifdef FT_HAS_SOLVERS
  this->resultHeader.push_back(&this->separator);
  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_deleteStressResults"));
  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_deleteModesResults"));
  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_deleteRosetteResults"));
  if (FapLicenseManager::hasFeature("FA-DRB")) {
    this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_deleteStrainCoatResults"));
#ifdef FT_HAS_NCODE
    this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_deleteDutyCycleResults"));
#endif
  }
#endif
  this->resultHeader.push_back(&this->separator);
  this->resultHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_addResultsProbe"));

  //windows
  this->windowsHeader.clear();

  this->windowsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_workSpace_cascade"));
  this->windowsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_workSpace_tile"));

  this->windowsHeader.push_back(&this->separator);
  this->windowsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_workSpace_tabs"));
  this->windowsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_workSpace_subWins"));

  this->windowsHeader.push_back(&this->separator);
  this->windowsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech_show"));
  if (FapLicenseManager::hasFeature("FA-CTR"))
    this->windowsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl_show"));
  this->windowsHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_tools_outputList"));

  //help
  this->helpHeader.clear();

  this->helpHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_help_startGuide"));

  this->helpHeader.push_back(&this->separator);
  if (FapLicenseManager::hasUserGuide())
    this->helpHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_help_usersGuide"));
  if (FapLicenseManager::hasTheoryGuide())
    this->helpHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_help_theoryGuide"));
  if (FapLicenseManager::hasReferenceGuide())
    this->helpHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_contextHelp_select"));
  if (FapLicenseManager::hasComAPIGuide())
    this->helpHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_help_comApi"));
  this->helpHeader.push_back(&this->separator);
  this->helpHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_help_about"));


  //Toolbar order in FuiQtMainWindow constr

  //STD TOOLBAR
  cmds->toolBars[FuiMainWindow::STD].clear();

  cmds->toolBars[FuiMainWindow::STD].push_back(FFuaCmdItem::getCmdItem("cmdId_file_nu"));
  cmds->toolBars[FuiMainWindow::STD].push_back(FFuaCmdItem::getCmdItem("cmdId_file_open"));
  cmds->toolBars[FuiMainWindow::STD].push_back(FFuaCmdItem::getCmdItem("cmdId_file_save"));
  /* doesn't work property yet
  cmds->toolBars[FuiMainWindow::STD].push_back(&this->separator);
  cmds->toolBars[FuiMainWindow::STD].push_back(FFuaCmdItem::getCmdItem("cmdId_edit_undo"));
  */

  cmds->toolBars[FuiMainWindow::STD].push_back(&this->separator);
  cmds->toolBars[FuiMainWindow::STD].push_back(FFuaCmdItem::getCmdItem("cmdId_listView_ensureListViewSelectedVisible"));
  cmds->toolBars[FuiMainWindow::STD].push_back(FFuaCmdItem::getCmdItem("cmdId_backward_select"));
  cmds->toolBars[FuiMainWindow::STD].push_back(FFuaCmdItem::getCmdItem("cmdId_forward_select"));
  cmds->toolBars[FuiMainWindow::STD].push_back(FFuaCmdItem::getCmdItem("cmdId_edit_erase"));

  cmds->toolBars[FuiMainWindow::STD].push_back(&this->separator);
  cmds->toolBars[FuiMainWindow::STD].push_back(FFuaCmdItem::getCmdItem("cmdId_tools_viewFilter"));
  cmds->toolBars[FuiMainWindow::STD].push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_setObjAppearance"));
  cmds->toolBars[FuiMainWindow::STD].push_back(FFuaCmdItem::getCmdItem("cmdId_animation_showControls"));

  //WINDOWS TOOLBAR
  cmds->toolBars[FuiMainWindow::WINDOWS].clear();

  cmds->toolBars[FuiMainWindow::WINDOWS].push_back(FFuaCmdItem::getCmdItem("cmdId_mech_show"));
  if (FapLicenseManager::hasFeature("FA-CTR"))
    cmds->toolBars[FuiMainWindow::WINDOWS].push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl_show"));
  cmds->toolBars[FuiMainWindow::WINDOWS].push_back(FFuaCmdItem::getCmdItem("cmdId_tools_outputList"));
  cmds->toolBars[FuiMainWindow::WINDOWS].push_back(FFuaCmdItem::getCmdItem("cmdId_tools_miniFileBrowser"));
  cmds->toolBars[FuiMainWindow::WINDOWS].push_back(FFuaCmdItem::getCmdItem("cmdId_mainWin_showModelManager"));
  cmds->toolBars[FuiMainWindow::WINDOWS].push_back(FFuaCmdItem::getCmdItem("cmdId_mainWin_showProperties"));

  //SOLVE TOOLBAR
  cmds->toolBars[FuiMainWindow::SOLVE].clear();
  this->stressHeader.clear();
  this->modesHeader.clear();
  this->rosetteHeader.clear();
  this->strainCoatHeader.clear();
#ifdef FT_HAS_NCODE
  this->dutyCycleHeader.clear();
#endif

#ifdef FT_HAS_SOLVERS
  cmds->toolBars[FuiMainWindow::SOLVE].push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveDynamicsBasic"));

  cmds->toolBars[FuiMainWindow::SOLVE].push_back(&this->stressHeader);
  this->stressHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveStress"));
  this->stressHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_manageSolveStress"));

  cmds->toolBars[FuiMainWindow::SOLVE].push_back(&this->modesHeader);
  this->modesHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveModes"));
  this->modesHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_manageSolveModes"));

  cmds->toolBars[FuiMainWindow::SOLVE].push_back(&this->rosetteHeader);
  this->rosetteHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveRosette"));
  this->rosetteHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_manageSolveRosette"));

  if (FapLicenseManager::hasFeature("FA-DRB")) {
    cmds->toolBars[FuiMainWindow::SOLVE].push_back(&this->strainCoatHeader);
    this->strainCoatHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveStrainCoat"));
    this->strainCoatHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_manageSolveStrainCoat"));
#ifdef FT_HAS_NCODE
    cmds->toolBars[FuiMainWindow::SOLVE].push_back(&this->dutyCycleHeader);
    this->dutyCycleHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveDutyCycle"));
    this->dutyCycleHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_solve_manageSolveDutyCycle"));
#endif
  }

  cmds->toolBars[FuiMainWindow::SOLVE].push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveAll"));
  if (FapLicenseManager::hasFeature("FA-SEV"))
    cmds->toolBars[FuiMainWindow::SOLVE].push_back(FFuaCmdItem::getCmdItem("cmdId_solve_solveEvents"));

  cmds->toolBars[FuiMainWindow::SOLVE].push_back(&this->separator);
  cmds->toolBars[FuiMainWindow::SOLVE].push_back(FFuaCmdItem::getCmdItem("cmdId_solve_stopSolver"));
  cmds->toolBars[FuiMainWindow::SOLVE].push_back(FFuaCmdItem::getCmdItem("cmdId_solve_eraseSimuleRes"));
  if (FapLicenseManager::hasFeature("FA-SEV"))
    cmds->toolBars[FuiMainWindow::SOLVE].push_back(FFuaCmdItem::getCmdItem("cmdId_solve_eraseEventRes"));
  cmds->toolBars[FuiMainWindow::SOLVE].push_back(FFuaCmdItem::getCmdItem("cmdId_solve_refreshRDB"));
#else
  Fui::getMainWindow()->removeToolBar(FuiMainWindow::SOLVE);
#endif

  //VIEWCTRL TOOLBAR
  cmds->toolBars[FuiMainWindow::VIEWCTRL].clear();

  cmds->toolBars[FuiMainWindow::VIEWCTRL].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_zoomAll"));
  cmds->toolBars[FuiMainWindow::VIEWCTRL].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_zoomTo"));
  cmds->toolBars[FuiMainWindow::VIEWCTRL].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_zoomWindow"));
  /* currently not available with Qwt 6.1.2
  cmds->toolBars[FuiMainWindow::VIEWCTRL].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_zoomAllWindow"));
  */
  cmds->toolBars[FuiMainWindow::VIEWCTRL].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_zoomIn"));
  cmds->toolBars[FuiMainWindow::VIEWCTRL].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_zoomOut"));

  cmds->toolBars[FuiMainWindow::VIEWCTRL].push_back(&this->separator);
  cmds->toolBars[FuiMainWindow::VIEWCTRL].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_shiftLeft"));
  cmds->toolBars[FuiMainWindow::VIEWCTRL].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_shiftRight"));
  cmds->toolBars[FuiMainWindow::VIEWCTRL].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_shiftUp"));
  cmds->toolBars[FuiMainWindow::VIEWCTRL].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_shiftDown"));

  // Make two extra toolbars, for when only the modeler view or graph views, respectively, are open
  cmds->toolBars[FuiMainWindow::VIEWCTRL1] = { cmds->toolBars[FuiMainWindow::VIEWCTRL][0],
                                               cmds->toolBars[FuiMainWindow::VIEWCTRL][1] };
  cmds->toolBars[FuiMainWindow::VIEWCTRL2] = cmds->toolBars[FuiMainWindow::VIEWCTRL];
  cmds->toolBars[FuiMainWindow::VIEWCTRL2].erase(cmds->toolBars[FuiMainWindow::VIEWCTRL2].begin()+1);

  //3DVIEWS TOOLBAR
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].clear();

  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_solidView"));
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_solidViewNoEdge"));
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_lineView"));
  /* removed to save toolbar space (not-so-important features)
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_flatShadedView"));
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_topLightOnly"));
  */
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_perspectiveView"));
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_parallellView"));

  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(&this->separator);
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_LinkSelection_show"));
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_LinkSelection_hide"));

  this->threedSymbolSizeHeader.clear();
  this->threedSymbolSizeHeader.setText("Symbol Size");
  this->threedSymbolSizeHeader.setSmallIcon(symbolSize_xpm);
  this->threedSymbolSizeHeader.setToolTip("Set symbol Size");
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(&this->separator);
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(&this->threedSymbolSizeHeader);
  this->threedSymbolSizeHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_symbolSize0001"));
  this->threedSymbolSizeHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_symbolSize001"));
  this->threedSymbolSizeHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_symbolSize01"));
  this->threedSymbolSizeHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_symbolSize1"));
  this->threedSymbolSizeHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_symbolSize10"));

  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(&this->separator);
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_isometricView"));
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_XYpZpY"));
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_XYnZpY"));
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_XZnYpZ"));
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_XZpYpZ"));
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_YZpXpZ"));
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_YZnXpZ"));

  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(&this->separator);
  cmds->toolBars[FuiMainWindow::THREEDVIEWS].push_back(FFuaCmdItem::getCmdItem("cmdId_addResultsProbe"));

  //MECH WIND TOOLBAR
  cmds->toolBars[FuiMainWindow::MECHWIND].clear();
  if (FapLicenseManager::hasFeature("FA-WND")) {
    cmds->toolBars[FuiMainWindow::MECHWIND].push_back(FFuaCmdItem::getCmdItem("cmdId_tools_airfoilDefinition"));
    cmds->toolBars[FuiMainWindow::MECHWIND].push_back(FFuaCmdItem::getCmdItem("cmdId_tools_bladeDefinition"));
    cmds->toolBars[FuiMainWindow::MECHWIND].push_back(FFuaCmdItem::getCmdItem("cmdId_tools_createTurbineAssembly"));
    cmds->toolBars[FuiMainWindow::MECHWIND].push_back(FFuaCmdItem::getCmdItem("cmdId_tools_createTurbineTower"));

    cmds->toolBars[FuiMainWindow::MECHWIND].push_back(&this->separator);
    cmds->toolBars[FuiMainWindow::MECHWIND].push_back(FFuaCmdItem::getCmdItem("cmdId_tools_airEnvironment"));
    cmds->toolBars[FuiMainWindow::MECHWIND].push_back(FFuaCmdItem::getCmdItem("cmdId_tools_seaEnvironment"));
  }
  else
    Fui::getMainWindow()->removeToolBar(FuiMainWindow::MECHWIND);

  //MECH CREATE TOOLBAR
  cmds->toolBars[FuiMainWindow::MECHCREATE].clear();

  this->mechCreateSingleJointHeader.clear();
  this->mechCreateMultiJointHeader.clear();
  this->mechCreateGearHeader.clear();
  this->mechCreateSpringHeader.clear();
  this->mechCreateTireHeader.clear();
  this->mechCreateForceHeader.clear();
  this->mechCreateSensorHeader.clear();
  this->mechStickerHeader.clear();
  this->mechMoveHeader.clear();

  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(FFuaCmdItem::getCmdItem("cmdId_file_loadLink"));
  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_generic_part"));
  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_beams"));

  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(&this->separator);
  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_BeamProperty"));
  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(FFuaCmdItem::getCmdItem("cmdId_dBCreate_MaterialProperty"));

  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(&this->separator);
  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(&this->mechCreateSingleJointHeader);
  this->mechCreateSingleJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createRevJoint"));
  this->mechCreateSingleJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createBallJoint"));
  this->mechCreateSingleJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createRigidJoint"));
  this->mechCreateSingleJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createFreeJoint"));
  this->mechCreateSingleJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createFreeJoint2"));

  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(&this->mechCreateMultiJointHeader);
  this->mechCreateMultiJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createPrismJoint"));
  this->mechCreateMultiJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createPrismJoint2"));
  this->mechCreateMultiJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createCylJoint"));
  this->mechCreateMultiJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createCylJoint2"));
  this->mechCreateMultiJointHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createCamJoint"));

  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(&this->mechCreateGearHeader);
  this->mechCreateGearHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createGear"));
  this->mechCreateGearHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createRackPinon"));

  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(&this->mechCreateSpringHeader);
  this->mechCreateSpringHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createSpring"));
  this->mechCreateSpringHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createDamper"));

  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createTriad"));

  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(&this->mechCreateForceHeader);
  this->mechCreateForceHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createForce"));
  this->mechCreateForceHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createTorque"));

  if (FapLicenseManager::hasFeature("FA-TII")) {
    cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(&this->mechCreateTireHeader);
    this->mechCreateTireHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createTire"));
    this->mechCreateTireHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createRoad"));
  }

  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(&this->mechCreateSensorHeader);
  this->mechCreateSensorHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createSimpleSensor"));
  this->mechCreateSensorHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createRelativeSensor"));

  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(FFuaCmdItem::getCmdItem("cmdId_createStrainRosette"));

  cmds->toolBars[FuiMainWindow::MECHCREATE].push_back(&this->createSpiderHeader);

  //MECH MODELLING TOOLBAR
  cmds->toolBars[FuiMainWindow::MECHMODELLINGTOOLS].clear();

  cmds->toolBars[FuiMainWindow::MECHMODELLINGTOOLS].push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_attach"));
  cmds->toolBars[FuiMainWindow::MECHMODELLINGTOOLS].push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_detach"));
  cmds->toolBars[FuiMainWindow::MECHMODELLINGTOOLS].push_back(&this->mechMoveHeader);
  this->mechMoveHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_pointToPointMove"));
  this->mechMoveHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_allignCS"));
  this->mechMoveHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_allignRotations"));
  this->mechMoveHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_moveToCenter"));

  /* removed to save toolbar space (stickers not that important)
  cmds->toolBars[FuiMainWindow::MECHMODELLINGTOOLS].push_back(&this->mechStickerHeader);
  this->mechStickerHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_eraseStickers"));
  this->mechStickerHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_mech3DObj_createSticker"));
  */

  //CTRL CREATE TOOLBAR
  cmds->toolBars[FuiMainWindow::CTRLCREATE].clear();
  this->ctrlCreateInputHeader.clear();
  this->ctrlCreateAmplifierHeader.clear();
  this->ctrlCreateComparatorHeader.clear();
  this->ctrlCreateIntegratorHeader.clear();
  this->ctrlCreateTimeDelayHeader.clear();
  this->ctrlCreateLogicalSwitchHeader.clear();
  this->ctrlCreateDeadZoneHeader.clear();
  this->ctrlCreatePidHeader.clear();
  this->ctrlCreatePlimIlimDHeader.clear();
  this->ctrlCreateRealPoleHeader.clear();
  this->ctrlCreate1ordTFHeader.clear();

  if (FapLicenseManager::hasFeature("FA-CTR")) {
    cmds->toolBars[FuiMainWindow::CTRLCREATE].push_back(&this->ctrlCreateInputHeader);
    this->ctrlCreateInputHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createInput"));
    this->ctrlCreateInputHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createOutput"));

    cmds->toolBars[FuiMainWindow::CTRLCREATE].push_back(&this->ctrlCreateAmplifierHeader);
    this->ctrlCreateAmplifierHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createAmplifier"));
    this->ctrlCreateAmplifierHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPower"));

    cmds->toolBars[FuiMainWindow::CTRLCREATE].push_back(&this->ctrlCreateComparatorHeader);
    this->ctrlCreateComparatorHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createComparator"));
    this->ctrlCreateComparatorHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createAdder"));
    this->ctrlCreateComparatorHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createMultiplyer"));

    cmds->toolBars[FuiMainWindow::CTRLCREATE].push_back(&this->ctrlCreateIntegratorHeader);
    this->ctrlCreateIntegratorHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createIntegrator"));
    this->ctrlCreateIntegratorHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createLimDerivator"));

    cmds->toolBars[FuiMainWindow::CTRLCREATE].push_back(&this->ctrlCreateTimeDelayHeader);
    this->ctrlCreateTimeDelayHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createTimeDelay"));
    this->ctrlCreateTimeDelayHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createSampleHold"));

    cmds->toolBars[FuiMainWindow::CTRLCREATE].push_back(&this->ctrlCreateLogicalSwitchHeader);
    this->ctrlCreateLogicalSwitchHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createLogicalSwitch"));
    this->ctrlCreateLogicalSwitchHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createLimitation"));

    cmds->toolBars[FuiMainWindow::CTRLCREATE].push_back(&this->ctrlCreateDeadZoneHeader);
    this->ctrlCreateDeadZoneHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createDeadZone"));
    this->ctrlCreateDeadZoneHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createHysteresis"));

    cmds->toolBars[FuiMainWindow::CTRLCREATE].push_back(&this->ctrlCreatePidHeader);
    this->ctrlCreatePidHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPid"));
    this->ctrlCreatePidHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPi"));
    this->ctrlCreatePidHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPd"));

    cmds->toolBars[FuiMainWindow::CTRLCREATE].push_back(&this->ctrlCreatePlimIlimDHeader);
    this->ctrlCreatePlimIlimDHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPlimIlimD"));
    this->ctrlCreatePlimIlimDHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPIlimD"));
    this->ctrlCreatePlimIlimDHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPlimI"));
    this->ctrlCreatePlimIlimDHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createPlimD"));

    cmds->toolBars[FuiMainWindow::CTRLCREATE].push_back(&this->ctrlCreateRealPoleHeader);
    this->ctrlCreateRealPoleHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createRealPole"));
    this->ctrlCreateRealPoleHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_createCompConjPole"));

    cmds->toolBars[FuiMainWindow::CTRLCREATE].push_back(&this->ctrlCreate1ordTFHeader);
    this->ctrlCreate1ordTFHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_create1ordTF"));
    this->ctrlCreate1ordTFHeader.push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_create2ordTF"));
  }
  else
    Fui::getMainWindow()->removeToolBar(FuiMainWindow::CTRLCREATE);

  //CTRL MODELLING TOOLBAR
  cmds->toolBars[FuiMainWindow::CTRLMODELLINGTOOLS].clear();
  if (FapLicenseManager::hasFeature("FA-CTR")) {
    cmds->toolBars[FuiMainWindow::CTRLMODELLINGTOOLS].push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_rotate"));
    cmds->toolBars[FuiMainWindow::CTRLMODELLINGTOOLS].push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_addPoint"));
    cmds->toolBars[FuiMainWindow::CTRLMODELLINGTOOLS].push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl3DObj_removePoint"));
    cmds->toolBars[FuiMainWindow::CTRLMODELLINGTOOLS].push_back(FFuaCmdItem::getCmdItem("cmdId_ctrl_gridSnap"));
  }
  else
    Fui::getMainWindow()->removeToolBar(FuiMainWindow::CTRLMODELLINGTOOLS);

  return cmds;
}
//----------------------------------------------------------------------------

void FapUAMainWindow::finishUI()
{
  FapFileCmds::exit();
}
