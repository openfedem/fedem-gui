// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdSeaState.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdExtraGraphics.H"
#include "vpmDisplay/FdSeaStateKit.H"
#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDisplay/FdAnimateModel.H"

#include "vpmDB/FmSeaState.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmGlobalViewSettings.H"
#include "vpmApp/vpmAppCmds/FapAnimationCmds.H"

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoDrawStyle.h>


Fmd_SOURCE_INIT(FDSEASTATE,FdSeaState,FdObject);


FdSeaState::FdSeaState(FmSeaState* pt)
{
  Fmd_CONSTRUCTOR_INIT(FdSeaState);

  itsFmOwner = pt;

  itsKit = new FdSeaStateKit;
  itsKit->ref();

  SO_GET_PART(itsKit,"backPt",FdBackPointer)->setPointer(this);

  this->highlightBoxId = NULL;
}


FdSeaState::~FdSeaState()
{
  this->fdDisconnect();
  itsKit->unref();
}


SoNodeKitListPart* FdSeaState::getListSw() const
{
  return SO_GET_PART(FdDB::getMechanismKit(),"seaStateListSw",SoNodeKitListPart);
}


bool FdSeaState::updateFdApperance()
{
  SoShapeHints* shapeHint = SO_GET_PART(itsKit,"shapeHint",SoShapeHints);
  shapeHint->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
  shapeHint->faceType = SoShapeHints::UNKNOWN_FACE_TYPE;

  FmSeaState* seaState = static_cast<FmSeaState*>(itsFmOwner);
  SbVec3f seaColor(FdConverter::toSbVec3f(seaState->getRGBColor()));

  std::array<SoMaterial*,3> mats = {
    SO_GET_PART(itsKit, "planeMaterial", SoMaterial),
    SO_GET_PART(itsKit, "lineMaterial", SoMaterial),
    SO_GET_PART(itsKit, "csAppearance.material", SoMaterial)
  };
  for (SoMaterial* material : mats) {
    material->diffuseColor.setValue(seaColor);
    material->ambientColor.setValue(seaColor);
    material->specularColor.setValue(seaColor);
    material->emissiveColor.setValue(SbVec3f(0.0f,0.0f,0.0f));
    material->shininess = 0.9;
  }
  mats.front()->transparency.setValue(seaState->getTransparency());

  SO_GET_PART(itsKit,"frameStyle",SoDrawStyle)->lineWidth.setValue(0);
  SO_GET_PART(itsKit,"csStyle",SoDrawStyle)->lineWidth.setValue(0);

  return true;
}


bool FdSeaState::updateFdCS()
{
  SoTransform* transform = SO_GET_PART(itsKit,"planeTransform",SoTransform);
  transform->setMatrix(FdConverter::toSbMatrix(static_cast<FmSeaState*>(itsFmOwner)->getLocalCS()));

  return true;
}


