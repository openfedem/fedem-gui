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


namespace
{
  enum State {
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

  State            ourState   = START;
  FmStrainRosette* ourRosette = NULL;
  FmPart*          ourFEPart  = NULL;
  FaVec3           ourDirection;

  //! \brief Convenience method returning the currently selected strain rosette.
  FmStrainRosette* selectedStrainRosette()
  {
    FFaViewItem* selectedItem = FapEventManager::getFirstPermSelectedObject();
    return dynamic_cast<FmStrainRosette*>(selectedItem);
  }

  //! \brief Updates \a ourState in the rosette creation process and sets tip.
  void setCreateRosetteState(State newState)
  {
    switch ((ourState = newState))
      {
      case START:
        ourDirection = FaVec3(1.0,0.0,0.0);
        if (ourRosette)
          ListUI <<"Created "<< ourRosette->getIdString()
                 <<" on "<< ourFEPart->getIdString() <<".\n";
        ourRosette = NULL;
        ourFEPart = NULL;
        FapEventManager::permUnselectAll();
#ifdef USE_INVENTOR
        FdPickedPoints::deselectNode();
        FdPickedPoints::resetPPs();
        FdExtraGraphics::hideLine();
        FdExtraGraphics::hideDirection();
#endif
        Fui::tip("Select first node of the strain gage element");
        break;
      case N_1_SELECTED:
        Fui::tip("Press Done to confirm the first node, "
                 "or pick again to select a different node");
        break;
      case N_1_ACCEPTED:
        Fui::tip("Select second node of the strain gage element");
        break;
      case N_2_SELECTED:
        Fui::tip("Press Done to confirm the second node, "
                 "or pick again to select a different node");
        break;
      case N_2_ACCEPTED:
        Fui::tip("Select third node of the strain gage element");
        break;
      case N_3_SELECTED:
        Fui::tip("Press Done to confirm the third node, "
                 "or pick again to select a different node");
        break;
      case N_3_ACCEPTED:
        Fui::tip("Select fourth node of the strain gage element, "
                 "or press Done to create a three node element");
        break;
      case N_4_SELECTED:
        Fui::tip("Press Done to confirm the fourth node, "
                 "or pick again to select a different node");
        break;
      case START_DIRECTION:
        Fui::tip("Select a reference direction for the strain rosette "
                 "(select Points or Lines)");
        break;
      case DIR_POINT_1_SELECTED:
#ifdef USE_INVENTOR
        FdPickedPoints::deselectNode();
        FdExtraGraphics::hideLine();
        FdExtraGraphics::hideDirection();
#endif
        Fui::tip("Press Done to confirm the first point, "
                 "or pick again to reconsider (select Points or Lines)");
        break;
      case DIR_POINT_1_ACCEPTED:
        Fui::tip("Select second point for the reference direction");
        break;
      case DIR_POINT_2_SELECTED:
        Fui::tip("Press Done to confirm the second point, "
                 "or pick again to reconsider");
        break;
      case DIR_EDGE_SELECTED:
        Fui::tip("Press Done to confirm the direction, "
                 "or pick again to reconsider");
        break;
      default:
        break;
      }
  }

#ifdef USE_INVENTOR

  //! \brief Helper returning the snapped-to location of the picked point.
  FaVec3 findSnapPoint(const SoPickedPoint* point)
  {
    FdObject* pickedObject = FdPickFilter::findFdObject(point->getPath());
    if (!pickedObject) return FdConverter::toFaVec3(point->getPoint());

    return pickedObject->findSnapPoint(point->getObjectPoint(),
                                       point->getObjectToWorld(), NULL,
                                       const_cast<SoPickedPoint*>(point));
  }

  //! \brief Helper displaying the proposed rosette direction vectors.
  void showRosetteDirection(const FaVec3& p0, const FaVec3& p1)
  {
    FaMat34 rosT = ourRosette->getSymbolPosMx(true);
    ourDirection = p1 - p0;
    ourDirection -= (rosT[2] * ourDirection) * rosT[2];
    FdExtraGraphics::showDirection(rosT[3], ourDirection);
  }

