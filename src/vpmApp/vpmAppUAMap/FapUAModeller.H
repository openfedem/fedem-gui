// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_MODELLER_H
#define FAP_UA_MODELLER_H

#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <cstddef>

class FuiModeller;
class FaVec3;


//! \brief Interface between FuiModeller and the application.

namespace FapUAModeller
{
  void init(FuiModeller* ui);

  // Mode and state interface

  void updateMode();
  void updateState(int newState);
  void cancel();

  // 3D point select UI interface

  void updatePointUI(int globalOnly = -1, int idx = -1);

  // FE node select UI interface

  void updateNodeUI(int nodeId, const FaVec3& nodePos);
  void setNodeChangedCB(const FFaDynCB1<int>& cb);
}

#endif