bool FdSeaState::updateFdDetails()
{
  // Clean up old visualization
  SoSeparator* topPlane = SO_GET_PART(itsKit,"topPlaneSep",SoSeparator);
  topPlane->removeAllChildren();

  SoIndexedFaceSet* plane = SO_GET_PART(itsKit,"planeShape",SoIndexedFaceSet);
  plane->coordIndex.deleteValues(0,plane->coordIndex.getNum());
  SoIndexedFaceSet* plane2 = SO_GET_PART(itsKit,"planeShape2",SoIndexedFaceSet);
  plane2->coordIndex.deleteValues(0,plane2->coordIndex.getNum());
  SoIndexedFaceSet* plane3 = SO_GET_PART(itsKit,"planeShape3",SoIndexedFaceSet);
  plane3->coordIndex.deleteValues(0,plane3->coordIndex.getNum());
  SoIndexedFaceSet* plane4 = SO_GET_PART(itsKit,"planeShape4",SoIndexedFaceSet);
  plane4->coordIndex.deleteValues(0,plane4->coordIndex.getNum());
  SoIndexedFaceSet* plane5 = SO_GET_PART(itsKit,"planeShape5",SoIndexedFaceSet);
  plane5->coordIndex.deleteValues(0,plane5->coordIndex.getNum());
  SoIndexedFaceSet* plane6 = SO_GET_PART(itsKit,"planeShape6",SoIndexedFaceSet);
  plane6->coordIndex.deleteValues(0,plane6->coordIndex.getNum());
  SoIndexedLineSet* frame = SO_GET_PART(itsKit,"frameShape",SoIndexedLineSet);
  frame->coordIndex.deleteValues(0,frame->coordIndex.getNum());

  SoSeparator* sep = SO_GET_PART(itsKit,"wireSep",SoSeparator);
  sep->removeAllChildren();

  FmSeaState* seaState = static_cast<FmSeaState*>(itsFmOwner);
  bool finiteDepth = seaState->seaDepth.getValue() > 0.0;
  FmMathFuncBase* waveFunction = this->evaluateWave(seaState);
  if (waveFunction)
  {
    int i, j, num = seaState->getQuantization() > 2 ? seaState->getQuantization() : 2;
    int numY = waveFunction->isSurfaceFunc() ? num : 2;
    int num2 = num*numY; // total number of grid points

    //----- Create Frame -----

    for (i = j = 0; i < num; i++)
      frame->coordIndex.set1Value(j++,i);

    for (i = 2*num-1; i < num2-num; i += num)
      frame->coordIndex.set1Value(j++,i);

    for (i = 1; i <= num; i++)
      frame->coordIndex.set1Value(j++,num2-i);

    for (i = num; i < num2-num-1; i += num)
      frame->coordIndex.set1Value(j++,num2-i);

    frame->coordIndex.set1Value(j,0);

    if (finiteDepth)
    {
      SoIndexedLineSet* cornerLine1 = new SoIndexedLineSet;
      cornerLine1->coordIndex.set1Value(0,0);
      cornerLine1->coordIndex.set1Value(1,num2);
      cornerLine1->coordIndex.set1Value(2,SO_END_LINE_INDEX);
      SoIndexedLineSet* cornerLine2 = new SoIndexedLineSet;
      cornerLine2->coordIndex.set1Value(0,num-1);
      cornerLine2->coordIndex.set1Value(1,num2+1);
      cornerLine2->coordIndex.set1Value(2,SO_END_LINE_INDEX);
      SoIndexedLineSet* cornerLine3 = new SoIndexedLineSet;
      cornerLine3->coordIndex.set1Value(0,num2-1);
      cornerLine3->coordIndex.set1Value(1,num2+3);
      cornerLine3->coordIndex.set1Value(2,SO_END_LINE_INDEX);
      SoIndexedLineSet* cornerLine4 = new SoIndexedLineSet;
      cornerLine4->coordIndex.set1Value(0,num2-num);
      cornerLine4->coordIndex.set1Value(1,num2+2);
      cornerLine4->coordIndex.set1Value(2,SO_END_LINE_INDEX);

      SoIndexedLineSet* bottomLine1 = new SoIndexedLineSet;
      bottomLine1->coordIndex.set1Value(0,num2);
      bottomLine1->coordIndex.set1Value(1,num2+1);
      bottomLine1->coordIndex.set1Value(2,SO_END_LINE_INDEX);
      SoIndexedLineSet* bottomLine2 = new SoIndexedLineSet;
      bottomLine2->coordIndex.set1Value(0,num2+1);
      bottomLine2->coordIndex.set1Value(1,num2+3);
      bottomLine2->coordIndex.set1Value(2,SO_END_LINE_INDEX);
      SoIndexedLineSet* bottomLine3 = new SoIndexedLineSet;
      bottomLine3->coordIndex.set1Value(0,num2+3);
      bottomLine3->coordIndex.set1Value(1,num2+2);
      bottomLine3->coordIndex.set1Value(2,SO_END_LINE_INDEX);
      SoIndexedLineSet* bottomLine4 = new SoIndexedLineSet;
      bottomLine4->coordIndex.set1Value(0,num2+2);
      bottomLine4->coordIndex.set1Value(1,num2);
      bottomLine4->coordIndex.set1Value(2,SO_END_LINE_INDEX);

      sep->addChild(cornerLine1);
      sep->addChild(cornerLine2);
      sep->addChild(cornerLine3);
      sep->addChild(cornerLine4);

      sep->addChild(bottomLine1);
      sep->addChild(bottomLine2);
      sep->addChild(bottomLine3);
      sep->addChild(bottomLine4);
    }

    if (seaState->getShowGrid())
    {
      //----- Create sea surface grid -----

      for (i = 1; i < num-1; i++)
      {
	SoIndexedLineSet* edge = new SoIndexedLineSet;
	for (j = 0; j < numY; j++)
	  edge->coordIndex.set1Value(j,num*j+i);
	sep->addChild(edge);
      }

      for (j = 1; j < numY-1; j++)
      {
	SoIndexedLineSet* edge = new SoIndexedLineSet;
	for (i = 0; i < num; i++)
	  edge->coordIndex.set1Value(i,num*j+i);
	sep->addChild(edge);
      }
    }

    if (seaState->getShowSolid())
    {
      SoMaterial* material = SO_GET_PART(itsKit,"planeMaterial",SoMaterial);
      topPlane->addChild(material);

      // Create topology for the sea surface
      for (j = 0; j < numY-1; j++)
	for (i = 0; i < num-1; i++)
	{
	  SoIndexedFaceSet* strip = new SoIndexedFaceSet();
	  strip->coordIndex.set1Value(0,j*num+i);
	  strip->coordIndex.set1Value(1,j*num+i+1);
	  strip->coordIndex.set1Value(2,j*num+num+i+1);
	  strip->coordIndex.set1Value(3,j*num+num+i);
	  strip->coordIndex.set1Value(4,SO_END_FACE_INDEX);
	  topPlane->addChild(strip);
	}

      if (finiteDepth)
      {
	// Create topology for bottom plane
	plane2->coordIndex.set1Value(0,num2);
	plane2->coordIndex.set1Value(1,num2+1);
	plane2->coordIndex.set1Value(2,num2+3);
	plane2->coordIndex.set1Value(3,num2+2);
	plane2->coordIndex.set1Value(4,SO_END_FACE_INDEX);

	// Create topology for left plane
	for (i = 0; i < num; i++)
	  plane3->coordIndex.set1Value(i,num-1-i);
	plane3->coordIndex.set1Value(num,num2);
	plane3->coordIndex.set1Value(num+1,num2+1);
	plane3->coordIndex.set1Value(num+2,SO_END_FACE_INDEX);

	// Create topology for right plane
	for (i = 0; i < num; i++)
	  plane4->coordIndex.set1Value(i,num2-num+i);
	plane4->coordIndex.set1Value(i++,num2+3);
	plane4->coordIndex.set1Value(i++,num2+2);
	plane4->coordIndex.set1Value(i,SO_END_FACE_INDEX);

	// Create topology for front plane
	for (i = 0; i < numY; i++)
	  plane5->coordIndex.set1Value(i,num2-i*num-1);
	plane5->coordIndex.set1Value(i++,num2+1);
	plane5->coordIndex.set1Value(i++,num2+3);
	plane5->coordIndex.set1Value(i,SO_END_FACE_INDEX);
	
	// Create topology for back plane
	for (i = 0; i < numY; i++)
	  plane6->coordIndex.set1Value(i,num2-(numY-i)*num);
	plane6->coordIndex.set1Value(i++,num2+2);
	plane6->coordIndex.set1Value(i++,num2);
	plane6->coordIndex.set1Value(i,SO_END_FACE_INDEX);
      }
    }
  }
  else
  {
    // No wave function, create simple rectangle or box
    if (seaState->getShowSolid())
    {
      plane->coordIndex.set1Value(0,3);
      plane->coordIndex.set1Value(1,2);
      plane->coordIndex.set1Value(2,1);
      plane->coordIndex.set1Value(3,0);
      plane->coordIndex.set1Value(4,SO_END_FACE_INDEX);

      if (finiteDepth)
      {
	plane2->coordIndex.set1Value(0,7);
	plane2->coordIndex.set1Value(1,6);
	plane2->coordIndex.set1Value(2,5);
	plane2->coordIndex.set1Value(3,4);
	plane2->coordIndex.set1Value(4,SO_END_FACE_INDEX);

	plane3->coordIndex.set1Value(0,4);
	plane3->coordIndex.set1Value(1,7);
	plane3->coordIndex.set1Value(2,3);
	plane3->coordIndex.set1Value(3,0);
	plane3->coordIndex.set1Value(4,SO_END_FACE_INDEX);

	plane4->coordIndex.set1Value(0,7);
	plane4->coordIndex.set1Value(1,6);
	plane4->coordIndex.set1Value(2,2);
	plane4->coordIndex.set1Value(3,3);
	plane4->coordIndex.set1Value(4,SO_END_FACE_INDEX);

	plane5->coordIndex.set1Value(0,6);
	plane5->coordIndex.set1Value(1,5);
	plane5->coordIndex.set1Value(2,1);
	plane5->coordIndex.set1Value(3,2);
	plane5->coordIndex.set1Value(4,SO_END_FACE_INDEX);

	plane6->coordIndex.set1Value(0,5);
	plane6->coordIndex.set1Value(1,4);
	plane6->coordIndex.set1Value(2,0);
	plane6->coordIndex.set1Value(3,1);
	plane6->coordIndex.set1Value(4,SO_END_FACE_INDEX);
      }
    }

    frame->coordIndex.set1Value(0,0);
    frame->coordIndex.set1Value(1,1);
    frame->coordIndex.set1Value(2,2);
    frame->coordIndex.set1Value(3,3);
    frame->coordIndex.set1Value(4,0);
    frame->coordIndex.set1Value(5,SO_END_LINE_INDEX);

    if (finiteDepth)
    {
      SoIndexedLineSet* cornerLine1 = new SoIndexedLineSet;
      cornerLine1->coordIndex.set1Value(0,0);
      cornerLine1->coordIndex.set1Value(1,4);
      cornerLine1->coordIndex.set1Value(2,SO_END_LINE_INDEX);

      SoIndexedLineSet* cornerLine2 = new SoIndexedLineSet;
      cornerLine2->coordIndex.set1Value(0,1);
      cornerLine2->coordIndex.set1Value(1,5);
      cornerLine2->coordIndex.set1Value(2,SO_END_LINE_INDEX);

      SoIndexedLineSet* cornerLine3 = new SoIndexedLineSet;
      cornerLine3->coordIndex.set1Value(0,2);
      cornerLine3->coordIndex.set1Value(1,6);
      cornerLine3->coordIndex.set1Value(2,SO_END_LINE_INDEX);

      SoIndexedLineSet* cornerLine4 = new SoIndexedLineSet;
      cornerLine4->coordIndex.set1Value(0,3);
      cornerLine4->coordIndex.set1Value(1,7);
      cornerLine4->coordIndex.set1Value(2,SO_END_LINE_INDEX);

      SoIndexedLineSet* bottomFrame = new SoIndexedLineSet;
      bottomFrame->coordIndex.set1Value(0,7);
      bottomFrame->coordIndex.set1Value(1,6);
      bottomFrame->coordIndex.set1Value(2,5);
      bottomFrame->coordIndex.set1Value(3,4);
      bottomFrame->coordIndex.set1Value(4,7);
      bottomFrame->coordIndex.set1Value(5,SO_END_LINE_INDEX);

      topPlane->addChild(cornerLine1);
      topPlane->addChild(cornerLine2);
      topPlane->addChild(cornerLine3);
      topPlane->addChild(cornerLine4);
      topPlane->addChild(bottomFrame);
    }
  }

  return true;
}


