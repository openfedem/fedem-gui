// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FDLINJOINT_H
#define FDLINJOINT_H

#include "vpmDisplay/FdBase.H"

class FmMMJointBase;


class FdLinJoint : public FdObject
{
  Fmd_HEADER_INIT();

public:
  FdLinJoint(FmMMJointBase* pt);

  virtual bool updateFdTopology(bool updateChildrenDisplay = true);
  virtual bool updateFdDetails();
  virtual bool updateFdApperance();
  virtual void hide();

  virtual void smartMove(const FaVec3& firstPoint, const FaVec3& secondPoint,
			 const FaDOF& dof);
  virtual SbVec3f findSnapPoint(const SbVec3f& pointOnObject,
				const SbMatrix& objToWorld,
				SoDetail* pickDetail = 0,
				SoPickedPoint* pPoint = 0);
  virtual int getDegOfFreedom(SbVec3f& centerPoint, SbVec3f& direction);

protected:
  virtual ~FdLinJoint();

  virtual SoNodeKitListPart* getListSw() const;

  virtual void showHighlight();
  virtual void hideHighlight();
};

#endif
