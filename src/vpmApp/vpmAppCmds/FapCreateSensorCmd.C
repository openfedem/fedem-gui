// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapCreateSensorCmd.H"
#include "vpmApp/vpmAppUAMap/FapUAFunctionProperties.H"
#include "vpmUI/FuiModes.H"
#include "vpmUI/Fui.H"

#ifdef USE_INVENTOR
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/events/SoMouseButtonEvent.h>

#include "vpmDisplay/FdEvent.H"
#include "vpmDisplay/FdPickFilter.H"
#include "vpmDisplay/FdSelector.H"
#include "vpmDisplay/FdTriad.H"
#include "vpmDisplay/FdSimpleJoint.H"
#include "vpmDisplay/FdLinJoint.H"
#include "vpmDisplay/FdCamJoint.H"
#include "vpmDisplay/FdAxialSprDa.H"
#include "vpmDisplay/FdStrainRosette.H"
#endif

#include "vpmDB/FmTriad.H"
#include "vpmDB/FmEngine.H"
#include "vpmDB/FmcInput.H"
#include "vpmDB/FmcOutput.H"
#include "vpmDB/FmJointBase.H"
#include "vpmDB/FmAxialSpring.H"
#include "vpmDB/FmAxialDamper.H"
#include "vpmDB/FmStrainRosette.H"
#include "vpmDB/FmCreate.H"

#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "FFaLib/FFaDefinitions/FFaViewItem.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"


namespace
{
  enum CmdState {
    START,
    ANY_SELECTED,
    TRIAD_SELECTED,
    TRIAD1_ACCEPTED,
    TRIAD2_SELECTED,
    TRIAD2_ACCEPTED,
    TRIAD3_SELECTED,
    TRIAD3_ACCEPTED,
    TRIAD4_SELECTED,
    CREATING
  };

  CmdState ourState = START;

  void createSimpleSensor()
  {
    FFaViewItem* item = FapEventManager::getFirstPermSelectedObject();
    FmIsMeasuredBase* obj = dynamic_cast<FmIsMeasuredBase*>(item);

    // If we are dealing with a control output object, we have to bypass
    // its sensor and use a sensor on the auto-created engine associated with it
    if (obj && obj->isOfType(FmcOutput::getClassTypeID()))
      obj = static_cast<FmcOutput*>(obj)->getEngine();

    FapCreateSensorCmd::instance()->createSensor({obj});
  }

  void createRelativeSensor()
  {
    std::vector<FmIsMeasuredBase*> triads;
    triads.reserve(4);

    FmTriad* triad = NULL;
    while ((triad = dynamic_cast<FmTriad*>(FapEventManager::getFirstPermSelectedObject())))
    {
      triads.push_back(triad);
      FapEventManager::popPermSelection();
    }

    FapCreateSensorCmd::instance()->createSensor(triads);
  }

