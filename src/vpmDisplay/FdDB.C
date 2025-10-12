// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdEvent.H"
#include "vpmDisplay/FdDBPointSelectionData.H"
#include "vpmDisplay/FdCtrlDB.H"
#include "vpmDisplay/qtViewers/FdQtViewer.H"

#include "vpmUI/Fui.H"
#include "vpmUI/FuiModes.H"
#include "vpmUI/vpmUITopLevels/FuiModeller.H"
#include "vpmUI/vpmUITopLevels/FuiMainWindow.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFaLib/FFaDefinitions/FFaAppInfo.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"

#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/Fd2dPictureNode.H"
#include "vpmDisplay/Fd2DPoints.H"
#include "vpmDisplay/FdAxisCross.H"
#include "vpmDisplay/FdAnimationInfo.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdPickFilter.H"
#include "vpmDisplay/FdPickedPoints.H"
#include "vpmDisplay/FdExportIv.H"
#include "vpmDisplay/FdSelector.H"
#include "vpmDisplay/FdExtraGraphics.H"

#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdTransformKit.H"
#include "vpmDisplay/FdSimpleJointKit.H"
#include "vpmDisplay/FdLinJointKit.H"
#include "vpmDisplay/FdLoadDirEngine.H"
#include "vpmDisplay/FdLoadTransformKit.H"
#include "vpmDisplay/FdSprDaPlacer.H"
#include "vpmDisplay/FdSprDaTransformKit.H"
#include "vpmDisplay/FdTriadSwKit.H"
#include "vpmDisplay/FdRefPlaneKit.H"
#include "vpmDisplay/FdSeaStateKit.H"
#include "vpmDisplay/FdAppearanceKit.H"
#include "vpmDisplay/FdCamJointKit.H"
#include "vpmDisplay/FdCurveKit.H"
#include "vpmDisplay/FdFEModelKit.H"
#include "vpmDisplay/FdFEGroupPartKit.H"
#include "vpmDisplay/FdMultiplyTransforms.H"
#include "vpmDisplay/FdPart.H"
#include "vpmDisplay/FdBeam.H"
#include "vpmDisplay/FdRefPlane.H"
#include "vpmDisplay/FdAxialSprDa.H"
#include "vpmDisplay/FdSimpleJoint.H"
#include "vpmDisplay/FdRevJoint.H"
#include "vpmDisplay/FdPrismJoint.H"
#include "vpmDisplay/FdLinJoint.H"
#include "vpmDisplay/FdTriad.H"
#include "vpmDisplay/FdLoad.H"
#include "vpmDisplay/FdCamJoint.H"
#include "vpmDisplay/FdTire.H"
#include "vpmDisplay/FdSeaState.H"
#include "vpmDisplay/FdHP.H"
#include "vpmDisplay/FdSensor.H"
#include "vpmDisplay/FdStrainRosette.H"
#include "vpmDisplay/FdStrainRosetteKit.H"
#include "vpmDisplay/FdPipeSurfaceKit.H"
#include "vpmDisplay/FdPipeSurface.H"
#include "vpmDisplay/FdConverter.H"
#include "FFdCadModel/FdCadHandler.H"
#include "FFdCadModel/FdCadEdge.H"
#include "FFdCadModel/FdCadFace.H"
#include "FFdCadModel/FdCadInfo.H"

#include "vpmPM/FpPM.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmGlobalViewSettings.H"
#include "vpmDB/FmSubAssembly.H"
#include "vpmDB/FmSeaState.H"
#include "vpmDB/FmRefPlane.H"
#include "vpmDB/FmRevJoint.H"
#include "vpmDB/FmCamJoint.H"
#include "vpmDB/FmCylJoint.H"
#include "vpmDB/FmPrismJoint.H"
#include "vpmDB/FmBallJoint.H"
#include "vpmDB/FmRigidJoint.H"
#include "vpmDB/FmFreeJoint.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmBeam.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmTire.H"
#include "vpmDB/FmLoad.H"
#include "vpmDB/FmCtrlElementBase.H"
#include "vpmDB/FmCtrlLine.H"
#include "vpmDB/FmCreate.H"
#include "vpmApp/FapEventManager.H"

#include <QtOpenGL>
#include <QWidget>

#ifdef win32
#include <windows.h>
#endif

#include <Inventor/Qt/SoQt.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/manips/SoCenterballManip.h>
#include <Inventor/manips/SoTrackballManip.h>
#include <Inventor/manips/SoJackManip.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SoOffscreenRenderer.h>
#include <Inventor/actions/SoLineHighlightRenderAction.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/details/SoNodeKitDetail.h>
#include <Inventor/details/SoLineDetail.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoEnvironment.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/SoLists.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/C/tidbits.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoExtSelection.h>
#include <Inventor/nodes/SoPolygonOffset.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodekits/SoNodeKitListPart.h>

#ifdef USE_SMALLCHANGE
#include <SmallChange/nodekits/LegendKit.h>
#include <SmallChange/misc/Init.h>
#endif

// This file is generated by using gimp (Gnu Image manipulation)
// and exported as C header file. It contains a struct with image metadata,
// and the pixel data. Windows compiler has trouble when the size of the
// initialization string exceeds 65535 so keep the size small.
#include "FdDemoWarningPicture.h"

static FdDBPointSelectionData pointSelData;
static const FaVec3 negZaxis(0.0,0.0,-1.0);


///////////////////////////////////////////////////
//
// Initialisation of static variables in the FdDB
//
//////////////////////////////////////////////////

SoExtSelection *FdDB::selectionRoot= NULL;
SoSeparator    *FdDB::ourExtraGraphicsRoot = NULL;
SoSeparator    *FdDB::ourRoot      = NULL;
FdMechanismKit *FdDB::mechanismKit = NULL;
SoEnvironment  *FdDB::fogNode      = NULL;
SoLightModel   *FdDB::lightModel   = NULL;
SoShapeHints   *FdDB::ourShapeHint = NULL;

// Point-to-point move variables:

FaDOF     FdDB::smartMoveDOF;
FdObject *FdDB::objectToMoveTo = NULL;
FdObject *FdDB::objectToMove = NULL;
int       FdDB::DOF = FdObject::FREE;

// Create variables:

FdObject *FdDB::firstObjectToCreateNear = NULL;
FmIsRenderedBase *FdDB::firstPickedFmObject = NULL;
FaVec3    FdDB::firstCreateDirection(negZaxis);
bool      FdDB::firstCreateDirDefined = false;

FdObject *FdDB::secondObjectToCreateNear = NULL;
FmIsRenderedBase *FdDB::secondPickedFmObject = NULL;
FaVec3    FdDB::secondCreateDirection(negZaxis);
bool      FdDB::secondCreateDirDefined = false;

FdObject *FdDB::thirdObjectToCreateNear = NULL;
FmIsRenderedBase *FdDB::thirdPickedFmObject = NULL;
FaVec3    FdDB::thirdCreateDirection(negZaxis);
bool      FdDB::thirdCreateDirDefined = false;

FmCamJoint* FdDB::tempCam = NULL;
bool      FdDB::ourAllowCompleteCamCurveSelection = true;

// Attach Vars:

FdObject * FdDB::objectToAttach = NULL;
FdObject * FdDB::linkToAttachTo = NULL;

// Detach Vars:

FdObject *FdDB::objectToDetach = NULL;

// Viewer :

FdQtViewer    *FdDB::viewer = NULL;

// Highlight type:

bool FdDB::usesLineHighlight = true;

// Axis cross in viewer:

FdAxisCross *FdDB::axisCross = NULL;

// Animation info node:

FdAnimationInfo *FdDB::animationInfo = NULL;

// Legend:

LegendKit * FdDB::legend = NULL;

Fd2dPictureNode * FdDB::demoWarning = NULL;


//////////////////////////////////////////////////////
//
//     Class Methods
//
////////////////////////////////////////////////////////

void FdDB::init()
{
#ifdef sunos
  coin_setenv("COIN_FULL_INDIRECT_RENDERING", "1", 1);
  coin_setenv("SOQT_NO_X11_ERRORHANDLER", "1", 1);
#endif

  SoQt::init(dynamic_cast<QWidget*>(Fui::getMainWindow()));
#ifdef USE_SMALLCHANGE
  smallchange_init();
#endif

  coin_setenv("COIN_OFFSCREENRENDERER_TILEWIDTH", "1600", 1);
  coin_setenv("COIN_OFFSCREENRENDERER_TILEHEIGHT", "1200", 1);

  // init the custom Inventor classes:
  FdBackPointer::init();
  FdSymbolKit::init();
  FdLoadDirEngine::init();
  FdSprDaPlacer::init();
  FdTransformKit::init();
  FdSprDaTransformKit::init();
  FdLoadTransformKit::init();
  FdSimpleJointKit::init();
  FdLinJointKit::init();
  FdTriadSwKit::init();
  FdMechanismKit::init();
  Fd2dPictureNode::init();
  Fd2DPoints::init();
  FdAxisCross::init();
  FdAnimationInfo::init();
  FdRefPlaneKit::init();
  FdSeaStateKit::init();
  FdAppearanceKit::init();
  FdExtraGraphics::init();
  FdCamJointKit::init();
  FdCurveKit::init();
  FdStrainRosetteKit::init();
  FdPipeArcKit::init();
  FdPipeSurfaceKit::init();
  FdFEModelKit::init();
  FdFEGroupPartKit::init();
  FdMultiplyTransforms::init();

#ifdef USE_SMALLCHANGE
  LegendKit::initClass();
#endif

  FdCadHandler::initFdCad();

  FdDB::selectionRoot = new SoExtSelection;
  FdDB::ourExtraGraphicsRoot = new SoSeparator;
  FdDB::ourRoot          = new SoSeparator;

  FdEvent::init();
  FdCtrlDB::init();
}


void FdDB::start()
{
  FdCtrlDB::start(!FFaAppInfo::isConsole());

  if (FFaAppInfo::isConsole())
  {
    // Initialize the static symboldef class and build the symbols
    FdSymbolDefs::init();
    return;
  }

  // The remaining is for interactive runs only

  FdDB::viewer = dynamic_cast<FdQtViewer*>(Fui::getViewer());
  FdDB::mechanismKit = new FdMechanismKit;

  // Set up selection node:

#ifdef FD_SHOW_TRIANGLES
  FdDB::selectionRoot->ref();
  FdDB::selectionRoot->policy = SoSelection::SINGLE;
  FdDB::viewer->setGLRenderAction(new SoLineHighlightRenderAction);
#endif

  FdDB::selectionRoot->setPickMatching(false);

  // Initialize static class to handle changing of picked points
  FdPickedPoints::init();

  // Make axis cross with g-vector
  FdDB::axisCross = new FdAxisCross;
  FdDB::axisCross->corner.setValue(FdAxisCross::LOWERLEFT);
  FdDB::axisCross->gravityColor.setValue(0.95f, 0.55f, 0.01f);

  // Make animation info object
  FdDB::animationInfo = new FdAnimationInfo;

  // Make legend
#ifdef USE_SMALLCHANGE
  FdDB::legend = new LegendKit();
  FdDB::legend->ref();
  FdDB::legend->setTickAndLinesColor(SbColor(1,1,0));
  FdDB::legend->setTextColor(SbColor(1,1,1));
  FdDB::legend->enableBackground(false);
  FdDB::legend->imageWidth.setValue(10);
  FdDB::legend->on.setValue(false);
#endif

  // Set up possible demo warning on viewer:

  FdDB::demoWarning = new Fd2dPictureNode();
  FdDB::demoWarning->position.setValue(0.35f, 0.3f);
  FdDB::demoWarning->scale.setValue(1, 1);
  FdDB::demoWarning->isOn.setValue(false); // Turn off initially

  // Flip the bitmap buffer to make it compliant with the OpenGL
  // 2D coordinate system.
  Fd2dPictureNode::flipBuffer((unsigned char*)gimp_image.pixel_data, gimp_image.width, gimp_image.height);
  FdDB::demoWarning->setPixmapData((unsigned char*)gimp_image.pixel_data, gimp_image.width, gimp_image.height);

  // Set up viewer :
  FdDB::setNiceTransparency(true);
  FdDB::viewer->setHeadlight(true);
  FdDB::usesLineHighlight = true;
  FdDB::viewer->redrawOnSelectionChange(selectionRoot);

  // Set up Environment in viewer:
  FdDB::fogNode = new SoEnvironment;
  FdDB::fogNode->ambientIntensity.setValue(0.2f);
  FdDB::fogNode->fogColor.setValue(0,0,0);
  FdDB::lightModel = new SoLightModel;
  FdDB::lightModel->model.setValue(SoLightModel::PHONG);

  // Set up shape hints on the objects included
  FdDB::ourShapeHint = new SoShapeHints;
  FdDB::ourShapeHint->shapeType      = SoShapeHints::UNKNOWN_SHAPE_TYPE;
  FdDB::ourShapeHint->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  FdDB::ourShapeHint->creaseAngle    = 0;

  // Build top of graph:
  // The screenInfo switch is used to turn off screen space stuff when doing view all.
  SoSwitch * screenInfoSep = new SoSwitch;
  screenInfoSep->setName("FdQtViewerIgnoreViewAllSwitch");
  screenInfoSep->whichChild.setValue(SO_SWITCH_ALL);

  // Set up event callback nodes:
  FdEvent::start(FdDB::ourRoot);

  FdDB::ourRoot->addChild(ourShapeHint);
  FdDB::ourRoot->addChild(fogNode);
  FdDB::ourRoot->addChild(lightModel);
  FdDB::ourRoot->addChild(new SoPolygonOffset);
  FdDB::ourRoot->addChild(FdDB::selectionRoot);
  FdDB::selectionRoot->addChild(mechanismKit);

  FdDB::ourRoot->addChild(screenInfoSep);
  screenInfoSep->addChild(axisCross);
  screenInfoSep->addChild(animationInfo);
  screenInfoSep->addChild(FdPickedPoints::getHighlighter());
  screenInfoSep->addChild(FdDB::demoWarning);
#ifdef USE_SMALLCHANGE
  screenInfoSep->addChild(legend);
#endif
  FdDB::ourRoot->addChild(FdDB::ourExtraGraphicsRoot);

  // Switch all symbols on by default:
  FdDB::showTriads(true);
  FdDB::showParts(true);
  FdDB::showBeams(true);
  FdDB::showBeamTriads(true);
  FdDB::showBeamCS(false);
  FdDB::showPartCS(false);
  FdDB::showJoints(true);
  FdDB::showSprDas(true);
  FdDB::showLoads(true);
  FdDB::showHPs(true);
  FdDB::showFeedbacks(true);
  FdDB::showStickers(true);
  FdDB::showRefPlanes(true);
  FdDB::showSeaStates(false);
  FdDB::showWaves(false);
  FdDB::showTires(true);
  FdDB::showStrainRosettes(true);

  // Initialize the static symboldef class and build the symbols
  FdSymbolDefs::init();
}


void FdDB::enableDemoWarning(bool enable)
{
  if (FdDB::demoWarning)
    FdDB::demoWarning->isOn.setValue(enable);
}


//
//    Get and set methods
//
////////////////////////////////////////

void FdDB::updateGDirection(const FaVec3& gDir)
{
  if (FFaAppInfo::isConsole()) return;

  FdDB::axisCross->gravityVector.setValue(FdConverter::toSbVec3f(gDir));
}


cameraData FdDB::getView()
{
  SbMatrix mx;
  viewer->getOrient(mx);
  FaMat34 myOrientation = FdConverter::toFaMat34(mx);
  myOrientation[VW] = FdConverter::toFaVec3(viewer->getPos());

  return { myOrientation,
      viewer->getFocalDistance(),
      viewer->getOHeightOrHAngle(),
      viewer->isOrthographicView()
    };
}


double FdDB::getCameraDistance(const FaVec3& from)
{
  return (FdConverter::toFaVec3(FdDB::viewer->getPos()) - from).length();
}


bool FdDB::getOrthographicFlag()
{
  return FdDB::viewer->isOrthographicView();
}


