// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <array>

#include <Inventor/SbVec3f.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoFaceSet.h>

#include "vpmDisplay/FdCtrlSymbolKit.H"
#include "vpmDisplay/FdCtrlSymDef.H"


namespace
{
  SoMaterial* defaultMaterial = NULL;
  SoMaterial* backMaterial = NULL;
  SoMaterial* highlightMaterial = NULL;

  std::array<SoSeparator*,2> portSymbols{};


  FdCtrlSymbolKit* createSymbol(const float* coords, int numCoords,
                                const int32_t* lineIndex, int numIndex,
                                float XWidth = 0.5f, float YWidth = 0.5f,
                                int nInPorts = 1, int nOutPorts = 1)
  {
    // Set coordinates
    SoCoordinate3* CSCoord = new SoCoordinate3;
    for (int cn = 0; cn < numCoords; cn++, coords += 3)
      CSCoord->point.set1Value(cn,coords[0],coords[1],coords[2]);

    // Make shape Set indices
    SoIndexedLineSet* line = new SoIndexedLineSet;
    line->coordIndex.setValues(0,numIndex,lineIndex);

    // Define the square's spatial coordinates
    SoCoordinate3* fCoord = new SoCoordinate3;
    fCoord->point.set1Value(0, SbVec3f( XWidth,  YWidth, -0.1));
    fCoord->point.set1Value(1, SbVec3f( XWidth, -YWidth, -0.1));
    fCoord->point.set1Value(2, SbVec3f(-XWidth, -YWidth, -0.1));
    fCoord->point.set1Value(3, SbVec3f(-XWidth,  YWidth, -0.1));

    // Define a face set
    SoFaceSet* faceSet = new SoFaceSet;
    faceSet->numVertices.set1Value(0, 4);

    FdCtrlSymbolKit* elemBody = new FdCtrlSymbolKit;
    elemBody->setPart("coords",CSCoord);
    elemBody->setPart("line",line);
    elemBody->setPart("faceCoord",fCoord);
    elemBody->setPart("elemFaceSet",faceSet);
    elemBody->setPart("faceMaterial",backMaterial);

    const float verticalportSpacing = 0.25f;

    bool inPort = true;
    for (SoSeparator* port : portSymbols)
    {
      const int nPorts = inPort ? nInPorts : nOutPorts;
      const int numPortsPerSide = (inPort ? nInPorts : nOutPorts-1)/2;
      float transform = verticalportSpacing*numPortsPerSide;
      SoSeparator* sep = SO_GET_PART(elemBody,
                                     inPort ? "inPortSep" : "outPortSep",
                                     SoSeparator);

      for (int i = 0; i < nPorts; i++)
      {
        SoTransform* xf = new SoTransform;
        xf->translation.setValue(SbVec3f(0.0f,transform,0.0f));
        transform = nPorts == 2 ? -verticalportSpacing*2 : -verticalportSpacing;
        sep->addChild(xf);
        sep->addChild(port);
      }

      inPort = false;
    }

    return elemBody;
  }
}


