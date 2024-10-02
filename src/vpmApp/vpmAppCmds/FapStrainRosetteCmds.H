// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_STRAIN_ROSETTE_CMDS_H
#define FAP_STRAIN_ROSETTE_CMDS_H

#include "FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"
#include "FFaLib/FFaAlgebra/FFaVec3.H"


class SoEventCallback;
class FmPart;
class FmStrainRosette;


class FapStrainRosetteCmds : public FapCmdsBase
{
private:
  static void init();

  static void setState(int newState);
  static void eventCB(void*, SoEventCallback* eventCallbackNode);

public:
  static void enterMode();
  static void cancelMode();
  static void done();

  static void flipStrainRosetteZDirection();

private:
  static int    ourState;
  static FaVec3 ourDirection;
  static FaVec3 ourDirP1;
  static FaVec3 ourDirP2;

  static FmPart*          ourRosettePart;
  static FmStrainRosette* ourCreatedStrainRosette;

  friend class FFaInitialisation<FapStrainRosetteCmds>;
};

static FFaInitialisation<FapStrainRosetteCmds> FapStrainRosetteCmds_Init;

#endif