  //! \brief Helper checking whether the picked \a pPoint is on a line or not.
  bool checkLinePick(const SoPickedPoint* pPoint)
  {
    SoFullPath* path = (SoFullPath*)pPoint->getPath();
    if (!path) return false;

    const SoDetail* detail = pPoint->getDetail(path->getTail());
    if (!detail || detail->getTypeId() != SoLineDetail::getClassTypeId())
      return false; // picked a point

    // Picked a line, use that as direction
    SoLineDetail* linDet = (SoLineDetail*)detail;
    int cordIdx0 = linDet->getPoint0()->getCoordinateIndex();
    int cordIdx1 = linDet->getPoint1()->getCoordinateIndex();

    std::array<SbVec3f,2> sbp;
    if (SoCoordinate3* coord = FdPickFilter::findLastCoordNode(path); coord)
      sbp = { coord->point[cordIdx0], coord->point[cordIdx1] };
    else if (SoVertexProperty* vx = FdPickFilter::findLastVxPropNode(path); vx)
      sbp = { vx->vertex[cordIdx0], vx->vertex[cordIdx1] };
    else
      return false;

    FaMat34 objToWorld = FdConverter::toFaMat34(pPoint->getObjectToWorld());
    FaVec3 p0 = objToWorld * FdConverter::toFaVec3(sbp[0]);
    FaVec3 p1 = objToWorld * FdConverter::toFaVec3(sbp[1]);
    showRosetteDirection(p0,p1);
    FdExtraGraphics::showLine(p0,p1);
    return true;
  }

  //! \brief Event call-back for creating a new strain rosette.
  void createRosetteCB(void*, SoEventCallback* eventCallbackNode)
  {
    const SoEvent* e = eventCallbackNode->getEvent();
    if (!e) return;

    if (e->isOfType(SoMouseButtonEvent::getClassTypeId()) &&
        SoMouseButtonEvent::isButtonPressEvent(e,SoMouseButtonEvent::BUTTON1))
    {
      SoHandleEventAction* evHaAction = eventCallbackNode->getAction();
      const SoPickedPointList& ppl = evHaAction->getPickedPointList();

      FdObject* rosettePart = ourFEPart ? ourFEPart->getFdPointer() : NULL;
      int    i, nodeID = -1;
      FaVec3 worldNodePos;

      switch (ourState)
        {
        case START:
        case N_1_SELECTED:
          // Find first part in ppl
          if (FdPart* fdpart = FdPickFilter::findFirstPartHit(i,ppl); fdpart)
          {
            // Find actual position on the selected part
            if (fdpart->findNode(nodeID,worldNodePos,ppl[i]->getPoint()))
            {
              // Select first node, and highlight it
              FdPickedPoints::selectNode(0,nodeID,worldNodePos);
              setCreateRosetteState(N_1_SELECTED);

              // Store the associated FE Part
              ourFEPart = static_cast<FmPart*>(fdpart->getFmOwner());

              // Deselect previously selected instance
              // to put it as the last one selected
              if (FapEventManager::isPermSelected(ourFEPart))
                FapEventManager::permUnselect(ourFEPart);
              FapEventManager::permSelect(ourFEPart);
              break;
            }
            else
              Fui::dismissDialog("The FE part has to be loaded in order to "
                                 "add a strain gage to it.",FFuDialog::INFO);
          }

          // No or invalid hit, start over
          setCreateRosetteState(START);
          break;

        case N_1_ACCEPTED:
        case N_2_SELECTED:
          if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,rosettePart))
          {
            FdPickedPoints::selectNode(1,nodeID,worldNodePos);
            setCreateRosetteState(N_2_SELECTED);
          }
          break;

        case N_2_ACCEPTED:
        case N_3_SELECTED:
          if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,rosettePart))
          {
            FdPickedPoints::selectNode(2,nodeID,worldNodePos);
            setCreateRosetteState(N_3_SELECTED);
          }
          break;

