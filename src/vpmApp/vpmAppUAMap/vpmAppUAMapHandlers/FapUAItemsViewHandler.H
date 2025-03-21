// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_ITEMS_VIEW_HANDLER_H
#define FAP_UA_ITEMS_VIEW_HANDLER_H

#include "vpmApp/FapEventManager.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <vector>
#include <map>

class FFuUAItemsViewHandler;
class FFaViewItem;


/*!
  \brief Plugin class and receiver for FFuUAItemsViewHandler.

  The task of this class is to handle viewing and simple manipulation
  of DB components in a UI.
  Plugin whenever needing that functionality.
*/

class FapUAItemsViewHandler
{
public:
  //! \brief The constructor sets up the FFuUAItemsViewHandler callbacks
  FapUAItemsViewHandler(FFuUAItemsViewHandler* ui);
  virtual ~FapUAItemsViewHandler() {}

  const char* getTypeIDName() const { return "FapUAItemsViewHandler"; }

  // Functionality
  void setHasApplIndependentSelection(bool h) { hasApplIndependentSelection = h; }
  void setEnsureItemVisibleOnPermSelection(bool v) { ensureItemVisibleOnPermSelection = v; }

  virtual void updateSession() {}
  virtual void clearSession();

  std::vector<FFaViewItem*> getUISelectedItems() const;
  bool isItemUISelected(FFaViewItem* item) const;

  void permTotSelectUIItems(const std::vector<FFaViewItem*>& totalSelection);

  void setPermTotUISelectionChangedCB(const FFaDynCB1<FapUAItemsViewHandler*>& dynCB)
  {this->permTotUISelectionChangedCB = dynCB;}

protected:
  virtual void deleteUIItem(FFaViewItem* item);
  bool shouldIUpdateOnChanges() const;
  virtual void ensureItemVisible(FFaViewItem*) {}

  // internal message givers
  virtual void onPermTotSelectionChanged(const std::vector<FFaViewItem*>&) {}
  virtual void permSelectionChangedEvent() {}
  virtual void tmpSelectionChangedEvent() {}

  virtual void permTotSelectItems(std::vector<int>& totalSelection);
  void permUnselectAll();

  // misc
  std::vector<FFaViewItem*> convertItems(const std::vector<int>& in) const;
  std::vector<int> convertItems(const std::vector<FFaViewItem*>& in) const;

private:
  // For callback purpose only
  void setUpdateOnChangesWhenInMem(bool u) { updateOnChangesWhenInMem = u; }
  void getUpdateOnChangesWhenInMem(bool& u) { u = updateOnChangesWhenInMem; }

  // slots from EventManager
  void onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
			      const std::vector<FFaViewItem*>& selectedSinceLast,
			      const std::vector<FFaViewItem*>& unselectedSinceLast);

  void onTmpSelectionChanged(FFaViewItem* tmpSelected,
			     FFaViewItem* tmpUnselected,
			     const std::vector<FFaViewItem*>& totalSelection);

  // This method should be re-impl if you want this class to work as expected
  virtual bool getItemSelectAble(FFaViewItem*) { return true; }

protected:
  // ItemMap interface
  int getMapItem(FFaViewItem* item) const;
  FFaViewItem* getMapItem(int item) const;

  void putMapItem(int intItem, FFaViewItem* ptrItem);

  void eraseMapItem(int item);
  void eraseMapItem(FFaViewItem* item);

  void clearMaps();

protected:
  std::map<int,FFaViewItem*> intMap;
  std::map<FFaViewItem*,int> ptrMap;

  // If not, you're supposed to updateSession/clearSession on
  // onPoppedUpFromMem/onPoppedDownToMem
  bool updateOnChangesWhenInMem;
  bool hasApplIndependentSelection;
  bool ensureItemVisibleOnPermSelection;

private:
  FFuUAItemsViewHandler* ui;
  FFaDynCB1<FapUAItemsViewHandler*> permTotUISelectionChangedCB;

  // Signal receivers
  FapPermSelChangedReceiver<FapUAItemsViewHandler> permSelReceiver;
  FapTmpSelChangedReceiver<FapUAItemsViewHandler>  tmpSelReceiver;
  friend class FapPermSelChangedReceiver<FapUAItemsViewHandler>;
  friend class FapTmpSelChangedReceiver<FapUAItemsViewHandler>;
};

#endif
