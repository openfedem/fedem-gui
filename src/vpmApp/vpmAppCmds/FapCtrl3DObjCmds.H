// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_CTRL_3D_OBJ_CMDS_H
#define FAP_CTRL_3D_OBJ_CMDS_H

#include "FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"


class FapCtrl3DObjCmds : public FapCmdsBase
{
  static void init();

  static void ctrlCreate(int elemType);
  static void ctrlEdit(int editMode);

  friend class FFaInitialisation<FapCtrl3DObjCmds>;
};

static FFaInitialisation<FapCtrl3DObjCmds> FapCtrl3DObjCmdsInit;

#endif
