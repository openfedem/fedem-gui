// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapStrainRosetteCmds.H"
#include "vpmApp/FapEventManager.H"
#include "vpmApp/vpmAppUAMap/FapUAProperties.H"

#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "vpmUI/Fui.H"
#include "vpmUI/FuiModes.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "vpmDB/FmStrainRosette.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "FFaLib/FFaDefinitions/FFaViewItem.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"

#ifdef USE_INVENTOR
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/details/SoLineDetail.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoVertexProperty.h>

#include "vpmDisplay/FdExtraGraphics.H"
#include "vpmDisplay/FdPickFilter.H"
#include "vpmDisplay/FdPickedPoints.H"
#include "vpmDisplay/FdFEModel.H"
#include "vpmDisplay/FdPart.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDisplay/FdEvent.H"
#endif


enum {
  START,
  N_1_SELECTED,
  N_1_ACCEPTED,
  N_2_SELECTED,
  N_2_ACCEPTED,
  N_3_SELECTED,
  N_3_ACCEPTED,
  N_4_SELECTED,
  START_DIRECTION,
  DIR_POINT_1_SELECTED,
  DIR_POINT_1_ACCEPTED,
  DIR_POINT_2_SELECTED,
  DIR_POINT_2_ACCEPTED,
  DIR_EDGE_SELECTED,
  DIR_EDGE_ACCEPTED
};

int FapStrainRosetteCmds::ourState = START;
FmPart* FapStrainRosetteCmds::ourRosettePart = NULL;
FmStrainRosette* FapStrainRosetteCmds::ourCreatedStrainRosette = NULL;

FaVec3 FapStrainRosetteCmds::ourDirection;
FaVec3 FapStrainRosetteCmds::ourDirP1;
FaVec3 FapStrainRosetteCmds::ourDirP2;


void FapStrainRosetteCmds::init()
{
  FFuaCmdItem* cmdItem = new FFuaCmdItem("cmdId_createStrainRosette");
  cmdItem->setSmallIcon(makeStrainRosette_xpm);
  cmdItem->setText("Strain Rosette");
  cmdItem->setToolTip("Create a strain rosette on a FE model");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ FuiModes::setMode(FuiModes::MAKESTRAINROSETTE_MODE); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::isModellerTouchable,bool&));
}


void FapStrainRosetteCmds::enterMode()
{
  FapEventManager::permUnselectAll();
#ifdef USE_INVENTOR
  FdEvent::addEventCB(FapStrainRosetteCmds::eventCB);
#endif
  FapStrainRosetteCmds::setState(START);
}


void FapStrainRosetteCmds::cancelMode()
{
  FapEventManager::permUnselectAll();
#ifdef USE_INVENTOR
  FdPickedPoints::deselectNode();
  FdPickedPoints::resetPPs();
  FdExtraGraphics::hideDirection();
  FdExtraGraphics::hideLine();
  FdEvent::removeEventCB(FapStrainRosetteCmds::eventCB);
#endif
  FapStrainRosetteCmds::ourState = START;
  FapStrainRosetteCmds::ourRosettePart = NULL;

  if (ourCreatedStrainRosette)
    ourCreatedStrainRosette->erase();
  ourCreatedStrainRosette = NULL;
}


