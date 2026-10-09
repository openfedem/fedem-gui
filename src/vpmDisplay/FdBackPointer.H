// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

/*****************************************************************
 * CLASS: FdBackPointer
 * AUTHOR: Jens LIEN
 * LAST MODIFIED: 16 nov 1995
 *
 * DESCRIPTION: Open Inventor node with pointer to an instance of
 * FdObject or inherited classes.
 *****************************************************************/

#ifndef FD_BACK_POINTER_H
#define FD_BACK_POINTER_H

#include <Inventor/nodes/SoSubNode.h>

class FdObject;

#ifdef win32
#include <SoWinLeaveScope.h>
#endif

class FdBackPointer : public SoNode
{
  SO_NODE_HEADER(FdBackPointer);

public:
  static void init();

  FdBackPointer(FdObject* pt = NULL);

  FdObject* getPointer() { return structPt; }
  void setPointer(FdObject* pt) { structPt = pt; }

  // Dummy override to avoid any output to file
  void write(SoWriteAction*) override {}

private:
  FdObject* structPt;
};

#ifdef win32
#include <SoWinEnterScope.h>
#endif

#endif
