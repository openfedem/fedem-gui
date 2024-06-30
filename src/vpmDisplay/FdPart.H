// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_PART_H
#define FD_PART_H

#include "vpmDisplay/FdLink.H"

class FmPart;
class FFlGroupPartCreator;


class FdPart : public FdLink
{
  Fmd_HEADER_INIT();

public:
  FdPart(FmPart* part);

  virtual bool updateFdDetails();
  virtual bool updateFdApperance();
  virtual bool updateFmOwner();

  virtual void updateSimplifiedViz();
  virtual void updateElementVisibility();

  bool updateSpecialLines(double scale = 0.0);

  virtual void smartMove(const FaVec3& p1, const FaVec3& p2, const FaDOF& dof);
  virtual SbVec3f findSnapPoint(const SbVec3f& pointOnObject,
				const SbMatrix& objToWorld,
				SoDetail* pickDetail = NULL,
				SoPickedPoint* pPoint = NULL);
  bool findNode(int& nodeID, FaVec3& worldNodePos,
                const SbVec3f& pickPoint) const;
  virtual int getDegOfFreedom(SbVec3f& centerPoint, SbVec3f& direction);

  void showInternalCSs(bool doShow);
  virtual void showCoGCS(bool doShow);

  virtual void removeDisplayData();
  virtual void removeVisualizationData(bool removeCadDataToo = false);

  FFlGroupPartCreator* getGroupPartCreator() const { return myGroupPartCreator; }

private:
  bool createFEViz();

protected:
  virtual ~FdPart();

  virtual void showHighlight();

  virtual SoNodeKitListPart* getListSw() const;

private:
  FFlGroupPartCreator* myGroupPartCreator;
};

#endif
