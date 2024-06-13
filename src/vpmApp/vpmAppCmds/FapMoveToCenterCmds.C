// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapMoveToCenterCmds.H"
#include "vpmApp/vpmAppCmds/FapAllignCSCmds.H"
#include "vpmApp/FapEventManager.H"
#include "vpmApp/vpmAppUAMap/FapUAModeller.H"
#include "vpmUI/Fui.H"
#include "vpmUI/FuiModes.H"
#include "vpmDB/FmTriad.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFaLib/FFaAlgebra/FFaMat34.H"
#include "FFaLib/FFaGeometry/FFaCylinderGeometry.H"

#ifdef USE_INVENTOR
#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/details/SoDetail.h>

#include "vpmDisplay/FdExtraGraphics.H"
#include "vpmDisplay/FdPickFilter.H"
#include "vpmDisplay/FdPickedPoints.H"
#include "vpmDisplay/FdSelector.H"
#include "vpmDisplay/FdTriad.H"
#include "vpmDisplay/FdLink.H"
#include "vpmDisplay/FdSimpleJoint.H"
#include "vpmDisplay/FdFreeJoint.H"
#include "vpmDisplay/FdRefPlane.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDisplay/FdEvent.H"
#endif

#include "vpmUI/Icons/MoveToCenter.xpm"


enum {
  START,
  OBJECT_SELECTED,
  OBJECT_ACCEPTED,
  P_1_SELECTED,
  P_1_ACCEPTED,
  P_2_SELECTED,
  P_2_ACCEPTED,
  P_3_SELECTED,
  P_3_ACCEPTED,
  AXISPOS_SELECTED,
  AXISPOS_ACCEPTED,
};


int FapMoveToCenterCmds::ourState = START;
std::vector<FaVec3> FapMoveToCenterCmds::ourCirclePoints;
int FapMoveToCenterCmds::ourNumSelectedToMove = 0;


void FapMoveToCenterCmds::init()
{
  FFuaCmdItem* cmdItem = new FFuaCmdItem("cmdId_moveToCenter");
  cmdItem->setSmallIcon(MoveToCenter_xpm);
  cmdItem->setText("Move To Center");
  cmdItem->setToolTip("Move an object to the center of a 3 point circle");
  cmdItem->setActivatedCB(FFaDynCB0S(FapMoveToCenterCmds::moveToCenter));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapMoveToCenterCmds::getSensitivity,bool&));
}


void FapMoveToCenterCmds::moveToCenter()
{
  FuiModes::setMode(FuiModes::MOVE_TO_CENTER_MODE);
}


void FapMoveToCenterCmds::enterMode()
{
  FapEventManager::permUnselectAll();
  ourCirclePoints.clear();
  ourNumSelectedToMove = 0;
#ifdef USE_INVENTOR
  FdEvent::addEventCB(FapMoveToCenterCmds::eventCB);
#endif
  FapMoveToCenterCmds::setState(START);
}


void FapMoveToCenterCmds::cancelMode()
{
  FapEventManager::permUnselectAll();
#ifdef USE_INVENTOR
  FdPickedPoints::resetPPs();
  FdExtraGraphics::hideCylinder();
  FdEvent::removeEventCB(FapMoveToCenterCmds::eventCB);
#endif
  ourCirclePoints.clear();
  FapMoveToCenterCmds::ourState = START;
}