void FdCtrlSymDef::init()
{
  defaultMaterial = new SoMaterial;
  defaultMaterial->ref();
  defaultMaterial->diffuseColor.setValue(0,0,0);
  defaultMaterial->ambientColor.setValue(0,0,0);
  defaultMaterial->specularColor.setValue(0,0,0);
  defaultMaterial->emissiveColor.setValue(0,0,0);

  backMaterial = new SoMaterial;
  backMaterial->ref();
  backMaterial->diffuseColor.setValue(1,1,1);
  backMaterial->ambientColor.setValue(1,1,1);
  backMaterial->specularColor.setValue(1,1,1);
  backMaterial->emissiveColor.setValue(1,1,1);
  backMaterial->transparency.setValue(1);

  highlightMaterial = new SoMaterial;
  highlightMaterial->ref();
  highlightMaterial->diffuseColor.setValue(1,0,0);
  highlightMaterial->ambientColor.setValue(1,0,0);
  highlightMaterial->specularColor.setValue(1,0,0);
  highlightMaterial->emissiveColor.setValue(1,0,0);

  static float CoordsInp[5][3] = {{-0.5 ,  0.0,  0.0}, //0 inport
                                  {-0.75 , 0.0,  0.0}, //1
                                  {-0.675, 0.075,0.0}, //2
                                  {-0.675,-0.075,0.0}, //3
                                  {-0.6  , 0.0,  0.0}};//4

  static float CoordsOut[5][3] = {{ 0.5 , 0.0,  0.0}, //0 outport
                                  { 0.75, 0.0,  0.0}, //1
                                  { 0.6 , 0.05, 0.0}, //2
                                  { 0.6 ,-0.05, 0.0}, //3
                                  { 0.65, 0.0,  0.0}};//4

  static int32_t lineIndex[6] = { 0,4,3,1,2,4 };

  bool inPort = true;
  for (SoSeparator*& port : portSymbols)
  {
    // Set coordinates
    SoCoordinate3* coord = new SoCoordinate3;
    coord->point.setValues(0, 5, inPort ? CoordsInp : CoordsOut);
    inPort = false;

    // Make shape set indices
    SoIndexedLineSet* line = new SoIndexedLineSet;
    line->coordIndex.setValues(0,6,lineIndex);

    port = new SoSeparator;
    port->addChild(coord);
    port->addChild(line);
    port->ref();
  }
}


SoMaterial* FdCtrlSymDef::getDefaultMaterial()
{
  return defaultMaterial;
}

SoMaterial* FdCtrlSymDef::getBackMaterial()
{
  return backMaterial;
}

SoMaterial* FdCtrlSymDef::getHighlightMaterial()
{
  return highlightMaterial;
}


/*!
  Standard symbol for elements with one output port.
*/

FdCtrlSymbolKit* FdCtrlSymDef::stdSym(int nInp, int nOutp)
{
  static float CoordsVal[] = { 0.5,  0.5, 0.0,
                               0.5, -0.5, 0.0,
                              -0.5, -0.5, 0.0,
                              -0.5,  0.5, 0.0 };

  static int32_t lineIndex[] = { 0,1,2,3,0 };

  return createSymbol(CoordsVal,4,lineIndex,5,0.5f,0.5f,nInp,nOutp);
}


FdCtrlSymbolKit* FdCtrlSymDef::adderSym()
{
  static float CoordsVal[] = { 0.000*0.4,  1.000*0.4, 0.0, // 0 Body
                               0.500*0.4,  0.866*0.4, 0.0, // 1
                               0.866*0.4,  0.500*0.4, 0.0,
                               1.000*0.4,  0.000*0.4, 0.0,
                               0.866*0.4, -0.500*0.4, 0.0,
                               0.500*0.4, -0.866*0.4, 0.0,
                               0.000*0.4, -1.000*0.4, 0.0,
                              -0.500*0.4, -0.866*0.4, 0.0,
                              -0.866*0.4, -0.500*0.4, 0.0,
                              -1.000*0.4,  0.000*0.4, 0.0,
                              -0.866*0.4,  0.500*0.4, 0.0,
                              -0.500*0.4,  0.866*0.4, 0.0,
                               0.000*0.4,  1.000*0.4, 0.0, // 12

                              -0.35,  0.33, 0.0, // 13  Upper
                              -0.35,  0.47, 0.0, // 14  plus sign
                              -0.42,  0.40, 0.0, // 15
                              -0.28,  0.40, 0.0, // 16

                              -0.35, -0.33, 0.0, // 17 Lower
                              -0.35, -0.47, 0.0, // 18 plus sign
                              -0.42, -0.40, 0.0, // 19
                              -0.28, -0.40, 0.0, // 20

                              -0.5,  0.25,  0.0, // 21 Touching upper port
                              -0.5, -0.25,  0.0, // 22 Touching lower port
                               0.5,  0,     0 };

  static int32_t lineIndex[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12, -1,
                                 13,14,-1,
                                 15, 16,-1,
                                 17,18,-1,
                                 19, 20,-1,
                                 8,22,-1,
                                 10, 21,-1,
                                 3,23 };

   return createSymbol(CoordsVal,24,lineIndex,34,0.4f,0.4f,2,1);
}


