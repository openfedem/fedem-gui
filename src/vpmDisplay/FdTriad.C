// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdTriad.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdTriadSwKit.H"
#include "vpmDisplay/FdTransformKit.H"
#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDisplay/FdPtPMoveAnimator.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmLink.H"
#include "vpmDB/FmSticker.H"
#include "vpmDB/FmRigidJoint.H"
#include "vpmDB/FmRevJoint.H"
#include "vpmDB/FmBallJoint.H"
#include "vpmDB/FmFreeJoint.H"
#include "vpmDB/FmPrismJoint.H"
#include "vpmDB/FmCylJoint.H"
#include "vpmDB/FmCamJoint.H"
#include "vpmDB/FmDB.H"

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#ifdef USE_SMALLCHANGE
#include <SmallChange/nodes/SmDepthBuffer.h>
#endif


/**********************************************************************
 *
 * CLASS FdTriad
 *
 **********************************************************************/


Fmd_SOURCE_INIT(FDTRIAD,FdTriad,FdObject);


FdTriad::FdTriad(FmTriad* pt) : FdObject()
{
  Fmd_CONSTRUCTOR_INIT(FdTriad);

  itsFmOwner = pt;

  myCurrentResultsFrame = 0;
  myPosMxReadOp = NULL;

  itsKit = new FdTriadSwKit;
  itsKit->ref();
  itsTrKit = SO_GET_PART(itsKit,"triadSw[0]",FdTransformKit);

  // Set up back pointer
  SO_GET_PART(itsTrKit,"backPt",FdBackPointer)->setPointer(this);
}


FdTriad::~FdTriad()
{
  this->deleteAnimationData();
  this->fdDisconnect();
  itsKit->unref();
  this->setPosMxReadOp(NULL);
}


void FdTriad::setPosMxReadOp(FFaOperation<FaMat34>* readOp)
{
  if (readOp != myPosMxReadOp)
  {
    if (readOp)        readOp->ref();
    if (myPosMxReadOp) myPosMxReadOp->unref();
  }

  myPosMxReadOp = readOp;
}


SoNodeKitListPart* FdTriad::getListSw() const
{
  return SO_GET_PART(FdDB::getMechanismKit(),"triadListSw",SoNodeKitListPart);
}


static FmJointBase* getFirstJoint(FdTriad* triad)
{
  std::vector<FmJointBase*> joints;
  static_cast<FmTriad*>(triad->getFmOwner())->getJointBinding(joints);
  return joints.empty() ? NULL : joints.front();
}


