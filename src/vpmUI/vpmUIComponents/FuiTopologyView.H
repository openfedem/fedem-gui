// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_TOPOLOGY_VIEW_H
#define FUI_TOPOLOGY_VIEW_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

#include <vector>
#include <string>

class FFuListView;
class FFuListViewItem;
class FFuaCmdItem;
class FFuPopUpMenu;


struct FuiTopologyItem
{
  std::string type;
  std::string id;
  std::string description;
  int depth = -1;
};

using FuiTopologyItems = std::vector<FuiTopologyItem>;
using FFuCmdItems      = std::vector<FFuaCmdItem*>;

class FuiTopologyView : public virtual FFuComponentBase
{
public:
  void setTree(const FuiTopologyItems& topology);

  void setAutoUnHighlight(bool doIt) { IAmAutoUnHighlighting = doIt; }

  FFuListView* getListView() { return myView; }

  void setHighlightedCB  (const FFaDynCB2<int,bool>& aDynCB);
  void setActivatedCB    (const FFaDynCB1<int>& aDynCB);
  void setBuildPopUpCB   (const FFaDynCB2<int,FFuCmdItems&>& aDynCB);
  void setPopUpSelectedCB(const FFaDynCB2<const std::vector<int>&,FFuaCmdItem*>& aDynCB);

protected:
  void initWidgets();

  FFuListView* myView;

private:
  void onMyViewSelectionChanged();
  void onMyViewMouseReleased();
  void onMyViewActivated(FFuListViewItem* item);
  void onMyViewRMBPressed(FFuListViewItem* item);
  void onMyViewPopUpSelected(const std::vector<FFuListViewItem*>&,FFuaCmdItem*);

  bool             IAmAutoUnHighlighting = true;
  FFuListViewItem* previousSelected = NULL;

  FFaDynCB2<int,bool> myHighlightedCB;
  FFaDynCB1<int> myActivatedCB;
  FFaDynCB2<int,FFuCmdItems&> myBuildPopUpCB;
  FFaDynCB2<const std::vector<int>&,FFuaCmdItem*> myPopUpSelectedCB;
};

#endif
