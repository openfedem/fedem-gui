// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_SIMPLE_JOINT_H
#define FD_SIMPLE_JOINT_H

#include "vpmDisplay/FdBase.H"

class FmSMJointBase;


class FdSimpleJoint : public FdObject
{
  Fmd_HEADER_INIT();

public:
  FdSimpleJoint(FmSMJointBase* pt);

  virtual void hide();
  virtual bool updateFdTopology(bool updateChildrenDisplay);
  virtual bool updateFdDetails();
  virtual bool updateFdApperance();
  virtual bool updateFmOwner();

  virtual void smartMove(const FaVec3& p1, const FaVec3& p2, const FaDOF& dof);
  virtual FaVec3 findSnapPoint(const SbVec3f& pointOnObject,
                               const SbMatrix& objToWorld,
                               SoDetail*, SoPickedPoint*);
  virtual int getDegOfFreedom(SbVec3f& centerPoint, SbVec3f& direction);

protected:
  virtual ~FdSimpleJoint();

  virtual SoNodeKitListPart* getListSw() const;

  virtual void showHighlight();
  virtual void hideHighlight();
};

#endif
