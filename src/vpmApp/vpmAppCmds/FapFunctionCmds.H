// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_FUNCTION_CMDS_H
#define FAP_FUNCTION_CMDS_H

#include "FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"


class FapFunctionCmds : public FapCmdsBase
{
public:
  static void getConvertFuncSensitivity(bool& sens, bool& sprDaSens, bool& driveFileSens);

private:
  static void init();

  static void createGeneralFunction();
  static void createDriveFile();

  friend class FFaInitialisation<FapFunctionCmds>;
};

static FFaInitialisation<FapFunctionCmds> FapFunctionCmds_Init;

#endif
