// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdFEModel.H"
#include "FFlLib/FFlLinkHandler.H"
#include "FFlLib/FFlVisualization/FFlGroupPartCreator.H"


FdFEModel::FdFEModel()
{
  IAmHighlighted = false;
  myDetailHighlighted = FdFEVisControl::OUTLINE;
  myHighlightColor = { 1.0f, 0.0f, 0.0f };

  myCurrentResultsFrame = -1;
  myPosMxReadOp = NULL;
}


FdFEModel::~FdFEModel()
{
  if (myPosMxReadOp)
    myPosMxReadOp->unref();
}


/*!
  Update the vertexes of the model.
  It must be followed by either updateGroupParts.
*/

void FdFEModel::updateVertexes(FFlLinkHandler* FEModel)
{
  if (FEModel)
    this->setVertexes(FEModel->getVertexes());
}


/*!
  Updates the shape indices for all group parts in the one piece visualization.
  Depends on updateVertexes.
*/

void FdFEModel::updateGroupParts(FFlGroupPartCreator* gpc)
{
  this->deleteVisualization(true);
  if (!gpc) return;

  for (FFlGroupPartCreator::GroupPartMap::value_type& gp : *gpc)
    if (!gp.second.shapeIndexes.empty())
    {
      FdFEGroupPart* fdGP = this->createGroupPart();
      fdGP->setFaceIndexes(!gp.second.isLineShape,gp.second.shapeIndexes);
      if (gp.second.isLineShape)
        fdGP->setLinePattern(0xffff);
      gp.second.visualModel = fdGP;
      myGroupParts[gp.first].push_back(fdGP);
    }
    else if (!gp.second.facePointers.empty() || !gp.second.hiddenFaces.empty() ||
             !gp.second.edgePointers.empty() || !gp.second.hiddenEdges.empty())
    {
      FdFEGroupPart* fdGP = this->createGroupPart();
      fdGP->setGroupPartData(&gp.second);
      myGroupParts[gp.first].push_back(fdGP);
    }

  for (FFlGroupPartCreator::GroupPartMap::value_type& gp : gpc->getSpecialLines())
    if (!gp.second.edgePointers.empty())
    {
      FdFEGroupPart* fdGP = this->createGroupPart();
      fdGP->setGroupPartData(&gp.second,gp.first);
      myGroupParts[FFlGroupPartCreator::SPECIAL_LINES].push_back(fdGP);
    }

  this->updateVisControl();
}


void FdFEModel::addGroupPart(FdFEGroupPartSet::GroupPartType type,
                             FFlGroupPartData* groupPartData)
{
  if (!groupPartData)
    return;

  FdFEGroupPart* newFdGP = this->createGroupPart();
  if (groupPartData->shapeIndexes.empty())
    newFdGP->setGroupPartData(groupPartData);
  else
  {
    newFdGP->setFaceIndexes(!groupPartData->isLineShape,
                            groupPartData->shapeIndexes);

    if (groupPartData->isLineShape)
      newFdGP->setLinePattern(0xffff);

    groupPartData->visualModel = newFdGP;
  }

  myGroupParts.addGroupPart(type,newFdGP);
}


void FdFEModel::addLineShape(const std::vector<IntVec>& indices,
                             unsigned short int pattern)
{
  if (indices.empty())
    return;

  FdFEGroupPart* newFdGP = this->createGroupPart();
  newFdGP->setFaceIndexes(false,indices);
  newFdGP->setLinePattern(pattern);
  myGroupParts.addGroupPart(FdFEGroupPartSet::RED_OUTLINE_LINES,newFdGP);
}


void FdFEModel::updateElementVisibility()
{
  this->forEachGroupPart(&FdFEGroupPart::updateElementVisibility);
}


/*!
  Syncs the vsualization parametres stored as myVisParams
  with the myGroupParts FdFEVisControl, making sure they are in sync.
*/

void FdFEModel::updateVisControl()
{
  myGroupParts.setDrawDetail( myVisParams.detailLevel);
  myGroupParts.setLineDetail(myVisParams.lineDetailLevel);
  myGroupParts.setDrawStyle( myVisParams.drawStyle);
  myGroupParts.setLook( myVisParams.look );
  myGroupParts.showResults( myVisParams.showResults);
  myGroupParts.showColorResults( myVisParams.showColorResults);
  myGroupParts.showVertexResults( myVisParams.showVertexResults);
  myGroupParts.show( myVisParams.doShow );
}


/*!
  Turns link on and off. It will show the One-piece or
  the groupwise representation according to the visparameters set.
  \sa FdFEModel::updateVisParams()
*/