void FdDB::setView(const cameraData& cd)
{
  if (cd.itsIsOrthographicFlag)
    FdDB::parallellView();
  else
    FdDB::perspectiveView();

  viewer->setPosition(FdConverter::toSbMatrix(cd.itsCameraOrientation));
  viewer->setFocalDistance(cd.itsFocalDistance);
  viewer->setOHeightOrHAngle(cd.itsHeight);
}


void FdDB::setLinkToFollow(FmIsRenderedBase* link)
{
  if (FFaAppInfo::isConsole()) return;

  FdLink* dlink = link ? dynamic_cast<FdLink*>(link->getFdPointer()) : NULL;
  FdFEModelKit* feMod = dlink ? static_cast<FdFEModelKit*>(dlink->getVisualModel()) : NULL;
  viewer->setRelativeTransform(feMod ? (SoTransform*)feMod->getPart("transform",false) : NULL);
}


FaVec3 FdDB::getPPoint(int idx, bool GlobalYesOrNo)
{
  return FdPickedPoints::getPickedPoint(idx,GlobalYesOrNo);
}


bool FdDB::setPPoint(int idx, bool GlobalYesOrNo, const FaVec3& fromPoint)
{
  // Don't change anything if the new point location is
  // within the position tolerance of the old location
  if (fromPoint.equals(FdPickedPoints::getPickedPoint(idx,GlobalYesOrNo),
                       FmDB::getPositionTolerance())) return false;

  FdPickedPoints::setPickedPoint(idx,GlobalYesOrNo,fromPoint);
  if (idx == 0)
  {
    FdDB::firstObjectToCreateNear = NULL; // Bugfix #388: To allow creation
    // of triads at arbitrary point, after picking on an FE part first.
    FdExtraGraphics::moveDirection(FdPickedPoints::getFirstPickedPoint());
  }

  return true;
}


//
//      Event, state and mode handling
//
////////////////////////////////////////////////////////////////////////

/*!
  Setter opp callBacks noedvendig naar en ny modus entres.
  Forrige modus er alltid EXAM_MODE fordi det blir kjoert cancel()
  foer updateMode
*/

void FdDB::updateMode()
{
  bool showDirVec = false;
  double xPP = -0.2;

  switch (FuiModes::getMode())
    {
    case FuiModes::APPEARANCE_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::addEventCB(FdDB::appearanceEventCB);
      FdDB::usesLineHighlight = false;
      break;

    case FuiModes::PTPMOVE_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::addEventCB(FdDB::smartMoveEventCB);
      FdDB::objectToMove = NULL;
      FdDB::objectToMoveTo = NULL;
      FdDB::DOF = FdObject::FREE;
      FdPickedPoints::resetPPs();
      break;

    case FuiModes::ATTACH_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::addEventCB(FdDB::attachEventCB);
      break;

    case FuiModes::DETACH_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::addEventCB(FdDB::detachEventCB);
      break;

    case FuiModes::MAKEFORCE_MODE:
    case FuiModes::MAKETORQUE_MODE:
    case FuiModes::MAKEREVJOINT_MODE:
      showDirVec = true;
    case FuiModes::MAKEBALLJOINT_MODE:
    case FuiModes::MAKERIGIDJOINT_MODE:
    case FuiModes::MAKETRIAD_MODE:
    case FuiModes::MAKESTICKER_MODE:
    case FuiModes::COMPICKPOINT_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::addEventCB(FdDB::onePickCreateEventCB);
      FdDB::firstObjectToCreateNear = NULL;
      FdDB::firstCreateDirection = negZaxis;
      FdPickedPoints::resetPPs();
      FdPickedPoints::setFirstPP(FaVec3(),FaMat34());
      if (showDirVec)
        FdExtraGraphics::showDirection(FdPickedPoints::getFirstPickedPoint(),FdDB::firstCreateDirection);
      break;

    case FuiModes::MAKEFREEJOINT_MODE:
    case FuiModes::MAKEFREEJOINTBETWEENTRIADS_MODE:
      xPP = 0.0;
    case FuiModes::MAKECYLJOINT_MODE:
    case FuiModes::MAKEPRISMJOINT_MODE:
    case FuiModes::MAKESPRING_MODE:
    case FuiModes::MAKEDAMPER_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::addEventCB(FdDB::twoPickCreateEventCB);
      FdDB::firstObjectToCreateNear = NULL;
      FdDB::secondObjectToCreateNear = NULL;
      FdDB::firstPickedFmObject = NULL;
      FdDB::secondPickedFmObject = NULL;
      FdDB::secondCreateDirection = negZaxis;
      FdPickedPoints::resetPPs();
      FdPickedPoints::setFirstPP(FaVec3(xPP,0.0,0.0),FaMat34());
      break;

    case FuiModes::MAKECYLJOINTBETWEENTRIADS_MODE:
    case FuiModes::MAKEPRISMJOINTBETWEENTRIADS_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::addEventCB(FdDB::threePickCreateEventCB);
      FdDB::firstObjectToCreateNear = NULL;
      FdDB::secondObjectToCreateNear = NULL;
      FdDB::thirdObjectToCreateNear = NULL;
      FdDB::firstPickedFmObject = NULL;
      FdDB::secondPickedFmObject = NULL;
      FdDB::thirdPickedFmObject = NULL;
      FdDB::secondCreateDirection = negZaxis;
      FdDB::thirdCreateDirection  = negZaxis;
      FdPickedPoints::resetPPs();
      FdPickedPoints::setFirstPP(FaVec3(),FaMat34());
      break;

    case FuiModes::MAKECAMJOINT_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::addEventCB(FdDB::makeCamJointEventCB);
      FdDB::ourAllowCompleteCamCurveSelection = true;
      FdDB::firstObjectToCreateNear = NULL;
      FdDB::firstCreateDirection = negZaxis;
      FdPickedPoints::resetPPs();
      FdPickedPoints::setFirstPP(FaVec3(),FaMat34());
      FdExtraGraphics::showDirection(FdPickedPoints::getFirstPickedPoint(),FdDB::firstCreateDirection);
      FdDB::tempCam = NULL;
      break;

    case FuiModes::MAKEGEAR_MODE:
    case FuiModes::MAKERACKPIN_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::addEventCB(FdDB::createHPEventCB);
      FdDB::firstObjectToCreateNear = NULL;
      FdDB::secondObjectToCreateNear = NULL;
      break;

    case FuiModes::MAKESIMPLESENSOR_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::addEventCB(FdDB::createSimpleSensorEventCB);
      break;

    case FuiModes::MAKETIRE_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::addEventCB(FdDB::createTireEventCB);
      break;

    case FuiModes::MAKERELATIVESENSOR_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::addEventCB(FdDB::createRelativeSensorEventCB);
      break;

    case FuiModes::PICKLOADFROMPOINT_MODE:
    case FuiModes::PICKLOADTOPOINT_MODE:
    case FuiModes::PICKLOADATTACKPOINT_MODE:
    case FuiModes::ADDMASTERINLINJOINT_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::addEventCB(FdDB::pickLoadPointEventCB);
      FdPickedPoints::resetPPs();
      break;

    case FuiModes::MEASURE_DISTANCE_MODE:
    case FuiModes::MEASURE_ANGLE_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::addEventCB(FdDB::pickMeasurePointEventCB);
      FdPickedPoints::resetPPs();
      break;

    default:
      break; // Do nothing for all the others
    }
}


/*!
  Skal soerge for at alt blir ryddet opp fra modi og state
  slik at iv vil oppfoere seg som EXAM_MODE.
  FuiModes soerger for aa sette modus og state variablene.
*/

void FdDB::cancel()
{
  switch (FuiModes::getMode())
    {
    case FuiModes::APPEARANCE_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::removeEventCB(FdDB::appearanceEventCB);
      FdDB::usesLineHighlight = true;
      break;

    case FuiModes::PTPMOVE_MODE:
      FapEventManager::permUnselectAll();
      FdExtraGraphics::hideDOFVisualizing();
      FdEvent::removeEventCB(FdDB::smartMoveEventCB);
      FdDB::objectToMove = NULL;
      FdDB::objectToMoveTo = NULL;
      FdDB::DOF = FdObject::FREE;
      FdPickedPoints::resetPPs();
      FdDB::hidePointSelectionUI();
      break;

    case FuiModes::ATTACH_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::removeEventCB(FdDB::attachEventCB);
      break;

    case FuiModes::DETACH_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::removeEventCB(FdDB::detachEventCB);
      break;

    case FuiModes::MAKEFORCE_MODE:
    case FuiModes::MAKETORQUE_MODE:
    case FuiModes::MAKETRIAD_MODE:
    case FuiModes::MAKEREVJOINT_MODE:
    case FuiModes::MAKEBALLJOINT_MODE:
    case FuiModes::MAKERIGIDJOINT_MODE:
    case FuiModes::MAKESTICKER_MODE:
    case FuiModes::COMPICKPOINT_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::removeEventCB(FdDB::onePickCreateEventCB);
      FdDB::firstObjectToCreateNear = NULL;
      FdDB::firstCreateDirection = negZaxis;
      FdPickedPoints::resetPPs();
      FdExtraGraphics::hideDirection();
      FdDB::hidePointSelectionUI();
      break;

    case FuiModes::MAKESPRING_MODE:
    case FuiModes::MAKEDAMPER_MODE:
    case FuiModes::MAKEFREEJOINT_MODE:
    case FuiModes::MAKEFREEJOINTBETWEENTRIADS_MODE:
    case FuiModes::MAKECYLJOINT_MODE:
    case FuiModes::MAKEPRISMJOINT_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::removeEventCB(FdDB::twoPickCreateEventCB);
      FdDB::firstObjectToCreateNear = NULL;
      FdDB::firstCreateDirection = negZaxis;
      FdDB::secondObjectToCreateNear = NULL;
      FdDB::secondCreateDirection = negZaxis;
      FdPickedPoints::resetPPs();
      FdExtraGraphics::hideDirection();
      FdDB::hidePointSelectionUI();
      break;

    case FuiModes::MAKECYLJOINTBETWEENTRIADS_MODE:
    case FuiModes::MAKEPRISMJOINTBETWEENTRIADS_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::removeEventCB(FdDB::threePickCreateEventCB);
      FdDB::firstObjectToCreateNear = NULL;
      FdDB::firstCreateDirection = negZaxis;
      FdDB::secondObjectToCreateNear = NULL;
      FdDB::secondCreateDirection = negZaxis;
      FdDB::thirdObjectToCreateNear = NULL;
      FdDB::thirdCreateDirection = negZaxis;
      FdPickedPoints::resetPPs();
      FdExtraGraphics::hideDirection();
      FdDB::hidePointSelectionUI();
      break;

    case FuiModes::MAKECAMJOINT_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::removeEventCB(FdDB::makeCamJointEventCB);
      FdDB::firstObjectToCreateNear = NULL;
      FdDB::firstCreateDirection = negZaxis;
      FdDB::ourAllowCompleteCamCurveSelection = true;
      FdDB::tempCam = NULL;
      FdPickedPoints::resetPPs();
      FdExtraGraphics::hideDirection();
      FdDB::hidePointSelectionUI();
      break;

    case FuiModes::MAKEGEAR_MODE:
    case FuiModes::MAKERACKPIN_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::removeEventCB(FdDB::createHPEventCB);
      FdDB::firstObjectToCreateNear = NULL;
      FdDB::secondObjectToCreateNear = NULL;
      break;

    case FuiModes::MAKESIMPLESENSOR_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::removeEventCB(FdDB::createSimpleSensorEventCB);
      break;

    case FuiModes::MAKETIRE_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::removeEventCB(FdDB::createTireEventCB);
      break;

    case FuiModes::MAKERELATIVESENSOR_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::removeEventCB(FdDB::createRelativeSensorEventCB);
      break;

    case FuiModes::PICKLOADFROMPOINT_MODE:
    case FuiModes::PICKLOADTOPOINT_MODE:
    case FuiModes::PICKLOADATTACKPOINT_MODE:
    case FuiModes::ADDMASTERINLINJOINT_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::removeEventCB(FdDB::pickLoadPointEventCB);
      FdPickedPoints::resetPPs();
      break;

    case FuiModes::MEASURE_DISTANCE_MODE:
    case FuiModes::MEASURE_ANGLE_MODE:
      FapEventManager::permUnselectAll();
      FdEvent::removeEventCB(FdDB::pickMeasurePointEventCB);
      FdPickedPoints::resetPPs();
      break;

    default:
      break; // Do nothing for all the others
    }
}


/*!
  Skal soerge for aa sette opp riktige callback
  ved overgang til ny state
  Og handle hvis det er paakrevet (Utfoere smart move f.eks.)
*/

