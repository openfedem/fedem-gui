// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapViewCtrlCmds.H"
#include "vpmApp/vpmAppUAMap/FapUAViewSettings.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUACommandHandler.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmUI/vpmUITopLevels/FuiGraphView.H"
#endif
#include "vpmUI/Icons/FuiIconPixmaps.H"
#ifdef USE_INVENTOR
#include "vpmDisplay/FdDB.H"
#endif
#include "vpmDB/FmDB.H"
#include "vpmDB/FmGlobalViewSettings.H"
#include "vpmDB/FmIsRenderedBase.H"

//----------------------------------------------------------------------------

void FapViewCtrlCmds::init()
{
  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_zoomAll");
  cmdItem->setSmallIcon(zoomAll_xpm);
  cmdItem->setText("Zoom All");
  cmdItem->setToolTip("Zoom All");
  cmdItem->setActivatedCB(FFaDynCB0S(FapViewCtrlCmds::zoomAll));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerOrGraphViewActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_zoomTo");
  cmdItem->setSmallIcon(zoomTo_xpm);
  cmdItem->setText("Zoom To");
  cmdItem->setToolTip("Zoom to selected object");
  cmdItem->setActivatedCB(FFaDynCB0S(FapViewCtrlCmds::zoomTo));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapViewCtrlCmds::getZoomToSensitivity,bool&));

  // Mechanism

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_solidView");
  cmdItem->setSmallIcon(solidView_xpm);
  cmdItem->setText("Solid View");
  cmdItem->setToolTip("Solid View");
  cmdItem->setToggleBehaveAsRadio(true);
  cmdItem->setToggledCB(FFaDynCB1S(FapViewCtrlCmds::solidView,bool));
  cmdItem->setGetToggledCB(FFaDynCB1S(FapViewCtrlCmds::getSolidViewToggle,bool&));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_solidViewNoEdge");
  cmdItem->setSmallIcon(solidViewNoEdge_xpm);
  cmdItem->setText("Solid View without Edges");
  cmdItem->setToolTip("Solid View without Edges");
  cmdItem->setToggleBehaveAsRadio(true);
  cmdItem->setToggledCB(FFaDynCB1S(FapViewCtrlCmds::solidViewNoEdge,bool));
  cmdItem->setGetToggledCB(FFaDynCB1S(FapViewCtrlCmds::getSolidViewNoEdgeToggle,bool&));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_lineView");
  cmdItem->setSmallIcon(lineView_xpm);
  cmdItem->setText("Line View");
  cmdItem->setToolTip("Line View");
  cmdItem->setToggleBehaveAsRadio(true);
  cmdItem->setToggledCB(FFaDynCB1S(FapViewCtrlCmds::lineView,bool));
  cmdItem->setGetToggledCB(FFaDynCB1S(FapViewCtrlCmds::getLineViewToggle,bool&));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_flatShadedView");
  cmdItem->setSmallIcon(flatColor_xpm);
  cmdItem->setText("Flat Colors");
  cmdItem->setToolTip("Turn off shading");
  cmdItem->setToggleAble(true);
  cmdItem->setToggledCB(FFaDynCB1S(FapViewCtrlCmds::flatColorView,bool));
  cmdItem->setGetToggledCB(FFaDynCB1S(FapViewCtrlCmds::getFlatColorToggle,bool&));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_topLightOnly");
  cmdItem->setSmallIcon(showTopFaces_xpm);
  cmdItem->setText("Show Top Faces");
  cmdItem->setToolTip("Show the top faces");
  cmdItem->setToggleAble(true);
  cmdItem->setToggledCB(FFaDynCB1S(FapViewCtrlCmds::topLightOnly,bool));
  cmdItem->setGetToggledCB(FFaDynCB1S(FapViewCtrlCmds::getTopLightOnlyToggle,bool&));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_perspectiveView");
  cmdItem->setSmallIcon(perspectiveView_xpm);
  cmdItem->setText("Perspective");
  cmdItem->setToolTip("Perspective");
  cmdItem->setToggleBehaveAsRadio(true);
  cmdItem->setToggledCB(FFaDynCB1S(FapViewCtrlCmds::perspectiveView,bool));
  cmdItem->setGetToggledCB(FFaDynCB1S(FapViewCtrlCmds::getPerspectiveViewToggle,bool&));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_parallellView");
  cmdItem->setSmallIcon(paralellView_xpm);
  cmdItem->setText("Parallel Projection");
  cmdItem->setToolTip("Parallel Projection");
  cmdItem->setToggleBehaveAsRadio(true);
  cmdItem->setToggledCB(FFaDynCB1S(FapViewCtrlCmds::parallellView,bool));
  cmdItem->setGetToggledCB(FFaDynCB1S(FapViewCtrlCmds::getParallellViewToggle,bool&));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_symbolSize0001");
  cmdItem->setText("0.001");
  cmdItem->setToolTip("Symbol Size");
  cmdItem->setActivatedCB(FFaDynCB0S([](){FapViewCtrlCmds::symbolSize(0.001);}));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_symbolSize001");
  cmdItem->setText("0.01");
  cmdItem->setToolTip("Symbol Size");
  cmdItem->setActivatedCB(FFaDynCB0S([](){FapViewCtrlCmds::symbolSize(0.01);}));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_symbolSize01");
  cmdItem->setText("0.1 (default)");
  cmdItem->setToolTip("Symbol Size");
  cmdItem->setActivatedCB(FFaDynCB0S([](){FapViewCtrlCmds::symbolSize(0.1);}));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_symbolSize1");
  cmdItem->setText("1");
  cmdItem->setToolTip("Symbol Size");
  cmdItem->setActivatedCB(FFaDynCB0S([](){FapViewCtrlCmds::symbolSize(1.0);}));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_symbolSize10");
  cmdItem->setText("10");
  cmdItem->setToolTip("Symbol Size");
  cmdItem->setActivatedCB(FFaDynCB0S([](){FapViewCtrlCmds::symbolSize(10.0);}));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  // Views

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_isometricView");
  cmdItem->setSmallIcon(isometricView_xpm);
  cmdItem->setText("Isometric");
  cmdItem->setToolTip("Isometric");
