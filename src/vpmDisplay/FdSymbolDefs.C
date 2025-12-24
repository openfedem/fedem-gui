// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdSymbolKit.H"

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#ifdef USE_SMALLCHANGE
#include <SmallChange/nodes/SmDepthBuffer.h>
#endif
#include <functional>


namespace
{
  // Scale and style

  SoTransform* GlobalSymbolScale      = NULL;
  SoTransform* GlobalAxialSymbolScale = NULL;
  SoDrawStyle* GlobalSymbolStyle      = NULL;

  // Materials and symbols

  std::array<SoMaterial*,FdSymbolDefs::MATERIAL_COUNT> Materials;
  std::array<FdSymbolKit*,FdSymbolDefs::SYMBOL_COUNT> Symbols;

  SoMaterial* new_material(float r, float g, float b)
  {
    SoMaterial* material = new SoMaterial;

    material->ref();
    material->diffuseColor.setValue(r,g,b);
    material->ambientColor.setValue(r,g,b);
    material->emissiveColor.setValue(r,g,b);

    return material;
  }

  FdSymbolKit* make_symbol(const float* XYZ, const int32_t* index,
                           int nCoord, int nIndex, char scale = 'G')
  {
    FdSymbolKit* result = new FdSymbolKit;
    result->ref();

    if (scale)
      result->setPart("scale", scale == 'A' ? GlobalAxialSymbolScale : GlobalSymbolScale);
    result->setPart("style", GlobalSymbolStyle);

    SoCoordinate3* coords = new SoCoordinate3;
    coords->point.setValuesPointer(3*nCoord,XYZ);
    result->setPart("coords", coords);

    SoIndexedLineSet* nonAxis = new SoIndexedLineSet;
    nonAxis->coordIndex.setValues(0,nIndex,index);
    result->setPart("nonAxis", nonAxis);

    return result;
  }

  void setMaterialColor(FdSymbolDefs::MaterialType idx,
                        const FdSymbolDefs::FdColor& color)
  {
    if (!Materials[idx]) return;

    float rgb[3];
    for (int i = 0; i < 3; i++)
      rgb[i] = color[i] > 1.0f ? 1.0f : (color[i] < 0.0f ? 0.0f : color[i]);

    Materials[idx]->diffuseColor.setValue(rgb);
    Materials[idx]->ambientColor.setValue(rgb);
    Materials[idx]->emissiveColor.setValue(rgb);
  }
}


////////////////////////////////////////////////////////////////////////////////

