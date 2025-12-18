// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/qtViewers/FdQtViewer.H"

#include <Inventor/SbVec2f.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/details/SoDetail.h>
#include <Inventor/details/SoLineDetail.h>
#include <Inventor/details/SoNodeKitDetail.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/projectors/SbPlaneProjector.h>

#include "vpmDisplay/FdCtrlKit.H"
#include "vpmDisplay/FdCtrlElemKit.H"
#include "vpmDisplay/FdCtrlLineKit.H"
#include "vpmDisplay/FdCtrlSymbolKit.H"
#include "vpmDisplay/FdCtrlGridKit.H"

#include "vpmDisplay/FdCtrlDB.H"
#include "vpmDisplay/FdCtrlElement.H"
#include "vpmDisplay/FdCtrlGrid.H"
#include "vpmDisplay/FdCtrlLine.H"
#include "vpmDisplay/FdCtrlObject.H"
#include "vpmDisplay/FdCtrlSymDef.H"

#include "vpmDisplay/FdConverter.H"
#include "vpmDisplay/FdPickFilter.H"
#include "vpmDisplay/FdSelector.H"
#include "vpmApp/FapEventManager.H"

#include "vpmDB/FmGlobalViewSettings.H"
#include "vpmDB/FmControlAdmin.H"
#include "vpmDB/FmCtrlLine.H"
#include "vpmDB/FmCtrlElementBase.H"

#include "vpmUI/Fui.H"
#include "vpmUI/FuiCtrlModes.H"
#include "vpmUI/vpmUITopLevels/FuiCtrlModeller.H"
#include "FFuLib/FFuBase/FFuTopLevelShell.H"

#include <utility>


namespace
{
  // General variables
  FdQtViewer*  ctrlViewer = NULL;
  SoSeparator* ctrlRoot = NULL; // Top node in the ctrl scene graph
  SoSeparator* ourExtraGraphicsSep = NULL;
  FdCtrlKit*   ctrlKit = NULL;
  bool         cancel = false;

  // Callback nodes
  SoEventCallback* manipEventCbNode = NULL;
  SoEventCallback* adminEventCbNode = NULL;

  // CreateCB
  FdCtrlElement* newFdElement = NULL;
  bool isNewElemVisible = false;

  // SelectCB variables
  FdObject* pickedObject = NULL;
  int pickedElemDetail = FdCtrlElement::NONE;
  int pickedPortNumber = 0; // No Port
  int lineDetail = 0;

  // Line variables
  FdCtrlElement* startLineElem = NULL;
  FdCtrlElement* endLineElem = NULL;
  int point = 0;

  // Move group variable
  std::vector<FdCtrlElement*> selectedElements;
  std::vector<SbVec3f>        elemTransArray;


  /*!
    Function to select objects in the scene graph;
  */

  FdObject* pickObject(bool single, SoHandleEventAction* action)
  {
    std::vector<FdObject*> selectedObjects;
    FdSelector::getSelectedObjects(selectedObjects);

    long int indexToPitem = -1;
    bool     wasASelected = false;
    FdObject* obj = FdPickFilter::getInterestingPObj(&action->getPickedPointList(),
                                                     selectedObjects, // An array of selected FdObject's
                                                     {}, false, // No variables filtering
                                                     indexToPitem, wasASelected); // Variables returning values
    if (obj)
    {
      if (single)
        FapEventManager::permTotalSelect(obj->getFmOwner());
      else
        FapEventManager::permSelect(obj->getFmOwner());
    }
    else if (single)
      FapEventManager::permUnselectAll();

    return obj;
  }

  // Manipulation events
  void selectCB(void*, SoEventCallback*);

  void moveElemCB(void*, SoEventCallback*);
  void moveGroupCB(void*, SoEventCallback*);

  void createElemCB(void*, SoEventCallback*);
  void deleteObjectCB(void*, SoEventCallback*);
  void rotateElemCB(void*, SoEventCallback*);

  void drawLineCB(void*, SoEventCallback*);
  void moveLineCB(void*, SoEventCallback*);
  void addLinePointCB(void*, SoEventCallback*);
  void removeLinePointCB(void*, SoEventCallback*);

  // Adminstrate events
  void adminEventCB(void* ,SoEventCallback*);
}


