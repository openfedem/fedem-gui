// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_GRAPH_CMDS_H
#define FAP_GRAPH_CMDS_H

#include "FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"
#include <vector>

class FapUAGraphViewTLS;
class FmGraph;
class FmCurveSet;


class FapGraphCmds : public FapCmdsBase
{
public:
  static std::vector<FmGraph*>    findSelectedGraphs(bool& graphsOnly);
  static std::vector<FmCurveSet*> findSelectedCurves(bool& curvesOnly);

  static void editAxis(int axis, FmCurveSet* curve = NULL);

  static void killAllGraphViews();

  static void show(FmGraph* graph);

private:
  static void init();

  static void show();

  static void repeatCurve(int fromID, int toID);
  static void onRepeatCurveDone(int button);
  static void toggleAutoExport(bool enable);

  static void getShowSensitivity(bool& sensitivity);
  static void getEditXAxisSensitivity(bool& sensitivity);
  static void getEditYAxisSensitivity(bool& sensitivity);
  static void getRepeatCurveSensitivity(bool& sensitivity);

  static FapUAGraphViewTLS* getTLS(FmGraph* graph);

  friend class FFaInitialisation<FapGraphCmds>;
};

static FFaInitialisation<FapGraphCmds> FapGraphCmds_Init;

#endif