void FdSeaState::showHighlight()
{
  if (!FdDB::usesLineHighlight)
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

void FdSeaState::hideHighlight()
{
  if (this->highlightBoxId)
    FdExtraGraphics::removeBBox(this->highlightBoxId);

  this->highlightBoxId = NULL;
  this->updateFdApperance();
}


void FdSeaState::selectAnimationFrame(size_t frameNr)
{
  this->evaluateWave(static_cast<FmSeaState*>(itsFmOwner), frameNr > 0);
}


FmMathFuncBase* FdSeaState::evaluateWave(FmSeaState* seaState, bool animate) const
{
  FmMathFuncBase* waveFunc = NULL;
  if (FmDB::getActiveViewSettings()->visibleWaves())
    if ((waveFunc = seaState->waveFunction.getPointer()))
      if (!waveFunc->initGetValue())
        waveFunc = NULL; // Wave initialization failed

  SoCoordinate3* coords = SO_GET_PART(itsKit,"planeCoords",SoCoordinate3);

  // Dimensions of the sea surface plane/box
  float dxDiv2 = 0.5f*(float)seaState->xLength.getValue();
  float dyDiv2 = 0.5f*(float)seaState->yLength.getValue();
  double depth = seaState->seaDepth.getValue();
  float bottom = -(float)depth;

  if (!waveFunc)
  {
    // Create simple a rectangle when no wave visualization
    float dxDiv2 = 0.5f*(float)seaState->xLength.getValue();
    float dyDiv2 = 0.5f*(float)seaState->yLength.getValue();
    coords->point.set1Value(0,-dxDiv2,-dyDiv2, 0.0f);
    coords->point.set1Value(1,-dxDiv2, dyDiv2, 0.0f);
    coords->point.set1Value(2, dxDiv2, dyDiv2, 0.0f);
    coords->point.set1Value(3, dxDiv2,-dyDiv2, 0.0f);
    if (depth > 0.0)
    {
      coords->point.set1Value(4,-dxDiv2,-dyDiv2, bottom);
      coords->point.set1Value(5,-dxDiv2, dyDiv2, bottom);
      coords->point.set1Value(6, dxDiv2, dyDiv2, bottom);
      coords->point.set1Value(7, dxDiv2,-dyDiv2, bottom);
    }
    return waveFunc;
  }

  FdAnimateModel* animator = animate ? FapAnimationCmds::getFdAnimator() : NULL;
  double time = animator ? animator->getCurrentTime() : 0.0;
  double g = seaState->getGrav().length();

  // Position of the sea surface plane
  float x = (float)seaState->getX();
  float y = (float)seaState->getY();

  // Surface grid discretization
  int num = seaState->getQuantization() > 2 ? seaState->getQuantization() : 2;
  float incX = dxDiv2*2.0f/float(num-1);
  float incY = dyDiv2*2.0f/float(num-1);

  //----- Find wave height for all gridpoints -----

  FaVec3 pos;
  if (waveFunc->isSurfaceFunc()) // Evaluate in a 2D grid
  {
    int i, j, k;
    float yp = -dyDiv2;
    for (j = k = 0; j < num; j++, yp += incY)
    {
      pos.y(y+yp);
      float xp = -dxDiv2;
      for (i = 0; i < num; i++, k++, xp += incX) {
	pos.x(x+xp);
	float eta = (float)waveFunc->getValue(g,depth,pos,time);
	// Sea surface coordinates
	coords->point.set1Value(k, xp, yp, eta);
      }
    }
    num *= num;
  }
  else // Evaluate in x-direction only (no spreading)
  {
    float xp = -dxDiv2;
    for (int i = 0; i < num; i++, xp += incX) {
      pos.x(x+xp);
      double eta = waveFunc->getValue(g,depth,pos,time);
      // Sea surface coordinates
      coords->point.set1Value(    i, xp,-dyDiv2, eta);
      coords->point.set1Value(num+i, xp, dyDiv2, eta);
    }
    num *= 2;
  }

  if (depth > 0.0)
  {
    // Bottom plane coordinates (corner points only)
    coords->point.set1Value(num++,-dxDiv2,-dyDiv2, bottom);
    coords->point.set1Value(num++, dxDiv2,-dyDiv2, bottom);
    coords->point.set1Value(num++,-dxDiv2, dyDiv2, bottom);
    coords->point.set1Value(num++, dxDiv2, dyDiv2, bottom);
  }

  return waveFunc;
}
