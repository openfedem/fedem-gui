// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_RDB_LISTVIEW_H
#define FAP_UA_RDB_LISTVIEW_H

#include "vpmApp/vpmAppUAMap/FapUAItemsListView.H"

class FpExtractor;
class FFrExtractor;
class FFrEntryBase;
class FFaResultDescription;


class FapUARDBListView : virtual public FapUAItemsListView
{
public:
  FapUARDBListView(FuiItemsListView* ui);
  virtual ~FapUARDBListView() {}

  void setExtractor(FFrExtractor* ex);

  void setTopLevelItemDescr(const FFaResultDescription& item, bool includeMyself);
  FFrEntryBase* findItem(const FFaResultDescription& item);
  void showTopLevelVarsOnly();

  virtual bool singleSelectionMode() const { return true; }

protected:
  // Reimplementations from FapUAItemsListView
  virtual bool verifyItem(FFaListViewItem* item);
  virtual void getChildren(FFaListViewItem* parent,
			   std::vector<FFaListViewItem*>& children) const;

  virtual std::string getItemText(FFaListViewItem* item);
  virtual const char** getItemPixmap(FFaListViewItem* item);

  // Reimplementations from FapUACommandHandler
  virtual FFuaUICommands* getCommands();

protected:
  FpExtractor* extractor;

private:
  bool topLevelVarsOnly;
};

#endif
