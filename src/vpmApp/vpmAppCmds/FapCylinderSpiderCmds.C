// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapCylinderSpiderCmds.H"
#include "vpmApp/FapEventManager.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "vpmUI/FuiModes.H"
#include "vpmUI/Fui.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmPart.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"

#ifdef USE_INVENTOR
#include "vpmDisplay/FdExtraGraphics.H"
#include "vpmDisplay/FdTriad.H"
#include "vpmDisplay/FdPart.H"
#include "vpmDisplay/FdSelector.H"
#include "vpmDisplay/FdPickFilter.H"
#include "vpmDisplay/FdPickedPoints.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDisplay/FdEvent.H"

#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/details/SoDetail.h>
#include <Inventor/SoPickedPoint.h>
#endif

int FapCylinderSpiderCmds::ourState = 0;
int FapCylinderSpiderCmds::ourSpiderType = FmTriad::NONE;

std::vector<FaVec3> FapCylinderSpiderCmds::ourCylinderPoints;
FmPart*  FapCylinderSpiderCmds::ourSpiderPart = NULL;
bool     FapCylinderSpiderCmds::ourUseArcOnly = true;
bool     FapCylinderSpiderCmds::ourSelectedRefNode = false;
FaVec3   FapCylinderSpiderCmds::ourGlobalRefNodePos;
FmTriad* FapCylinderSpiderCmds::ourSelectedTriad = NULL;


void FapCylinderSpiderCmds::init()
{
  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_createRBE2GeomSpider");
  cmdItem->setSmallIcon(spider_geom_rbe2_xpm);
  cmdItem->setText("By cylinder surface");
  cmdItem->setToolTip("Create a rigid connection element by defining a cylinder.\n"
                      "The master node is placed in the center.\n"
                      "Nodes on the cylinder surface are selected as slaves.");
  cmdItem->setActivatedCB(FFaDynCB0S(FapCylinderSpiderCmds::createRBE2Spider));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_createRBE3GeomSpider");
  cmdItem->setSmallIcon(spider_geom_rbe3_xpm);
  cmdItem->setText("By cylinder surface");
  cmdItem->setToolTip("Create a flexible connection element by defining a cylinder.\n"
                      "The reference node is placed in the center.\n"
                      "Nodes on the cylinder surface are selected as masters.");
  cmdItem->setActivatedCB(FFaDynCB0S(FapCylinderSpiderCmds::createRBE3Spider));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));
}


void FapCylinderSpiderCmds::createRBE2Spider()
{
  FapCylinderSpiderCmds::ourSpiderType = FmTriad::RIGID;
  FuiModes::setMode(FuiModes::MAKECYLINDERSPIDER_MODE);
}


void FapCylinderSpiderCmds::createRBE3Spider()
{
  FapCylinderSpiderCmds::ourSpiderType = FmTriad::AVERAGE_MOTION;
  FuiModes::setMode(FuiModes::MAKECYLINDERSPIDER_MODE);
}


void FapCylinderSpiderCmds::enterMode()
{
  FapEventManager::permUnselectAll();
  ourCylinderPoints.clear();
#ifdef USE_INVENTOR
  FdEvent::addEventCB(FapCylinderSpiderCmds::eventCB);
#endif
  FapCylinderSpiderCmds::setState(START);
}


void FapCylinderSpiderCmds::cancelMode()
{
  FapEventManager::permUnselectAll();
#ifdef USE_INVENTOR
  FdPickedPoints::deselectNode();
  FdPickedPoints::resetPPs();
  FdExtraGraphics::hideCylinder();
  FdEvent::removeEventCB(FapCylinderSpiderCmds::eventCB);
#endif
  ourCylinderPoints.clear();
  FapCylinderSpiderCmds::ourState = 0;
  FapCylinderSpiderCmds::ourSpiderPart = NULL;
}


