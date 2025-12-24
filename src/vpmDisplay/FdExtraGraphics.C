// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdExtraGraphics.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdBase.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdSymbolKit.H"
#include "vpmDisplay/FdTransformKit.H"
#include "vpmDisplay/FdConverter.H"
#include "FFaLib/FFaAlgebra/FFaMat34.H"
#include "FFaLib/FFaAlgebra/FFa3DLocation.H"
#include "FFaLib/FFaGeometry/FFaCylinderGeometry.H"

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoPolygonOffset.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/manips/SoTrackballManip.h>
#include <Inventor/manips/SoJackManip.h>
#include <Inventor/draggers/SoDragger.h>


namespace
{
  SoGroup*     cornerBox = NULL;
  SoSeparator* DOFHighlightSep = NULL;
  SoSeparator* directionSep = NULL;
  SoSeparator* CSSep = NULL;
  SoSeparator* lineSep = NULL;
  SoSeparator* our3DLocationSep = NULL;
  SoSeparator* cylSep = NULL;

  FaVec3 ourCurrentDirection(1.0,0.0,0.0);


  SoMaterial* new_material(float r, float g, float b)
  {
    SoMaterial* mat = new SoMaterial;
    mat->diffuseColor.setValue(r,g,b);
    mat->ambientColor.setValue(r,g,b);
    mat->emissiveColor.setValue(r,g,b);
    return mat;
  }


  SoSeparator* makeDOFVisualizing(int DOF,
                                  const FaVec3& center, const FaVec3& direction)
  {
    SoPickStyle* pickStyle = new SoPickStyle;
    pickStyle->style.setValue(SoPickStyle::UNPICKABLE);

    SoTransform* transform = new SoTransform;
    transform->translation.setValue(FdConverter::toSbVec3f(center));
    transform->rotation.setValue(SbRotation(SbVec3f(1,0,0),
                                 FdConverter::toSbVec3f(direction)));
    const float scale = 0.07f * FdDB::getCameraDistance(center);
    transform->scaleFactor.setValue(scale,scale,scale);

    SoSeparator* sep = new SoSeparator;
    sep->addChild(pickStyle);
    sep->addChild(transform);

    SoTrackballManip* ballManip;
    SoJackManip* jackManip;
    SoDragger* dragger;
    SoDrawStyle* drawStyle;
    SoCoordinate3* xLineCoords;

    switch (DOF) {
    case FdObject::RIGID:
      drawStyle = new SoDrawStyle;
      drawStyle->style.setValue(SoDrawStyle::LINES);
      sep->addChild(drawStyle);
      sep->addChild(new SoCube);
      break;

    case FdObject::REV:
      ballManip = new SoTrackballManip;
      ballManip->ref();
      dragger = ballManip->getDragger();
      sep->addChild(SO_GET_PART(dragger,"XRotator",SoSeparator));
      ballManip->unref();

      xLineCoords = new SoCoordinate3;
      xLineCoords->point.set1Value(0,-1,0,0);
      xLineCoords->point.set1Value(0, 1,0,0);
      sep->addChild(xLineCoords);
      sep->addChild(new SoLineSet);
      break;

    case FdObject::REV_STRECH:
    case FdObject::REV_CYL:
    case FdObject::REV_PRISM:
    case FdObject::CYL:
      ballManip = new SoTrackballManip;
      ballManip->ref();
      dragger = ballManip->getDragger();
      sep->addChild(SO_GET_PART(dragger,"XRotator",SoSeparator));
      ballManip->unref();

      jackManip = new SoJackManip;
      jackManip->ref();
      dragger = jackManip->getDragger();
      sep->addChild(SO_GET_PART(dragger,"translator.xFeedback",SoSeparator));
      jackManip->unref();
      break;

    case FdObject::PRISM_STRECH:
    case FdObject::PRISM:
      jackManip = new SoJackManip;
      jackManip->ref();
      dragger = jackManip->getDragger();
      sep->addChild(SO_GET_PART(dragger,"translator.xFeedback",SoSeparator));
      jackManip->unref();
      break;

    case FdObject::BALL:
      ballManip = new SoTrackballManip;
      ballManip->ref();
      dragger = ballManip->getDragger();
      sep->addChild(SO_GET_PART(dragger,"XRotator",SoSeparator));
      sep->addChild(SO_GET_PART(dragger,"YRotator",SoSeparator));
      sep->addChild(SO_GET_PART(dragger,"ZRotator",SoSeparator));
      ballManip->unref();
      break;

    case FdObject::BALL_STRECH:
      ballManip = new SoTrackballManip;
      ballManip->ref();
      dragger = ballManip->getDragger();
      sep->addChild(SO_GET_PART(dragger,"XRotator",SoSeparator));
      sep->addChild(SO_GET_PART(dragger,"YRotator",SoSeparator));
      sep->addChild(SO_GET_PART(dragger,"ZRotator",SoSeparator));
      ballManip->unref();

      jackManip = new SoJackManip;
      jackManip->ref();
      dragger = jackManip->getDragger();
      sep->addChild(SO_GET_PART(dragger,"translator.xFeedback",SoSeparator));
      jackManip->unref();
      break;

    case FdObject::FREE:
      jackManip = new SoJackManip;
      jackManip->ref();
      dragger = jackManip->getDragger();
      sep->addChild(SO_GET_PART(dragger,"translator.xFeedback",SoSeparator));
      sep->addChild(SO_GET_PART(dragger,"translator.yFeedback",SoSeparator));
      sep->addChild(SO_GET_PART(dragger,"translator.zFeedback",SoSeparator));
      jackManip->unref();
      break;
    }

    return sep;
  }


