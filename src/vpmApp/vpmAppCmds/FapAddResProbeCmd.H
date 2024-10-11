// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_ADD_RES_PROBE_CMD_H
#define FAP_ADD_RES_PROBE_CMD_H

#include "vpmApp/vpmAppCmds/FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"
#include "FFaLib/FFaAlgebra/FFaVec3.H"

class SoEventCallback;
class FdPart;
class FdFEModel;

typedef std::pair<FdFEModel*,void*> FapResProbeLabel;


class FapAddResProbeCmd : public FapCmdsBase
{
  static void init();

  static void getSensitivity(bool& isSensitive);

  static void setState(int newState);
  static void eventCB(void*, SoEventCallback* eventCallbackNode);

public:
  static void enterMode();
  static void cancelMode();
  static void done();

private:
  static int         myState;
  static FdPart*     myPart;
  static FaVec3      myPosition;
  static std::string myValue;

  static std::vector<FapResProbeLabel> ourAddedLabels;

  friend class FFaInitialisation<FapAddResProbeCmd>;
};

static FFaInitialisation<FapAddResProbeCmd> FapAddResProbeCmd_Init;

#endif
