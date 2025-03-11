// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapCreateSensorCmd.H"
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
    CREATING
  };

  CmdState ourState = START;

  void createSimpleSensor()
  {
    FFaViewItem* item = FapEventManager::getFirstPermSelectedObject();
    FmIsMeasuredBase* obj = dynamic_cast<FmIsMeasuredBase*>(item);
    if (!obj) return;

    FmSensorBase* sens = obj->getSimpleSensor(true);

    // If we are dealing with a control output object, we have to bypass
    // its sensor and use a sensor on the auto-created engine associated with it
    if (obj->isOfType(FmcOutput::getClassTypeID()))
    {
      FmEngine* engine = static_cast<FmcOutput*>(obj)->getEngine();
      if (engine) sens = engine->getSimpleSensor(true);
    }

    ListUI <<"Creating "<< sens->getUserDescription() <<"\n";
    FapCreateSensorCmd::instance()->setSensor(sens);
  }

  void createRelativeSensor()
  {
    FFaViewItem* item = FapEventManager::getFirstPermSelectedObject();
    FmTriad* t1 = dynamic_cast<FmTriad*>(item);
    FapEventManager::popPermSelection();
    item = FapEventManager::getFirstPermSelectedObject();
    FmTriad* t2 = dynamic_cast<FmTriad*>(item);
    if (!t1 || !t2 || t1 == t2)
    {
      ListUI <<"ERROR: Relative sensors should be used on different objects.\n";
      return;
    }

    FmSensorBase* sens = t1->getRelativeSensor(t2,true);
    ListUI <<"Creating "<< sens->getUserDescription() <<"\n";
    FapCreateSensorCmd::instance()->setSensor(sens);
  }

  void setState(CmdState newState)
  {
    switch ((ourState = newState))
      {
      case START:
        Fui::tip("Select argument from the 3D view or the objects browser");
        break;
      case TRIAD_SELECTED:
        Fui::tip("Accept argument by pressing Done, or select a different object");
        break;
      case TRIAD1_ACCEPTED:
        Fui::tip("Accept Triad as argument by pressing Done, or select Triad 2 to create a relative sensor");
        break;
      case ANY_SELECTED:
        Fui::tip("Accept argument by pressing Done, or select a different object");
        break;
      case TRIAD2_SELECTED:
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


void FapCreateSensorCmd::setSensor(FmSensorBase* sens)
{
  if (myEngine)
    myEngine->setSensor(sens,myArg);
}


void FapCreateSensorCmd::onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
                                                const std::vector<FFaViewItem*>&,
                                                const std::vector<FFaViewItem*>&)
{
  if (FuiModes::getMode() != FuiModes::CREATE_SENSOR_MODE)
    return;

  FmEngine* e;
  switch (ourState)
    {
    case START:
    case ANY_SELECTED:
    case TRIAD_SELECTED:
      if (totalSelection.empty())
        setState(START);
      else if (dynamic_cast<FmTriad*>(totalSelection.front()))
        setState(TRIAD_SELECTED);
      else if (((e = dynamic_cast<FmEngine*> (totalSelection.front())) && e != myEngine) ||
               dynamic_cast<FmJointBase*>    (totalSelection.front()) ||
               dynamic_cast<FmAxialSpring*>  (totalSelection.front()) ||
               dynamic_cast<FmAxialDamper*>  (totalSelection.front()) ||
               dynamic_cast<FmStrainRosette*>(totalSelection.front()) ||
               dynamic_cast<FmcOutput*>      (totalSelection.front()))
        setState(ANY_SELECTED);
      else
        setState(START);
      break;

    case TRIAD1_ACCEPTED:
      if (!totalSelection.empty() && dynamic_cast<FmTriad*>(totalSelection[0]))
        setState(TRIAD2_SELECTED);
      break;

    case TRIAD2_SELECTED:
      if (totalSelection.size() > 1 && dynamic_cast<FmTriad*>(totalSelection[1]))
        setState(TRIAD2_SELECTED);
      else
        setState(TRIAD1_ACCEPTED);
      break;

    default:
      break;
    }
}


void FapCreateSensorCmd::createSensor(unsigned int iArg)
{
  myArg = iArg;
  myEngine = dynamic_cast<FmEngine*>(FapEventManager::getFirstPermSelectedObject());
  if (!myEngine)
  {
    FmcInput* in = dynamic_cast<FmcInput*>(FapEventManager::getFirstPermSelectedObject());
    if (in) myEngine = in->getEngine();
  }

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
  if (FapEventManager::hasStackedSelections())
    FapEventManager::popPermSelection();

  ourState = START;
}


void FapCreateSensorCmd::done()
{
  switch (ourState)
    {
    case START:
      FuiModes::cancel();
      break;
    case ANY_SELECTED:
      createSimpleSensor();
      FapEventManager::permUnselectAll();
      FuiModes::cancel();
      break;
    case TRIAD_SELECTED:
      FapEventManager::pushPermSelection();
      setState(TRIAD1_ACCEPTED);
      break;
    case TRIAD1_ACCEPTED:
      FapEventManager::popPermSelection();
      createSimpleSensor();
      FapEventManager::permUnselectAll();
      FuiModes::cancel();
      break;
    case TRIAD2_SELECTED:
      createRelativeSensor();
      FapEventManager::permUnselectAll();
      FuiModes::cancel();
      break;
    default:
      FuiModes::cancel();
      break;
    }
}