  void* getCornerBoxGraph(SbXfBox3f* bbox)
  {
    SbVec3f max, min;
    bbox->getBounds(min,max);

    SoTransform* xf = new SoTransform;
    xf->setMatrix(bbox->getTransform());
    xf->scaleFactor.setValue(SbVec3f(1,1,1));

    SoTransform* scale = new SoTransform;
    scale->translation.setValue(min);
    scale->scaleFactor.setValue(max - min);

    SoSeparator* sep = new SoSeparator;
    sep->addChild(xf);
    sep->addChild(scale);
    sep->addChild(cornerBox);

    FdDB::getExtraGraphicsSep()->addChild(sep);

    return (void*)sep;
  }


  SoSeparator* makeDirectionVisualizing(const FaVec3& from,
                                        const FaVec3& direction)
  {
    SoTransform* transform = new SoTransform;
    transform->translation.setValue(FdConverter::toSbVec3f(from));
    transform->rotation.setValue(SbRotation(SbVec3f(1,0,0),
                                            FdConverter::toSbVec3f(direction)));
    const float scale = 0.1f * FdDB::getCameraDistance(from);
    transform->scaleFactor.setValue(scale,scale,scale);

    SoPickStyle* pickStyle = new SoPickStyle;
    pickStyle->style.setValue(SoPickStyle::UNPICKABLE);

    static float coordsVal[6][3] = {{0,0,0}, //0
                                    {1,0,0}, //1
                                    {0.67f,  0,  0.056f},//2
                                    {0.67f,  0, -0.056f},//3
                                    {0.67f,  0.056f,  0},//4
                                    {0.67f, -0.056f,  0}};//5

    static int32_t indexes[15] = { 0 , 1, 2, 4, 1, 3, 5,  1,-1,
                                   4 , 3, -1,
                                   5 , 2, -1};

    SoGroup* arrow = new SoGroup;
    arrow->addChild(new SoDrawStyle);
    arrow->addChild(new_material(1,1,0));

    SoCoordinate3* coords = new SoCoordinate3;
    coords->point.setValues(0,6,coordsVal);
    arrow->addChild(coords);

    SoIndexedLineSet* box = new SoIndexedLineSet;
    box->coordIndex.setValues(0,15,indexes);
    arrow->addChild(box);

    SoSeparator* sep = new SoSeparator;
    sep->addChild(pickStyle);
    sep->addChild(transform);
    sep->addChild(arrow);

    return sep;
  }


  SoSeparator* makeLineVisualizing(const FaVec3& from, const FaVec3& to,
                                   const FdExtraGraphics::FdColor& color)
  {
    SoCoordinate3* coords = new SoCoordinate3;
    coords->point.set1Value(0, (float)from[0], (float)from[1], (float)from[2]);
    coords->point.set1Value(1, (float)to[0], (float)to[1], (float)to[2]);

    SoIndexedLineSet* line = new SoIndexedLineSet;
    line->coordIndex.set1Value(0,0);
    line->coordIndex.set1Value(1,1);
    line->coordIndex.set1Value(2,-1);

    SoSeparator* sep = new SoSeparator;
    sep->addChild(new_material(color[0], color[1], color[2]));
    sep->addChild(coords);
    sep->addChild(line);

    return sep;
  }


