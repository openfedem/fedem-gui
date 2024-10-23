// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapEditStrainRosetteDirCmd.H"
#include "vpmApp/FapEventManager.H"
#include "vpmDB/FmStrainRosette.H"

#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "vpmUI/FuiModes.H"
#include "vpmUI/Fui.H"

#ifdef USE_INVENTOR
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/details/SoLineDetail.h>
#include <Inventor/details/SoDetail.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoVertexProperty.h>

#include "vpmDisplay/FdPickFilter.H"
#include "vpmDisplay/FdPickedPoints.H"
#include "vpmDisplay/FdExtraGraphics.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDisplay/FdBase.H"
#include "vpmDisplay/FdEvent.H"
#endif

enum {
  START,
  DIR_POINT_1_SELECTED,
  DIR_POINT_1_ACCEPTED,
  DIR_POINT_2_SELECTED,
  DIR_POINT_2_ACCEPTED,
  DIR_EDGE_SELECTED
};

int FapEditStrainRosetteDirCmd::ourState = START;
FmStrainRosette* FapEditStrainRosetteDirCmd::ourStrainRosette = NULL;
FaVec3 FapEditStrainRosetteDirCmd::ourDirection;


void FapEditStrainRosetteDirCmd::init()
{
  FFuaCmdItem* cmdItem = new FFuaCmdItem("cmdId_editStrainRosetteDir");
  cmdItem->setText("Edit Strain Rosette orientation");
  cmdItem->setToolTip("Edit the reference direction of a strain rosette element");
  cmdItem->setActivatedCB(FFaDynCB0S(FapEditStrainRosetteDirCmd::editStrainRosetteDir));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapEditStrainRosetteDirCmd::getSensitivity,bool&));
}


void FapEditStrainRosetteDirCmd::editStrainRosetteDir()
{
  ourStrainRosette = dynamic_cast<FmStrainRosette*>(FapEventManager::getFirstPermSelectedObject());

  FuiModes::setMode(FuiModes::EDITSTRAINROSETTEDIR_MODE);
}


void FapEditStrainRosetteDirCmd::enterMode()
{
#ifdef USE_INVENTOR
  FdEvent::addEventCB(FapEditStrainRosetteDirCmd::eventCB);
#endif
  FapEditStrainRosetteDirCmd::setState(START); // To get the tip set right.
}


void FapEditStrainRosetteDirCmd::cancelMode()
{
#ifdef USE_INVENTOR
  FdPickedPoints::resetPPs();
  FdEvent::removeEventCB(FapEditStrainRosetteDirCmd::eventCB);
  FdExtraGraphics::hideLine();
  FdExtraGraphics::hideDirection();
  FdPickedPoints::resetPPs();
#endif
  FapEditStrainRosetteDirCmd::ourState = START;
  ourStrainRosette = NULL;
}


void FapEditStrainRosetteDirCmd::setState(int newState)
{
  switch (FapEditStrainRosetteDirCmd::ourState = newState)
    {
    case START:
#ifdef USE_INVENTOR
      FdPickedPoints::resetPPs();
#endif
      Fui::tip("Select a reference direction for the strain rosette. (Select Points or Lines)");
      break;
    case DIR_POINT_1_SELECTED:
      Fui::tip("Press Done to confirm the first point, or pick again to reconsider. (Select Points or Lines)");
      break;
    case DIR_POINT_1_ACCEPTED:
      Fui::tip("Select point number two for the reference direction.");
      break;
    case DIR_POINT_2_SELECTED:
      Fui::tip("Press Done to confirm the second point, or pick again to reconsider.");
      break;
    case DIR_EDGE_SELECTED:
      Fui::tip("Press Done to confirm the direction, or pick again to reconsider.");
      break;
    }
}


void FapEditStrainRosetteDirCmd::done()
{
 switch (FapEditStrainRosetteDirCmd::ourState)
    {
    case START:
      FapEditStrainRosetteDirCmd::setState(START);
      break;
    case DIR_POINT_1_SELECTED:
      FapEditStrainRosetteDirCmd::setState(DIR_POINT_1_ACCEPTED);
      break;
    case DIR_POINT_1_ACCEPTED:
      break;
    case DIR_POINT_2_SELECTED:
    case DIR_EDGE_SELECTED:
      ourStrainRosette->setGlobalAngleOriginVector(ourDirection);
      ourStrainRosette->draw();
    default:
      FuiModes::cancel();
      break;
    }
}


void FapEditStrainRosetteDirCmd::getSensitivity(bool& isSensitive)
{
  FapCmdsBase::isModelTouchable(isSensitive);
  if (!isSensitive) return;

  FapCmdsBase::isModellerActive(isSensitive);
  if (!isSensitive) return;

  isSensitive = dynamic_cast<FmStrainRosette*>(FapEventManager::getFirstPermSelectedObject()) != NULL;
}


#ifdef USE_INVENTOR
void FapEditStrainRosetteDirCmd::eventCB(void*, SoEventCallback* eventCallbackNode)
{
  if (!ourStrainRosette) return;

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
    FaMat34 rosT = ourStrainRosette->getGlobSymbolPosMx(dummy);
    ourDirection = p1 - p0;
    ourDirection -= (rosT[2] * ourDirection) * rosT[2];
    FdExtraGraphics::showDirection(rosT[3], ourDirection);
  };

  const SoPickedPoint* pPoint = eventCallbackNode->getPickedPoint();
  if (event->isOfType(SoMouseButtonEvent::getClassTypeId()) &&
      SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1))
    switch (FapEditStrainRosetteDirCmd::ourState)
      {
      case START:
      case DIR_EDGE_SELECTED:
      case DIR_POINT_1_SELECTED:

        // Check whether we have a line or some other hit
        // (which we will regard as a point hit)
        if (pPoint) {
          SoFullPath* path = (SoFullPath*)pPoint->getPath();
          const SoDetail* detail = pPoint->getDetail(path->getTail());
          if (detail->getTypeId() == SoLineDetail::getClassTypeId())
            {
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
                FapEditStrainRosetteDirCmd::setState(DIR_EDGE_SELECTED);
              }
            }
          else
            {
              FdExtraGraphics::hideLine();
              FdExtraGraphics::hideDirection();
              FdPickedPoints::setFirstPP(findSnapPoint(pPoint),
                                         FdConverter::toFaMat34(pPoint->getObjectToWorld()));
              FapEditStrainRosetteDirCmd::setState(DIR_POINT_1_SELECTED);
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

          FapEditStrainRosetteDirCmd::setState(DIR_POINT_2_SELECTED);
        }
        break;
      }

  else if (!SoMouseButtonEvent::isButtonReleaseEvent(event,SoMouseButtonEvent::BUTTON1))
    return;

  eventCallbackNode->setHandled();
}
#else
void FapEditStrainRosetteDirCmd::eventCB(void*, SoEventCallback*) {}
#endif
