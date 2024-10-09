// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_CYLINDER_SPIDER_CMDS_H
#define FAP_CYLINDER_SPIDER_CMDS_H

#include "FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"
#include "FFaLib/FFaAlgebra/FFaVec3.H"

class FmPart;
class FmTriad;
class SoEventCallback;


class FapCylinderSpiderCmds : public FapCmdsBase
{
  static void init();

  static void createRBE2Spider();
  static void createRBE3Spider();
  static void setCylinderPoint(size_t index, const FaVec3& ptPos);
  static void createSpider(bool doProjectRefNodePosToAxis);

  static void setState(int newState);
  static void eventCB(void*, SoEventCallback* eventCallbackNode);

public:
  static void enterMode();
  static void cancelMode();
  static void done();

private:
  enum {
    START,
    MASTER_OBJPOINT_SELECTED,
    MASTER_OBJPOINT_ACCEPTED,
    N_1_SELECTED_AFTER_MAST_OBJPOINT,
    N_1_SELECTED,
    N_1_ACCEPTED,
    N_2_SELECTED,
    N_2_ACCEPTED,
    N_3_SELECTED,
    N_3_ACCEPTED,
    START_NODE_SELECTED,
    START_NODE_ACCEPTED,
    END_NODE_SELECTED,
    END_NODE_ACCEPTED,
    MASTER_PLANE_SELECTED,
    MASTER_PLANE_ACCEPTED
  };

  static int      ourState;
  static int      ourSpiderType;
  static FmPart*  ourSpiderPart;
  static FmTriad* ourSelectedTriad;
  static FaVec3   ourGlobalRefNodePos;
  static std::vector<FaVec3> ourCylinderPoints;
  static bool     ourUseArcOnly;
  static bool     ourSelectedRefNode;

  friend class FFaInitialisation<FapCylinderSpiderCmds>;
};

static FFaInitialisation<FapCylinderSpiderCmds> FapCylinderSpiderCmds_Init;

#endif
