// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdEvent.H"
#include "vpmDisplay/FdPickFilter.H"
#include "vpmDisplay/FdPickedPoints.H"
#include "vpmDisplay/FdRefPlane.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/qtViewers/FdQtViewer.H"
#include "vpmDB/FmIsRenderedBase.H"
#include "vpmUI/FuiModes.H"
#include "vpmApp/FapEventManager.H"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>


namespace
{
  SoEventCallback* adminCallbackNode = NULL;
  SoEventCallback* eventCallbackNode = NULL;


  /*!
    This event callback handles the MB2 "done" and MB3 "cancel" events.
    It calls FuiModes::done() and FuiModes::cancel() which call
    FdDB::updateState() and FdDB::cancel() to set up the right callbacks
    (and so on) in FdDB.
  */

  void adminCB(void*, SoEventCallback* eventCB)
  {
    const SoEvent* event = eventCB->getEvent();
    if (!event) return;

    if (event->isOfType(SoMouseButtonEvent::getClassTypeId()))
    {
      if (SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON3))
        FuiModes::done();
      else if (SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON2))
        FuiModes::cancel();
      else
	return;
    }
    else if (SoKeyboardEvent::isKeyReleaseEvent(event,SoKeyboardEvent::ENTER))
      FuiModes::done();
    else if (SoKeyboardEvent::isKeyReleaseEvent(event,SoKeyboardEvent::RETURN))
      FuiModes::done();
    else if (SoKeyboardEvent::isKeyReleaseEvent(event,SoKeyboardEvent::ESCAPE))
      FuiModes::cancel();
    else
      return;

    eventCB->setHandled();
  }


  /*!
    This event callback demands the selection root
    to have the correct callbacks set up previously.
  */

  void examCB(void*, SoEventCallback* eventCB)
  {
    const SoEvent* event = eventCB->getEvent();
    if (!event) return;

    if (!event->isOfType(SoMouseButtonEvent::getClassTypeId()))
      return;

    if (!SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
    {
      eventCB->setHandled();
      return;
    }

    SoHandleEventAction* evHaAction = eventCB->getAction();

    // Build array of non-interesting types
    std::vector<int> types;
    if (FuiModes::getMode() == FuiModes::ERASE_MODE)
      types = { FdRefPlane::getClassTypeID() };

    long int  indexToInterestingPP = -1;
    FdObject* pickedObject = FdPickFilter::getCyceledInterestingPObj(&evHaAction->getPickedPointList(),
                                                                     types,false, // Filter variables
                                                                     indexToInterestingPP);

    if (!event->wasCtrlDown())
    {
      if (pickedObject)
        FapEventManager::permUnselectAll(); // Bugfix #381: Unselect everything else
      else
        FapEventManager::permUnselectLast();
    }
    else if (pickedObject)
      if (FapEventManager::isPermSelected(pickedObject->getFmOwner()))
      {
        FapEventManager::permUnselect(pickedObject->getFmOwner()); // Deselect previously selected instance
	pickedObject = NULL;
      }

    int nodeID = -1;
    if (pickedObject)
    {
      FaVec3 nodePos;
      if (FdPickFilter::findNodeHit(nodeID,nodePos,evHaAction->getPickedPointList(),pickedObject))
        FdPickedPoints::selectNode(0,nodeID,nodePos); // A FE node was hit, mark it in the viewer
      FapEventManager::permSelect(pickedObject->getFmOwner()); // Select the new instance
    }
    if (nodeID < 0)
      FdPickedPoints::deselectNode();

    int newState = FapEventManager::getNumPermSelected() < 1 ? 0 : 1;
    if (FuiModes::getState() != newState)
      FuiModes::setState(newState);

    eventCB->setHandled();
  }
}


void FdEvent::init()
{
  adminCallbackNode = new SoEventCallback;
  eventCallbackNode = new SoEventCallback;
}


void FdEvent::start(SoSeparator* dbRoot)
{
  adminCallbackNode->addEventCallback(SoEvent::getClassTypeId(),adminCB);
  eventCallbackNode->addEventCallback(SoEvent::getClassTypeId(),examCB);
  SoSeparator* superRoot = new SoSeparator;
  FdDB::getViewer()->setSceneGraph(superRoot);
  superRoot->addChild(adminCallbackNode);
  superRoot->addChild(eventCallbackNode);
  superRoot->addChild(dbRoot);
}


void FdEvent::addEventCB(SoEventCallbackCB* cb)
{
  eventCallbackNode->removeEventCallback(SoEvent::getClassTypeId(),examCB);
  eventCallbackNode->addEventCallback(SoEvent::getClassTypeId(),cb);
}


void FdEvent::removeEventCB(SoEventCallbackCB* cb)
{
  eventCallbackNode->removeEventCallback(SoEvent::getClassTypeId(),cb);
  eventCallbackNode->addEventCallback(SoEvent::getClassTypeId(),examCB);
}
