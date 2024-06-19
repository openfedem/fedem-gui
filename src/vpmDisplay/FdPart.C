// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdPart.H"
#include "vpmDB/FmPart.H"

#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdPtPMoveAnimator.H"
#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdConverter.H"

#include "vpmDisplay/FdFEModelKit.H"
#include "vpmDisplay/FdFEGroupPart.H"
#include "FFlLib/FFlLinkHandler.H"
#include "FFlLib/FFlFEParts/FFlNode.H"
#include "FFlLib/FFlVisualization/FFlGroupPartCreator.H"
#include "FFdCadModel/FdCadHandler.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmGlobalViewSettings.H"
#include "vpmDB/FmSticker.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmRigidJoint.H"
#include "vpmDB/FmRevJoint.H"
#include "vpmDB/FmBallJoint.H"
#include "vpmDB/FmPrismJoint.H"
#include "vpmDB/FmCylJoint.H"

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoDrawStyle.h>


// To show vertex indexes as node labels : (Debug only _SLOW_)
//#define NODE_LABELS
// To show nodeids instead
//#define USE_NODEIDS_AS_LABELS


Fmd_SOURCE_INIT(FDPART,FdPart,FdLink);


FdPart::FdPart(FmPart* pt) : FdLink(pt)
{
  Fmd_CONSTRUCTOR_INIT(FdPart);

  myGroupPartCreator = NULL;
}


FdPart::~FdPart()
{
  delete myGroupPartCreator;
}


SoNodeKitListPart* FdPart::getListSw() const
{
  return SO_GET_PART(FdDB::getMechanismKit(),"partListSw",SoNodeKitListPart);
}


bool FdPart::updateFdDetails()
{
#ifdef FD_DEBUG
  std::cout <<"FdPart::updateFdDetails() "
            << itsFmOwner->getIdString(true) << std::endl;
#endif

  FmPart* part = static_cast<FmPart*>(itsFmOwner);
  if (part->isEarthLink())
  {
    this->updateSimplifiedViz();
    return true;
  }

  FdDB::setAutoRedraw(false);

  // Get settings from DB :

  int modelType = part->getModelType();
  int meshType  = part->getMeshType();
  bool solidMode = FmDB::getActiveViewSettings()->getSolidMode();
  bool solidWithEdges = FmDB::getActiveViewSettings()->getSolidModeWithEdges();

  // Select FE or CAD visualization.
  // Load the appropriate if not loaded already.

  if (!part->useGenericProperties.getValue())
    IAmUsingGenPartVis = false;
  else if (myCadHandler->getCadComponent())
    IAmUsingGenPartVis = this->createCadViz();
  else
    IAmUsingGenPartVis = this->loadVrmlViz();

  if (IAmUsingGenPartVis)
    myFEKit->setVizMode(FdFEGroupPart::SPECIAL);
  else
  {
    this->createFEViz();
    myFEKit->setVizMode(FdFEGroupPart::NORMAL);
  }

  if (solidMode && modelType != FmLink::OFF)
  {
    if (solidWithEdges && meshType != FmLink::OFF)
      myFEKit->setDrawStyle(FdFEVisControl::SOLID_LINES);
    else
      myFEKit->setDrawStyle(FdFEVisControl::SOLID);
  }
  else
  {
    if (meshType != FmLink::OFF)
      myFEKit->setDrawStyle(FdFEVisControl::LINES);
  }

  if (modelType == FmLink::OFF && meshType == FmLink::OFF)
  {
    myFEKit->setDrawDetail(FdFEVisControl::BBOX);
    myFEKit->show(false);
  }
  else
  {
    switch (modelType)
      {
      case FmLink::FULL:
        myFEKit->setDrawDetail(FdFEVisControl::FULL);
        break;

      case FmLink::RED_FULL:
        myFEKit->setDrawDetail(FdFEVisControl::RED_FULL);
        break;

      case FmLink::SURFACE:
        myFEKit->setDrawDetail(FdFEVisControl::SURFACE);
        break;

      case FmLink::REDUCED:
        myFEKit->setDrawDetail(FdFEVisControl::OUTLINE);
        break;

      default:
        myFEKit->setDrawDetail(FdFEVisControl::OFF);
        break;
      }

    switch (meshType)
      {
      case FmLink::RED_FULL:
      case FmLink::FULL:
        myFEKit->setLineDetail(FdFEVisControl::FULL);
        break;

      case FmLink::SURFACE:
        myFEKit->setLineDetail(FdFEVisControl::SURFACE);
        break;

      case FmLink::REDUCED:
        myFEKit->setLineDetail(FdFEVisControl::OUTLINE);
        break;

      case FmLink::REDUCED_NO1D:
        myFEKit->setLineDetail(FdFEVisControl::OUTLINE_NO1D);
        break;

      default:
        myFEKit->setLineDetail(FdFEVisControl::OFF);
        break;
      }

    myFEKit->show(true);
  }

  FdDB::setAutoRedraw(true);
  this->updateSimplifiedViz();

  return true;
}