void FdSymbolDefs::init()
{
  ////////////////////////////////////////////////////
  // Lambda functions for making the different symbols
  ////////////////////////////////////////////////////

#define LAMBDA_FUNC(NAME) std::function<FdSymbolKit*()> NAME = []()

LAMBDA_FUNC(makeCoordSyst)
{
  static float CSCoordsVal[22*3] = { 0, 0, 0, // 0
				     1, 0, 0, // 1
				     0, 1, 0, // 2
				     0, 0, 1, // 3

				     0.75, 0.075,0, // 4 XAxis Arrow
				     0.75,-0.075,0, // 5

				     0.075, 0.75,0, // 6 YAxis Arrow
				    -0.075, 0.75,0, // 7

				     0.075, 0,0.75, // 8 ZAxis Arrow
				    -0.075, 0,0.75, // 9

				     1.075,-0.075, .125, // 10 XAxis Label
				     1.075, 0.075,-.125, // 11
				     1.075, 0.075, .125, // 12
				     1.075,-0.075,-.125, // 13

				     0.075, 1.075, .125, // 14 YAxis Label
				     0,     1.075,  0  , // 15
				    -0.075, 1.075, .125, // 16
				     0.075, 1.075,-.125, // 17

				     0.075,-.125 ,1.075, // 18 ZAxis Arrow
				    -0.075,-.125 ,1.075, // 19
				     0.075, .125 ,1.075, // 20
				    -0.075, .125 ,1.075 }; // 21

  static int32_t nonAxisIndex[35] = { 0,1,4,5,1,-1,  10,11,-1,  12,13,-1, // X
				      0,2,6,7,2,-1,  14,15,-1,  16,17,-1, // Y
				      0,3,8,9,3,-1,  18,19,20,21,-1 };    // Z

  return make_symbol(CSCoordsVal,nonAxisIndex,22,35);
};


LAMBDA_FUNC(makeLinkCoordSyst)
{
  static float CSCoordsVal[22*3] = { 0, 0, 0, // 0
				     1, 0, 0, // 1
				     0, 1, 0, // 2
				     0, 0, 1, // 3

				     0.7, 0  , 0, // 4 XAxis plane start
				     0.7, 0.7, 0, // 5 XY plane

				     0, 0.7, 0  , // 6 YAxis plane start
				     0, 0.7, 0.7, // 7 YZ plane

				     0  , 0, 0.7, // 8 ZAxis plane start
				     0.7, 0, 0.7, // 9 ZX plane

				     1.075,-0.075, .125, // 10 XAxis Label
				     1.075, 0.075,-.125, // 11
				     1.075, 0.075, .125, // 12
				     1.075,-0.075,-.125, // 13

				     0.075, 1.075, .125, // 14 YAxis Label
				       0,   1.075,  0  , // 15
				    -0.075, 1.075, .125, // 16
				     0.075, 1.075,-.125, // 17

				     0.075,-.125 ,1.075, // 18 ZAxis Label
				    -0.075,-.125 ,1.075, // 19
				     0.075, .125 ,1.075, // 20
				    -0.075, .125 ,1.075 }; // 21

  static int32_t nonAxisIndex[38] = { 0,1,-1,  4,5,6,-1,  10,11,-1,  12,13,-1, // X
				      0,2,-1,  6,7,8,-1,  14,15,-1,  16,17,-1, // Y
				      0,3,-1,  8,9,4,-1,  18,19,20,21,-1 };    // Z

  return make_symbol(CSCoordsVal,nonAxisIndex,22,38);
};


LAMBDA_FUNC(makeInternalLinkCoordSyst)
{
  static float CSCoordsVal[22*3] = { 0, 0, 0, // 0
				     1, 0, 0, // 1
				     0, 1, 0, // 2
				     0, 0, 1, // 3

				     0.7, 0  , 0, // 4 XAxis plane start
				     0.7, 0.7, 0, // 5 XY plane

				     0, 0.7, 0  , // 6 YAxis plane start
				     0, 0.7, 0.7, // 7 YZ plane

				     0  , 0, 0.7, // 8 ZAxis plane start
				     0.7, 0, 0.7, // 9 ZX plane

				     1.075,-0.075, .125, // 10 XAxis Label
				     1.075, 0.075,-.125, // 11
				     1.075, 0.075, .125, // 12
				     1.075,-0.075,-.125, // 13

				     0.075, 1.075, .125, // 14 YAxis Label
				       0,   1.075,  0  , // 15
				    -0.075, 1.075, .125, // 16
				     0.075, 1.075,-.125, // 17

				     0.075,-.125 ,1.075, // 18 ZAxis Label
				    -0.075,-.125 ,1.075, // 19
				     0.075, .125 ,1.075, // 20
				    -0.075, .125 ,1.075 }; // 21

  static int32_t nonAxisIndex[35] = { 0,1,-1,  4,6,-1,  10,11,-1,  12,13,-1, // X
				      0,2,-1,  6,8,-1,  14,15,-1,  16,17,-1, // Y
				      0,3,-1,  8,4,-1,  18,19,20,21,-1 };    // Z

  return make_symbol(CSCoordsVal,nonAxisIndex,22,35);
};


LAMBDA_FUNC(makeForce)
{
  static float CoordsVal[7*3] = { 0, 0, 0, // 0
				  2, 0, 0, // 1
				  0.4, 0  , 0  , // 2
				  0.5, 0.2, 0  , // 3
				  0.5, 0  , 0.2, // 4
				  0.5,-0.2, 0  , // 5
				  0.5, 0  ,-0.2 }; // 6

  static int32_t nonAxisIndex[19] = { 0,1,-1, 2,3,0,-1, 2,4,0,-1, 2,5,0,-1, 2,6,0,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,7,19);
};


LAMBDA_FUNC(makeTorque)
{
  static float CoordsVal[12*3] = { 0, 0, 0, // 0
				   2, 0, 0, // 1
				   0.4,   0,   0, // 2
				   0.5, 0.2,   0, // 3
				   0.5,   0, 0.2, // 4
				   0.5,-0.2,   0, // 5
				   0.5,   0,-0.2, // 6
				   0.8,   0,   0, // 7
				   0.9, 0.2,   0, // 8
				   0.9,   0, 0.2, // 9
				   0.9,-0.2,   0, // 10
				   0.9,   0,-0.2 }; // 11

  static int32_t nonAxisIndex[35] = { 0,1,-1, 2,3,0,-1, 2,4,0,-1, 2,5,0,-1, 2,6,0,-1,
				      7,8,2,-1, 7,9,2,-1, 7,10,2,-1, 7,11,2,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,12,35);
};


LAMBDA_FUNC(makeAxialDamper)
{
  static float coords[16*3] = {  0, 0, 0, // 0
				.2, 0, 0, // 1
				.2,  0,-.75, // 2
				.2, .75,  0, // 3
				.2,  0, .75, // 4
				.2,-.75,  0, // 5
				.8,  0,-.75, // 6
				.8, .75,  0, // 7
				.8,  0, .75, // 8
				.8,-.75,  0, // 9
				.4,  0,-.7,  // 10
				.4, .7,  0,  // 11
				.4,  0, .7,  // 12
				.4,-.7,  0,  // 13
				.4,  0,  0,  // 14
				 1,  0,  0 }; // 15

  static int32_t nonAxisIndex[50] = { 0, 1,-1,14,15,-1,
				      1, 2, 3, 1, 4, 3,-1,
				      1, 5, 4,-1,
				      5, 2,-1,          // Tupp

				      2, 6,-1,
				      3, 7,-1,
				      4, 8,-1,          // Langsgaaende linjer
				      5, 9,-1,

				      6, 7, 8, 9, 6,-1, // Den aapne enden

				      10,11,12,13,10,14,11,-1,12,14,13,-1 }; // Stempel

  return make_symbol(coords,nonAxisIndex,16,50,'A');
};


LAMBDA_FUNC(makeAxialSpring)
{
  const double piOver6 = M_PI/6.0;
  static float sprPts[66*3];
  memset(sprPts,0,sizeof(float)*66*3);
  sprPts[3] = 0.2f;
  sprPts[6] = 0.2f;
  sprPts[8] = 0.8f;
  for (int i = 0; i < 60; i++)
  {
    sprPts[ 9+3*i] = 0.2f + 0.01f*i;
    sprPts[10+3*i] = 0.85*sin(piOver6*i);
    sprPts[11+3*i] = 0.85*cos(piOver6*i);
  }
  sprPts[3*63] = sprPts[3*63+2] = 0.8f;
  sprPts[3*64] = 0.8f;
  sprPts[3*65] = 1.0f;

  int32_t nonAxisIndex[67];
  for (int32_t idx = 0; idx < 66; idx++)
    nonAxisIndex[idx] = idx;
  nonAxisIndex[66] = -1;

  return make_symbol(sprPts,nonAxisIndex,66,67,'A');
};


LAMBDA_FUNC(makeSensor)
{
  static float CoordsVal[27*3] = { 0, 0, 0, // 0
				   1, 0, 0, // 1
				   0.5,0  ,0, // 2
				   0.5,1  ,1, // 3
				   0.5,1.1,1, // 4
				   0.5,1.9,1, // 5
				   0.5,2  ,1, // 6
				   0.5,1.9,1.5, // 7
				   0.5,1.1,1.5, // 8
				   0.5,1.8,1.5, // 9
				   0.5,1.7,1.5, // 10
				   0.5,1.6,1.5, // 11
				   0.5,1.5,1.5, // 12
				   0.5,1.4,1.5, // 13
				   0.5,1.3,1.5, // 14
				   0.5,1.2,1.5, // 15
				   0.5,1.3,1.3, // 16
				   0.5,1.7  ,1.3, // 17
				   0.5,1.35 ,1.4, // 18
				   0.5,1.5  ,1.4, // 19
				   0.5,1.65 ,1.4, // 20
				   0.5,1.25 ,1.45, // 21
				   0.5,1.412,1.45, // 22
				   0.5,1.588,1.45, // 23
				   0.5,1.75 ,1.45, // 24
				   0.5,1.47 ,1.07, // 25
				   0.5,1.53 ,1.05 }; // 26

  static int32_t nonAxisIndex[42] = { 0,1,-1, 2,3,6,-1, 5,7,8,4,-1,
				      8,16,-1,
				     15,21,-1,
				     14,18,-1,
				     13,22,-1,
				     12,19,-1,
				     11,23,25,26,23,-1,
				     10,20,-1,
				      9,24,-1,
				      7,17,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,27,42,'A');
};


LAMBDA_FUNC(makeSmallSingleGage)
{
  static float CoordsVal[3*3] = { -0.5,  0,   0, // 0
				   0.5,  0,   0, // 1
				   0.33,-0.1, 0 }; // 2

  static int32_t nonAxisIndex[4] = { 0,1,2,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,3,4,false);
};


LAMBDA_FUNC(makeSmallDoubleGage90)
{
  static float CoordsVal[8*3] = { -0.5,  0  , 0, // 0
				   0.5,  0  , 0, // 1
				   0.33,-0.1, 0, // 2
				     0, -0.5, 0, // 3
				     0,  0.5, 0, // 4
				   0.1,  0.33,0, // 5
				     0,  0.4, 0, // 6
				   0.1,  0.23,0 }; // 7

  static int32_t nonAxisIndex[11] = { 0,1,2,-1, 3,4,5,-1, 6,7,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,8,11,false);
};


LAMBDA_FUNC(makeSmallTripleGage60)
{
  static float CoordsVal[15*3] = { -0.5,  0  , 0, // 0 First leg
				    0.5,  0  , 0, // 1
				    0.33,-0.1, 0, // 2

				   -0.25  , 0.433 , 0, // 3 Third leg
				    0.25  ,-0.433 , 0, // 4
				   -0.0784, 0.3358, 0, // 5
				   -0.2   , 0.3464, 0, // 6
				   -0.0284, 0.2492, 0, // 7
				   -0.15  , 0.2598, 0, // 8
				    0.0216, 0.1626, 0, // 9

				   -0.25 , -0.433,  0, // 10 Second leg
				    0.25 ,  0.433,  0, // 11
				    0.252,  0.2358, 0, // 12
				    0.2   , 0.3464, 0, // 13
				    0.2016, 0.1492, 0 }; // 14

  static int32_t nonAxisIndex[21] = { 0,1,2,-1, 4,3,5,-1, 6,7,-1, 8,9,-1,
				      10,11,12,-1, 13,14,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,15,21,false);
};


LAMBDA_FUNC(makeSmallTripleGage45)
{
  static float CoordsVal[15*3] = { -0.5,  0  , 0, // 0
				    0.5,  0  , 0, // 1
				    0.33,-0.1 ,0, // 2

				      0, -0.5, 0, // 3
				      0,  0.5, 0, // 4
				    0.1,  0.33,0, // 5
				      0,  0.4, 0, // 6
				    0.1,  0.23,0, // 7
				      0,  0.3, 0, // 8
				    0.1,  0.13,0, // 9

				   -0.353, -0.353,  0, // 10
				    0.353,  0.353,  0, // 11
				    0.304,  0.1626, 0, // 12
				    0.2828, 0.2828, 0, // 13
				    0.2333, 0.0919, 0 }; // 14

  static int32_t nonAxisIndex[21] = { 0,1,2,-1, 3,4,5,-1, 6,7,-1, 8,9,-1,
				      10,11,12,-1, 13,14,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,15,21,false);
};


LAMBDA_FUNC(makeLargeSingleGage)
{
  static float CoordsVal[10*3] = { -0.5,  0  , 1, // 0
				    0.5,  0  , 1, // 1
				   0.33,-0.1 , 1, // 2

				      0,    0, 0, // 3 Line from FE model to symbol
				      0,    0, 1, // 4
				   -0.7, -0.7, 1, // 5 Square to embrace symbol
				    0.7, -0.7, 1, // 6
				    0.7,  0.7, 1, // 7
				   -0.7,  0.7, 1 }; // 8

  static int32_t nonAxisIndex[15] = { 0,1,2,-1,
				      3,4,-1, 5,6,7,8,5,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,10,15);
};


LAMBDA_FUNC(makeLargeDoubleGage90)
{
  static float CoordsVal[14*3] = { -0.5,  0  , 1, // 0
				    0.5,  0  , 1, // 1
				    0.33,-0.1, 1, // 2

				      0, -0.5, 1, // 3
				      0,  0.5, 1, // 4
				    0.1, 0.33, 1, // 5
				      0,  0.4, 1, // 6
				    0.1, 0.23, 1, // 7

				      0,  0  , 0, // 8 Line from FE model to symbol
				      0,  0  , 1, // 9

				   -0.7, -0.7, 1, // 10 Square to embrace symbol
				    0.7, -0.7, 1, // 11
				    0.7,  0.7, 1, // 12
				   -0.7,  0.7, 1 }; // 13

  static int32_t nonAxisIndex[20] = { 0,1,2,-1, 3,4,5,-1, 6,7,-1,
				      8,9,-1, 10,11,12,13,10,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,14,20);
};


LAMBDA_FUNC(makeLargeTripleGage60)
{
  static float CoordsVal[21*3] = { -0.5,  0  , 1, // 0 First leg
				    0.5,  0  , 1, // 1
				    0.33,-0.1, 1, // 2

				   -0.25,   0.433 , 1, // 3 Third leg
				    0.25,  -0.433 , 1, // 4
				   -0.0784, 0.3358, 1, // 5
				   -0.20  , 0.3464, 1, // 6
				   -0.0284, 0.2492, 1, // 7
				   -0.15  , 0.2598, 1, // 8
				    0.0216, 0.1626, 1, // 9

				   -0.25 , -0.433,  1, // 10 Second leg
				    0.25 ,  0.433,  1, // 11
				    0.252,  0.2358, 1, // 12
				    0.2   , 0.3464, 1, // 13
				    0.2016, 0.1492, 1, // 14

				    0,    0  , 0, // 15 Line from FE model to symbol
				    0,    0  , 1, // 16
				   -0.7, -0.7, 1, // 17 Square to embrace symbol
				    0.7, -0.7, 1, // 18
				    0.7,  0.7, 1, // 19
				   -0.7,  0.7, 1 }; // 20

  static int32_t nonAxisIndex[30] = { 0,1,2,-1, 4,3,5,-1, 6,7,-1, 8,9,-1,
				      10,11,12,-1, 13,14,-1,
				      15,16,-1, 17,18,19,20,17,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,21,30);
};


LAMBDA_FUNC(makeLargeTripleGage45)
{
  static float CoordsVal[21*3] = { -0.5,  0  , 1, // 0
				    0.5,  0  , 1, // 1
				    0.33,-0.1, 1, // 2

				      0, -0.5, 1, // 3
				      0,  0.5, 1, // 4
				    0.1,  0.33,1, // 5
				      0,  0.4, 1, // 6
				    0.1,  0.23,1, // 7
				      0,  0.3, 1, // 8
				    0.1,  0.13,1, // 9

				   -0.353, -0.353,  1, // 10
				    0.353,  0.353,  1, // 11
				    0.304,  0.1626, 1, // 12
				    0.2828, 0.2828, 1, // 13
				    0.2333, 0.0919, 1, // 14

				    0,    0  , 0, // 15 Line from FE model to symbol
				    0,    0  , 1, // 16
				   -0.7, -0.7, 1, // 17 Square to embrace symbol
				    0.7, -0.7, 1, // 18
				    0.7,  0.7, 1,  // 19
				   -0.7,  0.7, 1 }; // 20

  static int32_t nonAxisIndex[30] = { 0,1,2,-1, 3,4,5,-1, 6,7,-1, 8,9,-1,
				      10,11,12,-1, 13,14,-1,
				      15,16,-1, 17,18,19,20,17,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,21,30);
};


LAMBDA_FUNC(makeRevJoint)
{
  static float CoordsVal[23*3] = { 0, 0, 0,   // 0
				   0, 0, 1.5, // 1
				   0, 0,-1.5, // 2

				   1, 0, 0,   // 3

				   0.92387953,0.38268343,0, // 4
				   0.70710678,0.70710678,0, // 5
				   0.38268343,0.92387953,0, // 6

				   0, 1, 0,   // 7

				   -0.38268343,0.92387953,0, // 8
				   -0.70710678,0.70710678,0, // 9
				   -0.92387953,0.38268343,0, // 10

				   -1, 0, 0,  // 11

				   -0.92387953,-0.38268343,0, // 12
				   -0.70710678,-0.70710678,0, // 13
				   -0.382683433,-0.9238795,0, // 14

				   0,-1, 0,   // 15

				   0.38268343,-0.92387953,0,  // 16
				   0.70710678,-0.70710678,0,  // 17
				   0.92387953,-0.38268343,0,  // 18

				    0.075,-.125, 1.575, // 19 ZAxis Label
				   -0.075,-.125, 1.575, // 20
				    0.075, .125, 1.575, // 21
				   -0.075, .125, 1.575 }; // 22

  static int32_t nonAxisIndex[26] = { 1,2,-1,
				      3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,3,-1,
				      19,20,21,22,-1 }; // Z label

  return make_symbol(CoordsVal,nonAxisIndex,23,26);
};


LAMBDA_FUNC(makeRevJointMaster)
{
  static float CoordsVal[6*3] = { 0, 0, 0, // 0
				  1.3, 0, 0, // 1
				  1.375,-0.075, .125, // 2 XAxis Label
				  1.375, 0.075,-.125, // 3
				  1.375, 0.075, .125, // 4
				  1.375,-0.075,-.125 }; // 5

  static int32_t nonAxisIndex[9] = { 0,1,-1,
				     2,3,-1, 4,5,-1}; // X

  return make_symbol(CoordsVal,nonAxisIndex,6,9);
};


LAMBDA_FUNC(makeRevJointSlave)
{
  static float CoordsVal[10*3] = { 0, 0, 0, // 0
				   1, 0, 0, // 1

				   0.91, -0.15, 0,     // 2
				   0.8,  -0.38268343,0,// 3
				   1.04, -0.25, 0,     // 4

				   1.02 ,-0.45, 0,     // 5

				   1.375,-0.075, .125, // 6 XAxis Label
				   1.375, 0.075,-.125, // 7
				   1.375, 0.075, .125, // 8
				   1.375,-0.075,-.125 }; // 9

  static int32_t nonAxisIndex[17] = { 0,1,-1,  1,2,3,-1,  1,4,5,-1,
				      6,7,-1,  8,9,-1 }; // X

  return make_symbol(CoordsVal,nonAxisIndex,10,17);
};


LAMBDA_FUNC(makeBallJoint)
{
  static float CoordsVal[49*3] = { 0, 0, 0, // 0

				   1,   0, 0, // 1
				   1.3, 0, 0, // 2
				  -1,   0, 0, // 3
				  -1.3, 0, 0, // 4

				   0, 1,   0, // 5
				   0, 1.3, 0, // 6
				   0,-1,   0, // 7
				   0,-1.3, 0, // 8

				   0, 0,   1, // 9
				   0, 0, 1.3, // 10
				   0, 0,  -1, // 11
				   0, 0,-1.3, // 12

				   // Circle in XY

				    0.92387953, 0.38268343,0, // 13
				    0.70710678, 0.70710678,0, // 14
				    0.38268343, 0.92387953,0, // 15

				   -0.38268343, 0.92387953,0, // 16
				   -0.70710678, 0.70710678,0, // 17
				   -0.92387953, 0.38268343,0, // 18

				   -0.92387953,-0.38268343,0, // 19
				   -0.70710678,-0.70710678,0, // 20
				   -0.38268343,-0.92387953,0, // 21

				    0.38268343,-0.92387953,0, // 22
				    0.70710678,-0.70710678,0, // 23
				    0.92387953,-0.38268343,0, // 24

				   // Circle in XZ

				    0.92387953,0, 0.38268343, // 25
				    0.70710678,0, 0.70710678, // 26
				    0.38268343,0, 0.92387953, // 27

				   -0.38268343,0, 0.92387953, // 28
				   -0.70710678,0, 0.70710678, // 29
				   -0.92387953,0, 0.38268343, // 30

				   -0.92387953,0,-0.38268343, // 31
				   -0.70710678,0,-0.70710678, // 32
				   -0.38268343,0,-0.92387953, // 33

				    0.38268343,0,-0.92387953, // 34
				    0.70710678,0,-0.70710678, // 35
				    0.92387953,0,-0.38268343, // 36

				   // Circle in YZ

				    0, 0.92387953, 0.38268343, // 37
				    0, 0.70710678, 0.70710678, // 38
				    0, 0.38268343, 0.92387953, // 39

				    0,-0.38268343, 0.92387953, // 40
				    0,-0.70710678, 0.70710678, // 41
				    0,-0.92387953, 0.38268343, // 42

				    0,-0.92387953,-0.38268343, // 43
				    0,-0.70710678,-0.70710678, // 44
				    0,-0.38268343,-0.92387953, // 45

				    0, 0.38268343,-0.92387953, // 46
				    0, 0.70710678,-0.70710678, // 47
				    0, 0.92387953,-0.38268343 }; // 48

  static int32_t nonAxisIndex[54] = { 1,13,14,15,5,16,17,18,3,19,20,21, 7,22,23,24,1,-1,
				      1,25,26,27,9,28,29,30,3,31,32,33,11,34,35,36,1,-1,
				      5,37,38,39,9,40,41,42,7,43,44,45,11,46,47,48,5,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,49,54);
};


LAMBDA_FUNC(makeBallJointMaster)
{
  static float CoordsVal[25*3] = { 0, 0, 0, // 0

				    1,  0, 0, // 1
				  1.3,  0, 0, // 2
				   -1,  0, 0, // 3
				 -1.3,  0, 0, // 4

				  0,   1,  0, // 5
				  0, 1.3,  0, // 6
				  0, -1,   0, // 7
				  0,- 1.3, 0, // 8

				  0,  0,   1, // 9
				  0,  0, 1.3, // 10
				  0,  0,  -1, // 11
				  0,  0,-1.3, // 12

				  1.375,-0.075, .125, // 13 XAxis Label
				  1.375, 0.075,-.125, // 14
				  1.375, 0.075, .125, // 15
				  1.375,-0.075,-.125, // 16

				  0.075, 1.375, .125, // 17 YAxis Label
				    0,   1.375,   0 , // 18
				 -0.075, 1.375, .125, // 19
				  0.075, 1.375,-.125, // 20

				  0.075,-.125 ,1.375, // 21 ZAxis Arrow
				 -0.075,-.125 ,1.375, // 22
				  0.075, .125 ,1.375, // 23
				 -0.075, .125 ,1.375 }; // 24

  static int32_t nonAxisIndex[35] = { 1, 2,-1, 3, 4,-1,
				      5, 6,-1, 7, 8,-1,
				      9,10,-1,11,12,-1,

				      13,14,-1,  15,16,-1, // X
				      17,18,-1,  19,20,-1, // Y
				      21,22,23,24,-1 };    // Z

  return make_symbol(CoordsVal,nonAxisIndex,25,35);
};


LAMBDA_FUNC(makeBallJointSlave)
{
  static float CoordsVal[43*3] = { 0, 0, 0, // 0

				   0.9, 0, 0, // 1
				  -0.9, 0, 0, // 2

				   0, 0.9, 0, // 3
				   0,-0.9, 0, // 4

				   0, 0, 0.9, // 5
				   0, 0,-0.9, // 6

				  // Circle in XY:
				   0.880,-0.166,0, // 7
				   0.880, 0.166,0, // 8

				   0.166, 0.880,0, // 9
				  -0.166, 0.880,0, // 10

				  -0.880, 0.166,0, // 11
				  -0.880,-0.166,0, // 12

				  -0.166,-0.880,0, // 13
				   0.166,-0.880,0, // 14

				  // Circle in XZ
				   0.880, 0,-0.166, // 15
				   0.880, 0, 0.166, // 16

				   0.166, 0, 0.880, // 17
				  -0.166, 0, 0.880, // 18

				  -0.880, 0, 0.166, // 19
				  -0.880, 0,-0.166, // 20

				  -0.166, 0,-0.880, // 21
				   0.166, 0,-0.880, // 22

				  // Circle in YZ
				   0, 0.880,-0.166, // 23
				   0, 0.880, 0.166, // 24

				   0, 0.166, 0.880, // 25
				   0,-0.166, 0.880, // 26

				   0,-0.880, 0.166, // 27
				   0,-0.880,-0.166, // 28

				   0,-0.166,-0.880, // 29
				   0, 0.166,-0.880, // 30

				   1.375,-0.075, .125, // 31 XAxis Label
				   1.375, 0.075,-.125, // 32
				   1.375, 0.075, .125, // 33
				   1.375,-0.075,-.125, // 34

				   0.075, 1.375, .125, // 35 YAxis Label
				     0,   1.375,   0 , // 36
				  -0.075, 1.375, .125, // 37
				   0.075, 1.375,-.125, // 38

				   0.075,-.125, 1.375, // 39 ZAxis Arrow
				  -0.075,-.125, 1.375, // 40
				   0.075, .125, 1.375, // 41
				  -0.075, .125, 1.375 }; // 42

  static int32_t nonAxisIndex[74] = { 1, 2,-1, // Aksene
				      3, 4,-1,
				      5, 6,-1,
				      // Buene i endene
				      7,  1, 8,-1,   9, 3,10,-1,  11, 2,12,-1,  13, 4,14,-1,
				      15, 1,16,-1,  17, 5,18,-1,  19, 2,20,-1,  21, 6,22,-1,
				      23, 3,24,-1,  25, 5,26,-1,  27, 4,28,-1,  29, 6,30,-1,
				      // Labeler
				      31,32,-1,  33,34,-1, // X
				      35,36,-1,  37,38,-1, // Y
				      39,40,41,42,-1 };    // Z

  return make_symbol(CoordsVal,nonAxisIndex,43,74);
};


LAMBDA_FUNC(makeCenterOfGravity)
{
  static float CoordsVal[61*3] = { 0, 0, 0, // 0

				   1, 0, 0, // 1
				   3, 0, 0, // 2
				  -1, 0, 0, // 3
				  -3, 0, 0, // 4

				   0,  1, 0, // 5
				   0,  3, 0, // 6
				   0, -1, 0, // 7
				   0, -3, 0, // 8

				   0, 0,  1, // 9
				   0, 0,  3, // 10
				   0, 0, -1, // 11
				   0, 0, -3, // 12

				  // Circle in XY:

				   0.92387953, 0.38268343,0, // 13
				   0.70710678, 0.70710678,0, // 14
				   0.38268343, 0.92387953,0, // 15

				  -0.38268343, 0.92387953,0, // 16
				  -0.70710678, 0.70710678,0, // 17
				  -0.92387953, 0.38268343,0, // 18

				  -0.92387953,-0.38268343,0, // 19
				  -0.70710678,-0.70710678,0, // 20
				  -0.38268343,-0.92387953,0, // 21

				   0.38268343,-0.92387953,0, // 22
				   0.70710678,-0.70710678,0, // 23
				   0.92387953,-0.38268343,0, // 24

				  // Circle in XZ

				   0.92387953, 0, 0.38268343, // 25
				   0.70710678, 0, 0.70710678, // 26
				   0.38268343, 0, 0.92387953, // 27

				  -0.38268343, 0, 0.92387953, // 28
				  -0.70710678, 0, 0.70710678, // 29
				  -0.92387953, 0, 0.38268343, // 30

				  -0.92387953, 0,-0.38268343, // 31
				  -0.70710678, 0,-0.70710678, // 32
				  -0.38268343, 0,-0.92387953, // 33

				   0.38268343, 0,-0.92387953, // 34
				   0.70710678, 0,-0.70710678, // 35
				   0.92387953, 0,-0.38268343, // 36

				  // Circle in YZ

				   0, 0.92387953, 0.38268343, // 37
				   0, 0.70710678, 0.70710678, // 38
				   0, 0.38268343, 0.92387953, // 39

				   0,-0.38268343, 0.92387953, // 40
				   0,-0.70710678, 0.70710678, // 41
				   0,-0.92387953, 0.38268343, // 42

				   0,-0.92387953,-0.38268343, // 43
				   0,-0.70710678,-0.70710678, // 44
				   0,-0.38268343,-0.92387953, // 45

				   0, 0.38268343,-0.92387953, // 46
				   0, 0.70710678,-0.70710678, // 47
				   0, 0.92387953,-0.38268343, // 48

				   1.075,-0.075, .125, // 49 XAxis Label
				   1.075, 0.075,-.125, // 50
				   1.075, 0.075, .125, // 51
				   1.075,-0.075,-.125, // 52

				   0.075, 1.075, .125, // 53 YAxis Label
				     0,   1.075,0    , // 54
				  -0.075, 1.075, .125, // 55
				   0.075, 1.075,-.125, // 56

				   0.075,-.125 ,1.075, // 57  ZAxis Label
				  -0.075,-.125 ,1.075, // 58
				   0.075, .125 ,1.075, // 59
				  -0.075, .125 ,1.075 }; // 60

  static int32_t nonAxisIndex[80] = { 1, 13, 14, 15,  5, 16, 17, 18,  3, 19, 20, 21,  7, 22, 23, 24,  1, -1,
				      1, 25, 26, 27,  9, 28, 29, 30,  3, 31, 32, 33, 11, 34, 35, 36,  1, -1,
				      5, 37, 38, 39,  9, 40, 41, 42,  7, 43, 44, 45, 11, 46, 47, 48,  5, -1,
				      2,  3, -1,  6,  7, -1, 10, 11, -1,
				      49, 50, -1, 51, 52, -1, 53, 54, -1, 55, 56, -1, 57, 58, 59, 60, -1 };

  for (int idx = 0; idx < 49*3; idx++)
    CoordsVal[idx] /= 3.0;

  return make_symbol(CoordsVal,nonAxisIndex,61,80);
};


LAMBDA_FUNC(makeLinJointMaster)
{
  static float CoordsVal[7*3] = { 0 ,  0, 0, // 0
				  .5,  0, 0, // 1
				 -.5,  0, 0, // 2
				  0 , .5, 0, // 3
				  0 ,-.5, 0, // 4
				  0 ,  0, .5, // 5
				  0 ,  0,-.5 }; // 6

  static int32_t nonAxisIndex[9] = { 1,2,-1, 3,4,-1, 5,6,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,7,9);
};


LAMBDA_FUNC(makeLinPrismSlave)
{
  static float CoordsVal[21*3] = { 0, 0, 0, // 0

				   1, 0, 0, // 1
				  -1, 0, 0, // 2

				   0, 1, 0, // 3
				   0,-1, 0, // 4

				   0, 0, 1, // 5
				   0, 0,-1, // 6

				   1, 1, 0, // 7
				  -1, 1, 0, // 8

				   1,-1, 0, // 9
				  -1,-1, 0, // 10

				  .1, 0, 0.7, // 11 ZAxis Arrow
				 -.1, 0, 0.7, // 12

				 1.1,-0.075, .125,  // 13 XAxis Label
				 1.1, 0.075,-.125,  // 14
				 1.1, 0.075, .125,  // 15
				 1.1,-0.075,-.125,  // 16

				  0.075,-.125 ,1.1, // 17 ZAxis Label
				 -0.075,-.125 ,1.1, // 18
				  0.075, .125 ,1.1, // 19
				 -0.075, .125 ,1.1 }; // 20

  static int32_t nonAxisIndex[37] = { 1, 2,-1,  7,8,-1, 10,9,-1,
				      3, 4,-1, 10,8,-1, 9,7,-1,
				      0, 5,-1,
				      5,11,12, 5,-1,
				      // Labels :
				      13,14,-1,  15,16,-1, // X
				      17,18,19,20,-1 };    // Z

  return make_symbol(CoordsVal,nonAxisIndex,21,37);
};


LAMBDA_FUNC(makeLinCylSlave)
{
  static float CoordsVal[28*3] = { 0, 0, 0, // 0

				   .1, 0, 0.7, // 1 ZAxis Arrow
				  -.1, 0, 0.7, // 2

				   1, 0, 0, // 3

				  0.92387953,0.38268343,0, // 4
				  0.70710678,0.70710678,0, // 5
				  0.38268343,0.92387953,0, // 6

				   0, 1, 0, // 7

				  -0.38268343,0.92387953,0, //  8
				  -0.70710678,0.70710678,0, //  9
				  -0.92387953,0.38268343,0, // 10

				  -1, 0, 0, // 11

				  -0.92387953,-0.38268343, 0, // 12
				  -0.70710678,-0.70710678, 0, // 13
				  -0.382683433,-0.9238795, 0, // 14

				   0,-1, 0, // 15

				  0.38268343,-0.92387953,0, // 16
				  0.70710678,-0.70710678,0, // 17
				  0.92387953,-0.38268343,0, // 18

				   0, 0, 1, // 19 Zaxis point

				  1.1,-0.075, .125, // 20 XAxis Label
				  1.1, 0.075,-.125, // 21
				  1.1, 0.075, .125, // 22
				  1.1,-0.075,-.125, // 23

				   0.075,-.125, 1.1, // 24 ZAxis Label
				  -0.075,-.125, 1.1, // 25
				   0.075, .125, 1.1, // 26
				  -0.075, .125, 1.1 }; // 27

  static int32_t nonAxisIndex[40] = { 0, 3, -1,
				      0, 19, -1,
				      3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,3,-1,// Ring
				      19, 1, 2,19,-1, // Z axis arrow;
				      // Labels
				      20,21,-1,  22,23,-1, // X
				      24,25,26,27,-1 };    // Z

  return make_symbol(CoordsVal,nonAxisIndex,28,40);
};


LAMBDA_FUNC(makeLinJointLine)
{
  static float coords[10*3] = { 0,  0,  0, // 0
				1,  0,  0, // 1
				0, .1,  0, // 2
				0,  0, .1, // 3
				0,-.1,  0, // 4
				0,  0,-.1, // 5
				1, .1,  0, // 6
				1,  0, .1, // 7
				1,-.1,  0, // 8
				1,  0,-.1 }; // 9

  static int32_t nonAxisIndex[27] = { 0,1,-1,
				      2,6,-1,
				      3,7,-1,
				      4,8,-1,
				      5,9,-1,
				      2,4,-1,
				      3,5,-1,
				      6,8,-1,
				      7,9,-1 };

  return make_symbol(coords,nonAxisIndex,10,27,'A');
};


LAMBDA_FUNC(makeFreeJoint)
{
  static float CoordsVal[25*3] = { 0, 0, 0, // 0
				   1, 0, 0, // 1
				   0, 1, 0, // 2
				   0, 0, 1, // 3

				   0.92387955, 0.38268343, 0         , // 4  XY plane
				   0.7071068 , 0.7071068 , 0         , // 5
				   0.38268343, 0.92387955, 0         , // 6

				   0.9931705 , 0.41138469, 0         , // 7
				   0.85458855, 0.35398216, 0         , // 8

				   0.35398216, 0.85458855, 0         , // 9
				   0.41138469, 0.9931705 , 0         , // 10

				   0.92387955, 0         , 0.38268343, // 11 XZ plane
				   0.7071068 , 0         , 0.7071068 , // 12
				   0.38268343, 0         , 0.92387955, // 13

				   0.9931705 , 0         , 0.41138469, // 14
				   0.85458855, 0         , 0.35398216, // 15

				   0.35398216, 0         , 0.85458855, // 16
				   0.41138469, 0         , 0.9931705 , // 17

				   0         , 0.92387955, 0.38268343, // 18 YZ plane
				   0         , 0.7071068 , 0.7071068 , // 19
				   0         , 0.38268343, 0.92387955, // 20

				   0         , 0.9931705 , 0.41138469, // 21
				   0         , 0.85458855, 0.35398216, // 22

				   0         , 0.35398216, 0.85458855, // 23
				   0         , 0.41138469, 0.9931705 }; // 24

  static int32_t nonAxisIndex[36] = { 1, 8, 7,1, 4, 5, 6,2, 9,10,2,-1,
				      1,14,15,1,11,12,13,3,16,17,3,-1,
				      2,21,22,2,18,19,20,3,23,24,3,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,25,36);
};


LAMBDA_FUNC(makeFreeJointSlave)
{
  static float CSCoordsVal[28*3] = { 0, 0, 0, // 0
				     1, 0, 0, // 1
				     0, 1, 0, // 2
				     0, 0, 1, // 3

				    0.75, 0.075, 0, // 4 XAxis Arrow
				    0.75,-0.075, 0, // 5

				    0.25, 0.075, 0, // 6 XAxis Arrow
				    0.25,-0.075, 0, // 7

				     0.075, 0.75, 0, // 8 YAxis Arrow
				    -0.075, 0.75, 0, // 9

				     0.075, .25, 0, // 10 YAxis Arrow
				    -0.075, .25, 0, // 11

				     0.075, 0, 0.75, // 12  ZAxis Arrow
				    -0.075, 0, 0.75, // 13

				     0.075, 0, 0.25, // 14  ZAxis Arrow
				    -0.075, 0, 0.25, // 15

				    1.375,-0.075, .125, // 16 XAxis Label
				    1.375, 0.075,-.125, // 17
				    1.375, 0.075, .125, // 18
				    1.375,-0.075,-.125, // 19

				     0.075, 1.375, .125, // 20 YAxis Label
				       0,   1.375,   0 , // 21
				    -0.075, 1.375, .125, // 22
				     0.075, 1.375,-.125, // 23

				     0.075,-.125, 1.375, // 24 ZAxis Label
				    -0.075,-.125, 1.375, // 25
				     0.075, .125, 1.375, // 26
				    -0.075, .125, 1.375 }; // 27

  static int32_t nonAxisIndex[44] = { 0, 7, 6,  0, 1, 4,  5, 1,-1,
				      0,10,11,  0, 2, 8,  9, 2,-1,
				      0,14,15,  0, 3,12, 13, 3,-1,
				     16,17,-1, 18,19,-1, // X
				     20,21,-1, 22,23,-1, // Y
				     24,25,26, 27,-1 };  // Z

  return make_symbol(CSCoordsVal,nonAxisIndex,28,44);
};


LAMBDA_FUNC(makeSimpleLine)
{
  static float   CoordsVal[2*3] = { 0,0,0, 1,0,0 };
  static int32_t nonAxisIndex[3] = { 0,1,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,2,3,'A');
};


LAMBDA_FUNC(makeRigidJoint)
{
  static float CoordsVal[9*3] = { 0, 0, 0, // 0

				  .5,  .5,  .5, // 1
				 -.5,  .5,  .5, // 2

				 -.5, -.5,  .5, // 3
				  .5, -.5,  .5, // 4

				  .5, -.5, -.5, // 5
				  .5,  .5, -.5, // 6

				 -.5,  .5, -.5, // 7
				 -.5, -.5, -.5 }; // 8

  static int32_t nonAxisIndex[36] = { 2, 1, -1,  4, 3,-1,  8, 5,-1,  7, 6,-1,
				      4, 1, -1,  3, 2,-1,  8, 7,-1,  5, 6,-1,
				      5, 4, -1,  6, 1,-1,  7, 2,-1,  8, 3,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,9,36);
};


LAMBDA_FUNC(makeRigidJointMaster)
{
  static float CoordsVal[25*3] = { 0, 0, 0, // 0

				   .5,  0, 0, // 1
				  0.7,  0, 0, // 2
				  -.5,  0, 0, // 3
				 -0.7,  0, 0, // 4

				  0,  .5,  0, // 5
				  0, 0.7,  0, // 6
				  0, -.5,  0, // 7
				  0,-0.7,  0, // 8

				  0,  0,  .5, // 9
				  0,  0, 0.7, // 10
				  0,  0, -.5, // 11
				  0,  0,-0.7, // 12

				  0.775, -0.075,  .125, // 13 XAxis Label
				  0.775,  0.075, -.125, // 14
				  0.775,  0.075,  .125, // 15
				  0.775, -0.075, -.125, // 16

				  0.075,  0.775,  .125, // 17 YAxis Label
				    0,    0.775,    0 , // 18
				 -0.075,  0.775,  .125, // 19
				  0.075,  0.775, -.125, // 20

				  0.075, -.125 , 0.775, // 21 ZAxis Arrow
				 -0.075, -.125 , 0.775, // 22
				  0.075,  .125 , 0.775, // 23
				 -0.075,  .125 , 0.775 }; // 24

  static int32_t nonAxisIndex[35] = { 1, 2,-1, 3, 4,-1,
				      5, 6,-1, 7, 8,-1,
				      9,10,-1,11,12,-1,

				      13,14,-1,  15,16,-1, // X
				      17,18,-1,  19,20,-1, // Y
				      21,22,23,24,-1 };    // Z

  return make_symbol(CoordsVal,nonAxisIndex,25,35);
};


LAMBDA_FUNC(makeRigidJointSlave)
{
  static float CoordsVal[19*3] = { 0, 0, 0, // 0

				   0.4, 0, 0,  // 1
				  -0.4, 0, 0,  // 2

				   0,  0.4, 0, // 3
				   0, -0.4, 0, // 4

				   0, 0,  0.4, // 5
				   0, 0, -0.4, // 6

				  0.775,-0.075, .125, // 7 XAxis Label
				  0.775, 0.075,-.125, // 8
				  0.775, 0.075, .125, // 9
				  0.775,-0.075,-.125, // 10

				  0.075, 0.775, .125, // 11 YAxis Label
				    0,   0.775,   0 , // 12
				 -0.075, 0.775, .125, // 13
				  0.075, 0.775,-.125, // 14

				  0.075,-.125 ,0.775, // 15 ZAxis Label
				 -0.075,-.125 ,0.775, // 16
				  0.075, .125 ,0.775, // 17
				 -0.075, .125 ,0.775 }; // 18

  static int32_t nonAxisIndex[26] = { 1, 2,-1, // Aksene
				      3, 4,-1,
				      5, 6,-1,
				      // Labeler
				      7,  8,-1,  9, 10,-1, // X
				      11,12,-1,  13,14,-1, // Y
				      15,16,17,18,-1 };    // Z

  return make_symbol(CoordsVal,nonAxisIndex,19,26);
};


LAMBDA_FUNC(makeCamJointMaster)
{
  static float CoordsVal[17*3] = { 0, 0, 0, // 0

				   1, 0, 0, // 1
				   0, 0, 1, // 2

				   0,  0.2, 0, // 3
				   0, -0.2, 0, // 4

				  1.500, 0,-0.075, // 5 XAxis Label
				  1.250, 0, 0.075, // 6
				  1.500, 0, 0.075, // 7
				  1.250, 0,-0.075, // 8

				  0.125, 0 ,1.300, // 9 ZAxis Label
				  0.125, 0 ,1.450, // 10
				 -0.125, 0 ,1.300, // 11
				 -0.125, 0 ,1.450, // 12

				  0.75, 0,  0.075, // 13 XAxis Arrow
				  0.75, 0, -0.075, // 14

				  0.075, 0, 0.75,  // 15 ZAxis Arrow
				 -0.075, 0, 0.75 }; // 16

  static int32_t nonAxisIndex[25] = { 3,  0,  1, 13, 14,  1, -1,
				      5,  6, -1,  7,  8, -1,
				      4,  0,  2, 15, 16,  2, -1,
				      9, 10, 11, 12, -1 };

  return make_symbol(CoordsVal,nonAxisIndex,17,25);
};


LAMBDA_FUNC(makeCamJointSlave)
{
  static float CoordsVal[13*3] = { 0, 0, 0, // 0

				  -0.5,  0.2, 0, // 1
				   0.5, -0.2, 0, // 2
				   0.5,  0.2, 0, // 3
				  -0.5, -0.2, 0, // 4

				   0, 0.5, -0.2, // 5
				   0,-0.5,  0.2, // 6
				   0,-0.5, -0.2, // 7
				   0, 0.5,  0.2, // 8

				  -0.2, 0,  0.5, // 9
				   0.2, 0, -0.5, // 10
				  -0.2, 0, -0.5, // 11
				   0.2, 0,  0.5 }; // 12

  static int32_t nonAxisIndex[18] = { 1,  2,  3,  4, 1, -1,
				      5,  6,  7,  8, 5, -1,
				      9, 10, 11, 12, 9, -1 };

  return make_symbol(CoordsVal,nonAxisIndex,13,18);
};


LAMBDA_FUNC(makeSticker)
{
  static float CoordsVal[5*3] = { 0, 0, 0, // 0

				  .5, -.5, -1, // 1
				  .5,  .5, -1, // 2

				 -.5,  .5, -1, // 3
				 -.5, -.5, -1 }; // 4

  static int32_t nonAxisIndex[14] = { 0,1,2,0,3,4,0,-1,
				      2,3,-1,
				      4,1,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,5,14);
};


LAMBDA_FUNC(makePoint)
{
  static float CoordsVal[7*3] = { 0, 0, 0, // 0

				  .2,   0,  0, // 1
				   0,  .2,  0, // 2
				   0,   0, .2, // 3

				 -.2,   0,  0, // 4
				   0, -.2,  0, // 5
				   0,   0,-.2 }; // 6

  static int32_t nonAxisIndex[18] = { 1,6,2, 3,5,6, 4,3,1, 5,2,1, 4,2,-1,
				      4,5,-1 };

  return make_symbol(CoordsVal,nonAxisIndex,7,18);
};


LAMBDA_FUNC(makeRefCS)
{
  static float CoordsVal[22*3] = { -0.2, -0.2, 0, // 0
				    0.2, -0.2, 0, // 1
				    0.2,  0.2, 0, // 2
				   -0.2,  0.2, 0, // 3

				  // X part

				    1, 0.2,  0, // 4
				    1, -.2,  0, // 5
				    1, -.4,  0, // 6
				  1.4, 0  ,  0, // 7
				    1,  .4,  0, // 8

				  // Ypart

				   -.2, 1, 0, // 9
				    .2, 1, 0, // 10
				    .4, 1, 0, // 11
				   0  , 1.4, 0, // 12
				   -.4, 1, 0, // 13

				  // X label
				   1.6 , .1, 0, // 14
				   1.6 ,-.1, 0, // 15
				   1.75, .1, 0, // 16
				   1.75,-.1, 0, // 17
				  // Y label
				  -.1 , 1.8, 0, // 18
				  -.1 , 1.6, 0, // 19
				  0   , 1.7, 0, // 20
				   .1 , 1.8, 0 }; // 21

  static int32_t nonAxisIndex[28] = { 0,  5,  6,  7,  8,  4, 3, -1, // X-axis
				      0,  9, 13, 12, 11, 10, 1, -1, // Y-axis
				     14, 17, -1, 16, 15, -1,   // Xlabel
				     21, 19, -1, 18, 20, -1 }; // Ylabel

  return make_symbol(CoordsVal,nonAxisIndex,22,28);
};
////////////////////////////////////////////////////////////////////////////////

  // Setting up the shared scale and style nodes:

  GlobalSymbolScale      = new SoTransform;
  GlobalAxialSymbolScale = new SoTransform;
  GlobalSymbolStyle      = new SoDrawStyle;

  GlobalSymbolScale->scaleFactor.setValue(0.1, 0.1, 0.1);
  GlobalAxialSymbolScale->scaleFactor.setValue(1, 0.05, 0.05);

  GlobalSymbolStyle->lineWidth.setValue(1);
  GlobalSymbolStyle->pointSize.setValue(4);
  GlobalSymbolStyle->style.setValue(SoDrawStyle::LINES);

  // Make the shared material nodes initialized with default colors:

  Materials[DEFAULT]    = new_material(0.7f, 0.7f, 0.7f);
  Materials[HIGHLIGHT]  = new_material(1.0f, 0.0f, 0.0f);
  Materials[TRIAD]      = new_material(0.0f, 0.5f, 0.0f);
  Materials[GROUND]     = new_material(0.0f, 0.0f, 0.6f);
  Materials[JOINT]      = new_material(0.0f, 0.5f, 0.5f);
  Materials[SPR_DMP]    = new_material(0.0f, 0.0f, 1.0f);
  Materials[STICK]      = new_material(0.5f, 0.3f, 0.0f);
  Materials[LOAD]       = new_material(0.2f, 0.2f, 1.0f);
  Materials[HP]         = new_material(0.0f, 1.0f, 0.0f);
  Materials[LINK_CS]    = new_material(1.0f, 1.0f, 0.0f);
  Materials[REF_PLANE]  = new_material(0.0f, 0.0f, 0.8f);
  Materials[SENSOR_MAT] = new_material(0.7f, 0.0f, 0.7f);
  Materials[STRAIN_ROS] = new_material(0.0f, 0.0f, 1.0f);

  // Initialize the symbol array:

  Symbols[COORD_SYST]    = makeCoordSyst();
  Symbols[LINK_COORD_SYS] = makeLinkCoordSyst();
  Symbols[INT_LINK_COORD_SYS] = makeInternalLinkCoordSyst();
  Symbols[CENTER_OF_GRAVITY] = makeCenterOfGravity();
  Symbols[FORCE]         = makeForce();
  Symbols[TORQUE]        = makeTorque();
  Symbols[AXIAL_DAMPER]  = makeAxialDamper();
  Symbols[AXIAL_SPRING]  = makeAxialSpring();
  Symbols[SENSOR]        = makeSensor();

  Symbols[S_SINGLE_GAGE]    = makeSmallSingleGage();
  Symbols[S_DOUBLE_GAGE_90] = makeSmallDoubleGage90();
  Symbols[S_TRIPLE_GAGE_60] = makeSmallTripleGage60();
  Symbols[S_TRIPLE_GAGE_45] = makeSmallTripleGage45();

  Symbols[L_SINGLE_GAGE]    = makeLargeSingleGage();
  Symbols[L_DOUBLE_GAGE_90] = makeLargeDoubleGage90();
  Symbols[L_TRIPLE_GAGE_60] = makeLargeTripleGage60();
  Symbols[L_TRIPLE_GAGE_45] = makeLargeTripleGage45();

  Symbols[HIGHER_PAIR] = makeSimpleLine();

  Symbols[REVJOINT]         = makeRevJoint();
  Symbols[REVJOINT_MASTER]  = makeRevJointMaster();
  Symbols[REVJOINT_SLAVE]   = makeRevJointSlave();

  Symbols[BALLJOINT]        = makeBallJoint();
  Symbols[BALLJOINT_MASTER] = makeBallJointMaster();
  Symbols[BALLJOINT_SLAVE]  = makeBallJointSlave();

  Symbols[LINJOINT_MASTER]  = makeLinJointMaster();
  Symbols[LINJOINT_LINE]    = makeLinJointLine();
  Symbols[LINPRISM_SLAVE]   = makeLinPrismSlave();
  Symbols[LINCYL_SLAVE]     = makeLinCylSlave();

  Symbols[FREEJOINT]        = makeFreeJoint();
  Symbols[FREEJOINT_SLAVE]  = makeFreeJointSlave();
  Symbols[FREEJOINT_LINE]   = makeSimpleLine();

  Symbols[RIGIDJOINT]        = makeRigidJoint();
  Symbols[RIGIDJOINT_MASTER] = makeRigidJointMaster();
  Symbols[RIGIDJOINT_SLAVE]  = makeRigidJointSlave();

  Symbols[CAMJOINT_MASTER] = makeCamJointMaster();
  Symbols[CAMJOINT_SLAVE]  = makeCamJointSlave();

  Symbols[STICKER] = makeSticker();
  Symbols[POINT] = makePoint();
  Symbols[REFCS] = makeRefCS();
}


