// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/qtViewers/FdQtViewer.H"

#include <Inventor/SbLinear.h>
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


//General variables////////////////////////////////////////////////////
FdQtViewer  * FdCtrlDB::ctrlViewer;
SoSeparator * FdCtrlDB::ctrlRoot;//Top node in the ctrl scengraph.
SoSeparator * FdCtrlDB::ourExtraGraphicsSep;
FdCtrlKit   * FdCtrlDB::ctrlKit;

bool FdCtrlDB::cancel = false;

//Callback node.
SoEventCallback *FdCtrlDB::manipEventCbNode;
SoEventCallback *FdCtrlDB::adminEventCbNode;

//CreateCB
FdCtrlElement *FdCtrlDB::newFdElement = NULL;
bool FdCtrlDB::isNewElemVisible = false;

//SelectCB variables
FdObject *FdCtrlDB::pickedObject = NULL;
int FdCtrlDB::pickedElemDetail = FdCtrlElement::NONE;
int FdCtrlDB::pickedPortNumber = 0;// No Port
int FdCtrlDB::lineDetail;

//Line variables
FdCtrlElement *FdCtrlDB::startLineElem;
FdCtrlElement *FdCtrlDB::endLineElem;
int FdCtrlDB::point = 0;

//Move groupe variable.
std::vector<FdCtrlElement*> FdCtrlDB::selectedElements;
std::vector<SbVec3f>        FdCtrlDB::elemTransArray;


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

  FdCtrlDB::ctrlRoot = new SoSeparator;
  FdCtrlDB::ctrlRoot->ref();
  FdCtrlDB::ourExtraGraphicsSep = new SoSeparator;
  FdCtrlDB::ourExtraGraphicsSep->ref();
  FdCtrlDB::ctrlKit = new FdCtrlKit;
  FdCtrlDB::ctrlKit -> ref();

  // Sets up callback nodes 
  
  FdCtrlDB::adminEventCbNode = new SoEventCallback;
  FdCtrlDB::adminEventCbNode->addEventCallback(SoEvent::getClassTypeId(),adminEventCB);
  FdCtrlDB::manipEventCbNode = new SoEventCallback;

  // Build the top of the scengraph.

  FdCtrlDB::ctrlRoot->addChild(adminEventCbNode); 
  FdCtrlDB::ctrlRoot->addChild(manipEventCbNode);
  FdCtrlDB::ctrlRoot->addChild(FdCtrlDB::ctrlKit); 
  FdCtrlDB::ctrlRoot->addChild(FdCtrlDB::ourExtraGraphicsSep);

  if (!useGUI) return;

  Fui::ctrlModellerUI(false,true);
  FdCtrlDB::ctrlViewer = dynamic_cast<FdQtViewer*>(Fui::getCtrlViewer());
  FdCtrlDB::ctrlViewer->setBackgroundColor(SbColor(1,1,1));
  FdCtrlDB::ctrlViewer->setSceneGraph(FdCtrlDB::ctrlRoot);
}

void FdCtrlDB::openCtrl()
{
  // Starts the control window.

  Fui::ctrlModellerUI(true);
  FuiCtrlModes::setCtrlModellerState(true);
  FuiCtrlModes::cancel(); // dagc must still be there
}

void FdCtrlDB::closeCtrl()
{
  //FuiCtrlModes::cancel(); 
  Fui::ctrlModellerUI(false,true);
  FuiCtrlModes::setCtrlModellerState(false);
}


//View  Methods
//////////////////////////
ctrlViewData FdCtrlDB::getView()
{
  ctrlViewData cvData;
  SbMatrix mx = FdCtrlDB::ctrlViewer->getPosition();

  cvData.itsCameraTranslation = FaVec3(mx[3][0],mx[3][1],mx[3][2]);
  cvData.itsFocalDistance = FdCtrlDB::ctrlViewer->getFocalDistance();

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
  FdCtrlDB::ctrlViewer->setPosition(FdConverter::toSbVec3f(cvData.itsCameraTranslation));
  FdCtrlDB::ctrlViewer->setFocalDistance(cvData.itsFocalDistance);
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

  FdCtrlDB::ctrlViewer->viewAll(elmOrLine->getFdPointer()->getKit());
}