  SoSeparator* makeCylinderVisualizing(const FFaCylinderGeometry& cylinder,
                                       const FaMat34& globalMatrix,
                                       bool highlight)
  {
    SoTransform* globalTrans = new SoTransform;
    SoTransform* localTrans = new SoTransform;
    globalTrans->setMatrix(FdConverter::toSbMatrix(globalMatrix));
    localTrans->setMatrix(FdConverter::toSbMatrix(cylinder.getTransMatrix()));

    const std::pair<double,double> cylEnds   = cylinder.getZData();
    const std::pair<double,double> angleData = cylinder.getAngleData();

    const int sides = 64;
    const float fullCircle = 2*M_PI;
    float angle  = static_cast<float>(angleData.second - angleData.first);
    float radius = static_cast<float>(cylinder.getRadius());
    float deltaAngle = angle/static_cast<float>(sides);
    if (angle > fullCircle-deltaAngle)
    {
      angle = fullCircle;
      deltaAngle = angle/static_cast<float>(sides);
    }

    float x, y, z;
    int i, k, nCoords = 0, nLines = 0;
    SoCoordinate3* coords = new SoCoordinate3;

    for (k = 0; k < 2; k++) // set coordinates for top and bottom circles
    {
      z = static_cast<float>(k == 0 ? cylEnds.first : cylEnds.second);
      for (i = 0; i < sides; i++)
      {
        x = radius * cosf(deltaAngle*i);
        y = radius * sinf(deltaAngle*i);
        coords->point.set1Value(nCoords++, x, y, z);
      }
    }

    // coordinates for center of circles
    x = y = 0.0f;
    float crossLength = radius*0.1f;
    for (k = 0; k < 2; k++)
    {
      z = static_cast<float>(k == 0 ? cylEnds.first : cylEnds.second);
      coords->point.set1Value(nCoords++, crossLength, y, z);
      coords->point.set1Value(nCoords++, -crossLength, y, z);
      coords->point.set1Value(nCoords++, x, crossLength, z);
      coords->point.set1Value(nCoords++, x, -crossLength, z);
    }

    // coordinates for center line
    coords->point.set1Value(nCoords++, x, y, static_cast<float>(cylEnds.first));
    coords->point.set1Value(nCoords, x, y, static_cast<float>(cylEnds.second));

    // Draw the cylinder
    SoIndexedLineSet* line = new SoIndexedLineSet;

    // center line
    line->coordIndex.set1Value(nLines++, nCoords-1);
    line->coordIndex.set1Value(nLines++, nCoords);
    line->coordIndex.set1Value(nLines++, -1);

    // center of circle
    int temp = nCoords-2; // last of "center of circle" coords
    for (k = 0; k < 4; k++)
    {
      line->coordIndex.set1Value(nLines++, temp--);
      line->coordIndex.set1Value(nLines++, temp--);
      line->coordIndex.set1Value(nLines++, -1);
    }

    // circles
    for (k = 0; k < 2; k++)
    {
      int n = sides*k;
      for (i = 0; i < sides; i++)
        line->coordIndex.set1Value(nLines++,i+n);
      if (angle == fullCircle) // close the whole circle
        line->coordIndex.set1Value(nLines++,n);
      line->coordIndex.set1Value(nLines++,-1);
    }

    if (angle < fullCircle) // cake piece
    {
      //bottom
      line->coordIndex.set1Value(nLines++,0);
      line->coordIndex.set1Value(nLines++,nCoords-1);
      line->coordIndex.set1Value(nLines++,sides-1);
      line->coordIndex.set1Value(nLines++,-1);
      //top
      line->coordIndex.set1Value(nLines++,sides);
      line->coordIndex.set1Value(nLines++,nCoords);
      line->coordIndex.set1Value(nLines++,sides*2-1);
      line->coordIndex.set1Value(nLines++,-1);
      //along cylinder
      line->coordIndex.set1Value(nLines++,0);
      line->coordIndex.set1Value(nLines++,sides);
      line->coordIndex.set1Value(nLines++,-1);

      line->coordIndex.set1Value(nLines++,sides-1);
      line->coordIndex.set1Value(nLines++,sides*2-1);
      line->coordIndex.set1Value(nLines++,-1);
    }
    else // lines along whole cylinder
    {
      line->coordIndex.set1Value(nLines++,0);
      line->coordIndex.set1Value(nLines++,sides);
      line->coordIndex.set1Value(nLines++,-1);
      line->coordIndex.set1Value(nLines++,sides/2);
      line->coordIndex.set1Value(nLines++,sides/2+sides);
      line->coordIndex.set1Value(nLines++,-1);
    }

    SoSeparator* sep = new SoSeparator;
    if (highlight)
      sep->addChild(FdSymbolDefs::getHighlightMaterial());
    else
      sep->addChild(new_material(1,1,0)); // yellow
    sep->addChild(globalTrans);
    sep->addChild(localTrans);
    sep->addChild(coords);
    sep->addChild(line);

    return sep;
  }
}