FdSymbolKit* FdSymbolDefs::getSymbol(SymbolType symbolIndex)
{
  FdSymbolKit* symbol = new FdSymbolKit;

  symbol->setPart("scale",SO_GET_PART(Symbols[symbolIndex],"scale",SoTransform));
  symbol->setPart("style",SO_GET_PART(Symbols[symbolIndex],"style",SoDrawStyle));
  symbol->setPart("coords", SO_GET_PART(Symbols[symbolIndex],"coords",SoCoordinate3));
  symbol->setPart("nonAxis",SO_GET_PART(Symbols[symbolIndex],"nonAxis",SoIndexedLineSet));

  return symbol;
}


float FdSymbolDefs::getSymbolScale()
{
  return GlobalSymbolScale->scaleFactor.getValue()[1];
}

void FdSymbolDefs::setSymbolScale(float scale)
{
  GlobalSymbolScale->scaleFactor.setValue(scale,scale,scale);
  GlobalAxialSymbolScale->scaleFactor.setValue(1,scale,scale);
}


void FdSymbolDefs::setSymbolLineWidth(int width)
{
  GlobalSymbolStyle->lineWidth.setValue((float)width);
}


SoDrawStyle* FdSymbolDefs::getGlobalSymbolStyle()
{
  return GlobalSymbolStyle;
}


