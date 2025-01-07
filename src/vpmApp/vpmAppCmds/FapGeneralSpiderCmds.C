// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FapGeneralSpiderCmds.H"
#include "vpmApp/FapEventManager.H"
#include "vpmApp/vpmAppUAMap/FapUAModeller.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "vpmUI/FuiModes.H"
#include "vpmUI/Fui.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmPart.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFlLib/FFlLinkHandler.H"
#include "FFlLib/FFlVertex.H"
#include "FFlLib/FFlFEParts/FFlNode.H"
#include "FFaLib/FFaGeometry/FFaPointSetGeometry.H"

#ifdef USE_INVENTOR
#include "vpmDisplay/FdFEGroupPartKit.H"
#include "vpmDisplay/FdTriad.H"
#include "vpmDisplay/FdPart.H"
#include "vpmDisplay/FdSelector.H"
#include "vpmDisplay/FdPickFilter.H"
#include "vpmDisplay/FdPickedPoints.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDisplay/FdEvent.H"
#include "vpmDisplay/FdDB.H"

#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoExtSelection.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/details/SoLineDetail.h>
#include <Inventor/details/SoFaceDetail.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/SbColor.h>
#include <Inventor/SbVec2s.h>
#include <Inventor/SbViewVolume.h>
#endif


enum {
  START,
  MASTER_SELECTED,
  MASTER_ACCEPTED,
  VALID_SLAVE_NODES_SELECTED
};

int FapGeneralSpiderCmds::ourState = START;
int FapGeneralSpiderCmds::ourSpiderType = FmTriad::NONE;

FmPart*  FapGeneralSpiderCmds::ourSpiderPart = NULL;
FmTriad* FapGeneralSpiderCmds::ourSelectedTriad = NULL;
std::map<int,FaVec3> FapGeneralSpiderCmds::ourWindowedNodes;
int FapGeneralSpiderCmds::ourSelectionType = 1;
bool FapGeneralSpiderCmds::weMustInitWindowVolume = true;
bool FapGeneralSpiderCmds::weUseWindowSelection = false;

#ifdef USE_INVENTOR
static SbVec2s ourMouseDownPos;
static SbVec2s ourMouseReleasePos;
#endif


void FapGeneralSpiderCmds::init()
{
  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_createRBE2GeneralSpider");
  cmdItem->setSmallIcon(spider_gen_rbe2_xpm);
  cmdItem->setText("By selecting nodes");
  cmdItem->setToolTip("Create a rigid connection element by selecting nodes");
  cmdItem->setActivatedCB(FFaDynCB0S(FapGeneralSpiderCmds::createRBE2Spider));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));

  cmdItem = new FFuaCmdItem("cmdId_createRBE3GeneralSpider");
  cmdItem->setSmallIcon(spider_gen_rbe3_xpm);
  cmdItem->setText("By selecting nodes");
  cmdItem->setToolTip("Create a flexible connection element by selecting nodes");
  cmdItem->setActivatedCB(FFaDynCB0S(FapGeneralSpiderCmds::createRBE3Spider));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerEditable,bool&));
}


void FapGeneralSpiderCmds::createRBE2Spider()
{
  FapGeneralSpiderCmds::ourSpiderType = FmTriad::RIGID;
  FuiModes::setMode(FuiModes::MAKEGENERALSPIDER_MODE);
}


void FapGeneralSpiderCmds::createRBE3Spider()
{
  FapGeneralSpiderCmds::ourSpiderType = FmTriad::AVERAGE_MOTION;
  FuiModes::setMode(FuiModes::MAKEGENERALSPIDER_MODE);
}


void FapGeneralSpiderCmds::enterMode()
{
  FapEventManager::permUnselectAll();
#ifdef USE_INVENTOR
  FdEvent::addEventCB(FapGeneralSpiderCmds::eventCB);

  FdDB::getExtSelectionRoot()->setTriangleFilterCallback(FapGeneralSpiderCmds::mSoExtSelectionTriangleCB);
  FdDB::getExtSelectionRoot()->setLineSegmentFilterCallback(FapGeneralSpiderCmds::mSoExtSelectionLineSegmentCB);
  FdDB::getExtSelectionRoot()->addFinishCallback(FapGeneralSpiderCmds::windowSelectionFinishedCB);

  FdDB::getExtSelectionRoot()->lassoType = SoExtSelection::NOLASSO;
  FdDB::getExtSelectionRoot()->lassoPolicy = SoExtSelection::PART;
  FdDB::getExtSelectionRoot()->lassoMode = SoExtSelection::VISIBLE_SHAPES;
  FdDB::getExtSelectionRoot()->setLassoColor(SbColor(0.9f,0.7f,0.2f));
  FdDB::getExtSelectionRoot()->setLassoWidth(1);
  FdDB::getExtSelectionRoot()->setOverlayLassoPattern(0xf0f0);  // 0011 0011 0011 0011
  FdDB::getExtSelectionRoot()->animateOverlayLasso(false);
#endif
  FapUAModeller::updateMode();
  FapGeneralSpiderCmds::setState(START);
}


