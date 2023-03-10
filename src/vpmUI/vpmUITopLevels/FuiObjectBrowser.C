// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiObjectBrowser.H"
#include "vpmUI/Pixmaps/objectBrowser1.xpm"
#include "vpmUI/Pixmaps/objectBrowser2.xpm"
#include "vpmUI/Fui.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmLink.H"
#include "vpmDB/FmResultBase.H"
#include "vpmDB/FmSubAssembly.H"

#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuMemo.H"
#include "FFuLib/FFuListView.H"
#include "FFuLib/FFuListViewItem.H"
#include "FFuLib/FFuPushButton.H"


Fmd_SOURCE_INIT(FUI_OBJECTBROWSER,FuiObjectBrowser,FFuTopLevelShell);

//----------------------------------------------------------------------------

FuiObjectBrowser::FuiObjectBrowser()
{
  Fmd_CONSTRUCTOR_INIT(FuiObjectBrowser);
}
//----------------------------------------------------------------------------

void FuiObjectBrowser::initWidgets()
{
  this->headerImage1->setPixMap(objectBrowser1_xpm, false);
  this->headerImage2->setPixMap(objectBrowser2_xpm, true);

  this->searchLabel->setLabel("Search and browse objects:");

  this->searchField->setValue("(All)");
  this->searchField->setToolTip("Type a search expression here. See Help for more information.");
  this->searchField->setAcceptedCB(FFaDynCB1M(FuiObjectBrowser,this,
					      onSearchFieldChanged,char*));

  this->searchButton->setLabel("Search");
  this->searchButton->setActivateCB(FFaDynCB0M(FuiObjectBrowser,this,onSearchButtonClicked));
  this->searchButton->setToolTip("Click to perform search");

  this->searchView->clearList();
  this->searchView->setListColumns({"BaseID","Type and UserID","Description"});
  this->searchView->setListRootIsDecorated(true);
  this->searchView->setAllListColumnsShowSelection(true);
  this->searchView->setHeaderClickEnabled(-1,true);
  this->searchView->setHeaderOff(true);
  this->searchView->setListSorting(0,true); // enable sorting
  this->searchView->setPermSelectionChangedCB(FFaDynCB0M(FuiObjectBrowser,this,
					      onSearchViewSelectionChanged));

  this->outputLabel->setLabel("Object details:");

  this->outputMemo->clearText();
  this->outputMemo->setEditable(false);

  this->closeButton->setLabel("Close");
  this->closeButton->setActivateCB(FFaDynCB0M(FFuComponentBase,this,popDown));

  this->helpButton->setLabel("Help");
  this->helpButton->setActivateCB(FFaDynCB0M(FuiObjectBrowser,this,onHelpButtonClicked));

  FFuaPalette pal;
  pal.setStdBackground(211,211,211);
  this->sepLabel->setColors(pal);

  this->copyDataButton->setLabel("Copy data");
  this->copyDataButton->setActivateCB(FFaDynCB0M(FuiObjectBrowser,this,onCopyDataButtonClicked));

  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//----------------------------------------------------------------------------

void FuiObjectBrowser::placeWidgets(int width, int height)
{
  int border = this->getBorder();
  int fieldHeight = 20; // hard coded field height?!

  int v1 = border;
  int v2 = width/2 - 2;
  int v3 = width/2 + 2;
  int v4 = width-border;
  int y1 = border + 77;
  int y2 = height - border - 33;

  this->headerImage1->setEdgeGeometry(0, 605, 0, 74);
  this->headerImage2->setEdgeGeometry(0, width, 0, 77);

  int y = y1;
  this->searchLabel->setEdgeGeometry(v1, v2, y, y+fieldHeight);
  y += fieldHeight+2;
  this->searchField->setEdgeGeometry(v1, v2-93, y, y+fieldHeight);
  this->searchButton->setEdgeGeometry(v2-88, v2, y, y+fieldHeight);
  y += fieldHeight+2;

  this->searchView->setEdgeGeometry(v1, v2, y, y2);

  y = y2 + border;
  int d = (v2-v1)/3;
  this->closeButton->setEdgeGeometry(v1, v1+d-4, y, height-border);
  this->helpButton->setEdgeGeometry(v1+d, v1+d*2-4, y, height-border);
  this->sepLabel->setEdgeGeometry(v1+d*2, v1+d*2+1, y, height-border);
  this->copyDataButton->setEdgeGeometry(v1+d*2+4, v1+d*3, y, height-border);

  y = y1;
  this->outputLabel->setEdgeGeometry(v3, v4, y, y+fieldHeight);
  y += fieldHeight+2;
  this->outputMemo->setEdgeGeometry(v3, v4, y, height-border);

  if (height >= 200) {
    this->closeButton->popUp();
    this->helpButton->popUp();
    this->sepLabel->popUp();
    this->copyDataButton->popUp();
  }
  else {
    this->closeButton->popDown();
    this->helpButton->popDown();
    this->sepLabel->popDown();
    this->copyDataButton->popDown();
  }
}
//-----------------------------------------------------------------------------

void FuiObjectBrowser::onPoppedUp()
{
  this->placeWidgets(this->getWidth(),this->getHeight());
  this->updateUIValues();
}
//----------------------------------------------------------------------------

void FuiObjectBrowser::onSearchButtonClicked()
{
  // Clear
  searchView->clearList();
  searchView->setHeaderOff(false); // show header

  // Get search field
  std::string pszSearch = searchField->getValue();

  // Groups
  FFuListViewItem* newItem = NULL;
  FFuListViewItem* gTriads = NULL;
  FFuListViewItem* gParts = NULL;
  FFuListViewItem* gResults = NULL;
  FFuListViewItem* gSubass = NULL;
  if (pszSearch.empty() || FFaUpperCaseString(pszSearch) == "(ALL)") {
    pszSearch.clear();
    gTriads = searchView->createListItem(0,0,0,"Triads");
    gParts = searchView->createListItem(0,0,0,"Parts");
    gResults = searchView->createListItem(0,0,0,"Results");
    gSubass = searchView->createListItem(0,0,0,"High-level");
  }

  // Get all
  std::vector<FmModelMemberBase*> items;
  std::vector<FmModelMemberBase*>::reverse_iterator it;
  FmDB::getAllOfType(items,FmModelMemberBase::getClassTypeID());
  for (it = items.rbegin(); it != items.rend(); ++it)
  {
    // Search
    if (!pszSearch.empty())
      if ((*it)->getUserDescription().find(pszSearch) == std::string::npos)
        continue; // doesn't match

    // Create list view item
    if ((*it)->isOfType(FmTriad::getClassTypeID()))
      newItem = searchView->createListItem(gTriads,0);
    else if ((*it)->isOfType(FmLink::getClassTypeID()))
      newItem = searchView->createListItem(gParts,0);
    else if ((*it)->isOfType(FmResultBase::getClassTypeID()))
      newItem = searchView->createListItem(gResults,0);
    else if ((*it)->isOfType(FmSubAssembly::getClassTypeID()))
      newItem = searchView->createListItem(gSubass,0);
    else
      newItem = searchView->createListItem(0,0);

    newItem->setItemText(0,FFaNumStr("%5d",(*it)->getBaseID()).c_str());
    newItem->setItemText(1,(*it)->getIdString().c_str());
    newItem->setItemText(2,(*it)->getUserDescription().c_str());
    searchView->openListItem(newItem,true);
  }
}
//----------------------------------------------------------------------------

void FuiObjectBrowser::onSearchViewSelectionChanged()
{
  this->outputMemo->clearText();

  std::vector<FFuListViewItem*> lvItems = searchView->getSelectedListItems();
  if (lvItems.empty()) return;

  int nBaseId = std::stoi(lvItems.front()->getItemText(0));
  FmSimulationModelBase* obj = dynamic_cast<FmSimulationModelBase*>(FmDB::findObject(nBaseId));
  if (obj) outputMemo->setAllText(obj->getObjectInfo().c_str());
}
//----------------------------------------------------------------------------

void FuiObjectBrowser::onHelpButtonClicked()
{
  Fui::showCHM("dialogbox/object-browser.htm");
}
//----------------------------------------------------------------------------

FFuaUIValues* FuiObjectBrowser::createValuesObject()
{
  return new FuaObjectBrowserValues();
}
//----------------------------------------------------------------------------

void FuiObjectBrowser::setUIValues(const FFuaUIValues* values)
{
  // Clear fields
  this->searchField->setValue("(All)");
  this->searchView->clearList();
  this->searchView->setHeaderOff(false); // show header
  this->outputMemo->clearText();

  // Get selected item
  FmModelMemberBase* obj = static_cast<const FuaObjectBrowserValues*>(values)->obj;
  if (!obj) return;

  // Search field
  searchField->setValue(obj->getUserDescription());

  // List view
  FFuListViewItem* newItem = searchView->createListItem(0,0);
  newItem->setItemText(0,FFaNumStr("%5d",obj->getBaseID()).c_str());
  newItem->setItemText(1,obj->getIdString().c_str());
  newItem->setItemText(2,obj->getUserDescription().c_str());
  searchView->openListItem(newItem,true);

  // Memo field
  FmSimulationModelBase* simobj = dynamic_cast<FmSimulationModelBase*>(obj);
  if (simobj) outputMemo->setAllText(simobj->getObjectInfo().c_str());
}
