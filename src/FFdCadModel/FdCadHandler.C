// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <array>
#include <limits>
#include <string>
#include <cmath>

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCoordinate3.h>

#include "FFdCadModel/FdCadFace.H"
#include "FFdCadModel/FdCadEdge.H"
#include "FFdCadModel/FdCadSolid.H"
#include "FFdCadModel/FdCadSolidWire.H"
#include "FFdCadModel/FdCadInfo.H"
#include "FFdCadModel/FdCadHandler.H"

#include "vpmDB/FmBladeProperty.H"

using FdColor = std::array<float,3>;


void FdCadHandler::initFdCad()
{
  FdCadFace::initClass();
  FdCadEdge::initClass();
  FdCadSolid::initClass();
  FdCadSolidWire::initClass();
}


void FdCadHandler::deleteCadData()
{
  delete myCadData;
  myCadData = NULL;
}


/*!
  Returns the contained CAD component casted to FdCadPart, if that is legal.
  If no CAD component has been created, creates one first.
  Returns NULL if the contained CAD component is an assembly.
*/

FdCadPart* FdCadHandler::getCadPart()
{
  FdCadPart* part = dynamic_cast<FdCadPart*>(myCadData);
  if (part) return part;
  if (myCadData) return NULL;

  myCadData = part = new FdCadPart;
  return part;
}


/*!
  Returns the contained CAD component casted to FdCadAssembly, if that is legal.
  If no CAD component has been created, creates one first.
  Returns NULL if the contained CAD component is a part.
*/

FdCadAssembly* FdCadHandler::getCadAssembly()
{
  FdCadAssembly* ass = dynamic_cast<FdCadAssembly*>(myCadData);
  if (ass) return ass;
  if (myCadData) return NULL;

  myCadData = ass = new FdCadAssembly;
  return ass;
}


bool FdCadHandler::hasPart()
{
  return dynamic_cast<FdCadPart*>(myCadData) ? true : false;
}


bool FdCadHandler::hasAssembly()
{
  return dynamic_cast<FdCadAssembly*>(myCadData) ? true : false;
}


void FdCadPart::deleteCadData()
{
  for (const FdSolidWirePair& solid : mySolids)
  {
    solid.first->unref();
    solid.second->unref();
  }

  mySolids.clear();
}


void FdCadPart::addSolid(FdCadSolid* solid, FdCadSolidWire* wire)
{
  if (solid) solid->ref();
  if (wire) wire->ref();

  mySolids.push_back(std::make_pair(solid,wire));
}


void FdCadAssembly::deleteCadData()
{
  for (FdCadComponent* cad : myComponents) delete cad;

  myComponents.clear();
}


/////////////////////////////
//
// File IO
//

/*!
  This file format has the syntax:
  <identifier>[whitesp]{<data>}

  <identifier> can consist of Letters and numbers
*/

void getIdentifier(std::istream& in, std::string& identifier)
{
  identifier = "";
  char c;
  in >> c;
  while (in)
    {
      if (isalnum(c))
        identifier += c;
      else{
        in.putback(c);
        break;
      }
      in.get(c);
    }
}


void skipToData(std::istream& in)
{
  in.ignore(std::numeric_limits<int>::max(), '{');
}


// Todo: To allow handling of nested unknown items
// this method needs to be adjusted. Needs to count '{'
// and match them to '}'

void skipToDataEnd(std::istream& in)
{
  in.ignore(std::numeric_limits<int>::max(), '}');
}


void readCadEntityInfo(std::istream& in, FdCadEntityInfo* cadInf)
{
  if (!(cadInf && in ))
    return;

  std::string identifier;
  while (in)
  {
    getIdentifier(in, identifier);
    if (identifier == "")
      break;
    skipToData(in);

    if (identifier == "Origin"){
      in >> cadInf->origin;
      cadInf->myOriginIsValid = true;
      skipToDataEnd(in);
    } else if (identifier == "Axis"){
      in >> cadInf->axis;
      cadInf->myAxisIsValid = true;
      skipToDataEnd(in);
    } else if (identifier == "Type"){
      in >> cadInf->type;
      skipToDataEnd(in);
    }
  }
}


void writeCadEntityInfo(std::ostream& out, const std::string& indent, FdCadEntityInfo* cadInf)
{
  if (cadInf && (cadInf->myOriginIsValid || cadInf->myAxisIsValid)) {
    out << indent << "GeometryInfo {\n";
    if (cadInf->myOriginIsValid)
      out << indent << "  Origin { " << cadInf->origin << " }\n";
    if (cadInf->myAxisIsValid)
      out << indent << "  Axis { " << cadInf->axis << " }\n";
    out << indent << "  Type { " << cadInf->type << " }\n";
    out << indent << "}\n";
  }
}


inline std::ostream& operator<<(std::ostream& os, const FdColor& v)
{
  return os << v[0] << ' ' << v[1] << ' ' << v[2];
}

