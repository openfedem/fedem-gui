// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdCtrlGrid.H"
#include "vpmDisplay/FdCtrlGridKit.H"
#include "vpmDisplay/FdCtrlDB.H"

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <cmath>


namespace
{
  // Grid variables
  FdCtrlGridKit* gridNode = NULL;

  float gridWidth  = 100.0f;
  float gridHeight = 100.0f; //Grid area

  bool  isGridOn  = false;
  float gridSizeX = 1.0f; //Square size
  float gridSizeY = 1.0f;

  // Snap variables
  bool  isSnapOn      = false;
  float snapDistanceX = 0.25f; // Note! Make sure default values
  float snapDistanceY = 0.25f; // divide equally to gridSize

  // Grid color
  float red   = 0.5f;
  float green = 0.5f;
  float blue  = 0.5f;

  // Adjusts the snap distance so it relates to the grid size.
  float adjustSnap(float snap, float gridSize)
  {
    if (snap == 0.0f) snap = 0.1f;

    float numPt = fabsf(gridSize/snap);
    if (numPt >= 1.0f)
      return gridSize / floorf(numPt);

    int numGrid = static_cast<int>(floorf(1.0f/numPt));
    numPt -= numGrid;
    if (numPt == 0.0f)
      return gridSize * numGrid;

    return gridSize * (numGrid + 1.0f/floorf(1.0f/numPt));
  }
}


/////////////////////////////
//   Grid
////////////////////////////

void FdCtrlGrid::addGrid() // Adds a grid node to the scene graph
{
  FdCtrlGrid::removeGrid();

  gridNode = new FdCtrlGridKit;
  gridNode->ref();

  FdCtrlDB::getCtrlExtraGraphicsRoot()->addChild(gridNode);

  // Calculate number of rows and columns, that make the middle of
  // the grid equal with the midpoint of the scene

  int numX = static_cast<int>(gridWidth/gridSizeX) + 1;
  int numY = static_cast<int>(gridHeight/gridSizeY)+ 1;
  float sizeX = numX * gridSizeX; //Adjusted grid width
  float sizeY = numY * gridSizeY; //Adjusted grid higth

  SoMaterial* mat = SO_GET_PART(gridNode,"appearance.material", SoMaterial);
  mat->diffuseColor.setValue( red, green, blue);
  mat->ambientColor.setValue( red, green, blue);
  mat->specularColor.setValue(red, green, blue);

  SoDrawStyle* style = SO_GET_PART(gridNode,"appearance.style", SoDrawStyle);
  style->style.setValue(SoDrawStyle::LINES);
  style->lineWidth.setValue(1);
  style->linePattern.setValue(0xf0f0);

  SoCoordinate3* coord = SO_GET_PART(gridNode,"coords", SoCoordinate3);
  SoIndexedLineSet* line = SO_GET_PART(gridNode,"lines", SoIndexedLineSet);

  const float z = 0.0f; // The grid lies in the (z=0)-plane

  // Draw the horisontal lines in the grid
  float coordX = sizeX;
  float coordY = sizeY;

  for (int i = 0; i < numY; i++)
  {
    int j = 4*i;

    coord->point.set1Value(j,   -coordX,  coordY, z);
    coord->point.set1Value(j+1,  coordX,  coordY, z);
    coordY -= gridSizeY;
    coord->point.set1Value(j+2,  coordX,  coordY, z);
    coord->point.set1Value(j+3, -coordX,  coordY, z);
    coordY -= gridSizeY;

    int32_t index[5] = {j,j+1,j+2,j+3,-1};
    line->coordIndex.setValues(j, 5, index);
  }

  // Draw the vertical lines in the grid
  coordX = sizeX;
  coordY = sizeY;

  for (int k = numY; k <= numX+numY; k++)
  {
    int l = 4*k;

    coord->point.set1Value(l,   -coordX, -coordY, z);
    coord->point.set1Value(l+1, -coordX,  coordY, z);
    coordX -= gridSizeX;
    coord->point.set1Value(l+2, -coordX,  coordY, z);
    coord->point.set1Value(l+3, -coordX, -coordY, z);
    coordX -= gridSizeX;

    int32_t index[5] = {l, l+1, l+2, l+3, -1};
    line->coordIndex.setValues(l, 5, index);
  }

  // Frame around the grid
  int l = 4*(numX+numY);

  coord->point.set1Value(l+4,  sizeX, -sizeY, z);
  coord->point.set1Value(l+5,  sizeX,  sizeY, z);
  coord->point.set1Value(l+6, -sizeX,  sizeY, z);
  coord->point.set1Value(l+7, -sizeX, -sizeY, z);
  coord->point.set1Value(l+8,  sizeX, -sizeY, z);

  int32_t index[6] = {l+4, l+5, l+6, l+7, l+8, -1};
  line->coordIndex.setValues(l, 6, index);

  isGridOn = true;
}

void FdCtrlGrid::removeGrid() // Remove the grid node to the scene graph
{
  if (isGridOn)
    FdCtrlDB::getCtrlExtraGraphicsRoot()->removeChild(gridNode);

  isGridOn = false;
}


void FdCtrlGrid::setGridMaterial(float r, float g, float b)
{
  red   = fabsf(r) < 1.0f ? fabsf(r) : 1.0f;
  green = fabsf(g) < 1.0f ? fabsf(g) : 1.0f;
  blue  = fabsf(b) < 1.0f ? fabsf(b) : 1.0f;
}


float FdCtrlGrid::getGridSizeX()
{
  return gridSizeX;
}

float FdCtrlGrid::getGridSizeY()
{
  return gridSizeY;
}

void FdCtrlGrid::setGridSize(float sizeX, float sizeY)
{
  gridSizeX = fabsf(sizeX);
  gridSizeY = fabsf(sizeY);

  if (isGridOn)
    FdCtrlGrid::addGrid();
}


bool FdCtrlGrid::getGridState()
{
  return isGridOn;
}

void FdCtrlGrid::setGridState(bool state)
{
  if (state)
    FdCtrlGrid::addGrid();
  else
    FdCtrlGrid::removeGrid();
}


//////////////////
//   Snap
/////////////////

bool FdCtrlGrid::getSnapState()
{
  return isSnapOn;
}

void FdCtrlGrid::setSnapState(bool s)
{
  isSnapOn = s;
}


float FdCtrlGrid::getSnapDistanceX()
{
  return snapDistanceX;
}

float FdCtrlGrid::getSnapDistanceY()
{
  return snapDistanceY;
}

void FdCtrlGrid::setSnapDistance(float X, float Y)
{
  snapDistanceX = adjustSnap(X,gridSizeX);
  snapDistanceY = adjustSnap(Y,gridSizeY);
}


void FdCtrlGrid::snapToNearestPoint(SbVec3f& vec)
{
  if (!isSnapOn) return;

  int nX = (vec[0] > 0.0f ? 0.5f : -0.5f)*snapDistanceX + vec[0]/snapDistanceX;
  int nY = (vec[1] > 0.0f ? 0.5f : -0.5f)*snapDistanceY + vec[1]/snapDistanceY;
  vec[0] = nX * snapDistanceX;
  vec[1] = nY * snapDistanceY;
}
