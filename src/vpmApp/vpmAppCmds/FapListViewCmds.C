// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FapListViewCmds.H"
#include "vpmApp/vpmAppUAMap/FapUAItemsListView.H"

#include "vpmUI/Fui.H"
#include "vpmUI/vpmUITopLevels/FuiMainWindow.H"
#include "vpmUI/vpmUIComponents/FuiItemsListView.H"
#include "vpmUI/vpmUIComponents/FuiItemsListViews.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"


void FapListViewCmds::init()
{
  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_listView_sortListViewByID");
  cmdItem->setText("Sort by ID");
  cmdItem->setToolTip("Sort by ID");
  cmdItem->setActivatedCB(FFaDynCB0S(FapListViewCmds::sortListViewByID));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapListViewCmds::getSortListViewSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_listView_sortListViewByName");
  cmdItem->setText("Sort by Name");
  cmdItem->setToolTip("Sort by Name");
  cmdItem->setActivatedCB(FFaDynCB0S(FapListViewCmds::sortListViewByName));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapListViewCmds::getSortListViewSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_listView_ensureListViewSelectedVisible");
  cmdItem->setSmallIcon(treeObjectsSel_xpm);
  cmdItem->setText("Show Selected");
  cmdItem->setToolTip("Show Selected in Model Manager");
  cmdItem->setActivatedCB(FFaDynCB0S(FapListViewCmds::ensureListViewSelectedVisible));
}

void FapListViewCmds::sortListViewByID()
{
  FapUAItemsListView* lv = FapListViewCmds::getPoppedUpLV();
  if (lv) 
    lv->sortByID();
}

void FapListViewCmds::sortListViewByName()
{
  FapUAItemsListView* lv = FapListViewCmds::getPoppedUpLV();
  if (lv)
    lv->sortByName();
}

void FapListViewCmds::ensureListViewSelectedVisible()
{
  FapUAItemsListView* lv = FapListViewCmds::getPoppedUpLV();
  if (lv)
    lv->ensureSelectedVisible();
}

FapUAItemsListView* FapListViewCmds::getPoppedUpLV()
{
  FuiItemsListView* lv = Fui::getMainWindow()->getModelManager();
  if (!lv) return NULL;

  if (!lv->isPoppedUp()) {
    // Sorting is implemented for model manager list views only
    lv = Fui::getMainWindow()->getResultManager();
    if (!lv->isPoppedUp()) return NULL;
  }
  return (FapUAItemsListView*) lv->getUA();
}

void FapListViewCmds::getSortListViewSensitivity(bool& isSensitive)
{
  isSensitive = true;
//   FapUAItemsListView* lv = FapListViewCmds::getPoppedUpLV();
//   isSensitive = lv ? !lv->isEmpty() : false;
}
