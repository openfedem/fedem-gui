// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_MECH_3D_OBJ_CMDS_H
#define FAP_MECH_3D_OBJ_CMDS_H

#include "FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"


class FapMech3DObjCmds : public FapCmdsBase
{
  static void init();

  static void createTire();
  static void createRoad();

  friend class FFaInitialisation<FapMech3DObjCmds>;
};

static FFaInitialisation<FapMech3DObjCmds> FapMech3DObjCmds_Init;

#endif