void FdDB::updateState(int newState)
{
  int mode  = FuiModes::getMode();
  int state = FuiModes::getState();
  double X1 = -0.2;
  double dX = 0.4;

  switch (mode)
    {
    case FuiModes::EXAM_MODE:
    case FuiModes::APPEARANCE_MODE:
      if (state == 1 && newState == 0)
        FapEventManager::permUnselectAll();
      break;

    case FuiModes::PTPMOVE_MODE:
      switch (state)
        {
        case 1:
          if (newState == 0)
            {
              FapEventManager::permUnselectAll();
              FdPickedPoints::resetPPs();
              FdExtraGraphics::hideDOFVisualizing();
              FdDB::objectToMove = NULL;
            }
          break;

        case 2:
        case 3:
          if (newState == 0)
            {
              FpPM::vpmSetUndoPoint("Move");
              FdExtraGraphics::hideDOFVisualizing();
              if (state == 3)
                FapEventManager::permUnselectLast();
              FdSelector::smartMoveSelection(FdPickedPoints::getFirstPickedPoint(),
                                             FdPickedPoints::getSecondPickedPoint(),
                                             FdDB::smartMoveDOF);
              FapEventManager::permUnselectAll();
              FdPickedPoints::resetPPs();
              FdDB::objectToMove = NULL;
              FdDB::objectToMoveTo = NULL;
            }
          else if (state == 3 && newState == 2)
            {
              FdPickedPoints::removeSecondPP();
              FdDB::objectToMoveTo = NULL;
            }
          break;
        }
      break;

    case FuiModes::ERASE_MODE:
      if (state == 1 && newState == 0)
        {
          FpPM::vpmSetUndoPoint("Delete");
          std::vector<FdObject*> objectsToErase;
          FdSelector::getSelectedObjects(objectsToErase);
          for (FdObject* obj : objectsToErase)
            obj->getFmOwner()->interactiveErase();
          FFaMsg::resetToAllAnswer();
        }
      break;

    case FuiModes::ATTACH_MODE:
      switch (state)
        {
        case 1:
        case 2:
          if (newState == 0)
            FapEventManager::permUnselectAll();
          break;

        case 3:
          if (newState == 0)
            {
              FpPM::vpmSetUndoPoint("Attach");
              // When a reference plane is supposed to represent earth,
              // we have to check here what kind of object linkToAttachTo is
              FmIsRenderedBase* link = FdDB::linkToAttachTo->getFmOwner();
              FmIsRenderedBase* obj = FdDB::objectToAttach->getFmOwner();
              FapEventManager::permUnselectAll();
              if (FdDB::linkToAttachTo->isOfType(FdLink::getClassTypeID()))
                {
                  ListUI <<"Attaching "<< obj->getIdString() <<" to "<< link->getIdString() <<"\n";
                  if (static_cast<FmLink*>(link)->attach(obj))
                    FFaMsg::list("Attached.\n");
                  else
                    FFaMsg::list("Could not attach !\n",true);
                }
              else if (FdDB::linkToAttachTo->isOfType(FdRefPlane::getClassTypeID()))
                {
                  ListUI <<"Attaching "<< obj->getIdString() <<" to ground.\n";
                  if (static_cast<FmRefPlane*>(link)->attach(obj))
                    FFaMsg::list("Attached.\n");
                  else
                    FFaMsg::list("Could not attach to ground !\n",true);
                }
            }
          else if (newState == 2)
            FapEventManager::permUnselect(1);
          break;
        }
      break;

    case FuiModes::DETACH_MODE:
      if (state == 1 && newState == 0 && FdDB::objectToDetach)
        {
          FpPM::vpmSetUndoPoint("Detach");
          // Deselect before detaching
          FapEventManager::permUnselectAll();
          FmIsRenderedBase* obj = FdDB::objectToDetach->getFmOwner();
          ListUI <<"Detaching "<< obj->getIdString() <<"\n";
          if (obj->detach())
            FFaMsg::list("Detached.\n");
          else
            FFaMsg::list("Could not detach !\n",true);
        }
      break;

    case FuiModes::MAKEFORCE_MODE:
    case FuiModes::MAKETORQUE_MODE:
      switch (state)
        {
        case 0:
          if (newState == 2) {
            FdDB::firstObjectToCreateNear = NULL;
            FdDB::objectCreator(mode);
          }
          break;

        case 1:
          if (newState == 0)
            {
              FapEventManager::permUnselectAll();
              FdDB::firstObjectToCreateNear = NULL;
              FdDB::firstCreateDirection = negZaxis;
              FdPickedPoints::setFirstPP(FaVec3(),FaMat34());
              FdExtraGraphics::showDirection(FdPickedPoints::getFirstPickedPoint(),FdDB::firstCreateDirection);
              FdDB::hidePointSelectionUI();
            }
          else if (newState == 2)
            {
              FdDB::objectCreator(mode);
              FdDB::hidePointSelectionUI();
            }
          break;
        }
      break;

    case FuiModes::MAKETRIAD_MODE:
      switch (state)
        {
        case 0:
          if (newState == 2) {
            FdDB::firstObjectToCreateNear = NULL;
            FdDB::objectCreator(mode);
          }
          break;

        case 1:
          if (newState == 0)
            {
              FapEventManager::permUnselectAll();
              FdDB::firstObjectToCreateNear = NULL;
              FdPickedPoints::setFirstPP(FaVec3(),FaMat34());
              FdDB::hidePointSelectionUI();
            }
          else if (newState == 2)
            {
              FapEventManager::permUnselectAll();
              FdDB::objectCreator(mode);
              FdDB::hidePointSelectionUI();
            }
          break;
        }
      break;

    case FuiModes::MAKEREVJOINT_MODE:
    case FuiModes::MAKEBALLJOINT_MODE:
    case FuiModes::MAKERIGIDJOINT_MODE:
      switch (state)
        {
        case 0:
          if (newState == 2)
            FdDB::objectCreator(mode);
          break;

        case 1:
          if (newState == 0)
            {
              FapEventManager::permUnselectAll();
              FdDB::firstObjectToCreateNear = NULL;
              if (mode == FuiModes::MAKEREVJOINT_MODE)
                FdDB::firstCreateDirection = negZaxis;
              FdPickedPoints::setFirstPP(FaVec3(),FaMat34());
              if (mode == FuiModes::MAKEREVJOINT_MODE)
                FdExtraGraphics::showDirection(FdPickedPoints::getFirstPickedPoint(),FdDB::firstCreateDirection);
              FdDB::hidePointSelectionUI();
            }
          else if (newState == 2)
            {
              FdDB::objectCreator(mode);
              FdDB::hidePointSelectionUI();
            }
          break;
        }
      break;

    case FuiModes::MAKEFREEJOINTBETWEENTRIADS_MODE:
      X1 = dX = 0.0;
      if (state != 8 && newState == 0)
      {
        FapEventManager::permUnselectAll();
        FdDB::firstObjectToCreateNear = NULL;
        FdDB::firstCreateDirection = negZaxis;
        FdPickedPoints::setFirstPP(FaVec3(X1,0.0,0.0),FaMat34());
        FdDB::hidePointSelectionUI();
      }
      else if (newState == 4 || newState == 6 || newState == 7)
      {
        FdDB::secondObjectToCreateNear = NULL;
        FdDB::secondCreateDirection = FdDB::firstCreateDirection;

        FdPickedPoints::setSecondPP(FdPickedPoints::getFirstPickedPoint() + FaVec3(dX,0.0,0.0), FaMat34());
        if (state >= 1 && state <= 7 && state != 4)
          FdDB::hidePointSelectionUI();
      }
      else if (state == 8 && newState == 0)
      {
        FdDB::objectCreator(mode);
        FdDB::firstObjectToCreateNear = NULL;
        FdDB::secondObjectToCreateNear = NULL;
        FdPickedPoints::removeSecondPP();
        FdExtraGraphics::hideDirection();
        FdDB::hidePointSelectionUI();
      }
      break;

    case FuiModes::MAKEPRISMJOINTBETWEENTRIADS_MODE:
    case FuiModes::MAKECYLJOINTBETWEENTRIADS_MODE:
      X1 = dX = 0.0;
      if (state != 2 && newState == 0)
      {
        FapEventManager::permUnselectAll();
        FdDB::firstObjectToCreateNear = NULL;
        FdDB::firstCreateDirection = negZaxis;
        FdPickedPoints::setFirstPP(FaVec3(X1,0.0,0.0), FaMat34());
        FdDB::hidePointSelectionUI();
      }
      else if (newState == 3 || newState == 5) // pick end-triad
      {
        FdDB::secondObjectToCreateNear = NULL;
        FdDB::secondCreateDirection = FdDB::firstCreateDirection;

        FdPickedPoints::setSecondPP(FdPickedPoints::getFirstPickedPoint() + FaVec3(dX,0.0,0.0), FaMat34());
        if (state == 1 || state == 4)
          FdDB::hidePointSelectionUI();
      }
      else if (newState == 6 || newState == 8 || newState == 9) // pick slave
      {
        FdDB::thirdObjectToCreateNear = NULL;
        FdDB::thirdCreateDirection = FdDB::secondCreateDirection;

        FdPickedPoints::setThirdPP(FdPickedPoints::getSecondPickedPoint() + FaVec3(dX,0.0,0.0), FaMat34());
        if (state == 1 || state == 4)
          FdDB::hidePointSelectionUI();
      }
      else if ((state == 6 || state == 7) && newState == 10) // create joint
      {
        FdDB::objectCreator(mode);
        FdDB::firstObjectToCreateNear = NULL;
        FdDB::secondObjectToCreateNear = NULL;
        FdDB::thirdObjectToCreateNear = NULL;
        FdPickedPoints::removeSecondPP();
        FdPickedPoints::removeThirdPP();
        FdExtraGraphics::hideDirection();
        FdDB::hidePointSelectionUI();
      }
    break;

    case FuiModes::MAKEFREEJOINT_MODE:
      X1 = dX = 0.0;
    case FuiModes::MAKECYLJOINT_MODE:
    case FuiModes::MAKEPRISMJOINT_MODE:
    case FuiModes::MAKESPRING_MODE:
    case FuiModes::MAKEDAMPER_MODE:
      switch (state)
        {
          case 0:
          case 1:
          case 3:
            if (state == 1 && newState == 0)
              {
                FapEventManager::permUnselectAll();
                FdDB::firstObjectToCreateNear = NULL;
                FdDB::firstCreateDirection = negZaxis;
                FdPickedPoints::setFirstPP(FaVec3(X1,0.0,0.0),FaMat34());
                FdDB::hidePointSelectionUI();
              }
            else if (newState == 2)
              {
                FdDB::secondObjectToCreateNear = NULL;
                if (mode == FuiModes::MAKEFREEJOINT_MODE)
                  FdDB::secondCreateDirection = FdDB::firstCreateDirection;
                else
                  FdDB::secondCreateDirection = negZaxis;
                FdPickedPoints::setSecondPP(FdPickedPoints::getFirstPickedPoint() + FaVec3(dX,0.0,0.0), FaMat34());
                if (state == 1 || state == 3)
                  FdDB::hidePointSelectionUI();
              }
            else if (state == 3 && newState == 4)
              {
                FdDB::objectCreator(mode);
                FdDB::firstObjectToCreateNear = NULL;
                FdDB::secondObjectToCreateNear = NULL;
                FdPickedPoints::removeSecondPP();
                FdExtraGraphics::hideDirection();
                FdDB::hidePointSelectionUI();
              }
            break;
          case 2:
            if (newState == 4)
              FdDB::objectCreator(mode);
            break;
        }
      break;

    case FuiModes::MAKECAMJOINT_MODE:
      switch (state)
        {
        case 0: // Bugfix #386: This also applies for state == 0
        case 1:
          if (newState == 0)
            {
              FapEventManager::permUnselectAll();
              FdDB::firstObjectToCreateNear = NULL;
              FdDB::firstCreateDirection = negZaxis;
              FdPickedPoints::setFirstPP(FaVec3(),FaMat34());
              FdExtraGraphics::showDirection(FdPickedPoints::getFirstPickedPoint(),FdDB::firstCreateDirection);
            }
          else if (newState == 2)
            {
              FpPM::vpmSetUndoPoint("Cam joint");

              std::vector<FmModelMemberBase*> selection;
              FmModelMemberBase* dummy;
              FapEventManager::getMMBSelection(selection,dummy);
              FmTriad* follower = NULL;
              FmPart* followerPart = NULL;
              if (!selection.empty()) {
                if (selection.front()->isOfType(FmTriad::getClassTypeID()))
                  follower = static_cast<FmTriad*>(selection.front());
                else if (selection.front()->isOfType(FmPart::getClassTypeID()))
                {
                  followerPart = static_cast<FmPart*>(selection.front());
                  follower = followerPart->getTriadAtPoint(FdPickedPoints::getFirstPickedPoint(),
                                                           FmDB::getPositionTolerance());
                }
              }
              if (!follower)
                follower = Fedem::createTriad(FdPickedPoints::getFirstPickedPoint(),followerPart);

              if (!(FdDB::tempCam = Fedem::createCamJoint(follower)))
                FuiModes::cancel();
              FapEventManager::permUnselectAll();
            }
          break;

        case 2:
          if (newState == 0)
            {
              FapEventManager::permUnselectAll();
              FdDB::firstObjectToCreateNear = NULL;
              FdDB::firstCreateDirection = negZaxis;
              FdPickedPoints::setFirstPP(FaVec3(),FaMat34());
              FdExtraGraphics::showDirection(FdPickedPoints::getFirstPickedPoint(),FdDB::firstCreateDirection);
              FdDB::ourAllowCompleteCamCurveSelection = true;
            }
          // Bugfix #386: No break here, continue as if state == 3

        case 3:
          if (newState == 4)
          {
            if (!tempCam)
            {
              std::cerr <<"LOGIC ERROR in FdDB::updateState(newState=4,state=3,mode=MAKECAMJOINT): "
                        <<"Should not be here when tempCam==NULL"<< std::endl;
              return;
            }
            std::vector<FmModelMemberBase*> selection;
            FmModelMemberBase* dummy;
            FapEventManager::getMMBSelection(selection,dummy);
            FmTriad* newMaster = NULL;
            if (!selection.empty()) {
              if (selection.front()->isOfType(FmTriad::getClassTypeID()))
                newMaster = static_cast<FmTriad*>(selection.front());
              else if (selection.front()->isOfType(FmLink::getClassTypeID()))
                newMaster = static_cast<FmLink*>(selection.front())->getTriadAtPoint(FdPickedPoints::getFirstPickedPoint(),
                                                                                     FmDB::getPositionTolerance());
            }

            if (!newMaster)
            {
              FaVec3 norm = FdDB::firstCreateDirection;
              FaVec3 camX = FdConverter::toFaVec3(FdDB::viewer->getPos());
              if (norm * (camX - FdPickedPoints::getFirstPickedPoint()) < 0.0)
                norm = -norm;

              std::vector<FmTriad*> mTriads;
              FdDB::tempCam->getMasterTriads(mTriads);

              FaVec3 tang(0.0,1.0,0.0);
              if (!mTriads.empty())
                tang = FdPickedPoints::getFirstPickedPoint() - mTriads.back()->getGlobalTranslation();

              if (tang.isParallell(norm,2.0e-7))
                tang = FaVec3(0.0,1.0,0.0);

              FaVec3 trans = (tang^norm).normalize();
              norm.normalize();
              tang = norm^trans;

              newMaster = Fedem::createTriad(FdPickedPoints::getFirstPickedPoint());
              newMaster->setOrientation(FaMat33(norm,trans,tang));
              Fedem::createSticker(newMaster,FdPickedPoints::getFirstPickedPoint());
            }

            FdDB::ourAllowCompleteCamCurveSelection = false;
            FdDB::tempCam->addAsMasterTriad(newMaster);
            FdDB::tempCam->setDefaultRotationOnMasters();
            newMaster->draw();
            newMaster->updateChildrenDisplayTopology();
            FapEventManager::permUnselectAll();
          }
          break;

        case 5:
          if (newState == 0)
          {
            if (!tempCam)
            {
              std::cerr <<"LOGIC ERROR in FdDB::updateState(newState=0,state=5,mode=MAKECAMJOINT): "
                        <<"Should not be here when tempCam==NULL"<< std::endl;
              return;
            }

            std::vector<FmModelMemberBase*> selection;
            FmModelMemberBase* dummy;
            FapEventManager::getMMBSelection(selection,dummy);

            if (!selection.empty() && selection.front()->isOfType(FmCamJoint::getClassTypeID()))
            {
              FmCamJoint* selectedCam = static_cast<FmCamJoint*>(selection.front());
              FdDB::tempCam->setMaster(selectedCam->getMaster());
              FdDB::tempCam->setThickness(selectedCam->getThickness());
              FdDB::tempCam->setWidth(selectedCam->getWidth());
              FdDB::tempCam->setFriction(selectedCam->getFriction());
              FdDB::tempCam->draw();
            }
            FapEventManager::permUnselectAll();
            FdDB::firstObjectToCreateNear = NULL;
            FdDB::firstCreateDirection = negZaxis;
            FdPickedPoints::setFirstPP(FaVec3(),FaMat34());
            FdExtraGraphics::showDirection(FdPickedPoints::getFirstPickedPoint(),FdDB::firstCreateDirection);
            FdDB::ourAllowCompleteCamCurveSelection = true;
          }
          break;
        }
      break;

    case FuiModes::MAKEGEAR_MODE:
    case FuiModes::MAKERACKPIN_MODE:
      switch (state)
        {
        case 1:
         if (newState == 0)
           {
             FapEventManager::permUnselectAll();
             FdDB::firstObjectToCreateNear = NULL;
           }
         else if (newState == 2)
           {
             FdDB::secondObjectToCreateNear = NULL;
           }
         break;
        case 3:
          if (newState == 2)
            {
              FdDB::secondObjectToCreateNear = NULL;
            }
          else if (newState == 4)
            {
              FdDB::objectCreator(mode);
              FdDB::firstObjectToCreateNear = NULL;
              FdDB::secondObjectToCreateNear = NULL;
            }
        }
      break;

    case FuiModes::MAKESTICKER_MODE:
      switch (state)
        {
        case 1:
          if (newState == 0)
            {
              FapEventManager::permUnselectAll();
              FdDB::firstObjectToCreateNear = NULL;
              FdDB::firstCreateDirection = negZaxis;
              FdPickedPoints::resetPPs();
              FdExtraGraphics::hideDirection();
            }
          else if (newState == 2 && FdDB::firstObjectToCreateNear)
            {
              FdDB::objectCreator(mode);
              FdDB::firstObjectToCreateNear = NULL;
              FdDB::firstCreateDirection = negZaxis;
              FdPickedPoints::resetPPs();
              FdExtraGraphics::hideDirection();
            }
          break;
        }
      break;

    case FuiModes::MAKESIMPLESENSOR_MODE:
      switch (state)
        {
        case 1:
          if (newState == 0)
            FapEventManager::permUnselectAll();
          else if (newState == 2)
            FdDB::objectCreator(mode);
          break;
        }
      break;

    case FuiModes::MAKETIRE_MODE:
      switch (state)
        {
        case 1:
          if (newState == 0)
            FapEventManager::permUnselectAll();
          else if (newState == 2)
            FdDB::objectCreator(mode);
          break;
        }
      break;

    case FuiModes::MAKERELATIVESENSOR_MODE:
      switch (state)
        {
        case 1:
          if (newState == 0)
            FapEventManager::permUnselectAll();
          break;
        case 3:
          if (newState == 2)
            FapEventManager::permUnselect(1);
          else if (newState == 4)
            FdDB::objectCreator(mode);
          break;
        }
      break;

    case FuiModes::PICKLOADFROMPOINT_MODE:
    case FuiModes::PICKLOADTOPOINT_MODE:
    case FuiModes::PICKLOADATTACKPOINT_MODE:
    case FuiModes::ADDMASTERINLINJOINT_MODE:
      switch (state)
        {
        case 1:
          if (newState == 2)
            switch (mode)
              {
              case FuiModes::PICKLOADFROMPOINT_MODE:
                FmLoad::changeFromPt(dynamic_cast<FmIsPositionedBase*>(FapEventManager::getPermSelectedObject(0)),
                                     FdPickedPoints::getFirstPickedPoint());
                break;
              case FuiModes::PICKLOADTOPOINT_MODE:
                FmLoad::changeToPt(dynamic_cast<FmIsPositionedBase*>(FapEventManager::getPermSelectedObject(0)),
                                   FdPickedPoints::getFirstPickedPoint());
                break;
              case FuiModes::PICKLOADATTACKPOINT_MODE:
                FmLoad::changeAttackPt(dynamic_cast<FmLink*>(FapEventManager::getPermSelectedObject(0)),
                                       FdPickedPoints::getFirstPickedPoint());
                break;
              case FuiModes::ADDMASTERINLINJOINT_MODE:
                FmMMJointBase::addMasterTriad(FdPickedPoints::getFirstPickedPoint());
                break;
              }

        case 2:
          if (newState == 0 || (state == 1 && newState == 2))
            {
              FapEventManager::permUnselectAll();
              FdPickedPoints::resetPPs();
            }
          break;
        }
      break;

    default:
      break; // Do nothing for all the others
    }
}


