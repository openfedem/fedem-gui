// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_LIN_JOINT_H
#define FD_LIN_JOINT_H

#include "vpmDisplay/FdBase.H"

class FmMMJointBase;


class FdLinJoint : public FdObject
{
  Fmd_HEADER_INIT();

public:
  FdLinJoint(FmMMJointBase* pt);

  virtual bool updateFdTopology(bool updateChildrenDisplay);
  virtual bool updateFdDetails();
  virtual bool updateFdApperance();
  virtual void hide();

  virtual void smartMove(const FaVec3& firstPoint, const FaVec3& secondPoint,
                         const FaDOF& dof);
  virtual FaVec3 findSnapPoint(const SbVec3f& pointOnObject,
                               const SbMatrix& objToWorld,
                               SoDetail* pickDetail, SoPickedPoint* pPoint);
  virtual int getDegOfFreedom(SbVec3f& centerPoint, SbVec3f& direction);

protected:
  virtual ~FdLinJoint();

  virtual SoNodeKitListPart* getListSw() const;

  virtual void showHighlight();
  virtual void hideHighlight();
};

#endif
