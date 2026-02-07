// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>

#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdPipeSurfaceKit.H"


SO_KIT_SOURCE(FdPipeSurfaceKit);


void FdPipeSurfaceKit::init()
{
  SO_KIT_INIT_CLASS(FdPipeSurfaceKit, SoBaseKit, "BaseKit");
}


FdPipeSurfaceKit::FdPipeSurfaceKit()
{
  SO_KIT_CONSTRUCTOR(FdPipeSurfaceKit);
  isBuiltIn = TRUE;

  SO_KIT_ADD_CATALOG_ENTRY(      material, SoMaterial,    TRUE, this, \x0, TRUE );
  SO_KIT_ADD_CATALOG_LIST_ENTRY( pipeList, SoSeparator,   TRUE, this, \x0, SoSeparator, TRUE );
  SO_KIT_ADD_CATALOG_ENTRY(      backPt,   FdBackPointer, TRUE, this, \x0, TRUE );

  SO_KIT_INIT_INSTANCE();
}


FdPipeSurfaceKit::~FdPipeSurfaceKit()
{
}
