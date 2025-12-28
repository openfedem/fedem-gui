// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdLink.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdExtraGraphics.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDisplay/FdPickFilter.H"
#include "vpmDisplay/FdFEModelKit.H"
#include "vpmDisplay/FdObjParser.H"
#include "FFdCadModel/FdCadHandler.H"
#include "FFdCadModel/FdCadSolid.H"
#include "FFdCadModel/FdCadSolidWire.H"
#include "FFdCadModel/FdCadFace.H"
#include "FFdCadModel/FdCadEdge.H"

#include "vpmDB/FmLink.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmFileSys.H"

#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"

#include <Inventor/details/SoLineDetail.h>
#include <Inventor/details/SoFaceDetail.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/VRMLnodes/SoVRMLCoordinate.h>
#include <Inventor/VRMLnodes/SoVRMLVertexShape.h>

#ifdef USE_OPENCASCADE
#include "SoBrepShape.h"
#endif

#include <fstream>


namespace
{
  // Wavefront OBJ model parser.
  SoSeparator* loadObjFile(const char* fName, float scale, int& groupId,
                           FdCadHandler* cad, FdFEModelKit* feKit)
  {
    FdObjParser obj(fName,groupId);
    if (obj.nFace < 1) return NULL; // parse failure

    // Clean up
    if (cad->hasPart() || cad->hasAssembly())
      cad->deleteCadData();

    // Get cad part
    FdCadPart* part = cad->getCadPart();
    if (part == NULL) return NULL; // unexpected

    groupId = obj.groupId;

    // Create cad solid and wire representations
    FdCadSolid* body = new FdCadSolid();
    FdCadSolidWire* wire = new FdCadSolidWire();
    part->addSolid(body, wire);

    SoCoordinate3* coords = new SoCoordinate3();
    body->insertChild(coords,0);
    wire->insertChild(coords,0);
    coords->point.setNum(obj.vertices.size());

    // Add points to coordinate-array
    size_t i = 0;
    SbVec3f* coord = coords->point.startEditing();
    for (const FdObjParser::XYZ& vertex : obj.vertices)
      coord[i++].setValue(vertex[0],vertex[1],vertex[2]);
    coords->point.finishEditing();

    // Create CAD face
    FdCadFace* face = new FdCadFace();
    face->coordIndex.enableNotify(false);
    face->coordIndex.deleteValues(0);
    face->coordIndex.setValues(0,obj.vertexIndices.size(),
                               obj.vertexIndices.data());
    face->coordIndex.enableNotify(true);
    face->coordIndex.touch();
    body->addChild(face);

    SoSeparator* facesSep = new SoSeparator();
    facesSep->addChild(body);

    // Create CAD edge
    std::vector<int> idx;
    idx.reserve(obj.nFace*12);
    FdCadEdge* edge = new FdCadEdge();
    for (size_t j = i = 0; i < obj.nFace; i++, j++)
      if (j+3 < obj.vertexIndices.size() && obj.vertexIndices[j+3] == -1)
      {
        idx.push_back(obj.vertexIndices[j  ]);
        idx.push_back(obj.vertexIndices[j+1]);
        idx.push_back(-1);

        idx.push_back(obj.vertexIndices[j+1]);
        idx.push_back(obj.vertexIndices[j+2]);
        idx.push_back(-1);

        idx.push_back(obj.vertexIndices[j+2]);
        idx.push_back(obj.vertexIndices[j  ]);
        idx.push_back(-1);

        j += 3;
      }
      else if (j+4 < obj.vertexIndices.size() && obj.vertexIndices[j+4] == -1)
      {
        idx.push_back(obj.vertexIndices[j  ]);
        idx.push_back(obj.vertexIndices[j+1]);
        idx.push_back(-1);

        idx.push_back(obj.vertexIndices[j+1]);
        idx.push_back(obj.vertexIndices[j+2]);
        idx.push_back(-1);

        idx.push_back(obj.vertexIndices[j+2]);
        idx.push_back(obj.vertexIndices[j+3]);
        idx.push_back(-1);

        idx.push_back(obj.vertexIndices[j+3]);
        idx.push_back(obj.vertexIndices[j  ]);
        idx.push_back(-1);

        j += 4;
      }

    edge->coordIndex.enableNotify(false);
    edge->coordIndex.deleteValues(0);
    edge->coordIndex.setValues(0,idx.size(),idx.data());
    edge->coordIndex.enableNotify(true);
    edge->coordIndex.touch();
    wire->addChild(edge);

    SoSeparator* linesSep = new SoSeparator();
    linesSep->addChild(wire);

    SoShapeHints* sh = new SoShapeHints;
    sh->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    sh->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    sh->creaseAngle = 0.3f;

    SoScale* unitConv = new SoScale;
    unitConv->scaleFactor.setValue(SbVec3f(scale,scale,scale));

    SoSeparator* objRoot = new SoSeparator;
    objRoot->addChild(sh);
    objRoot->addChild(unitConv);
    objRoot->addChild(facesSep);

    feKit->addGroupPart(FdFEGroupPartSet::SURFACE_FACES,facesSep);
    //feKit->addGroupPart(FdFEGroupPartSet::RED_SURFACE_FACES,facesSep);
    //feKit->addGroupPart(FdFEGroupPartSet::OUTLINE_LINES,linesSep);
    feKit->addGroupPart(FdFEGroupPartSet::RED_OUTLINE_LINES,linesSep);

    return facesSep;
  }


