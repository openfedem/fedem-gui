// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdLoad.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdLoadTransformKit.H"
#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDB/FmLoad.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmLink.H"

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>


/**********************************************************************
 *
 * CLASS FdLoad
 *
 **********************************************************************/

Fmd_SOURCE_INIT(FDLOAD,FdLoad,FdObject);


FdLoad::FdLoad(FmLoad* pt) : FdObject()
{
  Fmd_CONSTRUCTOR_INIT(FdLoad);

  itsFmOwner = pt;

  itsKit = new FdLoadTransformKit;
  itsKit->ref();

  // Set up back pointer
  FdBackPointer* bp_pointer = SO_GET_PART(itsKit,"backPt",FdBackPointer);
  bp_pointer->setPointer(this);
}


FdLoad::~FdLoad()
{
  fdDisconnect();
  itsKit->unref();
}


SoNodeKitListPart* FdLoad::getListSw() const
{
  return SO_GET_PART(FdDB::getMechanismKit(),"loadListSw",SoNodeKitListPart);
}


bool FdLoad::updateFdApperance()
{
  if (this->highlightRefCount < 1) // This test makes sure we do not
    // un-highlight something when it is supposed to be highlighted
    if (((FmLoad*)itsFmOwner)->getOwnerTriad()) {
      if (((FmLoad*)itsFmOwner)->getOwnerTriad()->isAttached())
        itsKit->setPart("appearance.material",FdSymbolDefs::getMaterial(FdSymbolDefs::LOAD));
      else
        itsKit->setPart("appearance.material",FdSymbolDefs::getMaterial());
    }

  return true;
}


bool FdLoad::updateFdDetails()
{
  // Set the symbols

  if (((FmLoad*)itsFmOwner)->getLoadType()==FmLoad::FORCE)
    itsKit->setPart("symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::FORCE));
  else if (((FmLoad*)itsFmOwner)->getLoadType()==FmLoad::TORQUE)
    itsKit->setPart("symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::TORQUE));

  return true;
}


bool FdLoad::updateFdTopology(bool)
{
  SoTransform* transLocal;
  SoTransform* transLink;
  FmIsPositionedBase* ref;
  FaVec3 point;

  //
  // Set up connection for ATTACK - Point
  //
  ///////////////////////////////////////

  FmTriad* ownerTriad = ((FmLoad*)itsFmOwner)->getOwnerTriad();
  if (ownerTriad)
  {
    transLink = SO_GET_PART(ownerTriad->getFdPointer()->getKit(),"firstTrans",SoTransform);
    transLocal = SO_GET_PART(ownerTriad->getFdPointer()->getKit(),"secondTrans",SoTransform);
    ((FdLoadTransformKit*)itsKit)->attackCoord.connectFrom(&transLocal->translation);
    ((FdLoadTransformKit*)itsKit)->connectAttackSpace(transLink,NULL);
  }
  else
    // Load has no owner triad.
    // Disconnect and do not move:
    ((FdLoadTransformKit*)itsKit)->attackCoord.disconnect();


  //
  // Set up connection for FROM - Point
  //
  ///////////////////////////////////

  point = ((FmLoad*)itsFmOwner)->getLocalFromPoint();
  ((FdLoadTransformKit*)itsKit)->firstCoord.setValue(FdConverter::toSbVec3f(point));

  ref = ((FmLoad*)itsFmOwner)->getFromRef();
  if (ref)
    {
      if (ref->isOfType(FmLink::getClassTypeID()) && ref->getFdPointer())
	{
	  transLink = SO_GET_PART(ref->getFdPointer()->getKit(),"transform",SoTransform);
	  transLocal = NULL;
	  ((FdLoadTransformKit*)itsKit)->connectFirstSpace(transLink,transLocal);
	}
      else if (ref->isOfType(FmTriad::getClassTypeID()))
	{
	  transLink = SO_GET_PART(ref->getFdPointer()->getKit(),"firstTrans",SoTransform);
	  transLocal = SO_GET_PART(ref->getFdPointer()->getKit(),"secondTrans",SoTransform);
	 ((FdLoadTransformKit*)itsKit)->connectFirstSpace(transLink,transLocal);
	}
      else
        ((FdLoadTransformKit*)itsKit)->disConnectFirstSpace();
    }
  else
    // No reference item for this point, assume global reference.
    // Disconnect and do not move:
    ((FdLoadTransformKit*)itsKit)->disConnectFirstSpace();


  //
  // Set up connection for TO - Point
  //
  ///////////////////////////////////

  point = ((FmLoad*)itsFmOwner)->getLocalToPoint();
  ((FdLoadTransformKit*)itsKit)->secondCoord.setValue(FdConverter::toSbVec3f(point));

  ref = ((FmLoad*)itsFmOwner)->getToRef();
  if (ref)
    {
      if (ref->isOfType(FmLink::getClassTypeID()) && ref->getFdPointer())
	{
	  transLink = SO_GET_PART(ref->getFdPointer()->getKit(),"transform",SoTransform);
	  transLocal = NULL;
	  ((FdLoadTransformKit*)itsKit)->connectSecondSpace(transLink,transLocal);
	}
      else if (ref->isOfType(FmTriad::getClassTypeID()))
	{
	  transLink = SO_GET_PART(ref->getFdPointer()->getKit(),"firstTrans",SoTransform);
	  transLocal = SO_GET_PART(ref->getFdPointer()->getKit(),"secondTrans",SoTransform);
	  ((FdLoadTransformKit*)itsKit)->connectSecondSpace(transLink,transLocal);
	}
      else
        ((FdLoadTransformKit*)itsKit)->disConnectSecondSpace();
    }
  else
    // No reference item for this point, assume global reference.
    // Disconnect and do not move:
    ((FdLoadTransformKit*)itsKit)->disConnectSecondSpace();

  return true;
}