inline std::istream& operator>>(std::istream& is, FdColor& v)
{
  return is >> v[0] >> v[1] >> v[2];
}

void writeVisProp(std::ostream& out, const std::string& indent, const FFdLook& prop)
{
  if (prop.isDefined)
    out << indent <<"VisualProperties {\n"
        << indent <<"  "<< prop.ambientColor <<"\n"
        << indent <<"  "<< prop.diffuseColor <<"\n"
        << indent <<"  "<< prop.specularColor <<"\n"
        << indent <<"  "<< prop.emissiveColor <<"\n"
        << indent <<"  "<< prop.transparency <<"\n"
        << indent <<"  "<< prop.shininess <<"\n"
        << indent <<"}\n";
}


void readVisProp(std::istream& in, FFdLook& prop)
{
  if (in) {
    in >> prop.ambientColor;
    in >> prop.diffuseColor;
    in >> prop.specularColor;
    in >> prop.emissiveColor;
    in >> prop.transparency;
    in >> prop.shininess;
    prop.isDefined = true;
  }

  skipToDataEnd(in);
}


inline std::ostream& operator<<(std::ostream& os, const SbVec3f& v)
{
  return os << v[0] << ' ' << v[1] << ' ' << v[2];
}

void writeVisProp(std::ostream& out, const std::string& indent, SoMaterial* visProp)
{
  if (visProp)
    out << indent <<"VisualProperties {\n"
        << indent <<"  "<< visProp->ambientColor[0] << "\n"
        << indent <<"  "<< visProp->diffuseColor[0] << "\n"
        << indent <<"  "<< visProp->specularColor[0] << "\n"
        << indent <<"  "<< visProp->emissiveColor[0] << "\n"
        << indent <<"  "<< visProp->transparency[0] << "\n"
        << indent <<"  "<< visProp->shininess[0] << "\n"
        << indent <<"}\n";
}


void readVisProp(std::istream& in, SoMaterial* visProp)
{
  if (visProp && in){
    FdColor ambient, diffuse, specular, emissive;
    float transp, shin;

    in >> ambient;
    in >> diffuse;
    in >> specular;
    in >> emissive;
    in >> transp;
    in >> shin;

    visProp->ambientColor.setValue(ambient.data());
    visProp->diffuseColor.setValue(diffuse.data());
    visProp->specularColor.setValue(specular.data());
    visProp->emissiveColor.setValue(emissive.data());
    visProp->transparency.setValue(transp);
    visProp->shininess.setValue(shin);
  }

  skipToDataEnd(in);
}


void writeCoords(std::ostream& out, const std::string& indent, SoCoordinate3* coords)
{
  if (!coords || coords->point.getNum() < 1)
    return;

  out << indent <<"Coordinates {\n";
  for (int i = 0; i < coords->point.getNum(); i++)
    out << indent <<"  "
        << coords->point[i][0] <<" "
        << coords->point[i][1] <<" "
        << coords->point[i][2] <<"\n";
  out << indent <<"}\n";
}


void readCoords(std::istream& in,  SoCoordinate3* coords)
{
  if (!(coords && in))
    return;

  float x, y, z;
  std::vector< std::array<float,3> > coordVec;

  in >> x >> y >> z;
  while (in && in.good()){
    coordVec.push_back({x,y,z});
    in >> x >> y >> z;
  }

  coords->point.setNum(coordVec.size());
  SbVec3f* coord = coords->point.startEditing();
  for (const std::array<float,3>& vec : coordVec)
    (coord++)->setValue(vec.data());
  coords->point.finishEditing();

  in.clear();
  skipToDataEnd(in);
}


void writeFace(std::ostream& out, const std::string& indent, FdCadFace* face)
{
  if (!face)
    return;

  out << indent << "Face {\n";
  if (face->getGeometryInfo())
    writeCadEntityInfo(out, indent + "  ", face->getGeometryInfo());

  if ( face->coordIndex.getNum()){
    out << indent << "  TriangleIndexes {\n";
    out << indent << "    ";
    for (int i = 0; i < face->coordIndex.getNum(); i++) {
      out << face->coordIndex[i] << " ";
      if (face->coordIndex[i] == -1 && i != face->coordIndex.getNum()-1)
        out << "\n" << indent << "    ";
    }
    out << "\n";
    out << indent << "  }\n";
  }
  out << indent << "}\n";
}