bool FdPart::updateFdApperance()
{
  bool status = this->FdLink::updateFdApperance();

  myFEKit->myGroupParts.setLineWidth(FdFEGroupPartSet::SPECIAL_LINES,1.0);

  return status;
}


bool FdPart::updateSpecialLines(double scale)
{
  if (!myGroupPartCreator) return false;

  if (!myGroupPartCreator->recreateSpecialLines(scale))
    return false;

  myFEKit->deleteGroupParts(FdFEGroupPartSet::SPECIAL_LINES);
  myFEKit->updateVertexes(static_cast<FmPart*>(itsFmOwner)->getLinkHandler());

  for (const FFlGroupPartCreator::GroupPartMap::value_type& gp : myGroupPartCreator->getSpecialLines())
    myFEKit->addGroupPart(FdFEGroupPartSet::SPECIAL_LINES,gp.second);

  return true;
}


/*!
  This method creates the visualization data, and populates the FdFEModelKit
  and friends based on the FE data stored in the FFlLinkHandler object.
  You need to delete the existing visualization before calling this method.
  It returns true on success and false on failure.
*/

bool FdPart::createFEViz()
{
  if (myGroupPartCreator) return true; // We already have the data

  FmPart* part = static_cast<FmPart*>(itsFmOwner);
  FFlLinkHandler* linkHandler = part->getLinkHandler();
  if (!linkHandler) return false;

  myGroupPartCreator = new FFlGroupPartCreator(linkHandler);
  myGroupPartCreator->makeLinkParts();

  myFEKit->updateVertexes(linkHandler);
  myFEKit->updateGroupParts(myGroupPartCreator);

  myGroupPartCreator->deleteShapeIndexes();

  std::vector<FaMat34> internalCSs;
  linkHandler->getAllInternalCoordSys(internalCSs);
  for (const FaMat34& cs : internalCSs)
    myFEKit->addInternalCS(cs);

  myFEKit->setInternalCSToggle(FmDB::getActiveViewSettings()->visibleInternalPartCSs());

#ifdef NODE_LABELS // Make node labels on all parts, slow, debug only
  for (NodesCIter it = linkHandler->nodesBegin(); it != linkHandler->nodesEnd(); it++)
#ifdef USE_NODEIDS_AS_LABELS // if 1 use Node IDs else use vertex indices
    myFEKit->addLabel(std::to_string((*it)->getID()), (*it)->getPos());
#else
    myFEKit->addLabel(std::to_string((*it)->getVertexID()), (*it)->getPos());
#endif
#endif

  // Touch visualization
  myFEKit->setDrawDetail(FdFEVisControl::OFF);
  myFEKit->setLineDetail(FdFEVisControl::OFF);

  return true;
}


