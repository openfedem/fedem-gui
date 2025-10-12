// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_TRIAD_H
#define FD_TRIAD_H

#include "vpmDisplay/FdBase.H"
#include "vpmDisplay/FdAnimatedBase.H"

#include "FFaLib/FFaOperation/FFaOperation.H"
#include "FFaLib/FFaAlgebra/FFaMat34.H"

class FmTriad;


class FdTriad : public FdObject, public FdAnimatedBase
{
  Fmd_HEADER_INIT();

public:
  FdTriad(FmTriad* pt);

  virtual SoBaseKit* getKit() const { return itsTrKit; }

  virtual bool updateFdTopology(bool updateChildrenDisplay);
  virtual bool updateFdDetails();
  virtual bool updateFdApperance();
  virtual bool updateFdCS();
  virtual bool updateFmOwner();

  virtual void initAnimation() {}
  virtual void resetAnimation() { this->selectAnimationFrame(0); }
  virtual void selectAnimationFrame(size_t frameNr = 0);
  virtual void deleteAnimationData();

  bool hasResultTransform(size_t frameIdx);
  void setResultTransform(size_t frameIdx, const FaMat34& pos);

  virtual void smartMove(const FaVec3& p1, const FaVec3& p2, const FaDOF& dof);
  virtual FaVec3 findSnapPoint(const SbVec3f& pointOnObject,
                               const SbMatrix& objToWorld,
                               SoDetail* pickDetail, SoPickedPoint* pPoint);
  virtual int getDegOfFreedom(SbVec3f& centerPoint, SbVec3f& direction);

  void setPosMxReadOp(FFaOperation<FaMat34>* readOp);

  FFaOperation<FaMat34>* getPosMxReadOp() { return myPosMxReadOp; }

protected:
  virtual ~FdTriad();

  virtual SoNodeKitListPart* getListSw() const;

  virtual void showHighlight();
  virtual void hideHighlight();

private:
  SoBaseKit* itsTrKit;

  size_t myCurrentResultsFrame;

  FFaOperation<FaMat34>* myPosMxReadOp;
  std::vector<FaMat34*>  myResultsFrames;

  FaMat34& findOrCreateXfMx(size_t frameIdx);
};

#endif