void FapGeneralSpiderCmds::cancelMode()
{
  FapEventManager::permUnselectAll();
#ifdef USE_INVENTOR
  FdPickedPoints::clearNodeSet();
  FdPickedPoints::resetPPs();
  FdEvent::removeEventCB(FapGeneralSpiderCmds::eventCB);

  FdDB::getExtSelectionRoot()->lassoType = SoExtSelection::NOLASSO;
  FdDB::getExtSelectionRoot()->setTriangleFilterCallback(0);
  FdDB::getExtSelectionRoot()->setLineSegmentFilterCallback(0);
  FdDB::getExtSelectionRoot()->removeFinishCallback(FapGeneralSpiderCmds::windowSelectionFinishedCB);
#endif
  FapGeneralSpiderCmds::ourState = START;
  FapGeneralSpiderCmds::ourSpiderPart = NULL;
}


void FapGeneralSpiderCmds::setState(int newState)
{
  switch (FapGeneralSpiderCmds::ourState = newState)
    {
    case START:
      FapEventManager::permUnselectAll();
      ourSpiderPart = NULL;
      ourSelectedTriad = NULL;
#ifdef USE_INVENTOR
      FdPickedPoints::clearNodeSet();
      FdPickedPoints::resetPPs();
      FdPickedPoints::setFirstPP(FaVec3(),FaMat34());
      FdDB::getExtSelectionRoot()->lassoType = SoExtSelection::NOLASSO;
#endif
      Fui::tip("Select the position of the reference node,\n"
               "OR the object to attach with the connector");
      FapUAModeller::updateState(0);
      break;
    case MASTER_SELECTED:
      Fui::tip("Accept by pressing Done,\n"
               "OR pick again to reselect.");
      FapUAModeller::updateState(0);
      break;
    case MASTER_ACCEPTED:
      Fui::tip("Select FE nodes by picking or dragging a window.\n"
               "Press and hold the CTRL-key to remove nodes from selection");
#ifdef USE_INVENTOR
      FdDB::getExtSelectionRoot()->lassoType = SoExtSelection::RECTANGLE;
#endif
      ourWindowedNodes.clear();
      break;
    case VALID_SLAVE_NODES_SELECTED:
      Fui::tip("Select FE nodes by picking or dragging a window.\n"
               "Press and hold the CTRL-key to remove nodes from selection");
      break;
    }
}


void FapGeneralSpiderCmds::done()
{
  std::vector<FdNode> selectedNodes;

  switch (FapGeneralSpiderCmds::ourState)
    {
#ifdef USE_INVENTOR
    case START:
      if (FdPickedPoints::hasPickedPoints()) {
        FapUAModeller::updateState(1);
        FapGeneralSpiderCmds::setState(MASTER_ACCEPTED);
      }
      else
        FuiModes::cancel();
      break;
#endif
    case MASTER_SELECTED:
      FapUAModeller::updateState(1);
      FapGeneralSpiderCmds::setState(MASTER_ACCEPTED);
      break;
    case MASTER_ACCEPTED:
      break;
    case VALID_SLAVE_NODES_SELECTED:
#ifdef USE_INVENTOR
      FdPickedPoints::getNodeSet(selectedNodes);
#endif
      FapEventManager::permUnselectAll();
      FapGeneralSpiderCmds::createSpider(selectedNodes);
      FapGeneralSpiderCmds::setState(START);
      break;
    default:
      FuiModes::cancel();
      break;
    }
}