void FapMoveToCenterCmds::setState(int newState)
{
  switch (FapMoveToCenterCmds::ourState = newState)
    {
    case START:
      Fui::tip("Select the object to move");
      FapEventManager::permUnselectAll();
#ifdef USE_INVENTOR
      FdPickedPoints::resetPPs();
      FdExtraGraphics::hideCylinder();
#endif
      ourCirclePoints.clear();
      FapUAModeller::showPointUI(false);
      ourNumSelectedToMove = 0;
      break;
    case OBJECT_SELECTED:
      Fui::tip("Accept by pressing Done,\n"
               "or pick again to reselect the object to move");
      break;
    case OBJECT_ACCEPTED:
      ourNumSelectedToMove = FapEventManager::getNumPermSelected();
      Fui::tip("Select the first of three points on the circle circumference");
      FapUAModeller::setPointIdx(0);
      FapUAModeller::setGlobalOnly();
      FapUAModeller::updatePointUI();
      FapUAModeller::showPointUI(true);
      break;
    case P_1_SELECTED:
      Fui::tip("Accept by pressing Done,\n"
               "or pick again to reselect first circle point.");
      FapUAModeller::showPointUI(true);
      FapUAModeller::setLocal();
      FapUAModeller::updatePointUI();
      break;
    case P_1_ACCEPTED:
      Fui::tip("Select second point at the circle circumference.");
      FapUAModeller::setGlobalOnly();
      FapUAModeller::setPointIdx(1);
      FapUAModeller::updatePointUI();
      FapUAModeller::showPointUI(true);
      break;
    case P_2_SELECTED:
      Fui::tip("Accept by pressing Done,\n"
               "or pick again to reselect the second circle point");
      FapUAModeller::setLocal();
      FapUAModeller::updatePointUI();
      FapUAModeller::showPointUI(true);
      break;
    case P_2_ACCEPTED:
      Fui::tip("Select third point at the circle circumference");
      FapUAModeller::setPointIdx(2);
      FapUAModeller::setGlobalOnly();
      FapUAModeller::updatePointUI();
      FapUAModeller::showPointUI(true);
      break;
    case P_3_SELECTED:
      Fui::tip("Accept by pressing Done,\n"
               "or pick again to reselect the third circle point");
      FapUAModeller::setLocal();
      FapUAModeller::updatePointUI();
      FapUAModeller::showPointUI(true);
#ifdef USE_INVENTOR
      FdPickedPoints::getAllPickedPointsGlobal(ourCirclePoints);
      FdExtraGraphics::showCylinder(FaMat34(),ourCirclePoints);
#endif
      break;
    case P_3_ACCEPTED:
#ifdef USE_INVENTOR
      FdPickedPoints::getAllPickedPointsGlobal(ourCirclePoints);
      FdExtraGraphics::showCylinder(FaMat34(),ourCirclePoints);
#endif
      Fui::tip("Accept position by pressing Done,\n"
               "or select a point to place the object along the circle axis");
      FapUAModeller::setPointIdx(3);
      FapUAModeller::showPointUI(false);
      break;
    case AXISPOS_SELECTED:
      Fui::tip("Accept by pressing Done,\n"
               "or pick again to reselect the position along the axis");
      FapUAModeller::setLocal();
      FapUAModeller::updatePointUI();
      FapUAModeller::showPointUI(true);
      break;
    }
}


void FapMoveToCenterCmds::done()
{
  switch (FapMoveToCenterCmds::ourState)
    {
    case START:
      FuiModes::cancel();
      break;
    case OBJECT_SELECTED:
      FapMoveToCenterCmds::setState(OBJECT_ACCEPTED);
      break;
    case P_1_SELECTED:
      FapMoveToCenterCmds::setState(P_1_ACCEPTED);
      break;
    case P_1_ACCEPTED:
      break;
    case P_2_SELECTED:
      FapMoveToCenterCmds::setState(P_2_ACCEPTED);
      break;
    case P_2_ACCEPTED:
      break;
    case P_3_SELECTED:
      FapMoveToCenterCmds::setState(P_3_ACCEPTED);
      break;
    case P_3_ACCEPTED:
    case AXISPOS_SELECTED:
#ifdef USE_INVENTOR
      FdPickedPoints::getAllPickedPointsGlobal(ourCirclePoints);
#endif
      if (FapEventManager::getNumPermSelected() > ourNumSelectedToMove)
        FapEventManager::permUnselectLast();
      FapMoveToCenterCmds::moveToCenter(ourCirclePoints);
      FapMoveToCenterCmds::setState(START);
      break;
    default:
      FuiModes::cancel();
      break;
    }
}


void FapMoveToCenterCmds::getSensitivity(bool& isSensitive)
{
  FapCmdsBase::isModelEditable(isSensitive);
  if (!isSensitive) return;

  FapCmdsBase::isModellerActive(isSensitive);
}