void FdExtraGraphics::init()
{
  static float coordsVal[32][3] = {{ 0, 0.19f, 0    },// 0
                                   { 0,     0, 0    },// 1
                                   { 0.19f, 0, 0    },// 2
                                   { 0,     0, 0.19f},// 3
                                   { 0.81f, 0, 0    },// 4
                                   { 1,     0, 0    },// 5
                                   { 1, 0.19f, 0    },// 6
                                   { 1,     0, 0.19f},// 7
                                   { 1, 0.81f, 0    },// 8
                                   { 1,     1, 0    },// 9
                                   { 0.81f, 1, 0    },// 10
                                   { 1,     1, 0.19f},// 11
                                   { 0.19f, 1, 0    },// 12
                                   { 0,     1, 0    },// 13
                                   { 0, 0.81f, 0    },// 14
                                   { 0,     1, 0.19f},// 15
                                   { 0, 0.19f, 1    },// 16
                                   { 0,     0, 1    },// 17
                                   { 0.19f, 0, 1    },// 18
                                   { 0,     0, 0.81f},// 19
                                   { 0.81f, 0, 1    },// 20
                                   { 1,     0, 1    },// 21
                                   { 1, 0.19f, 1    },// 22
                                   { 1,     0, 0.81f},// 23
                                   { 1, 0.81f, 1    },// 24
                                   { 1,     1, 1    },// 25
                                   { 0.81f, 1, 1    },// 26
                                   { 1,     1, 0.81f},// 27
                                   { 0.19f, 1, 1    },// 28
                                   { 0,     1, 1    },// 29
                                   { 0, 0.81f, 1    },// 30
                                   { 0,     1, 0.81f}};// 31

  static int32_t indexes[56] = { 0 , 1, 2,-1,  1, 3,-1,
                                 4 , 5, 6,-1,  5, 7,-1,
                                 8 , 9,10,-1,  9,11,-1,
                                 12,13,14,-1, 13,15,-1,
                                 16,17,18,-1, 17,19,-1,
                                 20,21,22,-1, 21,23,-1,
                                 24,25,26,-1, 25,27,-1,
                                 28,29,30,-1, 29,31,-1 };

  SoCoordinate3* coords = new SoCoordinate3;
  coords->point.setValues(0,32,coordsVal);

  SoIndexedLineSet* box = new SoIndexedLineSet;
  box->coordIndex.setValues(0,56,indexes);

  cornerBox = new SoGroup;
  cornerBox->addChild(new SoDrawStyle);
  cornerBox->addChild(new_material(1,1,1));
  cornerBox->addChild(coords);
  cornerBox->addChild(box);
  cornerBox->ref();
}


void* FdExtraGraphics::showBBox(SoNode* rootOfObjectToSurround)
{
  if (!rootOfObjectToSurround)
    return NULL;

  SbViewportRegion vp;
  SoGetBoundingBoxAction bboxGetter(vp);
  bboxGetter.apply(rootOfObjectToSurround);
  SbXfBox3f bbox = bboxGetter.getXfBoundingBox();

  return getCornerBoxGraph(&bbox);
}

void* FdExtraGraphics::showBBox(const FaVec3& min, const FaVec3& max,
                                const FaMat34& where)
{
  SbXfBox3f ivBbox(FdConverter::toSbVec3f(min), FdConverter::toSbVec3f(max));
  ivBbox.setTransform(FdConverter::toSbMatrix(where));

  return getCornerBoxGraph(&ivBbox);
}

void FdExtraGraphics::removeBBox(void* bBoxId)
{
  FdDB::getExtraGraphicsSep()->removeChild((SoNode*)bBoxId);
}


void FdExtraGraphics::showDOFVisualizing(int DOF,
                                         const FaVec3& center,
                                         const FaVec3& direct)
{
  FdExtraGraphics::hideDOFVisualizing();
  DOFHighlightSep = makeDOFVisualizing(DOF,center,direct);

  FdDB::getExtraGraphicsSep()->addChild(DOFHighlightSep);
}

void FdExtraGraphics::hideDOFVisualizing()
{
  if (DOFHighlightSep)
    FdDB::getExtraGraphicsSep()->removeChild(DOFHighlightSep);

  DOFHighlightSep = NULL;
}


void FdExtraGraphics::showDirection(const FaVec3& from, const FaVec3& direction)
{
  FdExtraGraphics::hideDirection();
  directionSep = makeDirectionVisualizing(from, direction);

  ourCurrentDirection = direction;

  FdDB::getExtraGraphicsSep()->addChild(directionSep);
}

void FdExtraGraphics::hideDirection()
{
  if (directionSep)
    FdDB::getExtraGraphicsSep()->removeChild(directionSep);

  directionSep = NULL;
}

void FdExtraGraphics::moveDirection(const FaVec3& newFromPoint)
{
  if (directionSep)
    FdExtraGraphics::showDirection(newFromPoint,ourCurrentDirection);
}