//////////////////////////////////////////////////////
//
//     Class Methods
//
/////////////////////////////////////////////////////

void FdCtrlDB::init()
{
  // Initiate control display Inventor classes.
  FdCtrlKit::init();
  FdCtrlElemKit::init();
  FdCtrlLineKit::init();
  FdCtrlSymbolKit::init();
  FdCtrlGridKit::init();
  FdCtrlSymDef::init();
}


void FdCtrlDB::start(bool useGUI)
{
  // Create the top of the scene graph
  ctrlRoot = new SoSeparator;
  ctrlRoot->ref();
  ourExtraGraphicsSep = new SoSeparator;
  ourExtraGraphicsSep->ref();
  ctrlKit = new FdCtrlKit;
  ctrlKit->ref();

  // Sets up callback nodes
  adminEventCbNode = new SoEventCallback;
  adminEventCbNode->addEventCallback(SoEvent::getClassTypeId(),adminEventCB);
  manipEventCbNode = new SoEventCallback;

  // Build the top of the scene graph
  ctrlRoot->addChild(adminEventCbNode);
  ctrlRoot->addChild(manipEventCbNode);
  ctrlRoot->addChild(ctrlKit);
  ctrlRoot->addChild(ourExtraGraphicsSep);

  if (!useGUI) return;

  Fui::ctrlModellerUI(false,true);
  ctrlViewer = dynamic_cast<FdQtViewer*>(Fui::getCtrlViewer());
  ctrlViewer->setBackgroundColor(SbColor(1,1,1));
  ctrlViewer->setSceneGraph(ctrlRoot);
}


void FdCtrlDB::openCtrl()
{
  // Starts the control window
  Fui::ctrlModellerUI(true);
  FuiCtrlModes::setCtrlModellerState(true);
  FuiCtrlModes::cancel(); // dagc must still be there
}

void FdCtrlDB::closeCtrl()
{
  Fui::ctrlModellerUI(false,true);
  FuiCtrlModes::setCtrlModellerState(false);
}


//View  Methods
//////////////////////////

ctrlViewData FdCtrlDB::getView()
{
  ctrlViewData cvData;

  cvData.itsCameraTranslation = FdConverter::toFaVec3(ctrlViewer->getPos());
  cvData.itsFocalDistance = ctrlViewer->getFocalDistance();

  cvData.itsGridSizeX = FdCtrlGrid::getGridSizeX();
  cvData.itsGridSizeY = FdCtrlGrid::getGridSizeY();
  cvData.isGridOn = FdCtrlGrid::getGridState();

  cvData.itsSnapDistanceX = FdCtrlGrid::getSnapDistanceX();
  cvData.itsSnapDistanceY = FdCtrlGrid::getSnapDistanceY();
  cvData.isSnapOn = FdCtrlGrid::getSnapState();

  return cvData;
}

void FdCtrlDB::setView(const ctrlViewData& cvData)
{
  ctrlViewer->setPosition(FdConverter::toSbVec3f(cvData.itsCameraTranslation));
  ctrlViewer->setFocalDistance(cvData.itsFocalDistance);
  FdCtrlGrid::setGridSize(cvData.itsGridSizeX, cvData.itsGridSizeY);
  FdCtrlGrid::setSnapDistance(cvData.itsSnapDistanceX, cvData.itsSnapDistanceY);
  FdCtrlGrid::setGridState(cvData.isGridOn);
  FdCtrlGrid::setSnapState(cvData.isSnapOn);
}


void FdCtrlDB::zoomTo(FmIsRenderedBase* elmOrLine)
{
  if (!elmOrLine) return;

  FFuTopLevelShell* modeller = FFuTopLevelShell::getInstanceByType(FuiCtrlModeller::getClassTypeID());
  if (modeller) modeller->popUp();

  ctrlViewer->viewAll(elmOrLine->getFdPointer()->getKit());
}


FdQtViewer* FdCtrlDB::getCtrlViewer()
{
  return ctrlViewer;
}

FdCtrlKit* FdCtrlDB::getCtrlKit()
{
  return ctrlKit;
}

SoSeparator* FdCtrlDB::getCtrlExtraGraphicsRoot()
{
  return ourExtraGraphicsSep;
}