bool FdTriad::updateFdTopology(bool updateChildrenDisplay)
{
#ifdef FD_DEBUG
  std::cout <<"\nFdTriad::updateFdTopology() "
            << itsFmOwner->getIdString(true) << std::endl;
#endif

  SoTransform* transLink;

  FmTriad* triad = static_cast<FmTriad*>(itsFmOwner);
  FmLink* ownerLink = triad->getOwnerLink(0);

  if (ownerLink) // Triad is attached
  {
    if (ownerLink->getFdPointer())
      transLink = SO_GET_PART(ownerLink->getFdPointer()->getKit(),"transform",SoTransform);
    else
    {
      transLink = new SoTransform;
      transLink->setMatrix(SbMatrix::identity());
    }
    itsTrKit->setPart("firstTrans", transLink);

    // Set transform from DB

#ifdef FD_DEBUG
    std::cout <<"\tAttached to "<< ownerLink->getIdString() <<": 2nd transform"
              << ownerLink->getGlobalCS().inverse()*triad->getGlobalCS()
              << std::endl;
#endif
    SoTransform* transLocal = SO_GET_PART(itsTrKit,"secondTrans",SoTransform);
    transLocal->setMatrix(FdConverter::toSbMatrix(ownerLink->getGlobalCS().inverse()*triad->getGlobalCS()));
    itsTrKit->setPart("secondTrans",transLocal);
  }
  else // Triad is not attached to a part
  {
    // Joints have to update topology after the triad
    // to connect to the triad and not to the link.
    FmJointBase* joint = getFirstJoint(this);
    if (joint)
    {
#ifdef FD_DEBUG
      std::cout <<"\tNot attached, but in Joint "<< joint->getIdString();
#endif

      // Triad is in joint
      FmLink* otherLink = NULL;
      if (joint->isAttachedToLink())
	if (!joint->isOfType(FmFreeJoint::getClassTypeID()) && !joint->isOfType(FmCamJoint::getClassTypeID()))
	  otherLink = joint->getOtherLink(triad);

      if (otherLink)
      {
	// The other part of the joint is attached to a link.
	// This part of the joint must also follow the link:
	// Get the link transform and install it:

	if (otherLink->getFdPointer())
	  transLink = SO_GET_PART(otherLink->getFdPointer()->getKit(),"transform",SoTransform);
	else
	{
	  transLink = new SoTransform;
	  transLink->setMatrix(SbMatrix::identity());
	}
	itsTrKit->setPart("firstTrans", transLink);

#ifdef FD_DEBUG
        std::cout <<"\n\tMoving with "<< otherLink->getIdString() <<" to "
                  << otherLink->getGlobalCS().inverse()*triad->getGlobalCS()
                  << std::endl;
#endif

	// We have to adjust the transform of the triad that is not
	// attached so that it is transformed right

	transLink = new SoTransform;
	transLink->setMatrix(FdConverter::toSbMatrix(otherLink->getGlobalCS().inverse()*triad->getGlobalCS()));
	itsTrKit->setPart("secondTrans",transLink);
      }
      else // Joint is free in space
      {
	// Remove the possible transformconnection
	// to a link, and put an identity instead.

	transLink = new SoTransform;
	transLink->setMatrix(SbMatrix::identity());
	itsTrKit->setPart("firstTrans", transLink);

#ifdef FD_DEBUG
        std::cout <<"\n\tFree in space, moving Triad to "
                  << triad->getGlobalCS() << std::endl;
#endif

        // Set transform from DB

        transLink = SO_GET_PART(itsTrKit,"secondTrans",SoTransform);
        transLink->setMatrix(FdConverter::toSbMatrix(triad->getGlobalCS()));
      }
    }
    else
    {
      // Triad is not on a part and not in a joint
      // Remove the possible transform connection
      // to a link, and put a identity instead.

      transLink = new SoTransform;
      transLink->setMatrix(SbMatrix::identity());
      itsTrKit->setPart("firstTrans", transLink);

#ifdef FD_DEBUG
      std::cout <<"\n\tFree in space, moving Triad to "
                << triad->getGlobalCS() << std::endl;
#endif

      // Set transform from DB

      transLink = SO_GET_PART(itsTrKit,"secondTrans",SoTransform);
      transLink->setMatrix(FdConverter::toSbMatrix(triad->getGlobalCS()));
    }
  }

  // Set transform from DB

  /* Recursive update of the display topology of the
  // enteties affected by this entety:
  //              Axial Spring/Damper
  //            /
  // Link->Triad->Joint->HP
  //            \
  //              Load
  */

  if (updateChildrenDisplay)
    itsFmOwner->updateChildrenDisplayTopology();

  return true;
}


bool FdTriad::updateFdApperance()
{
#ifdef FD_DEBUG
  std::cout <<"FdTriad::updateFdAppearance() "
            << itsFmOwner->getIdString(true) << std::endl;
#endif

  // This test makes sure we do not unhighlight something
  // when it is supposed to be highlighted
  if (this->highlightRefCount > 0) return true;

  FmLink* owner = ((FmTriad*)itsFmOwner)->getOwnerLink(0);
  if (owner == FmDB::getEarthLink()) // Grounded triad
    itsTrKit->setPart("appearance.material",FdSymbolDefs::getGndTriadMaterial());
  else if (owner)                    // Attached triad
    itsTrKit->setPart("appearance.material",FdSymbolDefs::getTriadMaterial());
  else                               // Detached triad
    itsTrKit->setPart("appearance.material", FdSymbolDefs::getDefaultMaterial());

#ifdef USE_SMALLCHANGE
  itsTrKit->setPart("appearance.depth",NULL);
#endif
  return true;
}


bool FdTriad::updateFdCS()
{
#ifdef FD_DEBUG
  std::cout <<"FdTriad::updateFdCS() "
            << itsFmOwner->getIdString(true) << std::endl;
#endif
  SoTransform* transLocal = SO_GET_PART(itsTrKit,"secondTrans",SoTransform);
  transLocal->setMatrix(FdConverter::toSbMatrix(((FmTriad*)itsFmOwner)->getLocalCS()));
  return true;
}


