// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_SOLVE_CMDS_H
#define FAP_SOLVE_CMDS_H

#include "vpmApp/vpmAppCmds/FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"
#include <string>

class FFuaTimer;


class FapSolveCmds : public FapCmdsBase
{
private:
  static void init();

public:
  static void reduceAllParts();
  static void solveDynamics();
  static void solveEvents();
  static void solveInCloud();

  // batch-solve preparators
  static void prepareAllBatch();
  static bool prepareForBatchReduction();
  static bool prepareForBatchSolving();

  static bool solveAll(bool preBatch);
  static bool solveStress(bool preBatch);
  static bool solveModes(bool preBatch);
  static bool solveRosette(bool preBatch);
  static bool solveStrainCoat(bool preBatch);

  static void eraseSimuleRes();
  static void eraseEventRes();

  static void degradeSoil(double degradeTime);

private:
  static void refreshRDB();

  // individual part(s) solving
  static void reducePart();
  static void solvePart();
  static void solveStressOnPart(); // group sensitive
  static void solveRosetteOnPart();
  static void solveStrainCoatOnPart(); // group sensitive

  static void checkRunningCloudJob();

public:
  static bool haveCloud();

  friend class FFaInitialisation<FapSolveCmds>;

private:
  static std::string ourCloudJobId;
  static std::string ourCloudAppId;
  static FFuaTimer*  ourCloudJobTimer;
  static char        haveCloudAccess;
};

static FFaInitialisation<FapSolveCmds> FapSolveCmds_Init;

#endif