FdObject* FdCtrlDB::getPickedObject()
{
  return pickedObject;
}

void FdCtrlDB::setCreatedElem(FmCtrlElementBase* createElem)
{
  newFdElement = (FdCtrlElement*)(createElem->getFdPointer());
  isNewElemVisible = false;
}


/*!
  Calculate a new vector from the start translation vector and a mouse position.
*/

SbVec3f FdCtrlDB::getNewVec(const SbVec2f& currPos, bool ignoreSnap)
{
  // Sets the view volume to the projector
  SbPlaneProjector planeProj;
  planeProj.setViewVolume(ctrlViewer->getViewVolume());

  // Sets the initial position from a point on the projector
  SbVec2f startVec(0.5,0.5);

  // Get new vector in the plane z=0 from a screen point
  SbVec3f newVec = planeProj.getVector(startVec,currPos);

  // This makes the new vector independent of window size
  SbVec2s windowSize = ctrlViewer->getWindowSize();
  if (windowSize[0] > windowSize[1])
    newVec[0] = (newVec[0]*windowSize[0])/windowSize[1];
  else
    newVec[1] = (newVec[1]*windowSize[1])/windowSize[0];

  // This makes the translation independent of viewer paning,
  // but the viewer rotation angle must be zero
  SbVec3f pos = ctrlViewer->getPos();
  newVec[0] += pos[0];
  newVec[1] += pos[1];
  newVec[2] = 0.0f;

  if (!ignoreSnap)
    FdCtrlGrid::snapToNearestPoint(newVec);

  return newVec;
}


/*!
  Handles all modes choosen from the icon panel.
  Connect and disconnect callback methods.
*/

void FdCtrlDB::updateMode(int newMode, int oldMode)
{
  switch (oldMode)
    {
    case FuiCtrlModes::NEUTRAL_MODE:
      manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(), selectCB);
      break;
    case FuiCtrlModes::CREATE_MODE:
      // Delete the new element if it's not visible to the user or cancel during create mode
      if ((!isNewElemVisible || cancel) && newFdElement)
      {
        newFdElement->erase();
        newFdElement = NULL;
      }
      manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(),createElemCB);
      break;
    case FuiCtrlModes::DELETE_MODE:
      manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(),deleteObjectCB);
      break;
    case FuiCtrlModes::ROTATE_MODE:
      manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(),rotateElemCB);
      break;
    case FuiCtrlModes::ADDLINEPOINT_MODE:
      manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(),addLinePointCB);
      break;
    case FuiCtrlModes::REMOVELINEPOINT_MODE:
      manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(),removeLinePointCB);
      break;
    case FuiCtrlModes::MOVEGROUP_MODE:
      manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(), moveGroupCB);
      break;
    }

  switch (newMode)
    {
    case FuiCtrlModes::NEUTRAL_MODE:
      manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(),selectCB);
      break;
    case FuiCtrlModes::CREATE_MODE:
      FapEventManager::permUnselectAll();
      manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(),createElemCB);
      break;
    case FuiCtrlModes::DELETE_MODE:
      FapEventManager::permUnselectAll();
      manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(),deleteObjectCB);
      break;
    case FuiCtrlModes::ROTATE_MODE:
      pickedObject = 0;
      FapEventManager::permUnselectAll();
      manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(),rotateElemCB);
      break;
    case FuiCtrlModes::ADDLINEPOINT_MODE:
      FapEventManager::permUnselectAll();
      manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(),addLinePointCB);
      break;
    case FuiCtrlModes::REMOVELINEPOINT_MODE:
      FapEventManager::permUnselectAll();
      manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(),removeLinePointCB);
      break;
    case FuiCtrlModes::MOVEGROUP_MODE:
      FapEventManager::permUnselectAll();
      manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(), moveGroupCB);
      break;
    }

  cancel = false;
}


/*!
  Handles direct manipulated callbacks but only when mode is NEUTRAL.
*/

