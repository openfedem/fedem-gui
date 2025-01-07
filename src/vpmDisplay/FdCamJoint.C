// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdCamJoint.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdCamJointKit.H"
#include "vpmDisplay/FdCurveKit.H"
#include "vpmDisplay/FdTransformKit.H"
#include "vpmDisplay/FdTriad.H"
#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdAppearanceKit.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdPtPMoveAnimator.H"
#include "vpmDisplay/FdConverter.H"

#include "FFaLib/FFaString/FFaStringExt.H"
#include "vpmDB/FmCamJoint.H"
#include "vpmDB/FmArcSegmentMaster.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmLink.H"

#include <Inventor/nodes/SoCoordinate4.h>
#include <Inventor/nodes/SoNurbsCurve.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoDrawStyle.h>
#ifdef USE_SMALLCHANGE
#include <SmallChange/nodes/SmDepthBuffer.h>
#endif


/**********************************************************************
 *
 * CLASS FdCamJoint
 *
 **********************************************************************/

Fmd_SOURCE_INIT(FDCAMJOINT,FdCamJoint,FdObject);

FdCamJoint::FdCamJoint(FmCamJoint* pt) : FdObject()
{
  Fmd_CONSTRUCTOR_INIT(FdCamJoint);

  itsFmOwner = pt;

  itsKit = new FdCamJointKit;
  itsKit->ref();

  // Set up back pointer
  FdBackPointer* bp_pointer = SO_GET_PART(itsKit,"backPt",FdBackPointer);
  bp_pointer->setPointer(this);

  // Set up back pointer connections in the kit.
  // The master connection has to be set up for each master.
  // This is done in FdCamJoint::updateFdTopology()
  itsKit->setPart("slave.backPt",bp_pointer);
}


FdCamJoint::~FdCamJoint()
{
  this->fdDisconnect();
  itsKit->unref();
}


SoNodeKitListPart* FdCamJoint::getListSw() const
{
  return SO_GET_PART(FdDB::getMechanismKit(),"camJointListSw",SoNodeKitListPart);
}