#ifdef USE_INVENTOR
  cmdItem->setActivatedCB(FFaDynCB0S(FdDB::isometricView));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_XYpZpY");
  cmdItem->setSmallIcon(View_XYpZpY_xpm);
  cmdItem->setText("Top");
  cmdItem->setToolTip("Top");
#ifdef USE_INVENTOR
  cmdItem->setActivatedCB(FFaDynCB0S(FdDB::XYpZpYView));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_XYnZpY");
  cmdItem->setSmallIcon(View_XYnZpY_xpm);
  cmdItem->setText("Bottom");
  cmdItem->setToolTip("Bottom");
#ifdef USE_INVENTOR
  cmdItem->setActivatedCB(FFaDynCB0S(FdDB::XYnZpYView));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_YZpXpZ");
  cmdItem->setSmallIcon(View_YZpXpZ_xpm);
  cmdItem->setText("Right");
  cmdItem->setToolTip("Right View");
#ifdef USE_INVENTOR
  cmdItem->setActivatedCB(FFaDynCB0S(FdDB::YZpXpZView));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_YZnXpZ");
  cmdItem->setSmallIcon(View_YZnXpZ_xpm);
  cmdItem->setText("Left");
  cmdItem->setToolTip("Left View");
#ifdef USE_INVENTOR
  cmdItem->setActivatedCB(FFaDynCB0S(FdDB::YZnXpZView));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_XZnYpZ");
  cmdItem->setSmallIcon(View_XZnYpZ_xpm);
  cmdItem->setText("Front");
  cmdItem->setToolTip("Front");
#ifdef USE_INVENTOR
  cmdItem->setActivatedCB(FFaDynCB0S(FdDB::XZnYpZView));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_XZpYpZ");
  cmdItem->setSmallIcon(View_XZpYpZ_xpm);
  cmdItem->setText("Back");
  cmdItem->setToolTip("Back View");