FdCtrlKit *FdCtrlDB::getCtrlKit()
{
  return FdCtrlDB::ctrlKit;
}

SoSeparator *FdCtrlDB::getCtrlExtraGraphicsRoot()
{
  return FdCtrlDB::ourExtraGraphicsSep;
}

FdObject *FdCtrlDB::getPickedObject()
{
  return FdCtrlDB::pickedObject;
}

void FdCtrlDB::setCreatedElem(FmCtrlElementBase *createElem)
{
  FdCtrlDB::newFdElement = (FdCtrlElement*)(createElem->getFdPointer());
  
  isNewElemVisible = false;
}


/*!
  Method to select objects in the scene graph;
*/

FdObject* FdCtrlDB::pickObject(bool single, SoHandleEventAction* evHaAction)
{
  std::vector<FdObject*> selectedObjects;
  FdSelector::getSelectedObjects(selectedObjects);

  long int indexToPitem = -1;
  bool     wasASelected = false;
  FdObject* obj = FdPickFilter::getInterestingPObj(&(evHaAction->getPickedPointList()),
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


/*!
  Calculate a new vector from the start translation vector and a mouse position.
*/

SbVec3f FdCtrlDB::getNewVec(const SbVec2f& currPos, bool ignoreSnap)
{
  // Sets the view volume to the projector
  SbPlaneProjector planeProj;
  planeProj.setViewVolume(FdCtrlDB::ctrlViewer->getViewVolume());

  // Sets the initial position from a point on the projector
  SbVec2f startVec(0.5,0.5);

  // Get new vector in the plane z=0 from a screen point
  SbVec3f newVec = planeProj.getVector(startVec,currPos);

  // This makes the new vector independent of window size
  SbVec2s windowSize = FdCtrlDB::ctrlViewer->getWindowSize();
  if (windowSize[0] > windowSize[1])
    newVec[0] = (newVec[0]*windowSize[0])/windowSize[1];
  else
    newVec[1] = (newVec[1]*windowSize[1])/windowSize[0];

  // This makes the translation independent of viewer paning,
  // but the viewer rotation angle must be zero
  SbMatrix mx = FdCtrlDB::ctrlViewer->getPosition();
  newVec[0] += mx[3][0];
  newVec[1] += mx[3][1];
  newVec[2] = 0.0f;

  if (!ignoreSnap)
    FdCtrlGrid::snapToNearestPoint(newVec);

  return newVec;
}


/*!
  Handels all modes choosen from the iconpanel. 
  Connect and disconnect callback methods.
*/

void FdCtrlDB::updateMode(int newMode, int oldMode)
{ 
  switch(oldMode)
    {    
    case FuiCtrlModes::NEUTRAL_MODE:     
      FdCtrlDB::manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(), selectCB);
      break;
    case FuiCtrlModes::CREATE_MODE:
      //Delete the new element if it's not visible to the user or cancel during create mode.     
      if( ( !isNewElemVisible || FdCtrlDB::cancel ) && FdCtrlDB::newFdElement )
	 {
	   ((FdCtrlObject*)FdCtrlDB::newFdElement)->erase();
	   newFdElement = NULL;
	 }
      
	 FdCtrlDB::manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(),createElemCB);
      break;
    case FuiCtrlModes::DELETE_MODE:
      FdCtrlDB::manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(),deleteObjectCB);
      break;
    case FuiCtrlModes::ROTATE_MODE:
      FdCtrlDB::manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(),rotateElemCB);
      break;
    case FuiCtrlModes::ADDLINEPOINT_MODE:
      FdCtrlDB::manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(),addLinePointCB);
      break;
    case FuiCtrlModes::REMOVELINEPOINT_MODE:
      FdCtrlDB::manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(),removeLinePointCB);
      break;    
    case FuiCtrlModes::MOVEGROUP_MODE:     
      FdCtrlDB::manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(), moveGroupCB);  
      break;
    } 

  switch(newMode)
    {
    case FuiCtrlModes::NEUTRAL_MODE:
      FdCtrlDB::manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(),selectCB);
      break;  
    case FuiCtrlModes::CREATE_MODE:
      FapEventManager::permUnselectAll();
      FdCtrlDB::manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(),createElemCB);
      break;
    case FuiCtrlModes::DELETE_MODE:
      FapEventManager::permUnselectAll();
      FdCtrlDB::manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(),deleteObjectCB);
      break;	  	
    case FuiCtrlModes::ROTATE_MODE:
      FdCtrlDB::pickedObject = 0;
      FapEventManager::permUnselectAll();
      FdCtrlDB::manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(),rotateElemCB);
      break;
    case FuiCtrlModes::ADDLINEPOINT_MODE:
      FapEventManager::permUnselectAll();
      FdCtrlDB::manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(),addLinePointCB);
      break;
    case FuiCtrlModes::REMOVELINEPOINT_MODE:
      FapEventManager::permUnselectAll();
      FdCtrlDB::manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(),removeLinePointCB);
      break;   
    case FuiCtrlModes::MOVEGROUP_MODE:
      FapEventManager::permUnselectAll();
      FdCtrlDB::manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(), moveGroupCB);
      break;
    }
  FdCtrlDB::cancel = false;
}