void FdDB::objectCreator(int mode)
{
  switch (mode)
    {
    case FuiModes::MAKETRIAD_MODE:
      FpPM::vpmSetUndoPoint("Triad");
      if (FdDB::firstObjectToCreateNear && FdDB::firstObjectToCreateNear->isOfType(FdPart::getClassTypeID()))
        Fedem::createTriad(FdPickedPoints::getFirstPickedPoint(),
                           FdDB::firstObjectToCreateNear->getFmOwner());
      else
        Fedem::createTriad(FdPickedPoints::getFirstPickedPoint(),
                           dynamic_cast<FmSubAssembly*>(FapEventManager::getFirstPermSelectedObject()));
      break;
    case FuiModes::MAKEFORCE_MODE:
      FpPM::vpmSetUndoPoint("Force");
      Fedem::createLoad(FmLoad::FORCE,FdPickedPoints::getFirstPickedPoint(), FdDB::firstCreateDirection,
                        FdDB::firstObjectToCreateNear ? FdDB::firstObjectToCreateNear->getFmOwner() : NULL);
      break;
    case FuiModes::MAKETORQUE_MODE:
      FpPM::vpmSetUndoPoint("Torque");
      Fedem::createLoad(FmLoad::TORQUE,FdPickedPoints::getFirstPickedPoint(), FdDB::firstCreateDirection,
                        FdDB::firstObjectToCreateNear ? FdDB::firstObjectToCreateNear->getFmOwner() : NULL);
      break;
    case FuiModes::MAKESTICKER_MODE:
      FpPM::vpmSetUndoPoint("Sticker");
      Fedem::createSticker(FdPickedPoints::getFirstPickedPoint(),FdDB::firstObjectToCreateNear->getFmOwner());
      break;
    case FuiModes::MAKETIRE_MODE:
      FpPM::vpmSetUndoPoint("Tire");
      Fedem::createTire(dynamic_cast<FmRevJoint*>(FapEventManager::getPermSelectedObject(0)));
      break;
    case FuiModes::MAKESIMPLESENSOR_MODE:
      FpPM::vpmSetUndoPoint("Simple sensor");
      Fedem::createSensor(dynamic_cast<FmIsMeasuredBase*>(FapEventManager::getPermSelectedObject(0)));
      break;
    case FuiModes::MAKERELATIVESENSOR_MODE:
      FpPM::vpmSetUndoPoint("Relative sensor");
      Fedem::createSensor(dynamic_cast<FmIsMeasuredBase*>(FapEventManager::getPermSelectedObject(0)),
                          dynamic_cast<FmIsMeasuredBase*>(FapEventManager::getPermSelectedObject(1)));
      break;
    case FuiModes::MAKEDAMPER_MODE:
      FpPM::vpmSetUndoPoint("Damper");
      Fedem::createAxialDamper(FdPickedPoints::getFirstPickedPoint(),
                               FdPickedPoints::getSecondPickedPoint(),
                               FdDB::firstObjectToCreateNear ? FdDB::firstObjectToCreateNear->getFmOwner() : NULL,
                               FdDB::secondObjectToCreateNear ? FdDB::secondObjectToCreateNear->getFmOwner() : NULL);
      break;
    case FuiModes::MAKEREVJOINT_MODE:
      FpPM::vpmSetUndoPoint("Revolute joint");
      Fedem::createRevJoint(FdPickedPoints::getFirstPickedPoint(),FdDB::firstCreateDirection);
      break;
    case FuiModes::MAKEBALLJOINT_MODE:
      FpPM::vpmSetUndoPoint("Ball joint");
      Fedem::createBallJoint(FdPickedPoints::getFirstPickedPoint());
      break;
    case FuiModes::MAKERIGIDJOINT_MODE:
      FpPM::vpmSetUndoPoint("Rigid joint");
      Fedem::createRigidJoint(FdPickedPoints::getFirstPickedPoint());
      break;
    case FuiModes::MAKEFREEJOINT_MODE:
      FpPM::vpmSetUndoPoint("Free joint");
      Fedem::createFreeJoint(FdPickedPoints::getFirstPickedPoint(),
                             FdPickedPoints::getSecondPickedPoint(),
                             FdDB::firstCreateDirDefined ? &FdDB::firstCreateDirection : NULL);
      break;
    case FuiModes::MAKEFREEJOINTBETWEENTRIADS_MODE:
      FpPM::vpmSetUndoPoint("Free joint");
      Fedem::createJoint(FmFreeJoint::getClassTypeID(),
                         FdDB::firstPickedFmObject,FdDB::secondPickedFmObject,
                         &FdPickedPoints::getFirstPickedPoint());
      break;
    case FuiModes::MAKECYLJOINT_MODE:
      FpPM::vpmSetUndoPoint("Cylindric joint");
      Fedem::createJoint(FmCylJoint::getClassTypeID(),
                         FdPickedPoints::getFirstPickedPoint(),
                         FdPickedPoints::getSecondPickedPoint(),
                         FdDB::firstCreateDirection);
      break;
    case FuiModes::MAKECYLJOINTBETWEENTRIADS_MODE:
      FpPM::vpmSetUndoPoint("Cylindric joint");
      Fedem::createJoint(FmCylJoint::getClassTypeID(),
                         FdDB::firstPickedFmObject,FdDB::secondPickedFmObject,
                         FdDB::firstCreateDirection,FdDB::thirdPickedFmObject);
      break;
    case FuiModes::MAKEPRISMJOINT_MODE:
      FpPM::vpmSetUndoPoint("Prismatic joint");
      Fedem::createJoint(FmPrismJoint::getClassTypeID(),
                         FdPickedPoints::getFirstPickedPoint(),
                         FdPickedPoints::getSecondPickedPoint(),
                         FdDB::firstCreateDirection);
      break;
    case FuiModes::MAKEPRISMJOINTBETWEENTRIADS_MODE:
      FpPM::vpmSetUndoPoint("Prismatic joint");
      Fedem::createJoint(FmPrismJoint::getClassTypeID(),
                         FdDB::firstPickedFmObject,FdDB::secondPickedFmObject,
                         FdDB::firstCreateDirection,FdDB::thirdPickedFmObject);
      break;
    case FuiModes::MAKESPRING_MODE:
      FpPM::vpmSetUndoPoint("Spring");
      Fedem::createAxialSpring(FdPickedPoints::getFirstPickedPoint(),
                               FdPickedPoints::getSecondPickedPoint(),
                               FdDB::firstObjectToCreateNear ? FdDB::firstObjectToCreateNear->getFmOwner() : NULL,
                               FdDB::secondObjectToCreateNear ? FdDB::secondObjectToCreateNear->getFmOwner() : NULL);
      break;
    case FuiModes::MAKEGEAR_MODE:
      if (FdDB::firstObjectToCreateNear && FdDB::secondObjectToCreateNear)
      {
        FpPM::vpmSetUndoPoint("Gear");
        Fedem::createGear(FdDB::firstObjectToCreateNear->getFmOwner(),
                          FdDB::secondObjectToCreateNear->getFmOwner());
      }
      break;
    case FuiModes::MAKERACKPIN_MODE:
      if (FdDB::firstObjectToCreateNear && FdDB::secondObjectToCreateNear)
      {
        FpPM::vpmSetUndoPoint("Rack-and-pinion");
        Fedem::createRackPinion(FdDB::firstObjectToCreateNear->getFmOwner(),
                                FdDB::secondObjectToCreateNear->getFmOwner());
      }
      break;
    default:
      break;
    }

  FapEventManager::permUnselectAll();
}


//
//    Event Callback methods
//
//
///////////////////////////////////////////////////////////

void FdDB::appearanceEventCB(void*, SoEventCallback* eventCBnode)
{
  const SoEvent* event = eventCBnode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  if (SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    SoHandleEventAction* evHaAction = eventCBnode->getAction();

    std::vector<FdObject*> selectedObjects;
    FdSelector::getSelectedObjects(selectedObjects);

    // Build array of interesting types
    std::vector<int> types = {
      FdLink::getClassTypeID(),
      FdRefPlane::getClassTypeID(),
      FdSeaState::getClassTypeID()
    };

    long int  indexToInterestingPP    = -1;
    bool      wasASelectedObjInPPList = false;
    FdObject* pickedObject = FdPickFilter::getInterestingPObj(&evHaAction->getPickedPointList(),
                                                              selectedObjects, // An array of FdObject*'s  that is selected
                                                              types,true, // Filter variables
                                                              indexToInterestingPP,wasASelectedObjInPPList); // Variables returning values
    if (!pickedObject)
      FuiModes::setState(0);
    else
    {
      FapEventManager::permUnselectAll();
      FapEventManager::permSelect(pickedObject->getFmOwner());
      FuiModes::setState(1);
    }
  }

  eventCBnode->setHandled();
}


void FdDB::onePickCreateEventCB(void*, SoEventCallback* eventCBnode)
{
  const SoEvent* event = eventCBnode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  if (SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    SoHandleEventAction* evHaAction = eventCBnode->getAction();

    std::vector<FdObject*> selectedObjects;
    FdSelector::getSelectedObjects(selectedObjects);

    // Build array of interesting or non-interesting types
    int mode = FuiModes::getMode();
    bool typesIsInteresting = true;
    std::vector<int> types;
    switch (mode) {
    case FuiModes::MAKETRIAD_MODE:
      types = {
        FdLink::getClassTypeID(),
        FdRefPlane::getClassTypeID()
      };
      break;
    case FuiModes::MAKEFORCE_MODE:
    case FuiModes::MAKETORQUE_MODE:
      types = {
        FdLink::getClassTypeID(),
        FdRefPlane::getClassTypeID(),
        FdTriad::getClassTypeID()
      };
      break;
    case FuiModes::MAKESTICKER_MODE:
      // Non-interesting types
      types = {
        FdAxialSprDa::getClassTypeID(),
        FdTire::getClassTypeID(),
        FdHP::getClassTypeID(),
        FdSensor::getClassTypeID()
      };
    default:
      typesIsInteresting = false;
    }

    long int  indexToInterestingPP    = -1;
    bool      wasASelectedObjInPPList = false;
    FdObject* pickedObject = FdPickFilter::getInterestingPObj(&evHaAction->getPickedPointList(),
                                                              selectedObjects, // This is to select objects behind the already selected one
                                                              types,typesIsInteresting, // Filter variables
                                                              indexToInterestingPP,wasASelectedObjInPPList); // Variables returning values
    if (!pickedObject)
    {
      // The user picked nothing.
      int state = FuiModes::getState();
      if (state == 0 || state == 1)
        FuiModes::setState(0);
    }
    else
    {
      // The user picked something
      FapEventManager::permUnselectAll();

      // Select path to object
      SoPickedPoint* interestingPickedPoint = evHaAction->getPickedPointList()[indexToInterestingPP];
      SoPath* path = interestingPickedPoint->getPath();

      // Get a copy of the pick detail in the shape picked:
      SoNode* tail = ((SoFullPath*)path)->getTail();
      const SoDetail* pDet = interestingPickedPoint->getDetail(tail);
      SoDetail* pickDetail = pDet ? pDet->copy() : NULL;

      // Get hit point on object in obj. space:
      SbVec3f pointOnObject = interestingPickedPoint->getObjectPoint();
      SbVec3f worldPoint    = interestingPickedPoint->getPoint();

      // Get object to world transformation:
      const SbMatrix& SbObjToWorld = interestingPickedPoint->getObjectToWorld(tail);
      FaMat34 objToWorld = FdConverter::toFaMat34(SbObjToWorld);

      FdDB::firstObjectToCreateNear = pickedObject;

      bool makingForce = mode == FuiModes::MAKEFORCE_MODE || mode == FuiModes::MAKETORQUE_MODE;
      bool showDirection = makingForce || mode == FuiModes::MAKECAMJOINT_MODE || mode == FuiModes::MAKEREVJOINT_MODE;

      if (FdDB::firstObjectToCreateNear)
      {
        FapEventManager::permSelect(pickedObject->getFmOwner());
        FaVec3 createPoint;
        if (makingForce && pickedObject->isOfType(FdTriad::getClassTypeID()))
          createPoint = static_cast<FmTriad*>(pickedObject->getFmOwner())->getGlobalTranslation();
        else
          createPoint = FdDB::firstObjectToCreateNear->findSnapPoint(pointOnObject,SbObjToWorld,pickDetail,interestingPickedPoint);

        FdPickedPoints::setFirstPP(createPoint,objToWorld);

        if (pickDetail && pickDetail->isOfType(SoLineDetail::getClassTypeId()))
          FdDB::firstCreateDirection = FdDB::getLineDir(path, pickDetail, pickedObject, SbObjToWorld);
        else
          FdDB::firstCreateDirection = -FdConverter::toFaVec3(interestingPickedPoint->getNormal()); // Need getNormal method in FdObjects.

        bool isEdge = tail->isOfType(FdCadEdge::getClassTypeId());
        if (isEdge || tail->isOfType(FdCadFace::getClassTypeId()))
        {
          FdCadEntityInfo* info = isEdge ? static_cast<FdCadEdge*>(tail)->getGeometryInfo() : static_cast<FdCadFace*>(tail)->getGeometryInfo();
          if (info)
            FdDB::showPointSelectionUI(isEdge,info->type,
                                       FdConverter::toFaVec3(worldPoint), createPoint,
                                       FdConverter::toFaVec3(interestingPickedPoint->getNormal()),
                                       objToWorld.direction()*info->axis, info->myAxisIsValid,
                                       objToWorld*info->origin, info->myOriginIsValid, showDirection);
          else
            FdDB::hidePointSelectionUI();
        }
        else if (showDirection)
          FdDB::showDirFlipUI(FdConverter::toFaVec3(worldPoint));
        else
          FdDB::hidePointSelectionUI();

        FuiModes::setState(1);
      }

      if (showDirection)
        FdExtraGraphics::showDirection(FdPickedPoints::getFirstPickedPoint(), FdDB::firstCreateDirection);

      if (pickDetail)
        delete pickDetail;
    }
  }

  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCBnode->setHandled();
}