bool FdCamJoint::updateFdTopology(bool updateChildrenDisplay)
{
  if (!itsFmOwner->isOfType(FmCamJoint::getClassTypeID()))
    return false;

  FmCamJoint* fmJoint = (FmCamJoint*)itsFmOwner;

  // Set up slave part of the joint:

  FmTriad* triad = fmJoint->getSlaveTriad();
  if (!triad) // Joints should always have a slave triad
  {
    std::cerr <<"No slave triad in "<< itsFmOwner->getIdString(true) << std::endl;
    return false;
  }

  FdObject*        fdTriad       = triad->getFdPointer();
  SoTransform*     transLink     = SO_GET_PART(fdTriad->getKit(),"firstTrans",SoTransform);
  SoTransform*     transLocal    = SO_GET_PART(fdTriad->getKit(),"secondTrans",SoTransform);
  FdAppearanceKit* appearanceKit = SO_GET_PART(fdTriad->getKit(),"appearance",FdAppearanceKit);
  FdBackPointer*   backPt        = SO_GET_PART(fdTriad->getKit(),"backPt",FdBackPointer);

  itsKit->setPart("slave.firstTrans",transLink);
  itsKit->setPart("slave.secondTrans",transLocal);
  itsKit->setPart("slave.appearance",appearanceKit);
  itsKit->setPart("slave.backPt",backPt);

  // Set up master part of the joint:

  SoNodeKitListPart* masterList = SO_GET_PART(itsKit,"masterList",SoNodeKitListPart);
  for (int c = masterList->getNumChildren()-1; c >= 0; c--)
    masterList->removeChild(c);

  std::vector<FmTriad*> masters;
  fmJoint->getMasterTriads(masters);
  for (FmTriad* master : masters)
  {
    fdTriad       = master->getFdPointer();
    transLink     = SO_GET_PART(fdTriad->getKit(),"firstTrans",SoTransform);
    transLocal    = SO_GET_PART(fdTriad->getKit(),"secondTrans",SoTransform);
    appearanceKit = SO_GET_PART(fdTriad->getKit(),"appearance",FdAppearanceKit);
    backPt        = SO_GET_PART(fdTriad->getKit(),"backPt",FdBackPointer);

    // Make transformkit and set it up:

    FdTransformKit* transformKit = new FdTransformKit;
    transformKit->setPart("firstTrans",transLink);
    transformKit->setPart("secondTrans",transLocal);
    transformKit->setPart("appearance",appearanceKit);
    transformKit->setPart("backPt",backPt);

    // Insert the transformKit in the master list:
    masterList->addChild(transformKit);
  }

  ////////////
  //
  // Set up line symbols:
  //
  ///////////////

  SoDrawStyle* style = new SoDrawStyle;
  SoMaterial* material = FdSymbolDefs::getJointMaterial();
  std::vector<double> knots, weights;
  std::vector<FaVec3> ctrlPoints;

  transLink = NULL;
  if (fmJoint->isMasterAttachedToLink())
  {
    FdObject* fdLink = fmJoint->getMasterLink()->getFdPointer();
    if (fdLink)
      transLink = SO_GET_PART(fdLink->getKit(),"transform",SoTransform);
  }
  itsKit->setPart("camTrans",transLink);

  std::vector<CurveSegment> curveSegments;
  FmArcSegmentMaster* master = dynamic_cast<FmArcSegmentMaster*>(fmJoint->getMaster());
  if (master) master->getCurveSegments(curveSegments);
  SoNodeKitListPart* curveList = SO_GET_PART(itsKit,"curveList",SoNodeKitListPart);
  for (int c = curveList->getNumChildren()-1; c >= 0; c--)
    curveList->removeChild(c);

  size_t i = 0, j;
  for (const CurveSegment& segment : curveSegments)
  {
    FdCurveKit* curveKit = new FdCurveKit;
    curveKit->setPart("style",style);
    curveKit->setPart("material",material);
    arcToNURBS(segment[0],segment[1],segment[2],knots,ctrlPoints,weights);

    SoCoordinate4* coords = SO_GET_PART(curveKit,"coords",SoCoordinate4);
    for (j = 0; j < ctrlPoints.size(); j++)
      coords->point.set1Value(j,ctrlPoints[j].x(),ctrlPoints[j].y(),ctrlPoints[j].z(),weights[j]);

    SoNurbsCurve* curveShape = SO_GET_PART(curveKit,"curve",SoNurbsCurve);
    curveShape->numControlPoints.setValue((int32_t)ctrlPoints.size());
    for (j = 0; j < knots.size(); j++)
      curveShape->knotVector.set1Value(j,(float)knots[j]);
    curveList->insertChild(curveKit,i++);
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


bool FdCamJoint::updateFdApperance()
{
  // This test makes sure we do not unhighlight something
  // when it is supposed to be highlighted
  if (this->highlightRefCount > 0) return true;

  bool isAttached = (((FmJointBase*)itsFmOwner)->isSlaveAttachedToLink(true) &&
		     ((FmJointBase*)itsFmOwner)->isMasterAttachedToLink(true));

  SoNodeKitListPart* list = SO_GET_PART(itsKit,"curveList",SoNodeKitListPart);
  int nNodes = list->getNumChildren();

  for (int i = 0; i < nNodes; i++)
  {
    SoBaseKit* node = (SoBaseKit*)list->getChild(i);
    if (isAttached)
      node->setPart("material",FdSymbolDefs::getJointMaterial());
    else
      node->setPart("material",FdSymbolDefs::getDefaultMaterial());
  }
  return true;
}

void FdCamJoint::hide()
{
  // Set the slave symbol

  itsKit->setPart("slave.symbol", NULL);

  // Set the master symbols

  SoNodeKitListPart* list = SO_GET_PART(itsKit, "masterList", SoNodeKitListPart);
  int nNodes = list->getNumChildren();
  for (int i = 0; i < nNodes; i++)
  {
    FFaNumStr name("masterList[%d].symbol", i);
    itsKit->setPart(name.c_str(), NULL);
  }

  SoNodeKitListPart* curveList = SO_GET_PART(itsKit, "curveList", SoNodeKitListPart);
  for (int c = curveList->getNumChildren() - 1; c >= 0; c--)
    curveList->removeChild(c);
}

bool FdCamJoint::updateFdDetails()
{
  // Set the slave symbol

  itsKit->setPart("slave.symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::CAMJOINT_SLAVE));

  // Set the master symbols

  SoNodeKitListPart* list = SO_GET_PART(itsKit,"masterList",SoNodeKitListPart);
  int nNodes = list->getNumChildren();
  for (int i = 0; i < nNodes; i++)
  {
    FFaNumStr name("masterList[%d].symbol",i);
    itsKit->setPart(name.c_str(), FdSymbolDefs::getSymbol(FdSymbolDefs::CAMJOINT_MASTER));
  }
  return true;
}


void FdCamJoint::showHighlight()
{
#ifdef USE_SMALLCHANGE
  SmDepthBuffer* dbn = FdSymbolDefs::getHighlightDepthBMod();
#endif
  SoMaterial* hmat = FdSymbolDefs::getHighlightMaterial();

  // Highlight Slave :

  itsKit->setPart("slave.appearance.material",hmat);
#ifdef USE_SMALLCHANGE
  itsKit->setPart("slave.appearance.depth",dbn);
#endif

  // Highlight Masters :

  SoNodeKitListPart* list = SO_GET_PART(itsKit,"masterList",SoNodeKitListPart);
  int i, nNodes = list->getNumChildren();
  for (i = 0; i < nNodes; i++)
  {
    SoNode* node = list->getChild(i);
    ((SoBaseKit*)node)->setPart("appearance.material",hmat);
#ifdef USE_SMALLCHANGE
    ((SoBaseKit*)node)->setPart("appearance.depth",dbn);
#endif
  }

  // Highlight Curves :

  list = SO_GET_PART(itsKit,"curveList",SoNodeKitListPart);
  nNodes = list->getNumChildren();
  for (i = 0; i < nNodes; i++)
    ((SoBaseKit*)(list->getChild(i)))->setPart("material",hmat);
}


void FdCamJoint::hideHighlight()
{
  this->updateFdApperance();
  ((FmJointBase*)itsFmOwner)->getSlaveTriad()->getFdPointer()->updateFdApperance();

  std::vector<FmTriad*> masters;
  ((FmMMJointBase*)itsFmOwner)->getMasterTriads(masters);

  for (FmTriad* master : masters)
    master->getFdPointer()->updateFdApperance();
}


void FdCamJoint::smartMove(const FaVec3& p1, const FaVec3& p2, const FaDOF& dof)
{
  SbVec3f translation(0,0,0);
  SbRotation rotation;

  switch (dof.getType())
    {
    case FaDOF::FREE:
      translation = FdConverter::toSbVec3f(p2 - p1);
      rotation = SbRotation::identity();
      break;

    case FaDOF::BALL:
      if (p1.equals(dof.getCenter()) || p2.equals(dof.getCenter()))
        rotation = SbRotation::identity();
      else // We will compute sensible rotation
        rotation.setValue(FdConverter::toSbVec3f(p1 - dof.getCenter()),
                          FdConverter::toSbVec3f(p2 - dof.getCenter()));
      break;

    case FaDOF::REV:
      {
        FaVec3 vec1 = dof.normalVector(p1);
        FaVec3 vec2 = dof.normalVector(p2);
        if (vec1.isZero() || vec2.isZero())
          rotation = SbRotation::identity();
        else // We will compute sensible rotation
          rotation.setValue(FdConverter::toSbVec3f(vec1),
                            FdConverter::toSbVec3f(vec2));
      }
      break;
    }

  SbVec3f firstPoint  = FdConverter::toSbVec3f(p1);
  SbVec3f centerPoint = FdConverter::toSbVec3f(dof.getCenter());

  FdPtPMoveAnimator* masterAnimator;
  SoTransform*       masterTransform;

  std::vector<FmTriad*> triads;
  ((FmCamJoint*)itsFmOwner)->getMasterTriads(triads);

  bool addSticker = true;
  for (FmTriad* triad : triads)
  {
    FdTriad* master = (FdTriad*)triad->getFdPointer();
    masterTransform = SO_GET_PART(master->getKit(),"secondTrans",SoTransform);
    masterAnimator = new FdPtPMoveAnimator(masterTransform, master, translation,rotation, centerPoint,firstPoint, addSticker);
    masterAnimator->start();
    addSticker = false;
  }

  masterTransform = SO_GET_PART(itsKit,"camTrans",SoTransform);
  masterAnimator = new FdPtPMoveAnimator(masterTransform, this, translation,rotation, centerPoint,firstPoint, false);
}


void FdCamJoint::arcToNURBS(const FaVec3& p1, const FaVec3& p2, const FaVec3& p3,
                            std::vector<double>& xi,
                            std::vector<FaVec3>& P, std::vector<double>& w)
{
  const double zero_tol = 1.0e-7;

  FaVec3 p1p2Vec = p2 - p1;
  FaVec3 p1p3Vec = p3 - p1;
  double p1p2 = p1p2Vec.length();
  double p1p3 = p1p3Vec.length();

  FaVec3 p1p3Dir(p1p3Vec);
  p1p3Dir.normalize();

  double l = p1p2Vec * p1p3Dir;
  if (p1p2*p1p2/l >= p1p3 - 2.0*zero_tol)
    return; // We have to make more control points and knots

  // The arc is less than 180 degrees

  FaVec3 hVecDir = p1p2Vec - p1p3Dir*l;
  double h = hVecDir.length();
  hVecDir.normalize();

  xi.resize(6,0.0);
  xi[3] = xi[4] = xi[5] = 1.0;

  double midP1p3ToNewCtrlPoint = 0.0;
  if (h >= zero_tol)
    midP1p3ToNewCtrlPoint = (p1p3*p1p3)/(2 * ((p1p3*l)-(p1p2*p1p2))/h);

  P.resize(3);
  P[0] = p1;
  P[1] = p1 + p1p3Vec*0.5 + hVecDir*midP1p3ToNewCtrlPoint;
  P[2] = p3;

  w.resize(3,1.0);
  w[1] = 0.5*p1p3 / sqrt(0.25*p1p3*p1p3 + midP1p3ToNewCtrlPoint*midP1p3ToNewCtrlPoint); // Cos(angle(p1NewCtrlpointVec,p1p3Vec))

  // Iv Conversion:
  P[1] *= w[1];
}