  bool createCadPartViz(SoSeparator* linesParent, SoSeparator* facesParent,
                        FdCadPart* part, bool isLinkPart)
  {
    if (!part)
      return false;
    else if (part->size() < 1)
      return true;

    if (part->myVisProp.isDefined && !isLinkPart)
    {
      SoMaterial* m = new SoMaterial();
      m->ambientColor = FdConverter::toSbVec3f(part->myVisProp.ambientColor);
      m->specularColor = FdConverter::toSbVec3f(part->myVisProp.specularColor);
      m->emissiveColor = FdConverter::toSbVec3f(part->myVisProp.emissiveColor);
      m->diffuseColor = FdConverter::toSbVec3f(part->myVisProp.diffuseColor);
      m->transparency = part->myVisProp.transparency;
      m->shininess = part->myVisProp.shininess;
      facesParent->addChild(m);
    }

    for (size_t j = 0; j < part->size(); j++)
    {
      linesParent->addChild(part->getSolid(j).second);
      facesParent->addChild(part->getSolid(j).first);
    }

    return true;
  }


  bool createCadAssemblyViz(SoSeparator* linesParent, SoSeparator* facesParent,
                            FdCadAssembly* fAss)
  {
    if (!fAss || !linesParent || !facesParent)
      return false;

    for (FdCadComponent* cmp : fAss->myComponents)
    {
      SoTransform* xf   = new SoTransform();
      SoSeparator* lsep = new SoSeparator();
      SoSeparator* fsep = new SoSeparator();

      xf->setMatrix(FdConverter::toSbMatrix(cmp->myPartCS));
      linesParent->addChild(lsep);
      facesParent->addChild(fsep);
      lsep->addChild(xf);
      fsep->addChild(xf);

      if (!createCadAssemblyViz(lsep, fsep, dynamic_cast<FdCadAssembly*>(cmp)))
        if (!createCadPartViz(lsep, fsep, dynamic_cast<FdCadPart*>(cmp), false))
          return false;
    }

    return true;
  }
}


/**********************************************************************
 *
 * CLASS FdLink
 *
 **********************************************************************/

Fmd_SOURCE_INIT(FDLINK,FdLink,FdObject);


FdLink::FdLink(FmLink* pt) : FdObject()
{
  Fmd_CONSTRUCTOR_INIT(FdLink);

  this->itsFmOwner = pt;
  FdFEModelKit* FEKit = new FdFEModelKit;
  myFEKit = FEKit;
  itsKit = FEKit;
  itsKit->ref();

  myCadHandler = new FdCadHandler();

  // Set up backpointer

  myFEKit->setFdPointer(this);

  IAmUsingGenPartVis = false;
  IHaveLoadedVrmlViz = false;
  IHaveCreatedCadViz = false;

  this->highlightBoxId = NULL;
}


FdLink::~FdLink()
{
  this->hideHighlight();
  this->fdDisconnect();
  itsKit->unref();
  delete myCadHandler;
}