void FdPart::updateSimplifiedViz()
{
  this->showCS(FmDB::getActiveViewSettings()->visiblePartCS()); // TT 2201

  SoSwitch* spiderSwitch = SO_GET_PART(itsKit, "spiderSwitch", SoSwitch);
  SoSwitch* cgSymbolSwitch = SO_GET_PART(itsKit, "cgSymbolSwitch", SoSwitch);

  spiderSwitch->removeAllChildren();
  cgSymbolSwitch->removeAllChildren();

  SoSeparator* cgSep = new SoSeparator();
  cgSymbolSwitch->addChild(cgSep);
  SoTransform* transform = new SoTransform();

  FmPart* part = static_cast<FmPart*>(itsFmOwner);
  FaMat34 cgPartCS = part->getPositionCG(false);
  transform->setMatrix(FdConverter::toSbMatrix(cgPartCS));

  cgSep->addChild(transform);
  cgSep->addChild(FdSymbolDefs::getGlobalSymbolStyle());
  cgSep->addChild(FdSymbolDefs::getSymbol(FdSymbolDefs::CENTER_OF_GRAVITY));

  SoSeparator* sep = new SoSeparator();
  SoCoordinate3* coords = new SoCoordinate3();
  SoIndexedLineSet* lines = new SoIndexedLineSet();

  spiderSwitch->addChild(sep);
  sep->addChild(coords);
  sep->addChild(FdSymbolDefs::getGlobalSymbolStyle());
  sep->addChild(lines);

  // Fetching triads
  std::vector<FmTriad*> triads;
  part->getTriads(triads);
  size_t numTriads = triads.size();

  if (part->isGenericPart())
    coords->point.set1Value(0, FdConverter::toSbVec3f(cgPartCS.translation()));
  else if (numTriads == 2) {
    coords->point.set1Value(0, FdConverter::toSbVec3f(triads.front()->getLocalTranslation(part)));
    coords->point.set1Value(1, FdConverter::toSbVec3f(triads.back()->getLocalTranslation(part)));
    lines->coordIndex.set1Value(0, 0); // first triad
    lines->coordIndex.set1Value(1, 1); // second triad
    lines->coordIndex.set1Value(2,-1); // sep
    numTriads = 0;
  }
  else if (numTriads > 2) {
    // Calculating geometrical center
    FaVec3 center;
    for (FmTriad* triad : triads)
      center += triad->getLocalTranslation(part) / (double)numTriads;
    coords->point.set1Value(0, FdConverter::toSbVec3f(center));
  }
  else
    numTriads = 0;

  // Loop over triads to set up coords and indices
  for (size_t i = 0; i < numTriads; i++) {
    coords->point.set1Value(i+1, FdConverter::toSbVec3f(triads[i]->getLocalTranslation(part)));
    lines->coordIndex.set1Value(3*i  ,  0); // center point
    lines->coordIndex.set1Value(3*i+1,i+1); // this point
    lines->coordIndex.set1Value(3*i+2, -1); // sep
  }

  this->showCoGCS(FmDB::getActiveViewSettings()->visiblePartCoGCSs());

  int meshType = part->getMeshType();
  int modelType = part->getModelType();

  if (IAmUsingGenPartVis || part->getLinkHandler())
  {
    if (meshType == FmLink::SIMPLIFIED)
      spiderSwitch->whichChild.setValue(SO_SWITCH_ALL);
    else if (meshType != FmLink::OFF && IAmUsingGenPartVis && !this->myCadHandler->getCadComponent())
      spiderSwitch->whichChild.setValue(SO_SWITCH_ALL);
    else
      spiderSwitch->whichChild.setValue(SO_SWITCH_NONE);
  }
  else
  {
    if (meshType == FmLink::OFF && modelType == FmLink::OFF)
      spiderSwitch->whichChild.setValue(SO_SWITCH_NONE);
    else
      spiderSwitch->whichChild.setValue(SO_SWITCH_ALL);
  }
}


void FdPart::updateElementVisibility()
{
  if (static_cast<FmPart*>(itsFmOwner)->isEarthLink())
    return;

  if (myGroupPartCreator)
    myGroupPartCreator->updateElementVisibility();

  myFEKit->updateElementVisibility();
}