void FdExtraGraphics::showLine(const FaVec3& from, const FaVec3& to,
                               const FdColor& color)
{
  FdExtraGraphics::hideLine();
  lineSep = makeLineVisualizing(from, to, color);

  FdDB::getExtraGraphicsSep()->insertChild(lineSep, 0);
}

void FdExtraGraphics::hideLine()
{
  if (lineSep)
    FdDB::getExtraGraphicsSep()->removeChild(lineSep);

  lineSep = NULL;
}


void FdExtraGraphics::showCylinder(const FaMat34& matrix,
                                   const std::vector<FaVec3>& points,
                                   bool arcOnly)
{
  FdExtraGraphics::showCylinder(FFaCylinderGeometry(points,arcOnly),matrix);
}

void FdExtraGraphics::showCylinder(const FFaCylinderGeometry& cylinder,
                                   const FaMat34& matrix, bool highlight)
{
  FdExtraGraphics::hideCylinder();
  cylSep = makeCylinderVisualizing(cylinder,matrix,highlight);

  FdDB::getExtraGraphicsSep()->addChild(cylSep);
}

void FdExtraGraphics::hideCylinder()
{
  if (cylSep)
    FdDB::getExtraGraphicsSep()->removeChild(cylSep);

  cylSep = NULL;
}


void FdExtraGraphics::highlight(const FFaGeometryBase* geo,
                                const FaMat34& matrix, bool onOff)
{
  const FFaCylinderGeometry* cyl = dynamic_cast<const FFaCylinderGeometry*>(geo);
  if (!cyl) return;

  if (onOff)
    FdExtraGraphics::showCylinder(*cyl,matrix,true);
  else
    FdExtraGraphics::hideCylinder();
}


void FdExtraGraphics::showCS(const FaMat34& globalCS)
{
  FdExtraGraphics::hideCS();

  SoPolygonOffset* poff = new SoPolygonOffset;
  poff->factor = 2;
  poff->units = 2;
  poff->styles = SoPolygonOffset::LINES;

  FdTransformKit* xfkit = new FdTransformKit;
  xfkit->setPart("symbol", FdSymbolDefs::getSymbol(FdSymbolDefs::INT_LINK_COORD_SYS));
  SoTransform* xf = SO_GET_PART(xfkit, "firstTrans", SoTransform);
  xf->setMatrix(FdConverter::toSbMatrix(globalCS));
  xfkit->setPart("appearance.material", FdSymbolDefs::getHighlightMaterial());

  CSSep = new SoSeparator;
  CSSep->addChild(poff);
  CSSep->addChild(xfkit);

  FdDB::getExtraGraphicsSep()->insertChild(CSSep, 0);
}

void FdExtraGraphics::hideCS()
{
  if (CSSep)
    FdDB::getExtraGraphicsSep()->removeChild(CSSep);

  CSSep = NULL;
}


