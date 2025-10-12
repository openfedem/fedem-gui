// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

/***********************************************************************
 * CLASS: FdCtrlObject
 * AUTHOR: Magnus Svanberg
 * LAST MODIFIED: 6 Okt.  1997
 *
 * DESCRIPTION:
 * Base class for the control objects. Derived from the FdObject class.
 **********************************************************************/

#ifndef FD_CTRL_OBJECT_H
#define FD_CTRL_OBJECT_H

#include "vpmDisplay/FdBase.H"

class FdCtrlLineKit;


class FdCtrlObject : public FdObject
{
  Fmd_HEADER_INIT();

public:
  FdCtrlObject();

  virtual bool updateFdApperance();

  virtual FaVec3 findSnapPoint(const SbVec3f&, const SbMatrix&,
                               SoDetail*, SoPickedPoint*);

  void erase();

  static void removeFeedbackLine();
  static void drawFbLine(const SbVec3f& startVec, const SbVec3f& endVec);

protected:
  virtual ~FdCtrlObject() {}

  virtual void showHighlight();
  virtual void hideHighlight();

private:
  static FdCtrlLineKit* fbLineNode;
};

#endif
