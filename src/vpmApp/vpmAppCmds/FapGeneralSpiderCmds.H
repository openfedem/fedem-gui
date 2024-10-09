// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_GENERAL_SPIDER_CMDS_H
#define FAP_GENERAL_SPIDER_CMDS_H

#include "FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"
#include "FFaLib/FFaAlgebra/FFaVec3.H"

typedef std::pair<long int,FaVec3> FdNode;

class FmPart;
class FmTriad;
class SoEventCallback;
class SoCallbackAction;
class SoPrimitiveVertex;
class SoSelection;


class FapGeneralSpiderCmds : public FapCmdsBase
{
  static void init();

  static void createRBE2Spider();
  static void createRBE3Spider();
  static void createSpider(const std::vector<FdNode>& nodes);

  static void setState(int newState);
  static void eventCB(void*, SoEventCallback* eventCallbackNode);

  static int mSoExtSelectionTriangleCB(void* userdata,
                                       SoCallbackAction* action,
                                       const SoPrimitiveVertex* v1,
                                       const SoPrimitiveVertex* v2,
                                       const SoPrimitiveVertex* v3);
  static int mSoExtSelectionLineSegmentCB(void* userdata,
                                          SoCallbackAction* action,
                                          const SoPrimitiveVertex* v1,
                                          const SoPrimitiveVertex* v2);
  static bool windowSelectPrimitiveCB(void* userdata,
                                      SoCallbackAction* action,
                                      const SoPrimitiveVertex* v1,
                                      const SoPrimitiveVertex* v2);
  static void windowSelectionFinishedCB(void* data, SoSelection* sel);

public:
  static void enterMode();
  static void cancelMode();
  static void done();

private:
  static int ourState;
  static int ourSpiderType;
  static FmPart* ourSpiderPart;
  static FmTriad* ourSelectedTriad;
  static std::map<int,FaVec3> ourWindowedNodes;
  static int ourSelectionType;

  static bool weMustInitWindowVolume;
  static bool weUseWindowSelection;

  friend class FFaInitialisation<FapGeneralSpiderCmds>;
};

static FFaInitialisation<FapGeneralSpiderCmds> FapGeneralSpiderCmds_Init;

#endif
