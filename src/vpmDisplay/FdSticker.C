// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdSticker.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdTransformKit.H"
#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDB/FmSticker.H"

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>


/**********************************************************************
 *
 * CLASS FdSticker
 *
 **********************************************************************/


Fmd_SOURCE_INIT(FDSTICKER,FdSticker,FdObject);


FdSticker::FdSticker(FmSticker* pt) : FdObject()
{
  Fmd_CONSTRUCTOR_INIT(FdSticker);

  itsFmOwner = pt;

  itsKit = new FdTransformKit;
  itsKit->ref();

  // Set up back pointer
  FdBackPointer* bp_pointer = SO_GET_PART(itsKit,"backPt",FdBackPointer);
  bp_pointer->setPointer(this);
}


FdSticker::~FdSticker()
{
  this->fdDisconnect();
  itsKit->unref();
}


SoNodeKitListPart* FdSticker::getListSw() const
{
  return SO_GET_PART(FdDB::getMechanismKit(),"stickerListSw",SoNodeKitListPart);
}


bool FdSticker::updateFdTopology(bool)
{ 
  return this->updateFdCS();
}


bool FdSticker::updateFdDetails()
{ 
  // Set the symbol
  itsKit->setPart("symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::STICKER));
  return true;
}


bool FdSticker::updateFdApperance()
{
  // This test makes sure we do not unhighlight something
  // when it is supposed to be highlighted
  if (this->highlightRefCount > 0) return true;

  itsKit->setPart("appearance.material", FdSymbolDefs::getMaterial(FdSymbolDefs::STICK));
  return true;
}


bool FdSticker::updateFdCS()
{
  SoTransform* transLocal = SO_GET_PART(itsKit,"secondTrans",SoTransform);
  transLocal->translation.setValue(FdConverter::toSbVec3f(((FmSticker*)itsFmOwner)->getPoint()));

  return true;
}  


FaVec3 FdSticker::findSnapPoint(const SbVec3f&, const SbMatrix&, SoDetail*, SoPickedPoint*)
{
  return ((FmSticker*)itsFmOwner)->getPoint();
}
