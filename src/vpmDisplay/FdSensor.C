// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdSensor.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdSprDaTransformKit.H"
#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDB/FmSimpleSensor.H"
#include "vpmDB/FmRelativeSensor.H"
#include "vpmDB/FmAxialSpring.H"
#include "vpmDB/FmAxialDamper.H"
#include "vpmDB/FmJointBase.H"
#include "vpmDB/FmTriad.H"

#include <Inventor/nodes/SoMaterial.h>


/**********************************************************************
 *
 * CLASS FdSensor
 *
 **********************************************************************/

Fmd_SOURCE_INIT(FDSENSOR,FdSensor,FdObject);

FdSensor::FdSensor(FmSensorBase* pt) : FdObject()
{
  Fmd_CONSTRUCTOR_INIT(FdSensor);

  itsFmOwner = pt;

  itsKit = new FdSprDaTransformKit;
  itsKit->ref();

  FdBackPointer* bp_pointer = SO_GET_PART(itsKit,"backPt",FdBackPointer);
  bp_pointer->setPointer(this);
}


FdSensor::~FdSensor()
{
  this->fdDisconnect();
  itsKit->unref();
}


SoNodeKitListPart* FdSensor::getListSw() const
{
  return SO_GET_PART(FdDB::getMechanismKit(),"sensorListSw",SoNodeKitListPart);
}


bool FdSensor::updateFdApperance()
{
  if (((FmSensorBase*)itsFmOwner)->isDrawable())
    if (this->highlightRefCount < 1) // This test makes sure we do not
      // un-highlight something when it is supposed to be highlighted
      itsKit->setPart("appearance.material",FdSymbolDefs::getSensorMaterial());

  return true;
}


bool FdSensor::updateFdDetails()
{
  // Set the symbol

  if (((FmSensorBase*)itsFmOwner)->isDrawable())
    itsKit->setPart("symbol",FdSymbolDefs::getSymbol(FdSymbolDefs::SENSOR));

  return true;
}


bool FdSensor::updateFdTopology(bool)
{
  if (!((FmSensorBase*)itsFmOwner)->isDrawable())
    return true; // Not drawable

  // Get triads to place sensor between

  std::vector<FmTriad*> triads;
  triads.reserve(2);
  if (itsFmOwner->isOfType(FmSimpleSensor::getClassTypeID()))
    {
      FmIsMeasuredBase* measured = ((FmSimpleSensor*)itsFmOwner)->getMeasured();
      if (measured) {
	if (measured->isOfType(FmJointBase::getClassTypeID()))
	  {
	    triads.push_back(((FmJointBase*)measured)->getSlaveTriad());
	    triads.push_back(((FmJointBase*)measured)->getSlaveTriad());
	  }
	else if (measured->isOfType(FmAxialSpring::getClassTypeID()))
	  {
	    triads.push_back(((FmAxialSpring*)measured)->getFirstTriad());
	    triads.push_back(((FmAxialSpring*)measured)->getSecondTriad());
	  }
	else if (measured->isOfType(FmAxialDamper::getClassTypeID()))
	  {
	    triads.push_back(((FmAxialDamper*)measured)->getFirstTriad());
	    triads.push_back(((FmAxialDamper*)measured)->getSecondTriad());
	  }
	else if (measured->isOfType(FmTriad::getClassTypeID()))
	  {
	    triads.push_back((FmTriad*)measured);
	    triads.push_back((FmTriad*)measured);
	  }
      }
    }
  else if (itsFmOwner->isOfType(FmRelativeSensor::getClassTypeID()))
    {
      std::vector<FmIsMeasuredBase*> measured;
      ((FmRelativeSensor*)itsFmOwner)->getMeasured(measured);
      for (FmIsMeasuredBase* obj : measured)
        triads.push_back(dynamic_cast<FmTriad*>(obj));
    }

  // Setting up connections:

  if (triads.size() != 2 || !triads.front() || !triads.back())
    return false;

  // First Triad:

  SoBaseKit* triadKit = triads[0]->getFdPointer()->getKit();
  SoTransform* transLink = SO_GET_PART(triadKit,"firstTrans",SoTransform);
  SoTransform* transLocal = SO_GET_PART(triadKit,"secondTrans",SoTransform);

  // Connecting slave transformations to connection symbol
  ((FdSprDaTransformKit*)itsKit)->connectFirstSpace(transLink,transLocal);

  // Second Triad:

  triadKit = triads[1]->getFdPointer()->getKit();
  transLink = SO_GET_PART(triadKit,"firstTrans",SoTransform);
  transLocal = SO_GET_PART(triadKit,"secondTrans",SoTransform);

  // Connecting slave transformations to connection symbol
  ((FdSprDaTransformKit*)itsKit)->connectSecondSpace(transLink,transLocal);

  return true;
}


FaVec3 FdSensor::findSnapPoint(const SbVec3f& pointOnObject,
                               const SbMatrix& objToWorld,
                               SoDetail*, SoPickedPoint*)
{
  if (((FmSensorBase*)itsFmOwner)->isDrawable())
    return this->FdObject::findSnapPoint(pointOnObject,objToWorld);

  return FaVec3();
}