/*!
  Handels  directmanipulated callbacks but only when mode is NEUTRAL 
*/

void FdCtrlDB::updateNeutralType(int newNeutralType, int  oldNeutralType)
{
  switch(oldNeutralType)
    {
    case FuiCtrlModes::EXAM_NEUTRAL:
      
      break;
    case FuiCtrlModes::MOVEELEM_NEUTRAL:
      FdCtrlDB::manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(), moveElemCB);
      break;
    case FuiCtrlModes::DRAW_NEUTRAL:
     FdCtrlObject::removeFeedbackLine();
      FdCtrlDB::manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(), drawLineCB);
      break;
    case FuiCtrlModes::MOVELINE_NEUTRAL: 
      FdCtrlDB::manipEventCbNode->removeEventCallback(SoEvent::getClassTypeId(), moveLineCB);
      break;
    }
    
  switch(newNeutralType)
    {
    case FuiCtrlModes::EXAM_NEUTRAL:
      
      break;
    case FuiCtrlModes::MOVEELEM_NEUTRAL:
      FdCtrlDB::manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(), moveElemCB);
      break;
    case FuiCtrlModes::DRAW_NEUTRAL:
      FdCtrlDB::manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(), drawLineCB);
      break;
    case FuiCtrlModes::MOVELINE_NEUTRAL: 
      FdCtrlDB::manipEventCbNode->addEventCallback(SoEvent::getClassTypeId(), moveLineCB);
      break;
    }
}
  


// Event Callback methods.
//////////////////////////////////////////////////////////
//Administration callback event. Handels mouse button 2 and 3 events which are 
//"done" and "cancel".

void FdCtrlDB::adminEventCB(void *,SoEventCallback *eventCB)
{
  const SoEvent *event = eventCB->getEvent();
  
  if(SO_MOUSE_RELEASE_EVENT(event,BUTTON3)){ 
    FuiCtrlModes::done();    
    eventCB->setHandled();

  } else if(SO_MOUSE_RELEASE_EVENT(event,BUTTON2)){
    FdCtrlDB::cancel = true; // Set to false in updateMode()
    FuiCtrlModes::cancel();
    FapEventManager::permUnselectAll(); 
    FdCtrlDB::pickedObject = NULL;
    eventCB->setHandled();
  
  }else if((SO_KEY_PRESS_EVENT(event, LEFT_CONTROL))){
    FuiCtrlModes::setMode(FuiCtrlModes::MOVEGROUP_MODE); 
    eventCB->setHandled();

  }else if((SO_KEY_RELEASE_EVENT(event, LEFT_CONTROL))){
    FuiCtrlModes::cancel();  
    eventCB->setHandled();
  }
}


// Direct manipulation callback events, for instance select, move etc.
////////////////////////////////////////////////////////////////////
/*!
  Makes it possible to select a object in the Scengraph, only in use when
  the mode is NEUTRAL.
  Handels all direct-manipulated methods, such as draw line, 
  move element etc. 
*/