        case N_3_ACCEPTED:
        case N_4_SELECTED:
          if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,rosettePart))
          {
            FdPickedPoints::selectNode(3,nodeID,worldNodePos);
            setCreateRosetteState(N_4_SELECTED);
          }
          break;

        case START_DIRECTION:
        case DIR_EDGE_SELECTED:
        case DIR_POINT_1_SELECTED:
          // Check whether we have a line or some other hit
          // (which we will regard as a point hit)
          if (const SoPickedPoint* pPoint = eventCallbackNode->getPickedPoint(); pPoint)
          {
            if (checkLinePick(pPoint))
            {
              // Picked a line
              FdPickedPoints::resetPPs();
              setCreateRosetteState(DIR_EDGE_SELECTED);
            }
            else
            {
              // Picked a point
              FdPickedPoints::setFirstPP(findSnapPoint(pPoint),
                                         FdConverter::toFaMat34(pPoint->getObjectToWorld()));
              setCreateRosetteState(DIR_POINT_1_SELECTED);
            }
          }
          break;

        case DIR_POINT_1_ACCEPTED:
        case DIR_POINT_2_SELECTED:
          if (const SoPickedPoint* pPoint = eventCallbackNode->getPickedPoint(); pPoint)
          {
            FdPickedPoints::setSecondPP(findSnapPoint(pPoint),
                                        FdConverter::toFaMat34(pPoint->getObjectToWorld()));
            showRosetteDirection(FdPickedPoints::getFirstPickedPoint(),
                                 FdPickedPoints::getSecondPickedPoint());
            setCreateRosetteState(DIR_POINT_2_SELECTED);
          }
          break;

        default:
          break;
        }
      }

    else if (!SoMouseButtonEvent::isButtonReleaseEvent(e,SoMouseButtonEvent::BUTTON1))
      return;

    eventCallbackNode->setHandled();
  }

  //! \brief Event call-back for editing strain rosette direction.
  void editDirCB(void*, SoEventCallback* eventCallbackNode)
  {
    const SoEvent* e = eventCallbackNode->getEvent();
    if (!e || !ourRosette) return;

    if (e->isOfType(SoMouseButtonEvent::getClassTypeId()) &&
        SoMouseButtonEvent::isButtonPressEvent(e,SoMouseButtonEvent::BUTTON1))

      switch (ourState)
        {
        case START_DIRECTION:
        case DIR_EDGE_SELECTED:
        case DIR_POINT_1_SELECTED:
          // Check whether we have a line or some other hit
          // (which we will regard as a point hit)
          if (const SoPickedPoint* pPoint = eventCallbackNode->getPickedPoint(); pPoint)
          {
            if (checkLinePick(pPoint))
            {
              // Picked a line
              FdPickedPoints::resetPPs();
              setCreateRosetteState(DIR_EDGE_SELECTED);
            }
            else
            {
              // Picked a point
              FdPickedPoints::setFirstPP(findSnapPoint(pPoint),
                                         FdConverter::toFaMat34(pPoint->getObjectToWorld()));
              setCreateRosetteState(DIR_POINT_1_SELECTED);
            }
          }
          break;

        case DIR_POINT_1_ACCEPTED:
        case DIR_POINT_2_SELECTED:
          if (const SoPickedPoint* pPoint = eventCallbackNode->getPickedPoint(); pPoint)
          {
            FdPickedPoints::setSecondPP(findSnapPoint(pPoint),
                                        FdConverter::toFaMat34(pPoint->getObjectToWorld()));
            showRosetteDirection(FdPickedPoints::getFirstPickedPoint(),
                                 FdPickedPoints::getSecondPickedPoint());
            setCreateRosetteState(DIR_POINT_2_SELECTED);
          }
          break;

        default:
          break;
        }

    else if (!SoMouseButtonEvent::isButtonReleaseEvent(e,SoMouseButtonEvent::BUTTON1))
      return;

    eventCallbackNode->setHandled();
  }

  //! \brief Event call-back for editing strain rosette nodes.
  void editNodesCB(void*, SoEventCallback* eventCallbackNode)
  {
    const SoEvent* e = eventCallbackNode->getEvent();
    if (!e || !ourRosette || !ourFEPart) return;

    if (e->isOfType(SoMouseButtonEvent::getClassTypeId()) &&
        SoMouseButtonEvent::isButtonPressEvent(e,SoMouseButtonEvent::BUTTON1))
    {
      SoHandleEventAction* evHaAction = eventCallbackNode->getAction();
      const SoPickedPointList& ppl = evHaAction->getPickedPointList();

      FdObject* rosettePart = ourFEPart->getFdPointer();
      int    nodeID = -1;
      FaVec3 worldNodePos;

      switch (ourState)
        {
        case START:
        case N_1_SELECTED:
          if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,rosettePart))
          {
            FdPickedPoints::selectNode(0,nodeID,worldNodePos);
            setCreateRosetteState(N_1_SELECTED);
          }
          break;

        case N_1_ACCEPTED:
        case N_2_SELECTED:
          if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,rosettePart))
          {
            FdPickedPoints::selectNode(1,nodeID,worldNodePos);
            setCreateRosetteState(N_2_SELECTED);
          }
          break;

        case N_2_ACCEPTED:
        case N_3_SELECTED:
          if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,rosettePart))
          {
            FdPickedPoints::selectNode(2,nodeID,worldNodePos);
            setCreateRosetteState(N_3_SELECTED);
          }
          break;

        case N_3_ACCEPTED:
        case N_4_SELECTED:
          if (FdPickFilter::findNodeHit(nodeID,worldNodePos,ppl,rosettePart))
          {
            FdPickedPoints::selectNode(3,nodeID,worldNodePos);
            setCreateRosetteState(N_4_SELECTED);
          }
          break;

        default:
          break;
        }
    }

    else if (!SoMouseButtonEvent::isButtonReleaseEvent(e,SoMouseButtonEvent::BUTTON1))
      return;

    eventCallbackNode->setHandled();
  }
