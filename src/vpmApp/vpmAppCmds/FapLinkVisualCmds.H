// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_LINK_VISUAL_CMDS_H
#define FAP_LINK_VISUAL_CMDS_H

#include "vpmApp/vpmAppCmds/FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"


class FapLinkVisualCmds : public FapCmdsBase
{
  static void init();

  static void setSelectionDetail(int detailType);
  static void subassemblySelection(bool show);

  static void getElmFaceHideShowSensitivity(bool& isSensitive);

  friend class FFaInitialisation<FapLinkVisualCmds>;
};

static FFaInitialisation<FapLinkVisualCmds> FapLinkVisualCmds_Init;

#endif