bool FdPart::updateFmOwner()
{
#ifdef FD_DEBUG
  std::cout <<"FdPart::updateFmOwner() "
            << itsFmOwner->getIdString(true) << std::endl;
#endif

  // put the actual transformation (from the kit) to the kit variable
  SoTransform* transform = SO_GET_PART(this->itsKit,"transform",SoTransform);
  SbMatrix updatedCS;
  updatedCS.setTransform(transform->translation.getValue(), transform->rotation.getValue(), SbVec3f(1,1,1));
  myFEKit->setTransform(FdConverter::toFaMat34(updatedCS));

  static_cast<FmIsPositionedBase*>(itsFmOwner)->setLocalCS(myFEKit->getTransform());

  return true;
}


void FdPart::smartMove(const FaVec3& p1, const FaVec3& p2, const FaDOF& dof)
{
  SbVec3f tempVec(0,0,0);
  SbRotation tempRotation;
  double angle = 0.0;

  switch (dof.getType())
    {
    case FREE:
      tempVec = FdConverter::toSbVec3f(p2 - p1);
      tempRotation = SbRotation::identity();
      break;

    case REV:
      angle = dof.rotationAngle(p1,p2);
      if (fabs(angle) <= 1.0e-6) return;
      tempRotation.setValue(FdConverter::toSbVec3f(dof.getDirection()),(float)angle);
      break;

    case BALL:
      if (p1.equals(dof.getCenter(),1.0e-7) || p2.equals(dof.getCenter(),1.0e-7))
        return;

      tempRotation.setValue(FdConverter::toSbVec3f(p1 - dof.getCenter()),
                            FdConverter::toSbVec3f(p2 - dof.getCenter()));
      break;

    case CYL:
      // Get translation along glider
      tempVec = FdConverter::toSbVec3f(dof.tangentVector(p1,p2));
      // Get rotation
      if (dof.normalVector(p1).isZero() || dof.normalVector(p2).isZero())
        // Rotation is undefined, just translate along glider
        tempRotation = SbRotation::identity();
      else
        tempRotation.setValue(FdConverter::toSbVec3f(p1 - dof.getCenter()),
                              FdConverter::toSbVec3f(p2 - dof.getCenter()));
      break;

    case PRISM:
      tempVec = FdConverter::toSbVec3f(dof.tangentVector(p1,p2));
      break;

    default:
      return;
    }

  SoTransform* transform = SO_GET_PART(this->itsKit,"transform",SoTransform);
  FdPtPMoveAnimator* animator = new FdPtPMoveAnimator(transform, this,
                                                      tempVec, tempRotation,
                                                      FdConverter::toSbVec3f(dof.getCenter()),
                                                      FdConverter::toSbVec3f(p1));
  animator->start();
}


SbVec3f FdPart::findSnapPoint(const SbVec3f& pointOnObject,
			      const SbMatrix& objToWorld,
			      SoDetail* detail, SoPickedPoint* pPoint)
{
  // PointOnObject is the local point, objToWorld is the global system

  if (IAmUsingGenPartVis)
    return this->FdLink::findSnapPoint(pointOnObject,objToWorld,detail,pPoint);

  FFlLinkHandler* linkHandler = static_cast<FmPart*>(itsFmOwner)->getLinkHandler();
  if (!linkHandler)
    return this->FdLink::findSnapPoint(pointOnObject,objToWorld,detail,pPoint);

  SbVec3f nearestWorld;
  NodesCIter node = linkHandler->findClosestNode(FdConverter::toFaVec3(pointOnObject));
  if (node != linkHandler->nodesEnd())
    objToWorld.multVecMatrix(FdConverter::toSbVec3f((*node)->getPos()),nearestWorld);
  else
    objToWorld.multVecMatrix(pointOnObject,nearestWorld);

  return nearestWorld;
}


