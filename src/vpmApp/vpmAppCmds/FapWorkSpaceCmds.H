// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_WORK_SPACE_CMDS_H
#define FAP_WORK_SPACE_CMDS_H

#include "FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"


class FapWorkSpaceCmds : public FapCmdsBase
{
  static void init();

  friend class FFaInitialisation<FapWorkSpaceCmds>;
};

static FFaInitialisation<FapWorkSpaceCmds> FapWorkSpaceCmds_Init;

#endif
