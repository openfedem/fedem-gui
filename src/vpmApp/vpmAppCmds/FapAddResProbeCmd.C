// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapAddResProbeCmd.H"
#include "vpmApp/FapEventManager.H"

#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "FFaLib/FFaString/FFaStringExt.H"

#include "vpmUI/Icons/addResultsProbe.xpm"

#include "vpmUI/FuiModes.H"
#include "vpmUI/Fui.H"
#include "vpmDB/FmAnimation.H"

#ifdef USE_INVENTOR
#include "vpmDisplay/FdPickFilter.H"
#include "vpmDisplay/FdPickedPoints.H"
#include "vpmDisplay/FdEvent.H"
#include "vpmDisplay/FdPart.H"
#include "vpmDisplay/FdFEGroupPartKit.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDisplay/FdFEModel.H"

#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#endif


enum {
  START,
  PROBE_POS_SELECTED,
  PROBE_POS_ACCEPTED
};


int                           FapAddResProbeCmd::myState = START;
FdPart*                       FapAddResProbeCmd::myPart = NULL;
FaVec3                        FapAddResProbeCmd::myPosition;
std::string                   FapAddResProbeCmd::myValue;
std::vector<FapResProbeLabel> FapAddResProbeCmd::ourAddedLabels;


void FapAddResProbeCmd::init()
{
  FFuaCmdItem* cmdItem = new FFuaCmdItem("cmdId_addResultsProbe");
  cmdItem->setSmallIcon(addResultsProbe_xpm);
  cmdItem->setText("Add results probe");
  cmdItem->setToolTip("Add a label showing the results at a specific position");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FuiModes::setMode(FuiModes::ADD_RESULTSPROBE_MODE); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapAddResProbeCmd::getSensitivity,bool&));
}


void FapAddResProbeCmd::enterMode()
{
#ifdef USE_INVENTOR
  FdEvent::addEventCB(FapAddResProbeCmd::eventCB);
#endif
  FapAddResProbeCmd::setState(START); // To get the tip set right.
}


void FapAddResProbeCmd::cancelMode()
{
#ifdef USE_INVENTOR
  FdPickedPoints::resetPPs();
  for (const FapResProbeLabel& label : ourAddedLabels)
    label.first->removeLabels(label.second);
  FdEvent::removeEventCB(FapAddResProbeCmd::eventCB);
#endif
  myState = START;
  myPart = NULL;
  ourAddedLabels.clear();
}


void FapAddResProbeCmd::setState(int newState)
{
  switch (FapAddResProbeCmd::myState = newState)
    {
    case START:
#ifdef USE_INVENTOR
      FdPickedPoints::resetPPs();
#endif
      myPart = NULL;
      myPosition = FaVec3();
      myValue = "";
      Fui::tip("Select the position you want to add a results probe");
      break;
    case PROBE_POS_SELECTED:
      Fui::tip("Press Done to confirm the selected position,\nor pick again to select a different position");
      break;
    case PROBE_POS_ACCEPTED:
      Fui::tip(NULL);
      break;
    }
}


void FapAddResProbeCmd::done()
{
  switch (FapAddResProbeCmd::myState)
    {
    case START:
      FuiModes::cancel();
      break;
    case PROBE_POS_SELECTED:
#ifdef USE_INVENTOR
      if (myPart && myPart->getVisualModel())
      {
        FdFEModel* fem = myPart->getVisualModel();
        ourAddedLabels.push_back(std::make_pair(fem,fem->addLabel(myValue,myPosition)));
      }
#endif
      FapAddResProbeCmd::setState(START);
      break;
    default:
      FuiModes::cancel();
      break;
    }
}


void FapAddResProbeCmd::getSensitivity(bool& isSensitive)
{
  FapCmdsBase::isModellerActive(isSensitive);
  if (!isSensitive) return;

  FmAnimation* anim = FapEventManager::getActiveAnimation();
  isSensitive = anim && anim->showFringes.getValue();
}


#ifdef USE_INVENTOR
void FapAddResProbeCmd::eventCB(void*, SoEventCallback* eventCallbackNode)
{
  const SoEvent* event = eventCallbackNode->getEvent();
  if (!event) return;

  if (event->isOfType(SoMouseButtonEvent::getClassTypeId()) &&
      SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    int i;
    const SoPickedPointList& ppl = eventCallbackNode->getAction()->getPickedPointList();
    if (myState == START || myState == PROBE_POS_SELECTED)
      if ((myPart = FdPickFilter::findFirstPartHit(i,ppl)))
      {
        myPosition = FdConverter::toFaVec3(ppl[i]->getObjectPoint());
        SoFullPath* fpath = (SoFullPath*)ppl[i]->getPath();
        if (fpath)
        {
          int pathLength = fpath->getLength();
          for (int j = 0; j < pathLength; j++)
          {
            SoNode* n = fpath->getNodeFromTail(j);
            if (n->isOfType(FdFEGroupPartKit::getClassTypeId()))
            {
              FdFEGroupPartKit* gp = static_cast<FdFEGroupPartKit*>(n);
              myValue = FFaNumStr(gp->getResultFromMaterialIndex(ppl[i]->getMaterialIndex()));
              FapAddResProbeCmd::setState(PROBE_POS_SELECTED);
              FdPickedPoints::setFirstPP(FdConverter::toFaVec3(ppl[i]->getPoint()),
                                         FdConverter::toFaMat34(ppl[i]->getObjectToWorld()));
              break;
            }
          }
        }
      }
  }

  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCallbackNode->setHandled();
}
#else
void FapAddResProbeCmd::eventCB(void*,SoEventCallback*) {}
#endif
