// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdBase.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdConverter.H"

#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodes/SoMaterial.h>
#ifdef USE_SMALLCHANGE
#include <SmallChange/nodes/SmDepthBuffer.h>
#endif


Fmd_SOURCE_INIT(FDOBJECT,FdObject,FdObject);


FdObject::FdObject() : itsFmOwner(NULL), itsKit(NULL)
{
  Fmd_CONSTRUCTOR_INIT(FdObject);

  isInserted = false;
  highlightRefCount = selectRefCount = 0;
}


bool FdObject::fdErase()
{
  delete this;
  return true;
}


bool FdObject::fdDisconnect()
{
  if (!isInserted) return true;

  // Fjerner dette objektet fra IV grafen. Burde kanskje rydde opp
  // rekursivt i transformasjonskoblingene til de avhengige symbolene.
  // Foreloebig overlates det til FmDB aa kalle updateFdTopology().

  itsKit->ref();
  this->getListSw()->removeChild(itsKit);
  itsKit->unrefNoDelete();

  isInserted = false;
  return true;
}


bool FdObject::updateFdAll(bool updateChildrenDisplay)
{
  if (!isInserted) {
    this->getListSw()->addChild(itsKit);
    isInserted = true;
  }
  this->updateFdTopology(updateChildrenDisplay);
  this->updateFdDetails();
  this->updateFdApperance();

  return true;
}


bool FdObject::updateFdHighlight()
{
  if (highlightRefCount)
    this->showHighlight();
  else
    this->hideHighlight();

  return highlightRefCount > 0;
}


void FdObject::highlight(bool onOrOff)
{
  if (onOrOff) {
    if (++highlightRefCount == 1)
      this->showHighlight();
  }
  else {
    if (highlightRefCount > 0)
      --highlightRefCount;
    if (highlightRefCount == 0)
      this->hideHighlight();
  }
}


void FdObject::showHighlight()
{
  itsKit->setPart("appearance.material",FdSymbolDefs::getHighlightMaterial());
#ifdef USE_SMALLCHANGE
  itsKit->setPart("appearance.depth",FdSymbolDefs::getHighlightDepthBMod());
#endif
}


void FdObject::hideHighlight()
{
  this->updateFdApperance();
#ifdef USE_SMALLCHANGE
  itsKit->setPart("appearance.depth",NULL);
#endif
}


FaVec3 FdObject::findSnapPoint(const SbVec3f& pointOnObject,
                               const SbMatrix& objToWorld,
                               SoDetail*, SoPickedPoint*)
{
  SbVec3f nearestWorld;
  objToWorld.multVecMatrix(pointOnObject,nearestWorld);
  return FdConverter::toFaVec3(nearestWorld);
}


bool FdObject::isParallell(const SbVec3f& v1, const SbVec3f& v2, float tolerance)
{
  if (v1.length() <= tolerance || v2.length() <= tolerance)
    return false;

  SbVec3f tmpVec1(v1);
  SbVec3f tmpVec2(v2);
  tmpVec1.normalize();
  tmpVec2.normalize();

  return fabsf(tmpVec1.dot(tmpVec2)) > 1.0f - tolerance;
}
