// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapEditStrainRosetteNodesCmd.H"
#include "vpmApp/FapEventManager.H"
#include "vpmApp/vpmAppUAMap/FapUAProperties.H"
#include "vpmDB/FmStrainRosette.H"

#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "vpmUI/FuiModes.H"
#include "vpmUI/Fui.H"

#ifdef USE_INVENTOR
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/events/SoMouseButtonEvent.h>

#include "vpmDisplay/FdPickFilter.H"
#include "vpmDisplay/FdPickedPoints.H"
#include "vpmDisplay/FdEvent.H"
#endif


enum {
  START,
  ROSETTE_SELECTED,
  ROSETTE_ACCEPTED,
  N_1_SELECTED,
  N_1_ACCEPTED,
  N_2_SELECTED,
  N_2_ACCEPTED,
  N_3_SELECTED,
  N_3_ACCEPTED,
  N_4_SELECTED,
  N_4_ACCEPTED
};

int FapEditStrainRosetteNodesCmd::ourState = START;
FmStrainRosette* FapEditStrainRosetteNodesCmd::ourStrainRosette = NULL;


void FapEditStrainRosetteNodesCmd::init()
{
  FFuaCmdItem* cmdItem = new FFuaCmdItem("cmdId_editStrainRosetteNodes");
  cmdItem->setText("Edit Strain Rosette Nodes");
  cmdItem->setToolTip("Edit the nodes of a strain rosette element");
  cmdItem->setActivatedCB(FFaDynCB0S(FapEditStrainRosetteNodesCmd::editStrainRosetteNodes));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapEditStrainRosetteNodesCmd::getSensitivity,bool&));
}


void FapEditStrainRosetteNodesCmd::editStrainRosetteNodes()
{
  ourStrainRosette = dynamic_cast<FmStrainRosette*>(FapEventManager::getFirstPermSelectedObject());

  FuiModes::setMode(FuiModes::EDITSTRAINROSETTENODES_MODE);
}


void FapEditStrainRosetteNodesCmd::enterMode()
{
#ifdef USE_INVENTOR
  FdEvent::addEventCB(FapEditStrainRosetteNodesCmd::eventCB);
#endif
  FapEditStrainRosetteNodesCmd::setState(START); // To get the tip set right.
}


void FapEditStrainRosetteNodesCmd::cancelMode()
{
#ifdef USE_INVENTOR
  FdPickedPoints::resetPPs();
  FdEvent::removeEventCB(FapEditStrainRosetteNodesCmd::eventCB);
#endif
  FapEditStrainRosetteNodesCmd::ourState = START;
  ourStrainRosette = NULL;
}


void FapEditStrainRosetteNodesCmd::setState(int newState)
{
  switch (FapEditStrainRosetteNodesCmd::ourState = newState)
    {
    case START:
#ifdef USE_INVENTOR
      FdPickedPoints::resetPPs();
      FdPickedPoints::deselectNode();
#endif
      Fui::tip("Select first node of the strain gage element");
      break;
    case N_1_SELECTED:
      Fui::tip("Press Done to confirm the first node, or pick again to select a different node.");
      break;
    case N_1_ACCEPTED:
      Fui::tip("Select second node of the strain gage element");
      break;
    case N_2_SELECTED:
      Fui::tip("Press Done to confirm the second node, or pick again to select a different node.");
      break;
    case N_2_ACCEPTED:
      Fui::tip("Select third node of the strain gage element");
      break;
    case N_3_SELECTED:
      Fui::tip("Press Done to confirm the third node, or pick again to select a different node.");
      break;
    case N_3_ACCEPTED:
      Fui::tip("Select fourth node of the strain gage element, or press Done to finish.");
      break;
    case N_4_SELECTED:
      Fui::tip("Press Done to confirm the fourth node, or pick again to select a different node.");
      break;
    }
}


void FapEditStrainRosetteNodesCmd::done()
{
 switch (FapEditStrainRosetteNodesCmd::ourState)
    {
    case START:
      break;
    case N_1_SELECTED:
      FapEditStrainRosetteNodesCmd::setState(N_1_ACCEPTED);
      break;
    case N_1_ACCEPTED:
      break;
    case N_2_SELECTED:
      FapEditStrainRosetteNodesCmd::setState(N_2_ACCEPTED);
      break;
    case N_2_ACCEPTED:
      break;
    case N_3_SELECTED:
      FapEditStrainRosetteNodesCmd::setState(N_3_ACCEPTED);
      break;
    case N_3_ACCEPTED:
    case N_4_SELECTED:
      FapEditStrainRosetteNodesCmd::setStrainRosetteNodes();
    default:
      FuiModes::cancel();
      break;
    }
}


void FapEditStrainRosetteNodesCmd::getSensitivity(bool& isSensitive)
{
  FapCmdsBase::isModelEditable(isSensitive);
  if (!isSensitive) return;

  FapCmdsBase::isModellerActive(isSensitive);
  if (!isSensitive) return;

  isSensitive = dynamic_cast<FmStrainRosette*>(FapEventManager::getFirstPermSelectedObject()) != NULL;
}


#ifdef USE_INVENTOR
void FapEditStrainRosetteNodesCmd::eventCB(void*, SoEventCallback* eventCallbackNode)
{
  if (!ourStrainRosette) return;

  const SoEvent* event = eventCallbackNode->getEvent();
  if (!event) return;

  if (event->isOfType(SoMouseButtonEvent::getClassTypeId()) &&
      SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    SoHandleEventAction* evHaAction = eventCallbackNode->getAction();
    const SoPickedPointList& ppl = evHaAction->getPickedPointList();
    FdObject* rosettePart = ourStrainRosette->rosetteLink->getFdPointer();
    int    nodeID = -1;
    FaVec3 worldNodePos;

    switch (FapEditStrainRosetteNodesCmd::ourState)
      {
      case START:
      case N_1_SELECTED:
        if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,rosettePart)) {
          FdPickedPoints::selectNode(0,nodeID,worldNodePos);
          FapEditStrainRosetteNodesCmd::setState(N_1_SELECTED);
        }
        break;

      case N_1_ACCEPTED:
      case N_2_SELECTED:
        if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,rosettePart)) {
          FdPickedPoints::selectNode(1,nodeID,worldNodePos);
          FapEditStrainRosetteNodesCmd::setState(N_2_SELECTED);
        }
        break;

      case N_2_ACCEPTED:
      case N_3_SELECTED:
        if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,rosettePart)) {
          FdPickedPoints::selectNode(2,nodeID,worldNodePos);
          FapEditStrainRosetteNodesCmd::setState(N_3_SELECTED);
        }
        break;

      case N_3_ACCEPTED:
      case N_4_SELECTED:
        if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,rosettePart)) {
	  FdPickedPoints::selectNode(3,nodeID,worldNodePos);
          FapEditStrainRosetteNodesCmd::setState(N_4_SELECTED);
        }
      }
  }

  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCallbackNode->setHandled();
}
#else
void FapEditStrainRosetteNodesCmd::eventCB(void*, SoEventCallback*) {}
#endif


void FapEditStrainRosetteNodesCmd::setStrainRosetteNodes()
{
#ifdef USE_INVENTOR
  ourStrainRosette->setTopology(NULL,FdPickedPoints::getSelectedNodes());
#endif
  ourStrainRosette->syncWithFEModel(true);
  ourStrainRosette->draw();

  FapUAProperties* uap = FapUAProperties::getPropertiesHandler();
  if (uap) uap->updateUIValues();
}
