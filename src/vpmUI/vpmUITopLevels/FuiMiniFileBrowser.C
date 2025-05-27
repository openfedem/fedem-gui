// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiMiniFileBrowser.H"
#include "FFuLib/FFuListView.H"
#include "FFuLib/FFuListViewItem.H"
#include "FFuLib/FFuMemo.H"
#include "FFuLib/FFuDialogButtons.H"
#include "FFuLib/FFuPopUpMenu.H"


/*!
  \class FuiMiniFileBrowser FuiMiniFileBrowser.H

  \brief An autonomous result file browser, with delete, enable and disable
  result file capabilities. This is the ui part.

  \note If you #define FUI_DEBUG you will get two additional
  buttons at the bottom; Kill and Rebuild
  \author Sven-Kaare Evenseth \date March 2003
  \sa FapUAMiniFileBrowser
*/


Fmd_SOURCE_INIT(FUI_MINIFILEBROWSER, FuiMiniFileBrowser, FFuTopLevelShell);


//! list view columns
enum {FILE_COL, SIZE_COL, MODIFIED_COL};


FuiMiniFileBrowser::FuiMiniFileBrowser()
{
  Fmd_CONSTRUCTOR_INIT(FuiMiniFileBrowser);

  listView = NULL;
  infoView = NULL;

  dialogButtons = NULL;
}


/*!
  Creates an item in list view
  \return item id if success, -1 if item could not be created
*/

int FuiMiniFileBrowser::createItem(int parent, int after, const std::string& label,
				   const std::string& size, const std::string& modified,
				   const char** icon)
{
  FFuListViewItem* item = listView->createListItem(label.c_str(),
                                                   listView->getListItem(parent),
                                                   listView->getListItem(after));
  if (!item) return -1;

  item->setItemText(SIZE_COL, size.c_str());
  item->setItemText(MODIFIED_COL, modified.c_str());
  item->setItemImage(FILE_COL, icon);

  return item->getItemId();
}


/*!
  Updates an item
  \return true if success, false if item is not present in list view.
*/

bool FuiMiniFileBrowser::updateItem(int id, const std::string& label, const std::string& size,
				    const std::string& modified, const char** icon)
{
  FFuListViewItem* item = listView->getListItem(id);
  if (!item) return false;

  item->setItemText(FILE_COL, label.c_str());
  item->setItemImage(FILE_COL, icon);
  item->setItemText(SIZE_COL, size.c_str());
  item->setItemText(MODIFIED_COL, modified.c_str());

  return true;
}


/*!
  Removes an item from list view
  \return true if success, false if item is not in list view
*/

bool FuiMiniFileBrowser::deleteItem(int id)
{
  FFuListViewItem* item = listView->getListItem(id);
  if (!item) return false;

  listView->deleteListItem(item);
  return true;
}


/*!
  Selects or deselects the list view item \a id
*/

void FuiMiniFileBrowser::selectItemId(int id, bool select)
{
  FFuListViewItem* item = listView->getListItem(id);
  if (item) listView->permSelectListItem(item,select);
}


/*!
  Checks if item with \a id is present
*/

bool FuiMiniFileBrowser::isItemPresent(int id)
{
  return listView->getListItem(id) ? true : false;
}


/*!
  \return true if item \a id is expanded
*/

bool FuiMiniFileBrowser::isItemExpanded(int id)
{
  FFuListViewItem* item = listView->getListItem(id);
  return item ? item->isItemExpanded() : false;
}


/*!
  Clears list view
*/

void FuiMiniFileBrowser::clearListView()
{
  listView->clearList();
}


/*!
  Will open (expand) an item, if it has children. Note that it will expand
  an item also if it doesn't have children, but it will not be visible
  until before it gets one or more. Children. Sj�.
*/

void FuiMiniFileBrowser::openListViewItem(int id, bool open, bool notify)
{
  FFuListViewItem* item = listView->getListItem(id);
  if (!item) return;

  listView->openListItem(item, open, notify);
}


/*!
  Clears list view and info view
*/

void FuiMiniFileBrowser::clearAll()
{
  this->clearListView();
  this->clearTextInfo();
}


bool FuiMiniFileBrowser::isViewingTextEnd()
{
  return infoView->isViewingEnd();
}

bool FuiMiniFileBrowser::isDraggingVScroll()
{
  return infoView->isDraggingVScroll();
}

void FuiMiniFileBrowser::scrollTextToBottom()
{
  infoView->scrollToEnd();
}


/*!
  Inserts \a cmdItems in list view pop up
*/

void FuiMiniFileBrowser::setPopUpCmds(const std::vector<FFuaCmdItem*>& cmdItems)
{
  FFuPopUpMenu* menu = listView->getPopUpMenu();
  menu->deleteItems();

  for (FFuaCmdItem* cmd : cmdItems)
    menu->insertCmdItem(cmd);
}


/*!
  Updates sensitivity on \a item
*/

void FuiMiniFileBrowser::updateCmdSensitivity(FFuaCmdItem* item)
{
  listView->getPopUpMenu()->updateCmdItem(item,true);
}


/*!
  Updates \a item toggle
*/

void FuiMiniFileBrowser::updateCmdToggle(FFuaCmdItem* item)
{
  listView->getPopUpMenu()->updateCmdItem(item,false);
}


/*!
  Passes fileName to text view for display
*/

void FuiMiniFileBrowser::setFileToShow(const std::string& fileName)
{
  infoView->displayTextFile(fileName.c_str());
}