#ifdef USE_INVENTOR
  cmdItem->setActivatedCB(FFaDynCB0S(FdDB::XZpYpZView));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerActive,bool&));

  // Graph

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_zoomWindow");
  cmdItem->setSmallIcon(curvePlotZoomWindow_xpm);
  cmdItem->setText("Zoom Window");
  cmdItem->setToolTip("Zoom Window");
#ifdef FT_HAS_GRAPHVIEW
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapCmdsBase::getActiveGraphView()->zoomWindow(); }));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isGraphViewActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_zoomAllWindow");
  cmdItem->setSmallIcon(curvePlotZoomWindowAS_xpm);
  cmdItem->setText("Zoom Window With Autoscale");
  cmdItem->setToolTip("Zoom Window With Autoscale");
#ifdef FT_HAS_GRAPHVIEW
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapCmdsBase::getActiveGraphView()->zoomAllInWindow(); }));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isGraphViewActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_zoomIn");
  cmdItem->setSmallIcon(zoomIn_xpm);
  cmdItem->setText("Zoom In");
  cmdItem->setToolTip("Zoom In");
#ifdef FT_HAS_GRAPHVIEW
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapCmdsBase::getActiveGraphView()->zoomIn(); }));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isGraphViewActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_zoomOut");
  cmdItem->setSmallIcon(zoomOut_xpm);
  cmdItem->setText("Zoom Out");
  cmdItem->setToolTip("Zoom Out");
#ifdef FT_HAS_GRAPHVIEW
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapCmdsBase::getActiveGraphView()->zoomOut(); }));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isGraphViewActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_shiftLeft");
  cmdItem->setSmallIcon(panLeft_xpm);
  cmdItem->setText("Pan Left");
  cmdItem->setToolTip("Pan Left");
#ifdef FT_HAS_GRAPHVIEW
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapCmdsBase::getActiveGraphView()->shiftLeft(); }));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isGraphViewActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_shiftRight");
  cmdItem->setSmallIcon(panRight_xpm);
  cmdItem->setText("Pan Right");
  cmdItem->setToolTip("Pan Right");
#ifdef FT_HAS_GRAPHVIEW
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapCmdsBase::getActiveGraphView()->shiftRight(); }));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isGraphViewActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_shiftUp");
  cmdItem->setSmallIcon(panUp_xpm);
  cmdItem->setText("Pan Up");
  cmdItem->setToolTip("Pan Up");
#ifdef FT_HAS_GRAPHVIEW
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapCmdsBase::getActiveGraphView()->shiftUp(); }));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isGraphViewActive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_viewCtrl_shiftDown");
  cmdItem->setSmallIcon(panDown_xpm);
  cmdItem->setText("Pan Down");
  cmdItem->setToolTip("Pan Down");