FdCtrlSymbolKit* FdCtrlSymDef::comparatorSym()
{
  static float CoordsVal[] = { 0.000*0.4,  1.000*0.4, 0.0, // 0 Body
                               0.500*0.4,  0.866*0.4, 0.0, // 1
                               0.866*0.4,  0.500*0.4, 0.0,
                               1.000*0.4,  0.000*0.4, 0.0, // 3
                               0.866*0.4, -0.500*0.4, 0.0,
                               0.500*0.4, -0.866*0.4, 0.0,
                               0.000*0.4, -1.000*0.4, 0.0, // 6
                              -0.500*0.4, -0.866*0.4, 0.0,
                              -0.866*0.4, -0.500*0.4, 0.0,
                              -1.000*0.4,  0.000*0.4, 0.0,
                              -0.866*0.4,  0.500*0.4, 0.0,
                              -0.500*0.4,  0.866*0.4, 0.0,
                               0.000*0.4,  1.000*0.4, 0.0, // 12

                              -0.35,  0.33, 0.0, // 13  Upper
                              -0.35,  0.47, 0.0, // 14  plus sign
                              -0.42,  0.40, 0.0, // 15
                              -0.28,  0.40, 0.0, // 16

                              -0.42, -0.40, 0.0, // 17  Lower
                              -0.28, -0.40, 0.0, // 18  minus sign

                              -0.5,   0.25, 0.0, // 19 Touching upper port
                              -0.5,  -0.25, 0.0, // 20 Touching lower port
                               0.5,   0.0,  0.0 };

  static int32_t lineIndex[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,-1,
                                 13,14,-1,
                                 15,16,-1,
                                 17,18,-1 ,
                                 8,20,-1,
                                 10,19,-1,
                                 3,21};

  return createSymbol(CoordsVal,22,lineIndex,31,0.4f,0.4f,2,1);
}


/*
 ***********************************************************************
 *   Output symbol with one in-port.
 ***********************************************************************
 */

FdCtrlSymbolKit* FdCtrlSymDef::outputSym()
{
  static float CoordsVal[] = { -0.5,    0.375, 0.0, // 0 Body
                                0.0,    0.375, 0.0, // 1
                                0.1,    0.35,  0.0, // 2
                                0.175,  0.3,   0.0, // 3
                                0.225,  0.2,   0.0, // 4
                                0.25,   0.1,   0.0, // 5
                                0.25,   0.,    0.0, // 6
                                0.25,  -0.1,   0.0, // 7
                                0.225, -0.2,   0.0, // 8
                                0.175, -0.3,   0.0, // 9
                                0.1,   -0.35,  0.0, // 10
                                0.0,   -0.375, 0.0, // 11
                               -0.5,   -0.375, 0.0, // 12

                                0.0,   0.125,  0.0,   // 13
                                0.0,  -0.125,  0.0,   // 14
                                0.125, 0.0,    0.0 }; // 15

  static int32_t lineIndex[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,0,-1,
                                 13,14,15,13 };

  return createSymbol(CoordsVal,16,lineIndex,19,0.5f,0.5f,1,0);
}


/*
 ***********************************************************************
 *   Input symbol with one outport.
 ***********************************************************************
 */