bool FdTriad::updateFdDetails()
{
#ifdef FD_DEBUG
  std::cout <<"FdTriad::updateFdDetails() "
            << itsFmOwner->getIdString(true) << std::endl;
#endif

  // Get single triad on/off variable and set this switch accordingly

  if (((FmTriad*)itsFmOwner)->showSymbol())
    SO_GET_PART(itsKit,"triadSw",SoNodeKitListPart)->containerSet("whichChild -3"); // SO_SWITCH_ALL
  else
    SO_GET_PART(itsKit,"triadSw",SoNodeKitListPart)->containerSet("whichChild -1"); // SO_SWITCH_NONE

  // Get how to display Triad and set shape switch accordingly

  if (((FmTriad*)itsFmOwner)->showDirections())
    itsTrKit->setPart("symbol",FdSymbolDefs::getSymbol(FdSymbolDefs::COORD_SYST));
  else
    itsTrKit->setPart("symbol",FdSymbolDefs::getSymbol(FdSymbolDefs::POINT));

  return true;
}


bool FdTriad::updateFmOwner()
{
#ifdef FD_DEBUG
  std::cout <<"FdTriad::updateFmOwner() "
            << itsFmOwner->getIdString(true) << std::endl;
#endif

  SoTransform* globalTransform = new SoTransform;

  globalTransform->ref();
  globalTransform->combineLeft(SO_GET_PART(itsTrKit,"firstTrans",SoTransform));
  globalTransform->combineLeft(SO_GET_PART(itsTrKit,"secondTrans",SoTransform));

  SbMatrix trans;
  trans.setTransform(globalTransform->translation.getValue(),
		     globalTransform->rotation.getValue(),
		     SbVec3f(1,1,1));

  ((FmTriad*)itsFmOwner)->setGlobalCS(FdConverter::toFaMat34(trans));
  ((FmTriad*)itsFmOwner)->updateTopologyInViewer();

  globalTransform->unref();
  return true;
}


void FdTriad::showHighlight()
{
  itsTrKit->setPart("appearance.material",FdSymbolDefs::getHighlightMaterial());
#ifdef USE_SMALLCHANGE
  itsTrKit->setPart("appearance.depth",FdSymbolDefs::getHighlightDepthBMod());
#endif
}


void FdTriad::hideHighlight()
{
  this->updateFdApperance();
#ifdef USE_SMALLCHANGE
  itsTrKit->setPart("appearance.depth",NULL);
#endif
}


void FdTriad::deleteAnimationData()
{
  for (FaMat34* frame : myResultsFrames) delete frame;

  std::vector<FaMat34*> empty;
  myResultsFrames.swap(empty);
}


bool FdTriad::hasResultTransform(size_t frameIdx)
{
  if (myResultsFrames.size() > frameIdx)
    return myResultsFrames[frameIdx] ? true : false;
  else
    return false;
}


void FdTriad::setResultTransform(size_t frameIdx, const FaMat34& pos)
{
  this->findOrCreateXfMx(frameIdx) = pos;
}


FaMat34& FdTriad::findOrCreateXfMx(size_t frameIdx)
{
  if (frameIdx >= myResultsFrames.size())
    myResultsFrames.resize(frameIdx+1,NULL);

  if (!myResultsFrames[frameIdx])
    myResultsFrames[frameIdx] = new FaMat34();

  return *myResultsFrames[frameIdx];
}