void FdCtrlDB::selectCB(void *,SoEventCallback *eventCB)
{  
  long lineIndex;

  SbVec3f startLineVec;
  SbVec3f currLineVec;
  SbVec3f portCorrVec(0.75, 0, 0);

  const SoEvent *event = eventCB -> getEvent();
  const SoPickedPoint *pickedPoint = NULL;
  SoHandleEventAction *eventAction = eventCB->getAction();

  if(SO_MOUSE_PRESS_EVENT(event,BUTTON1))
    {
  
      /////////////////////////////////////////////////////
      //                Object picking
      ///////////////////////////////////////////////////

      FdCtrlDB::pickedObject = pickObject(true,eventAction);
      
      if( FdCtrlDB::pickedObject 
          && (FuiCtrlModes::getNeutralType() == FuiCtrlModes::EXAM_NEUTRAL))
        {
          pickedPoint = eventCB->getPickedPoint();

          if(FdCtrlDB::pickedObject->isOfType(FdCtrlElement::getClassTypeID()))
            {  	
             ((FdCtrlElement *)FdCtrlDB::pickedObject)->
               getElementDetail(pickedPoint->getPoint(), FdCtrlDB::pickedElemDetail, FdCtrlDB::pickedPortNumber);

              if(FdCtrlDB::pickedElemDetail == FdCtrlElement::BODY)//The body was hit.
                {
                  FuiCtrlModes::setNeutralType(FuiCtrlModes::MOVEELEM_NEUTRAL);
                }
              else if(FdCtrlDB::pickedElemDetail == FdCtrlElement::OUTPUT)//The out port was hit.
                {
                  FdCtrlDB::startLineElem = ((FdCtrlElement*) FdCtrlDB::pickedObject);
		  
                  FuiCtrlModes::setNeutralType(FuiCtrlModes::DRAW_NEUTRAL);
                  FuiCtrlModes::setNeutralState(0);
                }
              eventCB->setHandled();
            }
          else if(FdCtrlDB::pickedObject->isOfType(FdCtrlLine::getClassTypeID()))
            { 
              // Shows which detail on the line the user has hit.
              // 1->Point1, 2->Point2,....,-1->Line1,,-2->Line2......

              SbVec3f pt = pickedPoint->getObjectPoint();
              lineIndex = ((SoLineDetail*)pickedPoint->getDetail())->getPartIndex();
	     
              FdCtrlDB::lineDetail = ((FdCtrlLine *)FdCtrlDB::pickedObject)
                ->pickedLineDetail(pt, lineIndex);
 	   
              FuiCtrlModes::setNeutralType(FuiCtrlModes::MOVELINE_NEUTRAL);
            }
        }
      eventCB->setHandled();  
    }
  else if(event->isOfType(SoLocation2Event::getClassTypeId()))
    { 
      if(FuiCtrlModes::getNeutralType() == FuiCtrlModes::DRAW_NEUTRAL)
        { 
          ///////////////////////////////////////////////
          ////                Line drawing             //
          ///////////////////////////////////////////////
          
          //  Pick second element.
          // Drawing feedback line to show the user that the first line point
          // is found

          startLineVec = FdCtrlDB::startLineElem->getElemTranslation()
            + (portCorrVec * (FdCtrlDB::startLineElem->isElementLeftRotated() 
                              ? -1 : 1));

          currLineVec = 
            getNewVec(event->getNormalizedPosition(FdCtrlDB::ctrlViewer->
                                                   getViewportRegion()), true);
	  
          FdCtrlObject::drawFbLine(startLineVec, currLineVec);

          // Ok Lets see if muse is over a port

          FdCtrlDB::pickedObject = 0;
          const SoPickedPointList  & ppl = eventAction->getPickedPointList();
          for (int i = 0; i < ppl.getLength(); i++)
            {
              FdCtrlDB::pickedObject = FdPickFilter::findFdObject(ppl[i]->getPath());
              if (FdCtrlDB::pickedObject){
                pickedPoint = ppl[i];
                break;
              }
            }
          /*
          SoPath * path = 0;
          if (pickedPoint = eventAction->getPickedPoint())
            path = pickedPoint->getPath();
          FdCtrlDB::pickedObject = FdPickFilter::findFdObject(path);
          */
          bool isMouseOverValidPort = false;

          if(FdCtrlDB::pickedObject)
            {	     
              if(FdCtrlDB::pickedObject->isOfType(FdCtrlElement::getClassTypeID()))
                {
                  ((FdCtrlElement *)FdCtrlDB::pickedObject)->
                    getElementDetail(pickedPoint->getPoint(), FdCtrlDB::pickedElemDetail, FdCtrlDB::pickedPortNumber);
                  
                  if(  ( FdCtrlDB::pickedElemDetail != FdCtrlElement::BODY)
                       &&( FdCtrlDB::pickedElemDetail != FdCtrlElement::OUTPUT)
                       && ( FdCtrlDB::pickedElemDetail != FdCtrlElement::NONE))
                    {
                      // A line can't have the same start- and end element.
                      if (FdCtrlDB::startLineElem == FdCtrlDB::pickedObject)
                        FuiCtrlModes::setNeutralState(0);

                      // Checks if the port is occupied,  before a new line can be created.
                      else if (((FmCtrlElementBase*)FdCtrlDB::pickedObject->getFmOwner())->getLine(FdCtrlDB::pickedPortNumber))
                        FuiCtrlModes::setNeutralState(2);

                      else {
                        isMouseOverValidPort = true;
                        FuiCtrlModes::setNeutralState(1);
                      }
                    }
                  else if(FuiCtrlModes::getNeutralState() != 0) 
                    FuiCtrlModes::setNeutralState(0);
                }
            }
          else // Did not hit anything
            if (FuiCtrlModes::getNeutralState() != 0) 
              FuiCtrlModes::setNeutralState(0);
          
          if (!isMouseOverValidPort)
            {
              if (endLineElem)
                FapEventManager::permUnselect(endLineElem->getFmOwner());
              endLineElem = 0;
            }
          else
            {
              FapEventManager::permSelect(FdCtrlDB::pickedObject->getFmOwner());
              endLineElem = (FdCtrlElement*)FdCtrlDB::pickedObject;
            }
 
         eventCB->setHandled();  
        }
      else if (FuiCtrlModes::getNeutralType() == FuiCtrlModes::MOVEELEM_NEUTRAL)
      {
        // Should be done .... Move MoveGrop stuff here. To make direct manipulation wo all the done things.
      }
    }
  else if (SO_MOUSE_RELEASE_EVENT(event,BUTTON1))
    {
      
    }
}