FdCtrlSymbolKit* FdCtrlSymDef::inputSym()
{
  static float CoordsVal[] = { -0.25,   0.0,   0.0, // 0 Box Body
                               -0.25,   0.1,   0.0, // 1
                               -0.225,  0.2,   0.0, // 2
                               -0.175,  0.3,   0.0, // 3
                               -0.1,    0.35,  0.0, // 4
                                0.0,    0.375, 0.0, // 5
                                0.5,    0.375, 0.0, // 6
                                0.5,   -0.375, 0.0, // 7
                                0.0,   -0.375, 0.0, // 8
                               -0.1,   -0.35,  0.0, // 9
                               -0.175, -0.3,   0.0, // 10
                               -0.225, -0.2,   0.0, // 11
                               -0.25,  -0.1,   0.0, // 12

                                0.125,  0.0,   0.0,   // 13
                                0.0,    0.125, 0.0,   // 14
                                0.0,   -0.125, 0.0 }; // 15

  static int32_t lineIndex[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,0,-1,
                                 13,14,15,13 };

  return createSymbol(CoordsVal,16,lineIndex,19,0.5f,0.5f,0,1);
}


/*
 ***********************************************************************
 * Dead Zone symbol with one in- and one outport.
 ***********************************************************************
 */

FdCtrlSymbolKit* FdCtrlSymDef::deadZoneSym()
{
  static float CoordsVal[] = { 0.5,  0.5, 0.0, // 0 Box Body
                               0.5, -0.5, 0.0, // 1
                              -0.5, -0.5, 0.0, // 2
                              -0.5,  0.5, 0.0, // 3

                              -0.375,  0.0,   0.0, // 4 Cross
                               0.375,  0.0,   0.0, // 5
                               0.0,   -0.375, 0.0, // 6
                               0.0,    0.375, 0.0, // 7

                              -0.375, -0.375, 0.0,   // 8 Symbol
                              -0.125,  0.0,   0.0,   // 9
                               0.125,  0.0,   0.0,   // 10
                               0.375,  0.375, 0.0 }; // 11

  static int32_t lineIndex[] = { 0,1,2,3,0,-1,
                                 4,5,-1,
                                 6,7,-1,
                                 8,9,10,11 };

  return createSymbol(CoordsVal,12,lineIndex,16);
}


/*
 ***********************************************************************
 * Amplifier/Gain symbol with one in- and one outport.
 ***********************************************************************
 */

FdCtrlSymbolKit* FdCtrlSymDef::amplifierSym()
{
  static float CoordsVal[] = { -0.5, -0.5, 0.0,   // 0 Body
                               -0.5,  0.5, 0.0,   // 1
                                0.5,  0.0, 0.0 }; // 2

  static int32_t lineIndex[4] = { 0,1,2,0 };

  return createSymbol(CoordsVal,3,lineIndex,4);
}


/*
 ***********************************************************************
 *   Sample & Hold symbol with one outport.
 ***********************************************************************
 */

FdCtrlSymbolKit* FdCtrlSymDef::sampleHoldSym()
{
  static float CoordsVal[] = { 0.5,  0.5, 0.0, // 0 Box Body
                               0.5, -0.5, 0.0, // 1
                              -0.5, -0.5, 0.0, // 2
                              -0.5,  0.5, 0.0, // 3

                              -0.5,    0.0,   0.0, // 4 Symbol
                              -0.25,   0.0,   0.0, // 5

                              -0.25,   0.125, 0.0, // 6
                              -0.25,  -0.125, 0.0, // 7

                              -0.125,  0.125, 0.0, // 8
                              -0.125, -0.125, 0.0, // 9

                               0.0,    0.0,   0.0, // 10
                               0.125,  0.0,   0.0, // 11

                               0.25,   0.0,   0.0, // 12
                               0.375,  0.0,   0.0, // 13

                               0.0,    0.375, 0.0,   // 14
                               0.2,    0.375, 0.0,   // 15
                               0.1,    0.125, 0.0,   // 16
                               0.1,    0.375, 0.0 }; // 17

  static int32_t lineIndex[] = { 0,1,2,3,0,-1,
                                 4,5,-1,
                                 6,7,-1,
                                 8,9,-1,
                                 10,11,-1,
                                 12,13,-1,
                                 14,15,-1,
                                 16,17 };

  return createSymbol(CoordsVal,18,lineIndex,26);
}


