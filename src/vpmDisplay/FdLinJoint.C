// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdLinJoint.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdLinJointKit.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdTriad.H"
#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdAppearanceKit.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdSprDaTransformKit.H"
#include "vpmDisplay/FdPtPMoveAnimator.H"
#include "vpmDisplay/FdConverter.H"

#include <Inventor/nodes/SoMaterial.h>
#ifdef USE_SMALLCHANGE
#include <SmallChange/nodes/SmDepthBuffer.h>
#endif

#include "vpmDB/FmCamJoint.H"
#include "vpmDB/FmCylJoint.H"
#include "vpmDB/FmPrismJoint.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmLink.H"


/**********************************************************************
 *
 * CLASS FdLinJoint
 *
 **********************************************************************/


Fmd_SOURCE_INIT(FDLINJOINT,FdLinJoint,FdObject);


FdLinJoint::FdLinJoint(FmMMJointBase* pt) : FdObject()
{
  Fmd_CONSTRUCTOR_INIT(FdLinJoint);

  itsFmOwner = pt;

  itsKit = new FdLinJointKit();
  itsKit->ref();

  // Set up back pointer
  FdBackPointer* bp_pointer = SO_GET_PART(itsKit,"backPt",FdBackPointer);
  bp_pointer->setPointer(this);

  // Set up back pointer connections in the kit.
  // The master connection has to be set up for each master.
  // This is done in FdLinJoint::updateFdTopology()
  itsKit->setPart("slave.backPt",bp_pointer);
  itsKit->setPart("lineSymbol.backPt",bp_pointer);
}


FdLinJoint::~FdLinJoint()
{
  this->fdDisconnect();
  itsKit->unref();
}


SoNodeKitListPart* FdLinJoint::getListSw() const
{
  return SO_GET_PART(FdDB::getMechanismKit(),
                     "linJointListSw",SoNodeKitListPart);
}