void FapCylinderSpiderCmds::setState(int newState)
{
  const char* texts[] = {"Complete circle", "Arc only", NULL};

  switch (FapCylinderSpiderCmds::ourState = newState)
    {
    case START:
      FapEventManager::permUnselectAll();
#ifdef USE_INVENTOR
      FdPickedPoints::deselectNode();
      FdPickedPoints::resetPPs();
      FdExtraGraphics::hideCylinder();
#endif
      ourSpiderPart = NULL;
      ourSelectedTriad = NULL;
      ourGlobalRefNodePos = FaVec3();
      ourCylinderPoints.clear();
      ourUseArcOnly = false;
      ourSelectedRefNode = false;

      Fui::tip("Select the triad to attach with the connector "
               "OR the position of the reference node,\n"
               "OR the first of three nodes on the circle circumference");
      break;
    case MASTER_OBJPOINT_SELECTED:
      Fui::tip("Accept by pressing Done,\n"
               "OR pick again to reselect triad, reference node position or circle point.");
      break;
    case MASTER_OBJPOINT_ACCEPTED:
      Fui::tip("Select the first of three points on the circle circumference");
      break;
    case N_1_SELECTED_AFTER_MAST_OBJPOINT:
      Fui::tip("Accept by pressing Done,\n"
               "OR pick again to reselect first circle point");
      break;
    case N_1_SELECTED:
      Fui::tip("Accept by pressing Done,\n"
               "OR pick again to reselect circle point, triad or reference node position.");
      break;
    case N_1_ACCEPTED:
      Fui::tip("Select second node at the circle circumference.");
      break;
    case N_2_SELECTED:
      Fui::tip("Accept by pressing Done,\n"
               "or pick again to reselect the second circle point");
      break;
    case N_2_ACCEPTED:
      Fui::tip("Select third node at the circle circumference.");
      break;
    case N_3_SELECTED:
      Fui::tip("Accept by pressing Done,\n"
               "or pick again to reselect the third circle point");
#ifdef USE_INVENTOR
      FdExtraGraphics::showCylinder(ourSpiderPart->getGlobalCS(),ourCylinderPoints,ourUseArcOnly);
#endif
      break;
    case N_3_ACCEPTED:
      ourUseArcOnly = FFaMsg::dialog("Do you want to use the complete circle or just the arc ?", FFaMsg::GENERIC, texts) == 1;
      Fui::tip("Select the start of the cylinder.\n"
               "(Pick a node in the plane)");
#ifdef USE_INVENTOR
      FdExtraGraphics::showCylinder(ourSpiderPart->getGlobalCS(),ourCylinderPoints,ourUseArcOnly);
#endif
      break;
    case START_NODE_SELECTED:
      Fui::tip("Accept by pressing Done,\n"
               "or pick again to reselect the starting point of the cylinder.");
#ifdef USE_INVENTOR
      FdExtraGraphics::showCylinder(ourSpiderPart->getGlobalCS(),ourCylinderPoints,ourUseArcOnly);
#endif
      break;
    case START_NODE_ACCEPTED:
      Fui::tip("Select the end point of the cylinder.\n"
               "(Pick a node in the plane)");
      break;
    case END_NODE_SELECTED:
      Fui::tip("Accept by pressing Done,\n"
               "or pick again to reselect the end point of the cylinder.");
#ifdef USE_INVENTOR
      FdExtraGraphics::showCylinder(ourSpiderPart->getGlobalCS(),ourCylinderPoints,ourUseArcOnly);
#endif
      break;
    case END_NODE_ACCEPTED:
    case MASTER_PLANE_SELECTED:
      Fui::tip("Press Done to confirm master node plane, or pick again to select a different node.");
      break;
    }
}


void FapCylinderSpiderCmds::done()
{
  switch (FapCylinderSpiderCmds::ourState)
    {
    case START:
      FuiModes::cancel();
      break;
    case MASTER_OBJPOINT_SELECTED:
      FapCylinderSpiderCmds::setState(MASTER_OBJPOINT_ACCEPTED);
      break;
    case MASTER_OBJPOINT_ACCEPTED:
      break;
    case N_1_SELECTED_AFTER_MAST_OBJPOINT:
      FapCylinderSpiderCmds::setState(N_1_ACCEPTED);
      break;
    case N_1_SELECTED:
      FapCylinderSpiderCmds::setState(N_1_ACCEPTED);
      break;
    case N_1_ACCEPTED:
      break;
    case N_2_SELECTED:
      FapCylinderSpiderCmds::setState(N_2_ACCEPTED);
      break;
    case N_2_ACCEPTED:
      break;
    case N_3_SELECTED:
      FapCylinderSpiderCmds::setState(N_3_ACCEPTED);
      break;
    case N_3_ACCEPTED:
      FapEventManager::permUnselectAll();
      FapCylinderSpiderCmds::createSpider(false);
      FapCylinderSpiderCmds::setState(START);
      break;
    case START_NODE_SELECTED:
      FapCylinderSpiderCmds::setState(START_NODE_ACCEPTED);
      break;
    case START_NODE_ACCEPTED:
      FapEventManager::permUnselectAll();
      FapCylinderSpiderCmds::createSpider(false);
      FapCylinderSpiderCmds::setState(START);
      break;
    case END_NODE_SELECTED:
      if (ourSelectedRefNode) {
        FapEventManager::permUnselectAll();
        FapCylinderSpiderCmds::createSpider(false);
        FapCylinderSpiderCmds::setState(START);
      }
      else
        FapCylinderSpiderCmds::setState(END_NODE_ACCEPTED);
      break;
    case END_NODE_ACCEPTED:
      FapEventManager::permUnselectAll();
      FapCylinderSpiderCmds::createSpider(false);
      FapCylinderSpiderCmds::setState(START);
      break;
    case MASTER_PLANE_SELECTED:
      FapEventManager::permUnselectAll();
      FapCylinderSpiderCmds::createSpider(true);
      FapCylinderSpiderCmds::setState(START);
      break;
    default:
      FuiModes::cancel();
      break;
    }
}


