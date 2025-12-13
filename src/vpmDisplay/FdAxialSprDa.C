// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdAxialSprDa.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdSprDaTransformKit.H"
#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDB/FmAxialSpring.H"
#include "vpmDB/FmAxialDamper.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmLink.H"

#include <Inventor/nodes/SoMaterial.h>


/**********************************************************************
 *
 * CLASS FdAxialSprDa
 *
 **********************************************************************/

Fmd_SOURCE_INIT(FDAXIALSPRDA,FdAxialSprDa,FdObject);

FdAxialSprDa::FdAxialSprDa(FmIsControlledBase* pt) : FdObject()
{
  Fmd_CONSTRUCTOR_INIT(FdAxialSprDa);

  itsFmOwner = pt;

  itsKit = new FdSprDaTransformKit;
  itsKit->ref();

  FdBackPointer* bp_pointer = SO_GET_PART(itsKit,"backPt",FdBackPointer);
  bp_pointer->setPointer(this);
}


FdAxialSprDa::~FdAxialSprDa()
{
  this->fdDisconnect();
  itsKit->unref();
}


SoNodeKitListPart* FdAxialSprDa::getListSw() const
{
  return SO_GET_PART(FdDB::getMechanismKit(),"sprDaListSw",SoNodeKitListPart);
}


bool FdAxialSprDa::updateFdTopology(bool)
{
  // Set slave transform connection

  FmTriad* triad1 = NULL;
  FmTriad* triad2 = NULL;
  if (itsFmOwner->isOfType(FmAxialSpring::getClassTypeID()))
    {
      triad1 = ((FmAxialSpring*)itsFmOwner)->getFirstTriad();
      triad2 = ((FmAxialSpring*)itsFmOwner)->getSecondTriad();
    }
  else if (itsFmOwner->isOfType(FmAxialDamper::getClassTypeID()))
    {
      triad1 = ((FmAxialDamper*)itsFmOwner)->getFirstTriad();
      triad2 = ((FmAxialDamper*)itsFmOwner)->getSecondTriad();
    }

  if ( !(triad1 && triad2) ) return false;

  // First Triad:

  SoBaseKit* triadKit = triad1->getFdPointer()->getKit();
  SoTransform* transLink = SO_GET_PART(triadKit,"firstTrans",SoTransform);
  SoTransform* transLocal = SO_GET_PART(triadKit,"secondTrans",SoTransform);

  // Connecting slave transformations to connection symbol
  ((FdSprDaTransformKit*)itsKit)->connectFirstSpace(transLink,transLocal);

  // Second Triad:

  triadKit = triad2->getFdPointer()->getKit();
  transLink = SO_GET_PART(triadKit,"firstTrans",SoTransform);
  transLocal = SO_GET_PART(triadKit,"secondTrans",SoTransform);

  // Connecting slave transformations to connection symbol
  ((FdSprDaTransformKit*)itsKit)->connectSecondSpace(transLink,transLocal);

  return true;
}


bool FdAxialSprDa::updateFdApperance()
{
  // This test makes sure we do not unhighlight something
  // when it is supposed to be highlighted
  if (this->highlightRefCount > 0) return true;

  FmTriad* triad1 = NULL;
  FmTriad* triad2 = NULL;
  if (itsFmOwner->isOfType(FmAxialSpring::getClassTypeID()))
    {
      triad1 = ((FmAxialSpring*)itsFmOwner)->getFirstTriad();
      triad2 = ((FmAxialSpring*)itsFmOwner)->getSecondTriad();
    }
  else if (itsFmOwner->isOfType(FmAxialDamper::getClassTypeID()))
    {
      triad1 = ((FmAxialDamper*)itsFmOwner)->getFirstTriad();
      triad2 = ((FmAxialDamper*)itsFmOwner)->getSecondTriad();
    }

  if (!triad1 || !triad2) return false;

  // Setting color according to attachment
  if (triad1->isAttached() && triad2->isAttached())
    itsKit->setPart("appearance.material",FdSymbolDefs::getMaterial(FdSymbolDefs::SPR_DMP));
  else
    itsKit->setPart("appearance.material",FdSymbolDefs::getMaterial());

  return true;
}


bool FdAxialSprDa::updateFdDetails()
{
  // Set the symbols
  if (itsFmOwner->isOfType(FmAxialSpring::getClassTypeID()))
    itsKit->setPart("symbol",FdSymbolDefs::getSymbol(FdSymbolDefs::AXIAL_SPRING));
  else if (itsFmOwner->isOfType(FmAxialDamper::getClassTypeID()))
    itsKit->setPart("symbol",FdSymbolDefs::getSymbol(FdSymbolDefs::AXIAL_DAMPER));
  else
    return false;

  return true;
}


FmTriad* FdAxialSprDa::findClosestTriad(const SbVec3f& pointOnObject)
{
  FmTriad* triad1 = NULL;
  FmTriad* triad2 = NULL;
  if (itsFmOwner->isOfType(FmAxialSpring::getClassTypeID()))
    {
      triad1 = ((FmAxialSpring*)itsFmOwner)->getFirstTriad();
      triad2 = ((FmAxialSpring*)itsFmOwner)->getSecondTriad();
    }
  else if (itsFmOwner->isOfType(FmAxialDamper::getClassTypeID()))
    {
      triad1 = ((FmAxialDamper*)itsFmOwner)->getFirstTriad();
      triad2 = ((FmAxialDamper*)itsFmOwner)->getSecondTriad();
    }

  return pointOnObject[0] < 0.5 ? triad1 : triad2;
}
