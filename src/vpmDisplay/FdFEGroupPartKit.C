// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPackedColor.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoPolygonOffset.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>

#include "FFlLib/FFlVisualization/FFlGroupPartCreator.H"
#include "vpmDisplay/FdFEGroupPartKit.H"
#include "vpmDisplay/FdBackPointer.H"

SoLightModel * FdFEGroupPartKit::ourBaseColorLightModel;

SoDrawStyle * FdFEGroupPartKit::ourLW2DrawStyle;
SoDrawStyle * FdFEGroupPartKit::ourLW3DrawStyle;

SoPolygonOffset * FdFEGroupPartKit::ourLinesOffset;

SoMaterialBinding * FdFEGroupPartKit::ourPrPartMaterialBinding;
SoMaterialBinding * FdFEGroupPartKit::ourPrFaceMaterialBinding;
SoMaterialBinding * FdFEGroupPartKit::ourPrVertexMaterialBinding;
SoMaterialBinding * FdFEGroupPartKit::ourPrFaceVertexMaterialBinding;

SoPackedColor * FdFEGroupPartKit::ourNoResultsColor = NULL;

SO_KIT_SOURCE(FdFEGroupPartKit);


void FdFEGroupPartKit::init()
{
  SO_KIT_INIT_CLASS(FdFEGroupPartKit, SoBaseKit, "BaseKit");

  ourBaseColorLightModel = new SoLightModel;
  ourBaseColorLightModel->ref();
  ourBaseColorLightModel->model.setValue(SoLightModel::BASE_COLOR);

  ourLW2DrawStyle = new SoDrawStyle;
  ourLW2DrawStyle->ref();
  ourLW2DrawStyle->lineWidth.setValue(2);

  ourLW3DrawStyle = new SoDrawStyle;
  ourLW2DrawStyle->ref();
  ourLW2DrawStyle->lineWidth.setValue(3);

  ourLinesOffset = new SoPolygonOffset;
  ourLinesOffset->ref();
  ourLinesOffset->styles.setValue(SoPolygonOffset::FILLED);
  ourLinesOffset->factor.setValue(1);
  ourLinesOffset->units.setValue(1);

  ourPrPartMaterialBinding = new SoMaterialBinding;
  ourPrPartMaterialBinding->ref();
  ourPrPartMaterialBinding->value.setValue(SoMaterialBinding::OVERALL);

  ourPrFaceMaterialBinding = new SoMaterialBinding;
  ourPrFaceMaterialBinding->ref();
  ourPrFaceMaterialBinding->value.setValue(SoMaterialBinding::PER_FACE);

  ourPrFaceVertexMaterialBinding = new SoMaterialBinding;
  ourPrFaceVertexMaterialBinding->ref();
  ourPrFaceVertexMaterialBinding->value.setValue(SoMaterialBinding::PER_VERTEX);

  ourPrVertexMaterialBinding = new SoMaterialBinding;
  ourPrVertexMaterialBinding->ref();
  ourPrVertexMaterialBinding->value.setValue(SoMaterialBinding::PER_VERTEX_INDEXED);

  ourNoResultsColor = new SoPackedColor;
  ourNoResultsColor->ref();
  ourNoResultsColor->orderedRGBA.setNum(1);
  ourNoResultsColor->orderedRGBA.set1Value(0, 0x888888ff);
}

//    Constructor