SoDrawStyle* FdSymbolDefs::getHighlightSymbolStyle()
{
  SoDrawStyle* ds = (SoDrawStyle*)GlobalSymbolStyle->copy();
  float lw = ds->lineWidth.getValue() + 2.0f;
  if (lw < 3.0f) lw ++;
  ds->lineWidth.setValue(lw);
  return ds;
}


#ifdef USE_SMALLCHANGE
SmDepthBuffer* FdSymbolDefs::getHighlightDepthBMod()
{
  SmDepthBuffer* dbn = new SmDepthBuffer;
  dbn->func.setValue(SmDepthBuffer::ALWAYS);
  dbn->enable.setValue(true);
  return dbn;
}
#endif


SoMaterial* FdSymbolDefs::getMaterial(MaterialType matIndex)
{
  return Materials[matIndex];
}

SoMaterial* FdSymbolDefs::getHighlightMaterial()
{
  return Materials[HIGHLIGHT];
}


void FdSymbolDefs::setDefaultColor(const FdColor& color)
{
  setMaterialColor(DEFAULT,color);
}

void FdSymbolDefs::setSensorColor(const FdColor& color)
{
  setMaterialColor(SENSOR_MAT,color);
}

void FdSymbolDefs::setTriadColor(const FdColor& color)
{
  setMaterialColor(TRIAD,color);
}

void FdSymbolDefs::setGndTriadColor(const FdColor& color)
{
  setMaterialColor(GROUND,color);
}

void FdSymbolDefs::setJointColor(const FdColor& color)
{
  setMaterialColor(JOINT,color);
}

void FdSymbolDefs::setSprDaColor(const FdColor& color)
{
  setMaterialColor(SPR_DMP,color);
}

void FdSymbolDefs::setStickerColor(const FdColor& color)
{
  setMaterialColor(STICK,color);
}

void FdSymbolDefs::setLoadColor(const FdColor& color)
{
  setMaterialColor(LOAD,color);
}

void FdSymbolDefs::setHPColor(const FdColor& color)
{
  setMaterialColor(HP,color);
}

void FdSymbolDefs::setLinkCoordSysColor(const FdColor& color)
{
  setMaterialColor(LINK_CS,color);
}

void FdSymbolDefs::setRefPlaneColor(const FdColor& color)
{
  setMaterialColor(REF_PLANE,color);
}

void FdSymbolDefs::setStrainRosetteColor(const FdColor& color)
{
  setMaterialColor(STRAIN_ROS,color);
}
