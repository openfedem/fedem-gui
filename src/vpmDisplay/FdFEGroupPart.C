// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFlLib/FFlVisualization/FFlGroupPartCreator.H"
#include "vpmDisplay/FdFEGroupPart.H"


FdFEGroupPart::FdFEGroupPart()
{
  IAmAFaceShape = true;
  myRenderRefCount = 0;
  myResultsOnRefCount = 0;
  myResultsLookOnRefCount = 0;
  myResultsVertexOnRefCount = 0;
  myGroupPartData = NULL;
}


void FdFEGroupPart::initWhenConstructed()
{
  this->toggleOn(myRenderRefCount ? true : false);
  this->setResultsOn(myResultsOnRefCount ? true : false);
  this->setResultLookOn(myResultsLookOnRefCount ? true : false);
}


void FdFEGroupPart::addRenderRef()
{
  this->toggleOn(++myRenderRefCount ? true : false);
}

void FdFEGroupPart::removeRenderRef()
{
  if (myRenderRefCount > 0)
    myRenderRefCount--;
  this->toggleOn(myRenderRefCount ? true : false);
}


void FdFEGroupPart::setFaceIndexes(bool isFaceShape,
                                   const std::vector< std::vector<int> >& faces)
{
  IAmAFaceShape = isFaceShape;
  this->setLineOffsetOn(IAmAFaceShape);
  this->setGouradOn(IAmAFaceShape);
  this->setShapeIndexes(isFaceShape, faces);
}


void FdFEGroupPart::setGroupPartData(FFlGroupPartData* groupPartData,
                                     unsigned short int linePattern)
{
  myGroupPartData = groupPartData;
  if (myGroupPartData)
  {
    IAmAFaceShape = !groupPartData->isLineShape;
    this->setLineOffsetOn(IAmAFaceShape);
    this->setGouradOn(IAmAFaceShape);
    this->setLinePattern(linePattern);
    groupPartData->visualModel = this;
  }

  this->generateShapeIndexes();
}


void FdFEGroupPart::updateElementVisibility()
{
  this->generateShapeIndexes();
  this->remapLookResults();
}


void FdFEGroupPart::updateOverallLook(void* id, const FFdLook& look)
{
  myAccumulatedLooks[id] = look;

  int nLooks = 0;
  for (const std::pair<void* const,FFdLook>& alook : myAccumulatedLooks)
    if (++nLooks == 1)
      myAccumulatedLook = alook.second;
    else
      myAccumulatedLook += alook.second;

  if (nLooks > 1)
    myAccumulatedLook *= 1.0f/(float)nLooks;

  if (myOverallOverrideLooks.empty() &&
      myHighlightOverrideLooks.empty() &&
      myContrastOverrideLooks.empty())
    this->setLook(myAccumulatedLook);
}


void FdFEGroupPart::pushOverallOverrideLook(const FFdLook& look)
{
  myOverallOverrideLooks.push(look);
  this->updateOverrideLook();
}

FFdLook FdFEGroupPart::popOverallOverrideLook()
{
  FFdLook look;
  if (!myOverallOverrideLooks.empty())
  {
    look = myOverallOverrideLooks.top();
    myOverallOverrideLooks.pop();
    this->updateOverrideLook();
  }

  return look;
}


void FdFEGroupPart::pushContrastOverrideLook(const FFdLook& look)
{
  myContrastOverrideLooks.push(look);
  this->updateOverrideLook();
}

FFdLook FdFEGroupPart::popContrastOverrideLook()
{
  FFdLook look;
  if (!myContrastOverrideLooks.empty())
  {
    look = myContrastOverrideLooks.top();
    myContrastOverrideLooks.pop();
    this->updateOverrideLook();
  }

  return look;
}


void FdFEGroupPart::pushHighlightOverrideLook(const FFdLook& look)
{
  myHighlightOverrideLooks.push(look);
  this->updateOverrideLook();
}

FFdLook FdFEGroupPart::popHighlightOverrideLook()
{
  FFdLook look;
  if (!myHighlightOverrideLooks.empty())
  {
    look = myHighlightOverrideLooks.top();
    myHighlightOverrideLooks.pop();
    this->updateOverrideLook();
  }

  return look;
}


void FdFEGroupPart::updateOverrideLook()
{
  if (!myHighlightOverrideLooks.empty())
  {
    this->setLook(myHighlightOverrideLooks.top());
    this->setResultLookOn(false);
  }
  else if (!myContrastOverrideLooks.empty())
  {
    this->setLook(myContrastOverrideLooks.top());
    this->setResultLookOn(false);
  }
  else if (!myOverallOverrideLooks.empty())
    this->setLook(myOverallOverrideLooks.top());
  else
  {
    this->setResultLookOn(myResultsLookOnRefCount ? true : false);
    this->setLook(myAccumulatedLook);
  }
}


void FdFEGroupPart::setLineWidth(int nPoints)
{
  if (!IAmAFaceShape)
    this->set3DLLineWidth(nPoints);
}


void FdFEGroupPart::setLinePattern(unsigned short pattern)
{
  if (!IAmAFaceShape)
    this->set3DLLinePattern(pattern);
}


void FdFEGroupPart::showResults(bool resultIsOn)
{
  if (resultIsOn)
  {
    if (++myResultsOnRefCount == 1)
      this->setResultsOn(true);
  }
  else
  {
    if (myResultsOnRefCount > 0 && --myResultsOnRefCount == 0)
      this->setResultsOn(false);
  }
}


void FdFEGroupPart::remapLookResults(const FFaLegendMapper& mapping)
{
  myLegendMapper = mapping;
  this->remapLookResults();
}


void FdFEGroupPart::showResultLook(bool resultsIsOn)
{
  if (resultsIsOn)
  {
    if (++myResultsLookOnRefCount == 1 && myOverallOverrideLooks.empty())
      this->setResultLookOn(true);
  }
  else
  {
    if (myResultsLookOnRefCount > 0 && --myResultsLookOnRefCount == 0)
      this->setResultLookOn(false);
  }
}
