// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_DB_CREATE_CMDS_H
#define FAP_DB_CREATE_CMDS_H

#include "FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"

class FmBase;
class FmBladeDesign;


class FapDBCreateCmds : public FapCmdsBase
{
private:
  static void init();

  static void createAnimation();
  static void createBeamDiagram();
  static void createGraph();
  static void createGraphGroup();
  static void createCurve();
  static void createImportedGraphs();
  static void createImportedCurves();
  static void createBeamForceGraph();

  static void createFileReferenceObject();
  static void createVesselMotionObject();
#ifdef FT_HAS_EXTCTRL
  static void createExternalCtrlSys();
#endif
  static void createGenericDBObject();
  static void createGenericPart();
  static void createBeams();

  static void createRotFriction();
  static void createTransFriction();
  static void createBearingFriction();
  static void createPrismaticFriction();
  static void createCamFriction();

  static void createBeamProperty();
  static void createMaterialProperty();
  static void createAssembly();

  static void createUserElm(int typeIdx);

  static void createMooringLine();

public:
  static FmBase* getSelectedAssembly(int classType = -1);
  static bool readBladeDesign(const std::string& bdFile,
                              FmBladeDesign*& currentDesign);
  static bool updateWindTurbine(bool haveTurbine);
  static bool updateWindTurbineTower();

private:
  friend class FFaInitialisation<FapDBCreateCmds>;
};

static FFaInitialisation<FapDBCreateCmds> FapDBCreateCmds_Init;

#endif