bool FdPart::findNode(int& nodeID, FaVec3& worldNodePos, const SbVec3f& pickPoint) const
{
  FFlLinkHandler* linkHandler = static_cast<FmPart*>(itsFmOwner)->getLinkHandler();
  if (!linkHandler) return false;

  FaMat34 partTrans = this->getActiveTransform();
  FaVec3  partPoint = partTrans.inverse()*FdConverter::toFaVec3(pickPoint);
  NodesCIter nodeIt = linkHandler->findClosestNode(partPoint);
  if (nodeIt == linkHandler->nodesEnd()) return false;

  nodeID = (*nodeIt)->getID();
  worldNodePos = partTrans * (*nodeIt)->getPos();
  return true;
}


int FdPart::getDegOfFreedom(SbVec3f& centerPoint, SbVec3f& direction)
{
  centerPoint.setValue(0,0,0);
  direction.setValue(1,0,0);

  FmPart* part = static_cast<FmPart*>(itsFmOwner);
  if (part->isEarthLink())
    return RIGID;

  SbVec3f tempVec1;
  SbVec3f tempVec2;

  int currentDOFs = FREE;

  // First, check the stickers, if any

  std::vector<FmSticker*> stickers;
  for (part->getStickers(stickers); !stickers.empty(); stickers.pop_back())
  {
    FmSticker* sticker = stickers.back();
    switch (currentDOFs)
      {
      case RIGID:
      case PRISM:
        return RIGID;

      case CYL:
        // Get vector between sticker and cylinder axis
        tempVec2 = centerPoint - FdConverter::toSbVec3f(sticker->getPoint());
        // Test if sticker is on cylinder axis
        if (isParallell(direction,tempVec2))
          currentDOFs = REV;
        else
          return RIGID;
        break;

      case REV:
        // Get vector between sticker and revolution axis
        tempVec2 = centerPoint - FdConverter::toSbVec3f(sticker->getPoint());
        // Test if sticker is on revolution axis
        if (isParallell(direction,tempVec2))
          break;
        else
          return RIGID;

      case BALL:
        tempVec2 = FdConverter::toSbVec3f(sticker->getPoint());
        if (tempVec2 != centerPoint)
        {
          currentDOFs = REV;
          direction = centerPoint - tempVec2;
        }
        break;

      case FREE:
        currentDOFs = BALL;
        centerPoint = FdConverter::toSbVec3f(sticker->getPoint());
        break;
      }
  }

  // Then, if the part is still movable, check the joints, if any

  std::vector<FmJointBase*> joints;
  part->getJoints(joints);

  while (!joints.empty())
  {
    // Skip joints that are not attached to another part
    FmJointBase* joint = joints.back();
    while (!joint->isAttachedToLink(part))
      if (joints.empty())
        return currentDOFs;
      else
      {
        joints.pop_back();
        joint = joints.back();
      }

    FaMat34 triadCS = joint->getSlaveTriad()->getGlobalCS();
    switch (currentDOFs)
      {
      case REV:
        if (joint->isOfType(FmRigidJoint::getClassTypeID()))
          return RIGID;

        else if (joint->isOfType(FmRevJoint::getClassTypeID()))
        {
          // Get vector between revolution axis and the revolute joint
          tempVec2 = centerPoint - FdConverter::toSbVec3f(triadCS.translation());

          // Test if revolution axis is on the revolute joint axis
          if (!isParallell(direction,tempVec2))
            return RIGID;

          tempVec2 = FdConverter::toSbVec3f(triadCS[2]); // Z-axis
          if (!isParallell(direction,tempVec2))
            return RIGID;
        }

        else if (joint->isOfType(FmBallJoint::getClassTypeID()))
        {
          // Get vector between revolution axis and ball joint
          tempVec2 = centerPoint - FdConverter::toSbVec3f(triadCS.translation());

          // Test if ball joint is on revolution axis
          if (!isParallell(direction,tempVec2))
            return RIGID;
        }

        if (joint->isOfType(FmPrismJoint::getClassTypeID()))
          return RIGID;

        else if (joint->isOfType(FmCylJoint::getClassTypeID()))
        {
          // Get vector between revolution axis and cylindric joint axis
          tempVec2 = centerPoint - FdConverter::toSbVec3f(triadCS.translation());

          // Test if revolution axis is on the cylindric joint axis
          if (!isParallell(direction,tempVec2))
            return RIGID;

          tempVec2 = FdConverter::toSbVec3f(triadCS[2]); // Z-axis
          if (!isParallell(direction,tempVec2))
            return RIGID;
        }
        break;

      case PRISM:
        if (joint->isOfType(FmRigidJoint::getClassTypeID()) ||
            joint->isOfType(FmRevJoint::getClassTypeID())   ||
            joint->isOfType(FmBallJoint::getClassTypeID()))
          return RIGID;

        else if (joint->isOfType(FmPrismJoint::getClassTypeID()) ||
                 joint->isOfType(FmCylJoint::getClassTypeID()))
        {
          tempVec2 = FdConverter::toSbVec3f(triadCS[2]); // Z-axis
          if (!isParallell(direction,tempVec2))
            return RIGID;
        }
        break;

      case CYL:
        if (joint->isOfType(FmRigidJoint::getClassTypeID()))
          return RIGID;

        else if (joint->isOfType(FmRevJoint::getClassTypeID()))
        {
          // Get vector between cylinder axis and revolute joint axis
          tempVec2 = centerPoint - FdConverter::toSbVec3f(triadCS.translation());

          // Test if cylinder axis is on the revolute joint axis
          if (!isParallell(direction,tempVec2))
            return RIGID;

          tempVec2 = FdConverter::toSbVec3f(triadCS[2]); // Z-axis
          if (!isParallell(direction,tempVec2))
            return RIGID;

          currentDOFs = REV;
        }

        else if (joint->isOfType(FmBallJoint::getClassTypeID()))
        {
          // Get vector between cylinder and ball joint
          tempVec2 = centerPoint - FdConverter::toSbVec3f(triadCS.translation());

          // Test if cylinder axis goes through the ball joint
          if (isParallell(direction,tempVec2))
            return RIGID;

          currentDOFs = REV;
        }

        else if (joint->isOfType(FmPrismJoint::getClassTypeID()))
        {
          // Get prismatic joint direction
          tempVec2 = FdConverter::toSbVec3f(triadCS[2]); // Z-axis

          // Test if the cylinder axis is paralell with the prismatic joint
          if (!isParallell(direction,tempVec2))
            return RIGID;

          currentDOFs = PRISM;
        }

        else if (joint->isOfType(FmCylJoint::getClassTypeID()))
        {
          // Get cylindric joint direction
          tempVec2 = FdConverter::toSbVec3f(triadCS[2]); // Z-axis

          // Test if the cylinder axis is paralell with the cylindric joint
          if (!isParallell(direction,tempVec2))
            return RIGID;

          // Get vector between cylinder and cylindric joint
          tempVec2 = centerPoint - FdConverter::toSbVec3f(triadCS.translation());

          // Test if cylinder is on the cylindric joint axis
          if (!isParallell(direction,tempVec2))
            currentDOFs = PRISM;
        }
        break;

      case BALL:
        if (joint->isOfType(FmRigidJoint::getClassTypeID()))
          return RIGID;

        else if (joint->isOfType(FmRevJoint::getClassTypeID()))
        {
          // Get the revolute joint axis direction
          tempVec1 = FdConverter::toSbVec3f(triadCS[2]);
          // Get vector between "ball" and revolute joint
          tempVec2 = centerPoint - FdConverter::toSbVec3f(triadCS.translation());

          // Test if ball is on the revolute joint axis
          if (!isParallell(tempVec1,tempVec2))
            return RIGID;

          direction = tempVec1; // Leave centerPoint as it was
          currentDOFs = REV;
        }

        else if (joint->isOfType(FmBallJoint::getClassTypeID()))
        {
          tempVec1 = FdConverter::toSbVec3f(triadCS.translation());
          if (centerPoint != tempVec1)
          {
            currentDOFs = REV;
            direction = centerPoint - tempVec1;
          }
        }

        else if (joint->isOfType(FmPrismJoint::getClassTypeID()))
          return RIGID;

        else if (joint->isOfType(FmCylJoint::getClassTypeID()))
        {
          // Get cylindric joint axis direction
          tempVec1 = FdConverter::toSbVec3f(triadCS[2]);
          // Get vector between "ball" and cylindric joint
          tempVec2 = centerPoint - FdConverter::toSbVec3f(triadCS.translation());

          // Test if ball is on cylindric joint axis
          if (!isParallell(tempVec1,tempVec2))
            return RIGID;

          direction = tempVec1; // Leave centerPoint as it was
          currentDOFs = REV;
        }
        break;

      case FREE:
        if (joint->isOfType(FmRigidJoint::getClassTypeID()))
          return RIGID;

        else if (joint->isOfType(FmRevJoint::getClassTypeID()))
        {
          currentDOFs = REV;
          centerPoint = FdConverter::toSbVec3f(triadCS.translation());
          direction   = FdConverter::toSbVec3f(triadCS[2]); // Z-axis
        }
        else if (joint->isOfType(FmPrismJoint::getClassTypeID()))
        {
          currentDOFs = PRISM;
          direction   = FdConverter::toSbVec3f(triadCS[2]); // Z-axis
        }
        else if (joint->isOfType(FmCylJoint::getClassTypeID()))
        {
          currentDOFs = CYL;
          centerPoint = FdConverter::toSbVec3f(triadCS.translation());
          direction   = FdConverter::toSbVec3f(triadCS[2]); // Z-axis
        }
        else if (joint->isOfType(FmBallJoint::getClassTypeID()))
        {
          currentDOFs = BALL;
          centerPoint = FdConverter::toSbVec3f(triadCS.translation());
        }
        break;
      }
  }

  return currentDOFs;
}