void FapGeneralSpiderCmds::eventCB(void*, SoEventCallback* eventCallbackNode)
{
  if (!eventCallbackNode) return;
#ifdef USE_INVENTOR
  const SoEvent* event = eventCallbackNode->getEvent();
  if (!event) return;
  if (!event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

  SoHandleEventAction* evHaAction = eventCallbackNode->getAction();
  const SoPickedPointList& ppl = evHaAction->getPickedPointList();

  if (SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    const SoPickedPointList& ppl = evHaAction->getPickedPointList();

    std::vector<FdObject*> selectedObjects;
    long indexToInterestingPP = -1;
    bool wasASelectedObjInPPList = false;
    FdObject* pickedObject = NULL;
    FdPart* fdpart = NULL;
    int i;

    switch (FapGeneralSpiderCmds::ourState)
      {
      case START:
      case MASTER_SELECTED:

        // Select/reselect some arbitrary 3D point, or select a triad.
        FdSelector::getSelectedObjects(selectedObjects);
        pickedObject = FdPickFilter::getInterestingPObj(&evHaAction->getPickedPointList(),
                                                        selectedObjects, // This is to select objects behind the already selected one
                                                        std::vector<int>(),false, // No variables filtering
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
          FaVec3 createPoint = FdConverter::toFaVec3(pickedObject->findSnapPoint(pointOnObject,objToWorld,pickDetail,interestingPickedPoint));

          if (pickedObject->isOfType(FdTriad::getClassTypeID()))
          {
            ourSelectedTriad = static_cast<FmTriad*>(pickedObject->getFmOwner());
            createPoint = ourSelectedTriad->getGlobalTranslation();
          }

          FdPickedPoints::setFirstPP(createPoint,FdConverter::toFaMat34(objToWorld));
          FapGeneralSpiderCmds::setState(MASTER_SELECTED);

          if (pickDetail)
            delete pickDetail;
        }
        else
          FapGeneralSpiderCmds::setState(START);

        eventCallbackNode->setHandled();
        break;

      case MASTER_ACCEPTED:
      case VALID_SLAVE_NODES_SELECTED:

        // Store the mouse down position
        ourMouseDownPos = event->getPosition();

        // Find the part to add elements to
        FdObject* fdSpiderPart = ourSpiderPart ? ourSpiderPart->getFdPointer() : NULL;
        if ((fdpart = FdPickFilter::findFirstPartHit(i,ppl,fdSpiderPart)))
          ourSpiderPart = dynamic_cast<FmPart*>(fdpart->getFmOwner());
        break;
      }
  }
  else if (SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    SbVec2s mouseMotion;
    weMustInitWindowVolume = true;
    switch (FapGeneralSpiderCmds::ourState)
      {
      case START:
      case MASTER_SELECTED:
        // Just handle the event to avoid the selection node to do it while selecting reference node
        eventCallbackNode->setHandled();
        break;

      case MASTER_ACCEPTED:
      case VALID_SLAVE_NODES_SELECTED:
        // Toggle-, or add-select the closest node if the mouse down
        // and the mouse up positions are close
        if (event->wasCtrlDown())
          ourSelectionType = FdPickedPoints::REMOVE_SELECT;
        else
          ourSelectionType = FdPickedPoints::ADD_SELECT;

        // Store the mouse down position for extra window filtering
        ourMouseReleasePos = event->getPosition();
	mouseMotion = ourMouseReleasePos - ourMouseDownPos;
	weUseWindowSelection = ((abs(mouseMotion[0]) > 2) || (abs(mouseMotion[1]) > 2));

        if (!weUseWindowSelection)
        {
          int    nodeID = -1;
          FaVec3 worldNodePos;
          FdDB::getExtSelectionRoot()->setLassoFilterCallback(0,0,0);
          FdObject* fdSpiderPart = ourSpiderPart ? ourSpiderPart->getFdPointer() : NULL;
          if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,fdSpiderPart))
            // Toggle-select node if shift was not down, and highlight it.
            FdPickedPoints::selectNodeSet(nodeID,worldNodePos,ourSelectionType);

          // Then if we have nodes selected
          if (FdPickedPoints::hasNodeSet()) {
            if (!FapEventManager::isPermSelected(ourSpiderPart))
              FapEventManager::permSelect(ourSpiderPart);
            FapGeneralSpiderCmds::setState(VALID_SLAVE_NODES_SELECTED);
          }
          else {
            FapEventManager::permUnselect(ourSpiderPart);
            ourSpiderPart = NULL;
            FapGeneralSpiderCmds::setState(MASTER_ACCEPTED);
          }
        }
        else
          // The SoExtSelection have to handle it. Mainly the finishcallback
          weMustInitWindowVolume = true;
        break;
      }
  }
#endif
}


/*!
  This callback is received for each triangle that the SoExtSelection node is
  inside the window selected. That means crosses or is inside the window border.

  We find the node for each vertex and store them in the ourWindowedNodes map.
  We also create a viewVolume that represents the window volume,
  to be able to take only what is really inside the window.
*/