/*
 ***********************************************************************
 *   LimDerivator symbol with one in- and one outport.
 ***********************************************************************
 */

FdCtrlSymbolKit* FdCtrlSymDef::limDerivatorSym()
{
  static float CoordsVal[] = { 0.5,  0.5, 0.0, // 0 Box Body
                               0.5, -0.5, 0.0, // 1
                              -0.5, -0.5, 0.0, // 2
                              -0.5,  0.5, 0.0, // 3

                              -0.375,  0.0,   0.0, // 4 Symbol
                               0.375,  0.0,   0.0, // 5

                              -0.375,  0.125, 0.0, // 6
                              -0.125,  0.125, 0.0, // 7
                              -0.25,   0.375, 0.0, // 8

                               0.0,    0.375, 0.0, // 9
                               0.0,    0.185, 0.0, // 10
                               0.06,   0.125, 0.0, // 11
                               0.19,   0.125, 0.0, // 12
                               0.25,   0.185, 0.0, // 13
                               0.25,   0.375, 0.0, // 14

                              -0.375, -0.375, 0.0, // 15
                              -0.125, -0.375, 0.0, // 16
                              -0.25,  -0.125, 0.0, // 17

                               0.03,  -0.2,   0.0,   // 18
                               0.22,  -0.2,   0.0,   // 19
                               0.125, -0.375, 0.0,   // 20
                               0.125, -0.125, 0.0 }; // 21


  static int32_t lineIndex[] = { 0,1,2,3,0,-1,
                                 4,5,-1,
                                 6,7,8,6,-1,
                                 9,10,11,12,13,14,-1,
                                 15,16,17,15,-1,
                                 18,19,-1,
                                 20,21 };

  return createSymbol(CoordsVal,22,lineIndex,31);
}


/*
 ***********************************************************************
 *   Logical Switch symbol with one in- and one outport.
 ***********************************************************************
 */

FdCtrlSymbolKit* FdCtrlSymDef::logicalSwitchSym()
{
  static float CoordsVal[] = { 0.5,  0.5, 0.0, // 0 Box Body
                               0.5, -0.5, 0.0, // 1
                              -0.5, -0.5, 0.0, // 2
                              -0.5,  0.5, 0.0, // 3

                              -0.5,   0.375, 0.0, // 4 Symbol
                              -0.125, 0.375, 0.0, // 5
                               0.25,  0.0,   0.0, // 6
                               0.5,   0.0,   0.0, // 7

                              -0.5,  0.0, 0.0, // 8
                              -0.25, 0.0, 0.0, // 9

                              -0.25, -0.125, 0.0, // 10
                              -0.25,  0.125, 0.0, // 11

                              -0.5,   -0.375, 0.0, // 12
                              -0.125, -0.375, 0.0, // 13

                               0.0,   0.0,  0.0,   // 14
                               0.0,   0.06, 0.0,   // 15
                               0.02,  0.125,0.0,   // 16
                               0.065, 0.19, 0.0,   // 17
                               0.125, 0.25, 0.0 }; // 18

  static int32_t lineIndex[] = { 0,1,2,3,0,-1,
                                 4,5,6,7,-1,
                                 8,9,-1,
                                 10,11,-1,
                                 12,13,-1,
                                 14,15,16,17,18 };

  return createSymbol(CoordsVal,19,lineIndex,25);
}


/*
 ***********************************************************************
 * Hysteresis (Backlash) symbol with one in- and one outport.
 ***********************************************************************
 */

