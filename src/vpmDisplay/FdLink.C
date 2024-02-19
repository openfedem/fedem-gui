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
#include "FFdCadModel/FdCadHandler.H"
#include "FFdCadModel/FdCadSolid.H"
#include "FFdCadModel/FdCadSolidWire.H"
#include "FFdCadModel/FdCadFace.H"

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

#include "FFdCadModel/FdCadFace.H"

#ifdef USE_OPENCASCADE
#include "SoBrepShape.h"
#endif

#include <fstream>


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
  itsKit->setPart("symbolMaterial", NULL);
  SoMaterial* symbolMaterial = SO_GET_PART(itsKit, "symbolMaterial", SoMaterial);
  symbolMaterial->diffuseColor.setValue(FdConverter::toSbVec3f(Link->getRGBColor()));
  symbolMaterial->ambientColor.setValue(FdConverter::toSbVec3f(Link->getRGBColor()));
  symbolMaterial->emissiveColor.setValue(FdConverter::toSbVec3f(Link->getRGBColor()));

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

    // Delete Old visualization

    this->removeVisualizationData();

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

  SoSeparator* vrmlSep = NULL;
  switch (FdDB::getCadFileType(fileName))
    {
    case FdDB::FD_VRML_FILE:
      {
	SoInput soFile;
	if (soFile.openFile(fileName.c_str()))
	  vrmlSep = SoDB::readAll(&soFile);
      }
      break;

    case FdDB::FD_BREP_FILE:
    case FdDB::FD_STEP_FILE:
    case FdDB::FD_IGES_FILE:
      {
#ifdef USE_OPENCASCADE
	SoBrepShape reader;
	reader.SetFile(fileName.c_str());
	vrmlSep = new SoSeparator;
	vrmlSep->ref();
	if (!reader.Compute(vrmlSep)) {
	  vrmlSep->unref();
	  vrmlSep = NULL;
	}
#endif
      }
      break;

    case FdDB::FD_FCAD_FILE:
      {
	std::ifstream in(fileName.c_str(), std::ios::in);
	if (myCadHandler->read(in))
	  if (this->createCadViz()) {
	    FFaMsg::list(" OK.\n");
	    IHaveLoadedVrmlViz = true;
	    return true;
	  }
      }
	case FdDB::FD_OBJ_FILE:
	{
		//TODO(Runar): Add parsing of material info

		bool triangles = true;
		std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
		std::vector< FaVec3 >* vertices = new std::vector<FaVec3>();
		std::vector< FaVec3 >* uvs =   new std::vector<FaVec3>();
		std::vector< FaVec3 >* normals = new std::vector<FaVec3>();

		FILE * file = fopen(fileName.c_str(), "r");


		int numGroups = 0;
		std::vector<std::tuple<std::string, long>> geometryGroups;

		while (1){

			char lineHeader[256];
			// read the first word of the line
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.

			if (strcmp(lineHeader, "v") == 0){
				float x, y, z;
				fscanf(file, "%f %f %f\n", &x, &y, &z);
				FaVec3 vertex(x, y, z);
				vertices->push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0){
				float x, y;
				fscanf(file, "%f %f\n", &x, &y);
				FaVec3 uv(x, y, 0);
				uvs->push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0){
				float x, y, z;
				fscanf(file, "%f %f %f\n", &x, &y, &z);
				FaVec3 normal(x, y, z);
				normals->push_back(normal);
			}
			else if (strcmp(lineHeader, "g") == 0){
				long pos = ftell(file);
				//TODO(runar): Fix this. Parse line in better way
				fscanf(file, "%s", lineHeader);
				geometryGroups.push_back(std::make_tuple(lineHeader, pos));
				fseek(file, pos, SEEK_SET);
				numGroups++;
			}
		}

		int allGroups = 0;
		int groupId = link->objFileGroupIndex.getValue();

        if (groupId == numGroups)
            allGroups = 1;

		if (!(groupId >= 0) || groupId > numGroups)
		{

			if (numGroups > 1){
				//Import all groups?
				allGroups = FFaMsg::dialog("Multiple geometry groups in obj-file. Import all?", FFaMsg::FFaDialogType::YES_NO);

				if (!allGroups){
					std::vector<std::string> buttonText;
					std::vector<std::string> selectionList;
					for (int k = 0; k < numGroups; k++)
					{
						selectionList.push_back(std::to_string(k));
						//selectionList.push_back(std::get<0>(geometryGroups[groupId]));
					}
					buttonText.push_back("Select");
					FFaMsg::dialog(groupId, "Multiple geometry groups in obj file. Please select group", FFaMsg::FFaDialogType::GENERIC, buttonText,
						selectionList);

					link->objFileGroupIndex.setValue(groupId);
				}
                else
                    link->objFileGroupIndex.setValue(numGroups);
			}
			else if (numGroups == 1)
			{
				link->objFileGroupIndex.setValue(0);
				groupId = 0;
			}
		}

		if (allGroups)
		{
			fseek(file, std::get<1>(geometryGroups[0]), SEEK_SET);
		}
		else if (numGroups > 0)
		{
			fseek(file, std::get<1>(geometryGroups[groupId]), SEEK_SET);
		}
		else
		{
			fseek(file,0, SEEK_SET);
		}

    auto getIntsFromString = [](const std::string& input)
    {
      std::vector<int> foundInts;
      std::string activeInt;
      for (char c : input)
        if (c == '#')//comment
          break;
        else if (isdigit(c))
          activeInt += c;
        else if (c == '-')
          activeInt += c;
        else if (c == '+')
          activeInt += c;
        else
        {
          if (activeInt.length() > 0)
            foundInts.push_back(atoi(activeInt.c_str()));

          activeInt = "";
        }

      //Add final number
      if (activeInt.length() > 0)
        foundInts.push_back(atoi(activeInt.c_str()));

      return foundInts;
    };

		while (1){

			char lineHeader[256];
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break;

			if (strcmp(lineHeader, "f") == 0){
				//TODO(Runar): Handle both triangles and quads
				if (triangles)
				{
                    fscanf(file,"%[^\n]", lineHeader);

                    std::vector<int> ints = getIntsFromString(std::string(lineHeader));
                    int matches = ints.size();

                    if (matches == 6)
                    {
                        vertexIndices.push_back(ints[0]);
                        vertexIndices.push_back(ints[2]);
                        vertexIndices.push_back(ints[4]);

                        normalIndices.push_back(ints[1]);
                        normalIndices.push_back(ints[3]);
                        normalIndices.push_back(ints[5]);
                    }
                    else if (matches == 9) 
                    {
                        vertexIndices.push_back(ints[0]);
                        vertexIndices.push_back(ints[3]);
                        vertexIndices.push_back(ints[6]);
                        uvIndices.push_back(ints[1]);
                        uvIndices.push_back(ints[4]);
                        uvIndices.push_back(ints[7]);
                        normalIndices.push_back(ints[2]);
                        normalIndices.push_back(ints[5]);
                        normalIndices.push_back(ints[8]);
                    }
                    else
                    {
                        FFaMsg::list("Could not parse obj-file. Try exporting faces as triangles.\n");
                        return false;
                    }
				}
				else
				{
                    fscanf(file, "%[^\n]", lineHeader);

                    std::vector<int> ints = getIntsFromString(std::string(lineHeader));
                    int matches = ints.size();

                    if (matches == 8)
                    {
                        vertexIndices.push_back(ints[0]);
                        vertexIndices.push_back(ints[2]);
                        vertexIndices.push_back(ints[4]);
                        vertexIndices.push_back(ints[6]);

                        normalIndices.push_back(ints[1]);
                        normalIndices.push_back(ints[3]);
                        normalIndices.push_back(ints[5]);
                        normalIndices.push_back(ints[7]);
                    }
                    else if (matches == 12)
                    {
                        vertexIndices.push_back(ints[0]);
                        vertexIndices.push_back(ints[3]);
                        vertexIndices.push_back(ints[6]);
                        vertexIndices.push_back(ints[9]);
                        uvIndices.push_back(ints[1]);
                        uvIndices.push_back(ints[4]);
                        uvIndices.push_back(ints[7]);
                        uvIndices.push_back(ints[10]);
                        normalIndices.push_back(ints[2]);
                        normalIndices.push_back(ints[5]);
                        normalIndices.push_back(ints[8]);
                        normalIndices.push_back(ints[11]);
                    }
                    else
                        printf("Could not parse obj-file. Try exporting faces as triangles.\n");
                    return false;
				}
			}
			if (!allGroups)
			{
				if (strcmp(lineHeader, "g") == 0){
					break;
				}
			}
		}

		// Clean up
		if (myCadHandler->hasPart() || myCadHandler->hasAssembly())
			myCadHandler->deleteCadData();

		// Get cad part
		FdCadPart* part = myCadHandler->getCadPart();
		if (part == NULL)
			return false; // unexpected


		// Create cad solid and wire representations
		FdCadSolid* body = new FdCadSolid();
		FdCadSolidWire* wire = new FdCadSolidWire();
		part->addSolid(body, wire);

		SoCoordinate3* coords = new SoCoordinate3();
		body->insertChild(coords, 0);
		wire->insertChild(coords, 0);
		coords->point.setNum(vertices->size());
		SbVec3f* coord = coords->point.startEditing();

                // Add points to coordinate-array
                for (size_t i = 0; i < vertices->size(); i++)
                  coord[i].setValue((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z());

		coords->point.finishEditing();

		// Create cad face
		FdCadFace* face = new FdCadFace();
		int *idx = new int[20000000];//TODO(runar): Pull this out as parameter?
		//int idx[65536];
		face->coordIndex.enableNotify(false);
		face->coordIndex.deleteValues(0);
		if (triangles)
		{
			for (size_t i = 0; i < vertexIndices.size()/3; i++){
				idx[i * 4] = vertexIndices[i * 3] - 1;
				idx[i * 4 + 1] = vertexIndices[i * 3 + 1] - 1;
				idx[i * 4 + 2] = vertexIndices[i * 3 + 2] - 1;
				idx[i * 4 + 3] = -1;
			}
			face->coordIndex.setValues(0, vertexIndices.size() + vertexIndices.size() / 3, &(idx[0]));
		}
		else
		{
			for (size_t i = 0; i < vertexIndices.size()/4; i++){
				idx[i * 5] = vertexIndices[i * 4] - 1;
				idx[i * 5 + 1] = vertexIndices[i * 4 + 1] - 1;
				idx[i * 5 + 2] = vertexIndices[i * 4 + 2] - 1;
				idx[i * 5 + 3] = vertexIndices[i * 4 + 3] - 1;
				idx[i * 5 + 4] = -1;
			}
			face->coordIndex.setValues(0, vertexIndices.size() + vertexIndices.size() / 4, &(idx[0]));
		}

		face->coordIndex.enableNotify(true);
		face->coordIndex.touch();
		body->addChild(face);

		// Create cad edge
		FdCadEdge* edge = new FdCadEdge();
		if (triangles)
		{
			for (size_t i = 0; i < vertexIndices.size()/3; i++){
				idx[i * 9] = vertexIndices[i * 3] - 1;
				idx[i * 9 + 1] = vertexIndices[i * 3 + 1] - 1;
				idx[i * 9 + 2] = -1;

				idx[i * 9 + 3] = vertexIndices[i * 3 + 1] - 1;
				idx[i * 9 + 4] = vertexIndices[i * 3 + 2] - 1;
				idx[i * 9 + 5] = -1;

				idx[i * 9 + 6] = vertexIndices[i * 3 + 2] - 1;
				idx[i * 9 + 7] = vertexIndices[i * 3] - 1;
				idx[i * 9 + 8] = -1;
			}
		}
		else
		{
			for (size_t i = 0; i < vertexIndices.size()/4; i++){
				idx[i * 12] = vertexIndices[i * 4] - 1;
				idx[i * 12 + 1] = vertexIndices[i * 4 + 1] - 1;
				idx[i * 12 + 2] = -1;

				idx[i * 12 + 3] = vertexIndices[i * 4 + 1] - 1;
				idx[i * 12 + 4] = vertexIndices[i * 4 + 2] - 1;
				idx[i * 12 + 5] = -1;

				idx[i * 12 + 6] = vertexIndices[i * 4 + 2] - 1;
				idx[i * 12 + 7] = vertexIndices[i * 4 + 3] - 1;
				idx[i * 12 + 8] = -1;

				idx[i * 12 + 9] = vertexIndices[i * 4 + 3] - 1;
				idx[i * 12 + 10] = vertexIndices[i * 4] - 1;
				idx[i * 12 + 11] = -1;
			}
		}

		edge->coordIndex.enableNotify(false);
		edge->coordIndex.deleteValues(0);
		if (triangles)
		{
			edge->coordIndex.setValues(0, vertexIndices.size() * 3, &(idx[0]));
		}
		else
		{
			edge->coordIndex.setValues(0, vertexIndices.size() * 4, &(idx[0]));
		}
		edge->coordIndex.enableNotify(true);
		edge->coordIndex.touch();
		wire->addChild(edge);

		SoSeparator* linesSep = new SoSeparator();
		SoSeparator* facesSep = new SoSeparator();

		facesSep->addChild(body);
		linesSep->addChild(wire);

		IHaveCreatedCadViz = true;
		vrmlSep = facesSep;

		SoSeparator* objRoot = new SoSeparator;
		SoShapeHints* sh = new SoShapeHints;
		SoScale* unitConv = new SoScale;

		objRoot->addChild(sh);
		objRoot->addChild(unitConv);
		objRoot->addChild(vrmlSep);

		sh->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
		sh->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
		sh->creaseAngle = 0.3f;
		double scaleF = 1.0;
		link->visDataFileUnitConverter.getValue().convert(scaleF, "LENGTH");
		unitConv->scaleFactor.setValue(SbVec3f((float)scaleF, (float)scaleF, (float)scaleF));

		((FdFEModelKit*)myFEKit)->addGroupPart(FdFEGroupPartSet::SURFACE_FACES, facesSep);
		//((FdFEModelKit*)myFEKit)->addGroupPart(FdFEGroupPartSet::RED_SURFACE_FACES, facesSep);
		//((FdFEModelKit*)myFEKit)->addGroupPart(FdFEGroupPartSet::OUTLINE_LINES, linesSep);
		((FdFEModelKit*)myFEKit)->addGroupPart(FdFEGroupPartSet::RED_OUTLINE_LINES, linesSep);

		// Touch visualization
		myFEKit->setDrawDetail(FdFEVisControl::OFF);
		myFEKit->setLineDetail(FdFEVisControl::OFF);
		myFEKit->updateVisControl();


		delete vertices;
		delete uvs;
		delete normals;
		delete [] idx;

		FFaMsg::list(" OK.\n");
		IHaveLoadedVrmlViz = true;
		return true;
	}
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
  double scaleF = 1.0;
  link->visDataFileUnitConverter.getValue().convert(scaleF, "LENGTH");
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


bool createCadPartViz(SoSeparator * linesParent, SoSeparator * facesParent, FdCadPart * part, bool isLinkPart)
{
  if (!part)
    return false;

  if (part->myVisProp.isDefined && !isLinkPart) {
    SoMaterial * mat = new SoMaterial();
    mat->ambientColor = FdConverter::toSbVec3f(part->myVisProp.ambientColor);
    mat->specularColor = FdConverter::toSbVec3f(part->myVisProp.specularColor);
    mat->emissiveColor = FdConverter::toSbVec3f(part->myVisProp.emissiveColor);
    mat->diffuseColor = FdConverter::toSbVec3f(part->myVisProp.diffuseColor);
    mat->transparency = part->myVisProp.transparency;
    mat->shininess = part->myVisProp.shininess;

    if (part->size())
      facesParent->addChild(mat);
  }

  for (size_t j = 0; j < part->size(); ++j)
    {
      linesParent->addChild(part->getSolid(j).second);
      facesParent->addChild(part->getSolid(j).first);
    }

  return true;
}


bool createCadAssemblyViz(SoSeparator * linesParent, SoSeparator * facesParent, FdCadAssembly * fAss)
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

  IHaveCreatedCadViz = true;
  return true;
}

void FdLink::updateElementVisibility()
{
  myFEKit->updateElementVisibility();
}


SbVec3f FdLink::findSnapPoint(const SbVec3f& pointOnObject,
			      const SbMatrix& objToWorld,
			      SoDetail* detail, SoPickedPoint* pPoint)
{
  // PointOnObject is the local point, objToWorld is the global system

  SbVec3f nearestWorld;

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
      {
        objToWorld.multVecMatrix(pointOnObject, nearestWorld);
        return nearestWorld;
      }
    }

    if (faceDet)
    {
      if (vrmlCoords || coords || vxProp)
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
	  objToWorld.multVecMatrix(vrmlCoords->point[idx], nearestWorld);
	else if (coords)
	  objToWorld.multVecMatrix(coords->point[idx], nearestWorld);
	else
	  objToWorld.multVecMatrix(vxProp->vertex[idx], nearestWorld);
      }
      else
	objToWorld.multVecMatrix(pointOnObject, nearestWorld);
    }
    else if (lineDet)
    {
      if (vrmlCoords || vxProp)
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
	  objToWorld.multVecMatrix(sbp0, nearestWorld);
	else
	  objToWorld.multVecMatrix(sbp1, nearestWorld);
      }
      else
	objToWorld.multVecMatrix(pointOnObject, nearestWorld);
    }
  }
  else
    objToWorld.multVecMatrix(pointOnObject, nearestWorld);

  return nearestWorld;
}