void FdFEModel::show(bool doShow)
{
  if (doShow && !myVisParams.doShow && myVisParams.showResults)
    this->selectResultFrame(myCurrentResultsFrame);

  this->setVisParam(myVisParams.doShow,
                    &FdFEVisControl::show,
                    doShow);
}


void FdFEModel::setDrawDetail(FdFEVisControl::DetailType detailLevel)
{
  this->setVisParam(myVisParams.detailLevel,
                    &FdFEVisControl::setDrawDetail,
                    detailLevel);
}


void FdFEModel::setLineDetail(FdFEVisControl::DetailType detailLevel)
{
  this->setVisParam(myVisParams.lineDetailLevel,
                    &FdFEVisControl::setLineDetail,
                    detailLevel);
}


void FdFEModel::setDrawStyle(FdFEVisControl::DrawStyleType drawStyle)
{
  this->setVisParam(myVisParams.drawStyle,
                    &FdFEVisControl::setDrawStyle,
                    drawStyle);
}


/*!
  Sets the overall look of the link. This is used as a default look
  for all groups, and as an override if the link is overriding the group looks.
*/

void FdFEModel::setLook(const FFdLook& aLook)
{
  if (myVisParams.look == aLook) return;

  myGroupParts.setLook(aLook);
  myVisParams.look = aLook;
}


void FdFEModel::highlight(bool isToHighlight,
                          FdFEVisControl::DetailType type,
                          const FdColor& color)
{
  if (IAmHighlighted == isToHighlight) return;

  IAmHighlighted = isToHighlight;
  myDetailHighlighted = type;
  myHighlightColor = color;

  myGroupParts.highlight(isToHighlight, type, color);
}


void FdFEModel::showResults(bool doShow)
{
  if (!doShow) {
    this->resetTempTransform();
    this->resetTempVxes();
  }

  this->setVisParam(myVisParams.showResults,
                    &FdFEVisControl::showResults,
                    doShow);
}


void FdFEModel::showColorResults(bool doShow)
{
  this->setVisParam(myVisParams.showColorResults,
                    &FdFEVisControl::showColorResults,
                    doShow);
}


void FdFEModel::setFringeLegendMapping(const FFaLegendMapper& mapping)
{
  for (std::vector<FdFEGroupPart*>& gpList : myGroupParts)
    for (FdFEGroupPart* gp : gpList)
      if (gp) gp->remapLookResults(mapping);

  // Remove result probes etc. to make sure they are not showing a wrong value.
  this->removeLabels();
}


void FdFEModel::showVertexResults(bool doShow)
{
  this->setVisParam(myVisParams.showVertexResults,
                    &FdFEVisControl::showVertexResults,
                    doShow);
}


void FdFEModel::addResultFrame(int beforeFrame)
{
  this->forEachGroupPart(&FdFEGroupPart::addResultFrame,beforeFrame);
}


void FdFEModel::deleteResultFrame(int frameIdx)
{
  this->forEachGroupPart(&FdFEGroupPart::deleteResultFrame,frameIdx);
}


void FdFEModel::selectResultFrame(int frameIdx)
{
  if (myVisParams.doShow)
    this->forEachGroupPart(&FdFEGroupPart::selectResultFrame,frameIdx);

  myCurrentResultsFrame = frameIdx;

  // Remove result probes etc. to make sure they are not showing a wrong value.
  this->removeLabels();
}


void FdFEModel::expandFrameArrayIfNeccesary(int frameIdx)
{
  this->forEachGroupPart(&FdFEGroupPart::expandFrameArrayIfNeccesary,frameIdx);
}


void FdFEModel::forEachGroupPart(shapeRefMethodType shapeRefMethod, int fidx)
{
  for (std::vector<FdFEGroupPart*>& gpList : myGroupParts)
    for (FdFEGroupPart* gp : gpList)
      if (gp) (gp->*shapeRefMethod)(fidx);
}


void FdFEModel::forEachGroupPart(shapeRefVoidMethod shapeRefMethod)
{
  for (std::vector<FdFEGroupPart*>& gpList : myGroupParts)
    for (FdFEGroupPart* gp : gpList)
      if (gp) (gp->*shapeRefMethod)();
}


template <class T>
void FdFEModel::setVisParam(T& onePieceParam,
                            void (FdFEVisControl::*setMethod)(T),
                            T param)
{
  if (onePieceParam == param) return;

  (myGroupParts.*setMethod)(param);
  onePieceParam = param;
}


/*!
  Makes the group parts show either the supplied special graphics
  (Typically a vrml model) or the generated FE visualization, or both;
*/

void FdFEModel::setVizMode(FdFEGroupPart::FdGpVizModeEnum type)
{
  for (std::vector<FdFEGroupPart*>& gpList : myGroupParts)
    for (FdFEGroupPart* gp : gpList)
      if (gp) gp->setVizMode(type);
}
