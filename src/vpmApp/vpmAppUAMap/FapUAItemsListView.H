// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_ITEMS_LISTVIEW_H
#define FAP_UA_ITEMS_LISTVIEW_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAItemsViewHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUACommandHandler.H"

class FuiItemsListView;
class FFaListViewItem;


/*!
  The class has an automatic connection with DB which enables it to maintain
  the view under changes in DB as connect/disconnect and description change.
  Before this, the system should be initiated by calling updateUIValues().

  Listview positioning principle: If one item is subject to positioning,
  then all siblings are too.

  If you want to control item positions in the listview, you must set the
  items positionInListView variable (> -1) before the item gets in touch
  with this class (e.g., in getChildren(), etc.)

  \author Dag R Christensen
*/

class FapUAItemsListView : public FapUAExistenceHandler,
			   public FapUAItemsViewHandler,
			   public FapUACommandHandler
{
  Fmd_HEADER_INIT();

public:
  FapUAItemsListView(FuiItemsListView* ui);
  virtual ~FapUAItemsListView() {}

  void setVerifyItemCB(const FFaDynCB2<FFaListViewItem*,bool&>& cb) { verifyItemCB = cb; }

  void setTopLevelItem(FFaListViewItem* item, bool includeMyself=true);

  // setTopLevelItem is the only possibility to change tli under freeze
  void setFreezeTopLevelItem(bool freeze) { freezeTopLevelItem = freeze; }
  bool getFreezeTopLevelItem() const { return freezeTopLevelItem; }

  void setLeavesOnlySelectable(bool leavesOnly) { leavesOnlySelectable = leavesOnly; }

  void ensureItemVisible(FFaViewItem* item);

  FFaListViewItem* getUIParent(FFaListViewItem* item) const;

  void sortByID();
  void sortByName();
  void ensureSelectedVisible();

protected:
  // Functionality
  void createUITopLevelItems(FFaListViewItem* parent);
  void createUIItem(FFaListViewItem* item,
		    FFaListViewItem* parent,
		    FFaListViewItem* after);
  virtual void deleteUIItem(FFaViewItem* item);

  virtual void tmpSelectionChangedEvent();

  void updateItemPositionsInDB(int uiparent);

  static void reportItem(FFaViewItem* item, const char* prefix = NULL);

private:
  // For callback purpose only
  void tmpSelectItem(int item);
  void expandItem(int item, bool open);

protected:
  // from FapUAItemsViewHandler
  // You should always start your view session by updateUIValues
  // since it initialises the session
  virtual void updateSession();
  void updateTopLevelItem();

  // from FapUAExistenceHandler
  // These methods should be re-impl if you want this class to work as expected

  // listview settings
  virtual bool singleSelectionMode() const { return false; }
  virtual bool getDecoratedRoot() { return true; }
  virtual bool getShowHeader() { return false; }
  virtual bool getHeaderClickEnabling() { return false; }

  // listview hierarchy
  virtual bool isHeaderOkAsLeaf(FFaListViewItem*) const { return false; }

  // This method is supposed to filter the model members that the lv receives
  // (the lv receives either through onModelMemberConnected or getChildren).
  // It must filter under the assumption that the lv will receive any object
  // with ModelMember parent.
  virtual bool verifyItem(FFaListViewItem* item);

  // Those who are asking for their parents have been verified. Its only purpose
  // is to tell supposed parents for onListViewItemConnected items, etc.
  virtual FFaListViewItem* getParent(FFaListViewItem*,
				     const std::vector<int>&) const { return NULL; }

  // The objects you let thru here will also be verified,
  // so you can give any FFaListViewItem object,
  // but the parent has already been verified
  virtual void getChildren(FFaListViewItem* parent,
			   std::vector<FFaListViewItem*>& children) const = 0;

  // listview item settings
  virtual bool getItemSelectAble(FFaViewItem*) { return true; }
  virtual bool getItemExpanded(FFaListViewItem* item);

  virtual std::string getItemText(FFaListViewItem* item);
  virtual bool getItemTextBold(FFaListViewItem*) { return false; }
  virtual bool getItemTextItalic(FFaListViewItem*) { return false; }
  virtual const char** getItemPixmap(FFaListViewItem* item);

  virtual bool getItemToggleAble(FFaListViewItem*) { return false; }
  virtual bool getItemThreeStepToggleAble(FFaListViewItem*) { return false; }
  virtual int getItemToggleValue(FFaListViewItem*) { return 0; }

private:
  void getVerifiedChildren(FFaListViewItem* parent,
			   std::vector<FFaListViewItem*>& items);

protected:
  FFaListViewItem* getMapLVItem(int item) const;

  void onListViewItemConnected(FFaListViewItem* item, bool doVerify = true);
  void onListViewItemDisconnected(FFaListViewItem* item);
  void onListViewItemChanged(FFaListViewItem* item);

private:
  void sortItems(std::vector<FFaListViewItem*>& items) const;

  int createSingleUIItem(FFaListViewItem* item,
                         FFaListViewItem* parent,
                         FFaListViewItem* after);

  FFaListViewItem* getItemBefore(FFaListViewItem* item,
                                 int itemsUIParent) const;

  void updateLeavesOnlySelectable();

protected:
  FuiItemsListView* ui;

  bool automUpdateParentsPresence;
  std::vector<FFuaCmdItem*> popUpCommands;
  int  childrenVecCap;
  bool leavesOnlySelectable;
  FFaListViewItem* topLevelItem;
  bool topLevelItemIncludeMyself;
  bool freezeTopLevelItem;
  bool maintainSorting;

  enum { NONE, SORT_ID, SORT_DESCR } sortMode;

private:
  FFaDynCB2<FFaListViewItem*,bool&> verifyItemCB;
};

#endif
