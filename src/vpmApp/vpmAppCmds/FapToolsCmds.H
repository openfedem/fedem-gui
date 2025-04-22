// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_TOOLS_CMDS_H
#define FAP_TOOLS_CMDS_H

#include "vpmApp/vpmAppCmds/FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"


class FapToolsCmds : public FapCmdsBase
{
public:
  static void setFileAssociations();
  static bool setFileAssociationsEx();
  static bool checkFileAssociations();

  static bool haveAddons();
  static bool getAddonExe(size_t index, std::string& addon,
                          bool fullPath = false);

private:
  static void init();

  static void addonLaunch(int index);

  friend class FFaInitialisation<FapToolsCmds>;

  static std::vector<std::string> ourAddons;
};

static FFaInitialisation<FapToolsCmds> _Init;

#endif