FaVec3 FdDB::getLineDir(SoPath* path, SoDetail* pickDetail, FdObject* pickedObject, const SbMatrix& SbObjToWorld)
{
  int p0Idx = ((SoLineDetail*)pickDetail)->getPoint0()->getCoordinateIndex();
  int p1Idx = ((SoLineDetail*)pickDetail)->getPoint1()->getCoordinateIndex();

  SbVec3f p0(0,0,0), p1(0,0,0);
  if (pickedObject->getKit()->isOfType(FdFEModelKit::getClassTypeId()))
  {
    FdFEModelKit* pickedFEmodel = static_cast<FdFEModelKit*>(pickedObject->getKit());
    if (pickedFEmodel->hasVertexes())
    {
      p0 = FdConverter::toSbVec3f(pickedFEmodel->getVertex(p0Idx));
      p1 = FdConverter::toSbVec3f(pickedFEmodel->getVertex(p1Idx));
    }
  }
  else if (path)
  {
    SoMFVec3f* points = FdPickFilter::findLastVertexes(path);
    if (points && p0Idx < points->getNum() && p1Idx < points->getNum())
    {
      p0 = (*points)[p0Idx];
      p1 = (*points)[p1Idx];
    }
  }

  if (p0 == p1)
    return FaVec3(1.0,0.0,0.0);

  SbVec3f wDir;
  SbObjToWorld.multVecMatrix(p0-p1,wDir);
  return FdConverter::toFaVec3(wDir);
}


void FdDB::twoPickCreateEventCB(void*, SoEventCallback* eventCBnode)
{
  int mode  = FuiModes::getMode();
  int state = FuiModes::getState();

  const SoEvent* event = eventCBnode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  if (SoMouseButtonEvent::isButtonPressEvent(event, SoMouseButtonEvent::BUTTON1))
  {
    SoHandleEventAction* evHaAction = eventCBnode->getAction();

    // Build array of interesting types
    bool typesIsInteresting = true;
    std::vector<int> types;
    switch (mode) {
    case FuiModes::MAKESPRING_MODE:
    case FuiModes::MAKEDAMPER_MODE:
      types = {
	FdLink::getClassTypeID(),
	FdRefPlane::getClassTypeID(),
	FdTriad::getClassTypeID(),
	FdSimpleJoint::getClassTypeID()
      };
      break;
    case FuiModes::MAKEFREEJOINTBETWEENTRIADS_MODE:
      if (state >= 4 && state <= 6) //create slave
        types = { FdTriad::getClassTypeID() };
      else // create master
        types = { FdTriad::getClassTypeID(), FdRefPlane::getClassTypeID() };
      break;
    default:
      typesIsInteresting = false;
    }

    long int  indexToInterestingPP = -1;
    FdObject* pickedObject = FdPickFilter::getCyceledInterestingPObj(&evHaAction->getPickedPointList(),
                                                                     types,typesIsInteresting, // Filter variables
                                                                     indexToInterestingPP); // Variables returning values

    if (!pickedObject)
    {
      // The user picked nothing.
      if (mode == FuiModes::MAKEFREEJOINTBETWEENTRIADS_MODE)
      {
        if (state == 0 || state == 1 || state == 2 || state == 3)
          FuiModes::setState(0);
        else if (state == 4 || state == 5 || state == 6 || state == 8)
          FuiModes::setState(4);
      }
      else
      {
        if (state == 0)
          FuiModes::setState(0);
        else if (state == 1)
          FuiModes::setState(0);
        else if (state == 2)
          FuiModes::setState(2);
        else if (state == 3)
          FuiModes::setState(2);
      }
    }
    else
    {
      if (state == 1)
        FapEventManager::permUnselectAll();

      bool showDirection = ((mode == FuiModes::MAKEFREEJOINT_MODE || mode == FuiModes::MAKEFREEJOINTBETWEENTRIADS_MODE) && (state == 0 || state == 1));

      // Select path to object

      SoPickedPoint* interestingPickedPoint = evHaAction->getPickedPointList()[indexToInterestingPP];
      SoPath* path = interestingPickedPoint->getPath();

      // Get a copy of the pick detail in the shape picked:
      SoNode* tail = ((SoFullPath*)path)->getTail();
      const SoDetail* pDet = interestingPickedPoint->getDetail(tail);
      SoDetail* pickDetail = pDet ? pDet->copy() : NULL;

      // Get hit point on object in obj. space:
      SbVec3f pointOnObject = interestingPickedPoint->getObjectPoint();
      SbVec3f worldPoint    = interestingPickedPoint->getPoint();

      // Get object to world transformation:
      const SbMatrix& SbObjToWorld = interestingPickedPoint->getObjectToWorld(tail);
      FaMat34 objToWorld = FdConverter::toFaMat34(SbObjToWorld);

      if (mode == FuiModes::MAKEPRISMJOINT_MODE || mode == FuiModes::MAKECYLJOINT_MODE)
        path = NULL; // For multi-master joints, use line-direction only when picking on FE parts

      // If state is for the first picked object
      if ((mode == FuiModes::MAKEFREEJOINTBETWEENTRIADS_MODE && state >= 2 && state <= 3) || state == 0 || state == 1)
      {
        FapEventManager::permSelect(pickedObject->getFmOwner(), 0);
        FdDB::firstObjectToCreateNear = pickedObject;
        if (FdDB::firstObjectToCreateNear)
        {
          FaVec3 createPoint = FdDB::firstObjectToCreateNear->findSnapPoint(pointOnObject,SbObjToWorld,pickDetail,interestingPickedPoint);
          FdPickedPoints::setFirstPP(createPoint,objToWorld);
          if (pickDetail && pickDetail->isOfType(SoLineDetail::getClassTypeId()))
            FdDB::firstCreateDirection = FdDB::getLineDir(path, pickDetail, pickedObject, SbObjToWorld);
          else
          {
            FdDB::firstCreateDirection = FdConverter::toFaVec3(interestingPickedPoint->getNormal()); // Need getNormal method in FdObjects.
            static bool toggleDir = true;
            if (toggleDir)
              FdDB::firstCreateDirection = -FdDB::firstCreateDirection;
            toggleDir = !toggleDir;
          }

          bool isEdge = tail->isOfType(FdCadEdge::getClassTypeId());
          FdCadEntityInfo* info = NULL;
          if (isEdge)
            info = static_cast<FdCadEdge*>(tail)->getGeometryInfo();
          else if (tail->isOfType(FdCadFace::getClassTypeId()))
            info = static_cast<FdCadFace*>(tail)->getGeometryInfo();

          if (info) {
            FdDB::showPointSelectionUI(isEdge, info->type,
                                       FdConverter::toFaVec3(worldPoint), createPoint,
                                       FdConverter::toFaVec3(interestingPickedPoint->getNormal()),
                                       objToWorld.direction()*info->axis, info->myAxisIsValid,
                                       objToWorld*info->origin, info->myOriginIsValid, showDirection);
            FdDB::firstCreateDirDefined = info->myAxisIsValid;
          }
          else {
            FdDB::hidePointSelectionUI();
            FdDB::firstCreateDirDefined = false;
          }

          if (mode == FuiModes::MAKEFREEJOINTBETWEENTRIADS_MODE)
          {
            FdDB::firstPickedFmObject = pickedObject->getFmOwner();
            if (dynamic_cast<FmTriad*>(FdDB::firstPickedFmObject))
              FuiModes::setState(1);
            else if (dynamic_cast<FmRefPlane*>(FdDB::firstPickedFmObject))
              FuiModes::setState(2);
            else
              FuiModes::setState(3);
          }
          else
            FuiModes::setState(1);
        }
      }

      // else if state is for the second picked object
      else if ((mode == FuiModes::MAKEFREEJOINTBETWEENTRIADS_MODE && state >= 4 && state <= 8) || state == 2 || state == 3)
      {
        FapEventManager::permSelect(pickedObject->getFmOwner(), 1);
        FdDB::secondObjectToCreateNear = pickedObject;
        if (FdDB::secondObjectToCreateNear)
        {
          FaVec3 createPoint = FdDB::secondObjectToCreateNear->findSnapPoint(pointOnObject,SbObjToWorld,pickDetail,interestingPickedPoint);
          FdPickedPoints::setSecondPP(createPoint,objToWorld);
          if (pickDetail && pickDetail->isOfType(SoLineDetail::getClassTypeId()))
            FdDB::secondCreateDirection = FdDB::getLineDir(path, pickDetail, pickedObject, SbObjToWorld);
          else
          {
            FdDB::secondCreateDirection = FdConverter::toFaVec3(interestingPickedPoint->getNormal()); // Need getNormal method in FdObjects.
            static bool toggleDir = true;
            if (toggleDir)
              FdDB::secondCreateDirection = -FdDB::secondCreateDirection;
            toggleDir = !toggleDir;
          }

          bool isEdge = tail->isOfType(FdCadEdge::getClassTypeId());
          FdCadEntityInfo* info = NULL;
          if (isEdge)
            info = static_cast<FdCadEdge*>(tail)->getGeometryInfo();
          else if (tail->isOfType(FdCadFace::getClassTypeId()))
            info = static_cast<FdCadFace*>(tail)->getGeometryInfo();

          if (info) {
            FdDB::showPointSelectionUI(isEdge, info->type,
                                       FdConverter::toFaVec3(worldPoint), createPoint,
                                       FdConverter::toFaVec3(interestingPickedPoint->getNormal()),
                                       objToWorld.direction()*info->axis, info->myAxisIsValid,
                                       objToWorld*info->origin, info->myOriginIsValid, showDirection, 1);
            FdDB::secondCreateDirDefined = info->myAxisIsValid;
          }
          else {
            FdDB::hidePointSelectionUI();
            FdDB::secondCreateDirDefined = false;
          }

          if (mode == FuiModes::MAKEFREEJOINTBETWEENTRIADS_MODE)
          {
            FdDB::secondPickedFmObject = pickedObject->getFmOwner();
            FmTriad* selectedTriad = dynamic_cast<FmTriad*>(FdDB::secondPickedFmObject);
            if (!selectedTriad)
              FuiModes::setState(7);
            else if (selectedTriad->getJointWhereSlave() != NULL)
              FuiModes::setState(6);
            else if (selectedTriad->isAttached(FmDB::getEarthLink()))
              FuiModes::setState(8);
            else
              FuiModes::setState(5);
          }
          else
            FuiModes::setState(3);
        }
      }

      if (pickDetail)
        delete pickDetail;
    }
  }

  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCBnode->setHandled();
}


void FdDB::threePickCreateEventCB(void*, SoEventCallback* eventCBnode)
{
  const SoEvent* event = eventCBnode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  if (SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    int state = FuiModes::getState();

    SoHandleEventAction* evHaAction = eventCBnode->getAction();

    long int  indexToInterestingPP = -1;
    FdObject* pickedObject = FdPickFilter::getCyceledInterestingPObj(&evHaAction->getPickedPointList(),
                                                                     {FdTriad::getClassTypeID()} ,true, // Filter variables
                                                                     indexToInterestingPP); // Variables returning values

    if (!pickedObject)
    {
      // The user picked nothing
      if (state >= 0 && state <= 2)
        FuiModes::setState(0);
      else if (state >= 3 && state <= 5)
        FuiModes::setState(3);
      else if (state >= 6 && state <= 9)
        FuiModes::setState(6);
    }
    else
    {
      if (state == 1)
        FapEventManager::permUnselectAll();

      bool showDirection = false;

      // Select path to object
      SoPickedPoint* interestingPickedPoint = evHaAction->getPickedPointList()[indexToInterestingPP];
      SoPath* path = interestingPickedPoint->getPath();

      // Get a copy of the pick detail in the shape picked:
      SoNode* tail = ((SoFullPath*)path)->getTail();
      const SoDetail* pDet = interestingPickedPoint->getDetail(tail);
      SoDetail* pickDetail = pDet ? pDet->copy() : NULL;

      // Get hit point on object in obj. space:
      SbVec3f pointOnObject = interestingPickedPoint->getObjectPoint();
      SbVec3f worldPoint    = interestingPickedPoint->getPoint();

      // Get object to world transformation:
      const SbMatrix& SbObjToWorld = interestingPickedPoint->getObjectToWorld(tail);
      FaMat34 objToWorld = FdConverter::toFaMat34(SbObjToWorld);

      std::cout <<"You picked "<< pickedObject->getFmOwner()->getIdString(true) << std::endl;
      if (state >= 0 && state <= 2) // state is for first picked object
      {
        FapEventManager::permSelect(pickedObject->getFmOwner(), 0);
        FdDB::firstObjectToCreateNear = pickedObject;
        if (FdDB::firstObjectToCreateNear)
        {
          FaVec3 createPoint = FdDB::firstObjectToCreateNear->findSnapPoint(pointOnObject,SbObjToWorld,pickDetail,interestingPickedPoint);
          FdPickedPoints::setFirstPP(createPoint,objToWorld);
          if (pickDetail && pickDetail->isOfType(SoLineDetail::getClassTypeId()))
            FdDB::firstCreateDirection = FdDB::getLineDir(path, pickDetail, pickedObject, SbObjToWorld);
          else
          {
            FdDB::firstCreateDirection = -(FdConverter::toFaVec3(interestingPickedPoint->getNormal())); // Need getNormal method in FdObjects.
            static bool toggleDir = false;
            if (toggleDir)
              FdDB::firstCreateDirection = -FdDB::firstCreateDirection;
            toggleDir = !toggleDir;
          }
          FdDB::firstCreateDirDefined = false;

          bool isEdge = tail->isOfType(FdCadEdge::getClassTypeId());
          FdCadEntityInfo* info = NULL;
          if (isEdge)
            info = static_cast<FdCadEdge*>(tail)->getGeometryInfo();
          else if (tail->isOfType(FdCadFace::getClassTypeId()))
            info = static_cast<FdCadFace*>(tail)->getGeometryInfo();

          if (info)
          {
            FdDB::showPointSelectionUI(isEdge, info->type,
                                       FdConverter::toFaVec3(worldPoint), createPoint,
                                       FdConverter::toFaVec3(interestingPickedPoint->getNormal()),
                                       objToWorld.direction()*info->axis, info->myAxisIsValid,
                                       objToWorld*info->origin, info->myOriginIsValid, showDirection);
            FdDB::firstCreateDirDefined = info->myAxisIsValid;
          }
          else
            FdDB::hidePointSelectionUI();

          FdDB::firstPickedFmObject = pickedObject->getFmOwner();
          FmTriad* selectedTriad = dynamic_cast<FmTriad*>(FdDB::firstPickedFmObject);
          if (selectedTriad)
            FuiModes::setState(selectedTriad->getJointWhereSlave() ? 2 : 1);
        }
      }

      else if (state >= 3 && state <= 5) // state is for second picked object
      {
        FapEventManager::permSelect(pickedObject->getFmOwner(), 1);
        FdDB::secondObjectToCreateNear = pickedObject;
        if (FdDB::secondObjectToCreateNear)
        {
          FaVec3 createPoint = FdDB::secondObjectToCreateNear->findSnapPoint(pointOnObject,SbObjToWorld,pickDetail,interestingPickedPoint);
          FdPickedPoints::setSecondPP(createPoint,objToWorld);
          if (pickDetail && pickDetail->isOfType(SoLineDetail::getClassTypeId()))
            FdDB::secondCreateDirection = FdDB::getLineDir(path, pickDetail, pickedObject, SbObjToWorld);
          else
          {
            FdDB::secondCreateDirection = -(FdConverter::toFaVec3(interestingPickedPoint->getNormal())); // Need getNormal method in FdObjects.
            static bool toggleDir = false;
            if (toggleDir)
              FdDB::secondCreateDirection = -FdDB::secondCreateDirection;
            toggleDir = !toggleDir;
          }
          FdDB::secondCreateDirDefined = false;

          bool isEdge = tail->isOfType(FdCadEdge::getClassTypeId());
          FdCadEntityInfo* info = NULL;
          if (isEdge)
            info = static_cast<FdCadEdge*>(tail)->getGeometryInfo();
          else if (tail->isOfType(FdCadFace::getClassTypeId()))
            info = static_cast<FdCadFace*>(tail)->getGeometryInfo();

          if (info)
          {
            FdDB::showPointSelectionUI(isEdge, info->type,
                                       FdConverter::toFaVec3(worldPoint), createPoint,
                                       FdConverter::toFaVec3(interestingPickedPoint->getNormal()),
                                       objToWorld.direction()*info->axis, info->myAxisIsValid,
                                       objToWorld*info->origin, info->myOriginIsValid, showDirection, 1);
            FdDB::secondCreateDirDefined = info->myAxisIsValid;
          }
          else
            FdDB::hidePointSelectionUI();

          FdDB::secondPickedFmObject = pickedObject->getFmOwner();
          FmTriad* selectedTriad = dynamic_cast<FmTriad*>(FdDB::secondPickedFmObject);
          if (selectedTriad)
            FuiModes::setState(selectedTriad->getJointWhereSlave() ? 5 : 4);
        }
      }

      else if (state >= 6 && state <= 9) // state is for third picked object
      {
        FapEventManager::permSelect(pickedObject->getFmOwner(), 1);
        FdDB::thirdObjectToCreateNear = pickedObject;
        if (FdDB::thirdObjectToCreateNear)
        {
          FaVec3 createPoint = FdDB::thirdObjectToCreateNear->findSnapPoint(pointOnObject,SbObjToWorld,pickDetail,interestingPickedPoint);
          FdPickedPoints::setThirdPP(createPoint,objToWorld);
          if (pickDetail && pickDetail->isOfType(SoLineDetail::getClassTypeId()))
            FdDB::thirdCreateDirection = FdDB::getLineDir(path, pickDetail, pickedObject, SbObjToWorld);
          else
          {
            FdDB::thirdCreateDirection = -(FdConverter::toFaVec3(interestingPickedPoint->getNormal())); // Need getNormal method in FdObjects.
            static bool toggleDir = false;
            if (toggleDir)
              FdDB::thirdCreateDirection = -FdDB::thirdCreateDirection;
            toggleDir = !toggleDir;
          }
          FdDB::thirdCreateDirDefined = false;

          bool isEdge = tail->isOfType(FdCadEdge::getClassTypeId());
          FdCadEntityInfo* info = NULL;
          if (isEdge)
            info = static_cast<FdCadEdge*>(tail)->getGeometryInfo();
          else if (tail->isOfType(FdCadFace::getClassTypeId()))
            info = static_cast<FdCadFace*>(tail)->getGeometryInfo();

          if (info)
          {
            FdDB::showPointSelectionUI(isEdge, info->type,
                                       FdConverter::toFaVec3(worldPoint), createPoint,
                                       FdConverter::toFaVec3(interestingPickedPoint->getNormal()),
                                       objToWorld.direction()*info->axis, info->myAxisIsValid,
                                       objToWorld*info->origin, info->myOriginIsValid, showDirection, 1);
            FdDB::thirdCreateDirDefined = info->myAxisIsValid;
          }
          else
            FdDB::hidePointSelectionUI();

          FdDB::thirdPickedFmObject = pickedObject->getFmOwner();

          FmTriad* selectedTriad = dynamic_cast<FmTriad*>(FdDB::thirdPickedFmObject);
          if (selectedTriad)
          {
            FuiModes::setState(selectedTriad->getJointWhereSlave() ? 8 : 7);
            FmTriad* firstTriad = dynamic_cast<FmTriad*>(FdDB::firstPickedFmObject);
            FmTriad* secondTriad = dynamic_cast<FmTriad*>(FdDB::secondPickedFmObject);
            if (firstTriad && secondTriad)
            {
              FaVec3 a  = firstTriad->getGlobalTranslation();
              FaVec3 ba = secondTriad->getGlobalTranslation() - a;
              FaVec3 ca = selectedTriad->getGlobalTranslation() - a;

              if ((ba^ca).length() > FmDB::getPositionTolerance())
                FuiModes::setState(9);

              double dotProd = ba*ca;
              if (dotProd < 0.0)
                FuiModes::setState(9);
              else if (dotProd > ba.sqrLength())
                FuiModes::setState(9);
            }
          }
        }
      }

      if (pickDetail)
        delete pickDetail;
    }
  }

  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCBnode->setHandled();
}


