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
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoPackedColor.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoPolygonOffset.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoTransform.h>

#include "vpmDisplay/FdFEModelKit.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdFEGroupPartKit.H"
#include "vpmDisplay/FdTransformKit.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdLabelKit.H"
#include "vpmDisplay/FdSymbolDefs.H"

SO_KIT_SOURCE(FdFEModelKit);


void FdFEModelKit::init()
{
  SO_KIT_INIT_CLASS(FdFEModelKit, SoBaseKit, "BaseKit");

  FdLabelKit::init();
}


FdFEModelKit::FdFEModelKit()
{
  SO_KIT_CONSTRUCTOR(FdFEModelKit);

  isBuiltIn = TRUE;

  SO_KIT_ADD_CATALOG_ENTRY(toggle,   SoSwitch ,            FALSE,
			   this,    \x0 , TRUE );
  SO_KIT_ADD_CATALOG_ENTRY(linkSep,   SoSeparator ,        FALSE,
			   toggle,  \x0 , TRUE );
  SO_KIT_ADD_CATALOG_ENTRY(transform,   SoTransform ,      FALSE,
			   linkSep, \x0 , TRUE );
  SO_KIT_ADD_CATALOG_ENTRY(transform2,   SoTransform ,     TRUE,
			   linkSep, \x0 , TRUE );
  SO_KIT_ADD_CATALOG_ENTRY(coords,   SoVertexProperty,     FALSE,
			   linkSep, \x0 , TRUE );
  SO_KIT_ADD_CATALOG_ENTRY(groupParts,   SoGroup ,         FALSE,
			   linkSep, \x0 , TRUE );
  SO_KIT_ADD_CATALOG_ENTRY(vrmlGraphics, SoSeparator ,     FALSE,
			   linkSep, \x0 , TRUE );

  SO_KIT_ADD_CATALOG_ENTRY(backPt,        FdBackPointer,   TRUE,
			   this,    \x0 , TRUE );

  SO_KIT_ADD_CATALOG_ENTRY(labelMaterial,SoMaterial,       FALSE,
			   linkSep, \x0 , TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(labels,        SoSeparator,     FALSE,
			   linkSep, \x0 , TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(symbolMaterial,SoMaterial,      FALSE,
			   linkSep, \x0 , TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(spiderSwitch,  SoSwitch,        FALSE,
			   linkSep, \x0,  TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(cgSymbolSwitch,SoSwitch,        FALSE,
			   linkSep, \x0,  TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(symbolSwitch,  SoSwitch,        FALSE,
			   linkSep, \x0,  TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(beamCSSwitch,  SoSwitch,        FALSE,
			   linkSep, \x0,  TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(symbolSep,  SoSeparator,        FALSE,
			   symbolSwitch, \x0,  TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(symbol,        FdSymbolKit,     FALSE,
			   symbolSep, \x0 , TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(corotCSSwitch, SoSwitch,        FALSE,
			   linkSep, \x0 , TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(corotCSSep,  SoSeparator,       FALSE,
			   corotCSSwitch, \x0,  TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(corotCSSymbol, FdTransformKit,  FALSE,
			   corotCSSep, \x0 , TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(internalCSSwitch, SoSwitch,     FALSE,
			   linkSep,  \x0, TRUE);

  SO_KIT_INIT_INSTANCE();

  myVertexes = new SoVertexProperty;
  myVertexes->ref();
  myCurrentResultsFrame = -1;
  IAmUsingMyTransform = true;
  myDeformationScale = 1;

  this->setPart("coords", myVertexes);
  IAmUsingMyVertexes = true;

  SoSwitch* gfSw = (SoSwitch*)this->toggle.getValue();
  if (gfSw) gfSw->whichChild.setValue(SO_SWITCH_ALL);
  SoSeparator* sep = (SoSeparator*)this->linkSep.getValue();
  sep->renderCaching.setValue(SoSeparator::OFF);
}


FdFEModelKit::~FdFEModelKit()
{
  myVertexes->unref();
  this->deleteResultFrame(-1);
}


FdFEGroupPart* FdFEModelKit::createGroupPart()
{
  FdFEGroupPartKit* newOne = new FdFEGroupPartKit;
  ((SoGroup*)this->getPart("groupParts",TRUE))->addChild(newOne);

  return newOne;
}


void FdFEModelKit::addGroupPart(FdFEGroupPartSet::GroupPartType type,
                                SoSeparator* specialGraphics)
{
  if (!specialGraphics)
    return;

  bool isLineShape = false;
  switch (type) {
  case FdFEGroupPartSet::RED_OUTLINE_LINES:
  case FdFEGroupPartSet::OUTLINE_LINES:
  case FdFEGroupPartSet::RED_SURFACE_LINES:
  case FdFEGroupPartSet::SURFACE_LINES:
  case FdFEGroupPartSet::RED_INTERNAL_LINES:
  case FdFEGroupPartSet::INTERNAL_LINES:
  case FdFEGroupPartSet::RED_SPECIAL_LINES:
  case FdFEGroupPartSet::SPECIAL_LINES:
  case FdFEGroupPartSet::BBOX_LINES:
    isLineShape = true;
  default:
    break;
  }

  FdFEGroupPartKit* newOne = new FdFEGroupPartKit;
  ((SoGroup*)this->getPart("groupParts",TRUE))->addChild(newOne);
  newOne->setSpecialGraphics(specialGraphics,isLineShape);
  myGroupParts[type].push_back(newOne);
}

void FdFEModelKit::deleteGroupParts(FdFEGroupPartSet::GroupPartType type)
{
  SoGroup* group = (SoGroup*)this->getPart("groupParts",TRUE);
  for (FdFEGroupPart* gp : myGroupParts[type])
    group->removeChild(dynamic_cast<FdFEGroupPartKit*>(gp));
  myGroupParts[type].clear();
}

void FdFEModelKit::show(bool doShow)
{
  SoSwitch* gfSw = (SoSwitch*)this->toggle.getValue();
  if (gfSw)
    gfSw->whichChild.setValue(doShow ? SO_SWITCH_ALL : SO_SWITCH_NONE);

  this->FdFEModel::show(doShow);
}


/*!
  Deletes all the group parts and the book-keeping in FdFEVisControl class
  named myGroupParts.
  If keepSpecialGraphics is true, only the grouparts without some special graphics
  (typicaly some vrml stuff) is deleted.
*/

void FdFEModelKit::deleteVisualization(bool keepSpecialGraphics)
{
  SoGroup* group = (SoGroup*)this->getPart("groupParts",TRUE);
  if (!keepSpecialGraphics)
  {
    group->removeAllChildren();
    myGroupParts.clearGroupParts();
  }
  else
  {
    for (std::vector<FdFEGroupPart*>& gpList : myGroupParts)
    {
      std::vector<FdFEGroupPart*> specialGps;
      for (FdFEGroupPart* gp : gpList)
        if (gp->hasSpecialGraphics()) {
          specialGps.push_back(gp);
          ((FdFEGroupPartKit*)gp)->ref();
        }
      gpList.swap(specialGps);
    }

    group->removeAllChildren();

    for (std::vector<FdFEGroupPart*>& gpList : myGroupParts)
      for (FdFEGroupPart* gp : gpList)
      {
        group->addChild((FdFEGroupPartKit*)gp);
        ((FdFEGroupPartKit*)gp)->unref();
      }
  }

  for (ResultsFrame& frame : myResultsFrames)
    frame.eraseVxRes();
}


void FdFEModelKit::setFdPointer(FdObject* backPt)
{
  SO_GET_PART(this,"backPt",FdBackPointer)->setPointer(backPt);
}


void FdFEModelKit::selectResultFrame(int frameIdx)
{
  if (myVisParams.showTransformResults)
    this->setTransformFrame(frameIdx);

  if (myVisParams.showVertexResults)
    this->setVxFrame(frameIdx);

  this->FdFEModel::selectResultFrame(frameIdx);
}


void FdFEModelKit::setTransformFrame(unsigned int frameIdx)
{
  if (frameIdx >= myResultsFrames.size()) return;

  if (myResultsFrames[frameIdx].mx) {
    this->setTempTransform(*(myResultsFrames[frameIdx].mx));
    IAmUsingMyTransform = false;
  }
  else
    this->resetTempTransform();
}


void FdFEModelKit::setVxFrame(unsigned int frameIdx)
{
  if (frameIdx >= myResultsFrames.size()) return;

  if (myResultsFrames[frameIdx].vxProp) {
    this->setTempVxes(myResultsFrames[frameIdx].vxProp);
    IAmUsingMyVertexes = false;
  }
  else
    this->resetTempVxes();
}


void FdFEModelKit::showTransformResults(bool doShow)
{
  if (myVisParams.showTransformResults != doShow)
  {
    if (doShow)
      this->setTransformFrame(myCurrentResultsFrame);
    else
      this->resetTempTransform();
  }

  myVisParams.showTransformResults = doShow;
  myGroupWiseVisParams.showTransformResults = doShow;
}


void FdFEModelKit::showVertexResults(bool doShow)
{
  if (myVisParams.showVertexResults != doShow)
  {
    if (doShow)
      this->setVxFrame(myCurrentResultsFrame);
    else
      this->resetTempVxes();
  }
  this->FdFEModel::showVertexResults(doShow);

  myVisParams.showVertexResults = doShow;
  myGroupWiseVisParams.showVertexResults = doShow;
}


void* FdFEModelKit::addLabel(const std::string& text, const FaVec3& position)
{
  FdLabelKit* label = new FdLabelKit(text,position);
  ((SoSeparator*)this->getPart("labels",TRUE))->addChild(label);
  return (void*)label;
}

void FdFEModelKit::removeLabels(void* id)
{
  if (id)
    ((SoSeparator*)this->getPart("labels",TRUE))->removeChild((SoNode*)id);
  else
    this->setPart("labels", NULL);
}

FaMat34 FdFEModelKit::getActiveTransform()
{
  SoTransform* xf = (SoTransform*)transform.getValue();
  SbMatrix mx;
  mx.setTransform(xf->translation.getValue(), xf->rotation.getValue(), SbVec3f(1,1,1));
  return FdConverter::toFaMat34(mx);
}

void FdFEModelKit::setTransform(const FaMat34& pos)
{
  myTransformMx = pos;
  if (IAmUsingMyTransform)
    this->setTempTransform(pos);
}

void FdFEModelKit::setTempTransform(const FaMat34& pos)
{
  SoTransform* xf = (SoTransform*)transform.getValue();
  if (xf) xf->setMatrix(FdConverter::toSbMatrix(pos));
}

void FdFEModelKit::resetTempTransform()
{
  if (IAmUsingMyTransform) return;

  this->setTempTransform(myTransformMx);
  IAmUsingMyTransform = true;
}

void FdFEModelKit::setTempVxes(SoVertexProperty* vxes)
{
  this->setPart("coords", vxes);
}

void FdFEModelKit::resetTempVxes()
{
  if (IAmUsingMyVertexes) return;

  this->setTempVxes(myVertexes);
  IAmUsingMyVertexes = true;
}


void FdFEModelKit::setLinkCSToggle(bool doShow)
{
  SoSwitch* symbolSw = (SoSwitch*) this->symbolSwitch.getValue();
  if (symbolSw)
    symbolSw->whichChild.setValue( doShow ? SO_SWITCH_ALL : SO_SWITCH_NONE);
}

void FdFEModelKit::setBeamCSToggle(bool doShow)
{
  SoSwitch* symbolSw = (SoSwitch*) this->beamCSSwitch.getValue();
  if (symbolSw)
    symbolSw->whichChild.setValue( doShow ? SO_SWITCH_ALL : SO_SWITCH_NONE);
}


void FdFEModelKit::setInternalCSToggle(bool doShow)
{
  SoSwitch* symbolSw = (SoSwitch*) this->internalCSSwitch.getValue();
  if (symbolSw)
    symbolSw->whichChild.setValue( doShow ? SO_SWITCH_ALL : SO_SWITCH_NONE);
}

void FdFEModelKit::setCorotCSToggle(bool doShow)
{
  SoSwitch* symbolSw = (SoSwitch*) this->corotCSSwitch.getValue();
  if (symbolSw)
    symbolSw->whichChild.setValue( doShow ? SO_SWITCH_ALL : SO_SWITCH_NONE);
}

void FdFEModelKit::setCoGCSToggle(bool doShow)
{
  SoSwitch* symbolSw = (SoSwitch*) this->cgSymbolSwitch.getValue();
  if (symbolSw)
    symbolSw->whichChild.setValue( doShow ? SO_SWITCH_ALL : SO_SWITCH_NONE);
}

void FdFEModelKit::addInternalCS(const FaMat34& localMx)
{
  SoSwitch* symbolSw = (SoSwitch*) this->internalCSSwitch.getValue();
  FdTransformKit* xfKit = new FdTransformKit;
  xfKit->ref();
  SoTransform* xf = SO_GET_PART(xfKit, "firstTrans", SoTransform);
  xf->setMatrix(FdConverter::toSbMatrix(localMx));
  FdSymbolKit* symb = FdSymbolDefs::getSymbol(FdSymbolDefs::INT_LINK_COORD_SYS);
  xfKit->setPart("symbol", symb);
  xfKit->setPart("appearance", NULL);

  symbolSw->addChild(xfKit);
  xfKit->unref();
}

void FdFEModelKit::setCorotCS(const FaMat34& localMx)
{
  FdTransformKit* xfKit = SO_GET_PART(this, "corotCSSymbol", FdTransformKit);
  SoTransform* xf = SO_GET_PART(xfKit, "firstTrans", SoTransform);
  xf->setMatrix(FdConverter::toSbMatrix(localMx));
  FdSymbolKit* symb = FdSymbolDefs::getSymbol(FdSymbolDefs::LINK_COORD_SYS);
  xfKit->setPart("symbol", symb);
}

FaVec3 FdFEModelKit::getVertex(int idx) const
{
  if (idx >= 0 && idx < myVertexes->vertex.getNum())
    return FdConverter::toFaVec3(myVertexes->vertex[idx]);
  else
    return FaVec3();
}

void FdFEModelKit::setVertexes(const std::vector<FaVec3*>& vertexes)
{
  myVertexes->vertex.setNum(vertexes.size());

  SbVec3f* sbVectors = myVertexes->vertex.startEditing();
  for (FaVec3* vx : vertexes)
  {
    sbVectors->setValue((float)vx->x(),(float)vx->y(),(float)vx->z());
    sbVectors++;
  }

  myVertexes->vertex.finishEditing();
}

void FdFEModelKit::setVertexes(const VertexVec& vertexes)
{
  myVertexes->vertex.setNum(vertexes.size());

  SbVec3f* sbVectors = myVertexes->vertex.startEditing();
  for (const FaVec3& vx : vertexes)
  {
    sbVectors->setValue((float)vx.x(),(float)vx.y(),(float)vx.z());
    sbVectors++;
  }

  myVertexes->vertex.finishEditing();
}


/*!
  Result frames are just a storage for different result channels.
  The result frames are without results when created. An animation
  containing frames without results is supposed to show the FE model
  as it is without results.

  This will mean:
  * Vertices : FE model with original vertex positions are shown.
  * Colors : FE model with original color shown. If parts of the model
             has results it should probably show some "Neutral" color.
             Not done yet.
  * Transformations : If no transformations exist it will use the initial
                      Transformation. For single empty frames it would
                      perhaps be convenient to use the transformation
                      from the last frame containing a transformation.

  When adding (or setting) a specific results type for a frame,
  result frames are generated automatically to store the results.
  The other channels are empty.
  When deleting a specific result type for a frame, the frame will
  continue to exist until it is deleted explicitly.
*/

void FdFEModelKit::addResultFrame(int beforeFrame)
{
  if (beforeFrame < 0) // Adding to the end
    myResultsFrames.emplace_back();
  else
  {
    // Inserting in the middle :
    if (beforeFrame > 0)
      this->expandFrameArrayIfNeccesary(beforeFrame-1);
    std::vector<ResultsFrame>::iterator beforeFrameIt = myResultsFrames.begin();
    beforeFrameIt += beforeFrame;
    myResultsFrames.emplace(beforeFrameIt);
  }

  this->FdFEModel::addResultFrame(beforeFrame);
}


void FdFEModelKit::deleteResultFrame(int frameIdx)
{
  if (frameIdx < 0)
  {
    // Delete all frames :
    for (ResultsFrame& frame : myResultsFrames) frame.eraseAll();
    std::vector<ResultsFrame> dummy;
    myResultsFrames.swap(dummy);
  }
  else if ((size_t)frameIdx < myResultsFrames.size())
  {
    // Delete a single frame
    std::vector<ResultsFrame>::iterator frameIdxIt = myResultsFrames.begin();
    frameIdxIt += frameIdx;
    myResultsFrames.erase(frameIdxIt);
  }

  this->FdFEModel::deleteResultFrame(frameIdx);
}


bool FdFEModelKit::hasResultTransform(unsigned int frameIdx)
{
  if (myResultsFrames.size() > frameIdx)
    return myResultsFrames[frameIdx].mx ? true : false;
  else
    return false;
}


void FdFEModelKit::setResultTransform(unsigned int frameIdx, const FaMat34& pos)
{
  if (frameIdx >= myResultsFrames.size())
    myResultsFrames.resize(frameIdx+1);

  if (myResultsFrames[frameIdx].mx)
    *myResultsFrames[frameIdx].mx = pos;
  else
    myResultsFrames[frameIdx].mx = new FaMat34(pos);
}


bool FdFEModelKit::hasResultDeformation(unsigned int frameIdx)
{
  if (myResultsFrames.size() > frameIdx)
    return !myResultsFrames[frameIdx].deformation.empty();
  else
    return false;
}


/*!
  Sets the deformation of a frame.
  On return the input vector contains the old deformation.
*/

void FdFEModelKit::setResultDeformation(unsigned int frameIdx, const VertexVec& defs)
{
  this->expandFrameArrayIfNeccesary(frameIdx);

  myResultsFrames[frameIdx].deformation.resize(defs.size());
  for (size_t vxIdx = 0; vxIdx < defs.size(); vxIdx++) {
    Vec3f& vPt = myResultsFrames[frameIdx].deformation[vxIdx];
    vPt[0] = (float)defs[vxIdx][0];
    vPt[1] = (float)defs[vxIdx][1];
    vPt[2] = (float)defs[vxIdx][2];
  }

  if (!myResultsFrames[frameIdx].vxProp) {
    myResultsFrames[frameIdx].vxProp = new SoVertexProperty;
    myResultsFrames[frameIdx].vxProp->ref();
  }

  this->updateResultVertexes(myResultsFrames[frameIdx]);
}


void FdFEModelKit::updateResultVertexes(const ResultsFrame& frame)
{
  if (!frame.vxProp) return;

  frame.vxProp->vertex.setNum(myVertexes->vertex.getNum());

  SbVec3f* frmVxSbVec = frame.vxProp->vertex.startEditing();

  long int vxIdx = 0;
  for (const Vec3f& def : frame.deformation)
    if (vxIdx >= myVertexes->vertex.getNum())
      break;
    else
    {
      frmVxSbVec->setValue(def.data());
      (*frmVxSbVec) *= myDeformationScale;
      (*frmVxSbVec) += myVertexes->vertex[vxIdx];
      vxIdx++, frmVxSbVec++;
    }

  frame.vxProp->vertex.finishEditing();
}


void FdFEModelKit::setDeformationScale(float scale)
{
  myDeformationScale = scale;

  for (const ResultsFrame& frame : myResultsFrames)
    this->updateResultVertexes(frame);
}


void FdFEModelKit::expandFrameArrayIfNeccesary(int frameIdx)
{
  if (frameIdx >= (int)myResultsFrames.size())
    myResultsFrames.resize(frameIdx+1);

  this->FdFEModel::expandFrameArrayIfNeccesary(frameIdx);
}


void FdFEModelKit::ResultsFrame::eraseVxProp()
{
  if (!vxProp)
    return;

  vxProp->vertex.deleteValues(0,-1);
  vxProp->orderedRGBA.deleteValues(0,-1);
  vxProp->unref();
  vxProp = NULL;
}

void FdFEModelKit::ResultsFrame::eraseDef()
{
  std::vector<Vec3f> empty;
  deformation.swap(empty);
}