void FapStrainRosetteCmds::setState(int newState)
{
  switch (FapStrainRosetteCmds::ourState = newState)
    {
    case START:
      FapEventManager::permUnselectAll();
      ourDirection = FaVec3(1.0,0.0,0.0);
      ourDirP1 = FaVec3(0.0,0.0,0.0);
      ourDirP2 = FaVec3(0.0,1.0,0.0);
      ourRosettePart = NULL;
#ifdef USE_INVENTOR
      FdPickedPoints::deselectNode();
#endif
      if (ourCreatedStrainRosette)
        ListUI <<"Created "<< ourCreatedStrainRosette->getIdString()
               <<" on "<< ourCreatedStrainRosette->rosetteLink->getIdString()
               <<".\n";
      ourCreatedStrainRosette = NULL;
#ifdef USE_INVENTOR
      FdExtraGraphics::hideDirection();
      FdExtraGraphics::hideLine();
      FdPickedPoints::resetPPs();
#endif
      Fui::tip("Select first node of the strain gage element");
      break;
    case N_1_SELECTED:
      Fui::tip("Press Done to confirm the first node, or pick again to select a different node");
      break;
    case N_1_ACCEPTED:
      Fui::tip("Select second node of the strain gage element");
      break;
    case N_2_SELECTED:
      Fui::tip("Press Done to confirm the second node, or pick again to select a different node");
      break;
    case N_2_ACCEPTED:
      Fui::tip("Select third node of the strain gage element");
      break;
    case N_3_SELECTED:
      Fui::tip("Press Done to confirm the third node, or pick again to select a different node");
      break;
    case N_3_ACCEPTED:
      Fui::tip("Select fourth node of the strain gage element, or press Done to create a three node element");
      break;
    case N_4_SELECTED:
      Fui::tip("Press Done to confirm the fourth node, or pick again to select a different node");
      break;
    case START_DIRECTION:
      Fui::tip("Select a reference direction for the strain rosette (select Points or Lines)");
      break;
    case DIR_POINT_1_SELECTED:
#ifdef USE_INVENTOR
      FdPickedPoints::deselectNode();
      FdExtraGraphics::hideLine();
      FdExtraGraphics::hideDirection();
      FdPickedPoints::resetPPs();
#endif
      FapEventManager::permUnselectAll();
      Fui::tip("Press Done to confirm the first point, or pick again to reconsider (select Points or Lines)");
      break;
    case DIR_POINT_1_ACCEPTED:
      Fui::tip("Select second point for the reference direction");
      break;
    case DIR_POINT_2_SELECTED:
      Fui::tip("Press Done to confirm the second point, or pick again to reconsider");
      break;
    case DIR_EDGE_SELECTED:
      Fui::tip("Press Done to confirm the direction, or pick again to reconsider");
      break;
    }
}


void FapStrainRosetteCmds::done()
{
  switch (FapStrainRosetteCmds::ourState)
    {
    case START:
      FuiModes::cancel();
      break;
    case N_1_SELECTED:
      FapStrainRosetteCmds::setState(N_1_ACCEPTED);
      break;
    case N_2_SELECTED:
      FapStrainRosetteCmds::setState(N_2_ACCEPTED);
      break;
    case N_3_SELECTED:
      FapStrainRosetteCmds::setState(N_3_ACCEPTED);
      break;
    case N_1_ACCEPTED:
    case N_2_ACCEPTED:
      // Need at least 3 nodes
      break;
    case N_3_ACCEPTED:
    case N_4_SELECTED:
      ourCreatedStrainRosette = new FmStrainRosette();
      ourCreatedStrainRosette->rosetteLink = ourRosettePart;
      ourCreatedStrainRosette->rosetteType = FmStrainRosette::SINGLE_GAGE;
#ifdef USE_INVENTOR
      ourCreatedStrainRosette->numNodes = FdPickedPoints::numSelectedNodes();
      ourCreatedStrainRosette->node1    = FdPickedPoints::getSelectedNode(0);
      ourCreatedStrainRosette->node2    = FdPickedPoints::getSelectedNode(1);
      ourCreatedStrainRosette->node3    = FdPickedPoints::getSelectedNode(2);
      ourCreatedStrainRosette->node4    = FdPickedPoints::getSelectedNode(3);
#endif
      ourCreatedStrainRosette->angleOrigin = FmStrainRosette::LINK_VECTOR;
      ourCreatedStrainRosette->angleOriginVector = ourDirection;
      ourCreatedStrainRosette->connect();
      ourCreatedStrainRosette->syncWithFEModel();
      ourCreatedStrainRosette->draw();
      FapStrainRosetteCmds::setState(START_DIRECTION);
      break;
    case START_DIRECTION:
      FapStrainRosetteCmds::setState(START);
      break;
    case DIR_POINT_1_SELECTED:
      FapStrainRosetteCmds::setState(DIR_POINT_1_ACCEPTED);
      break;
    case DIR_POINT_1_ACCEPTED:
      break;
    case DIR_POINT_2_SELECTED:
      // Create based on dirP1 and dirP2
      ourCreatedStrainRosette->setGlobalAngleOriginVector(ourDirection);
      ourCreatedStrainRosette->draw();
      FapStrainRosetteCmds::setState(START);
      break;
    case DIR_EDGE_SELECTED:
      // Create based on edge direction
      ourCreatedStrainRosette->setGlobalAngleOriginVector(ourDirection);
      ourCreatedStrainRosette->draw();
      FapStrainRosetteCmds::setState(START);
      break;
    default:
      FuiModes::cancel();
      break;
    }
}


