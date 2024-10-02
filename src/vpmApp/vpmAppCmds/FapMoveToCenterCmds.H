// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_MOVE_TO_CENTER_CMDS_H
#define FAP_MOVE_TO_CENTER_CMDS_H

#include "FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"
#include "FFaLib/FFaAlgebra/FFaVec3.H"

class SoEventCallback;


class FapMoveToCenterCmds : public FapCmdsBase
{
private:
  static void init();

  static void moveToCenter(std::vector<FaVec3> circlePoints);

  static void setState(int newState);
  static void eventCB(void*, SoEventCallback* eventCallbackNode);

public:
  static void enterMode();
  static void cancelMode();
  static void done();

private:
  static std::vector<FaVec3> ourCirclePoints;
  static int ourNumSelectedToMove;
  static int ourState;

  friend class FFaInitialisation<FapMoveToCenterCmds>;
};

static FFaInitialisation<FapMoveToCenterCmds> FapMoveToCenterCmds_Init;

#endif