void FdCtrlDB::updateNeutralType(int newNeutralType, int oldNeutralType)
{
  switch (oldNeutralType)
    {
    case FuiCtrlModes::MOVEELEM_NEUTRAL:
      manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(), moveElemCB);
      break;
    case FuiCtrlModes::DRAW_NEUTRAL:
     FdCtrlObject::removeFeedbackLine();
      manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(), drawLineCB);
      break;
    case FuiCtrlModes::MOVELINE_NEUTRAL:
      manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(), moveLineCB);
      break;
    }

  switch (newNeutralType)
    {
    case FuiCtrlModes::MOVEELEM_NEUTRAL:
      manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(), moveElemCB);
      break;
    case FuiCtrlModes::DRAW_NEUTRAL:
      manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(), drawLineCB);
      break;
    case FuiCtrlModes::MOVELINE_NEUTRAL:
      manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(), moveLineCB);
      break;
    }
}


namespace {

// Event Callback methods.
//////////////////////////////////////////////////////////
// Administration callback event.
// Handles mouse button 2 and 3 events which are "done" and "cancel".

void adminEventCB(void*, SoEventCallback* eventCB)
{
  const SoEvent* event = eventCB->getEvent();

  if (SO_MOUSE_RELEASE_EVENT(event,BUTTON3))
  {
    FuiCtrlModes::done();
  }
  else if (SO_MOUSE_RELEASE_EVENT(event,BUTTON2))
  {
    cancel = true; // Set to false in updateMode()
    FuiCtrlModes::cancel();
    FapEventManager::permUnselectAll();
    pickedObject = NULL;
  }
  else if (SO_KEY_PRESS_EVENT(event,LEFT_CONTROL))
  {
    FuiCtrlModes::setMode(FuiCtrlModes::MOVEGROUP_MODE);
  }
  else if (SO_KEY_RELEASE_EVENT(event,LEFT_CONTROL))
  {
    FuiCtrlModes::cancel();
  }
  else
    return;

  eventCB->setHandled();
}


// Direct manipulation callback events, for instance select, move etc.
////////////////////////////////////////////////////////////////////
/*!
  Makes it possible to select an object in the scenegraph,
  only in use when the mode is NEUTRAL.
  Handles all direct-manipulated methods, such as draw line, move element etc.
*/

void selectCB(void*, SoEventCallback* eventCB)
{
  const SoEvent* event = eventCB->getEvent();
  if (SO_MOUSE_PRESS_EVENT(event,BUTTON1))
  {
    /////////////////////////////////////////////////////
    //                Object picking
    ///////////////////////////////////////////////////

    pickedObject = pickObject(true,eventCB->getAction());
    if (pickedObject && FuiCtrlModes::getNeutralType() == FuiCtrlModes::EXAM_NEUTRAL)
    {
      const SoPickedPoint* pickedPoint = eventCB->getPickedPoint();
      if (pickedObject->isOfType(FdCtrlElement::getClassTypeID()))
      {
        FdCtrlElement* pickedElem = (FdCtrlElement*)pickedObject;
        pickedElem->getElementDetail(pickedPoint->getPoint(),
                                     pickedElemDetail,pickedPortNumber);
        if (pickedElemDetail == FdCtrlElement::BODY)
        {
          // The body was hit
          FuiCtrlModes::setNeutralType(FuiCtrlModes::MOVEELEM_NEUTRAL);
        }
        else if (pickedElemDetail == FdCtrlElement::OUTPUT)
        {
          // The out port was hit
          startLineElem = pickedElem;
          FuiCtrlModes::setNeutralType(FuiCtrlModes::DRAW_NEUTRAL);
          FuiCtrlModes::setNeutralState(0);
        }
      }
      else if (pickedObject->isOfType(FdCtrlLine::getClassTypeID()))
      {
        // Shows which detail on the line the user has hit.
        // 1->Point1, 2->Point2,....,-1->Line1,,-2->Line2......
        SbVec3f pt = pickedPoint->getObjectPoint();
        long int lineIndex = ((SoLineDetail*)pickedPoint->getDetail())->getPartIndex();
        lineDetail = ((FdCtrlLine*)pickedObject)->pickedLineDetail(pt,lineIndex);
        FuiCtrlModes::setNeutralType(FuiCtrlModes::MOVELINE_NEUTRAL);
      }
    }
    eventCB->setHandled();
  }
  else if (event->isOfType(SoLocation2Event::getClassTypeId()))
  {
    if (FuiCtrlModes::getNeutralType() == FuiCtrlModes::DRAW_NEUTRAL)
    {
      ///////////////////////////////////////////////
      ////                Line drawing             //
      ///////////////////////////////////////////////

      // Pick second element.
      // Drawing feedback line to show user that the first line point is found
      const float offs = 0.75f;
      SbVec3f startLineVec = startLineElem->getElemTranslation();
      startLineVec[0] += startLineElem->isElementLeftRotated() ? -offs : offs;
      SbVec3f currLineVec = FdCtrlDB::getNewVec(event->getNormalizedPosition(ctrlViewer->getViewportRegion()),true);
      FdCtrlObject::drawFbLine(startLineVec, currLineVec);

      // Ok Lets see if mouse is over a port
      pickedObject = NULL;
      const SoPickedPoint* pickedPoint = NULL;
      const SoPickedPointList& ppl = eventCB->getAction()->getPickedPointList();
      for (int i = 0; i < ppl.getLength() && !pickedObject; i++)
        if ((pickedObject = FdPickFilter::findFdObject(ppl[i]->getPath())))
          pickedPoint = ppl[i];

      bool isMouseOverValidPort = false;
      if (pickedObject && pickedObject->isOfType(FdCtrlElement::getClassTypeID()))
      {
        FdCtrlElement* pickedElem = (FdCtrlElement*)pickedObject;
        pickedElem->getElementDetail(pickedPoint->getPoint(),
                                     pickedElemDetail,pickedPortNumber);
        if (pickedElemDetail != FdCtrlElement::BODY &&
            pickedElemDetail != FdCtrlElement::OUTPUT &&
            pickedElemDetail != FdCtrlElement::NONE)
        {
          // A line can't have the same start- and end element.
          if (startLineElem == pickedObject)
            FuiCtrlModes::setNeutralState(0);

          // Checks if the port is occupied, before a new line can be created.
          else if (((FmCtrlElementBase*)pickedObject->getFmOwner())->getLine(pickedPortNumber))
            FuiCtrlModes::setNeutralState(2);
          else
          {
            isMouseOverValidPort = true;
            FuiCtrlModes::setNeutralState(1);
          }
        }
        else if (FuiCtrlModes::getNeutralState() != 0)
          FuiCtrlModes::setNeutralState(0);
      }
      else if (!pickedObject) // Did not hit anything
        if (FuiCtrlModes::getNeutralState() != 0)
          FuiCtrlModes::setNeutralState(0);

      if (isMouseOverValidPort)
      {
        FapEventManager::permSelect(pickedObject->getFmOwner());
        endLineElem = (FdCtrlElement*)pickedObject;
      }
      else
      {
        if (endLineElem)
          FapEventManager::permUnselect(endLineElem->getFmOwner());
        endLineElem = NULL;
      }

      eventCB->setHandled();
    }
  }
}

//This are direct manipulated callback methods
///////////////////////////////////////////////////////

//! Makes it possible to move an element in the scengraph.

void moveElemCB(void*, SoEventCallback* eventCB)
{
  const SoEvent* event = eventCB->getEvent();

  if (FuiCtrlModes::getNeutralState() != 0)
    FuiCtrlModes::setNeutralState(0);

  if (pickedObject && event->isOfType(SoLocation2Event::getClassTypeId()))
  {
    // Current normalized mouse cursor position.
    SbVec2f currPos = event->getNormalizedPosition(ctrlViewer->getViewportRegion());
    // Sets the new element translation.
    ((FdCtrlElement*)pickedObject)->moveObject(FdCtrlDB::getNewVec(currPos),true);

    eventCB->setHandled();
  }
  else if (SO_MOUSE_RELEASE_EVENT(event,BUTTON1))
  {
    FuiCtrlModes::setNeutralType(FuiCtrlModes::EXAM_NEUTRAL);
  }
}

/*!
  When in neutralType : draw_neutral, Create a new line
  on mouse release if we've got a valid selection, or cancel if we ve not.
*/

void drawLineCB(void*, SoEventCallback* eventCB)
{
  if (SO_MOUSE_RELEASE_EVENT(eventCB->getEvent(),BUTTON1))
  {
    if (FuiCtrlModes::getNeutralState() == 1 &&
        pickedElemDetail != FdCtrlElement::BODY &&
        pickedElemDetail != FdCtrlElement::OUTPUT &&
        pickedElemDetail != FdCtrlElement::NONE)
    {
      FmCtrlLine* line = FmControlAdmin::createLine((FmCtrlElementBase*)startLineElem->getFmOwner(),
                                                    (FmCtrlElementBase*)endLineElem->getFmOwner(),
                                                        pickedPortNumber);
      FuiCtrlModes::setMode(FuiCtrlModes::NEUTRAL_MODE);
      FapEventManager::permTotalSelect(line);
    }
    else
      FuiCtrlModes::cancel();
  }
  eventCB->setHandled();
}


//! Makes it possible to move linesegments.

void moveLineCB(void*, SoEventCallback* eventCB)
{
  const SoEvent* event = eventCB->getEvent();

  if (FuiCtrlModes::getNeutralState() != 0)
    FuiCtrlModes::setNeutralState(0);

  if (SO_MOUSE_RELEASE_EVENT(event,BUTTON1))
  {
    FuiCtrlModes::setMode(FuiCtrlModes::NEUTRAL_MODE);
    eventCB->setHandled();
  }
  else if (event->isOfType(SoLocation2Event::getClassTypeId()))
  {
    SbVec3f currVec = FdCtrlDB::getNewVec(event->getNormalizedPosition(ctrlViewer->getViewportRegion()));
    if (!((FdCtrlLine*)pickedObject)->manipLine(lineDetail,currVec))
      FuiCtrlModes::setNeutralState(1);
    eventCB->setHandled();
  }
}


///////////////////////////////////////////////////////////////////////
//Callbacks methods choosen from the iconpanel.
////////////////////////////////////////////////////////////////////////////////////

//! Handles all events in the 3D-window concerning creating.
void createElemCB(void*, SoEventCallback* eventCB)
{
  if (!newFdElement || !eventCB) return;

  const SoEvent* event = eventCB->getEvent();

  if (event->isOfType(SoLocation2Event::getClassTypeId()))
  {
    isNewElemVisible = true;
    SbVec2f currPos = event->getNormalizedPosition(ctrlViewer->getViewportRegion());
    newFdElement->moveObject(FdCtrlDB::getNewVec(currPos),true);
  }
  else if (SO_MOUSE_PRESS_EVENT(event,BUTTON1))
  {
    FuiCtrlModes::cancel();
    FapEventManager::permSelect(newFdElement->getFmOwner());
    newFdElement = NULL;
  }
}

// Method to erase a element from the scene graph.
void deleteObjectCB(void*, SoEventCallback* eventCB)
{
  const SoEvent* event = eventCB->getEvent();

  if (SO_MOUSE_PRESS_EVENT(event,BUTTON1))
  {
    pickedObject = pickObject(true,eventCB->getAction());
    if (pickedObject) FuiCtrlModes::setState(1);
  }

  if (pickedObject)
    //Delete must be confirmed with "DONE" (mouse button 2 => state=2 if state was 1).
    if (FuiCtrlModes::getState() == 2)
    {
      ((FdCtrlObject*)pickedObject)->erase();
      pickedObject = NULL;
      FuiCtrlModes::setState(0);
    }
}

void rotateElemCB(void* ,SoEventCallback* eventCB)
{
  const SoEvent* event = eventCB->getEvent();

  if (SO_MOUSE_PRESS_EVENT(event,BUTTON1))
  {
    pickedObject = pickObject(true,eventCB->getAction());
    if (pickedObject) FuiCtrlModes::setState(1);
  }

  if (FuiCtrlModes::getState() == 2)
    if (pickedObject && pickedObject->isOfType(FdCtrlElement::getClassTypeID()))
    {
      // Delete must be confirmed with "DONE" (mouse button 2 => state=2)
      ((FdCtrlElement*)pickedObject)->rotateObject();
      FuiCtrlModes::setState(0);
      FapEventManager::permUnselectAll();
    }
}


//Methods to add new and delete old break points to a connection line.
void addLinePointCB(void*, SoEventCallback* eventCB)
{
  const SoEvent* event = eventCB->getEvent();

  if (SO_MOUSE_PRESS_EVENT(event,BUTTON1))
  {
    pickedObject = pickObject(true,eventCB->getAction());
    if (pickedObject && pickedObject->isOfType(FdCtrlLine::getClassTypeID()))
    {
      FuiCtrlModes::setState(1);
      ((FdCtrlLine*)pickedObject)->addLinePoint1(eventCB->getPickedPoint());
    }
  }
  else if (SO_MOUSE_RELEASE_EVENT(event,BUTTON1))
  {
    FuiCtrlModes::setState(0);
    FapEventManager::permUnselectAll();
    pickedObject = NULL;
    eventCB->setHandled();
  }
  else if (FuiCtrlModes::getState() == 1)
    if (event->isOfType(SoLocation2Event::getClassTypeId()))
    {
      SbVec3f currVec = FdCtrlDB::getNewVec(event->getNormalizedPosition(ctrlViewer->getViewportRegion()));
      ((FdCtrlLine*)pickedObject)->addLinePoint2(currVec);
      eventCB->setHandled();
    }
}

void removeLinePointCB(void*, SoEventCallback* eventCB)
{
  const SoEvent* event = eventCB->getEvent();

  if (SO_MOUSE_PRESS_EVENT(event,BUTTON1))
  {
    pickedObject = pickObject(true,eventCB->getAction());
    if (pickedObject && pickedObject->isOfType(FdCtrlLine::getClassTypeID()))
    {
      FdCtrlLine* pickedLine = (FdCtrlLine*)pickedObject;
      int numLines = pickedLine->getNumberOfSegments();
      const SoPickedPoint* pickedPoint = eventCB->getPickedPoint();
      lineDetail = pickedLine->pickedLineDetail(pickedPoint->getObjectPoint(),
                                                ((SoLineDetail*)pickedPoint->getDetail())->getPartIndex());
      if (lineDetail == 1 || lineDetail == 2 ||
          lineDetail == numLines || lineDetail == numLines+1)
        FuiCtrlModes::setState(2);
      else if (lineDetail > 0)
      {
        point = lineDetail;
        FuiCtrlModes::setState(1);
      }
      else
        FuiCtrlModes::setState(3);
    }
  }
  //Remove must be confirmed with "DONE" (mouse button 2 set state = 4 if state was 1).
  else if (FuiCtrlModes::getState() == 4)
  {
    ((FdCtrlLine*)pickedObject)->removeLinePoint(point);
    FuiCtrlModes::setState(0);
  }
}

// Makes it possible to move a group of elements in the scene graph.

void moveGroupCB(void*, SoEventCallback* eventCB)
{
  const SoEvent* event = eventCB->getEvent();

  if (FuiCtrlModes::getState() == 1 || FuiCtrlModes::getState() == 0)
  {
    if (SO_MOUSE_PRESS_EVENT(event,BUTTON1))
    {
      pickObject(false,eventCB->getAction());
      SbVec3f traVec = FdCtrlDB::getNewVec(event->getNormalizedPosition(ctrlViewer->getViewportRegion()));

      std::vector<FdObject*> selectedObjects;
      FdSelector::getSelectedObjects(selectedObjects);
      selectedElements.clear();
      elemTransArray.clear();
      for (FdObject* object : selectedObjects)
        if (object->isOfType(FdCtrlElement::getClassTypeID()))
        {
          selectedElements.push_back((FdCtrlElement*)object);
          elemTransArray.push_back(((FdCtrlElement*)object)->getElemTranslation() - traVec);
        }
      FuiCtrlModes::setState(2);
    }
  }
  else if (FuiCtrlModes::getState() == 2)
  {
    if (event->isOfType(SoLocation2Event::getClassTypeId()))
    {
      SbVec3f traVec = FdCtrlDB::getNewVec(event->getNormalizedPosition(ctrlViewer->getViewportRegion()));

      size_t i = 0;
      for (const SbVec3f vec : elemTransArray)
	if (i < selectedElements.size())
          selectedElements[i++]->moveObject(vec+traVec,false);
    }

    if (SO_MOUSE_RELEASE_EVENT(event,BUTTON1))
      FuiCtrlModes::setState(1);
  }
}

} // end anonymous namespace
