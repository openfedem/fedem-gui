// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_OBJECT_H
#define FD_OBJECT_H

#include "FFaLib/FFaTypeCheck/FFaTypeCheck.H"
#include <cstddef>

class FmIsRenderedBase;
class FaDOF;
class FaVec3;
class SoDetail;
class SoPickedPoint;
class SoBaseKit;
class SoNodeKitListPart;
class SbVec3f;
class SbMatrix;


class FdObject
{
  Fmd_BASE_HEADER_INIT();

public:
  enum DOFType
  {
    RIGID,
    REV,
    BALL,
    PRISM,
    CYL,
    FREE,
    REV_STRECH,
    REV_CYL,
    REV_PRISM,
    PRISM_STRECH,
    BALL_STRECH
  };

  bool fdErase();
  bool fdDisconnect();

  FmIsRenderedBase* getFmOwner() const { return itsFmOwner; }
  virtual SoBaseKit* getKit() const { return itsKit; }

  virtual bool updateFdAll(bool updateChildrenDisplay = true);
  virtual bool updateFdTopology(bool updateChildrenDisplay = true) = 0;
  virtual bool updateFdDetails() = 0;
  virtual bool updateFdApperance() = 0;
  virtual bool updateFdHighlight();
  virtual bool updateFdCS() { return true; }
  virtual bool updateFmOwner() { return true; }

  void select() { ++selectRefCount; }
  void deselect() { if (selectRefCount > 0) --selectRefCount; }
  bool isSelected() const { return selectRefCount > 0; }

  void highlight(bool onOrOff);

  // Internally (for vpmDisplay) used methods:

  virtual void smartMove(const FaVec3&, const FaVec3&, const FaDOF&) {}
  virtual FaVec3 findSnapPoint(const SbVec3f& pointOnObject,
                               const SbMatrix& objToWorld,
                               SoDetail* = NULL, SoPickedPoint* = NULL);
  virtual int getDegOfFreedom(SbVec3f&, SbVec3f&) { return RIGID; }

  virtual void removeDisplayData() {}

protected:
  FdObject();
  virtual ~FdObject() {}

  virtual SoNodeKitListPart* getListSw() const = 0;

  virtual void showHighlight();
  virtual void hideHighlight();

  static bool isParallell(const SbVec3f& v1, const SbVec3f& v2,
                          float tolerance = 1.0e-7f);

  FmIsRenderedBase* itsFmOwner;
  SoBaseKit*        itsKit;

  bool isInserted;
  int  highlightRefCount;
  int  selectRefCount;
};

#endif