//This are direct manipulated callback methods
///////////////////////////////////////////////////////

//! Makes it possible to move an element in the scengraph.

void FdCtrlDB::moveElemCB(void *, SoEventCallback *eventCB)
{  
  const SoEvent *event = eventCB -> getEvent();
  
  if(FuiCtrlModes::getNeutralState() != 0) 
    FuiCtrlModes::setNeutralState(0);

  if( ( event->isOfType(SoLocation2Event::getClassTypeId()) )
      && (FdCtrlDB::pickedObject) )
    {
      // Current normalized mouse cursor position.

      SbVec2f currPos = event->getNormalizedPosition(FdCtrlDB::ctrlViewer->
						     getViewportRegion());
      // Sets the new element translation.

      ((FdCtrlElement*)FdCtrlDB::pickedObject)->moveObject(getNewVec(currPos),true);

      eventCB->setHandled(); 
    }
  else if(SO_MOUSE_RELEASE_EVENT(event,BUTTON1))
    {  
      FuiCtrlModes::setNeutralType(FuiCtrlModes::EXAM_NEUTRAL);
    }
}

/*!
  When in neutralType : draw_neutral, Create a new line
  on mouse release if we've got a valid selection, or 
  cancel if we ve not.  
*/

void FdCtrlDB::drawLineCB(void *,SoEventCallback *eventCB)
{
  if (SO_MOUSE_RELEASE_EVENT(eventCB->getEvent(),BUTTON1))
    {
      if( (FuiCtrlModes::getNeutralState() == 1)
          && (FdCtrlDB::pickedElemDetail != FdCtrlElement::BODY)
          && (FdCtrlDB::pickedElemDetail != FdCtrlElement::OUTPUT)
          && (FdCtrlDB::pickedElemDetail != FdCtrlElement::NONE))
        {
          FmCtrlLine* line = FmControlAdmin::createLine((FmCtrlElementBase*)FdCtrlDB::startLineElem->getFmOwner(), 
                                                        (FmCtrlElementBase*)FdCtrlDB::endLineElem->getFmOwner(),
                                                        FdCtrlDB::pickedPortNumber);
          FuiCtrlModes::setMode(FuiCtrlModes::NEUTRAL_MODE);
          FapEventManager::permTotalSelect(line);
        }
      else
        FuiCtrlModes::cancel();
    }
  eventCB->setHandled();  
}