void readFace(std::istream& in, FdCadFace* face)
{
  if (!face)
    return;
  std::string identifier;
  while (in)
    {
      getIdentifier(in, identifier);
      if (identifier == "")
        break;
      skipToData(in);

      if (identifier == "GeometryInfo"){
        FdCadEntityInfo * cadInf = new FdCadEntityInfo();
        readCadEntityInfo(in, cadInf);
        face->setGeometryInfo(cadInf);
        skipToDataEnd(in);
      }else if (identifier == "TriangleIndexes"){
        int tIdx;
        std::vector<int> tIndexes;
        while (in && in.good())
          {
            in >> tIdx;
            if ( in.good())
              tIndexes.push_back(tIdx);
          }
        in.clear();
        skipToDataEnd(in);

        face->coordIndex.enableNotify(false);
        face->coordIndex.deleteValues(0);
        face->coordIndex.setValues(0, tIndexes.size(), &(tIndexes[0]));
        face->coordIndex.enableNotify(true);
        face->coordIndex.touch();
      } else {
        // Unknown identifier
        skipToDataEnd(in);
      }
    }

  skipToDataEnd(in);
}


void writeEdge(std::ostream& out, const std::string& indent, FdCadEdge* edge)
{
  if (!edge)
    return;

  out << indent << "Edge {\n";
  if (edge->getGeometryInfo())
    writeCadEntityInfo(out, indent + "  ", edge->getGeometryInfo());

  if ( edge->coordIndex.getNum()){
    out << indent << "  EdgeIndexes {\n";
    out << indent << "    ";
    for (int i = 0; i < edge->coordIndex.getNum(); ++i){
      out << edge->coordIndex[i] << " ";
      if ((edge->coordIndex[i] == -1) && (i != (edge->coordIndex.getNum()-1)) )
        out << "\n" << indent << "    ";
    }
    out << "\n";
    out << indent << "  }\n";
  }
  out << indent << "}\n";
}


void readEdge(std::istream& in, FdCadEdge * edge)
{
  if (!edge)
    return;
  std::string identifier;
  while (in )
    {
      getIdentifier(in, identifier);
      if (identifier == "")
        break;
      skipToData(in);

      if (identifier == "GeometryInfo"){
        FdCadEntityInfo * cadInf = new FdCadEntityInfo();
        readCadEntityInfo(in, cadInf);
        edge->setGeometryInfo(cadInf);
        skipToDataEnd(in);
      }else if (identifier == "EdgeIndexes"){
        int tIdx;
        std::vector<int> tIndexes;
        while (in && in.good())
          {
            in >> tIdx;
            if ( in.good())
              tIndexes.push_back(tIdx);
          }
        in.clear();
        skipToDataEnd(in);

        edge->coordIndex.enableNotify(false);
        edge->coordIndex.deleteValues(0);
        edge->coordIndex.setValues(0, tIndexes.size(), &(tIndexes[0]));
        edge->coordIndex.enableNotify(true);
        edge->coordIndex.touch();
      } else {
        // Unknown identifier
        skipToDataEnd(in);
      }
    }

  skipToDataEnd(in);
}


void writeBody(std::ostream& out, const std::string& indent, FdCadSolid* body, FdCadSolidWire* wire)
{
  if (! (body || wire))
    return;

  out << indent << "Body {\n";

  if (body){
    SoMaterial* mat = NULL;
    SoCoordinate3* coords = NULL;
    int numNodes = body->getNumChildren();
    for (int i = 0; i < numNodes; ++i){
      if (!mat && body->getChild(i)->isOfType(SoMaterial::getClassTypeId()))
        mat = static_cast<SoMaterial*>(body->getChild(i));
      if (!coords && body->getChild(i)->isOfType(SoCoordinate3::getClassTypeId()))
        coords = static_cast<SoCoordinate3*>(body->getChild(i));
    }

    writeVisProp(out, indent + "  ", mat);
    writeCoords(out, indent + "  ", coords);
    for (int i = 0; i < numNodes; i++)
      if (body->getChild(i)->isOfType(FdCadFace::getClassTypeId()))
        writeFace(out, indent + "    ", static_cast<FdCadFace*>(body->getChild(i)));
  }

  if (wire){
    int numNodes = wire->getNumChildren();
    for (int i = 0; i < numNodes; ++i)
      if (wire->getChild(i)->isOfType(FdCadEdge::getClassTypeId()))
        writeEdge(out, indent + "    ", static_cast<FdCadEdge*>(wire->getChild(i)));
  }
  out << indent << "}\n";
}


void readBody(std::istream& in, FdCadSolid* body, FdCadSolidWire* wire)
{
  if (! (body || wire))
    return;

  std::string identifier;
  while (in)
    {
      getIdentifier(in, identifier);
      if (identifier == "")
        break;
      skipToData(in);
      if (identifier == "VisualProperties"){
        SoMaterial * mat = new SoMaterial();
        body->insertChild(mat, 0);
        readVisProp(in, mat);
       } else if (identifier == "Coordinates"){
        SoCoordinate3 * coords = new SoCoordinate3();
        body->insertChild(coords, 0);
        wire->insertChild(coords, 0);
        readCoords(in, coords);
      } else if (identifier == "Face") {
        FdCadFace * face = new FdCadFace();
        body->addChild(face);
        readFace(in, face);
      } else if (identifier == "Edge") {
        FdCadEdge * edge = new FdCadEdge();
        wire->addChild(edge);
        readEdge(in, edge);
      } else {
        // Unknown identifier
        skipToDataEnd(in);
      }
    }

   skipToDataEnd(in);
}


