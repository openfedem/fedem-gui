// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_MINI_FILE_BROWSER_H
#define FUI_MINI_FILE_BROWSER_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <vector>

class FFuListView;
class FFuListViewItem;
class FFuMemo;
class FFuDialogButtons;
class FFuaCmdItem;


class FuiMiniFileBrowser : virtual public FFuTopLevelShell,
                           public FFuUAExistenceHandler,
                           public FFuUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FuiMiniFileBrowser();

  // static factory method
  static FuiMiniFileBrowser* create(int xpos, int ypos, int width, int height);

  // List view operations
  int  createItem(int parent, int after, const std::string& label,
                  const std::string& size="", const std::string& modified="",
                  const char** icon = NULL);
  bool updateItem(int item, const std::string& label,
                  const std::string& size="", const std::string& modified="",
                  const char** icon = NULL);
  bool deleteItem(int item);
  bool isItemPresent(int id);
  bool isItemExpanded(int id);
  void selectItemId(int id, bool select=true);
  void clearListView();
  void setPopUpCmds(const std::vector<FFuaCmdItem*>& cmdItems);
  void updateCmdSensitivity(FFuaCmdItem* item);
  void updateCmdToggle(FFuaCmdItem* item);
  void getListViewSelection(std::vector<int>& selected);
  void openListViewItem(int id, bool open=true, bool notify=false);
  std::vector<int> getAllListViewChildren(int parent);
  std::vector<int> getListViewChildren(int parent);
  std::vector<int> getListViewAncestors(int descendant);
  int getListViewParent(int child);

  // List view related CBs
  void setTempSelectionChangedCB(const FFaDynCB1<int>& aDynCB)
  {myTmpSelectionChangedCB = aDynCB;}
  void setPermSelectionChangedCB(const FFaDynCB0& aDynCB)
  {myPermSelectionChangedCB = aDynCB;}
  void setItemExpandedCB(const FFaDynCB2<int,bool>& aDynCB)
  {myItemExpandedCB = aDynCB;}

  // Text info operations
  void setFileToShow(const std::string& fileName);
  void setText(const std::string& text);
  void appendText(const char* text);
  void clearTextInfo();
  void clearAll();
  bool isViewingTextEnd();
  bool isDraggingVScroll();
  void scrollTextToBottom();

  // Debug cbs
  void setKillCB(const FFaDynCB0& cb) { myKillCB = cb; }
  void setRebuildCB(const FFaDynCB0& cb) { myRebuildCB = cb; }

protected:
  void initWidgets();
  virtual bool onClose();

  FFuListView*      listView;
  FFuMemo*          infoView;
  FFuDialogButtons* dialogButtons;

private:
  // CBs from list view
  void onLVTmpSelectionChanged(FFuListViewItem* item);
  void onLVPermSelectionChanged();
  void onLVItemExpanded(FFuListViewItem* item, bool expanded);

  // from btn
  void onDialogButtonClicked(int btn);

  FFaDynCB1<int> myTmpSelectionChangedCB;
  FFaDynCB0 myPermSelectionChangedCB;
  FFaDynCB2<int,bool> myItemExpandedCB;
  FFaDynCB0 myKillCB, myRebuildCB;
};

#endif