#ifdef USE_INVENTOR
void FapStrainRosetteCmds::eventCB(void*, SoEventCallback* eventCallbackNode)
{
  const SoEvent* event = eventCallbackNode->getEvent();
  if (!event) return;

  // Lambda function returning the snapped-to location of the picked point.
  auto&& findSnapPoint = [](const SoPickedPoint* point)
  {
    FdObject* pickedObject = FdPickFilter::findFdObject(point->getPath());
    if (!pickedObject)
      return FdConverter::toFaVec3(point->getPoint());

    return pickedObject->findSnapPoint(point->getObjectPoint(),
                                       point->getObjectToWorld(), NULL,
                                       const_cast<SoPickedPoint*>(point));
  };

  // Lambda function displaying the proposed rosette direction vectors.
  auto&& showRosetteDirection = [](const FaVec3& p0, const FaVec3& p1)
  {
    bool dummy;
    FaMat34 rosT = ourCreatedStrainRosette->getGlobSymbolPosMx(dummy);
    ourDirection = p1 - p0;
    ourDirection -= (rosT[2] * ourDirection) * rosT[2];
    FdExtraGraphics::showDirection(rosT[3], ourDirection);
  };

  if (event->isOfType(SoMouseButtonEvent::getClassTypeId()) &&
      SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
  {
    const SoPickedPoint* pPoint = eventCallbackNode->getPickedPoint();
    SoHandleEventAction* evHaAction = eventCallbackNode->getAction();
    const SoPickedPointList& ppl = evHaAction->getPickedPointList();

    FdObject* rosettePart = ourRosettePart ? ourRosettePart->getFdPointer() : NULL;
    FdPart*   fdpart = NULL;
    int    i, nodeID = -1;
    FaVec3 worldNodePos;

    switch (FapStrainRosetteCmds::ourState)
      {
      case START:
      case N_1_SELECTED:
        // Find first part in ppl
        if ((fdpart = FdPickFilter::findFirstPartHit(i,ppl)))
        {
          // Find actual position on the selected part
          if (fdpart->findNode(nodeID,worldNodePos,ppl[i]->getPoint()))
          {
            // Part is OK, store
            ourRosettePart = static_cast<FmPart*>(fdpart->getFmOwner());

            // Select node, and highlight it
            FdPickedPoints::selectNode(0,nodeID,worldNodePos);

            // Deselect previously selected instance
            // to put it as the last one selected
            if (FapEventManager::isPermSelected(ourRosettePart))
              FapEventManager::permUnselect(ourRosettePart);

            FapEventManager::permSelect(ourRosettePart);
            FapStrainRosetteCmds::setState(N_1_SELECTED);
          }
          else
          {
            Fui::dismissDialog("The FE part has to be loaded in order to add a strain gage to it.",FFuDialog::INFO);
            FapStrainRosetteCmds::setState(START);
          }
        }
        else
          FapStrainRosetteCmds::setState(START);
        break;

      case N_1_ACCEPTED:
      case N_2_SELECTED:
        if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,rosettePart)) {
          FdPickedPoints::selectNode(1,nodeID,worldNodePos);
          FapStrainRosetteCmds::setState(N_2_SELECTED);
        }
        break;

      case N_2_ACCEPTED:
      case N_3_SELECTED:
        if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,rosettePart)) {
          FdPickedPoints::selectNode(2,nodeID,worldNodePos);
          FapStrainRosetteCmds::setState(N_3_SELECTED);
        }
        break;

      case N_3_ACCEPTED:
      case N_4_SELECTED:
        if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,rosettePart)) {
          FdPickedPoints::selectNode(3,nodeID,worldNodePos);
          FapStrainRosetteCmds::setState(N_4_SELECTED);
        }
        break;

      case START_DIRECTION:
      case DIR_EDGE_SELECTED:
      case DIR_POINT_1_SELECTED:
        // Check whether we have a line or some other hit
        // (which we will regard as a point hit.)
        if (pPoint) {
          SoFullPath* path = (SoFullPath*)pPoint->getPath();
          const SoDetail* detail = path ? pPoint->getDetail(path->getTail()) : NULL;
          if (detail && detail->getTypeId() == SoLineDetail::getClassTypeId()) {
            // Picked a line, use that as direction
            SoCoordinate3* coords = FdPickFilter::findLastCoordNode(path);
            SoVertexProperty* vxProp = coords ? NULL : FdPickFilter::findLastVxPropNode(path);
            if (coords || vxProp) {
              SoLineDetail* linDet = (SoLineDetail*)detail;
              int cordIdx0 = linDet->getPoint0()->getCoordinateIndex();
              int cordIdx1 = linDet->getPoint1()->getCoordinateIndex();

              SbVec3f sbp0 = coords ? coords->point[cordIdx0] : vxProp->vertex[cordIdx0];
              SbVec3f sbp1 = coords ? coords->point[cordIdx1] : vxProp->vertex[cordIdx1];

              FaMat34 objToWorld = FdConverter::toFaMat34(pPoint->getObjectToWorld());
              FaVec3 p0 = objToWorld * FdConverter::toFaVec3(sbp0);
              FaVec3 p1 = objToWorld * FdConverter::toFaVec3(sbp1);
              showRosetteDirection(p0,p1);
              FdExtraGraphics::showLine(p0,p1);

              FdPickedPoints::resetPPs();
              FapStrainRosetteCmds::setState(DIR_EDGE_SELECTED);
            }
          }
          else {
            // Picked a point
            FdExtraGraphics::hideLine();
            FdExtraGraphics::hideDirection();
            FdPickedPoints::setFirstPP(findSnapPoint(pPoint),
                                       FdConverter::toFaMat34(pPoint->getObjectToWorld()));
            FapStrainRosetteCmds::setState(DIR_POINT_1_SELECTED);
          }
        }
        break;

      case DIR_POINT_1_ACCEPTED:
      case DIR_POINT_2_SELECTED:
        if (pPoint) {
          FdPickedPoints::setSecondPP(findSnapPoint(pPoint),
                                      FdConverter::toFaMat34(pPoint->getObjectToWorld()));
          showRosetteDirection(FdPickedPoints::getFirstPickedPoint(),
                               FdPickedPoints::getSecondPickedPoint());
          FapStrainRosetteCmds::setState(DIR_POINT_2_SELECTED);
        }
      }
    }

  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCallbackNode->setHandled();
}
#else
void FapStrainRosetteCmds::eventCB(void*, SoEventCallback*) {}
#endif


void FapStrainRosetteCmds::flipStrainRosetteZDirection()
{
  FmStrainRosette* ros = dynamic_cast<FmStrainRosette*>(FapEventManager::getFirstPermSelectedObject());
  if (!ros) return;

  ros->flipFaceNormal();
  ros->draw();

  FapUAProperties* uap = FapUAProperties::getPropertiesHandler();
  if (uap) uap->updateUIValues();
}