int FapGeneralSpiderCmds::mSoExtSelectionTriangleCB(void* userdata,
                                                    SoCallbackAction* action,
                                                    const SoPrimitiveVertex* v1,
                                                    const SoPrimitiveVertex* v2,
                                                    const SoPrimitiveVertex*)
{
  return FapGeneralSpiderCmds::windowSelectPrimitiveCB(userdata, action, v1, v2);
}


int FapGeneralSpiderCmds::mSoExtSelectionLineSegmentCB(void* userdata,
                                                       SoCallbackAction* action,
                                                       const SoPrimitiveVertex* v1,
                                                       const SoPrimitiveVertex* v2)
{
  return FapGeneralSpiderCmds::windowSelectPrimitiveCB(userdata, action, v1, v2);
}


bool FapGeneralSpiderCmds::windowSelectPrimitiveCB(void*,
                                                   SoCallbackAction* action,
                                                   const SoPrimitiveVertex* v1,
                                                   const SoPrimitiveVertex* v2)
{
  // Just return and stop calling this if things are really wrong.
  if (!weUseWindowSelection || !action)
    return true;

  // If the part to attach to has no link handler, we can't proceed.
  // Try to find a part with a link handler
  if (ourSpiderPart && !ourSpiderPart->getLinkHandler())
    ourSpiderPart = NULL;

#ifdef USE_INVENTOR
  // Find the part to attach to, if not specified,
  // or make sure that the primitive are from the correct part
  const SoPath* path = action->getCurPath();
  const SoFullPath* fullPath = static_cast<const SoFullPath*>(path);

  static SoNode* goodNode = NULL;

  // Reset goodpath if we have a new window selection,
  // to avoid accepting paths from old command sessions.
  if (weMustInitWindowVolume)
    goodNode = NULL;

  bool isFound = false;
  if (!ourSpiderPart)
  {
    FdObject* fdObj = FdPickFilter::findFdObject((SoPath*)path);
    FmPart* fmpart = fdObj ? dynamic_cast<FmPart*>(fdObj->getFmOwner()) : NULL;
    if (fmpart && fmpart->getLinkHandler())
    {
      int pathLength = fullPath->getLength();
      for (int i = 0; i < pathLength; i++)
        if (fullPath->getNodeFromTail(i)->isOfType(FdFEGroupPartKit::getClassTypeId())) {
	  isFound = true;
	  ourSpiderPart = fmpart;
	  goodNode = fullPath->getTail();
          break;
        }
    }
  }
  else if (fullPath->getTail() == goodNode)
    isFound = true;
  else
  {
    FdObject* fdObj = FdPickFilter::findFdObject((SoPath*)path);
    if (fdObj && fdObj->getFmOwner() == ourSpiderPart)
    {
      int pathLength = fullPath->getLength();
      for (int i = 0; i < pathLength; i++)
	if (fullPath->getNodeFromTail(i)->isOfType(FdFEGroupPartKit::getClassTypeId()))
	{
	  isFound = true;
	  goodNode = fullPath->getTail();
	  break;
	}
    }
  }

  if (!isFound) return false;

  // Initialize the window view volume to be used for extra filtering.
  // Also initialize a vector to accumulate the selected vxes

  static SbViewVolume windowVolume;
  static std::vector<bool> visitedVertexes;
  static FaMat34 partCS;

  if (weMustInitWindowVolume)
    {
      SbViewVolume vv = action->getViewVolume();
      SbViewportRegion vp = action->getViewportRegion();

      SbVec2s p = ourMouseDownPos - vp.getViewportOriginPixels();
      SbVec2s s = vp.getViewportSizePixels();

      SbVec2f mouseDown((float)p[0]/(float)s[0], (float)p[1]/(float)s[1]);
      p = ourMouseReleasePos - vp.getViewportOriginPixels();
      SbVec2f mouseUp((float)p[0]/(float)s[0], (float)p[1]/(float)s[1]);

      float left   = mouseDown[0] < mouseUp[0] ? mouseDown[0] : mouseUp[0];
      float right  = mouseDown[0] > mouseUp[0] ? mouseDown[0] : mouseUp[0];
      float bottom = mouseDown[1] < mouseUp[1] ? mouseDown[1] : mouseUp[1];
      float top    = mouseDown[1] > mouseUp[1] ? mouseDown[1] : mouseUp[1];

      windowVolume = vv.narrow(left, bottom, right, top);

      std::vector<bool> empty;
      visitedVertexes.swap(empty);
      visitedVertexes.resize(ourSpiderPart->getLinkHandler()->getVertexCount(), false);

      partCS = ourSpiderPart->getGlobalCS();
      weMustInitWindowVolume = false;
    }

  // Do the actual search for node and add to preliminary selection.

  const SoDetail* det = v1->getDetail();
  IntVec indexes;

  if (det && det->isOfType(SoFaceDetail::getClassTypeId()))
  {
    const SoFaceDetail* faceDet = static_cast<const SoFaceDetail*>(det);
    int numPoints = faceDet->getNumPoints();
    indexes.reserve(numPoints);
    for (int pIdx = 0; pIdx < numPoints; pIdx++)
      indexes.push_back(faceDet->getPoint(pIdx)->getCoordinateIndex());
  }
  else if (det && det->isOfType(SoLineDetail::getClassTypeId()))
  {
    const SoLineDetail* lineDet = static_cast<const SoLineDetail*>(det);
    indexes.push_back(lineDet->getPoint0()->getCoordinateIndex());
    indexes.push_back(lineDet->getPoint1()->getCoordinateIndex());
  }
  else if (det && det->isOfType(SoPointDetail::getClassTypeId()))
  {
    const SoPointDetail* pointDet = static_cast<const SoPointDetail*>(det);
    indexes.push_back(pointDet->getCoordinateIndex());
    if (v2 && v2->getDetail()->isOfType(SoPointDetail::getClassTypeId()))
    {
      pointDet = static_cast<const SoPointDetail*>(v2->getDetail());
      indexes.push_back(pointDet->getCoordinateIndex());
    }
  }

  for (int idx : indexes)
    if (idx < (int)visitedVertexes.size() && !visitedVertexes[idx])
    {
      visitedVertexes[idx] = true;
      FFlVertex* fflVx = static_cast<FFlVertex*>(ourSpiderPart->getLinkHandler()->getVertexes()[idx]);
      if (fflVx && fflVx->getNode()) {
        FaVec3 worldPos = partCS * (*fflVx);
        if (windowVolume.intersect(FdConverter::toSbVec3f(worldPos)))
          ourWindowedNodes[fflVx->getNode()->getID()] = worldPos;
      }
    }

#else
  if (v1 || v2) // Dummy statement to suppress compiler warning
    std::cout <<"FapGeneralSpiderCmds::windowSelectPrimitiveCB() dummy"<< std::endl;
#endif

  return false;
}