#endif
}


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
  FdEvent::addEventCB(createRosetteCB);
#endif
  setCreateRosetteState(START);
}


void FapStrainRosetteCmds::cancelMode()
{
  FapEventManager::permUnselectAll();
#ifdef USE_INVENTOR
  FdPickedPoints::deselectNode();
  FdPickedPoints::resetPPs();
  FdExtraGraphics::hideDirection();
  FdExtraGraphics::hideLine();
  FdEvent::removeEventCB(createRosetteCB);
#endif
  ourState  = START;
  ourFEPart = NULL;

  if (ourRosette)
    ourRosette->erase();
  ourRosette = NULL;
}


void FapStrainRosetteCmds::done()
{
  switch (ourState)
    {
    case N_1_SELECTED:
      setCreateRosetteState(N_1_ACCEPTED);
      break;
    case N_2_SELECTED:
      setCreateRosetteState(N_2_ACCEPTED);
      break;
    case N_3_SELECTED:
      setCreateRosetteState(N_3_ACCEPTED);
      break;
    case N_1_ACCEPTED:
    case N_2_ACCEPTED:
      // Need at least 3 nodes
      break;
    case N_3_ACCEPTED:
    case N_4_SELECTED:
      ourRosette = new FmStrainRosette();
#ifdef USE_INVENTOR
      ourRosette->setTopology(ourFEPart,FdPickedPoints::getSelectedNodes());
#endif
      ourRosette->angleOriginVector.setValue(ourDirection);
      ourRosette->connect();
      ourRosette->syncWithFEModel(true);
      ourRosette->draw();
      setCreateRosetteState(START_DIRECTION);
      break;
    case START_DIRECTION:
      setCreateRosetteState(START);
      break;
    case DIR_POINT_1_SELECTED:
      setCreateRosetteState(DIR_POINT_1_ACCEPTED);
      break;
    case DIR_POINT_1_ACCEPTED:
      break;
    case DIR_POINT_2_SELECTED:
    case DIR_EDGE_SELECTED:
      // Create based on dirP1 and dirP2 or edge direction
      ourRosette->setGlobalAngleOriginVector(ourDirection);
      ourRosette->draw();
      setCreateRosetteState(START);
      break;
    default:
      FuiModes::cancel();
      break;
    }
}