void FdLink::showHighlight()
{
  if (this->highlightBoxId)
    FdExtraGraphics::removeBBox(this->highlightBoxId);
  this->highlightBoxId = NULL;

  SbBox3f bbox;
  if (IAmUsingGenPartVis) {
    SoNode* n = itsKit->getPart("groupParts", false);
    if (n) {
      SoGetBoundingBoxAction* action = new SoGetBoundingBoxAction(SbViewportRegion());
      action->apply(n);
      SbBox3f box = action->getBoundingBox();
      if (!box.isEmpty())
	bbox.extendBy(box);
    }
  }

  FaVec3 max, min;
  FmLink* link = (FmLink*)this->itsFmOwner;
  if (link->getBBox(max,min)) {
    bbox.extendBy(FdConverter::toSbVec3f(min));
    bbox.extendBy(FdConverter::toSbVec3f(max));
  }

  if (!bbox.isEmpty()){
    min = FdConverter::toFaVec3(bbox.getMin());
    max = FdConverter::toFaVec3(bbox.getMax());
  }
  this->highlightBoxId = FdExtraGraphics::showBBox(min,max,link->getGlobalCS());

  // Highligth CG and CS too, if in a generic part
  if (FdDB::isUsingLineHighlight())
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

  IHaveLoadedVrmlViz = false;
  IAmUsingGenPartVis = false;
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

  IHaveLoadedVrmlViz = false;
  IAmUsingGenPartVis = false;
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
