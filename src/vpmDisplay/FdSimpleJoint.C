// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdSimpleJoint.H"
#include "vpmDisplay/FdSimpleJointKit.H"
#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdAppearanceKit.H"
#include "vpmDisplay/FdSprDaTransformKit.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdTriad.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDisplay/FdPtPMoveAnimator.H"

#include "vpmDB/FmFreeJoint.H"
#include "vpmDB/FmBallJoint.H"
#include "vpmDB/FmRevJoint.H"
#include "vpmDB/FmRigidJoint.H"
#include "vpmDB/FmSticker.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmLink.H"

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#ifdef USE_SMALLCHANGE
#include <SmallChange/nodes/SmDepthBuffer.h>
#endif
#include <cmath>


/**********************************************************************
 *
 * CLASS FdSimpleJoint
 *
 **********************************************************************/


Fmd_SOURCE_INIT(FDSIMPLEJOINT,FdSimpleJoint,FdObject);


FdSimpleJoint::FdSimpleJoint(FmSMJointBase* pt) : FdObject()
{
  Fmd_CONSTRUCTOR_INIT(FdSimpleJoint);

  itsFmOwner = pt;

  itsKit = new FdSimpleJointKit;
  itsKit->ref();

  // Set up back pointer
  FdBackPointer* bp_pointer = SO_GET_PART(itsKit,"backPt",FdBackPointer);
  bp_pointer->setPointer(this);

  // Set up back pointer connections in the kit
  // (The pointers in the master and slave are set equal to
  // the triads in updateFdTopology)
  itsKit->setPart("joint.backPt",bp_pointer);
  itsKit->setPart("master.backPt",bp_pointer);
  itsKit->setPart("slave.backPt",bp_pointer);
  itsKit->setPart("lineM.backPt",bp_pointer);
  itsKit->setPart("lineS.backPt",bp_pointer);
}


FdSimpleJoint::~FdSimpleJoint()
{
  this->fdDisconnect();
  itsKit->unref();
}


SoNodeKitListPart* FdSimpleJoint::getListSw() const
{
  return SO_GET_PART(FdDB::getMechanismKit(),"simpleJointListSw",SoNodeKitListPart);
}