#ifdef USE_INVENTOR
void FapCylinderSpiderCmds::eventCB(void*, SoEventCallback* eventCallbackNode)
{
  const SoEvent* event = eventCallbackNode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  if (SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    SoHandleEventAction* evHaAction = eventCallbackNode->getAction();
    const SoPickedPointList& ppl = evHaAction->getPickedPointList();

    std::vector<FdObject*> selectedObjects;
    long indexToInterestingPP = -1;
    bool wasASelectedObjInPPList = false;
    FdObject* pickedObject = NULL;
    FdObject* fdSpiderPart = ourSpiderPart ? ourSpiderPart->getFdPointer() : NULL;
    FdPart* fdpart = NULL;
    int    nodeID = -1;
    FaVec3 worldNodePos;

    switch (FapCylinderSpiderCmds::ourState)
      {
      case START:
      case N_1_SELECTED:
      case MASTER_OBJPOINT_SELECTED:
        FdSelector::getSelectedObjects(selectedObjects);
        pickedObject = FdPickFilter::getInterestingPObj(&evHaAction->getPickedPointList(),
                                                        selectedObjects, // This is to select objects behind the already selected one
                                                        {},false, // No variables filtering
                                                        indexToInterestingPP,wasASelectedObjInPPList); // Variables returning values
        if (pickedObject)
        {
          // The user picked something
          FapEventManager::permUnselectAll();
          FapEventManager::permSelect(pickedObject->getFmOwner());

          // Select path to object
          SoPickedPoint* interestingPickedPoint = evHaAction->getPickedPointList()[indexToInterestingPP];
          SoPath* path = interestingPickedPoint->getPath();

          // Get a copy of the pick detail in the shape picked:
          SoNode* tail = ((SoFullPath*)path)->getTail();
          const SoDetail* pDet = interestingPickedPoint->getDetail(tail);
          SoDetail* pickDetail = pDet ? pDet->copy() : NULL;

          // Get hit point on object in object space:
          SbVec3f pointOnObject = interestingPickedPoint->getObjectPoint();
          const SbMatrix& objToWorld = interestingPickedPoint->getObjectToWorld(tail);
          FaVec3 createPoint = FdConverter::toFaVec3(pickedObject->findSnapPoint(pointOnObject,objToWorld,pickDetail));

          if (pickedObject->isOfType(FdTriad::getClassTypeID()))
          {
            ourSelectedTriad = static_cast<FmTriad*>(pickedObject->getFmOwner());
            createPoint = ourSelectedTriad->getGlobalTranslation();
          }
          else
            ourSelectedTriad = NULL;

          if (pickedObject->isOfType(FdPart::getClassTypeID()))
          {
            ourSelectedRefNode = false;
            ourSpiderPart = dynamic_cast<FmPart*>(pickedObject->getFmOwner());
            FdPickedPoints::removeFirstPP();
            static_cast<FdPart*>(pickedObject)->findNode(nodeID,worldNodePos,interestingPickedPoint->getPoint());
            FdPickedPoints::selectNode(0,nodeID,worldNodePos);
            FapCylinderSpiderCmds::setCylinderPoint(0,worldNodePos);
            FapCylinderSpiderCmds::setState(N_1_SELECTED);
          }
          else
          {
            ourSelectedRefNode = true;
            ourSpiderPart = NULL;
            ourGlobalRefNodePos = createPoint;
            FdPickedPoints::setFirstPP(createPoint,FdConverter::toFaMat34(objToWorld));
            FapCylinderSpiderCmds::setState(MASTER_OBJPOINT_SELECTED);
          }

          if (pickDetail)
            delete pickDetail;
        }
        else
          FapCylinderSpiderCmds::setState(START);
        break;

      case N_1_SELECTED_AFTER_MAST_OBJPOINT:
      case MASTER_OBJPOINT_ACCEPTED:
        if ((fdpart = FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,fdSpiderPart))) {
          if (!ourSpiderPart) ourSpiderPart = static_cast<FmPart*>(fdpart->getFmOwner());
          if (!FapEventManager::isPermSelected(ourSpiderPart))
            FapEventManager::permSelect(ourSpiderPart);
          FdPickedPoints::selectNode(0,nodeID,worldNodePos);
          FapCylinderSpiderCmds::setCylinderPoint(0,worldNodePos);
          FapCylinderSpiderCmds::setState(N_1_SELECTED_AFTER_MAST_OBJPOINT);
	}
        break;

      case N_1_ACCEPTED:
      case N_2_SELECTED:
        if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,fdSpiderPart)) {
          FdPickedPoints::selectNode(1,nodeID,worldNodePos);
          FapCylinderSpiderCmds::setCylinderPoint(1,worldNodePos);
          FapCylinderSpiderCmds::setState(N_2_SELECTED);
        }
        break;

      case N_2_ACCEPTED:
      case N_3_SELECTED:
        if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,fdSpiderPart)) {
          FdPickedPoints::selectNode(2,nodeID,worldNodePos);
          FapCylinderSpiderCmds::setCylinderPoint(2,worldNodePos);
          FapCylinderSpiderCmds::setState(N_3_SELECTED);
        }
        break;

      case N_3_ACCEPTED:
      case START_NODE_SELECTED:
        if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,fdSpiderPart)) {
          FdPickedPoints::selectNode(3,nodeID,worldNodePos);
          FapCylinderSpiderCmds::setCylinderPoint(3,worldNodePos);
          FapCylinderSpiderCmds::setState(START_NODE_SELECTED);
        }
        break;

      case START_NODE_ACCEPTED:
      case END_NODE_SELECTED:
        if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,fdSpiderPart)) {
          FdPickedPoints::selectNode(4,nodeID,worldNodePos);
          FapCylinderSpiderCmds::setCylinderPoint(4,worldNodePos);
          FapCylinderSpiderCmds::setState(END_NODE_SELECTED);
        }
        break;

      case END_NODE_ACCEPTED:
      case MASTER_PLANE_SELECTED:
        if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,fdSpiderPart)) {
          FdPickedPoints::selectNode(5,nodeID,worldNodePos);
          ourGlobalRefNodePos = worldNodePos;
          ourSelectedRefNode = true;
          FapCylinderSpiderCmds::setState(MASTER_PLANE_SELECTED);
        }
        break;
      }
  }

  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCallbackNode->setHandled();
}
#else
void FapCylinderSpiderCmds::eventCB(void*, SoEventCallback*) {}
#endif