void FapGeneralSpiderCmds::windowSelectionFinishedCB(void*, SoSelection*)
{
  // If we did not have a good window, exit.
  if (!weUseWindowSelection) return;

#ifdef USE_INVENTOR
  // Insert the complete selection into the point selector.
  // Toggle or add as prescribed.
  for (const std::pair<const int,FaVec3>& node : ourWindowedNodes)
    FdPickedPoints::selectNodeSet(node.first, node.second, ourSelectionType);

  ourWindowedNodes.clear();

  // Then if we have nodes selected
  if (FdPickedPoints::hasNodeSet()) {
    if (!FapEventManager::isPermSelected(ourSpiderPart))
      FapEventManager::permSelect(ourSpiderPart);
    FapGeneralSpiderCmds::setState(VALID_SLAVE_NODES_SELECTED);
  }
  else {
   FapEventManager::permUnselect(ourSpiderPart);
   ourSpiderPart = NULL;
   FapGeneralSpiderCmds::setState(MASTER_ACCEPTED);
  }
#endif

  weMustInitWindowVolume = true;
}


void FapGeneralSpiderCmds::createSpider(const std::vector<FdNode>& selectedNodes)
{
  if (!ourSpiderPart || selectedNodes.empty())
    return;

  char isUsable = ourSpiderPart->isTriadConnectable(ourSelectedTriad);
  if (!isUsable) return;

  // Put the selected nodal points into a FFaCompoundGeometry
  FFaPointSetGeometry pointsGeom;
  FaMat34 xfInv = ourSpiderPart->getGlobalCS().inverse();
  for (const FdNode& node : selectedNodes)
    pointsGeom.addPoint(xfInv*node.second);
  FFaCompoundGeometry geometry(ourSpiderPart->getConnectorTolerance());
  geometry.addGeometry(pointsGeom);

  // Position of the spider reference node
#ifdef USE_INVENTOR
  FaVec3 nodePos = xfInv*FdPickedPoints::getFirstPickedPoint();
#else
  FaVec3 nodePos = xfInv[VW];
#endif

  // Create the spider element
  ourSpiderPart->createConnector(geometry,nodePos,ourSpiderPart->getOrientation(),
                                 isUsable == 2 ? NULL : ourSelectedTriad,
                                 static_cast<FmTriad::ConnectorType>(ourSpiderType));
}