//! Makes it possible to move linesegments.

void FdCtrlDB::moveLineCB(void*, SoEventCallback* eventCB)
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
    SbVec3f currVec = getNewVec(event->getNormalizedPosition(FdCtrlDB::ctrlViewer->getViewportRegion()));
    if (!((FdCtrlLine*)FdCtrlDB::pickedObject)->manipLine(FdCtrlDB::lineDetail,currVec))
      FuiCtrlModes::setNeutralState(1);
    eventCB->setHandled();
  }
}


///////////////////////////////////////////////////////////////////////
//Callbacks methods choosen from the iconpanel.
////////////////////////////////////////////////////////////////////////////////////

//! Handles all events in the 3D-window concerning creating.
void FdCtrlDB::createElemCB(void*, SoEventCallback* eventCB)
{
  if (!FdCtrlDB::newFdElement || !eventCB) return;

  const SoEvent* event = eventCB->getEvent();

  if (event->isOfType(SoLocation2Event::getClassTypeId()))
  {
    isNewElemVisible = true;
    SbVec2f currPos = event->getNormalizedPosition(FdCtrlDB::ctrlViewer->getViewportRegion());
    FdCtrlDB::newFdElement->moveObject(getNewVec(currPos),true);
  }
  else if (SO_MOUSE_PRESS_EVENT(event,BUTTON1))
  {
    FuiCtrlModes::cancel();
    FapEventManager::permSelect(FdCtrlDB::newFdElement->getFmOwner());
    FdCtrlDB::newFdElement = NULL;
  }
}

//Method to erase a element from the scengraph.
void FdCtrlDB::deleteObjectCB(void *,SoEventCallback *eventCB)
{ 
  const SoEvent *event = eventCB -> getEvent();
  SoHandleEventAction *eventAction = eventCB->getAction();

  if(SO_MOUSE_PRESS_EVENT(event,BUTTON1))
    {
      FdCtrlDB::pickedObject = pickObject(true,eventAction);
      if(FdCtrlDB::pickedObject) FuiCtrlModes::setState(1);
    }
    
  if(FdCtrlDB::pickedObject)
    {  
      //Delete must be confirmed with "DONE" (mouse button 2 => state=2 if state was 1).
      if(FuiCtrlModes::getState() == 2)
        {
          ((FdCtrlObject*)FdCtrlDB::pickedObject)->erase();
          FdCtrlDB::pickedObject = NULL;
       	  FuiCtrlModes::setState(0);
        }
    }
}

void FdCtrlDB::rotateElemCB(void *,SoEventCallback *eventCB)
{  
  const SoEvent *event = eventCB -> getEvent();
  SoHandleEventAction *eventAction = eventCB->getAction();

  if(SO_MOUSE_PRESS_EVENT(event,BUTTON1))
    {
      FdCtrlDB::pickedObject = pickObject(true,eventAction);
      if(FdCtrlDB::pickedObject) FuiCtrlModes::setState(1);
    }
  
  if(FuiCtrlModes::getState() == 2)
    { 	 
      if(FdCtrlDB::pickedObject)
        {
          if(FdCtrlDB::pickedObject->isOfType(FdCtrlElement::getClassTypeID()))
            { 
              //Delete must be confirmed with "DONE" (mouse button 2 => state=2). 
              ((FdCtrlElement *)FdCtrlDB::pickedObject)->rotateObject();
              FuiCtrlModes::setState(0);
              FapEventManager::permUnselectAll(); 
            }
        }
    }
}