/*!
  Sets text to display in text view
*/

void FuiMiniFileBrowser::setText(const std::string& text)
{
  infoView->setAllText(text.c_str());
}


/*!
  Appends text to display in text view.
*/

void FuiMiniFileBrowser::appendText(const char* text)
{
  infoView->insertText(text,true);
}


/*!
  Clears text view
*/

void FuiMiniFileBrowser::clearTextInfo()
{
  infoView->clearText();
}


/*!
  Customizes widgets, sets call backs etc
*/

void FuiMiniFileBrowser::initWidgets()
{
  listView->setListColumns({"File","Size","Last modified"});
  listView->setListColumnWidth(2, 100);
  listView->setListRootIsDecorated(true);
  listView->setSglSelectionMode(false);
  listView->setHeaderClickEnabled(-1, false);
  listView->setAllListColumnsShowSelection(true);

  listView->setPermSelectionChangedCB(FFaDynCB0M(FuiMiniFileBrowser, this,
						 onLVPermSelectionChanged));
  listView->setTmpSelectionChangedCB(FFaDynCB1M(FuiMiniFileBrowser, this,
 						onLVTmpSelectionChanged, FFuListViewItem*));
  listView->setItemExpandedCB(FFaDynCB2M(FuiMiniFileBrowser, this,
					 onLVItemExpanded, FFuListViewItem*, bool));

  // info view settings
  infoView->setEditable(false);
  infoView->enableUndoRedo(false);
  infoView->setNoWordWrap();

  // dialog button settings
  dialogButtons->setButtonLabel(FFuDialogButtons::LEFTBUTTON, "Close");
#ifdef FUI_DEBUG
  dialogButtons->setButtonLabel(FFuDialogButtons::MIDBUTTON, "Kill");
  dialogButtons->setButtonLabel(FFuDialogButtons::RIGHTBUTTON, "Rebuild");
#endif

  dialogButtons->setButtonClickedCB(FFaDynCB1M(FuiMiniFileBrowser,this,onDialogButtonClicked,int));

  FFuUAExistenceHandler::invokeCreateUACB(this);
}


//! CB from list view
void FuiMiniFileBrowser::onLVTmpSelectionChanged(FFuListViewItem* item)
{
  this->listView->clearListSelection(false);
  if (item)
    this->listView->permTotSelectListItems({item},true);
}


//! CB from list view
void FuiMiniFileBrowser::onLVPermSelectionChanged()
{
  myPermSelectionChangedCB.invoke();
}


//! CB from list view
void FuiMiniFileBrowser::onLVItemExpanded(FFuListViewItem* item, bool expanded)
{
  myItemExpandedCB.invoke(item ? item->getItemId() : -1, expanded);
}


//! CB from close button
void FuiMiniFileBrowser::onDialogButtonClicked(int btn)
{
  if (btn == FFuDialogButtons::LEFTBUTTON)
    this->invokeFinishedCB();
#ifdef FUI_DEBUG
  else if (btn == FFuDialogButtons::MIDBUTTON)
    this->myKillCB.invoke();
  else if (btn == FFuDialogButtons::RIGHTBUTTON)
    this->myRebuildCB.invoke();
#endif
}


//! Event from finish handler
bool FuiMiniFileBrowser::onClose()
{
  this->invokeFinishedCB();
  return false;
}


/*!
  \return ids of all descendants (recursive) of \a parent
*/

std::vector<int> FuiMiniFileBrowser::getAllListViewChildren(int parent)
{
  std::vector<FFuListViewItem*> items = listView->getAllListChildren(listView->getListItem(parent));
  std::vector<int> ret;
  ret.reserve(items.size());

  for (FFuListViewItem* item : items)
    ret.push_back(item->getItemId());

  return ret;
}


/*!
  \return children (on first level) of \a parent
*/

std::vector<int> FuiMiniFileBrowser::getListViewChildren(int parent)
{
  std::vector<FFuListViewItem*> items = listView->getListChildren(listView->getListItem(parent));
  std::vector<int> ret;
  ret.reserve(items.size());

  for (FFuListViewItem* item : items)
    ret.push_back(item->getItemId());

  return ret;
}


/*!
  \return a std::vector with all predecessors (parent, grand-parent etc)
  of \a descentant. Youngest first (-> vec[0] == parent, vec[1] = grand parent, etc)
*/

std::vector<int> FuiMiniFileBrowser::getListViewAncestors(int descendant)
{
  std::vector<int> ret;

  FFuListViewItem* item = listView->getListItem(descendant);
  if (item) item = item->getParentItem();

  while (item) {
    ret.push_back(item->getItemId());
    item = item->getParentItem();
  }

  return ret;
}


/*!
  \return parent of \a child in list view
*/

int FuiMiniFileBrowser::getListViewParent(int child)
{
  FFuListViewItem* uichild = listView->getListItem(child);
  if (!uichild) return -1;

  FFuListViewItem* uiparent = uichild->getParentItem();
  return uiparent ? uiparent->getItemId() : -1;
}


/*!
  \return references to tmp and perm selected items
*/

void FuiMiniFileBrowser::getListViewSelection(std::vector<int>& selected)
{
  std::vector<FFuListViewItem*> sel = listView->getSelectedListItems();
  selected.clear();
  selected.reserve(sel.size());

  for (FFuListViewItem* item : sel)
    selected.push_back(item ? item->getItemId() : -1);
}