void FdExtraGraphics::show3DLocation(const FaMat34& posRefCS,
                                     const FaMat34& rotRefCS,
                                     const FFa3DLocation& loc)
{
  FdExtraGraphics::hide3DLocation();

  SoPolygonOffset* offset = new SoPolygonOffset;
  offset->styles.setValue(SoPolygonOffset::LINES);
  offset->units.setValue(-1);

  SoSeparator* posSep = new SoSeparator;
  posSep->addChild(new_material(1,0.5,0.7));
  posSep->addChild(new_material(0.7,0.5,1));

  FaVec3 pos = loc.translation();
  double pmax = pos.length() / 3.0;
  double camToPos      = FdDB::getCameraDistance(pos);
  double camToPosRefCS = FdDB::getCameraDistance(posRefCS.translation());
  double scale = 0.06 * (camToPos < camToPosRefCS ? camToPos : camToPosRefCS);
  // pd - The length of the end-offset used to draw the end arrows
  float d, pd = static_cast<float>(scale < pmax ? scale : pmax);

  // Position

  SoTransform* posXf = new SoTransform;
  posSep->addChild(posXf);
  posXf->setMatrix(FdConverter::toSbMatrix(posRefCS));

  float x = static_cast<float>(pos.x());
  float y = static_cast<float>(pos.y());
  float z = static_cast<float>(pos.z());

  SoCoordinate3* coords;
  SoIndexedLineSet* lines;
  int idx = 0, idxCount = 0;

  auto&& NEW_LINESHAPE = [&coords,&lines,&idx,&idxCount](SoSeparator* root)
  {
    coords = new SoCoordinate3;
    lines = new SoIndexedLineSet;
    root->addChild(coords);
    root->addChild(lines);
    idx = idxCount = 0;
  };

  auto&& FD_LINE = [&coords,&lines,&idx,&idxCount](float x, float y, float z,
                                                   bool endOfLine = false)
  {
    coords->point.set1Value(idx,x,y,z);
    lines->coordIndex.set1Value(idxCount++,idx++);
    if (endOfLine)
      lines->coordIndex.set1Value(idxCount++,-1);
  };

  switch (loc.getPosType())
  {
    case FFa3DLocation::CART_X_Y_Z:
      NEW_LINESHAPE(posSep);

      d = x < 0 ? -pd : pd;
      if (fabsf(x) > 3.0f*pd)
      {
        // X Arrow start
        FD_LINE( 0, 0, 0);
        FD_LINE( d, d/4, 0);
        FD_LINE( d,-d/4, 0);
        FD_LINE( 0, 0, 0, true);
        // X Line
        FD_LINE( d, 0, 0);
        FD_LINE( x-d, 0, 0, true);
        // X Arrow end
        FD_LINE( x, 0, 0);
        FD_LINE( x-d, d/4, 0);
        FD_LINE( x-d,-d/4, 0);
        FD_LINE( x, 0, 0, true);
      }
      else
      {
        // X Arrow start
        FD_LINE( 0, 0, 0);
        FD_LINE(-d, d/4, 0);
        FD_LINE(-d,-d/4, 0);
        FD_LINE( 0, 0, 0, true);
        // X Line
        FD_LINE(-2*d, 0, 0);
        FD_LINE(-d, 0, 0, true);
        FD_LINE( 0, 0, 0);
        FD_LINE( x, 0, 0, true);
        FD_LINE( x+2*d, 0, 0);
        FD_LINE( x+d, 0, 0, true);
        // X Arrow end
        FD_LINE( x, 0, 0);
        FD_LINE( x+d, d/4, 0);
        FD_LINE( x+d,-d/4, 0);
        FD_LINE( x, 0, 0, true);
      }

      d = y < 0 ? -pd : pd;
      if (fabsf(y) > 3.0f*pd)
      {
        // Y Arrow Start
        FD_LINE( x, 0, 0);
        FD_LINE( x-d/4, d, 0);
        FD_LINE( x+d/4, d, 0);
        FD_LINE( x, 0, 0, true);
        // Y Line
        FD_LINE( x, d, 0);
        FD_LINE( x, y-d, 0, true);
        // Y Arrow end
        FD_LINE( x, y, 0);
        FD_LINE( x-d/4, y-d, 0);
        FD_LINE( x+d/4, y-d, 0);
        FD_LINE( x, y, 0, true);
      }
      else
      {
        // Y Arrow start
        FD_LINE( x, 0, 0);
        FD_LINE( x-d/4,-d, 0);
        FD_LINE( x+d/4,-d, 0);
        FD_LINE( x, 0, 0, true);
        // Y Line
        FD_LINE( x,-2*d, 0);
        FD_LINE( x,-d, 0, true);
        FD_LINE( x, 0, 0);
        FD_LINE( x, y, 0, true);
        FD_LINE( x, y+2*d, 0);
        FD_LINE( x, y+d, 0, true);
        // Y Arrow end
        FD_LINE( x, y, 0);
        FD_LINE( x+d/4, y+d, 0);
        FD_LINE( x-d/4, y+d, 0);
        FD_LINE( x, y, 0, true);
      }

      d = z < 0 ? -pd : pd;
      if (fabsf(z) > 3.0f*pd)
      {
        // Z Arrow start
        FD_LINE( x, y, 0);
        FD_LINE( x-d/4, y, d);
        FD_LINE( x+d/4, y, d);
        FD_LINE( x, y, 0, true);
        // Z Line
        FD_LINE( x, y, d);
        FD_LINE( x, y, z-d, true);
        // Z Arrow end
        FD_LINE( x , y, z);
        FD_LINE( x-d/4, y, z-d);
        FD_LINE( x+d/4, y, z-d);
        FD_LINE( x , y, z, true);
      }
      else
      {
        // Z Arrow start
        FD_LINE( x, y, 0);
        FD_LINE( x-d/4, y,-d);
        FD_LINE( x+d/4, y,-d);
        FD_LINE( x, y, 0, true);
        // Z Line
        FD_LINE( x, y,-2*d);
        FD_LINE( x, y,-d, true);
        FD_LINE( x, y, 0);
        FD_LINE( x, y, z, true);
        FD_LINE( x, y, z+2*d);
        FD_LINE( x, y, z+d, true);
        // Z Arrow end
        FD_LINE( x, y, z);
        FD_LINE( x+d/4, y,z+d);
        FD_LINE( x-d/4, y,z+d);
        FD_LINE( x, y, z, true);
      }
      break;

    case FFa3DLocation::CYL_R_XR_Z:
    case FFa3DLocation::CYL_R_ZR_Y:
    case FFa3DLocation::CYL_R_YR_X:
      {
        FFa3DLocation tmp(loc);

        // This lambda needs the angle to be degrees
        auto&& FD_C_LINE = [&FD_LINE,&tmp](float r, float th, float l,
                                           bool endOfLine = false)
        {
          tmp[0] = FaVec3(r,th,l);
          FaVec3 v = tmp.translation();
          FD_LINE(v.x(),v.y(),v.z(),endOfLine);
        };

        NEW_LINESHAPE(posSep);

        // Using the FFa3DLocation data as cylindrical coordinates directly.
        // The following is approximately the same as above,
        // and the FD_C_LINE lambda converts from cylindrical coordinates to
        // cartesian, and builds the line.
        // In addition, the d-value needs to be converted to angular value
        // d-Like-angle = d/r

        x = static_cast<float>(loc[0][0]);
        y = static_cast<float>(loc[0][1]); // This is degrees
        z = static_cast<float>(loc[0][2]);

        d = x < 0 ? -pd : pd;
        float da = d * 180.0f/M_PI; // Convert to degrees d/r*180/pi = (d*180/pi)/r

        if (fabsf(x) > 2.0f*pd)
        {
          // X Arrow start
          FD_C_LINE( 0, 0, 0);
          FD_C_LINE( d, da/(4*d), 0);
          FD_C_LINE( d,-da/(4*d), 0);
          FD_C_LINE( 0, 0, 0, true);
          // X Line
          FD_C_LINE( d, 0, 0);
          FD_C_LINE( x-d, 0, 0, true);
          // X Arrow end
          FD_C_LINE( x,0,0);
          FD_C_LINE( x-d, da/(4*(x-d)), 0);
          FD_C_LINE( x-d,-da/(4*(x-d)), 0);
          FD_C_LINE( x, 0, 0, true);
        }
        else
        {
          // X Arrow start
          FD_C_LINE( 0, 0, 0);
          FD_C_LINE(-d, da/(4*-d), 0);
          FD_C_LINE(-d,-da/(4*-d), 0);
          FD_C_LINE( 0, 0, 0, true);
          // X Line
          FD_C_LINE(-2*d, 0, 0);
          FD_C_LINE(-d, 0, 0, true);
          FD_C_LINE( 0, 0, 0);
          FD_C_LINE( x, 0, 0, true);
          FD_C_LINE( x+2*d, 0, 0);
          FD_C_LINE( x+d, 0, 0, true);
          // X Arrow end
          FD_C_LINE( x,0,0);
          FD_C_LINE( x+d, da/(4*(x+d)), 0);
          FD_C_LINE( x+d,-da/(4*(x+d)), 0);
          FD_C_LINE( x, 0, 0, true);
        }

        d = y < 0 ? -pd : pd;
        da = d * 180.0f/M_PI; // Convert to degrees: d/r*180/pi = (d*180/pi)/r

        if (fabsf(y) > fabsf(2.0f*da/x))
        {
          // Y Arrow Start
          FD_C_LINE( x, 0, 0);
          FD_C_LINE( x-d/4, da/fabsf(x-d/4), 0);
          FD_C_LINE( x+d/4, da/fabsf(x+d/4), 0);
          FD_C_LINE( x, 0, 0, true);

          // Y Line

          // p is dividing the curve into steps of 2 degrees.
          if (int p = static_cast<int>(fabsf(0.5f*y - da/fabsf(x))); p > 0)
            for (int i = 0; i <= p; i++)
              FD_C_LINE( x, da/x + i*(y-2*da/x)/p, 0, i == p);

          // Y Arrow end
          FD_C_LINE( x, y, 0);
          FD_C_LINE( x-d/4, y-da/fabsf(x-d/4), 0);
          FD_C_LINE( x+d/4, y-da/fabsf(x+d/4), 0);
          FD_C_LINE( x, y, 0, true);
        }
        else
        {
          // Y Arrow start
          FD_C_LINE( x, 0, 0);
          FD_C_LINE( x-d/4, -da/fabsf(x-d/4), 0);
          FD_C_LINE( x+d/4, -da/fabsf(x+d/4), 0);
          FD_C_LINE( x, 0, 0, true);

          // Y Line

          // p is dividing the curve into steps of 0.5 degrees.
          if (int p = static_cast<int>(fabsf(da/x)*2.0f); p > 0)
            for (int i = 0; i <= p; i++)
              FD_C_LINE( x,-2*da/abs(x) + i*(da/abs(x))/p, 0, i == p);

          // p is dividing the curve into steps of 2 degrees.
          if (int p = static_cast<int>(fabsf(y)); p > 0)
            for (int i = 0; i <= p; i++)
              FD_C_LINE( x, i*y/p, 0, i == p);

          // p is dividing the curve into steps of 0.5 degrees.
          if (int p = static_cast<int>(fabsf(da/x)*2.0f); p > 0)
            for (int i = 0; i <= p; i++)
              FD_C_LINE( x, (y+2*da/abs(x)) - i*(da/abs(x))/p, 0, i == p);

          // Y Arrow end
          FD_C_LINE( x, y, 0);
          FD_C_LINE( x+d/4, y + da/fabsf(x+d/4), 0);
          FD_C_LINE( x-d/4, y + da/fabsf(x-d/4), 0);
          FD_C_LINE( x, y, 0, true);
        }

        d = z < 0 ? -pd : pd;
        if (fabsf(z) > 2.0f*pd)
        {
          // Z Arrow start
          FD_C_LINE( x, y, 0);
          FD_C_LINE( x-d/4, y, d);
          FD_C_LINE( x+d/4, y, d);
          FD_C_LINE( x, y, 0, true);
          // Z Line
          FD_C_LINE( x, y, d);
          FD_C_LINE( x, y, z-d, true);
          // Z Arrow end
          FD_C_LINE( x , y, z);
          FD_C_LINE( x-d/4, y, z-d);
          FD_C_LINE( x+d/4, y, z-d);
          FD_C_LINE( x, y, z, true);
        }
        else
        {
          // Z Arrow start
          FD_C_LINE( x, y, 0);
          FD_C_LINE( x-d/4, y,-d);
          FD_C_LINE( x+d/4, y,-d);
          FD_C_LINE( x, y, 0, true);

          // Z Line
          FD_C_LINE( x, y,-2*d);
          FD_C_LINE( x, y,-d, true);
          FD_C_LINE( x, y, 0);
          FD_C_LINE( x, y, z, true);
          FD_C_LINE( x, y, z+2*d);
          FD_C_LINE( x, y, z+d, true);

          // Z Arrow end
          FD_C_LINE( x, y, z);
          FD_C_LINE( x+d/4, y,z+d);
          FD_C_LINE( x-d/4, y,z+d);
          FD_C_LINE( x, y, z, true);
        }
      }
      break;

    default:
      break;
    }

  // Rotation

  SoSeparator* rotSep = new SoSeparator;

  FaVec3 to = posRefCS * pos;
  if ( rotRefCS != FaMat34() )
  {
    FaVec3 from = rotRefCS.translation();
    NEW_LINESHAPE(rotSep);
    FD_LINE(from.x(), from.y(), from.z());
    FD_LINE(to.x(), to.y(), to.z(), true);
  }

  SoTransform* rotXf = new SoTransform;
  rotSep->addChild(rotXf);
  FaMat34 rot(rotRefCS.direction(), to);
  rotXf->setMatrix(FdConverter::toSbMatrix(rot));
  const float s = FdSymbolDefs::getSymbolScale();

  NEW_LINESHAPE(rotSep);

  FD_LINE( 1.15*s, 0, 0);
  FD_LINE( 1.65*s, 0, 0, true);
  FD_LINE( 1.75*s,-0.075*s, 0.125*s);
  FD_LINE( 1.75*s, 0.075*s,-0.125*s, true);
  FD_LINE( 1.75*s, 0.075*s, 0.125*s);
  FD_LINE( 1.75*s,-0.075*s,-0.125*s, true);

  FD_LINE( 0, 1.15*s, 0);
  FD_LINE( 0, 1.65*s, 0, true);
  FD_LINE( 0.075*s, 1.75*s, 0.125*s);
  FD_LINE( 0, 1.75*s, 0, true);
  FD_LINE(-0.075*s, 1.75*s, 0.125*s);
  FD_LINE( 0.075*s, 1.75*s,-0.125*s, true);

  FD_LINE( 0, 0, 1.15*s);
  FD_LINE( 0, 0, 1.65*s, true);

  FD_LINE( 0.075*s,-0.125*s, 1.75*s);
  FD_LINE(-0.075*s,-0.125*s, 1.75*s);
  FD_LINE( 0.075*s, 0.125*s, 1.75*s);
  FD_LINE(-0.075*s, 0.125*s, 1.75*s, true);

  our3DLocationSep = new SoSeparator;
  our3DLocationSep->addChild(offset);
  our3DLocationSep->addChild(posSep);
  our3DLocationSep->addChild(rotSep);

  FdDB::getExtraGraphicsSep()->addChild(our3DLocationSep);
}


void FdExtraGraphics::hide3DLocation()
{
  if (our3DLocationSep)
    FdDB::getExtraGraphicsSep()->removeChild(our3DLocationSep);

  our3DLocationSep = NULL;
}