bool FdLinJoint::updateFdTopology(bool updateChildrenDisplay)
{
  if (!itsFmOwner) return false;

  SoTransform* transLink;
  SoTransform* transLocal;
  FdAppearanceKit* appearKit = NULL;
  FdBackPointer* backPt = NULL;

  // Set up slave part of the joint:

  FmTriad* triad = ((FmJointBase*)itsFmOwner)->getSlaveTriad();
  if (triad)
  {
    transLink  = SO_GET_PART(triad->getFdPointer()->getKit(),
                             "firstTrans", SoTransform);
    transLocal = SO_GET_PART(triad->getFdPointer()->getKit(),
                             "secondTrans", SoTransform);
    appearKit  = SO_GET_PART(triad->getFdPointer()->getKit(),
                             "appearance", FdAppearanceKit);
    backPt     = SO_GET_PART(triad->getFdPointer()->getKit(),
                             "backPt", FdBackPointer);
  }
  else
  {
    // Joints should always have a slave triad
    std::cerr <<"FdLinJoint::updateFdTopology:: No slave triad in "
              << itsFmOwner->getIdString(true) << std::endl;
    transLink  = new SoTransform();
    transLocal = new SoTransform();
  }

  itsKit->setPart("slave.firstTrans",transLink);
  itsKit->setPart("slave.secondTrans",transLocal);
  itsKit->setPart("slave.appearance",appearKit);
  itsKit->setPart("slave.backPt",backPt);

  // Set up master part of the joint:

  std::vector<FmTriad*> masters;
  ((FmMMJointBase*)itsFmOwner)->getMasterTriads(masters);

  SoNodeKitListPart* masterList = SO_GET_PART(itsKit,
                                              "masterList", SoNodeKitListPart);
  for (int c = masterList->getNumChildren()-1; c >= 0; c--)
    masterList->removeChild(c);

  int idx = 0;
  for (FmTriad* triad : masters)
  {
    transLink  = SO_GET_PART(triad->getFdPointer()->getKit(),
                             "firstTrans", SoTransform);
    transLocal = SO_GET_PART(triad->getFdPointer()->getKit(),
                             "secondTrans", SoTransform);
    appearKit  = SO_GET_PART(triad->getFdPointer()->getKit(),
                             "appearance", FdAppearanceKit);
    backPt     = SO_GET_PART(triad->getFdPointer()->getKit(),
                             "backPt", FdBackPointer);

    // Make transformkit and set it up:

    FdTransformKit* transformKit = new FdTransformKit();
    transformKit->setPart("firstTrans",transLink);
    transformKit->setPart("secondTrans",transLocal);
    transformKit->setPart("appearance",appearKit);
    transformKit->setPart("backPt",backPt);

    // Insert the transformKit in the master list:

    masterList->insertChild(transformKit,idx++);
  }

  // Set up line symbol:

  FdSprDaTransformKit* symbol = SO_GET_PART(itsKit,
                                            "lineSymbol", FdSprDaTransformKit);

  // Connect first masters transformation to connection symbol
  if (!masters.empty())
  {
    transLink  = SO_GET_PART(masters.front()->getFdPointer()->getKit(),
                             "firstTrans", SoTransform);
    transLocal = SO_GET_PART(masters.front()->getFdPointer()->getKit(),
                             "secondTrans", SoTransform);
    symbol->connectFirstSpace(transLink, transLocal);
  }

  // Connect last masters transformation to connection symbol
  if (masters.size() > 1)
  {
    transLink  = SO_GET_PART(masters.back()->getFdPointer()->getKit(),
                             "firstTrans", SoTransform);
    transLocal = SO_GET_PART(masters.back()->getFdPointer()->getKit(),
                             "secondTrans", SoTransform);
    symbol->connectSecondSpace(transLink, transLocal);
  }

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


bool FdLinJoint::updateFdApperance()
{
  if (!itsFmOwner) return false;

  if (this->highlightRefCount > 0) // This test makes sure we do not
    // un-highlight something when it is supposed to be highlighted
    this->showHighlight();

  else if (((FmJointBase*)itsFmOwner)->isSlaveAttachedToLink(true) &&
           ((FmJointBase*)itsFmOwner)->isMasterAttachedToLink(true))
    itsKit->setPart("lineSymbol.appearance.material",FdSymbolDefs::getJointMaterial());
  else
    itsKit->setPart("lineSymbol.appearance.material",FdSymbolDefs::getDefaultMaterial());

  return true;
}


bool FdLinJoint::updateFdDetails()
{
  if (!itsFmOwner) return false;

  // Set the slave and line symbols

  if (itsFmOwner->isOfType(FmCylJoint::getClassTypeID()))
    itsKit->setPart("slave.symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::LINCYL_SLAVE));
  else
    itsKit->setPart("slave.symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::LINPRISM_SLAVE));

  itsKit->setPart("lineSymbol.symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::LINJOINT_LINE));

  SoNodeKitListPart* masterList = SO_GET_PART(itsKit,"masterList",SoNodeKitListPart);
  int nMasters = masterList->getNumChildren();
  for (int i = 0; i < nMasters; i++)
    ((SoBaseKit*)masterList->getChild(i))->setPart("symbol",FdSymbolDefs::getSymbol(FdSymbolDefs::LINJOINT_MASTER));

  return true;
}


void FdLinJoint::hide()
{
  itsKit->setPart("slave.symbol", NULL);
  itsKit->setPart("lineSymbol.symbol", NULL);
  SoNodeKitListPart* masterList = SO_GET_PART(itsKit, "masterList", SoNodeKitListPart);
  int nMasters = masterList->getNumChildren();
  for (int i = 0; i < nMasters; i++)
    ((SoBaseKit*)masterList->getChild(i))->setPart("symbol", NULL);
}


void FdLinJoint::showHighlight()
{
  itsKit->setPart("slave.appearance.material",     FdSymbolDefs::getHighlightMaterial());
  itsKit->setPart("lineSymbol.appearance.material",FdSymbolDefs::getHighlightMaterial());

#ifdef USE_SMALLCHANGE
  SmDepthBuffer* dbn = FdSymbolDefs::getHighlightDepthBMod();
  itsKit->setPart("lineSymbol.appearance.depth",dbn);
  itsKit->setPart("slave.appearance.depth",dbn);
#endif

  SoNodeKitListPart* masterList = SO_GET_PART(itsKit,"masterList",SoNodeKitListPart);
  int nMasters = masterList->getNumChildren();
  for (int i = 0; i < nMasters; i++)
  {
    SoBaseKit* node = (SoBaseKit*)masterList->getChild(i);
    node->setPart("appearance.material",FdSymbolDefs::getHighlightMaterial());
#ifdef USE_SMALLCHANGE
    node->setPart("appearance.depth",dbn);
#endif
  }
}


void FdLinJoint::hideHighlight()
{
  this->updateFdApperance();
#ifdef USE_SMALLCHANGE
  itsKit->setPart("lineSymbol.appearance.depth",NULL);
#endif

  std::vector<FmTriad*> triads;
  ((FmMMJointBase*)itsFmOwner)->getMasterTriads(triads);
  triads.push_back(((FmJointBase*)itsFmOwner)->getSlaveTriad());
  for (FmTriad* triad : triads)
    triad->getFdPointer()->updateFdApperance();
}


void FdLinJoint::smartMove(const FaVec3& p1, const FaVec3& p2, const FaDOF& dof)
{
  std::vector<FmTriad*> jointTriads;
  ((FmMMJointBase*)itsFmOwner)->getMasterTriads(jointTriads);
  size_t nMasters = jointTriads.size();
  if (nMasters < 2) return;

  const float move_tolr = 3.0e-6f;
  bool isClosestToFirst = false;

  // Lambda function checking whether three points are on a straight line or not.
  auto&& isNotBetween = [&isClosestToFirst](const FaVec3& a, const FaVec3& b, const FaVec3& c)
  {
    FaVec3 fromto = b - a;
    double lengthFromto = fromto.length();
    double posAlongLine = (c-a)*fromto.normalize();
    if (posAlongLine < 0.0)
      return -1;

    if (posAlongLine < 0.5*lengthFromto)
      isClosestToFirst = true;

    return posAlongLine > lengthFromto ? 1 : 0;
  };

  // Insert the slave triad into the array of master triads based on its position
  FmTriad* slaveTriad = ((FmMMJointBase*)itsFmOwner)->getSlaveTriad();
  size_t i = 0;
  for (i = 0; i < nMasters; i++)
    if (i+1 == nMasters)
      jointTriads.push_back(slaveTriad);
    else if (!isNotBetween(jointTriads[i]->getTranslation(),
                           jointTriads[i+1]->getTranslation(),
                           slaveTriad->getTranslation()))
    {
      jointTriads.insert(jointTriads.begin()+i,slaveTriad);
      break;
    }

  // Find the two triads the attacking point (p1) is between :

  long int nearest1Idx = nMasters;
  long int nearest2Idx = nMasters+1; // that's one outside the array
  size_t nearestTriadIdx = nMasters;

  for (i = 0; i < nMasters; i++) // It is nMasters+1 triads in the
  {                              // array but we don't do this for the last triad
    int notBetween = isNotBetween(jointTriads[i]->getTranslation(),
                                  jointTriads[i+1]->getTranslation(),p1);
    if (notBetween == 0)
    {
      nearest1Idx = i;
      nearest2Idx = i+1;
      nearestTriadIdx = isClosestToFirst ? i : i+1;
      break;
    }
    else if (notBetween < 0)
    {
      // This happens when the point is at the same spot as one of the triads
      nearest2Idx = i;
      nearest1Idx = nearest2Idx-1;
      nearestTriadIdx = i;
      break;
    }
  }

  // Find the first "stuck" triads in each direction :
  long int bwdStopperIdx = -1;
  for (long int j = nearest1Idx; j >= 0 && bwdStopperIdx < 0; j--)
    if (jointTriads[j]->hasStickers() || jointTriads[j]->isAttached())
      bwdStopperIdx = j;

  size_t fwdStopperIdx = nMasters+1;
  for (i = nearest2Idx; i <= nMasters && fwdStopperIdx > nMasters; i++)
    if (jointTriads[i]->hasStickers() || jointTriads[i]->isAttached())
      fwdStopperIdx = i;

  long int idx1 = nearest1Idx;
  long int idx2 = nearest2Idx;
  if (nearest1Idx < 0)
    ++idx1, ++idx2;
  else if (nearest2Idx > (long int)nMasters)
    --idx1, --idx2;

  FaVec3 lineDirection = jointTriads[idx2]->getTranslation() - jointTriads[idx1]->getTranslation();
  lineDirection.normalize();

  // Calculate movement and start moving :

  SbVec3f actualTrans(0.0f,0.0f,0.0f);
  SbVec3f newLineDirection, tempVec;
  SbRotation actualRot = SbRotation::identity();

  FaVec3 cToP1OnLineDirTroughCenter;
  FaVec3 cToP2OnNewLineDirTroughCenter;
  FaVec3 p2OnNewLineDirTroughCenter;
  FaVec3 centerToP1, centerToP2;
  FaVec3 centerToP1Norm, centerToP2Norm;
  double cToP2Length, offset;

  switch (dof.getType())
    {
    case FREE:
      actualTrans = FdConverter::toSbVec3f(p2 - p1);
      break;

    case BALL:
      centerToP1 = p1 - dof.getCenter();
      centerToP2 = p2 - dof.getCenter();
      cToP2Length = centerToP2.length();
      cToP1OnLineDirTroughCenter = lineDirection * (centerToP1*lineDirection);
      offset = (cToP1OnLineDirTroughCenter - centerToP1).length();
      if (cToP2Length > 1.0e-7 && cToP2Length > offset)
      {
        double scale  = offset/cToP2Length;
        FaVec3 normal = cToP1OnLineDirTroughCenter^centerToP1;
        normal.normalize();
        p2OnNewLineDirTroughCenter = (p2 - scale*scale*centerToP2 +
                                      scale*sqrt(1.0-scale*scale) * (centerToP2^normal));
        cToP2OnNewLineDirTroughCenter = p2OnNewLineDirTroughCenter - dof.getCenter();
        if (cToP2OnNewLineDirTroughCenter.isZero() || cToP1OnLineDirTroughCenter.isZero())
          actualTrans = FdConverter::toSbVec3f(cToP2OnNewLineDirTroughCenter - cToP1OnLineDirTroughCenter);
        else
        {
          actualRot.setValue(FdConverter::toSbVec3f(cToP1OnLineDirTroughCenter),
                             FdConverter::toSbVec3f(cToP2OnNewLineDirTroughCenter));
          actualRot.multVec(FdConverter::toSbVec3f(cToP1OnLineDirTroughCenter),tempVec);
          actualTrans = FdConverter::toSbVec3f(cToP2OnNewLineDirTroughCenter) - tempVec;
        }
      }
      else
        actualTrans = FdConverter::toSbVec3f(lineDirection * (lineDirection*(p2-p1)));
      break;

    case CYL:
    case REV:
      // Rotation:
      // Get Vectors normal to rot axis:
      centerToP1Norm = dof.normalVector(p1);
      centerToP2Norm = dof.normalVector(p2);
      if (!centerToP1Norm.isZero() && !centerToP2Norm.isZero())
      {
        if (!(centerToP2Norm^centerToP1Norm).isZero())
          actualRot.setValue(FdConverter::toSbVec3f(centerToP1Norm),
                             FdConverter::toSbVec3f(centerToP2Norm));
        else if (centerToP2Norm*centerToP1Norm < 0.0)
          actualRot.setValue(FdConverter::toSbVec3f(dof.getDirection()),(float)M_PI);
      }

      // Get translation along glider. New Line Direction !!!!
      centerToP1 = p1 - dof.getCenter();
      centerToP2 = p2 - dof.getCenter();
      actualRot.multVec(FdConverter::toSbVec3f(lineDirection),newLineDirection);
      actualRot.multVec(FdConverter::toSbVec3f(centerToP1),tempVec);
      actualTrans = newLineDirection * (newLineDirection.dot(FdConverter::toSbVec3f(centerToP2) - tempVec));
      break;

    case PRISM:
    case RIGID:
      // Get translation along glider
      actualTrans = FdConverter::toSbVec3f(lineDirection * (lineDirection*(p2-p1)));
      break;
    }

  // Go trough the triads in the joints and adjust translation for each before starting to animate them

  FdPtPMoveAnimator* animator;
  SoTransform* triadTransform;
  FaVec3 stopperTrans;
  FaVec3 stopperPos;
  FaVec3 stopperBehindPos;

  SbVec3f actualTriadTrans = actualTrans;
  SbVec3f transDirection = actualTrans;
  transDirection.normalize();
  FaVec3 p0 = jointTriads.front()->getTranslation();
  FaVec3 p1OnLine = p0 + lineDirection * ((p1-p0)*lineDirection);

  bool goingFwd = (p2-p1) * lineDirection > 0.0;

  if (jointTriads[nearestTriadIdx]->isSlaveTriad())
    for (i = 0; i <= nMasters; i++)
    {
      FdTriad* displayTriad = (FdTriad*)(jointTriads[i]->getFdPointer());
      triadTransform = SO_GET_PART(displayTriad->getKit(),"secondTrans",SoTransform);
      if (i == nearestTriadIdx)
      {
        stopperPos = goingFwd ? jointTriads[nMasters]->getTranslation() : p0;
        float lengthToStopper = (float)(stopperPos - jointTriads[i]->getTranslation()).length();
        actualTriadTrans = std::min(lengthToStopper-move_tolr,actualTrans.length()) * transDirection;
      }
      else if (dof.getType() != FREE)
        actualTriadTrans = SbVec3f(0,0,0);

      animator = new FdPtPMoveAnimator(triadTransform, displayTriad, actualTriadTrans, actualRot,
                                       FdConverter::toSbVec3f(dof.getCenter()),
                                       FdConverter::toSbVec3f(p1), i == nearestTriadIdx);

      animator->start();
    }

  else
  {
    // Cropping translation to be within stoppers :
    long int stopperIdx = goingFwd && fwdStopperIdx <= nMasters ? fwdStopperIdx : bwdStopperIdx;
    if (stopperIdx >= 0)
    {
      stopperPos = jointTriads[stopperIdx]->getTranslation();
      float lengthToStopper = (float)(stopperPos - p1OnLine).length();
      actualTrans = std::min(lengthToStopper-move_tolr,actualTrans.length()) * transDirection;
    }

    // Loop trough all triads and move them :
    for (i = 0; i <= nMasters; i++)
    {
      long int ii = i; // cast to signed int (i is unsigned)
      if (ii <= bwdStopperIdx || i >= fwdStopperIdx)
        actualTriadTrans = SbVec3f(0,0,0);
      else
      {
        // Triad is between stoppers
        if (ii >= nearest2Idx && fwdStopperIdx <= nMasters)
          stopperIdx = fwdStopperIdx;
        else if (ii < nearest2Idx)
          stopperIdx = bwdStopperIdx;
        else
          stopperIdx = -1;

        if (stopperIdx < 0)
          actualTriadTrans = actualTrans; // Moving an open end, do not crop
        else
        {
          // Moving between stopper triad and attackpoint
          stopperPos = jointTriads[stopperIdx]->getTranslation();
          double lengthAttackToStopper = (p1OnLine - stopperPos).length();
          double lengthTriadToStopper = (jointTriads[i]->getTranslation() - stopperPos).length();
          if (lengthAttackToStopper > 1.0e-16)
            actualTriadTrans = actualTrans * (float)(lengthTriadToStopper/lengthAttackToStopper);
          else
            actualTriadTrans = SbVec3f(0,0,0);
        }
      }

      bool applySticker = i == nearestTriadIdx;
      FdTriad* displayTriad = (FdTriad*)(jointTriads[i]->getFdPointer());
      triadTransform = SO_GET_PART(displayTriad->getKit(),"secondTrans",SoTransform);

      animator = new FdPtPMoveAnimator(triadTransform,displayTriad, actualTriadTrans, actualRot,
                                       FdConverter::toSbVec3f(dof.getCenter()),
                                       FdConverter::toSbVec3f(p1),applySticker);
      animator->start();
    }
  }
}


FaVec3 FdLinJoint::findSnapPoint(const SbVec3f& pointOnObject,
                                 const SbMatrix& objToWorld,
                                 SoDetail*, SoPickedPoint*)
{
  FaVec3 firstTrans = (((FmMMJointBase*)itsFmOwner)->getFirstMaster())->getGlobalTranslation();
  FaVec3 lastTrans  = (((FmMMJointBase*)itsFmOwner)->getLastMaster())->getGlobalTranslation();
  FaVec3 slaveTrans = (((FmMMJointBase*)itsFmOwner)->getSlaveTriad())->getGlobalTranslation();

  SbVec3f worldPoint;
  objToWorld.multVecMatrix(pointOnObject,worldPoint);
  FaVec3 point = FdConverter::toFaVec3(worldPoint);

  double pointTofirst = (firstTrans - point).sqrLength();
  double pointTolast  = (lastTrans  - point).sqrLength();
  double pointToslave = (slaveTrans - point).sqrLength();

  if (pointTofirst < pointToslave)
    return pointTofirst < pointTolast ? firstTrans : lastTrans;
  else
    return pointToslave < pointTolast ? slaveTrans : lastTrans;
}


int FdLinJoint::getDegOfFreedom(SbVec3f& centerPoint, SbVec3f& direction)
{
  bool mastersIsAttached = ((FmMMJointBase*)itsFmOwner)->isMasterAttachedToLink(true);
  bool slaveIsAttached   = ((FmMMJointBase*)itsFmOwner)->isSlaveAttachedToLink(true);
  if (mastersIsAttached && slaveIsAttached)
    return RIGID;

  int hasStickers = (((FmMMJointBase*)itsFmOwner)->getSlaveTriad())->hasStickers();
  FaMat34 triadCS = (((FmMMJointBase*)itsFmOwner)->getSlaveTriad())->getGlobalCS();
  if (mastersIsAttached)
  {
    direction = FdConverter::toSbVec3f(triadCS[2]);
    if (itsFmOwner->isOfType(FmCylJoint::getClassTypeID()))
    {
      centerPoint = FdConverter::toSbVec3f(triadCS.translation());
      // Act as if the sticker is in origo of the slave
      // but it might not be...
      return hasStickers ? REV : CYL;
    }
    else // Its a Prism Joint:
      return hasStickers ? RIGID : PRISM;
  }

  if ((((FmMMJointBase*)itsFmOwner)->getFirstMaster())->hasStickers()) hasStickers += 2;
  if ((((FmMMJointBase*)itsFmOwner)->getLastMaster())->hasStickers())  hasStickers += 4;
  if (slaveIsAttached)
  {
    direction = FdConverter::toSbVec3f(triadCS[2]);
    centerPoint = FdConverter::toSbVec3f(triadCS.translation());
    if (hasStickers < 6)
      // Could used a kind of CYL_STRECH for cylJoints (rotation about axis, and stretch translation)
      // but doesn't because it probably won't gain the user.
      return PRISM_STRECH;
    else
      // Both masters got stickers.
      // Act as if the stickers is on the joint axis if it is a CylJoint.
      return itsFmOwner->isOfType(FmPrismJoint::getClassTypeID()) ? RIGID : REV;
  }

  // Nothing is attached
  if (hasStickers%2 == 0) // no sticker on slave Triad
  {
    if (hasStickers == 4) // sticker on last master Triad only
      triadCS = (((FmMMJointBase*)itsFmOwner)->getLastMaster())->getGlobalCS();
    else if (hasStickers == 2) // sticker on first master Triad only
      triadCS = (((FmMMJointBase*)itsFmOwner)->getFirstMaster())->getGlobalCS();
  }
  direction = FdConverter::toSbVec3f(triadCS[2]);
  centerPoint = FdConverter::toSbVec3f(triadCS.translation());

  if (hasStickers == 7) // stickers on slave and both master triads
    return REV;
  else if (hasStickers == 5 || hasStickers == 3) // stickers on slave and one master triad
    return REV_STRECH;
  else if (hasStickers == 6) // stickers on both master triads but not on the slave triad
    return itsFmOwner->isOfType(FmCylJoint::getClassTypeID()) ? REV_CYL : REV_PRISM;
  else if (hasStickers > 0) // sticker on either the slave triad or one of the master triads
    return BALL_STRECH;

  return FREE;
}
