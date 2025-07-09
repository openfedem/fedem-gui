// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_OBJECT_BROWSER_H
#define FUI_OBJECT_BROWSER_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"

class FFuIOField;
class FFuPushButton;
class FFuListView;
class FFuListViewItem;
class FFuMemo;
class FmModelMemberBase;


class FuiObjectBrowser : virtual public FFuTopLevelShell,
			 public FFuUAExistenceHandler
{
  Fmd_HEADER_INIT();

public:
  FuiObjectBrowser();

  static FuiObjectBrowser* create(int xpos = 100, int ypos = 100,
				  int width = 100, int height = 100,
				  const char* title= "Fedem Object Browser",
				  const char* name = "FuiObjectBrowser");

protected:
  void initWidgets();

  virtual void onCopyDataButtonClicked() = 0;

private:
  void onSearchButtonClicked();
  void onSearchFieldChanged(char*) { this->onSearchButtonClicked(); }
  void onHelpButtonClicked();
  void onSelectionChanged();
  void onHeaderClicked(int col);
  void addListViewItem(FmModelMemberBase* obj, FFuListViewItem* parent = NULL);

public:
  void update(FmModelMemberBase* newObj, bool updateCurrent = false);

private:
  int lastSorted;
  bool sortOrder;

  FmModelMemberBase* myObj;

protected:
  FFuIOField*    searchField;
  FFuPushButton* searchButton;
  FFuListView*   searchView;
  FFuMemo*       outputMemo;
  FFuPushButton* closeButton;
  FFuPushButton* helpButton;
  FFuPushButton* copyDataButton;
};

#endif
