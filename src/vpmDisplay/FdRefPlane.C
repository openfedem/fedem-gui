// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdRefPlane.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdExtraGraphics.H"
#include "vpmDisplay/FdRefPlaneKit.H"
#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDB/FmRefPlane.H"

#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoDrawStyle.h>


/**********************************************************************
 *
 * CLASS FdRefPlane
 *
 **********************************************************************/

Fmd_SOURCE_INIT(FDREFPLANE,FdRefPlane,FdObject);


FdRefPlane::FdRefPlane(FmRefPlane* pt) : FdObject()
{
  Fmd_CONSTRUCTOR_INIT(FdRefPlane);

  itsFmOwner = pt;

  itsKit = new FdRefPlaneKit;
  itsKit->ref();

  SO_GET_PART(itsKit,"backPt",FdBackPointer)->setPointer(this);

  this->highlightBoxId = NULL;
}


FdRefPlane::~FdRefPlane()
{
  this->fdDisconnect();
  itsKit->unref();
}


SoNodeKitListPart* FdRefPlane::getListSw() const
{
  return SO_GET_PART(FdDB::getMechanismKit(),"refPlaneListSw",SoNodeKitListPart);
}


bool FdRefPlane::updateFdTopology(bool)
{
  return this->updateFdCS();
}


bool FdRefPlane::updateFdApperance()
{
  FmRefPlane* refPlane = static_cast<FmRefPlane*>(itsFmOwner);
  SbVec3f refpColor(FdConverter::toSbVec3f(refPlane->getRGBColor()));

  std::array<SoMaterial*,3> mats = {
    SO_GET_PART(itsKit, "planeMaterial", SoMaterial),
    SO_GET_PART(itsKit, "lineMaterial", SoMaterial),
    SO_GET_PART(itsKit, "csAppearance.material", SoMaterial)
  };
  for (SoMaterial* material : mats) {
    material->diffuseColor.setValue(refpColor);
    material->ambientColor.setValue(refpColor);
    material->specularColor.setValue(refpColor);
    material->emissiveColor.setValue(SbVec3f(0.0f,0.0f,0.0f));
    material->shininess = 0.9;
  }
  mats.front()->transparency.setValue(refPlane->getTransparency());

  SO_GET_PART(itsKit,"frameStyle",SoDrawStyle)->lineWidth.setValue(0);
  SO_GET_PART(itsKit,"csStyle",SoDrawStyle)->lineWidth.setValue(0);

  return true;
}


bool FdRefPlane::updateFdCS()
{
  SoTransform* transform = SO_GET_PART(itsKit,"planeTransform",SoTransform);
  transform->setMatrix(FdConverter::toSbMatrix(static_cast<FmRefPlane*>(itsFmOwner)->getLocalCS()));
  return true;
}


bool FdRefPlane::updateFdDetails()
{
  // Make the refplane:

  float x = static_cast<FmRefPlane*>(itsFmOwner)->getWidth();
  float y = static_cast<FmRefPlane*>(itsFmOwner)->getHeight();

  // Setting plane size and shape:

  SO_GET_PART(itsKit,"planeSize",SoScale)->scaleFactor.setValue(x,y,1);

  SoCoordinate3* coords = SO_GET_PART(itsKit,"planeCoords",SoCoordinate3);
  coords->point.set1Value(0,-0.5,-0.5,0);
  coords->point.set1Value(1,-0.5,0.5,0);
  coords->point.set1Value(2,0.5,0.5,0);
  coords->point.set1Value(3,0.5,-0.5,0);

  SoIndexedFaceSet* plane = SO_GET_PART(itsKit,"planeShape",SoIndexedFaceSet);
  plane->coordIndex.set1Value(0,0);
  plane->coordIndex.set1Value(1,1);
  plane->coordIndex.set1Value(2,2);
  plane->coordIndex.set1Value(3,3);
  plane->coordIndex.set1Value(4,SO_END_FACE_INDEX);

  SoIndexedLineSet* frame = SO_GET_PART(itsKit,"frameShape",SoIndexedLineSet);
  frame->coordIndex.set1Value(0,0);
  frame->coordIndex.set1Value(1,1);
  frame->coordIndex.set1Value(2,2);
  frame->coordIndex.set1Value(3,3);
  frame->coordIndex.set1Value(4,0);
  frame->coordIndex.set1Value(5,SO_END_FACE_INDEX);

  // Setting cs arrow symbol:

  SO_GET_PART(itsKit,"csTranslation",SoTranslation)->translation.setValue(-0.45f*x,-0.45f*y,0);

  itsKit->setPart("csSymbol",FdSymbolDefs::getSymbol(FdSymbolDefs::REFCS));
  itsKit->setPart("csSymbol.scale",NULL);
  SO_GET_PART(itsKit,"csSymbol.scale",SoTransform)->scaleFactor.setValue(0.1f*x,0.1f*y,1);

  return true;
}


void FdRefPlane::showHighlight()
{
  if (!FdDB::isUsingLineHighlight())
  {
    if (this->highlightBoxId)
      FdExtraGraphics::removeBBox(this->highlightBoxId);
    this->highlightBoxId = FdExtraGraphics::showBBox(this->itsKit);
  }
  else
  {
    std::array<SoMaterial*,3> mats = {
      SO_GET_PART(itsKit, "planeMaterial", SoMaterial),
      SO_GET_PART(itsKit, "lineMaterial", SoMaterial),
      SO_GET_PART(itsKit, "csAppearance.material", SoMaterial)
    };
    // Set red highlight color
    for (SoMaterial* mat : mats) {
      mat->diffuseColor.setValue(1.0f,0.0f,0.0f);
      mat->ambientColor.setValue(1.0f,0.0f,0.0f);
      mat->emissiveColor.setValue(1.0f,0.0f,0.0f);
    }
  }
}


void FdRefPlane::hideHighlight()
{
  if (this->highlightBoxId)
    FdExtraGraphics::removeBBox(this->highlightBoxId);

  this->highlightBoxId = NULL;
  this->updateFdApperance();
}