void FdCadPart::write(std::ostream& out, const std::string& indent)
{
  out << indent << "Part {\n";
  out << indent << "  CS {" << myPartCS << "\n";
  out << indent << "  }\n";

  writeVisProp(out, indent + "  ", myVisProp);

  for (const FdSolidWirePair& solid : mySolids)
    writeBody(out, indent + "  ", solid.first, solid.second);

  out << indent << "}\n";
}


void FdCadPart::read(std::istream& in)
{
  std::string identifier;
  while (in)
    {
      getIdentifier(in, identifier);
      if (identifier == "")
        break;
      skipToData(in);
      if (identifier == "CS"){
        in >> myPartCS;
        skipToDataEnd(in);
      } else if (identifier == "VisualProperties"){
        readVisProp(in, myVisProp);
      } else if (identifier == "Body"){
        FdCadSolid * solid = new FdCadSolid();
        FdCadSolidWire * wire = new FdCadSolidWire();
        this->addSolid(solid,wire);
        readBody(in, solid, wire);
      } else {
        // Unknown identifier
        skipToDataEnd(in);
      }
    }

  skipToDataEnd(in);
}


void FdCadAssembly::write(std::ostream& out, const std::string& indent)
{
  out << indent << "Assembly {\n";
  out << indent << "  CS {" << myPartCS << "\n";
  out << indent << "  }\n";

  for (FdCadComponent* cad : myComponents)
    cad->write(out, indent + "  ");

  out << indent << "}\n";
}


void FdCadAssembly::read(std::istream& in)
{
  std::string identifier;
  while (in)
    {
      getIdentifier(in, identifier);
      if (identifier == "")
        break;
      skipToData(in);
      if (identifier == "CS"){
        in >> myPartCS;
        skipToDataEnd(in);
      } else if (identifier == "Part"){
        myComponents.push_back(new FdCadPart());
        myComponents.back()->read(in);
      } else if (identifier == "Assembly"){
        myComponents.push_back(new FdCadAssembly());
        myComponents.back()->read(in);
      } else {
        // Unknown identifier
        skipToDataEnd(in);
      }
    }

   skipToDataEnd(in);
}


void FdCadHandler::write(std::ostream& out)
{
  if (!myCadData) return;

  out << "Fedem Technology Simplified CAD model\n\n";
  myCadData->write(out,"");
  out << std::flush;
}


bool FdCadHandler::read(std::istream& in)
{
  if(this->hasPart() || this->hasAssembly())
    this->deleteCadData();

  std::string firstLine;
  getline(in, firstLine);

  std::string identifier;
  getIdentifier(in, identifier);
  skipToData(in);

  if (identifier == "Part")
    this->getCadPart()->read(in);
  else if (identifier == "Assembly")
    this->getCadAssembly()->read(in);
  else
    return false;

  return true;
}


//////////////////////////////////////////////////////////////////////
// Create visualizations

// Helper macro
#define BEAM_DRAW_CIRCLE(position,radius) \
  for (int i = 0; i < 36; ++i) { \
    coord[i + coordOffset].setValue( \
      /* x */ position[0] + \
                radius * cos(i/36.0 * 2 * M_PI) * vn1[0] + \
                radius * sin(i/36.0 * 2 * M_PI) * vn2[0], \
      /* y */ position[1] + \
                radius * cos(i/36.0 * 2 * M_PI) * vn1[1] + \
                radius * sin(i/36.0 * 2 * M_PI) * vn2[1], \
      /* z */ position[2] + \
                radius * cos(i/36.0 * 2 * M_PI) * vn1[2] + \
                radius * sin(i/36.0 * 2 * M_PI) * vn2[2]); \
  }

// Helper macro
#define BEAM_MESH_CIRCLES(start,stop) \
  for (int i = start; i < stop; ++i) { \
    idx[cc++] = i + m1; \
    idx[cc++] = (i < 35) ? (i + m1 + 1) : (m1); \
    idx[cc++] = (i < 35) ? (i + m2 + 1) : (m2); \
    idx[cc++] = i + m2; \
    idx[cc++] = -1; \
  }