bool FdLink::updateFdApperance()
{
  FmLink* Link = (FmLink*)this->itsFmOwner;
#ifdef FD_DEBUG
  std::cout <<"FdLink::updateFdAppearance() "<< Link->getIdString(true)
            <<"\n\tColor        = "<< Link->getRGBColor()
            <<"\n\tTransparancy = "<< Link->getTransparency()
            <<"\n\tShininess    = "<< Link->getShininess() << std::endl;
#endif

  FFdLook look(Link->getRGBColor(),0.5f);
  look.transparency = (float)Link->getTransparency();
  look.shininess    = (float)Link->getShininess();
  myFEKit->setLook(look);

  // Link coordinate system symbol color
  itsKit->setPart("symbolMaterial",NULL);
  SoMaterial* mat = SO_GET_PART(itsKit,"symbolMaterial",SoMaterial);
  mat->diffuseColor.setValue(FdConverter::toSbVec3f(Link->getRGBColor()));
  mat->ambientColor.setValue(FdConverter::toSbVec3f(Link->getRGBColor()));
  mat->emissiveColor.setValue(FdConverter::toSbVec3f(Link->getRGBColor()));

  return true;
}


bool FdLink::updateFdCS()
{
#ifdef FD_DEBUG
  std::cout <<"FdLink::updateFdCS() "
            << itsFmOwner->getIdString(true) << std::endl;
#endif
  myFEKit->setTransform(((FmLink*)this->itsFmOwner)->getGlobalCS());
  this->updateSimplifiedViz();
  this->updateFdHighlight();
  return true;
}


bool FdLink::updateFdTopology(bool updateChildrenDisplay)
{
#ifdef FD_DEBUG
  std::cout <<"\n\nFdLink::updateFdTopology() "
            << itsFmOwner->getIdString(true) << std::endl;
#endif
  this->updateFdCS();

  /* Recursive update of the display topology of the
  // enteties affected by this entety:
  //              Axial Spring/Damper
  //            /
  // Link->Triad->Joint->HP
  //            \
  //              Load
  */

  if (updateChildrenDisplay)
    this->itsFmOwner->updateChildrenDisplayTopology();

  return true;
}