void FdPart::showHighlight()
{
  if (!FdDB::isUsingLineHighlight())
    this->FdLink::showHighlight();
  else if (IAmUsingGenPartVis && !this->myCadHandler->getCadComponent())
    this->FdLink::showHighlight();
  else
  {
    FmPart* part = static_cast<FmPart*>(itsFmOwner);
    int meshType  = part->getMeshType();
    int solidType = part->getModelType();

    if (meshType == FmLink::REDUCED_NO1D)
      myFEKit->highlight(true,FdFEVisControl::OUTLINE_NO1D);
    else if (meshType != FmLink::OFF && meshType != FmLink::SIMPLIFIED && meshType != FmLink::BBOX)
      myFEKit->highlight(true,FdFEVisControl::OUTLINE);
    else if (solidType != FmLink::OFF && solidType != FmLink::SIMPLIFIED && solidType != FmLink::BBOX)
      myFEKit->highlight(true,FdFEVisControl::OUTLINE_NO1D);

    itsKit->setPart("symbolMaterial",FdSymbolDefs::getHighlightMaterial());
  }
}


void FdPart::showInternalCSs(bool doShow)
{
  myFEKit->setInternalCSToggle(doShow);
}


void FdPart::showCoGCS(bool doShow)
{
  if (static_cast<FmPart*>(itsFmOwner)->useGenericProperties.getValue())
    myFEKit->setCoGCSToggle(doShow);
  else
    myFEKit->setCoGCSToggle(false);
}


/*!
  Removes all visualization data by removing the inventor kit completly.
*/

void FdPart::removeDisplayData()
{
  this->FdLink::removeDisplayData();

  delete myGroupPartCreator;
  myGroupPartCreator = NULL;
}


/*!
  Removes all visualization data by removing the data from the inventor kit.
  It retains the inventor kits, and the rigid body frames if present.
*/

void FdPart::removeVisualizationData(bool removeCadDataToo)
{
  this->FdLink::removeVisualizationData(removeCadDataToo);

  delete myGroupPartCreator;
  myGroupPartCreator = NULL;
}
