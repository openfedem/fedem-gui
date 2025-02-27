// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_SIM_MODEL_RDB_LISTVIEW_H
#define FAP_UA_SIM_MODEL_RDB_LISTVIEW_H

#include "vpmApp/vpmAppUAMap/FapUASimModelListView.H"
#include "vpmApp/vpmAppUAMap/FapUARDBListView.H"


class FapUASimModelRDBListView : public FapUASimModelListView, public FapUARDBListView
{
public:
  FapUASimModelRDBListView(FuiItemsListView* ui);
  virtual ~FapUASimModelRDBListView() {}

protected:
  // Reimplementations from FapUAItemsListView
  virtual bool verifyItem(FFaListViewItem* item);
  virtual void getChildren(FFaListViewItem* parent,
			   std::vector<FFaListViewItem*>& children) const;

  virtual std::string getItemText(FFaListViewItem* item);
  virtual const char** getItemPixmap(FFaListViewItem* item);

  // Reimplementations from FapUACommandHandler
  virtual FFuaUICommands* getCommands();

  // Reimplementations from FapUAItemsViewHandler
  virtual void permTotSelectItems(std::vector<int>& totalSelection);
  virtual void updateSession();

private:
  mutable std::set<int> usedOGBaseIDs;
};

#endif