// Create pipe visualization for beam
bool FdCadHandler::createBeamViz_Pipe(const FaVec3& v1, const FaVec3& v2,
                                      double Do, double Di, int nStartAngle, int nStopAngle)
{
  // Initial math that gives us the following vectors:
  //  vd : the vector that takes us from v1 to v2
  //  vn1 : a vector that is perpendicular to vd
  //  vn2 : a vector that is perpendicular to both vd and vn1
  // Note: Using vn1 and vn2, we can easily create a cylinder (or other shape) from v1 to v2.
  FaVec3 vd = v2 - v1;
  if (vd.isZero())
    return false; // don't visualize this
  FaVec3 cbv(1.0, 0.0, 0.0); // cardinal basis vector (i.e. perpendicular to vd)
  if ((abs(vd[1]) <= abs(vd[0])) && (abs(vd[1]) <= abs(vd[2]))) cbv = FaVec3(0.0, 1.0, 0.0);
  if ((abs(vd[2]) <= abs(vd[0])) && (abs(vd[2]) <= abs(vd[1]))) cbv = FaVec3(0.0, 0.0, 1.0);
  FaVec3 vn1 = vd ^ cbv; // a normal vector to vd
  if (vn1.isZero())
    return false; // unexpected
  vn1.normalize();
  FaVec3 vn2 = vd ^ vn1; // a normal vector to vd and vn1
  vn2.normalize();

  // Clean up
  if(this->hasPart() || this->hasAssembly())
    this->deleteCadData();

  // Get cad part
  FdCadPart* part = this->getCadPart();
  if (part == NULL)
    return false; // unexpected


  // Create cad solid and wire representations
  FdCadSolid* body = new FdCadSolid();
  FdCadSolidWire* wire = new FdCadSolidWire();
  part->addSolid(body, wire);


  // Create coordinates
  SoCoordinate3* coords = new SoCoordinate3();
  body->insertChild(coords, 0);
  wire->insertChild(coords, 0);
  coords->point.setNum(36 * 4);
  SbVec3f* coord = coords->point.startEditing();

  // draw outer circle around triad 1
  int coordOffset = 0;
  BEAM_DRAW_CIRCLE(v1,Do/2.0f);

  // draw outer circle around triad 2
  coordOffset += 36;
  BEAM_DRAW_CIRCLE(v2,Do/2.0f);

  // draw inner circle around triad 1
  coordOffset += 36;
  BEAM_DRAW_CIRCLE(v1,Di/2.0f);

  // draw inner circle around triad 2
  coordOffset += 36;
  BEAM_DRAW_CIRCLE(v2,Di/2.0f);

  coords->point.finishEditing();

  // Start and stop angles
  bool sliced = (nStartAngle != 0) || (nStopAngle != 360);
  int angle1 = nStartAngle/10;
  int angle2 = nStopAngle/10;
  if ((angle1 < 0) || (angle1 > 35))
    angle1 = 0;
  if ((angle2 < 0) || (angle2 > 36))
    angle2 = 36;
  if (angle2 < angle1)
    angle2 = angle1;

  // Create cad face
  FdCadFace* face = new FdCadFace();
  body->addChild(face);
  int idx[65536];
  int cc = 0;
  {
    // Set indexes for cap 1
    int m1 = 0;      // mesh 1 is outer circle 1
    int m2 = 36 * 2; // mesh 2 is inner circle 1
    BEAM_MESH_CIRCLES(angle1,angle2);
    // Set indexes for cap 2
    m1 = 36;     // mesh 1 is outer circle 2
    m2 = 36 * 3; // mesh 2 is inner circle 2
    BEAM_MESH_CIRCLES(angle1,angle2);
    // Set indexes for outer sides
    m1 = 0;  // mesh 1 is outer circle 1
    m2 = 36; // mesh 2 is outer circle 2
    BEAM_MESH_CIRCLES(angle1,angle2);
    // Set indexes for inner sides
    m1 = 36 * 2; // mesh 1 is inner circle 1
    m2 = 36 * 3; // mesh 2 is inner circle 2
    BEAM_MESH_CIRCLES(angle1,angle2);
    // Set indexes for slicing cut-out
    if (sliced) {
      idx[cc++] = (angle1 < 36) ? (angle1) : (0); // outer circle 1
      idx[cc++] = (angle1 < 36) ? (angle1 + 36*2) : (36*2); // inner circle 1
      idx[cc++] = (angle1 < 36) ? (angle1 + 36*3) : (36*3); // inner circle 2
      idx[cc++] = (angle1 < 36) ? (angle1 + 36) : (36); // outer circle 2
      idx[cc++] = -1;
      idx[cc++] = (angle2 < 36) ? (angle2) : (0); // outer circle 1
      idx[cc++] = (angle2 < 36) ? (angle2 + 36*2) : (36*2); // inner circle 1
      idx[cc++] = (angle2 < 36) ? (angle2 + 36*3) : (36*3); // inner circle 2
      idx[cc++] = (angle2 < 36) ? (angle2 + 36) : (36); // outer circle 2
      idx[cc++] = -1;
    }
  }
  face->coordIndex.enableNotify(false);
  face->coordIndex.deleteValues(0);
  face->coordIndex.setValues(0, cc, &(idx[0]));
  face->coordIndex.enableNotify(true);
  face->coordIndex.touch();

  // Create cad edge
  FdCadEdge* edge = new FdCadEdge();
  wire->addChild(edge);
  // Set indexes for cap 1
  cc = 0;
  int i;
  for (i = angle1; i < angle2; ++i) {
    idx[cc++] = i;
  }
  i--;
  idx[cc++] = (i < 35) ? (i + 1) : 0; // last line point
  idx[cc++] = -1;
  // Set indexes for cap 2
  for (i = angle1; i < angle2; ++i) {
    idx[cc++] = i + 36;
  }
  i--;
  idx[cc++] = (i < 35) ? (i + 36) : 36; // last line point
  idx[cc++] = -1;
  // Set indexes for sides
  for (i = angle1; i < angle2; (i += 4)) {
    idx[cc++] = i;
    idx[cc++] = i + 36;
    idx[cc++] = -1;
  }
  edge->coordIndex.enableNotify(false);
  edge->coordIndex.deleteValues(0);
  edge->coordIndex.setValues(0, cc, &(idx[0]));
  edge->coordIndex.enableNotify(true);
  edge->coordIndex.touch();

  return true;
}


