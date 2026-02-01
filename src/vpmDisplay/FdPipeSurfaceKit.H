// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_PIPE_SURFACE_KIT_H
#define FD_PIPE_SURFACE_KIT_H

#include <Inventor/nodekits/SoBaseKit.h>

#ifdef win32
#include <SoWinLeaveScope.h>
#endif

class FdPipeSurfaceKit : public SoBaseKit
{
  SO_KIT_HEADER(FdPipeSurfaceKit);

  // Defines fields for the new parts in the catalog

  SO_KIT_CATALOG_ENTRY_HEADER(material);
  SO_KIT_CATALOG_ENTRY_HEADER(pipeList);
  SO_KIT_CATALOG_ENTRY_HEADER(backPt);

public:
  FdPipeSurfaceKit();

  SoINTERNAL public:
  static void init();

private:
  virtual ~FdPipeSurfaceKit();
};

#ifdef win32
#include <SoWinEnterScope.h>
#endif

#endif
