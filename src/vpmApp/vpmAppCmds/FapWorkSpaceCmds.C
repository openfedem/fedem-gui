// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FapWorkSpaceCmds.H"

#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"

#include "vpmUI/Fui.H"
#include "vpmUI/vpmUITopLevels/FuiMainWindow.H"
#include "vpmUI/vpmUIComponents/FuiWorkSpace.H"


void FapWorkSpaceCmds::init()
{
  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_workSpace_cascade");
  cmdItem->setSmallIcon(cascade_xpm);
  cmdItem->setText("Cascade");
  cmdItem->setToolTip("Cascade");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ Fui::getMainWindow()->getWorkSpace()->cascadeWins(); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_workSpace_tile");
  cmdItem->setSmallIcon(tile_xpm);
  cmdItem->setText("Tile");
  cmdItem->setToolTip("Tile");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ Fui::getMainWindow()->getWorkSpace()->tileWins(); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive,bool&));

  cmdItem = new FFuaCmdItem("cmdId_workSpace_tabs");
  cmdItem->setSmallIcon(tabs_xpm);
  cmdItem->setText("Tabs");
  cmdItem->setToolTip("Tabs");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ Fui::getMainWindow()->getWorkSpace()->tabWins(); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive, bool&));

  cmdItem = new FFuaCmdItem("cmdId_workSpace_subWins");
  cmdItem->setSmallIcon(tile_xpm);
  cmdItem->setText("Sub Windows");
  cmdItem->setToolTip("Sub Windows");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ Fui::getMainWindow()->getWorkSpace()->subWins(); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapCmdsBase::alwaysSensitive, bool&));
}
