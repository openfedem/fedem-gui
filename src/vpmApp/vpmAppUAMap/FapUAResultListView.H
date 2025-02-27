// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_RESULT_LISTVIEW_H
#define FAP_UA_RESULT_LISTVIEW_H

#include "vpmApp/vpmAppUAMap/FapUAModMemListView.H"

class FmRingStart;


class FapUAResultListView : public FapUAModMemListView
{
public:
  FapUAResultListView(FuiItemsListView* ui);
  virtual ~FapUAResultListView() {}

private:
  // Callbacks for dragging and dropping on UI-item
  void startDrag(bool& accepted);
  void dropItems(int droppedOnItemIdx, int& dropAction);

protected:
  // Reimplementations from FapUAItemsListView
  virtual bool verifyItem(FFaListViewItem* item);
  virtual FFaListViewItem* getParent(FFaListViewItem* item,
                                     const std::vector<int>& assID) const;
  virtual void getChildren(FFaListViewItem* parent,
                           std::vector<FFaListViewItem*>& children) const;

  virtual std::string getItemText(FFaListViewItem* item);
  virtual const char** getItemPixmap(FFaListViewItem* item);

  virtual bool isHeaderOkAsLeaf(FFaListViewItem* item) const;
  virtual bool getItemTextBold(FFaListViewItem* item);

  // Reimplementations from FapUACommandHandler
  virtual FFuaUICommands* getCommands();

private:
  FmRingStart* funcPreviews;
  FmRingStart* beamDiagrams;

  FFuaCmdHeaderItem importItemHeader;
  FFuaCmdHeaderItem exportItemHeader;
};

#endif
