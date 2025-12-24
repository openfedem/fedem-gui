// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdStrainRosette.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdExtraGraphics.H"
#include "vpmDisplay/FdStrainRosetteKit.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDB/FmStrainRosette.H"
#include "vpmDB/FmLink.H"

#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>


/**********************************************************************
 *
 * CLASS FdStrainRosette
 *
 **********************************************************************/

Fmd_SOURCE_INIT(FDSTRAINROSETTE,FdStrainRosette,FdObject);

FdStrainRosette::FdStrainRosette(FmStrainRosette* pt) : FdObject()
{
  Fmd_CONSTRUCTOR_INIT(FdStrainRosette);

  itsFmOwner = pt;

  itsKit = new FdStrainRosetteKit;
  itsKit->ref();

  // Set up back pointer
  FdBackPointer* bp_pointer = SO_GET_PART(itsKit,"backPt",FdBackPointer);
  bp_pointer->setPointer(this);
}


FdStrainRosette::~FdStrainRosette()
{
  this->fdDisconnect();
  itsKit->unref();
}


SoNodeKitListPart* FdStrainRosette::getListSw() const
{
  return SO_GET_PART(FdDB::getMechanismKit(),"rosetteListSw",SoNodeKitListPart);
}


bool FdStrainRosette::updateFdTopology(bool)
{
  this->updateFdCS();

  SoTransform* transLink = NULL;
  FmLink* link = static_cast<FmStrainRosette*>(itsFmOwner)->rosetteLink;
  if (link && link->getFdPointer())
    transLink = SO_GET_PART(link->getFdPointer()->getKit(),"transform",SoTransform);

  itsKit->setPart("firstTrans",transLink);

  return true;
}


bool FdStrainRosette::updateFdApperance()
{
  if (highlightRefCount < 1) // This test makes sure we do not
    // un-highlight something when it is supposed to be highlighted
    itsKit->setPart("appearance.material",FdSymbolDefs::getMaterial(FdSymbolDefs::STRAIN_ROS));

  return true;
}


bool FdStrainRosette::updateFdCS()
{
  // Set link relative transformation for symbols
  FmStrainRosette* fmRosette = static_cast<FmStrainRosette*>(itsFmOwner);
  FaMat34 posMx = fmRosette->getSymbolPosMx();

  SoTransform* transLocal = new SoTransform;
  transLocal->setMatrix(FdConverter::toSbMatrix(posMx));
  itsKit->setPart("secondTrans",transLocal);

  // Set scale for small symbol

  float scale = float(0.8*fmRosette->getElmWidth());
  SoScale* smallScale = new SoScale;
  smallScale->scaleFactor.setValue(scale,scale,scale);
  itsKit->setPart("scale", smallScale);

  return true;
}


bool FdStrainRosette::updateFdDetails()
{
  FmStrainRosette* fmRosette = static_cast<FmStrainRosette*>(itsFmOwner);

  SoCoordinate3* coords = (SoCoordinate3*)itsKit->getPart("elmCoords", true);
  SoIndexedLineSet* shape = SO_GET_PART(itsKit, "elmShape", SoIndexedLineSet);

  int i;
  coords->point.deleteValues(0,-1);
  shape->coordIndex.deleteValues(0,-1);

  for (i = 0; i < fmRosette->getNoNodes(); i++)
  {
    coords->point.set1Value(i,FdConverter::toSbVec3f(fmRosette->getNodePos(i)));
    shape->coordIndex.set1Value(i,i);
  }
  shape->coordIndex.set1Value(i,0);
  shape->coordIndex.set1Value(i+1,-1);

  FdSymbolKit* smallSymbol = NULL;
  FdSymbolKit* largeSymbol = NULL;
  switch (fmRosette->rosetteType.getValue())
    {
    case FmStrainRosette::SINGLE_GAGE:
      smallSymbol = FdSymbolDefs::getSymbol(FdSymbolDefs::S_SINGLE_GAGE);
      largeSymbol = FdSymbolDefs::getSymbol(FdSymbolDefs::L_SINGLE_GAGE);
      break;
    case FmStrainRosette::DOUBLE_GAGE_90:
      smallSymbol = FdSymbolDefs::getSymbol(FdSymbolDefs::S_DOUBLE_GAGE_90);
      largeSymbol = FdSymbolDefs::getSymbol(FdSymbolDefs::L_DOUBLE_GAGE_90);
      break;
    case FmStrainRosette::TRIPLE_GAGE_60:
      smallSymbol = FdSymbolDefs::getSymbol(FdSymbolDefs::S_TRIPLE_GAGE_60);
      largeSymbol = FdSymbolDefs::getSymbol(FdSymbolDefs::L_TRIPLE_GAGE_60);
      break;
    case FmStrainRosette::TRIPLE_GAGE_45:
      smallSymbol = FdSymbolDefs::getSymbol(FdSymbolDefs::S_TRIPLE_GAGE_45);
      largeSymbol = FdSymbolDefs::getSymbol(FdSymbolDefs::L_TRIPLE_GAGE_45);
      break;
    }

  itsKit->setPart("smallSymbol", smallSymbol);
  itsKit->setPart("largeSymbol", largeSymbol);

  return true;
}