void FdDB::makeCamJointEventCB(void*, SoEventCallback* eventCBnode)
{
  const SoEvent* event = eventCBnode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  if (SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    int state = FuiModes::getState();
    SoHandleEventAction* evHaAction = eventCBnode->getAction();

    std::vector<FdObject*> selectedObjects;
    FdSelector::getSelectedObjects(selectedObjects);

    long int  indexToInterestingPP    = -1;
    bool      wasASelectedObjInPPList = false;
    FdObject* pickedObject = FdPickFilter::getInterestingPObj(&evHaAction->getPickedPointList(),
                                                              selectedObjects, // This is to select objects behind the already selected one
                                                              {},false, // No variables filtering
                                                              indexToInterestingPP,wasASelectedObjInPPList); // Variables returning values
    if (pickedObject) // The user picked something
    {
      FapEventManager::permUnselectAll();
      FdDB::firstObjectToCreateNear = pickedObject;
      FapEventManager::permSelect(pickedObject->getFmOwner());

      bool allowCompleteCurveSelection = FdDB::ourAllowCompleteCamCurveSelection && pickedObject->isOfType(FdCamJoint::getClassTypeID());
      if ( allowCompleteCurveSelection && (state == 2 || state == 3 || state == 5) )
      {
        FdPickedPoints::resetPPs();
        FdExtraGraphics::hideDirection();
      }
      else
      {
        // Select path to object:
        SoPickedPoint* interestingPickedPoint = evHaAction->getPickedPointList()[indexToInterestingPP];
        SoPath* path = interestingPickedPoint->getPath();

        // Get a copy of the pick detail in the shape picked:
        SoNode* tail = ((SoFullPath*)path)->getTail();
        const SoDetail* pDet = interestingPickedPoint->getDetail(tail);
        SoDetail* pickDetail = pDet ? pDet->copy() : NULL;

        // Get object to world transformation:
        const SbMatrix& objToWorld = interestingPickedPoint->getObjectToWorld(tail);

        // Get hit point on object in object space:
        SbVec3f pointOnObject = interestingPickedPoint->getObjectPoint();
        FaVec3  createPoint   = FdDB::firstObjectToCreateNear->findSnapPoint(pointOnObject,objToWorld,pickDetail,interestingPickedPoint);
        FdPickedPoints::setFirstPP(createPoint,FdConverter::toFaMat34(objToWorld));

        if (pickDetail && pickDetail->isOfType(SoLineDetail::getClassTypeId()))
          FdDB::firstCreateDirection = FdDB::getLineDir(path,pickDetail,pickedObject,objToWorld);
        else
          FdDB::firstCreateDirection = -FdConverter::toFaVec3(interestingPickedPoint->getNormal()); // Need getNormal method in FdObjects.

        FdExtraGraphics::showDirection(FdPickedPoints::getFirstPickedPoint(),FdDB::firstCreateDirection);

        if (pickDetail)
          delete pickDetail;
      }

      switch (state) {
      case 0:
      case 1:
        FuiModes::setState(1);
        break;
      case 2:
      case 3:
        if (allowCompleteCurveSelection)
          FuiModes::setState(5);
        else
          FuiModes::setState(3);
        break;
      case 5:
        if (pickedObject->isOfType(FdCamJoint::getClassTypeID()))
          FuiModes::setState(5);
        else
          FuiModes::setState(3);
        break;
      }
    }
    else // The user didn't pick anything
      switch (state) {
      case 0:
      case 1:
        FuiModes::setState(0);
        break;
      case 2:
      case 3:
      case 5:
        FuiModes::setState(2);
        break;
      }
  }
  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCBnode->setHandled();
}


void FdDB::createHPEventCB(void*, SoEventCallback* eventCBnode)
{
  const SoEvent* event = eventCBnode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  if (SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    int mode  = FuiModes::getMode();
    int state = FuiModes::getState();

    std::vector<FdObject*> selectedObjects;
    FdSelector::getSelectedObjects(selectedObjects);

    // Build Array of Interesting or nonInteresting types:
    bool typesIsInteresting = true;
    std::vector<int> types;
    if (mode == FuiModes::MAKEGEAR_MODE)
      types = { FdRevJoint::getClassTypeID() };
    else if (mode == FuiModes::MAKERACKPIN_MODE)
    {
      if (state == 0 || state == 1)
        types = { FdRevJoint::getClassTypeID() };
      else if (state == 2 || state == 3)
        types = { FdPrismJoint::getClassTypeID() };
    }
    else
      typesIsInteresting = false;

    long int  indexToInterestingPP    = -1;
    bool      wasASelectedObjInPPList = false;
    FdObject* pickedObject = FdPickFilter::getInterestingPObj(&eventCBnode->getAction()->getPickedPointList(),
                                                              selectedObjects, // This is to select objects behind the already selected one
                                                              types,typesIsInteresting, // Filter variables
                                                              indexToInterestingPP,wasASelectedObjInPPList); // Variables returning values
    if (!pickedObject)
    {
      // The user picked nothing.
      if (state == 0 || state == 1)
        FuiModes::setState(0);
      else if (state == 2 || state == 3)
        FuiModes::setState(2);
    }
    else
    {
      if (state == 1)
        FapEventManager::permUnselectAll();

      if (state == 0 || state == 1)
      {
        FapEventManager::permSelect(pickedObject->getFmOwner(),0);
        FdDB::firstObjectToCreateNear = pickedObject;
        if (FdDB::firstObjectToCreateNear)
          FuiModes::setState(1);
      }
      else if (state == 2 || state == 3)
      {
        FapEventManager::permSelect(pickedObject->getFmOwner(),1);
        FdDB::secondObjectToCreateNear = pickedObject;
        if (FdDB::secondObjectToCreateNear)
          FuiModes::setState(3);
      }
    }
  }

  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCBnode->setHandled();
}


void FdDB::smartMoveEventCB(void*, SoEventCallback* eventCBnode)
{
  const SoEvent* event = eventCBnode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  if (!SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    int state = FuiModes::getState();
    SoHandleEventAction* evHaAction = eventCBnode->getAction();

    // Build pick filter
    std::vector<int> types;
    if (state == 0 || state == 1)
      types = {
	FdRefPlane::getClassTypeID(),
	FdTire::getClassTypeID(),
	FdAxialSprDa::getClassTypeID(),
	FdHP::getClassTypeID(),
	FdSensor::getClassTypeID()
      };

    long int indexToInterestingPP = -1;
    FdObject* pickedObject = FdPickFilter::getCyceledInterestingPObj(&evHaAction->getPickedPointList(),
                                                                     types,false, // Filter variables
                                                                     indexToInterestingPP);

    if (!event->wasCtrlDown() && (state == 0 || state == 1))
      FdSelector::deselectExpandedLast(); // deselectExpandedLast

    if (state == 3)
      FapEventManager::permUnselectLast();

    if (pickedObject)
    {
      SoPickedPoint* interestingPickedPoint = evHaAction->getPickedPointList()[indexToInterestingPP];
      SoPath* path = interestingPickedPoint->getPath();
      SoNode* tail = ((SoFullPath*)path)->getTail();

      const SoDetail* pDet = interestingPickedPoint->getDetail(tail);
      SoDetail* pickDetail = pDet ? pDet->copy() : NULL;

      SbVec3f pointOnObject = interestingPickedPoint->getObjectPoint();
      const SbMatrix& objToWorld = interestingPickedPoint->getObjectToWorld(tail);

      if (state == 0 || state == 1)
      {
        if (FapEventManager::isPermSelected(pickedObject->getFmOwner()))
        {
          // Deselect previously selected instance
          FapEventManager::permUnselect(pickedObject->getFmOwner());
          // to put it as the last one selected
          FapEventManager::permSelect(pickedObject->getFmOwner());
        }
        else
          FdSelector::selectExpanded(pickedObject);

        FdDB::objectToMove = pickedObject;

        FaVec3 fromPoint = pickedObject->findSnapPoint(pointOnObject,objToWorld,pickDetail,interestingPickedPoint);
        FdPickedPoints::setFirstPP(fromPoint,FdConverter::toFaMat34(objToWorld));

        smartMoveDOF = FdSelector::getDegOfFreedom();
        switch (smartMoveDOF.getType())
          {
          case FaDOF::RIGID:
          case FaDOF::FREE:
            smartMoveDOF.setDirection(FaVec3(1,0,0));
          case FaDOF::PRISM:
            smartMoveDOF.setCenter(FdPickedPoints::getFirstPickedPoint());
          }
        FdExtraGraphics::showDOFVisualizing(smartMoveDOF.getType(),
                                            smartMoveDOF.getCenter(),
                                            smartMoveDOF.getDirection());
        if (FapEventManager::getNumPermSelected() > 0)
          FuiModes::setState(1);
        else if (FuiModes::getState() != 0)
          FuiModes::setState(0);
      }
      else if (state == 2 || state == 3)
      {
        // Selected Object is the Object to move to
        // Get snap-Point on object and show point on object:
        FapEventManager::permAddSelect(pickedObject->getFmOwner());
        FdDB::objectToMoveTo = pickedObject;

        FaVec3 toPoint = pickedObject->findSnapPoint(pointOnObject,objToWorld,pickDetail,interestingPickedPoint);
        FdPickedPoints::setSecondPP(toPoint,FdConverter::toFaMat34(objToWorld));
        FuiModes::setState(3);
      }

      if (pickDetail)
        delete pickDetail;
    }

    else if (state == 0 || state == 1)
    {
      if (FapEventManager::getNumPermSelected() > 0 &&
          dynamic_cast<FmIsRenderedBase*>(FapEventManager::getLastPermSelectedObject()))
      {
        smartMoveDOF = FdSelector::getDegOfFreedom();
        switch (smartMoveDOF.getType())
          {
          case FaDOF::RIGID:
          case FaDOF::FREE:
            smartMoveDOF.setDirection(FaVec3(1,0,0));
          case FaDOF::PRISM:
            smartMoveDOF.setCenter(FdPickedPoints::getFirstPickedPoint());
          }
        FdExtraGraphics::showDOFVisualizing(smartMoveDOF.getType(),
                                            smartMoveDOF.getCenter(),
                                            smartMoveDOF.getDirection());
      }

      if (FapEventManager::getNumPermSelected() > 0)
        FuiModes::setState(1);
      else if (FuiModes::getState() != 0)
        FuiModes::setState(0);
    }
    else if (state == 3)
      FuiModes::setState(2);
  }

  eventCBnode->setHandled();
}


void FdDB::createSimpleSensorEventCB(void*, SoEventCallback* eventCBnode)
{
  const SoEvent* event = eventCBnode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  if (SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    SoHandleEventAction* evHaAction = eventCBnode->getAction();

    std::vector<FdObject*> selectedObjects;
    FdSelector::getSelectedObjects(selectedObjects);

    // Build array of interesting types
    std::vector<int> types = {
      FdTriad::getClassTypeID(),
      FdSimpleJoint::getClassTypeID(),
      FdLinJoint::getClassTypeID(),
      FdCamJoint::getClassTypeID(),
      FdAxialSprDa::getClassTypeID(),
      FdStrainRosette::getClassTypeID()
    };

    long int  indexToInterestingPP    = -1;
    bool      wasASelectedObjInPPList = false;
    FdObject* pickedObject = FdPickFilter::getInterestingPObj(&evHaAction->getPickedPointList(),
                                                              selectedObjects, // This is to select objects behind the already selected one
                                                              types,true, // Filter variables
                                                              indexToInterestingPP,wasASelectedObjInPPList); // Variables returning values
    if (pickedObject)
    {
      // We shall select the object to put sensor on
      FapEventManager::permUnselectAll();
      FapEventManager::permSelect(pickedObject->getFmOwner());
      FuiModes::setState(1);
    }
    else if (FuiModes::getState() == 0 || FuiModes::getState() == 1)
      FuiModes::setState(0);
  }

  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCBnode->setHandled();
}