void FapStrainRosetteCmds::flipStrainRosetteZDirection()
{
  if (FmStrainRosette* ros = selectedStrainRosette(); ros)
  {
    ros->flipFaceNormal();
    ros->draw();

    if (FapUAProperties* uap = FapUAProperties::getPropertiesHandler(); uap)
      uap->updateUIValues();
  }
}


void FapEditStrainRosetteDirCmd::enterMode()
{
#ifdef USE_INVENTOR
  FdEvent::addEventCB(editDirCB);
  FdPickedPoints::resetPPs();
#endif
  Fui::tip("Select a reference direction for the strain rosette "
           "(select Points or Lines)");
  ourState   = START_DIRECTION;
  ourRosette = selectedStrainRosette();
}


void FapEditStrainRosetteDirCmd::cancelMode()
{
#ifdef USE_INVENTOR
  FdEvent::removeEventCB(editDirCB);
  FdExtraGraphics::hideLine();
  FdExtraGraphics::hideDirection();
  FdPickedPoints::resetPPs();
#endif
  ourState   = START_DIRECTION;
  ourRosette = NULL;
}


void FapEditStrainRosetteDirCmd::done()
{
  switch (ourState)
    {
    case START_DIRECTION:
#ifdef USE_INVENTOR
      FdPickedPoints::resetPPs();
#endif
      Fui::tip("Select a reference direction for the strain rosette "
               "(select Points or Lines)");
      break;
    case DIR_POINT_1_SELECTED:
      Fui::tip("Press Done to confirm the first point, "
               "or pick again to reconsider (select Points or Lines)");
      ourState = DIR_POINT_1_ACCEPTED;
      break;
    case DIR_POINT_1_ACCEPTED:
      break;
    case DIR_POINT_2_SELECTED:
    case DIR_EDGE_SELECTED:
      ourRosette->setGlobalAngleOriginVector(ourDirection);
      ourRosette->draw();
    default:
      FuiModes::cancel();
      break;
    }
}


void FapEditStrainRosetteNodesCmd::enterMode()
{
#ifdef USE_INVENTOR
  FdEvent::addEventCB(editNodesCB);
  FdPickedPoints::resetPPs();
  FdPickedPoints::deselectNode();
#endif
  Fui::tip("Select first node of the strain gage element");
  ourState   = START;
  ourRosette = selectedStrainRosette();
  ourFEPart  = ourRosette->rosetteLink.getPointer();
}


void FapEditStrainRosetteNodesCmd::cancelMode()
{
#ifdef USE_INVENTOR
  FdPickedPoints::resetPPs();
  FdEvent::removeEventCB(editNodesCB);
#endif
  ourState   = START;
  ourRosette = NULL;
  ourFEPart  = NULL;
}


void FapEditStrainRosetteNodesCmd::done()
{
  switch (ourState)
    {
    case START:
      break;
    case N_1_SELECTED:
      Fui::tip("Select second node of the strain gage element");
      ourState = N_1_ACCEPTED;
      break;
    case N_1_ACCEPTED:
      break;
    case N_2_SELECTED:
      Fui::tip("Select third node of the strain gage element");
      ourState = N_2_ACCEPTED;
      break;
    case N_2_ACCEPTED:
      break;
    case N_3_SELECTED:
      Fui::tip("Select fourth node of the strain gage element, "
               "or press Done to finish");
      ourState = N_3_ACCEPTED;
      break;
    case N_3_ACCEPTED:
    case N_4_SELECTED:
#ifdef USE_INVENTOR
      ourRosette->setTopology(NULL,FdPickedPoints::getSelectedNodes());
#endif
      ourRosette->syncWithFEModel(true);
      ourRosette->draw();
      if (FapUAProperties* uap = FapUAProperties::getPropertiesHandler(); uap)
        uap->updateUIValues();

    default:
      FuiModes::cancel();
      break;
    }
}
