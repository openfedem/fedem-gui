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


/*!
  The list view items are given an unique id (>-1) when created.
  This id is meant to serve as an identifier for external bookkeeping.
  Never bookkeep the item pointers as they may change during manipulation.
  Nevertheless, most manipulation methods are based on pointers since that
  is most practical. There is an conversion method between id's and pointers.

  The notification philosophy is to notify only under direct user-interaction
  You can override this for some cases by setting the notify argument to true

  The listview disables sorting as default.
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
  void setMenuItemSelectedCB(const FFaDynCB2<const std::vector<FFuListViewItem*>&,
			     FFuaCmdItem*>& aDynCB) 
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
  void permTotSelectListItems(const std::vector<FFuListViewItem*>& totalSel, bool notify=false);
  void tmpSelectListItem(FFuListViewItem* item,bool notify=false); 
  virtual void clearListSelection(bool notify=false)=0; 
  virtual void openListItem(FFuListViewItem* item,bool open,bool notify=false)=0;
  virtual void ensureListItemVisible(FFuListViewItem* item,bool notify=false)=0;

  // Note: Trolltech operates with the terms "current item" and "selected item".
  // Current item is the one with keyboard focus, selected item is the highlighted.
  // Current item differentiates the items under multiselection mode.
  
  std::vector<FFuListViewItem*> getSelectedListItems() const;
  virtual FFuListViewItem* getSelectedListItemSglMode() const = 0;
  virtual FFuListViewItem* getCurrentListItem() const = 0;
  virtual FFuListViewItem* getFirstChildItem() const = 0;
  FFuListViewItem* getListItemBefore(FFuListViewItem* itemsParent, int itemsListPosition) const;
  std::vector<FFuListViewItem*> getListChildren(FFuListViewItem* parent);//ordered in lv order 
  //all levels below parent, if parent = 0 all lv
  std::vector<FFuListViewItem*> getAllListChildren(FFuListViewItem* parent) const;//ordered in lv order

  FFuListViewItem* getListItem(int itemId) const;

  std::vector<FFuListViewItem*> arePresent(const std::vector<FFuListViewItem*>& in) const;

  // LIST MANIPULATION
 
  // The item gets default text in col 0 when created.
  // If parent=0, the item is created at top level in the list view
  // If after=0 the item is inserted first in parents list

  virtual FFuListViewItem* createListItem(FFuListViewItem* parent=0,FFuListViewItem* after=0,
					  FFuListViewItem* original=0,const char* label="item") = 0;
  
  void deleteListItem(FFuListViewItem* item);

  // Copies item and all children to desired position
  // Return 0 if irregularities
  FFuListViewItem* copyListItem(FFuListViewItem* itemToCopy,FFuListViewItem* copyToParent=0,
				FFuListViewItem* copyToAfter=0,bool copyIndices=false);
  
  // Moves item and all children to desired position
  // This method actually creates new items and therefore returns the new (parent) item's pointer, 
  // but inherits the original's indices.  
  // DO NEVER USE THE ITEM'S OLD POINTER AFTER A MOVEMENT.
  // Return 0 if irregularities, autom unselect and unexpand
  FFuListViewItem* moveListItem(FFuListViewItem* itemToMove,FFuListViewItem* moveToParent=0,
				FFuListViewItem* moveToAfter=0);
  
  // POPUP MENU 
  FFuPopUpMenu* getPopUpMenu() const { return this->popUpMenu; }

protected:
  //slots
  void onPermSelectionChanged();		
  void onTmpSelectionChanged(FFuListViewItem* listItem);
  void onMenuItemSelected(const std::vector<FFuListViewItem*>& listItems,
			  FFuaCmdItem* menuItem);		
  void onListItemOpened(FFuListViewItem* listItem,bool open);
public:
  void onListItemToggled(FFuListViewItem* listItem,int toggle);

protected:  
  //RE-IMPLs
  //These can be reimpl in subclass
  virtual void permSelectionChangedEvent() {}		
  virtual void tmpSelectionChangedEvent(FFuListViewItem*) {}
  virtual void menuItemSelectedEvent(const std::vector<FFuListViewItem*>&, FFuaCmdItem*) {}
  virtual void listItemOpenedEvent(FFuListViewItem*,bool) {}
  virtual void listItemToggledEvent(FFuListViewItem*,int) {}
  virtual void rightMousePressed(FFuListViewItem*) {}

protected:  
  
  // These methodes are called every time a lv pops up/down, also
  // from an iconified position
  virtual void onPoppedUp() {}
  virtual void onPoppedDown() {}

  // to/from memory only
  // called every time a ui appears on the screen (not from icon)
  virtual void onPoppedUpFromMem() {}
  // called when a ui is on its way to the memory (hide)
  virtual void onPoppedDownToMem() {}
  

protected:
  void invokePermSelectionChangedCB() 
    {this->permSelectionChangedCB.invoke();}
  void invokeTmpSelectionChangedCB(FFuListViewItem* listItem) 
    {this->tmpSelectionChangedCB.invoke(listItem);}
  void invokeMenuItemSelectedCB(const std::vector<FFuListViewItem*>& listItems,
				FFuaCmdItem* menuItem)
    {this->menuItemSelectedCB.invoke(listItems,menuItem);}
  void invokeItemExpandedCB(FFuListViewItem* item,bool open)
    {this->itemExpandedCB.invoke(item,open);}
  void invokeItemToggledCB(FFuListViewItem* item,int toggle)
    {this->itemToggledCB.invoke(item,toggle);}
  void invokeDoubleClickedCB(FFuListViewItem* listItem) 
    {this->doubleClickedCB.invoke(listItem);}
  void invokeReturnPressedCB(FFuListViewItem* listItem)
    {this->returnPressedCB.invoke(listItem);}
  void invokeLeftMouseBPressedCB(FFuListViewItem* listItem)
    {this->lMouseBPressedCB.invoke(listItem);}
  void invokeLeftMouseBReleaseCB()
    {this->lMouseBReleaseCB.invoke();}
  void invokeRightMouseBPressedCB(FFuListViewItem* listItem)
    {this->rMouseBPressedCB.invoke(listItem);}

  void executePopUp(FFuListViewItem* listItem);

protected:
  // Component "shadow" members
  FFuPopUpMenu* popUpMenu; 

  FFuListViewItem* tmpSelected;
  std::vector<FFuListViewItem*> permSelectedDuringTmpSelection;

  std::map<int,FFuListViewItem*> lviMap;

private:
  // Internal CB objects :
  FFaDynCB0                   permSelectionChangedCB;
  FFaDynCB1<FFuListViewItem*> tmpSelectionChangedCB;
  FFaDynCB2<FFuListViewItem*,bool> itemExpandedCB;
  FFaDynCB2<FFuListViewItem*,int> itemToggledCB;
  FFaDynCB1<FFuListViewItem*> doubleClickedCB;
  FFaDynCB1<FFuListViewItem*> returnPressedCB;
  FFaDynCB1<FFuListViewItem*> lMouseBPressedCB;
  FFaDynCB0                   lMouseBReleaseCB;
  FFaDynCB1<FFuListViewItem*> rMouseBPressedCB;

  FFaDynCB2<const std::vector<FFuListViewItem*>&,FFuaCmdItem*> menuItemSelectedCB;
};
/////////////////////////////////////////////////////////////////////////////

#endif