FdFEGroupPartKit::FdFEGroupPartKit()
{
  SO_KIT_CONSTRUCTOR(FdFEGroupPartKit);

  isBuiltIn = TRUE;

  SO_KIT_ADD_CATALOG_ENTRY(toggle,    SoSwitch,      FALSE, this,   \x0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(backPt,    FdBackPointer, TRUE,  this,   \x0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(separator, SoSeparator,   FALSE, toggle, \x0, TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(fullMaterial,  SoMaterial,      FALSE,
                           separator, \x0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(lightModel,    SoLightModel,    TRUE,
                           separator, \x0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(drawStyle,     SoDrawStyle,     TRUE,
                           separator, \x0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(polygonOffset, SoPolygonOffset, TRUE,
                           separator, \x0, TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(binding,        SoMaterialBinding, TRUE,
                           separator, \x0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(packedMaterial, SoPackedColor,     TRUE,
                           separator, \x0, TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(shapeToggle, SoSwitch, FALSE, separator, \x0, TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(shapeSep, SoSeparator, FALSE,
                           shapeToggle, \x0, TRUE);
  SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY(shape, SoIndexedShape, SoIndexedFaceSet, TRUE,
                                    shapeSep, \x0, TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(specialGraphics, SoSeparator, FALSE,
                           shapeToggle, \x0, TRUE);

  SO_KIT_INIT_INSTANCE();

  IAmOn = false;
  IAmShowingResults = false;
  IAmShowingResultLooks = false;
  IAmUsingPrivateDrawStyle = false;
  myVizMode = NORMAL;

  myCurrentFrame = 0;

  myLineWidth = 0;
  myLinePattern = 0xffff;
  myTransparency = 0.0f;

  this->initWhenConstructed();

  SoSeparator* sep = (SoSeparator*)this->separator.getValue();
  sep->renderCaching.setValue(SoSeparator::OFF);
}

FdFEGroupPartKit::~FdFEGroupPartKit()
{
  this->deleteResultFrame(-1);
}

void FdFEGroupPartKit::setSpecialGraphics(SoSeparator * scene, bool isLineShape = false)
{
  this->setPart("specialGraphics", scene);
  this->setLineOffsetOn(!isLineShape);
}

bool FdFEGroupPartKit::hasSpecialGraphics()
{
  return specialGraphics.getValue() ? true : false;
}

///////////////////////////////////////////
//
// Implementation of 3D lib interface :
//

void FdFEGroupPartKit::setFdPointer(FdObject* backPnt)
{
  SO_GET_PART(this,"backPt",FdBackPointer)->setPointer(backPnt);
}

void FdFEGroupPartKit::setShapeIndexes(bool isFaceShape,
				       const std::vector < std::vector<int> > & faces)
{
  // Make sure we have the right type of shape (faces/lines) :

  SoIndexedShape* aShape = (SoIndexedShape*)(this->getPart("shape",false));
  if (!aShape || (SbBool)isFaceShape != aShape->isOfType(SoIndexedFaceSet::getClassTypeId()))
  {
    // We have not got right type. Make a right one
    if (isFaceShape)
      aShape = new SoIndexedFaceSet;
    else
      aShape = new SoIndexedLineSet;

    // Install it in the kit
    this->setPart("shape",aShape);
  }

  if (faces.empty()) return;

  // Calculate needed storage from input

  int nIndices = 0;
  for (const std::vector<int>& face : faces)
    nIndices += face.size() + 1;
  aShape->coordIndex.setNum(nIndices);

  // Pointer into the internals of SoMFInt32
  int32_t* idx = aShape->coordIndex.startEditing();

  // Set the face indexes into the shape node
  for (const std::vector<int>& face : faces)
  {
    for (int faceIndex : face)
      *(idx++) = faceIndex;
    *(idx++) = -1; // Inserting face/line separator
  }

  aShape->coordIndex.finishEditing();
}

void FdFEGroupPartKit::generateShapeIndexes()
{
  if (!myGroupPartData)
    return;

  // Make sure we have the right type of shape (faces/lines) :

  bool isFaceShape = !myGroupPartData->isLineShape;
  SoIndexedShape* aShape = (SoIndexedShape*)(this->getPart("shape",false));
  if (!aShape || (SbBool)isFaceShape != aShape->isOfType(SoIndexedFaceSet::getClassTypeId()))
  {
    // We have not got right type. Make a right one
    if (isFaceShape)
      aShape = new SoIndexedFaceSet;
    else
      aShape = new SoIndexedLineSet;

    // Install it in the kit
    this->setPart("shape",aShape);
  }

  // Calculate needed storage from input

  int nIndexEntries = 0;
  if (myGroupPartData->isIndexShape)
    for (const std::vector<int>& shape : myGroupPartData->shapeIndexes)
      nIndexEntries += shape.size() + 1;
  else if (isFaceShape)
    nIndexEntries = myGroupPartData->facePointers.size() + myGroupPartData->nVisiblePrimitiveVertexes;
  else
    nIndexEntries = myGroupPartData->edgePointers.size()*3;

  // Set the face indexes into the shape node
  aShape->coordIndex.setNum(nIndexEntries);
  myGroupPartData->getShapeIndexes(aShape->coordIndex.startEditing());
  aShape->coordIndex.finishEditing();
}


void FdFEGroupPartKit::toggleOn(bool turnOn)
{
  if (IAmOn == turnOn) return;

  IAmOn = turnOn;
  this->updateContents();
}

void FdFEGroupPartKit::setVizMode(FdGpVizModeEnum mode)
{
  myVizMode = mode;
  this->updateContents();
}

void FdFEGroupPartKit::setLook(const FFdLook& aLook)
{
  SoMaterial* mat = (SoMaterial*)fullMaterial.getValue();

  mat->diffuseColor.setValue(aLook.diffuseColor.data());
  mat->ambientColor.setValue(aLook.ambientColor.data());
  mat->specularColor.setValue(aLook.specularColor.data());

  if (!(IAmShowingResultLooks && IAmShowingResults))
    mat->transparency = aLook.transparency;
  myTransparency = aLook.transparency;
  mat->shininess = aLook.shininess;
}

void FdFEGroupPartKit::setGouradOn(bool setOn)
{
  this->setPart("lightModel", setOn ? NULL : ourBaseColorLightModel);
}

void FdFEGroupPartKit::set3DLLineWidth(int lineWidth)
{
  myLineWidth = lineWidth;
  this->updateDrawStyle();
}

void FdFEGroupPartKit::set3DLLinePattern(unsigned short pattern)
{
  myLinePattern = pattern;
  this->updateDrawStyle();
}

void FdFEGroupPartKit::updateDrawStyle()
{
  if (myLinePattern == 0xfff)
    switch (myLineWidth)
      {
      case 0:
      case 1:
        this->setPart("drawStyle",NULL);
        IAmUsingPrivateDrawStyle = false;
        return;
      case 2:
        this->setPart("drawStyle",ourLW2DrawStyle);
        IAmUsingPrivateDrawStyle = false;
        return;
      case 3:
        this->setPart("drawStyle",ourLW3DrawStyle);
        IAmUsingPrivateDrawStyle = false;
        return;
      default:
        break;
      }

  SoDrawStyle* ds;
  if (IAmUsingPrivateDrawStyle)
    ds = (SoDrawStyle*)(this->getPart("drawStyle", true));
  else
  {
    ds = new SoDrawStyle;
    this->setPart("drawStyle",ds);
  }
  ds->lineWidth.setValue((float)myLineWidth);
  IAmUsingPrivateDrawStyle = true;
}

void FdFEGroupPartKit::setLineOffsetOn(bool turnOn)
{
  this->setPart("polygonOffset", turnOn ? ourLinesOffset : NULL);
}

void FdFEGroupPartKit::setResultsOn(bool resultIsOn)
{
  if (IAmShowingResults == resultIsOn) return;

  IAmShowingResults = resultIsOn;
  this->updateContents();
}

void FdFEGroupPartKit::selectResultFrame(int frameIdx)
{
  if (frameIdx < 0 || frameIdx == (int)myCurrentFrame)
    return;

  myCurrentFrame = frameIdx;
  this->updateContents();
}

void FdFEGroupPartKit::addResultFrame(int beforeFrame)
{
  if (beforeFrame < 0) // Adding one to the end
    this->expandFrameArrayIfNeccesary(myResultFrames.size());
  else
  {
    // Inserting in the middle :
    std::vector<ResultsFrame*>::iterator it = myResultFrames.begin();
    it += beforeFrame;
    if (beforeFrame > 0)
      this->expandFrameArrayIfNeccesary(beforeFrame-1);
    myResultFrames.insert(it, NULL);
  }
}

void FdFEGroupPartKit::deleteResultFrame(int frameIdx)
{
  if (frameIdx < 0)
  {
    // Delete all frames :
    this->deleteResultLook(-1);
    std::vector<ResultsFrame*> empty;
    myResultFrames.swap(empty);
  }
  else if (frameIdx < (int)myResultFrames.size())
  {
    // Delete a single frame
    std::vector<ResultsFrame*>::iterator it = myResultFrames.begin();
    this->deleteResultLook(frameIdx);
    it += frameIdx;
    myResultFrames.erase(it);
  }
}

void FdFEGroupPartKit::setResultLookOn(bool turnOn)
{
  if (IAmShowingResultLooks == turnOn) return;

  IAmShowingResultLooks = turnOn;
  this->updateContents();
}


void FdFEGroupPartKit::remapLookResults()
{
  for (ResultsFrame* frame : myResultFrames)
    this->remapLookResults(frame, myLegendMapper);
}


void FdFEGroupPartKit::remapLookResults(ResultsFrame* frame, const FFaLegendMapper& mapping)
{
  if (frame->resValues.empty()) return;

  SoPackedColor* pc = frame->getResColors();

  if (!myGroupPartData || myGroupPartData->isIndexShape)
  {
    pc->orderedRGBA.setNum(frame->resValues.size());
    uint32_t* packedColors = pc->orderedRGBA.startEditing();
    for (size_t i = 0; i < frame->resValues.size(); i++)
      packedColors[i] = mapping.getColor(frame->resValues[i]);
    pc->orderedRGBA.finishEditing();
  }
  else
  {
    size_t nColors = 0;

    switch (frame->resLookPolicy)
      {
      case PR_FACE:
        if (myGroupPartData->isLineShape)
          nColors = myGroupPartData->edgePointers.size();
        else
          nColors = myGroupPartData->facePointers.size();
        break;
      case PR_FACE_VERTEX:
        if (myGroupPartData->isLineShape)
          nColors = myGroupPartData->edgePointers.size()*2;
        else
          nColors = myGroupPartData->nVisiblePrimitiveVertexes;
        break;
      default:
        break;
      }

    pc->orderedRGBA.setNum(nColors);
    uint32_t* packedColors = pc->orderedRGBA.startEditing();
    int nValues = frame->resValues.size();

    switch (frame->resLookPolicy)
      {
      case PR_FACE:
        if (myGroupPartData->isLineShape)
        {
          for (size_t i = 0; i < nColors; i++)
            if (int resIdx = myGroupPartData->edgePointers[i].second; resIdx >= 0 && resIdx < nValues)
              packedColors[i] = mapping.getColor(frame->resValues[resIdx]);
            else
              packedColors[i] = mapping.getColor(HUGE_VAL);
        }
        else
        {
          for (size_t i = 0; i < nColors; i++)
            if (int resIdx = myGroupPartData->facePointers[i].second; resIdx >= 0 && resIdx < nValues)
              packedColors[i] = mapping.getColor(frame->resValues[resIdx]);
            else
              packedColors[i] = mapping.getColor(HUGE_VAL);
        }
        break;

      case PR_FACE_VERTEX:
        if (myGroupPartData->isLineShape)
        {
          size_t i, j;
          for (i = j = 0; i < myGroupPartData->edgePointers.size() && j < nColors; i++)
            if (int resIdx = myGroupPartData->edgePointers[i].second; resIdx >= 0 && resIdx < nValues)
              for (int vx = 0; vx < 2 && j < nColors; vx++, j++)
                packedColors[j] = mapping.getColor(frame->resValues[resIdx + vx]);
            else
              for (int vx = 0; vx < 2 && j < nColors; vx++, j++)
                packedColors[j] = mapping.getColor(HUGE_VAL);
        }
        else
        {
          size_t i, j;
          for (i = j = 0; i < myGroupPartData->facePointers.size() && j < nColors; i++)
            if (int resIdx = myGroupPartData->facePointers[i].second; resIdx >= 0 && resIdx < nValues)
              for (int vx = 0; vx < myGroupPartData->facePointers[i].first->getNumVertices() && j < nColors; vx++, j++)
                packedColors[j] = mapping.getColor(frame->resValues[resIdx + vx]);
            else
              for (int vx = 0; vx < myGroupPartData->facePointers[i].first->getNumVertices() && j < nColors; vx++, j++)
                packedColors[j] = mapping.getColor(HUGE_VAL);
        }
        break;

      default:
        break;
      }

    pc->orderedRGBA.finishEditing();
  }
}


float FdFEGroupPartKit::getResultFromMaterialIndex(unsigned int matIdx)
{
  if (myCurrentFrame >= myResultFrames.size() || !myResultFrames[myCurrentFrame] ||
      myResultFrames[myCurrentFrame]->resValues.empty())
    return float(HUGE_VAL); // Non-existing or empy frame

  int resIdx = -1;
  ResultsFrame* frame = myResultFrames[myCurrentFrame];
  if (!myGroupPartData || myGroupPartData->isIndexShape)
    resIdx = matIdx;
  else
    switch (frame->resLookPolicy)
      {
      case PR_FACE:
        if (myGroupPartData->isLineShape)
        {
          if (matIdx < myGroupPartData->edgePointers.size())
            resIdx = myGroupPartData->edgePointers[matIdx].second;
        }
        else
        {
          if (matIdx < myGroupPartData->facePointers.size())
            resIdx = myGroupPartData->facePointers[matIdx].second;
        }
        break;

      case PR_FACE_VERTEX:
        if (myGroupPartData->isLineShape)
        {
          if (matIdx < myGroupPartData->edgePointers.size()*2)
            resIdx = matIdx;
        }
        else
        {
          if ((int)matIdx < myGroupPartData->nVisiblePrimitiveVertexes)
            resIdx = matIdx;
        }
        break;

      default:
        break;
      }

  if (resIdx >= 0 && resIdx < (int)frame->resValues.size())
     return frame->resValues[resIdx];

  return float(HUGE_VAL);
}


bool FdFEGroupPartKit::hasResultLook(unsigned int frameIdx)
{
  if (frameIdx < myResultFrames.size() && myResultFrames[frameIdx])
    return !myResultFrames[frameIdx]->resValues.empty();

  return false;
}

void FdFEGroupPartKit::setResultLook(unsigned int frameIdx, lookPolicy lookBinding,
				     std::vector<double>& lookValues,
				     const FFaLegendMapper& mapping)
{
  this->expandFrameArrayIfNeccesary(frameIdx);

  if (!myResultFrames[frameIdx])
    myResultFrames[frameIdx] = new ResultsFrame;

  myResultFrames[frameIdx]->resValues.clear();
  myResultFrames[frameIdx]->resValues.reserve(lookValues.size());
  for (double look : lookValues)
    myResultFrames[frameIdx]->resValues.push_back((float)look);

  myResultFrames[frameIdx]->resLookPolicy = lookBinding;
  this->remapLookResults(myResultFrames[frameIdx], mapping);
}

void FdFEGroupPartKit::deleteResultLook(int frameIdx) // frame = -1 => all
{
  if (frameIdx < 0)
  {
    // Removing all result looks
    for (ResultsFrame*& frame : myResultFrames)
      if (frame)
      {
        delete frame;
        frame = NULL;
      }
  }
  else if (frameIdx < (int)myResultFrames.size())
    if (myResultFrames[frameIdx])
    {
      delete myResultFrames[frameIdx];
      myResultFrames[frameIdx] = NULL;
    }
}


///////////////////////////////////////////
//
// Convenience methods :
//

void FdFEGroupPartKit::expandFrameArrayIfNeccesary(int frameIdx)
{
  if (frameIdx >= (int)myResultFrames.size())
    myResultFrames.resize(frameIdx+1,NULL);
}


void FdFEGroupPartKit::updateContents()
{
  if (SoSwitch* sw = (SoSwitch*)(this->toggle.getValue()); sw)
    sw->whichChild = IAmOn ? SO_SWITCH_ALL : SO_SWITCH_NONE;

  if (SoSwitch* sw = (SoSwitch*)(this->shapeToggle.getValue()); sw)
    switch (myVizMode)
      {
      case SPECIAL:
        sw->whichChild = 1;
        break;
      case NORMAL:
        sw->whichChild = 0;
        break;
      default:
        sw->whichChild = SO_SWITCH_ALL;
        break;
      }

  if (!IAmOn)
    return;

  // If we got results for the new frame, add them to the scene graph

  if (IAmShowingResults && IAmShowingResultLooks)
  {
    // If we have no results to show, show as gray instead
    SoMaterialBinding* newBinding = ourPrPartMaterialBinding;
    SoPackedColor*   newResColors = ourNoResultsColor;
    if (myCurrentFrame < myResultFrames.size() &&
        myResultFrames[myCurrentFrame] && myResultFrames[myCurrentFrame]->resColors)
    {
      switch (myResultFrames[myCurrentFrame]->resLookPolicy)
        {
        case FdFEGroupPart::PR_FACE:
          newBinding = ourPrFaceMaterialBinding;
          break;
        case FdFEGroupPart::PR_VERTEX:
          newBinding = ourPrVertexMaterialBinding;
          break;
        case FdFEGroupPart::PR_FACE_VERTEX:
          newBinding = ourPrFaceVertexMaterialBinding;
          break;
        default:
          newBinding = NULL;
          break;
        }
      newResColors = myResultFrames[myCurrentFrame]->resColors;
    }

    if (newBinding != this->binding.getValue())
      this->setPart("binding", newBinding);

    if (newResColors != this->packedMaterial.getValue())
    {
      this->setPart("packedMaterial", newResColors);
      if (this->fullMaterial.getValue())
        ((SoMaterial*)this->fullMaterial.getValue())->transparency = 0.0f;
    }
  }
  else
  {
    // Remove old frame results from the scene graph:

    if (this->binding.getValue())
      this->setPart("binding", NULL);

    if (this->packedMaterial.getValue()) {
      this->setPart("packedMaterial", NULL);
      if (this->fullMaterial.getValue())
        ((SoMaterial*)(this->fullMaterial.getValue()))->transparency = myTransparency;
    }
  }
}


FdFEGroupPartKit::ResultsFrame::~ResultsFrame()
{
  if (resColors)
    resColors->unref();
}

SoPackedColor* FdFEGroupPartKit::ResultsFrame::getResColors()
{
  if (!resColors)
  {
    resColors = new SoPackedColor;
    resColors->ref();
  }

  return resColors;
}
