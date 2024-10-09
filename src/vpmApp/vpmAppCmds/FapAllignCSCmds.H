// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_ALLIGN_CS_CMDS_H
#define FAP_ALLIGN_CS_CMDS_H

#include "FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"
#include "FFaLib/FFaAlgebra/FFaMat34.H"

class SoEventCallback;


class FapAllignCSCmds : public FapCmdsBase
{
  static void init();

public:
  static void allignCS(const FaMat34& mx);
  static void allignRotations(const FaMat34& mx);

private:
  static void setState(int newState);
  static void eventCB(void*, SoEventCallback* eventCallbackNode);

public:
  static void enterMode();
  static void cancelMode();
  static void allignCSDone();

private:
  static FaMat34 ourAllignCS;
  static int myState;

  friend class FFaInitialisation<FapAllignCSCmds>;
};

static FFaInitialisation<FapAllignCSCmds> FapAllignCSCmds_Init;

#endif