void FdTriad::selectAnimationFrame(size_t frameNr)
{
  myCurrentResultsFrame = frameNr;

  SoTransform* transLink;

  FmTriad* triad = static_cast<FmTriad*>(itsFmOwner);
  FmLink* ownerLink = triad->getOwnerLink(0);

  if (ownerLink) // Triad is attached
  {
    transLink = new SoTransform;
    transLink->setMatrix(SbMatrix::identity());
    itsTrKit->setPart("firstTrans", transLink);

    // Set transform from DB

    transLink = SO_GET_PART(itsTrKit,"secondTrans",SoTransform);
    transLink->setMatrix(FdConverter::toSbMatrix(this->findOrCreateXfMx(myCurrentResultsFrame)));

  }
  else // Triad is not attached to a part
  {
    // Joints have to update topology after the triad
    // to connect to the triad and not to the link.
    FmJointBase* joint = getFirstJoint(this);
    if (joint)
    {
      // Triad is in joint
      FmLink* otherLink = NULL;
      if (joint->isAttachedToLink())
	if (!joint->isOfType(FmFreeJoint::getClassTypeID()) && !joint->isOfType(FmCamJoint::getClassTypeID()))
	  otherLink = joint->getOtherLink(triad);

      if (otherLink)
      {
	// The other part of the joint is attached to a link.
	// This part of the joint must also follow the link:
	// Get the link transform and install it:

	if (otherLink->getFdPointer())
	  transLink = SO_GET_PART(otherLink->getFdPointer()->getKit(),"transform",SoTransform);
	else
	{
	  transLink = new SoTransform;
	  transLink->setMatrix(SbMatrix::identity());
	}
	itsTrKit->setPart("firstTrans", transLink);

	// We have to adjust the transform of the triad that is not
	// attached so that it is transformed right

	transLink = new SoTransform;
	transLink->setMatrix(FdConverter::toSbMatrix(otherLink->getGlobalCS().inverse()*this->findOrCreateXfMx(myCurrentResultsFrame)));
	itsTrKit->setPart("secondTrans", transLink);
      }
      else {
	// Joint is free in space
	// Remove the possible transformconnection
	// to a link, and put an identity instead.

	transLink = new SoTransform;
	transLink->setMatrix(SbMatrix::identity());
	itsTrKit->setPart("firstTrans", transLink);

	// Set transform from DB

	transLink = SO_GET_PART(itsTrKit,"secondTrans",SoTransform);
	transLink->setMatrix(FdConverter::toSbMatrix(this->findOrCreateXfMx(myCurrentResultsFrame)));
      }
    }
    else {
      // Triad is not on a part and not in a joint
      // Remove the possible transformconnection
      // to a link, and put a identity instead.

      transLink = new SoTransform;
      transLink->setMatrix(SbMatrix::identity());
      itsTrKit->setPart("firstTrans", transLink);

      // Set transform from DB

      transLink = SO_GET_PART(itsTrKit,"secondTrans",SoTransform);
      transLink->setMatrix(FdConverter::toSbMatrix(this->findOrCreateXfMx(myCurrentResultsFrame)));
    }
  }

  // Set transform from DB

  /* Recursive update of the display topology of the
  // enteties affected by this entety:
  //              Axial Spring/Damper
  //            /
  // Link->Triad->Joint->HP
  //            \
  //              Load
  */

  itsFmOwner->updateChildrenDisplayTopology();
}


