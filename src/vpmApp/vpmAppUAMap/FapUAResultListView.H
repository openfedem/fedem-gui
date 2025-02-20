// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_RESULT_LISTVIEW_H
#define FAP_UA_RESULT_LISTVIEW_H

#include "FapUAModMemListView.H"

#include <map>
#include <array>

class FmRingStart;

typedef std::map<int,const char**> IconsMap;


class FapUAResultListView : public FapUAModMemListView
{
public:
  FapUAResultListView(FuiItemsListView* ui);
  virtual ~FapUAResultListView() {}

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
  virtual void dropItems(int droppedOnItemIdx, bool isCopy, void*);

  // Reimplementations from FapUACommandHandler
  virtual FFuaUICommands* getCommands();

private:
  FmRingStart* funcPreviews;
  FmRingStart* beamDiagrams;

  FFuaCmdHeaderItem importItemHeader;
  FFuaCmdHeaderItem exportItemHeader;

  // Curve symbol/color manipulations
  static std::vector<const char**>      ourCurveSymbolLib;
  static std::multimap<std::string,int> ourColorSymbol;
  static std::map<std::string,IconsMap> ourCurveSymbols;

  static const char** getCurvePixMap(const std::array<float,3>&, int symb);
};

#endif