void FdDB::createTireEventCB(void*, SoEventCallback* eventCBnode)
{
  const SoEvent* event = eventCBnode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  if (SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    std::vector<FdObject*> selectedObjects;
    FdSelector::getSelectedObjects(selectedObjects);

    long int  indexToInterestingPP    = -1;
    bool      wasASelectedObjInPPList = false;
    FdObject* pickedObject = FdPickFilter::getInterestingPObj(&eventCBnode->getAction()->getPickedPointList(),
                                                              selectedObjects, // This is to select objects behind the already selected one
                                                              {FdRevJoint::getClassTypeID()},true, // Filter variables
                                                              indexToInterestingPP,wasASelectedObjInPPList); // Variables returning values

    if (pickedObject)
    {
      // We shall select the object to put the tire on
      FapEventManager::permUnselectAll();
      FapEventManager::permSelect(pickedObject->getFmOwner());
      FuiModes::setState(1);
    }
    else if (FuiModes::getState() == 0 || FuiModes::getState() == 1)
      FuiModes::setState(0);
  }

  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCBnode->setHandled();
}


void FdDB::createRelativeSensorEventCB(void*, SoEventCallback* eventCBnode)
{
  const SoEvent* event = eventCBnode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  if (SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    int state = FuiModes::getState();

    SoHandleEventAction* evHaAction = eventCBnode->getAction();

    std::vector<FdObject*> selectedObjects;
    FdSelector::getSelectedObjects(selectedObjects);

    long int  indexToInterestingPP    = -1;
    bool      wasASelectedObjInPPList = false;
    FdObject* pickedObject = FdPickFilter::getInterestingPObj(&evHaAction->getPickedPointList(),
                                                              selectedObjects, // This is to select objects behind the already selected one
                                                              {FdTriad::getClassTypeID()},true, // Filter variables
                                                              indexToInterestingPP,wasASelectedObjInPPList); // Variables returning values

    if (!pickedObject)
    {
      // The user picked nothing
      if (state == 0 || state == 1)
        FuiModes::setState(0);
      else if (state == 2 || state == 3)
        FuiModes::setState(2);
    }
    else
    {
      if (state == 1)
        FapEventManager::permUnselectAll();

      if (state == 0 || state == 1)
      {
        FapEventManager::permSelect(pickedObject->getFmOwner(),0);
        FuiModes::setState(1);
      }
      else if (state == 2 || state == 3)
      {
        FapEventManager::permSelect(pickedObject->getFmOwner(),1);
        FuiModes::setState(3);
      }
    }
  }

  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCBnode->setHandled();
}


void FdDB::attachEventCB(void*, SoEventCallback* eventCBnode)
{
  const SoEvent* event = eventCBnode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  if (SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    int state = FuiModes::getState();
    SoHandleEventAction* evHaAction = eventCBnode->getAction();

    // Build array of interesting types
    std::vector<int> types;
    if (state == 0 || state == 1)
      types = {
	FdTriad::getClassTypeID(),
	FdSimpleJoint::getClassTypeID(),
	FdLinJoint::getClassTypeID(),
	FdLoad::getClassTypeID(),
	FdAxialSprDa::getClassTypeID(),
	FdCamJoint::getClassTypeID(),
	FdPipeSurface::getClassTypeID()
      };
    else if (state == 2 || state == 3)
      types = { FdLink::getClassTypeID(), FdRefPlane::getClassTypeID() };

    long int  indexToInterestingPP = -1;
    FdObject* pickedObject = FdPickFilter::getCyceledInterestingPObj(&evHaAction->getPickedPointList(),
                                                                     types,true, // Filter variables
                                                                     indexToInterestingPP); // Variables returning values

    if (!pickedObject)
    {
      // The user picked nothing
      if (state == 0 || state == 1)
        FuiModes::setState(0);
      else if (state == 2 || state == 3)
        FuiModes::setState(2);
    }
    else
    {
      // We shall select the object to attach to
      if (state == 1)
        FapEventManager::permUnselectAll();

      SoPickedPoint* interestingPickedPoint = evHaAction->getPickedPointList()[indexToInterestingPP];

      if (state == 0 || state == 1)
      {
        // Picked object is the possible object to attach
        if (pickedObject->isOfType(FdAxialSprDa::getClassTypeID()))
        {
          FmTriad* triad = static_cast<FdAxialSprDa*>(pickedObject)->findClosestTriad(interestingPickedPoint->getObjectPoint());
          if (triad)
          {
            FdDB::objectToAttach = triad->getFdPointer();
            FapEventManager::permSelect(triad,0);
            FuiModes::setState(1);
          }
          else
          {
            FdDB::objectToAttach = NULL;
            FuiModes::setState(0);
          }
        }
        else
        {
          FdDB::objectToAttach = pickedObject;
          FapEventManager::permSelect(pickedObject->getFmOwner(),0);
          FuiModes::setState(1);
        }
      }
      else if (state == 2 || state == 3)
      {
        // Selected object is the link or reference plane to attach to
        FdDB::linkToAttachTo = pickedObject;
        FapEventManager::permSelect(pickedObject->getFmOwner(),1);
        FuiModes::setState(3);
      }
    }
  }

  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCBnode->setHandled();
}


void FdDB::detachEventCB(void*, SoEventCallback* eventCBnode)
{
  const SoEvent* event = eventCBnode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  if (SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    SoHandleEventAction* evHaAction = eventCBnode->getAction();

    std::vector<FdObject*> selectedObjects;
    FdSelector::getSelectedObjects(selectedObjects);

    // Build array of interesting types
    std::vector<int> types = {
      FdTriad::getClassTypeID(),
      FdSimpleJoint::getClassTypeID(),
      FdLinJoint::getClassTypeID(),
      FdLoad::getClassTypeID(),
      FdAxialSprDa::getClassTypeID(),
      FdCamJoint::getClassTypeID()
    };

    long int  indexToInterestingPP    = -1;
    bool      wasASelectedObjInPPList = false;
    FdObject* pickedObject = FdPickFilter::getInterestingPObj(&evHaAction->getPickedPointList(),
                                                              selectedObjects, // This is to select objects behind the already selected one
                                                              types,true, // Filter variables
                                                              indexToInterestingPP,wasASelectedObjInPPList); // Variables returning values

    if (!pickedObject)
    {
      // The user picked nothing
      if (FuiModes::getState() == 0)
        FuiModes::setState(0);
      else if (FuiModes::getState() == 1)
      {
        FdDB::objectToDetach = NULL; // We don't want to detach them.
        FmIsRenderedBase::detachTriad = NULL;
        FuiModes::setState(0);
      }
    }
    else
    {
      // We shall select the object to detach.
      FapEventManager::permUnselectAll();

      FdDB::objectToDetach = pickedObject;
      if (pickedObject->isOfType(FdAxialSprDa::getClassTypeID()))
      {
        SoPickedPoint* interestingPickedPoint = evHaAction->getPickedPointList()[indexToInterestingPP];
        FmIsRenderedBase::detachTriad = static_cast<FdAxialSprDa*>(pickedObject)->findClosestTriad(interestingPickedPoint->getObjectPoint());
        if (FmIsRenderedBase::detachTriad)
        {
          FapEventManager::permSelect(FmIsRenderedBase::detachTriad);
          FuiModes::setState(1);
        }
        else
        {
          FdDB::objectToDetach = NULL;
          FuiModes::setState(0);
        }
      }
      else
      {
        FapEventManager::permSelect(pickedObject->getFmOwner());
        FuiModes::setState(1);
      }
    }
  }

  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCBnode->setHandled();
}


void FdDB::pickLoadPointEventCB(void*, SoEventCallback* eventCBnode)
{
  const SoEvent* event = eventCBnode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  if (SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    SoHandleEventAction* evHaAction = eventCBnode->getAction();

    std::vector<FdObject*> selectedObjects;
    FdSelector::getSelectedObjects(selectedObjects);

    // Build array of interesting types
    std::vector<int> types;
    switch (FuiModes::getMode()) {
    case FuiModes::PICKLOADATTACKPOINT_MODE:
    case FuiModes::ADDMASTERINLINJOINT_MODE:
      types = { FdLink::getClassTypeID() };
      break;
    default:
      types = { FdLink::getClassTypeID(), FdRefPlane::getClassTypeID() };
      break;
    }

    long int  indexToInterestingPP    = -1;
    bool      wasASelectedObjInPPList = false;
    FdObject* pickedObject = FdPickFilter::getInterestingPObj(&evHaAction->getPickedPointList(),
                                                              selectedObjects, // An array of FdObject*'s  that is selected
                                                              types,true, // Filter variables
                                                              indexToInterestingPP,wasASelectedObjInPPList); // Variables returning values

    FapEventManager::permUnselectAll();
    if (pickedObject)
    {
      FapEventManager::permSelect(pickedObject->getFmOwner());

      SoPickedPoint* interestingPickedPoint = evHaAction->getPickedPointList()[indexToInterestingPP];

      const SoDetail* pDet = interestingPickedPoint->getDetail();
      SoDetail* pickDetail = pDet ? pDet->copy() : NULL;

      SbVec3f pointOnObject = interestingPickedPoint->getObjectPoint();
      const SbMatrix& objToWorld = interestingPickedPoint->getObjectToWorld(NULL);
      FaVec3 fromPoint = pickedObject->findSnapPoint(pointOnObject,objToWorld,pickDetail,interestingPickedPoint);
      if (pickDetail)
        delete pickDetail;

      FdPickedPoints::setFirstPP(fromPoint,FdConverter::toFaMat34(objToWorld));
      FuiModes::setState(1);
    }
    else
      FuiModes::setState(0);
  }

  eventCBnode->setHandled();
}


void FdDB::pickMeasurePointEventCB(void*, SoEventCallback* eventCBnode)
{
  const SoEvent* event = eventCBnode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  if (SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    SoHandleEventAction* evHaAction = eventCBnode->getAction();

    std::vector<FdObject*> selectedObjects;
    FdSelector::getSelectedObjects(selectedObjects);

    long int  indexToInterestingPP    = -1;
    bool      wasASelectedObjInPPList = false;
    FdObject* pickedObject = FdPickFilter::getInterestingPObj(&evHaAction->getPickedPointList(),
                                                              selectedObjects, // An array of FdObject*'s  that is selected
                                                              {},false, // No variables filtering
                                                              indexToInterestingPP,wasASelectedObjInPPList); // Variables returning values

    int newState = 0;
    FapEventManager::permUnselectAll();
    if (pickedObject)
    {
      FapEventManager::permSelect(pickedObject->getFmOwner());

      SoPickedPoint* interestingPickedPoint = evHaAction->getPickedPointList()[indexToInterestingPP];

      const SoDetail* pDet = interestingPickedPoint->getDetail();
      SoDetail* pickDetail = pDet ? pDet->copy() : NULL;

      SbVec3f pointOnObject = interestingPickedPoint->getObjectPoint();
      const SbMatrix& objToWorld = interestingPickedPoint->getObjectToWorld(NULL);
      FaVec3 fromPoint = pickedObject->findSnapPoint(pointOnObject,objToWorld,pickDetail,interestingPickedPoint);
      if (pickDetail)
        delete pickDetail;

      switch (FuiModes::getState()) {
      case 0:
        FdPickedPoints::setFirstPP(fromPoint,FdConverter::toFaMat34(objToWorld));
        newState = 1;
        break;
      case 1:
        FdPickedPoints::setSecondPP(fromPoint,FdConverter::toFaMat34(objToWorld));
        if (FuiModes::getMode() == FuiModes::MEASURE_ANGLE_MODE) newState = 2;
        break;
      case 2:
        FdPickedPoints::setThirdPP(fromPoint,FdConverter::toFaMat34(objToWorld));
        break;
      }

      if (newState == 0)
      {
        std::vector<FaVec3> points;
        FdPickedPoints::getAllPickedPointsGlobal(points);
        if (points.size() > 0)
          ListUI <<"\n   * First picked point: "<< points[0];
        if (points.size() > 1)
          ListUI <<"\n   * Second picked point: "<< points[1];
        if (points.size() == 2)
          ListUI <<"\n   * Relative distance: "<< (points[1]-points[0]).length();
        else if (points.size() > 2)
          ListUI <<"\n   * Picked origin of rotation: "<< points[2]
                 <<"\n   * Rotation angle (rad): "<< (points[0]-points[2]).angle(points[1]-points[2]);
        if (!points.empty())
          FFaMsg::list("\n",true);
      }
    }
    FuiModes::setState(newState);
  }

  eventCBnode->setHandled();
}


//
//   View methods
//
//////////////////////////////////////////////////


void FdDB::setLineWidth(int width)
{
  FdSymbolDefs::setSymbolLineWidth(width);

  std::vector<FmLink*> links;
  FmDB::getAllLinks(links);

  for (FmLink* link : links)
    static_cast<FdLink*>(link->getFdPointer())->getVisualModel()->myGroupParts.setLineWidth(FdFEGroupPartSet::SPECIAL_LINES,width);
}


void FdDB::setNiceTransparency(bool nice)
{
  if (FdDB::viewer && FdDB::viewer->getGLRenderAction())
    FdDB::viewer->getGLRenderAction()->setTransparencyType(nice ? SoGLRenderAction::SORTED_OBJECT_BLEND : SoGLRenderAction::SCREEN_DOOR);

  FdDB::viewer->render();
}


void FdDB::setSolidView(bool isSolid)
{
  std::vector<FmLink*> links;
  FmDB::getAllLinks(links);

  for (FmLink* link : links)
    if (isSolid)
      link->getFdPointer()->updateFdDetails();
    else
      static_cast<FdLink*>(link->getFdPointer())->getVisualModel()->setDrawStyle(FdFEVisControl::LINES);
}

void FdDB::redraw()
{
  viewer->render();
}

void FdDB::setAutoRedraw(bool setOn)
{
  viewer->setAutoRedraw(setOn);
}

void FdDB::zoomTo(FmIsRenderedBase* obj)
{
  if (!obj) return;

  if (obj->isOfType(FmCtrlElementBase::getClassTypeID()) ||
      obj->isOfType(FmCtrlLine::getClassTypeID())) {
    FdCtrlDB::zoomTo(obj);
    return;
  }

  FFuTopLevelShell* modeller = FFuTopLevelShell::getInstanceByType(FuiModeller::getClassTypeID());
  if (modeller)
    modeller->popUp();
  if (obj->getFdPointer())
    viewer->viewAll(obj->getFdPointer()->getKit());
}

void FdDB::zoomAll()
{
  viewer->viewAll();
}

void FdDB::perspectiveView()
{
  viewer->perspectiveView();
}

void FdDB::parallellView()
{
  viewer->parallellView();
}

void FdDB::isometricView()
{
  viewer->isometricView();
}

void FdDB::XYpZpYView(){ viewer->XYpZpYView();}
void FdDB::XYnZpYView(){ viewer->XYnZpYView();}
void FdDB::XZnYpZView(){ viewer->XZnYpZView();}
void FdDB::XZpYpZView(){ viewer->XZpYpZView();}
void FdDB::YZpXpZView(){ viewer->YZpXpZView();}
void FdDB::YZnXpZView(){ viewer->YZnXpZView();}

void FdDB::RotateView(double xRot, double yRot)
{
  viewer->RotateView(xRot,yRot);
}

void FdDB::setFogOn(bool yesOrNo)
{
  FdDB::fogNode->fogType = yesOrNo ? SoEnvironment::HAZE : SoEnvironment::NONE;
}

void FdDB::setFogVisibility(double visibility)
{
  FdDB::fogNode->fogVisibility.setValue(visibility);
}

void FdDB::setAntialiazingOn(bool yesOrNo)
{
  FdDB::viewer->setAntialiasing(yesOrNo,2);
}


inline float cut(float c) { return c < 0.0f ? 0.0f : (c > 1.0f ? 1.0f : c); }