FdCtrlSymbolKit* FdCtrlSymDef::hysteresisSym()
{
  static float CoordsVal[] = { 0.5,  0.5, 0.0, // 0 Box Body
                               0.5, -0.5, 0.0, // 1
                              -0.5, -0.5, 0.0, // 2
                              -0.5,  0.5, 0.0, // 3

                              -0.375,  0.0,   0.0, // 4 Cross
                               0.375,  0.0,   0.0, // 5
                               0.0,   -0.375, 0.0, // 6
                               0.0,    0.375, 0.0, // 7

                              -0.375, -0.375, 0.0, // 8 Symbol
                              -0.125,  0.25,  0.0, // 9
                               0.31,   0.25,  0.0, // 10

                              -0.31,  -0.25,  0.0,   // 11
                               0.125, -0.25,  0.0,   // 12
                               0.375,  0.375, 0.0 }; // 13

  static int32_t lineIndex[] = { 0,1,2,3,0,-1,
                                 4,5,-1,
                                 6,7,-1,
                                 8,9,10,-1,
                                 11,12,13 };

  return createSymbol(CoordsVal,14,lineIndex,19);
}


/*
 ***********************************************************************
 * Multiplier symbol with two in- and one outport.
 ***********************************************************************
 */

FdCtrlSymbolKit* FdCtrlSymDef::multiplierSym()
{
  static float CoordsVal[] = { 0.000*0.4, 1.000*0.4, 0.0, // 0 Body
                               0.500*0.4, 0.866*0.4, 0.0, // 1
                               0.866*0.4, 0.500*0.4, 0.0,
                               1.000*0.4, 0.000*0.4, 0.0,
                               0.866*0.4,-0.500*0.4, 0.0,
                               0.500*0.4,-0.866*0.4, 0.0,
                               0.000*0.4,-1.000*0.4, 0.0,
                              -0.500*0.4,-0.866*0.4, 0.0,
                              -0.866*0.4,-0.500*0.4, 0.0,
                              -1.000*0.4, 0.000*0.4, 0.0,
                              -0.866*0.4, 0.500*0.4, 0.0,
                              -0.500*0.4, 0.866*0.4, 0.0,
                               0.000*0.4, 1.000*0.4, 0.0, // 12

                              -0.125,  0.125,  0.0, // 13
                              -0.125,  0.0,    0.0, // 14
                              -0.125, -0.125,  0.0, // 15

                               0.0,   0.125,   0.0, // 16
                               0.0,  -0.125,   0.0, // 17

                               0.125,  0.125,  0.0, // 18
                               0.125,  0.0,    0.0, // 19
                               0.125, -0.125,  0.0, // 20

                              -0.5,  0.25,  0.0,   // 21 Touching upper port
                              -0.5, -0.25,  0.0,   // 22 Touching lower port
                               0.5,  0.0,   0.0 }; // 23

  static int32_t lineIndex[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,-1,
                                 13,20,-1,
                                 14,19,-1,
                                 15,18,-1,
                                 16,17,-1,
                                  8,22,-1,
                                 10,21,-1,
                                  3,23 };

  return createSymbol(CoordsVal,24,lineIndex,34,0.4f,0.4f,2,1);
}


/*
 ***********************************************************************
 * Limitation (Saturation) symbol with one in- and one outport.
 ***********************************************************************
 */

FdCtrlSymbolKit* FdCtrlSymDef::limitationSym()
{
  static float CoordsVal[] = { 0.5,    0.5,   0.0,  // 0 Box Body
                               0.5,   -0.5,   0.0,  // 1
                              -0.5,   -0.5,   0.0,  // 2
                              -0.5,    0.5,   0.0,  // 3
                              -0.375,  0.0,   0.0,  // 4 Symbol
                               0.375,  0.0,   0.0,  // 5
                               0.0,   -0.375, 0.0,  // 6
                               0.0,    0.375, 0.0,  // 7
                              -0.375, -0.25,  0.0,  // 8
                              -0.125, -0.25,  0.0,  // 9
                               0.125,  0.25,  0.0,  // 10
                               0.375,  0.25,  0.0}; // 11

  static int32_t lineIndex[] = { 0,1,2,3,0,-1,
                                 4,5,-1,
                                 6,7,-1,
                                 8,9,10,11 };

  return createSymbol(CoordsVal,12,lineIndex,16);
}
