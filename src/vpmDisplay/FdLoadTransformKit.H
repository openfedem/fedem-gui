// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_LOAD_TRANSFORM_KIT_H
#define FD_LOAD_TRANSFORM_KIT_H

#include <Inventor/engines/SoCompose.h>
#include <Inventor/engines/SoConcatenate.h>

#include "vpmDisplay/FdLoadDirEngine.H"
#include "vpmDisplay/FdTransformKit.H"


#ifdef win32
#include <SoWinLeaveScope.h>
#endif

class SoTransform;


class FdLoadTransformKit : public FdTransformKit 
{ 
  SO_KIT_HEADER(FdLoadTransformKit); 
 
public:

  SoSFVec3f attackCoord;
  SoSFVec3f firstCoord;
  SoSFVec3f secondCoord;

  // Constructor
  FdLoadTransformKit(); 

  void connectAttackSpace(SoTransform* global, SoTransform* local = NULL);
  void disConnectAttackSpace();
  void connectFirstSpace(SoTransform* global, SoTransform* local = NULL);
  void disConnectFirstSpace();
  void connectSecondSpace(SoTransform* global, SoTransform* local = NULL);
  void disConnectSecondSpace();

  SoINTERNAL public:

  static void init(); 

protected: 
 
  FdLoadDirEngine* placerEngine;
  
  SoComposeMatrix* attackMxMaker;
  SoComposeMatrix* matrixMaker1;
  SoComposeMatrix* matrixMaker2;
  
  SoConcatenate* attackTransCat;
  SoConcatenate* attackRotCat;
  SoConcatenate* attackScaleCat;
  SoConcatenate* attackScaleOriCat;
  SoConcatenate* attackCenterCat;
  
  SoConcatenate* transCat1;
  SoConcatenate* rotCat1;
  SoConcatenate* scaleCat1;
  SoConcatenate* scaleOriCat1;
  SoConcatenate* centerCat1;
  
  SoConcatenate* transCat2;
  SoConcatenate* rotCat2;
  SoConcatenate* scaleCat2;
  SoConcatenate* scaleOriCat2;
  SoConcatenate* centerCat2;

  // Destructor
  virtual ~FdLoadTransformKit();
  void setUp();
};


#ifdef win32
#include <SoWinEnterScope.h>
#endif

#endif