void FdDB::setViewerBackground(const FdColor& color)
{
  float r = cut(color[0]);
  float g = cut(color[1]);
  float b = cut(color[2]);
  bool useBlack = r+g+b > 1.5f;

  FdDB::viewer->setBackgroundColor(SbColor(r,g,b));
  FdDB::fogNode->fogColor.setValue(SbColor(r,g,b));

  FdAnimationInfo * aInfo = FdDB::getAnimInfoNode();
  if (!useBlack){
    aInfo->timeColor.setValue(SbColor(1,1,1));
    aInfo->stepColor.setValue(SbColor(1,1,1));
    aInfo->progressColor.setValue(SbColor(1,1,0));
    aInfo->shadowColor.setValue(SbColor(0.1f, 0.1f, 0.1f));
  }else{
    aInfo->timeColor.setValue(SbColor(0,0,0));
    aInfo->stepColor.setValue(SbColor(0,0,0));
    aInfo->progressColor.setValue(SbColor(0,0,0.5f));
    aInfo->shadowColor.setValue(SbColor(0.9f, 0.9f, 0.9f));
  }

#ifdef USE_SMALLCHANGE
  if (useBlack){
    FdDB::legend->setTickAndLinesColor(SbColor(0,0,0.5f));
    FdDB::legend->setTextColor(SbColor(0,0,0));
  }else{
    FdDB::legend->setTickAndLinesColor(SbColor(1,1,0));
    FdDB::legend->setTextColor(SbColor(1,1,1));
  }
#endif
  if (useBlack){
    FdDB::axisCross->crossColor.setValue(SbColor(0,0,0));
    FdDB::axisCross->textColor.setValue(SbColor(0,0,0));
  }else{
    FdDB::axisCross->crossColor.setValue(SbColor(1,1,1));
    FdDB::axisCross->textColor.setValue(SbColor(1,1,1));
  }

  FdFEVisControl::setViewerBackgroundColor(color);

  // Update the FdFEVisControl objects to be aware of the new background.

  std::vector<FmLink*> links;
  FmDB::getAllLinks(links);

  for (FmLink* link : links)
    static_cast<FdLink*>(link->getFdPointer())->getVisualModel()->myGroupParts.update();
}

void FdDB::setShading(bool on)
{
  FdDB::lightModel->model.setValue(on ? SoLightModel::PHONG : SoLightModel::BASE_COLOR);
  FdDB::lightModel->setOverride(!on);
}

bool FdDB::isShading()
{
  return (FdDB::lightModel->model.getValue() == SoLightModel::PHONG);
}

void FdDB::setFrontFaceLightOnly(bool doIt)
{
  if (doIt)
    FdDB::ourShapeHint->vertexOrdering = SoShapeHints::UNKNOWN_ORDERING;
  else
    FdDB::ourShapeHint->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
}

bool FdDB::isFrontFaceLightOnly()
{
  return (FdDB::ourShapeHint->vertexOrdering.getValue() == SoShapeHints::UNKNOWN_ORDERING);
}

//
//
//  View filter methods
//
///////////////////////////////////////////////////////////

void FdDB::showJoints(int classTypeID, bool show)
{
  std::vector<FmModelMemberBase*> joints;
  FmDB::getAllOfType(joints,classTypeID);
  for (FmModelMemberBase* obj : joints)
  {
    FdObject* fdObj = static_cast<FmIsRenderedBase*>(obj)->getFdPointer();
    if (show)
    {
      if (obj->isOfType(FmCamJoint::getClassTypeID()))
        fdObj->updateFdTopology();
      fdObj->updateFdDetails();
    }
    else if (obj->isOfType(FmSMJointBase::getClassTypeID()))
      static_cast<FdSimpleJoint*>(fdObj)->hide();
    else if (obj->isOfType(FmCamJoint::getClassTypeID()))
      static_cast<FdCamJoint*>(fdObj)->hide();
    else if (obj->isOfType(FmMMJointBase::getClassTypeID()))
      static_cast<FdLinJoint*>(fdObj)->hide();
  }
}

void FdDB::showRevoluteJoints(bool YesOrNo)
{
  FdDB::showJoints(FmRevJoint::getClassTypeID(), YesOrNo);
}

void FdDB::showBallJoints(bool YesOrNo)
{
  FdDB::showJoints(FmBallJoint::getClassTypeID(), YesOrNo);
}

void FdDB::showRigidJoints(bool YesOrNo)
{
  FdDB::showJoints(FmRigidJoint::getClassTypeID(), YesOrNo);
}

void FdDB::showFreeJoints(bool YesOrNo)
{
  FdDB::showJoints(FmFreeJoint::getClassTypeID(), YesOrNo);
}

void FdDB::showPrismaticJoints(bool YesOrNo)
{
  FdDB::showJoints(FmPrismJoint::getClassTypeID(), YesOrNo);
}

void FdDB::showCylindricJoints(bool YesOrNo)
{
  FdDB::showJoints(FmCylJoint::getClassTypeID(), YesOrNo);
}

void FdDB::showCamJoints(bool YesOrNo)
{
  FdDB::showJoints(FmCamJoint::getClassTypeID(), YesOrNo);
}

void FdDB::showParts(const char* partList, bool YesOrNo)
{
  SoNodeKitListPart* klp = SO_GET_PART(mechanismKit,partList,SoNodeKitListPart);
  if (klp)
    klp->containerSet(YesOrNo ? "whichChild -3" : "whichChild -1");
  else
    std::cerr <<" *** FdDB::showParts: "<< partList
              <<" does not exist"<< std::endl;
}

void FdDB::showParts(bool YesOrNo)
{
  FdDB::showParts("partListSw",YesOrNo);
  FdDB::showParts("uelmListSw",YesOrNo);
}

void FdDB::showBeams(bool YesOrNo)
{
  FdDB::showParts("beamListSw",YesOrNo);
}

void FdDB::showPartCS(bool YesOrNo)
{
  std::vector<FmLink*> parts;
  FmDB::getAllLinks(parts);

  for (FmLink* link : parts)
    if (!link->isOfType(FmBeam::getClassTypeID()))
      static_cast<FdLink*>(link->getFdPointer())->showCS(YesOrNo);
}

void FdDB::showBeamCS(bool YesOrNo)
{
  std::vector<FmBeam*> beams;
  FmDB::getAllBeams(beams);

  for (FmBeam* beam : beams)
    static_cast<FdBeam*>(beam->getFdPointer())->showCS(YesOrNo);
}

void FdDB::showInternalPartCSs(bool YesOrNo)
{
  std::vector<FmPart*> parts;
  FmDB::getAllParts(parts);

  for (FmPart* part : parts)
    static_cast<FdPart*>(part->getFdPointer())->showInternalCSs(YesOrNo);
}

void FdDB::showPartCoGCSs(bool YesOrNo)
{
  std::vector<FmLink*> parts;
  FmDB::getAllLinks(parts);

  for (FmLink* link : parts)
    if (!link->isOfType(FmBeam::getClassTypeID()))
      static_cast<FdLink*>(link->getFdPointer())->showCoGCS(YesOrNo);
}

void FdDB::showTriads(bool YesOrNo)
{
  FdDB::showParts("triadListSw",YesOrNo);
}

void FdDB::showBeamTriads(bool)
{
  std::vector<FmTriad*> triads;
  FmDB::getAllTriads(triads);

  for (FmTriad* triad : triads)
    triad->updateDisplayDetails();
}

void FdDB::showJoints(bool YesOrNo)
{
  FdDB::showParts("simpleJointListSw",YesOrNo);
  FdDB::showParts("linJointListSw",YesOrNo);
  FdDB::showParts("camJointListSw",YesOrNo);
}

void FdDB::showSprDas(bool YesOrNo)
{
  FdDB::showParts("sprDaListSw",YesOrNo);
}

void FdDB::showHPs(bool YesOrNo)
{
  FdDB::showParts("HPListSw",YesOrNo);
}

void FdDB::showLoads(bool YesOrNo)
{
  FdDB::showParts("loadListSw",YesOrNo);
}

void FdDB::showFeedbacks(bool YesOrNo)
{
  FdDB::showParts("sensorListSw",YesOrNo);
}

void FdDB::showStickers(bool YesOrNo)
{
  FdDB::showParts("stickerListSw",YesOrNo);
}

void FdDB::showRefPlanes(bool YesOrNo)
{
  FdDB::showParts("refPlaneListSw",YesOrNo);
}

void FdDB::showSeaStates(bool YesOrNo)
{
  FdDB::showParts("seaStateListSw",YesOrNo);
}

void FdDB::showWaves(bool YesOrNo)
{
  FmSeaState* seaState = FmDB::getSeaStateObject(false);
  if (seaState)
  {
    static_cast<FdSeaState*>(seaState->getFdPointer())->showWaves(YesOrNo);
    seaState->getFdPointer()->updateFdDetails();
  }
}

void FdDB::showTires(bool YesOrNo)
{
  FdDB::showParts("tireListSw",YesOrNo);
}

void FdDB::showContactSurfaces(bool YesOrNo)
{
  FdDB::showParts("pipeSurfaceListSw",YesOrNo);
  FdDB::showParts("camJointListSw",YesOrNo);
}

void FdDB::showStrainRosettes(bool YesOrNo)
{
  FdDB::showParts("rosetteListSw",YesOrNo);
}

void FdDB::setTireColor(const FdColor& color)
{
  std::vector<FmModelMemberBase*> fmptrs;
  FmDB::getAllOfType(fmptrs,FmTire::getClassTypeID());

  for (FmModelMemberBase* obj : fmptrs)
    static_cast<FdTire*>(static_cast<FmIsRenderedBase*>(obj)->getFdPointer())->setColor(color);
}

void FdDB::setFEBeamSysScale(float scale)
{
  std::vector<FmPart*> parts;
  FmDB::getAllParts(parts);

  for (FmPart* part : parts)
    if (static_cast<FdPart*>(part->getFdPointer())->updateSpecialLines(scale))
      part->getFdPointer()->updateFdDetails();
}


//////////////////////////////////////////////////////////////
//
// Export graphics:
//
/////////////////////////////////////////////////////////////

bool FdDB::exportIV(const char* filename)
{
  return FdExportIv::exportGraph(FdDB::viewer->getSceneGraph(), filename);
}

bool FdDB::exportRGB(const char* filename)
{
  return FdDB::exportAsPicture(filename, "rgb");
}

bool FdDB::exportJPEG(const char* filename)
{
  return FdDB::exportAsPicture(filename, "jpeg");
}

bool FdDB::exportPNG(const char* filename)
{
  return FdDB::exportAsPicture(filename, "png");
}

bool FdDB::exportBMP(const char* filename)
{
  return FdDB::exportAsPicture(filename, "bmp");
}

bool FdDB::exportGIF(const char* filename)
{
  return FdDB::exportAsPicture(filename, "gif");
}

bool FdDB::exportTIFF(const char* filename)
{
  return FdDB::exportAsPicture(filename, "tiff");
}


// Method to do all the work for the others...
bool FdDB::exportAsPicture(const char* filename, const char* extension)
{
  // Render the scene
  SoOffscreenRenderer rend(FdDB::viewer->getViewportRegion());
  rend.setBackgroundColor(FdDB::viewer->getBackgroundColor());

#ifdef win32
  // store current context
  HGLRC glrc = wglGetCurrentContext();
  HDC dc = wglGetCurrentDC();
#endif

  bool success = rend.render(FdDB::viewer->getSceneManager()->getSceneGraph());

#ifdef win32
  // restore current context
  wglMakeCurrent(dc,glrc);
#endif

  return success && rend.writeToFile(SbString(filename),SbName(extension));
}


bool FdDB::exportEPS(const char* filename)
{
  FILE* file = fopen(filename,"w");
  if (!file) return false;

  const SbViewportRegion& vp = viewer->getViewportRegion();
  const SbVec2s& imagePixSize = vp.getViewportSizePixels();

  SbVec2f imageInches;
  float pixPerInch = SoOffscreenRenderer::getScreenPixelsPerInch();
  imageInches.setValue((float)imagePixSize[0] / pixPerInch,
                       (float)imagePixSize[1] / pixPerInch);

  int printerDPI = 75;
  SbVec2s postScriptRes;
  postScriptRes.setValue((short)(imageInches[0])*printerDPI,
                         (short)(imageInches[1])*printerDPI);

  SbViewportRegion vport;
  vport.setWindowSize(postScriptRes);
  vport.setPixelsPerInch((float)printerDPI);

  SoOffscreenRenderer rend(vport);
  bool success = rend.render(ourRoot);
  if (success) rend.writeToPostScript(file);

  fclose(file);
  return success;
}


bool FdDB::isVRMLFile(const std::string& fileName)
{
  SoInput vrmlReader;
  std::string aFile(fileName);
  FFaFilePath::makeItAbsolute(aFile,FmDB::getMechanismObject()->getAbsModelFilePath());
  vrmlReader.openFile(aFile.c_str());

  return vrmlReader.isFileVRML2() || vrmlReader.isFileVRML1();
}


int FdDB::getCadFileType(const std::string& fileName)
{
  static std::map<std::string,int> extToTypeMap;
  if (extToTypeMap.empty()) {
    extToTypeMap["vrml"] = FD_VRML_FILE;
    extToTypeMap["wrl"]  = FD_VRML_FILE;
    extToTypeMap["vrl"]  = FD_VRML_FILE;
    extToTypeMap["wrz"]  = FD_VRML_FILE;
    extToTypeMap["igs"]  = FD_IGES_FILE;
    extToTypeMap["iges"] = FD_IGES_FILE;
    extToTypeMap["stp"]  = FD_STEP_FILE;
    extToTypeMap["step"] = FD_STEP_FILE;
    extToTypeMap["brep"] = FD_BREP_FILE;
    extToTypeMap["rle"]  = FD_BREP_FILE;
    extToTypeMap["ftc"]  = FD_FCAD_FILE;
    extToTypeMap["obj"]  = FD_OBJ_FILE;
  }

  FFaLowerCaseString ext(FFaFilePath::getExtension(fileName));
  std::map<std::string,int>::const_iterator it = extToTypeMap.find(ext);
  if (it == extToTypeMap.end())
    return FD_UNKNOWN_FILE;
  else if (it->second == FD_VRML_FILE && !FdDB::isVRMLFile(fileName))
    return FD_UNKNOWN_FILE;

  return it->second;
}


void FdDB::showDirFlipUI(const FaVec3& pickedPoint)
{
  pointSelData.isDirOnly = true;
  pointSelData.pickedPoint = pickedPoint;
  pointSelData.ppNormal = FdDB::firstCreateDirection;

  pointSelData.createAndUpdateUI();
}


void FdDB::showPointSelectionUI(bool isEdge, int type,
                                const FaVec3& pickedPoint,
                                const FaVec3& vxSnappedPPoint,
                                const FaVec3& ppNormal,
                                const FaVec3& axis, bool isAxisDefined,
                                const FaVec3& origin, bool isOriginDefined,
                                bool showDirection, int pPointIdx)
{
  if (!isOriginDefined) return;

  pointSelData.isDirOnly = false;
  pointSelData.pPointIdx = pPointIdx;
  pointSelData.isEdge = isEdge;
  pointSelData.pickedPoint = pickedPoint;
  pointSelData.vxSnappedPPoint = vxSnappedPPoint;
  pointSelData.ppNormal = ppNormal;
  pointSelData.axis = axis;
  pointSelData.isAxisDefined = isAxisDefined;
  pointSelData.origin = origin;
  pointSelData.showDirection = showDirection;
  pointSelData.isUnknownShape = true;

  FaVec3 oPP = pickedPoint - origin;
  FaVec3 Ea = axis;
  Ea.normalize();

  const double epsTol = 1.0e-7; // Todo set tolerance more intelligently

  if (isEdge)
  {
    // Find whether pp is on axis.
    if (type == FdCadEntityInfo::CIRCLE) // Circle or ellipsis
      if ((Ea*(oPP*Ea) - oPP).sqrLength() > epsTol*epsTol) // PP is not on the axis
        pointSelData.isUnknownShape = false;
  }
  else switch (type) // Surface
  {
  case FdCadEntityInfo::SPHERE:
    pointSelData.isUnknownShape = isAxisDefined;
    break;
  case FdCadEntityInfo::CYLINDER:
  case FdCadEntityInfo::CONE:
  case FdCadEntityInfo::TORUS:
  case FdCadEntityInfo::SREV:
    pointSelData.isUnknownShape = false;
    break;
  default: // Plane
    if (!ppNormal.isParallell(axis,0.001) || oPP*Ea > epsTol)
      pointSelData.isUnknownShape = false;
    break;
  }

  pointSelData.createAndUpdateUI();
}


void FdDB::hidePointSelectionUI()
{
  pointSelData.hideUI();
}