void FapCylinderSpiderCmds::setCylinderPoint(size_t index, const FaVec3& posPt)
{
  if (ourCylinderPoints.size() <= index) ourCylinderPoints.resize(index+1);
  if (ourSpiderPart)
    ourCylinderPoints[index] = ourSpiderPart->getGlobalCS().inverse()*posPt;
  else
    ourCylinderPoints[index] = posPt;
}


void FapCylinderSpiderCmds::createSpider(bool doProjectRefNodePosToAxis)
{
  if (!ourSpiderPart || ourCylinderPoints.size() < 3)
    return;

  FmTriad* triad = ourSelectedTriad;
  char isUsable = ourSpiderPart->isTriadConnectable(triad);
  if (!isUsable)
    return;
  else if (isUsable == 2)
    triad = NULL; // Need a new triad

  if (ourSelectedRefNode)
  {
    FaVec3 refNodePos = ourSpiderPart->getGlobalCS().inverse()*ourGlobalRefNodePos;
    ourSpiderPart->createCylinderConnector(ourCylinderPoints,ourUseArcOnly,
                                           &refNodePos,doProjectRefNodePosToAxis,
                                           triad,ourSpiderType);
  }
  else
    ourSpiderPart->createCylinderConnector(ourCylinderPoints,ourUseArcOnly,NULL,false,
                                           triad,ourSpiderType);
}