int FdTriad::getDegOfFreedom(SbVec3f& centerPoint, SbVec3f& direction)
{
  FmLink* ownerLink = ((FmTriad*)itsFmOwner)->getOwnerLink();
  if (ownerLink && ownerLink->getFdPointer())
    return ownerLink->getFdPointer()->getDegOfFreedom(centerPoint,direction);
  else if (((FmTriad*)itsFmOwner)->isAttached())
    return RIGID; // attached to two (or more) links - not movable

  FmJointBase* joint = getFirstJoint(this);
  if (joint)
    if (joint->isOfType(FmSMJointBase::getClassTypeID()))
      if (!joint->isOfType(FmFreeJoint::getClassTypeID()))
	return joint->getFdPointer()->getDegOfFreedom(centerPoint,direction);

  SbVec3f tempVec1, tempVec2;

  centerPoint.setValue(0,0,0);
  direction.setValue(0,0,0);

  std::vector<FmSticker*> stickers;
  ((FmTriad*)itsFmOwner)->getStickers(stickers);
  int currentDOFs = FREE;

  while (!stickers.empty() && currentDOFs != RIGID)
    {
      FmSticker* sticker = stickers.back();
      stickers.pop_back();

      switch (currentDOFs)
	{
	case FREE:
	  currentDOFs = BALL;
	  centerPoint = FdConverter::toSbVec3f(sticker->getPoint());
	  break;

	case BALL:
	  tempVec1 = FdConverter::toSbVec3f(sticker->getPoint());
	  if (!centerPoint.equals(tempVec1,1.0e-7))
	    {
	      currentDOFs = REV;
	      direction = centerPoint - tempVec1;
	    }
	  break;

	case REV:
	  // Get RevJoint axisDir.
	  tempVec1 = direction;
	  // Get vector between sticker and rev
	  tempVec2 = centerPoint - FdConverter::toSbVec3f(sticker->getPoint());
	  // Test if sticker is on rev axis.
	  if (!isParallell(tempVec1,tempVec2))
	    currentDOFs = RIGID;
	  else
	    currentDOFs = REV;
	  break;
	}
     }

  if (!joint) return currentDOFs;

  if (joint->isAttachedToLink())
    // We know that this triad is not attached to a link,
    // but is member of a joint that is attached to a link
    if (joint->isOfType(FmSMJointBase::getClassTypeID()) && !joint->isOfType(FmFreeJoint::getClassTypeID()))
    {
      return RIGID; // Do not move triads in attached simple joints because of an error in PtPmove
      // The user thus have to change joint vars by editing them in the property editor
    }

  FaMat34 triadCS;
  switch (currentDOFs)
    {
    case FREE:
      triadCS = ((FmTriad*)itsFmOwner)->getGlobalCS();

      if (joint->isOfType(FmRigidJoint::getClassTypeID()))
	currentDOFs = RIGID;

      else if (joint->isOfType(FmRevJoint::getClassTypeID())) {
	centerPoint = FdConverter::toSbVec3f(triadCS.translation());
	direction   = FdConverter::toSbVec3f(triadCS[2]); //z-axis
	currentDOFs = REV;
      }
      else if (joint->isOfType(FmPrismJoint::getClassTypeID()))
	currentDOFs = (joint->getFdPointer())->getDegOfFreedom(centerPoint,direction);

      else if (joint->isOfType(FmCylJoint::getClassTypeID()))
	currentDOFs = (joint->getFdPointer())->getDegOfFreedom(centerPoint,direction);

      else if (joint->isOfType(FmBallJoint::getClassTypeID())) {
	centerPoint = FdConverter::toSbVec3f(triadCS.translation());
	currentDOFs = BALL;
      }
      else if (joint->isOfType(FmFreeJoint::getClassTypeID()))
	currentDOFs = FREE;

      break;

    case BALL:
      triadCS = (joint->getSlaveTriad())->getGlobalCS();

      if (joint->isOfType(FmRigidJoint::getClassTypeID()))
	currentDOFs = RIGID;

      else if (joint->isOfType(FmRevJoint::getClassTypeID())) {
	// Get RevJoint axisDir.
	tempVec1 = FdConverter::toSbVec3f(triadCS[2]);
	// Get vector between "Ball" and revJoint
	tempVec2 = centerPoint - FdConverter::toSbVec3f(triadCS.translation());
	// Test if ball  is on revjoint axis.
	if (!isParallell(tempVec1,tempVec2))
	  currentDOFs = RIGID;
	else {
	  direction   = tempVec1; // Leave centerPoint as it was.
	  currentDOFs = REV;
	}
      }
      else if (joint->isOfType(FmPrismJoint::getClassTypeID()) || joint->isOfType(FmCylJoint::getClassTypeID()))
	switch (joint->getFdPointer()->getDegOfFreedom(centerPoint,direction))
	  {
	  case FREE:
	    break;
	  case BALL_STRECH:
	    currentDOFs = BALL;
	    break;
	  case REV_STRECH:
	  case REV_PRISM:
	  case REV_CYL:
	  case REV:
	    currentDOFs = REV;
	    break;
	  case PRISM_STRECH:
	  case CYL:
	  case PRISM:
	  case RIGID:
	    currentDOFs = RIGID;
	    break;
	  }

      else if (joint->isOfType(FmBallJoint::getClassTypeID())) {
	tempVec1 = FdConverter::toSbVec3f(triadCS.translation());
	if (centerPoint.equals(tempVec1,1.0e-7))
	  currentDOFs = BALL; // Joint is in same place

	else {
	  direction   = centerPoint - tempVec1;
	  currentDOFs = REV;
	}
      }
      break;

    case REV:
      triadCS = (joint->getSlaveTriad())->getGlobalCS();

      if (joint->isOfType(FmRigidJoint::getClassTypeID()))
	currentDOFs = RIGID;

      else if (joint->isOfType(FmRevJoint::getClassTypeID())) {
	// Get rev axisDir.
	tempVec1 = direction;
	// Get vector between Rev and revJoint
	tempVec2 = centerPoint - FdConverter::toSbVec3f(triadCS.translation());
	// Test if rev  is on revjoint axis.
	if (!isParallell(tempVec1,tempVec2))
	  currentDOFs = RIGID;
	else {
	  tempVec2 = FdConverter::toSbVec3f(triadCS[2]);// z-axis
	  // Test if rev axis is paralell to revjoint axis.
	  if (!isParallell(tempVec1,tempVec2))
	    currentDOFs = RIGID;
	  else
	    currentDOFs = REV;
	}
      }
      else if (joint->isOfType(FmPrismJoint::getClassTypeID()) || joint->isOfType(FmCylJoint::getClassTypeID()))
	switch (joint->getFdPointer()->getDegOfFreedom(centerPoint,direction))
	  {
	  case FREE:
	    break;
	  case BALL_STRECH:
	    currentDOFs = BALL;
	    break;
	  case REV_STRECH:
	  case REV_PRISM:
	  case REV_CYL:
	  case REV:
	    currentDOFs = REV;
	    break;
	  case PRISM_STRECH:
	  case CYL:
	  case PRISM:
	  case RIGID:
	    currentDOFs = RIGID;
	    break;
	  }

      else if (joint->isOfType(FmBallJoint::getClassTypeID())) {
	// Get RevJoint axisDir.
	tempVec1 = direction;
	// Get vector between "Ball" and revJoint
	tempVec2 = centerPoint - FdConverter::toSbVec3f(triadCS.translation());
	// Test if ball joint is on rev axis.
	if (!isParallell(tempVec1,tempVec2))
	  currentDOFs = RIGID;
	else
	  currentDOFs = REV;
      }
      break;
    }

  return currentDOFs;
}


