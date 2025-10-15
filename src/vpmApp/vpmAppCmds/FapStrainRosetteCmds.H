// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_STRAIN_ROSETTE_CMDS_H
#define FAP_STRAIN_ROSETTE_CMDS_H

#include "vpmApp/vpmAppCmds/FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"


class FapStrainRosetteCmds : public FapCmdsBase
{
private:
  static void init();

  friend class FFaInitialisation<FapStrainRosetteCmds>;

public:
  static void enterMode();
  static void cancelMode();
  static void done();

  static void flipStrainRosetteZDirection();
};

static FFaInitialisation<FapStrainRosetteCmds> FapStrainRosetteCmds_Init;


namespace FapEditStrainRosetteDirCmd
{
  void enterMode();
  void cancelMode();
  void done();
}


namespace FapEditStrainRosetteNodesCmd
{
  void enterMode();
  void cancelMode();
  void done();
}

#endif