#ifdef USE_INVENTOR
void FapMoveToCenterCmds::eventCB(void*, SoEventCallback* eventCallbackNode)
{
  const SoEvent* event = eventCallbackNode->getEvent();
  if (!event) return;

  if (event->isOfType(SoMouseButtonEvent::getClassTypeId()) &&
      SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    int state = FapMoveToCenterCmds::ourState;
    SoHandleEventAction* evHaAction = eventCallbackNode->getAction();

    // Build array of selected objects:
    std::vector<FdObject*> selectedObjects;
    FdSelector::getSelectedObjects(selectedObjects);

    std::vector<int> types;
    bool typesIsInteresting = state <= OBJECT_SELECTED;
    if (typesIsInteresting) // Build array of interesting types
      types = {
        FdLink::getClassTypeID(),
        FdTriad::getClassTypeID(),
        FdSimpleJoint::getClassTypeID(),
        FdFreeJoint::getClassTypeID(),
        FdRefPlane::getClassTypeID()
      };

    long int  indexToInterestingPP    = -1;
    bool      wasASelectedObjInPPList = false;
    FdObject* pickedObject = FdPickFilter::getInterestingPObj(&evHaAction->getPickedPointList(),
                                                              selectedObjects, // This is to select objects behind the already selected one
                                                              types,typesIsInteresting, // Filter variables
                                                              indexToInterestingPP,wasASelectedObjInPPList); // Variables returning values

    if (state <= OBJECT_SELECTED)
    {
      if (!event->wasCtrlDown())
        FapEventManager::permUnselectLast();

      // Filter out triads in linear joints, moving them would mess up too much
      if (!pickedObject)
        FapMoveToCenterCmds::setState(START);
      else if (pickedObject->isOfType(FdTriad::getClassTypeID()) &&
               ((FmTriad*)pickedObject->getFmOwner())->isInLinJoint())
        FapMoveToCenterCmds::setState(START);
      else
      {
        // Deselect previously selected instance
        // to put it as the last one selected
        if (FapEventManager::isPermSelected(pickedObject->getFmOwner()))
          FapEventManager::permUnselect(pickedObject->getFmOwner());
        FapEventManager::permSelect(pickedObject->getFmOwner());
        FapMoveToCenterCmds::setState(OBJECT_SELECTED);
      }
    }
    else if (pickedObject)
    {
      SoPickedPoint* interestingPickedPoint = evHaAction->getPickedPointList()[indexToInterestingPP];
      SoPath* path = interestingPickedPoint->getPath();
      SoNode* tail = ((SoFullPath*)path)->getTail();
      const SoDetail* pDet = interestingPickedPoint->getDetail(tail);
      SoDetail* pickDetail = pDet ? pDet->copy() : NULL;

      SbVec3f pointOnObject = interestingPickedPoint->getObjectPoint();
      SbMatrix objToWorld = interestingPickedPoint->getObjectToWorld(tail);

      if (FapEventManager::getNumPermSelected() > ourNumSelectedToMove)
        FapEventManager::permUnselectLast();
      FapEventManager::permSelect(pickedObject->getFmOwner());

      FaVec3 worldPoint = FdConverter::toFaVec3(pickedObject->findSnapPoint(pointOnObject,objToWorld,pickDetail,interestingPickedPoint));
      delete pickDetail;

      unsigned int pointIdx = 0;
      int newState = P_1_SELECTED;
      if (state == P_1_ACCEPTED || state == P_2_SELECTED)
      {
        pointIdx = 1;
        newState = P_2_SELECTED;
      }
      else if (state == P_2_ACCEPTED || state == P_3_SELECTED)
      {
        pointIdx = 2;
        newState = P_3_SELECTED;
      }
      else if (state == P_3_ACCEPTED || state == AXISPOS_SELECTED)
      {
        pointIdx = 3;
        newState = AXISPOS_SELECTED;
      }

      FdPickedPoints::setPP(pointIdx,worldPoint,FdConverter::toFaMat34(objToWorld));
      if (ourCirclePoints.size() <= pointIdx)
        ourCirclePoints.resize(pointIdx+1);
      ourCirclePoints[pointIdx] = worldPoint;
      FapMoveToCenterCmds::setState(newState);
    }
  }
  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCallbackNode->setHandled();
}
#else
void FapMoveToCenterCmds::eventCB(void*, SoEventCallback*) {}
#endif


void FapMoveToCenterCmds::moveToCenter(std::vector<FaVec3> cylinderPoints)
{
  if (cylinderPoints.size() > 3)
  {
    FaVec3 globAxisPosPoint = cylinderPoints.back();
    cylinderPoints.pop_back();
    FFaCylinderGeometry cylinder(cylinderPoints,false);
    FaMat34 mx = cylinder.getTransMatrix();
    FaVec3 newGlobPoint = mx.inverse()*globAxisPosPoint;
    newGlobPoint[0] = newGlobPoint[1] = 0.0;
    mx[3] = mx*newGlobPoint;
    FapAllignCSCmds::allignCS(mx);
  }
  else
  {
    FFaCylinderGeometry cylinder(cylinderPoints,false);
    FapAllignCSCmds::allignCS(cylinder.getTransMatrix());
  }
}