SbVec3f FdTriad::findSnapPoint(const SbVec3f& pointOnObject,
			       const SbMatrix& objToWorld,
			       SoDetail*, SoPickedPoint*)
{
  const float snapDistance = 0.4f;

  SbVec3f nearest(0,0,0);
  if (pointOnObject.length() > snapDistance)
  {
    float Xpoint = pointOnObject[0];
    float Ypoint = pointOnObject[1];
    float Zpoint = pointOnObject[2];
    if (fabsf(Xpoint) > fabsf(Ypoint))
    {
      if (fabsf(Xpoint) > fabsf(Zpoint))
        nearest[0] = Xpoint > 0.0f ? 1.0f : -1.0f;
      else
        nearest[2] = Zpoint > 0.0f ? 1.0f : -1.0f;
    }
    else
    {
      if (fabsf(Ypoint) > fabsf(Zpoint))
        nearest[1] = Ypoint > 0.0f ? 1.0f : -1.0f;
      else
        nearest[2] = Zpoint > 0.0f ? 1.0f : -1.0f;
    }
  }

  SbVec3f nearestWorld;
  objToWorld.multVecMatrix(nearest,nearestWorld);
  return nearestWorld;
}


void FdTriad::smartMove(const FaVec3& p1, const FaVec3& p2, const FaDOF& dof)
{
  // First check if the triad is attached to a (single) link.
  // If it is, move the link (with the triad on it) instead of the triad itself.
  FmLink* ownerLink = ((FmTriad*)itsFmOwner)->getOwnerLink();
  if (ownerLink && ownerLink->getFdPointer())
  {
    ownerLink->getFdPointer()->smartMove(p1,p2,dof);
    return;
  }
  else if (((FmTriad*)itsFmOwner)->isAttached())
    return; // attached to two (or more) links - not movable

  FdTriad*     otherTriad1 = NULL;
  FdTriad*     otherTriad2 = NULL;
  SoTransform* otherTransform1 = NULL;
  SoTransform* otherTransform2 = NULL;
  SoTransform* otherLinkTransform = NULL;

  // Check if the triad is member of a joint.
  bool isInLinJoint = false;
  FmJointBase* joint = getFirstJoint(this);
  if (joint)
  {
    // Check if triad is member of a simple joint (Revolute,Ball,Rigid).
    if (joint->isOfType(FmSMJointBase::getClassTypeID()) && !joint->isOfType(FmFreeJoint::getClassTypeID()))
    {
      // It is, so move joint instead.
      joint->getFdPointer()->smartMove(p1,p2,dof);
      return;
    }

    // If triad is member of a linear joint we have to move the other
    // triads in the joint together with this triad.
    isInLinJoint = joint->isOfType(FmMMJointBase::getClassTypeID()) && !joint->isOfType(FmCamJoint::getClassTypeID());
    if (isInLinJoint)
    {
      if (joint->isAttachedToLink())
      {
	// We know that this triad is not attached to a link,
	// but is member of a (linear) joint that is attached to a link.
	// Thus we have to move this triad relatively to the link.
	FmLink* otherLink = joint->getOtherLink((FmTriad*)itsFmOwner);
	if (otherLink && otherLink->getFdPointer())
	  otherLinkTransform = SO_GET_PART(otherLink->getFdPointer()->getKit(),"transform",SoTransform);
      }

      // Get the other FdTriads and their transforms.

      otherTriad1 = (FdTriad*)(((FmMMJointBase*)joint)->getFirstMaster())->getFdPointer();
      otherTriad2 = (FdTriad*)(((FmMMJointBase*)joint)->getLastMaster())->getFdPointer();

      if (this == otherTriad1) // This triad is the first master in the linear joint
      {
	otherTriad1 = otherTriad2;
	otherTriad2 = (FdTriad*)(((FmMMJointBase*)joint)->getSlaveTriad())->getFdPointer();
      }
      else if (this == otherTriad2) // This triad is the last master in the linear joint
	otherTriad2 = (FdTriad*)(((FmMMJointBase*)joint)->getSlaveTriad())->getFdPointer();

      otherTransform1 = SO_GET_PART(otherTriad1->getKit(),"secondTrans",SoTransform);
      otherTransform2 = SO_GET_PART(otherTriad2->getKit(),"secondTrans",SoTransform);
    }
  }

  SbVec3f tempVec, origin(0,0,0);
  SbVec3f firstPoint  = FdConverter::toSbVec3f(p1);
  SbVec3f centerPoint = FdConverter::toSbVec3f(dof.getCenter());
  SbRotation tempRotation;
  double angle = 0.0;
  FdPtPMoveAnimator* thisAnimator = NULL;
  FdPtPMoveAnimator* otherAnimator1 = NULL;
  FdPtPMoveAnimator* otherAnimator2 = NULL;
  SoTransform* thisTransform = SO_GET_PART(itsTrKit,"secondTrans",SoTransform);

  switch (dof.getType())
    {
    case FREE:
      tempVec = FdConverter::toSbVec3f(p2 - p1);
      tempRotation = SbRotation::identity();
      thisAnimator = new FdPtPMoveAnimator(thisTransform, this, tempVec, tempRotation, centerPoint, firstPoint);
      if (isInLinJoint)
      {
	otherAnimator1 = new FdPtPMoveAnimator(otherTransform1, otherTriad1, tempVec, tempRotation, centerPoint, firstPoint, false);
	otherAnimator2 = new FdPtPMoveAnimator(otherTransform2, otherTriad2, tempVec, tempRotation, centerPoint, firstPoint, false);
	otherAnimator1->start();
	otherAnimator2->start();
      }
      thisAnimator->start();
      break;

    case BALL_STRECH:
      if (!isInLinJoint) return;

      // Translation:
      tempVec = FdConverter::toSbVec3f(p2 - p1);
      // Rotation:
      tempRotation.setValue(FdConverter::toSbVec3f(p1 - dof.getCenter()),
                            FdConverter::toSbVec3f(p2 - dof.getCenter()));

      thisAnimator   = new FdPtPMoveAnimator(thisTransform,   this,       tempVec, tempRotation, thisTransform->translation.getValue(), firstPoint);
      otherAnimator1 = new FdPtPMoveAnimator(otherTransform1, otherTriad1, origin, tempRotation, centerPoint, firstPoint, false);
      otherAnimator2 = new FdPtPMoveAnimator(otherTransform2, otherTriad2, origin, tempRotation, centerPoint, firstPoint, false);
      thisAnimator->start();
      otherAnimator1->start();
      otherAnimator2->start();
      break;

    case BALL:
      tempRotation.setValue(FdConverter::toSbVec3f(p1 - dof.getCenter()),
                            FdConverter::toSbVec3f(p2 - dof.getCenter()));

      thisAnimator = new FdPtPMoveAnimator(thisTransform,this,origin,tempRotation,centerPoint,firstPoint,true,otherLinkTransform);

      // We never get ball dof's on a linear joint that must be moved relative a link
      if (isInLinJoint)
      {
	otherAnimator1 = new FdPtPMoveAnimator(otherTransform1,otherTriad1,origin,tempRotation,centerPoint,firstPoint,false);
	otherAnimator2 = new FdPtPMoveAnimator(otherTransform2,otherTriad2,origin,tempRotation,centerPoint,firstPoint,false);
	otherAnimator1->start();
	otherAnimator2->start();
      }
      thisAnimator->start();
      break;

    case REV_CYL:
    case CYL:
      // Get translation along glider
      tempVec = FdConverter::toSbVec3f(dof.tangentVector(p1,p2));
      // Rotation around glider axis
      angle = dof.rotationAngle(p1,p2);
      if (fabs(angle) > 1.0e-6)
        tempRotation.setValue(FdConverter::toSbVec3f(dof.getDirection()),(float)angle);
      else // Rotation is zero or undefined. Just translate along glider:
        tempRotation = SbRotation::identity();

      thisAnimator = new FdPtPMoveAnimator(thisTransform,this,tempVec,tempRotation,centerPoint,firstPoint,true,otherLinkTransform);
      thisAnimator->start();
      break;

    case REV_STRECH:
    case REV_PRISM: // Do the same for both possibilities. This might pull the slave out of master range.
      if (isInLinJoint)
      {
        // Get vectors normal to rotation axis
        FaVec3 vec1 = dof.normalVector(p1);
        FaVec3 vec2 = dof.normalVector(p2);

        // Get translation along glider
        tempVec = FdConverter::toSbVec3f(dof.tangentVector(p1,p2));

        if (vec1.isZero() || vec2.isZero())
        {
          // Rotation is undefined. Just translate along glider:
          thisAnimator = new FdPtPMoveAnimator(thisTransform,this,tempVec,SbRotation::identity(),centerPoint,firstPoint);
          thisAnimator->start();
        }
        else
        {
          // Get rotation
          tempRotation.setValue(FdConverter::toSbVec3f(vec1),FdConverter::toSbVec3f(vec2));

          // Start moving
          thisAnimator   = new FdPtPMoveAnimator(thisTransform,this,tempVec,tempRotation,centerPoint,firstPoint);
          otherAnimator1 = new FdPtPMoveAnimator(otherTransform1,otherTriad1,origin,tempRotation,centerPoint,firstPoint,false);
          otherAnimator2 = new FdPtPMoveAnimator(otherTransform2,otherTriad2,origin,tempRotation,centerPoint,firstPoint,false);
          thisAnimator->start();
          otherAnimator1->start();
          otherAnimator2->start();
        }
      }
      break;

    case REV:
      angle = dof.rotationAngle(p1,p2);
      if (fabs(angle) > 1.0e-6)
      {
	tempRotation.setValue(FdConverter::toSbVec3f(dof.getDirection()),(float)angle);

	thisAnimator = new FdPtPMoveAnimator(thisTransform,this,origin,tempRotation,centerPoint,firstPoint,true,otherLinkTransform);
	thisAnimator->start();

	if (isInLinJoint && !((FmTriad*)(otherTriad1->getFmOwner()))->isAttached())
	{
	  otherAnimator1 = new FdPtPMoveAnimator(otherTransform1,otherTriad1,origin,tempRotation,centerPoint,firstPoint,false,otherLinkTransform);
	  otherAnimator1->start();
	}

	if (isInLinJoint && !((FmTriad*)(otherTriad2->getFmOwner()))->isAttached())
	{
	  otherAnimator2 = new FdPtPMoveAnimator(otherTransform2,otherTriad2,origin,tempRotation,centerPoint,firstPoint,false,otherLinkTransform);
	  otherAnimator2->start();
	}
      }
      break;

    case PRISM_STRECH:
    case PRISM:
      tempVec = FdConverter::toSbVec3f(dof.tangentVector(p1,p2));
      thisAnimator = new FdPtPMoveAnimator(thisTransform,this,tempVec,SbRotation::identity(),centerPoint,firstPoint,true,otherLinkTransform);
      thisAnimator->start();
      break;

    default:
      break;
    }
}
