// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_LIST_VIEW_H
#define FFU_LIST_VIEW_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

#include <vector>
#include <map>

class FFuListViewItem;
class FFuPopUpMenu;
class FFuaCmdItem;

using FFuListViewItems = std::vector<FFuListViewItem*>; //!< Convenience alias


/*!
  \brief Base class for various list views in FEDEM.

  \details The list view items are given an unique id (>-1) when created.
  This id is meant to serve as an identifier for external bookkeeping.
  Never bookkeep the item pointers as they may change during manipulation.
  Nevertheless, most manipulation methods are based on pointers since that
  is most practical. There is an conversion method between id's and pointers.

  The notification philosophy is to notify only under direct user-interaction
  You can override this for some cases by setting the notify argument to true

  The list view disables sorting as default.
  Single selection mode is default.

  Known bugs: Headerclick disabling doesn't work for frenetic clicking.
  Sometimes you are getting an unnecessary horizontal scroll,
  even if the items and header fit inside the given size.

  \author Dag R. Christensen
*/

class FFuListView : virtual public FFuComponentBase
{
public:
  FFuListView();
  virtual ~FFuListView() {}

  // This CB is invoked for any selection changes in the lv.
  void setPermSelectionChangedCB(const FFaDynCB0& aDynCB)
    {this->permSelectionChangedCB = aDynCB;}
  void setTmpSelectionChangedCB(const FFaDynCB1<FFuListViewItem*>& aDynCB)
    {this->tmpSelectionChangedCB = aDynCB;}
  void setMenuItemSelectedCB(const FFaDynCB2<const FFuListViewItems&,FFuaCmdItem*>& aDynCB)
    {this->menuItemSelectedCB = aDynCB;}
  void setItemExpandedCB(const FFaDynCB2<FFuListViewItem*,bool>& aDynCB)
    {this->itemExpandedCB = aDynCB;} 
  void setItemToggledCB(const FFaDynCB2<FFuListViewItem*,int>& aDynCB){
    this->itemToggledCB = aDynCB;}
  void setReturnPressedCB(const FFaDynCB1<FFuListViewItem*>& aDynCB)
    {this->returnPressedCB = aDynCB;}
  void setDoubleClickedCB(const FFaDynCB1<FFuListViewItem*>& aDynCB)
    {this->doubleClickedCB = aDynCB;}
  //FFuListViewItem* is selected when this cb is emitted
  void setLeftMouseBPressedCB(const FFaDynCB1<FFuListViewItem*>& aDynCB)
    {this->lMouseBPressedCB = aDynCB;}             
  void setLeftMouseBReleaseCB(const FFaDynCB0& aDynCB)                  
    {this->lMouseBReleaseCB = aDynCB;}
  //FFuListViewItem* is selected when this cb is emitted
  void setRightMouseBPressCB(const FFaDynCB1<FFuListViewItem*>& aDynCB)
    {this->rMouseBPressedCB = aDynCB;}
  void setHeaderClickedCB(const FFaDynCB1<int>& aDynCB)
    {this->headerClickedCB = aDynCB;}
 
  // LIST SETTINGS
  
  // If column is -1, sorting is disabled.
  virtual void setListSorting(int column,bool ascending=true) = 0;
  // If column is negative, clicking is enabled/disabled for all columns
  // HeaderClickEnabling also enables ascending sorting with regards to column 
  // when column is clicked
  virtual void setHeaderClickEnabled(int column,bool enable) = 0;
  virtual void setHeaderOff(bool Off)= 0;
  virtual void setAllListColumnsShowSelection(bool enable)= 0;

  // Sets this list view to show open/close signs on root items if enable is TRUE, 
  // and to not show such signs if enable is FALSE. Should be used if root objects 
  // have children
  virtual void setListRootIsDecorated(bool enable) = 0; 
  
  virtual void setSglSelectionMode(bool single) = 0;
  virtual bool isSglSelectionMode() const = 0;
  

  // BASIC LIST FUNCTION

  virtual void clearList() = 0;

  virtual void setListColumns(const std::vector<const char*>& labels) = 0;
  // sets the width mode to MANUAL for this column, default is MAXIMUM
  virtual void setListColumnWidth(int column, int width) = 0;
  virtual void resizeListColumnsToContents() = 0;

