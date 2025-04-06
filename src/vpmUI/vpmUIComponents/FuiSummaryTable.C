// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FuiSummaryTable.H"
#include "FFuLib/FFuTable.H"
#include "FFuLib/FFuLabel.H"


void FuiSummaryTable::initWidgets()
{
  mySummaryTable->showRowHeader(true);
  mySummaryTable->showColumnHeader(true);
  mySummaryTable->setSelectionPolicy(FFuTable::NO_SELECTION);

  myAddBCLabel->setLabel(" *) Fixed in initial equilibrium analysis");
  myAddBCLabel->setToolTip("Also fixed in eigenmode analysis\n"
                           "if activated in the \"Dynamics Solver\" dialog");
}
