// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdCtrlObject.H"
#include "vpmDisplay/FdCtrlLineKit.H"
#include "vpmDisplay/FdCtrlSymDef.H"
#include "vpmDisplay/FdCtrlDB.H"
#include "vpmDB/FmIsRenderedBase.H"

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedLineSet.h>


FdCtrlLineKit* FdCtrlObject::fbLineNode = NULL;

Fmd_SOURCE_INIT(FDCTRLOBJECT,FdCtrlObject,FdObject);


FdCtrlObject::FdCtrlObject()
{
  Fmd_CONSTRUCTOR_INIT(FdCtrlObject);
}


bool FdCtrlObject::updateFdApperance()
{
  this->hideHighlight();
  return true;
}


void FdCtrlObject::showHighlight()
{
  itsKit->setPart("appearance.material",FdCtrlSymDef::getHighlightMaterial());
}

void FdCtrlObject::hideHighlight()
{
  itsKit->setPart("appearance.material", FdCtrlSymDef::getDefaultMaterial());
  SoDrawStyle* drawStyle = SO_GET_PART(itsKit,"appearance.style",SoDrawStyle);
  drawStyle->lineWidth.setValue(2.0);
}


void FdCtrlObject::erase()
{
  itsFmOwner->erase();
}


FaVec3 FdCtrlObject::findSnapPoint(const SbVec3f&, const SbMatrix&,
                                   SoDetail*, SoPickedPoint*)
{
  return FaVec3();
}


void FdCtrlObject::removeFeedbackLine()
{
  if (FdCtrlObject::fbLineNode)
  {
    FdCtrlObject::fbLineNode->unref();
    FdCtrlDB::getCtrlExtraGraphicsRoot()->removeChild(FdCtrlObject::fbLineNode);
    FdCtrlObject::fbLineNode = NULL;
  }
}


void FdCtrlObject::drawFbLine(const SbVec3f& startVec, const SbVec3f& endVec)
{
  removeFeedbackLine();

  fbLineNode = new FdCtrlLineKit;
  fbLineNode->ref();
  FdCtrlDB::getCtrlExtraGraphicsRoot()->addChild(fbLineNode);
  fbLineNode->setPart("appearance.material",FdCtrlSymDef::getDefaultMaterial());

  float CoordsVal[2][3];

  CoordsVal[0][0] = startVec[0];
  CoordsVal[0][1] = startVec[1];
  CoordsVal[0][2] = 0.0f;

  CoordsVal[1][0] = endVec[0];
  CoordsVal[1][1] = endVec[1];
  CoordsVal[1][2] = 0.0f;

  static int32_t index[3] = {0,1,-1};

  SoCoordinate3* fdCoords = SO_GET_PART(fbLineNode,"symbol.coords",SoCoordinate3);
  fdCoords->point.setValues(0,2,CoordsVal);

  SoIndexedLineSet* fdLine = SO_GET_PART(fbLineNode,"symbol.nonAxis",SoIndexedLineSet);
  fdLine->coordIndex.setValues(0,3,index);
}