  // The notify setting is loyal to FFuComponentBase::blockNotification
  // In single sel mode this method touches other items selection, in other modes it does not.
  virtual void permSelectListItem(FFuListViewItem* item,bool select=true,bool notify=false)=0; 
  void permTotSelectListItems(const FFuListViewItems& totalSel, bool notify=false);
  void tmpSelectListItem(FFuListViewItem* item,bool notify=false); 
  virtual void clearListSelection(bool notify=false)=0; 
  virtual void openListItem(FFuListViewItem* item,bool open,bool notify=false)=0;
  virtual void ensureListItemVisible(FFuListViewItem* item,bool notify=false)=0;

  // Note: Trolltech operates with the terms "current item" and "selected item".
  // Current item is the one with keyboard focus, selected item is the highlighted.
  // Current item differentiates the items under multiselection mode.
  
  FFuListViewItems getSelectedListItems() const;
  virtual FFuListViewItem* getSelectedListItemSglMode() const = 0;
  virtual FFuListViewItem* getCurrentListItem() const = 0;
  virtual FFuListViewItem* getFirstChildItem() const = 0;
  FFuListViewItem* getListItemBefore(FFuListViewItem* itemsParent, int itemsListPosition) const;
  FFuListViewItems getListChildren(FFuListViewItem* parent); //ordered in lv order
  //all levels below parent, if parent = 0 all lv
  FFuListViewItems getAllListChildren(FFuListViewItem* parent) const; //ordered in lv order

  FFuListViewItem* getListItem(int itemId) const;

  FFuListViewItems arePresent(const FFuListViewItems& in) const;

  // LIST MANIPULATION
 
  // The item gets default text in column 0 when created.
  // If parent is null, the item is created at top level in the list view
  // If after is null the item is inserted first in parents list
  virtual FFuListViewItem* createListItem(const char* label = NULL,
                                          FFuListViewItem* parent = NULL,
                                          FFuListViewItem* after = NULL) = 0;
  void deleteListItem(FFuListViewItem* item);
  
  // POPUP MENU 
  FFuPopUpMenu* getPopUpMenu() const { return this->popUpMenu; }

protected:
  //slots
  void onPermSelectionChanged();
  void onTmpSelectionChanged(FFuListViewItem* listItem);
  void onMenuItemSelected(const FFuListViewItems& items, FFuaCmdItem* mItem);
  void onListItemOpened(FFuListViewItem* listItem, bool open);
public:
  void onListItemToggled(FFuListViewItem* listItem, int toggle);

protected:  
  //RE-IMPLs
  //These can be reimpl in subclass
  virtual void permSelectionChangedEvent() {}
  virtual void tmpSelectionChangedEvent(FFuListViewItem*) {}
  virtual void menuItemSelectedEvent(const FFuListViewItems&, FFuaCmdItem*) {}
  virtual void listItemOpenedEvent(FFuListViewItem*,bool) {}
  virtual void listItemToggledEvent(FFuListViewItem*,int) {}
  virtual void rightMousePressed(FFuListViewItem*) {}

  // These methodes are called every time a lv pops up/down, also
  // from an iconified position
  virtual void onPoppedUp() {}
  virtual void onPoppedDown() {}

  // to/from memory only
  // called every time a ui appears on the screen (not from icon)
  virtual void onPoppedUpFromMem() {}
  // called when a ui is on its way to the memory (hide)
  virtual void onPoppedDownToMem() {}

  void executePopUp(FFuListViewItem* listItem);

protected:
  // Component "shadow" members
  FFuPopUpMenu*    popUpMenu;
  FFuListViewItem* tmpSelected;
  FFuListViewItems permSelectedDuringTmpSelection;

  std::map<int,FFuListViewItem*> lviMap;

  // Internal call-back objects :
  FFaDynCB0                   permSelectionChangedCB;
  FFaDynCB1<FFuListViewItem*> tmpSelectionChangedCB;
  FFaDynCB2<FFuListViewItem*,bool> itemExpandedCB;
  FFaDynCB2<FFuListViewItem*,int> itemToggledCB;
  FFaDynCB1<FFuListViewItem*> doubleClickedCB;
  FFaDynCB1<FFuListViewItem*> returnPressedCB;
  FFaDynCB1<FFuListViewItem*> lMouseBPressedCB;
  FFaDynCB0                   lMouseBReleaseCB;
  FFaDynCB1<FFuListViewItem*> rMouseBPressedCB;
  FFaDynCB1<int>              headerClickedCB;

  FFaDynCB2<const FFuListViewItems&,FFuaCmdItem*> menuItemSelectedCB;
};

#endif
