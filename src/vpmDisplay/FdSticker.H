// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_STICKER_H
#define FD_STICKER_H

#include "vpmDisplay/FdBase.H"

class FmSticker;


class FdSticker : public FdObject
{
  Fmd_HEADER_INIT();

public:
  FdSticker(FmSticker* pt);

  virtual bool updateFdTopology(bool updateChildrenDisplay);
  virtual bool updateFdDetails();
  virtual bool updateFdApperance();
  virtual bool updateFdCS();

  virtual FaVec3 findSnapPoint(const SbVec3f&, const SbMatrix&,
                               SoDetail*, SoPickedPoint*);

protected:
  virtual ~FdSticker();

  virtual SoNodeKitListPart* getListSw() const;
};

#endif