  void setState(CmdState newState)
  {
    switch ((ourState = newState))
      {
      case START:
        Fui::tip("Select argument from the 3D view or the objects browser");
        break;
      case ANY_SELECTED:
        Fui::tip("Accept argument by pressing Done, or select a different object");
        break;
      case TRIAD_SELECTED:
        Fui::tip("Accept argument by pressing Done, or select a different triad");
        break;
      case TRIAD1_ACCEPTED:
        Fui::tip("Accept Triad as argument by pressing Done, or select Triad 2 to create a relative sensor");
        break;
      case TRIAD2_ACCEPTED:
        Fui::tip("Accept the Triads by pressing Done, or select Triad 3 to create a relative sensor measuring an angle");
        break;
      case TRIAD3_ACCEPTED:
        Fui::tip("Select Triad 4 to complete the relative sensor measuring an angle");
        break;
      case TRIAD2_SELECTED:
      case TRIAD3_SELECTED:
      case TRIAD4_SELECTED:
        Fui::tip("Accept by pressing Done, or select a different Triad");
        break;
      case CREATING:
        Fui::tip("Creating Sensor ...");
        break;
      }
  }

#ifdef USE_INVENTOR
  void eventCB(void*, SoEventCallback* eventCallbackNode)
  {
    const SoEvent* event = eventCallbackNode->getEvent();
    if (!event || !event->isOfType(SoMouseButtonEvent::getClassTypeId())) return;

    if (SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
    {
      SoHandleEventAction* evHaAction = eventCallbackNode->getAction();

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
        FapEventManager::permTotalSelect(pickedObject->getFmOwner());
      else
        FapEventManager::permUnselectAll();
    }
    else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
      return;

    eventCallbackNode->setHandled();
  }
#endif
}


void FapCreateSensorCmd::createSensor(const std::vector<FmIsMeasuredBase*>& objs)
{
  FmSensorBase* sensor = NULL;
  if (objs.size() >= 4)
    sensor = Fedem::createSensor(objs[3], objs[2], objs[1], objs[0]);
  else if (objs.size() >= 2)
    sensor = Fedem::createSensor(objs[1], objs[0]);
  else if (objs.size() == 1)
    sensor = Fedem::createSensor(objs[0]);

  if (myEngine && sensor)
  {
    myEngine->setSensor(sensor,myArg);
    FapUAExistenceHandler::doUpdateUI(FapUAFunctionProperties::getClassTypeID());
  }
}


void FapCreateSensorCmd::onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
                                                const std::vector<FFaViewItem*>&,
                                                const std::vector<FFaViewItem*>&)
{
  if (FuiModes::getMode() != FuiModes::CREATE_SENSOR_MODE || totalSelection.empty())
    return;

  switch (ourState)
    {
    case START:
    case ANY_SELECTED:
    case TRIAD_SELECTED:
      if (FmEngine* selEngine = dynamic_cast<FmEngine*>(totalSelection.front());
          selEngine && selEngine != myEngine)
        setState(ANY_SELECTED);
      else if (dynamic_cast<FmTriad*>(totalSelection.front()))
        setState(TRIAD_SELECTED);
      else if (dynamic_cast<FmJointBase*>    (totalSelection.front()) ||
               dynamic_cast<FmAxialSpring*>  (totalSelection.front()) ||
               dynamic_cast<FmAxialDamper*>  (totalSelection.front()) ||
               dynamic_cast<FmStrainRosette*>(totalSelection.front()) ||
               dynamic_cast<FmcOutput*>      (totalSelection.front()))
        setState(ANY_SELECTED);
      else
        setState(START);
      break;

    case TRIAD1_ACCEPTED:
    case TRIAD2_SELECTED:
      if (dynamic_cast<FmTriad*>(totalSelection.front()))
        setState(TRIAD2_SELECTED);
      break;

    case TRIAD2_ACCEPTED:
    case TRIAD3_SELECTED:
      if (dynamic_cast<FmTriad*>(totalSelection.front()))
        setState(TRIAD3_SELECTED);
      break;

    case TRIAD3_ACCEPTED:
    case TRIAD4_SELECTED:
      if (dynamic_cast<FmTriad*>(totalSelection.front()))
        setState(TRIAD4_SELECTED);
      break;

    default:
      break;
    }
}


void FapCreateSensorCmd::createSensor(int iArg)
{
  myArg = iArg;
  myEngine = dynamic_cast<FmEngine*>(FapEventManager::getFirstPermSelectedObject());
  if (!myEngine)
    if (FmcInput* in = dynamic_cast<FmcInput*>(FapEventManager::getFirstPermSelectedObject()); in)
      myEngine = in->getEngine();

  if (myEngine)
    FuiModes::setMode(FuiModes::CREATE_SENSOR_MODE);
}


void FapCreateSensorCmd::enterMode()
{
  FapEventManager::pushPermSelection();
#ifdef USE_INVENTOR
  FdEvent::addEventCB(eventCB);
#endif
  setState(START); // To get the tip set right
}


void FapCreateSensorCmd::cancelMode()
{
#ifdef USE_INVENTOR
  FdEvent::removeEventCB(eventCB);
#endif
  while (FapEventManager::hasStackedSelections())
    FapEventManager::popPermSelection();

  ourState = START;
}


void FapCreateSensorCmd::done()
{
  switch (ourState)
    {
    case ANY_SELECTED:
      createSimpleSensor();
      break;
    case TRIAD_SELECTED:
      FapEventManager::pushPermSelection();
      setState(TRIAD1_ACCEPTED);
      return;
    case TRIAD1_ACCEPTED:
      FapEventManager::popPermSelection();
      createSimpleSensor();
      break;
    case TRIAD2_SELECTED:
      FapEventManager::pushPermSelection();
      setState(TRIAD2_ACCEPTED);
      return;
    case TRIAD3_SELECTED:
      FapEventManager::pushPermSelection();
      setState(TRIAD3_ACCEPTED);
      return;
    case TRIAD3_ACCEPTED:
      return;
    case TRIAD2_ACCEPTED:
      FapEventManager::popPermSelection();
    case TRIAD4_SELECTED:
      createRelativeSensor();
      break;
    default:
      break;
    }

  FuiModes::cancel();
}