//Methods to add new and delete old  breakpoints to a connection line.
void FdCtrlDB::addLinePointCB(void *,SoEventCallback *eventCB)
{
  const SoEvent *event = eventCB->getEvent();
  SoHandleEventAction *eventAction = eventCB->getAction();

  SbVec3f currVec;

  if(SO_MOUSE_PRESS_EVENT(event,BUTTON1))
    {  
      FdCtrlDB::pickedObject = pickObject(true,eventAction);
      if(FdCtrlDB::pickedObject)
        {
          if(FdCtrlDB::pickedObject->isOfType(FdCtrlLine::getClassTypeID()))
            {
              FuiCtrlModes::setState(1);	   
              ((FdCtrlLine *)FdCtrlDB::pickedObject)->addLinePoint1(eventCB->getPickedPoint());
            }
        }
    }
  else if(SO_MOUSE_RELEASE_EVENT(event,BUTTON1))
    {                 
      FuiCtrlModes::setState(0);
      FapEventManager::permUnselectAll(); 
      FdCtrlDB::pickedObject = NULL;
      eventCB->setHandled(); 
    } 
  else if(FuiCtrlModes::getState() == 1)
    {     
      if(event->isOfType(SoLocation2Event::getClassTypeId())) 
        {
          currVec = getNewVec(event->getNormalizedPosition(FdCtrlDB::ctrlViewer->getViewportRegion()));
	  
          ((FdCtrlLine *)pickedObject)->addLinePoint2(currVec);
	  
          eventCB->setHandled(); 
        }    
    }
}

void FdCtrlDB::removeLinePointCB(void *,SoEventCallback *eventCB)
{
  const SoEvent *event = eventCB -> getEvent();
  const SoPickedPoint *pickedPoint;
  SoHandleEventAction *eventAction = eventCB->getAction();

  int numLines;

  if(SO_MOUSE_PRESS_EVENT(event,BUTTON1))
    {  
      FdCtrlDB::pickedObject = pickObject(true,eventAction);
      if(FdCtrlDB::pickedObject)
        {
          if(FdCtrlDB::pickedObject->isOfType(FdCtrlLine::getClassTypeID()))
            {
              numLines = ((FdCtrlLine *)pickedObject)->getNumberOfSegments();

              pickedPoint = eventCB->getPickedPoint();
	      
              FdCtrlDB::lineDetail = ((FdCtrlLine *)FdCtrlDB::pickedObject)
                ->pickedLineDetail(pickedPoint->getObjectPoint(), 
                                   ((SoLineDetail*)pickedPoint->getDetail())->getPartIndex());
	      
              if((FdCtrlDB::lineDetail == 1)||(FdCtrlDB::lineDetail == 2)
                 ||(FdCtrlDB::lineDetail == numLines)||(FdCtrlDB::lineDetail == numLines+1))
                {
                  FuiCtrlModes::setState(2);
                }  
              else if(FdCtrlDB::lineDetail > 0)
                {
                  point = FdCtrlDB::lineDetail;
                  FuiCtrlModes::setState(1);
                }
              else
                {	
                  FuiCtrlModes::setState(3);
                }
            }
        }
    }
  //Remove must be confirmed with "DONE" (mouse button 2 set state = 4 if state was 1).
  else if((FuiCtrlModes::getState() == 4))
    {
      ((FdCtrlLine*)FdCtrlDB::pickedObject)->removeLinePoint(point);
      FuiCtrlModes::setState(0);
    }
}

// Makes it possible to move a group of elements in the scene graph.

void FdCtrlDB::moveGroupCB(void*, SoEventCallback* eventCB)
{  
  const SoEvent* event = eventCB->getEvent();

  if(FuiCtrlModes::getState() == 1 || FuiCtrlModes::getState() == 0)
    {       
      if(SO_MOUSE_PRESS_EVENT(event,BUTTON1))
        {
          pickObject(false,eventCB->getAction());
          SbVec3f traVec = getNewVec(event->getNormalizedPosition
                                     (FdCtrlDB::ctrlViewer->getViewportRegion()));
          traVec[2] = 0.0f;

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
  else if(FuiCtrlModes::getState() == 2)
    { 
      if(event->isOfType(SoLocation2Event::getClassTypeId()))
        {
          SbVec3f traVec = getNewVec(event->getNormalizedPosition
                                     (FdCtrlDB::ctrlViewer->getViewportRegion()));
          traVec[2] = 0.0f;

          size_t i = 0;
          for (const SbVec3f vec : elemTransArray)
            if (i < selectedElements.size())
              selectedElements[i++]->moveObject(vec+traVec,false);
        }

      if(SO_MOUSE_RELEASE_EVENT(event,BUTTON1))
        FuiCtrlModes::setState(1);
    }
}