// Create visualization for blade
bool FdCadHandler::createBeamViz_Blade(const FmTriad*, const FmTriad*,
                                       void* pBladePropertycurr,
                                       void* pBladePropertyPrev,
                                       void* pBladePropertyNext,
                                       int segmentPart)
{
  FmBladeProperty* pBPcurr = (FmBladeProperty*)pBladePropertycurr;
  FmBladeProperty* pBPprev = (FmBladeProperty*)pBladePropertyPrev;
  FmBladeProperty* pBPnext = (FmBladeProperty*)pBladePropertyNext;

  // Clean up
  if (this->hasPart() || this->hasAssembly())
    this->deleteCadData();

  // Get cad part
  FdCadPart* part = this->getCadPart();
  if (part == NULL)
    return false; // unexpected

  // Create cad solid and wire representations
  FdCadSolid* body = new FdCadSolid();
  FdCadSolidWire* wire = new FdCadSolidWire();
  part->addSolid(body, wire);

  // Get parameters from the bladeproperty/Aerodyn node centered in this beam, and from adjacent beams
  float chordPrev = pBPprev->Chord.getValue();
  float chordCurr = pBPcurr->Chord.getValue();
  float chordNext = pBPnext->Chord.getValue();

  float lengthPrev = pBPprev->Length.getValue();
  float lengthCurr = pBPcurr->Length.getValue();
  float lengthNext = pBPnext->Length.getValue();

  float twistPrev = pBPprev->Twist.getValue();
  float twistCurr = pBPcurr->Twist.getValue();
  float twistNext = pBPnext->Twist.getValue();

  float thicknessPrev = pBPprev->Thick.getValue();
  float thicknessCurr = pBPcurr->Thick.getValue();
  float thicknessNext = pBPnext->Thick.getValue();

  float offsetXPrev = pBPprev->PitchCentre.getValue().first;
  float offsetXCurr = pBPcurr->PitchCentre.getValue().first;
  float offsetXNext = pBPnext->PitchCentre.getValue().first;

  float offsetYPrev = pBPprev->PitchCentre.getValue().second;
  float offsetYCurr = pBPcurr->PitchCentre.getValue().second;
  float offsetYNext = pBPnext->PitchCentre.getValue().second;

  // Interpolate between adjacent segments.
  //
  //        <-----Blade Segment---------><-------Blade Segment-------><-------BladeSegment-------->
  //                   AeroNode        Back         AeroNode        Front        AeroNode
  // Beams: |====prev0===|O|====prev1===||===curr0====|O|====curr1===||====next0===|O|====next1===|
  //                      |              ^             |              ^             |
  //                      |lengthPrev/2  | lengthCurr/2| lengthCurr/2 |lengthNext/2 |

  float scaleFront = lengthCurr/( lengthCurr + lengthNext );
  float scaleBack =  lengthPrev/( lengthPrev + lengthCurr );

  float interpolChordFront =  chordCurr + scaleFront*( chordNext - chordCurr );
  float interpolChordBack  = chordPrev + scaleBack*( chordCurr - chordPrev );

  // NOTE: The visualization is already rotated with the part, so only the difference in twist is needed
  float interpolTwistFront =  scaleFront*( twistNext - twistCurr );
  float interpolTwistBack  =  -scaleBack*( twistCurr - twistPrev );

  float interpolThicknessFront = thicknessCurr + scaleFront*( thicknessNext - thicknessCurr );
  float interpolThicknessBack  = thicknessPrev + scaleBack*( thicknessCurr - thicknessPrev );

  float interpolOffsetXFront = offsetXCurr + scaleFront*( offsetXNext - offsetXCurr );
  float interpolOffsetXBack  = offsetXPrev + scaleBack*( offsetXCurr - offsetXPrev );

  float interpolOffsetYFront = offsetYCurr + scaleFront*( offsetYNext - offsetYCurr );
  float interpolOffsetYBack  = offsetYPrev + scaleBack*( offsetYCurr - offsetYPrev );

  // If current segment is at the tip of the blade, extrapolate
  if (pBPnext == pBPcurr)
    interpolChordFront = chordCurr + scaleFront*(chordCurr - chordPrev);

  // Choose parameters based on if current beam is the back or front part of the blade-segment
  float chordFront, chordBack, thickFront, thickBack,
        offsetXFront, offsetXBack, offsetYFront, offsetYBack,
        twistFront, twistBack;

  if (segmentPart==0)
  {
    chordFront = chordCurr;
    chordBack = interpolChordBack;
    thickFront = thicknessCurr;
    thickBack = interpolThicknessBack;
    offsetXFront = offsetXCurr;
    offsetXBack = interpolOffsetXBack;
    offsetYFront = offsetYCurr;
    offsetYBack = interpolOffsetYBack;
    twistFront = twistCurr;
    twistBack = interpolTwistBack;
  }
  else
  {
    chordFront = interpolChordFront;
    chordBack = chordCurr;
    thickFront = interpolThicknessFront;
    thickBack = thicknessCurr;
    offsetXFront = interpolOffsetXFront;
    offsetXBack = offsetXCurr;
    offsetYFront = interpolOffsetYFront;
    offsetYBack = offsetYCurr;
    twistFront = interpolTwistFront;
    twistBack = twistCurr;
  }

  // Create coordinates
  const int numOfPoints = 200;

  SoCoordinate3* coords = new SoCoordinate3();
  body->insertChild(coords, 0);
  wire->insertChild(coords, 0);
  coords->point.setNum(2*numOfPoints);
  SbVec3f* coord = coords->point.startEditing();

  std::vector<float> pointsTopX, pointsTopY;
  std::vector<float> pointsBottomX, pointsBottomY;
  const float p = 0.4f;  // Location of maximum camber
  const float m = 0.03f; // Maximum camber
  const float width = 1.0f;
  float c, x, y, z, t, offsetX, offsetY;

  // For both sides of the beam: create coordinates for airfoil-profile
  for (int frontBack = 0; frontBack < 2; frontBack++)
  {
    pointsTopX.clear();
    pointsTopY.clear();
    pointsBottomX.clear();
    pointsBottomY.clear();

    if (frontBack == 0)
    {
      c = chordBack;
      z = 0;
      t = thickBack;
      offsetX = offsetXBack*c;
      offsetY = offsetYBack*c;
    }
    else
    {
      c = chordFront;
      z = lengthCurr/2;
      t = thickFront;
      offsetX = offsetXFront*c;
      offsetY = offsetYFront*c;
    }

    // Make sure that we don't get singular matrix when creating points around the airofoil with a zero chord-length
    if (c == 0.0f) c = 0.001f;

    if (t < 1)
    {
      // Draw cambered airfoil
      pointsTopX.push_back(-offsetX );
      pointsTopY.push_back( offsetY );
      pointsBottomX.push_back(-offsetX );
      pointsBottomY.push_back( offsetY );

      for (int i = 1; i < numOfPoints/4; i++)
      {
        float dx = p*c*((float)i/(float)(numOfPoints/4));
        float y_c = m*(dx/(p*p))*(2*p-(dx/c));
        float y_t = (t/0.2)*c*(  0.2969*sqrt(dx/c) -0.1260*(dx/c) - 0.3516*(dx/c)*(dx/c) + 0.2843 * (dx/c)*(dx/c)*(dx/c) - 0.1015*(dx/c)*(dx/c)*(dx/c)*(dx/c) );
        float x_U = dx - y_t*sin( atan( (y_c)/(dx) ));
        float y_U = y_c + y_t*cos( atan( (y_c)/(dx) ));
        float x_L = dx + y_t*sin( atan( (y_c)/(dx) ));
        float y_L = y_c - y_t*cos( atan( (y_c)/(dx) ));
        pointsTopX.push_back( -offsetX + x_U*width );
        pointsTopY.push_back( offsetY - y_U*width );
        pointsBottomX.push_back( -offsetX + x_L*width );
        pointsBottomY.push_back( offsetY - y_L*width );
      }

      for (int i = 1; i <= numOfPoints/4; i++)
      {
        float dx = p*c + (1-p)*c*((float)i/(float)(numOfPoints/4));
        float y_c = m*( (c -dx)/((1-p)*(1-p)) )*(1+(dx/c)-2*p);
        float y_t = (t/0.2)*c*(  0.2969*sqrt(dx/c)  -0.1260*(dx/c) - 0.3516*(dx/c)*(dx/c) + 0.2843 * (dx/c)*(dx/c)*(dx/c) - 0.1015*(dx/c)*(dx/c)*(dx/c)*(dx/c) );
        float x_U = dx - y_t*sin( atan( (y_c)/(dx) ) );
        float y_U = y_c + y_t*cos( atan( (y_c)/(dx) ) );
        float x_L = dx + y_t*sin( atan( (y_c)/(dx) ) );
        float y_L = y_c - y_t*cos( atan( (y_c)/(dx) ) );
        pointsTopX.push_back( -offsetX + x_U*width );
        pointsTopY.push_back( offsetY - y_U*width );
        pointsBottomX.push_back( -offsetX + x_L*width );
        pointsBottomY.push_back( offsetY - y_L*width );
      }
    }
    else // circle
      for(int i = 0; i < numOfPoints/2; i++)
      {
        float x_U = (c/2)*cos(( (float)(numOfPoints/2-i)/(float)(numOfPoints/2))*M_PI);
        float y_L = (c/2)*sin(( (float)(numOfPoints/2-i)/(float)(numOfPoints/2))*M_PI);
        float x_L = (c/2)*cos(( (float)(numOfPoints/2-i)/(float)(numOfPoints/2))*M_PI);
        float y_U = -(c/2)*sin(( (float)(numOfPoints/2-i)/(float)(numOfPoints/2))*M_PI);

        pointsTopX.push_back( -offsetX + c/2 + x_U*width );
        pointsTopY.push_back( offsetY + y_U*width );
        pointsBottomX.push_back( -offsetX + c/2 + x_L*width );
        pointsBottomY.push_back( offsetY + y_L*width );
      }

    // Add points to coordinate-array
    for (int i = 0; i < numOfPoints/2; i++)
      coord[frontBack*numOfPoints +i].setValue(z,pointsTopY.at(i),-pointsTopX.at(i));
    for (int i = 0; i < numOfPoints/2; i++)
      coord[frontBack*numOfPoints + numOfPoints/2+i].setValue(z,pointsBottomY.at(numOfPoints/2-i-1),-pointsBottomX.at(numOfPoints/2-i-1));
  }

  coords->point.finishEditing();

  // Transform coordinates based on twist
  twistFront = segmentPart == 0 ? 0 : -(M_PI/180.0)*twistFront - M_PI/2.0;
  twistBack  = segmentPart != 0 ? 0 : -(M_PI/180.0)*twistBack  - M_PI/2.0;
  for (int i= 0; i < numOfPoints; i++)
  {
    coord[i].getValue(x,y,z);
    t = i < numOfPoints ? twistBack : twistFront;
    coord[i].setValue(x, -sin(t)*z + cos(t)*y, cos(t)*z + sin(t)*y);
  }

  // Create cad face
  FdCadFace* face = new FdCadFace();
  body->addChild(face);
  int idx[5*numOfPoints];

  // Create faces between the ends of the beam
  for (int i = 0; i < numOfPoints-1; i++)
  {
    idx[i*5  ] = i+1;
    idx[i*5+1] = i;
    idx[i*5+2] = numOfPoints+i;
    idx[i*5+3] = numOfPoints+i+1;
    idx[i*5+4] = -1;
  }

  idx[numOfPoints*5-5] = 0;
  idx[numOfPoints*5-4] = numOfPoints-1;
  idx[numOfPoints*5-3] = 2*numOfPoints -1;
  idx[numOfPoints*5-2] = numOfPoints;
  idx[numOfPoints*5-1] = -1;

  face->coordIndex.enableNotify(false);
  face->coordIndex.deleteValues(0);
  face->coordIndex.setValues(0, 5*numOfPoints, idx);
  face->coordIndex.enableNotify(true);
  face->coordIndex.touch();

  // Create cad edge
  FdCadEdge* edge = new FdCadEdge();
  wire->addChild(edge);
  // Set indexes for cap 1
  int cc = 2;
  for (int i = 0; i < numOfPoints; i++)
    idx[cc++] = i;
  idx[cc++] = 0; // last line back to start point
  idx[cc++] = -1;
  // Set indexes for cap 2
  for (int i = 0; i < numOfPoints; i++)
    idx[cc++] = i + numOfPoints;
  idx[cc++] = numOfPoints; // last line back to start point
  idx[cc++] = -1;
  // Set indexes for sides
  for (int i = 0; i < numOfPoints; i += 10)
  {
    idx[cc++] = i;
    idx[cc++] = i + numOfPoints;
    idx[cc++] = -1;
  }
  edge->coordIndex.enableNotify(false);
  edge->coordIndex.deleteValues(0);
  edge->coordIndex.setValues(0, cc, idx);
  edge->coordIndex.enableNotify(true);
  edge->coordIndex.touch();

  return true;
}
