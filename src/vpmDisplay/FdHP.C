// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdHP.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdSprDaTransformKit.H"
#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdDB.H"

#include "vpmDB/FmHPBase.H"

#include <Inventor/nodes/SoMaterial.h>


/**********************************************************************
 *
 * CLASS FdHP
 *
 **********************************************************************/


Fmd_SOURCE_INIT(FDHP,FdHP,FdObject);


FdHP::FdHP(FmHPBase* pt) : FdObject()
{
  Fmd_CONSTRUCTOR_INIT(FdHP);

  itsFmOwner = pt;

  itsKit = new FdSprDaTransformKit;
  itsKit->ref();

  // Set up back pointer
  FdBackPointer* bp_pointer = SO_GET_PART(itsKit,"backPt",FdBackPointer);
  bp_pointer->setPointer(this);
}


FdHP::~FdHP()
{
  this->fdDisconnect();
  itsKit->unref();
}


SoNodeKitListPart* FdHP::getListSw() const
{
  return SO_GET_PART(FdDB::getMechanismKit(),"HPListSw",SoNodeKitListPart);
}


bool FdHP::updateFdTopology(bool)
{
  SoTransform* transLocal;
  SoTransform* transLink;

  // Get the first owner joint
  FmJointBase* joint = ((FmHPBase*)itsFmOwner)->getInputJoint();

  if (joint)
    {
      // First Joint:
      transLink = SO_GET_PART(joint->getFdPointer()->getKit(),"slave.firstTrans",SoTransform);
      transLocal = SO_GET_PART(joint->getFdPointer()->getKit(),"slave.secondTrans",SoTransform);

      // Connect slave transformations to connection symbol engine
      ((FdSprDaTransformKit*)itsKit)->connectFirstSpace(transLink, transLocal);
    }
  else
    {
      // HPs should always have two joints
      std::cout << itsFmOwner->getIdString(true) <<" is not connected to two joints."<< std::endl;
      return false;
    }

  // Get the second owner-joint
  joint = ((FmHPBase*)itsFmOwner)->getOutputJoint();
  if (joint)
    {
      // Second Joint:
      transLink = SO_GET_PART(joint->getFdPointer()->getKit(),"slave.firstTrans",SoTransform);
      transLocal =  SO_GET_PART(joint->getFdPointer()->getKit(),"slave.secondTrans",SoTransform);

      // Connecting transformations to connection symbol engine
      ((FdSprDaTransformKit*)itsKit)->connectSecondSpace(transLink, transLocal);
    }
  else
    {
      // HPs should always have two joints
      std::cout << itsFmOwner->getIdString(true) <<" is not connected to two joints."<< std::endl;
      return false;
    }

  return true;
}


bool FdHP::updateFdApperance()
{
  // This test makes sure we do not unhighlight something
  // when it is supposed to be highlighted
  if (this->highlightRefCount > 0) return true;

  FmJointBase* inputJoint  = ((FmHPBase*)itsFmOwner)->getInputJoint();
  if (!inputJoint) return false;

  FmJointBase* outputJoint = ((FmHPBase*)itsFmOwner)->getOutputJoint();
  if (!outputJoint) return false;

  if (inputJoint->isAttachedToLink() && outputJoint->isAttachedToLink())
    itsKit->setPart("appearance.material",FdSymbolDefs::getMaterial(FdSymbolDefs::HP));
  else
    itsKit->setPart("appearance.material",FdSymbolDefs::getMaterial());

  return true;
}


bool FdHP::updateFdDetails()
{
  itsKit->setPart("symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::HIGHER_PAIR));
  return true;
}