#ifdef FT_HAS_GRAPHVIEW
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FapCmdsBase::getActiveGraphView()->shiftDown(); }));
#endif
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isGraphViewActive,bool&));
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::zoomAll()
{
#ifdef USE_INVENTOR
  if (FapCmdsBase::getActiveModeller())
    FdDB::zoomAll();
#endif
#ifdef FT_HAS_GRAPHVIEW
  FuiGraphView* grafView = FapCmdsBase::getActiveGraphView();
  if (grafView) grafView->zoomAll();
#endif
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::zoomTo()
{
#ifdef USE_INVENTOR
  FmIsRenderedBase* obj;
  if (FapEventManager::hasTmpSelection())
    obj = dynamic_cast<FmIsRenderedBase*>(FapEventManager::getTmpMMBSelection());
  else
    obj = dynamic_cast<FmIsRenderedBase*>(FapEventManager::getFirstPermSelectedObject());

  FdDB::zoomTo(obj);
#endif
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::getZoomToSensitivity(bool& isSensitive)
{
  FmIsRenderedBase* obj;
  if (FapEventManager::hasTmpSelection())
    obj = dynamic_cast<FmIsRenderedBase*>(FapEventManager::getTmpMMBSelection());
  else
    obj = dynamic_cast<FmIsRenderedBase*>(FapEventManager::getFirstPermSelectedObject());

  isSensitive = obj ? true : false;
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::solidView(bool show)
{
  if (show) {
    FmDB::getActiveViewSettings()->setSolidMode(true,true);
    FapUACommandHandler::updateAllUICommandsToggle();
  }
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::getSolidViewToggle(bool& toggle)
{
  FmGlobalViewSettings* gvs = FmDB::getActiveViewSettings(false);
  toggle = gvs ? gvs->getSolidMode() && gvs->getSolidModeWithEdges() : false;
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::solidViewNoEdge(bool show)
{
  if (show) {
    FmDB::getActiveViewSettings()->setSolidMode(true, false);
    FapUACommandHandler::updateAllUICommandsToggle();
  }
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::getSolidViewNoEdgeToggle(bool& toggle)
{
  FmGlobalViewSettings* gvs = FmDB::getActiveViewSettings(false);
  toggle = gvs ? gvs->getSolidMode() && !gvs->getSolidModeWithEdges() : false;
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::lineView(bool show)
{
  if (show) {
    FmDB::getActiveViewSettings()->setSolidMode(false);
    FapUACommandHandler::updateAllUICommandsToggle();
  }
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::getLineViewToggle(bool& toggle)
{
  FmGlobalViewSettings* gvs = FmDB::getActiveViewSettings(false);
  toggle = gvs ? !gvs->getSolidMode() : false;
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::flatColorView(bool setFlat)
{
#ifdef USE_INVENTOR
  FdDB::setShading(!setFlat);
#else
  std::cout <<"  ** FapViewCtrlCmds::flatColorView("
            << std::boolalpha << setFlat <<")"<< std::endl;
#endif
  FapUACommandHandler::updateAllUICommandsToggle();
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::getFlatColorToggle(bool& isFlat)
{
#ifdef USE_INVENTOR
  isFlat = !FdDB::isShading();
#else
  isFlat = true;
#endif
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::topLightOnly(bool setFlat)
{
#ifdef USE_INVENTOR
  FdDB::setFrontFaceLightOnly(setFlat);
#else
  std::cout <<"  ** FapViewCtrlCmds::topLightOnly("
            << std::boolalpha << setFlat <<")"<< std::endl;
#endif
  FapUACommandHandler::updateAllUICommandsToggle();
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::getTopLightOnlyToggle(bool& isFlat)
{
#ifdef USE_INVENTOR
  isFlat = FdDB::isFrontFaceLightOnly();
#else
  isFlat = false;
#endif
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::perspectiveView(bool show)
{
  if (show) {
#ifdef USE_INVENTOR
    FdDB::perspectiveView();
#endif
    FapUACommandHandler::updateAllUICommandsToggle();
  }
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::getPerspectiveViewToggle(bool& toggle)
{
#ifdef USE_INVENTOR
  toggle = !FdDB::getOrthographicFlag();
#else
  toggle = true;
#endif
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::parallellView(bool show)
{
  if (show) {
#ifdef USE_INVENTOR
    FdDB::parallellView();
#endif
    FapUACommandHandler::updateAllUICommandsToggle();
  }
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::getParallellViewToggle(bool& toggle)
{
#ifdef USE_INVENTOR
  toggle = FdDB::getOrthographicFlag();
#else
  toggle = false;
#endif
}
//----------------------------------------------------------------------------

void FapViewCtrlCmds::symbolSize(double newSize)
{
  FmDB::getActiveViewSettings()->setSymbolScale(newSize);
  FapUAExistenceHandler* uav = FapUAExistenceHandler::getFirstOfType(FapUAViewSettings::getClassTypeID());
  if (uav) static_cast<FapUAViewSettings*>(uav)->updateUIValues();
}
//----------------------------------------------------------------------------
