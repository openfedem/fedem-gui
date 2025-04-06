// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_RDB_ME_FATIGUE_H
#define FAP_UA_RDB_ME_FATIGUE_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAFinishHandler.H"

class FuiRDBMEFatigue;


class FapUARDBMEFatigue : public FapUAExistenceHandler, public FapUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FapUARDBMEFatigue(FuiRDBMEFatigue* ui);
  virtual ~FapUARDBMEFatigue() {}

private:
  // from FapUAFinishHandler
  virtual void finishUI();

  // Callbacks
  void onCellClicked(int row, int col);
  void onUsePropToggle(bool value);

  // Updates the damage table
  void calcTableValues();

private:
  FuiRDBMEFatigue* ui;
  std::vector<double> probability;
  std::vector<std::vector<double> > damage;
};

#endif