bool FdLink::updateFdAll(bool updateChildrenDisplay)
{
#ifdef FD_DEBUG
  std::cout <<"FdLink::updateFdAll() "
            << itsFmOwner->getIdString(true) << std::endl;
#endif
  FmLink* link = (FmLink*)this->itsFmOwner;

  if (!this->isInserted) {
    this->getListSw()->addChild(itsKit);
    this->isInserted = true;
  }

  if (!link->isEarthLink()) {

    // Delete old visualization

    this->removeVisualizationData(false);

    // Setting the coordinate system symbol:

    itsKit->setPart("symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::LINK_COORD_SYS));
  }

  // Update everything else

  if (!this->updateFdDetails()) return false;
  if (!this->updateFdApperance()) return false;
  if (!this->updateFdTopology(updateChildrenDisplay)) return false;

  this->updateFdHighlight();
  return true;
}


bool FdLink::loadVrmlViz()
{
  if (IHaveLoadedVrmlViz)
    return true;

  FmLink* link = (FmLink*)this->itsFmOwner;
  std::string fileName = link->visDataFile.getValue();
  if (fileName.empty())
    return false;

  FFaFilePath::makeItAbsolute(fileName,FmDB::getMechanismObject()->getAbsModelFilePath());
  if (!FmFileSys::isFile(fileName)) {
    FFaMsg::list("  -> Error: Could not find visualization data file \"" + fileName +
		 "\"\n             referenced by " + link->getIdString(true) + "\n");
    return false;
  }

  FFaMsg::list("  -> Reading visualization data for " + link->getIdString(true) +
	       "\n     from file \"" + fileName + "\" ... ");

  double scaleF = 1.0;
  link->visDataFileUnitConverter.getValue().convert(scaleF, "LENGTH");

  SoSeparator* vrmlSep = NULL;
  switch (FdDB::getCadFileType(fileName))
    {
    case FdDB::FD_VRML_FILE:
      if (SoInput soFile; soFile.openFile(fileName.c_str()))
        vrmlSep = SoDB::readAll(&soFile);
      break;

    case FdDB::FD_BREP_FILE:
    case FdDB::FD_STEP_FILE:
    case FdDB::FD_IGES_FILE:
      {
#ifdef USE_OPENCASCADE
	SoBrepShape reader;
	reader.SetFile(fileName.c_str());
	SoSeparator* tmpSep = new SoSeparator;
	tmpSep->ref();
	if (reader.Compute(tmpSep))
	  vrmlSep = tmpSep;
	else
	  tmpSep->unref();
#endif
      }
      break;

    case FdDB::FD_FCAD_FILE:
      if (std::ifstream in(fileName.c_str(),std::ios::in);
          myCadHandler->read(in) && this->createCadViz())
      {
        FFaMsg::list("OK.\n");
        return true;
      }
      break;

    case FdDB::FD_OBJ_FILE:
      if ((vrmlSep = loadObjFile(fileName.c_str(), scaleF,
                                 link->objFileGroupIndex.getValue(),
                                 myCadHandler, (FdFEModelKit*)myFEKit)))
      {
        // Touch visualization
        myFEKit->setDrawDetail(FdFEVisControl::OFF);
        myFEKit->setLineDetail(FdFEVisControl::OFF);
        myFEKit->updateVisControl();
        FFaMsg::list("OK.\n");
        IHaveLoadedVrmlViz = true;
        IHaveCreatedCadViz = true;
        return true;
      }
      break;

    default:
      break;
    }

  if (!vrmlSep) {
    FFaMsg::list("Failed !\n     Visualization data could not be read.\n");
    return false;
  }

  SoSeparator* vrmlRoot = new SoSeparator;
  SoShapeHints* sh = new SoShapeHints;
  SoScale* unitConv = new SoScale;

  vrmlRoot->addChild(sh);
  vrmlRoot->addChild(unitConv);
  vrmlRoot->addChild(vrmlSep);

  sh->shapeType      = SoShapeHints::UNKNOWN_SHAPE_TYPE;
  sh->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  sh->creaseAngle    = 0.3f;
  unitConv->scaleFactor.setValue(SbVec3f((float)scaleF, (float)scaleF, (float)scaleF));

  ((FdFEModelKit*)myFEKit)->addGroupPart(FdFEGroupPartSet::SURFACE_FACES,vrmlRoot);
  ((FdFEModelKit*)myFEKit)->addGroupPart(FdFEGroupPartSet::RED_SURFACE_FACES,vrmlRoot);

  // Touch visualization
  myFEKit->setDrawDetail(FdFEVisControl::OFF);
  myFEKit->setLineDetail(FdFEVisControl::OFF);
  myFEKit->updateVisControl();

  FFaMsg::list(" OK.\n");
  IHaveLoadedVrmlViz = true;
  return true;
}


bool FdLink::createCadViz()
{
  if (!myCadHandler->getCadComponent())
    return false;

  if (IHaveCreatedCadViz)
    return true;

  SoSeparator* linesSep = new SoSeparator();
  SoSeparator* facesSep = new SoSeparator();

  if (myCadHandler->hasPart())
    createCadPartViz(linesSep, facesSep, myCadHandler->getCadPart(), true);
  else if (myCadHandler->hasAssembly())
    createCadAssemblyViz(linesSep, facesSep, myCadHandler->getCadAssembly());

  ((FdFEModelKit*)myFEKit)->addGroupPart(FdFEGroupPartSet::SURFACE_FACES, facesSep);
  ((FdFEModelKit*)myFEKit)->addGroupPart(FdFEGroupPartSet::RED_SURFACE_FACES, facesSep);
  ((FdFEModelKit*)myFEKit)->addGroupPart(FdFEGroupPartSet::OUTLINE_LINES, linesSep);
  ((FdFEModelKit*)myFEKit)->addGroupPart(FdFEGroupPartSet::RED_OUTLINE_LINES, linesSep);

  myFEKit->setDrawDetail(FdFEVisControl::OFF);
  myFEKit->setLineDetail(FdFEVisControl::OFF);
  myFEKit->updateVisControl();

  IHaveLoadedVrmlViz = true;
  IHaveCreatedCadViz = true;
  return true;
}

void FdLink::updateElementVisibility()
{
  myFEKit->updateElementVisibility();
}


FaVec3 FdLink::findSnapPoint(const SbVec3f& pointOnObject,
                             const SbMatrix& objToWorld,
                             SoDetail* detail, SoPickedPoint* pPoint)
{
  // PointOnObject is the local point, objToWorld is the global system

  SoFaceDetail* faceDet = NULL;
  SoLineDetail* lineDet = NULL;
  if (detail->isOfType(SoFaceDetail::getClassTypeId()))
    faceDet = (SoFaceDetail*)detail;
  else if (detail->isOfType(SoLineDetail::getClassTypeId()))
    lineDet = (SoLineDetail*)detail;

  if (faceDet || lineDet)
  {
    SoFullPath*       path = NULL;
    SoVRMLCoordinate* vrmlCoords = NULL;
    SoCoordinate3*    coords = NULL;
    SoVertexProperty* vxProp = NULL;

    if (pPoint)
    {
      path = (SoFullPath*) pPoint->getPath();
      vrmlCoords = FdPickFilter::findLastVRMLCoordNode(path);
      coords = FdPickFilter::findLastCoordNode(path);
      vxProp = FdPickFilter::findLastVxPropNode(path);
    }

    if (!vrmlCoords && !coords && vxProp && vxProp->vertex.getNum() == 0)
    {
      if (path->getTail()->isOfType(SoVRMLVertexShape::getClassTypeId()))
      {
        SoVRMLVertexShape* vrmlVertShape = (SoVRMLVertexShape*)path->getTail();
        if (vrmlVertShape->coord.isOfType(SoVRMLCoordinate::getClassTypeId()))
          vrmlCoords = (SoVRMLCoordinate*)&vrmlVertShape->coord;
        vxProp = NULL;
      }
      else if (path->getTail()->isOfType(SoVertexShape::getClassTypeId()))
      {
        SoVertexShape* vertShape = (SoVertexShape*)path->getTail();
        if (vertShape->vertexProperty.isOfType(SoVertexShape::getClassTypeId()))
          vxProp = (SoVertexProperty*)&vertShape->vertexProperty;
        if (vxProp->vertex.getNum() == 0)
          vxProp = NULL;
      }
      else
	return this->FdObject::findSnapPoint(pointOnObject,objToWorld);
    }

    if (faceDet && (vrmlCoords || coords || vxProp))
    {
      SbVec3f sbp;
      float length = 0.0f;
      int idx = 0;
      int n = faceDet->getNumPoints();
      for (int i=0; i<n; i++)
      {
        int temp = faceDet->getPoints()[i].getCoordinateIndex();
        if (vrmlCoords)
          sbp = vrmlCoords->point[temp];
        else if (coords)
          sbp = coords->point[temp];
        else
          sbp = vxProp->vertex[temp];
        float l = (sbp - pointOnObject).length();
        if (i == 0 || l < length)
        {
          length = l;
          idx = temp;
        }
      }

      if (vrmlCoords)
        return this->FdObject::findSnapPoint(vrmlCoords->point[idx],objToWorld);
      else if (coords)
        return this->FdObject::findSnapPoint(coords->point[idx],objToWorld);
      else
        return this->FdObject::findSnapPoint(vxProp->vertex[idx],objToWorld);
    }
    else if (lineDet && (vrmlCoords || vxProp))
    {
      int cordIdx0 = lineDet->getPoint0()->getCoordinateIndex();
      int cordIdx1 = lineDet->getPoint1()->getCoordinateIndex();

      SbVec3f sbp0,sbp1;
      if (vrmlCoords) {
        sbp0 = vrmlCoords->point[cordIdx0];
        sbp1 = vrmlCoords->point[cordIdx1];
      }
      else if (coords) {
        sbp0 = coords->point[cordIdx0];
        sbp1 = coords->point[cordIdx1];
      }
      else {
        sbp0 = vxProp->vertex[cordIdx0];
        sbp1 = vxProp->vertex[cordIdx1];
      }

      SbVec3f dist0 = sbp0 - pointOnObject;
      SbVec3f dist1 = sbp1 - pointOnObject;
      if (dist0.length() < dist1.length())
	return this->FdObject::findSnapPoint(sbp0,objToWorld);
      else
        return this->FdObject::findSnapPoint(sbp1,objToWorld);
    }
  }

  return this->FdObject::findSnapPoint(pointOnObject,objToWorld);
}


void FdLink::showHighlight()
{
  if (this->highlightBoxId)
    FdExtraGraphics::removeBBox(this->highlightBoxId);
  this->highlightBoxId = NULL;

  SbBox3f bbox;
  if (IAmUsingGenPartVis)
    if (SoNode* n = itsKit->getPart("groupParts",false); n) {
      SoGetBoundingBoxAction* action = new SoGetBoundingBoxAction(SbViewportRegion());
      action->apply(n);
      SbBox3f box = action->getBoundingBox();
      if (!box.isEmpty())
	bbox.extendBy(box);
    }

  FaVec3 max, min;
  FmLink* link = (FmLink*)this->itsFmOwner;
  if (link->getBBox(max,min)) {
    bbox.extendBy(FdConverter::toSbVec3f(min));
    bbox.extendBy(FdConverter::toSbVec3f(max));
  }

  if (!bbox.isEmpty()) {
    min = FdConverter::toFaVec3(bbox.getMin());
    max = FdConverter::toFaVec3(bbox.getMax());
  }
  this->highlightBoxId = FdExtraGraphics::showBBox(min,max,link->getGlobalCS());

  // Highligth CG and CS too, if in a generic part
  if (FdDB::usesLineHighlight)
    itsKit->setPart("symbolMaterial",FdSymbolDefs::getHighlightMaterial());
}


void FdLink::hideHighlight()
{
  if (this->highlightBoxId)
    FdExtraGraphics::removeBBox(this->highlightBoxId);
  this->highlightBoxId = NULL;

  myFEKit->highlight(false);

  // Link coordinate system symbol color :

  itsKit->setPart("symbolMaterial",NULL);

  SoMaterial* symbolMaterial = SO_GET_PART(itsKit, "symbolMaterial", SoMaterial);
  FmLink* Link = (FmLink*)this->itsFmOwner;

  symbolMaterial->diffuseColor.setValue(FdConverter::toSbVec3f(Link->getRGBColor()));
  symbolMaterial->ambientColor.setValue(FdConverter::toSbVec3f(Link->getRGBColor()));
  symbolMaterial->emissiveColor.setValue(FdConverter::toSbVec3f(Link->getRGBColor()));
}


void FdLink::showCS(bool doShow)
{
  myFEKit->setLinkCSToggle(doShow);
}

void FdLink::showCoGCS(bool doShow)
{
  myFEKit->setCoGCSToggle(doShow);
}

void FdLink::showLinkMotion(bool doShow)
{
  myFEKit->showTransformResults(doShow);
}

void FdLink::showDeformations(bool doShow)
{
  myFEKit->showVertexResults(doShow);
}

void FdLink::setDeformationScale(double scale)
{
  myFEKit->setDeformationScale((float)scale);
}

void FdLink::showFringes(bool doShow)
{
  myFEKit->showColorResults(doShow);
}

void FdLink::setFringeLegendMapping(const FFaLegendMapper& mapping)
{
  myFEKit->setFringeLegendMapping(mapping);
}

void FdLink::initAnimation()
{
  myFEKit->showResults(true);
}

void FdLink::selectAnimationFrame(size_t frameNr)
{
  myFEKit->selectResultFrame(frameNr);
}

void FdLink::resetAnimation()
{
  myFEKit->selectResultFrame(0);
  myFEKit->showResults(false);
}

void FdLink::deleteAnimationData()
{
  resetAnimation();
  myFEKit->deleteResultFrame(-1);
}

FaMat34 FdLink::getActiveTransform() const
{
  return myFEKit->getActiveTransform();
}


/*!
  Removes all visualization data by removing the inventor kit completly.
*/

void FdLink::removeDisplayData()
{
  this->hideHighlight();
  this->fdDisconnect();
  itsKit->unref();

  FdFEModelKit* FEKit = new FdFEModelKit;
  myFEKit = FEKit;
  itsKit = FEKit;
  itsKit->ref();
  myFEKit->setFdPointer(this);

  myCadHandler->deleteCadData();

  IAmUsingGenPartVis = false;
  IHaveLoadedVrmlViz = false;
  IHaveCreatedCadViz = false;
}


/*!
  Removes all visualization data by removing the data from the inventor kit.
  It retains the inventor kits, and the rigid body frames if present.
*/

void FdLink::removeVisualizationData(bool removeCadDataToo)
{
  this->hideHighlight();
  myFEKit->deleteVisualization();

  if (removeCadDataToo)
    myCadHandler->deleteCadData();

  IAmUsingGenPartVis = false;
  IHaveLoadedVrmlViz = false;
  IHaveCreatedCadViz = false;
}


FdCadComponent* FdLink::getCadComponent() const
{
  return myCadHandler->getCadComponent();
}


void FdLink::writeCad(std::ostream& out)
{
  myCadHandler->write(out);
}


bool FdLink::readCad(std::istream& in)
{
  return myCadHandler->read(in);
}


bool FdLink::getGenPartBoundingBox(FaVec3& max, FaVec3& min) const
{
  if (!IAmUsingGenPartVis) return false;

  SoNode* n = itsKit->getPart("groupParts",false);
  if (!n) return false;

  SoGetBoundingBoxAction* action = new SoGetBoundingBoxAction(SbViewportRegion());
  action->apply(n);
  SbBox3f bbox = action->getBoundingBox();
  if (bbox.isEmpty()) return false;

  max = FdConverter::toFaVec3(bbox.getMax());
  min = FdConverter::toFaVec3(bbox.getMin());
  return true;
}