bool FdSimpleJoint::updateFdTopology(bool updateChildrenDisplay)
{
  if (!itsFmOwner) return false;

#ifdef FD_DEBUG
  std::cout <<"FdSimpleJoint::updateFdTopology() "
            << itsFmOwner->getIdString(true) << std::endl;
#endif

  SoTransform* transLink;
  SoTransform* transLocal;
  FdAppearanceKit* appearanceKit = NULL;
  FdBackPointer* backPt = NULL;

  // Set Slave transform and backPt connection

  FmTriad* slave = ((FmJointBase*)itsFmOwner)->getSlaveTriad();
  if (slave)
  {
    transLink = SO_GET_PART(slave->getFdPointer()->getKit(),"firstTrans",SoTransform);
    transLocal = SO_GET_PART(slave->getFdPointer()->getKit(),"secondTrans",SoTransform);
    appearanceKit = SO_GET_PART(slave->getFdPointer()->getKit(),"appearance",FdAppearanceKit);
    backPt = SO_GET_PART(slave->getFdPointer()->getKit(),"backPt",FdBackPointer);
  }
  else
  {
    // Joints should always have a slave triad
    std::cerr <<"FdSimpleJoint::updateFdTopology: No slave triad in "
              << itsFmOwner->getIdString(true) << std::endl;
    transLink = new SoTransform;
    transLocal= new SoTransform;
  }

  itsKit->setPart("slave.firstTrans",transLink);
  itsKit->setPart("slave.secondTrans",transLocal);
  itsKit->setPart("slave.appearance",appearanceKit);
  itsKit->setPart("slave.backPt",backPt);

  // Set Master transform and backPt connection

  FmTriad* master = ((FmSMJointBase*)itsFmOwner)->getItsMasterTriad();
  if (master)
  {
    transLink = SO_GET_PART(master->getFdPointer()->getKit(),"firstTrans",SoTransform);
    transLocal = SO_GET_PART(master->getFdPointer()->getKit(),"secondTrans",SoTransform);
    appearanceKit = SO_GET_PART(master->getFdPointer()->getKit(),"appearance",FdAppearanceKit);
    backPt = SO_GET_PART(master->getFdPointer()->getKit(),"backPt",FdBackPointer);
  }
  else
  {
    // There is no master triad (this might be a modeling error)
    std::cout <<"FdSimpleJoint::updateFdTopology: No master triad in "
              << itsFmOwner->getIdString(true) << std::endl;
    // Transformation is set equal to slave transformation
    backPt = new FdBackPointer;
  }

  itsKit->setPart("master.firstTrans",transLink);
  itsKit->setPart("master.secondTrans",transLocal);
  itsKit->setPart("master.appearance",appearanceKit);
  itsKit->setPart("master.backPt",backPt);

  // Set the Joint transforms

  itsKit->setPart("joint.firstTrans",transLink);
  SoTransform* jointTransLocal = SO_GET_PART(itsKit,"joint.secondTrans",SoTransform);
  jointTransLocal->setMatrix(SbMatrix::identity());
  FaMat34 jcs = ((FmJointBase*)itsFmOwner)->getLocalCS();
  if (master)
  {
    // Needs to handle that the slavelink xf is actually used for the master as well,
    // to make it follow during the animation (smart move)
    SbMatrix masterMx;
    masterMx.setTransform(transLocal->translation.getValue(),
		          transLocal->rotation.getValue(),
			  transLocal->scaleFactor.getValue());
    jcs = FdConverter::toFaMat34(masterMx) * jcs;
#ifdef FD_DEBUG
    std::cout <<" Joint location from master:"<< jcs << std::endl;
#endif
  }
  jointTransLocal->setMatrix(FdConverter::toSbMatrix(jcs));

  FdSprDaTransformKit* lineSymbol = SO_GET_PART(itsKit,"lineS",FdSprDaTransformKit);
  SoTransform* firstTrans  = SO_GET_PART(itsKit,"slave.firstTrans",SoTransform);
  SoTransform* secondTrans = SO_GET_PART(itsKit,"slave.secondTrans",SoTransform);

  lineSymbol->connectFirstSpace(transLink,jointTransLocal);
  lineSymbol->connectSecondSpace(firstTrans,secondTrans);

  lineSymbol  = SO_GET_PART(itsKit,"lineM",FdSprDaTransformKit);
  firstTrans  = SO_GET_PART(itsKit,"master.firstTrans",SoTransform);
  secondTrans = SO_GET_PART(itsKit,"master.secondTrans",SoTransform);

  lineSymbol->connectFirstSpace(transLink,jointTransLocal);
  lineSymbol->connectSecondSpace(firstTrans,secondTrans);

  /* Recursive update of the display topology of the
  // entities affected by this entity:
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


bool FdSimpleJoint::updateFdApperance()
{
  if (!itsFmOwner) return false;

#ifdef FD_DEBUG
  std::cout <<"FdSimpleJoint::updateFdAppearance() "
            << itsFmOwner->getIdString(true) << std::endl;
#endif

  // This test makes sure we do not unhighlight something
  // when it is supposed to be highlighted
  if (this->highlightRefCount > 0) return true;

  bool attachedMaster = ((FmJointBase*)itsFmOwner)->isMasterAttachedToLink(true);
  bool attachedSlave  = ((FmJointBase*)itsFmOwner)->isSlaveAttachedToLink(true);
  if (!attachedMaster)
    itsKit->setPart("joint.appearance.material",FdSymbolDefs::getDefaultMaterial());
  else if (attachedSlave || itsFmOwner->isOfType(FmFreeJoint::getClassTypeID()))
    itsKit->setPart("joint.appearance.material",FdSymbolDefs::getJointMaterial());
  else
    itsKit->setPart("joint.appearance.material",FdSymbolDefs::getDefaultMaterial());

  if (attachedSlave && attachedMaster)
  {
    itsKit->setPart("lineM.appearance.material",FdSymbolDefs::getJointMaterial());
    itsKit->setPart("lineS.appearance.material",FdSymbolDefs::getJointMaterial());
  }
  else
  {
    itsKit->setPart("lineM.appearance.material",FdSymbolDefs::getDefaultMaterial());
    itsKit->setPart("lineS.appearance.material",FdSymbolDefs::getDefaultMaterial());
  }

  return true;
}


void FdSimpleJoint::hide()
{
  itsKit->setPart("joint.symbol", NULL);
  itsKit->setPart("master.symbol", NULL);
  itsKit->setPart("slave.symbol", NULL);
  itsKit->setPart("lineM.symbol", NULL);
  itsKit->setPart("lineS.symbol", NULL);
}


bool FdSimpleJoint::updateFdDetails()
{
  if (!itsFmOwner) return false;

#ifdef FD_DEBUG
  std::cout <<"FdSimpleJoint::updateFdDetails() "
            << itsFmOwner->getIdString(true) << std::endl;
#endif

  if (itsFmOwner->isOfType(FmRevJoint::getClassTypeID()))
    {
      itsKit->setPart("joint.symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::REVJOINT));
      itsKit->setPart("master.symbol",FdSymbolDefs::getSymbol(FdSymbolDefs::REVJOINT_MASTER));
      itsKit->setPart("slave.symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::REVJOINT_SLAVE));
    }
  else if (itsFmOwner->isOfType(FmBallJoint::getClassTypeID()))
    {
      itsKit->setPart("joint.symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::BALLJOINT));
      itsKit->setPart("master.symbol",FdSymbolDefs::getSymbol(FdSymbolDefs::BALLJOINT_MASTER));
      itsKit->setPart("slave.symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::BALLJOINT_SLAVE));
    }
  else if (itsFmOwner->isOfType(FmRigidJoint::getClassTypeID()))
    {
      itsKit->setPart("joint.symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::RIGIDJOINT));
      itsKit->setPart("master.symbol",FdSymbolDefs::getSymbol(FdSymbolDefs::RIGIDJOINT_MASTER));
      itsKit->setPart("slave.symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::RIGIDJOINT_SLAVE));
    }
  else if (itsFmOwner->isOfType(FmFreeJoint::getClassTypeID()))
    {
      itsKit->setPart("joint.symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::FREEJOINT));
      itsKit->setPart("master.symbol",FdSymbolDefs::getSymbol(FdSymbolDefs::COORD_SYST));
      itsKit->setPart("slave.symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::FREEJOINT_SLAVE));
    }
  else
    return false; // unknown simple Joint

  itsKit->setPart("lineM.symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::FREEJOINT_LINE));
  itsKit->setPart("lineS.symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::FREEJOINT_LINE));

  return true;
}


bool FdSimpleJoint::updateFmOwner()
{
#ifdef FD_DEBUG
  std::cout <<"FdSimpleJoint::updateFmOwner() "
	    << itsFmOwner->getIdString(true) << std::endl;
#endif
  SoTransform* firstTrans  = SO_GET_PART(itsKit,"joint.firstTrans", SoTransform);
  SoTransform* secondTrans = SO_GET_PART(itsKit,"joint.secondTrans", SoTransform);

  SoTransform* globalTransform = new SoTransform;
  globalTransform->ref();
  globalTransform->combineLeft(firstTrans);
  globalTransform->combineLeft(secondTrans);

  SbMatrix trans;
  trans.setTransform(globalTransform->translation.getValue(),
                     globalTransform->rotation.getValue(),
                     SbVec3f(1,1,1));

#ifdef FD_DEBUG
  std::cout <<" Joint location:" << FdConverter::toFaMat34(trans) << std::endl;
#endif

  ((FmIsPositionedBase*)itsFmOwner)->setGlobalCS(FdConverter::toFaMat34(trans));
  ((FmIsPositionedBase*)itsFmOwner)->updateTopologyInViewer();

  globalTransform->unref();
  return true;
}


void FdSimpleJoint::smartMove(const FaVec3& p1, const FaVec3& p2, const FaDOF& dof)
{
  if (!itsFmOwner) return;

  // Redirect the move to the link if this joint is attached to one (but not two)

  if (((FmJointBase*)itsFmOwner)->isAttachedToLink())
  {
    FmLink* slaveLink = ((FmJointBase*)itsFmOwner)->getSlaveLink();
    FmLink* masterLink = ((FmJointBase*)itsFmOwner)->getMasterLink();
    if (slaveLink && !masterLink && slaveLink->getFdPointer())
    {
      slaveLink->getFdPointer()->smartMove(p1,p2,dof);
      return;
    }
    else if (masterLink && !slaveLink && masterLink->getFdPointer())
    {
      masterLink->getFdPointer()->smartMove(p1,p2,dof);
      return;
    }
  }

  FaVec3 tempVec1, tempVec2;
  SbVec3f translation(0,0,0);
  SbRotation tempRotation;
  double angle = 0.0;

  switch (dof.getType())
    {
    case FREE:
      translation = FdConverter::toSbVec3f(p2 - p1);
      tempRotation = SbRotation::identity();
      break;

    case BALL:
      tempVec1 = p1 - dof.getCenter();
      if (tempVec1.isZero()) return;

      tempVec2 = p2 - dof.getCenter();
      if (tempVec2.isZero()) return;

      // We will compute sensible rotation
      tempRotation.setValue(FdConverter::toSbVec3f(tempVec1),
                            FdConverter::toSbVec3f(tempVec2));
      break;

    case REV:
      // We will compute sensible rotation
      angle = dof.rotationAngle(p1,p2);
      if (fabs(angle) <= 1.0e-6) return;
      tempRotation.setValue(FdConverter::toSbVec3f(dof.getDirection()),(float)angle);
      break;
    }

  FdTriad* masterFdTriad = (FdTriad*)((FmSMJointBase*)itsFmOwner)->getItsMasterTriad()->getFdPointer();
  FdTriad* slaveFdTriad  = (FdTriad*)((FmSMJointBase*)itsFmOwner)->getSlaveTriad()->getFdPointer();

  SoTransform* masterTransform = SO_GET_PART(masterFdTriad->getKit(),"secondTrans", SoTransform);
  SoTransform* slaveTransform  = SO_GET_PART(slaveFdTriad->getKit(), "secondTrans", SoTransform);
  SoTransform* jointTransform  = SO_GET_PART(this->getKit(),         "secondTrans", SoTransform);

  SbVec3f firstPoint  = FdConverter::toSbVec3f(p1);
  SbVec3f centerPoint = FdConverter::toSbVec3f(dof.getCenter());
  FdPtPMoveAnimator* masterAnimator = new FdPtPMoveAnimator(masterTransform, masterFdTriad, translation, tempRotation, centerPoint, firstPoint);
  FdPtPMoveAnimator* slaveAnimator  = new FdPtPMoveAnimator(slaveTransform,   slaveFdTriad, translation, tempRotation, centerPoint, firstPoint, false);
  FdPtPMoveAnimator* jointAnimator  = new FdPtPMoveAnimator(jointTransform,           this, translation, tempRotation, centerPoint, firstPoint, false);

  masterAnimator->start();
  slaveAnimator->start();
  jointAnimator->start();
}


int FdSimpleJoint::getDegOfFreedom(SbVec3f& centerPoint, SbVec3f& direction)
{
  if (!itsFmOwner) return FREE;

  centerPoint.setValue(0,0,0);
  direction.setValue(0,0,1);

  if (((FmJointBase*)itsFmOwner)->isAttachedToLink())
  {
    if (itsFmOwner->isOfType(FmFreeJoint::getClassTypeID()))
      return RIGID;

    FmLink* slaveLink = ((FmJointBase*)itsFmOwner)->getSlaveLink();
    FmLink* masterLink = ((FmJointBase*)itsFmOwner)->getMasterLink();
    if (slaveLink && !masterLink && slaveLink->getFdPointer())
      return slaveLink->getFdPointer()->getDegOfFreedom(centerPoint,direction);
    else if (masterLink && !slaveLink && masterLink->getFdPointer())
      return masterLink->getFdPointer()->getDegOfFreedom(centerPoint,direction);
    else
      return RIGID;
  }

  int currentDOFs = FREE;

  SbVec3f tempVec1, tempVec2;

  std::vector<FmSticker*> stickers;
  ((FmSMJointBase*)itsFmOwner)->getItsMasterTriad()->getStickers(stickers);
  ((FmSMJointBase*)itsFmOwner)->getSlaveTriad()->getStickers(stickers);

  for (FmSticker* sticker : stickers)
    switch (currentDOFs)
      {
      case FREE:
	currentDOFs = BALL;
	centerPoint = FdConverter::toSbVec3f(sticker->getPoint());
	break;

      case BALL:
	tempVec1 = FdConverter::toSbVec3f(sticker->getPoint());
	if (centerPoint != tempVec1)
	  {
	    direction = centerPoint - tempVec1;
	    currentDOFs = REV;
	  }
	break;

      case REV:
	// Test if sticker is on revolute axis.
	// Get vector between sticker and RevJoint
	tempVec2 = centerPoint - FdConverter::toSbVec3f(sticker->getPoint());
	if (tempVec2.length())
	  {
	    // Get RevJoint axis direction
	    tempVec1 = direction;
	    tempVec1.normalize();
	    tempVec2.normalize();
	    if (fabsf(tempVec1.dot(tempVec2)) != 1.0f)
	      currentDOFs = RIGID;
	  }
	break;

      case RIGID:
	// No need to check any further
	return currentDOFs;
      }

  return currentDOFs;
}


SbVec3f FdSimpleJoint::findSnapPoint(const SbVec3f& pointOnObject, const SbMatrix& objToWorld,
				     SoDetail*, SoPickedPoint*)
{
  SbVec3f nearest(0,0,0); // Default snap to center

  if (itsFmOwner)
    if ( (pointOnObject.length() > 1.1f && itsFmOwner->isOfType(FmRigidJoint::getClassTypeID())) ||
	 (pointOnObject.length() > 0.8f && itsFmOwner->isOfType(FmRevJoint::getClassTypeID())) )
      {
	float pointDotX = pointOnObject[0];
	float pointDotY = pointOnObject[1];
	float pointDotZ = pointOnObject[2];

	if (fabsf(pointDotX) > fabsf(pointDotY))
	{
	  if (fabsf(pointDotX) > fabsf(pointDotZ))
	    nearest[0] = copysignf(1.0f,pointDotX);
	  else
	    nearest[2] = copysignf(1.0f,pointDotZ);
	}
	else if (fabsf(pointDotY) > fabsf(pointDotZ))
	  nearest[1] = copysignf(1.0f,pointDotY);
	else
	  nearest[2] = copysignf(1.0f,pointDotZ);
      }

  SbVec3f nearestWorld;
  objToWorld.multVecMatrix(nearest,nearestWorld);
  return nearestWorld;
}


void FdSimpleJoint::showHighlight()
{
  SoMaterial* mat = FdSymbolDefs::getHighlightMaterial();
  itsKit->setPart("joint.appearance.material", mat);
  itsKit->setPart("master.appearance.material",mat);
  itsKit->setPart("slave.appearance.material", mat);
  itsKit->setPart("lineM.appearance.material", mat);
  itsKit->setPart("lineS.appearance.material", mat);

#ifdef USE_SMALLCHANGE
  SmDepthBuffer* dbn = FdSymbolDefs::getHighlightDepthBMod();
  itsKit->setPart("joint.appearance.depth", dbn);
  itsKit->setPart("master.appearance.depth",dbn);
  itsKit->setPart("slave.appearance.depth", dbn);
  itsKit->setPart("lineM.appearance.depth", dbn);
  itsKit->setPart("lineS.appearance.depth", dbn);
#endif
}


void FdSimpleJoint::hideHighlight()
{
  if (!this->updateFdApperance()) return;

  ((FmSMJointBase*)itsFmOwner)->getSlaveTriad()->getFdPointer()->updateFdApperance();
  ((FmSMJointBase*)itsFmOwner)->getItsMasterTriad()->getFdPointer()->updateFdApperance();;

#ifdef USE_SMALLCHANGE
  itsKit->setPart("joint.appearance.depth", NULL);
  itsKit->setPart("master.appearance.depth",NULL);
  itsKit->setPart("slave.appearance.depth", NULL);
  itsKit->setPart("lineM.appearance.depth", NULL);
  itsKit->setPart("lineS.appearance.depth", NULL);
#endif
}
