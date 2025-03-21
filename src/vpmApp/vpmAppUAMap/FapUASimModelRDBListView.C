// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUASimModelRDBListView.H"
#include "vpmUI/vpmUIComponents/FuiItemsListView.H"

#include "vpmDB/FmModelMemberBase.H"
#include "vpmDB/FmSimulationModelBase.H"
#include "vpmDB/FmRingStart.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmSubAssembly.H"
#include "vpmDB/FmSeaState.H"

#include "vpmPM/FpExtractor.H"
#include "FFrLib/FFrEntryBase.H"
#include "FFrLib/FFrVariableReference.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#ifdef LV_DEBUG
#include <time.h>
#endif


//----------------------------------------------------------------------------

FapUASimModelRDBListView::FapUASimModelRDBListView(FuiItemsListView* ui)
  : FapUAItemsListView(ui),
    FapUASimModelListView(ui),
    FapUARDBListView(ui)
{
  this->hasApplIndependentSelection = false;
}
//----------------------------------------------------------------------------

void FapUASimModelRDBListView::updateSession()
{
  this->usedOGBaseIDs.clear();

#ifdef LV_DEBUG
  clock_t start = clock();
#endif

  this->FapUAItemsListView::updateSession();

  if (this->extractor && !this->freezeTopLevelItem)
  {
    std::vector<FFaListViewItem*> items, verified;
    items.reserve(this->childrenVecCap);
    verified.reserve(this->childrenVecCap);

    // Get the super object groups containing unused object groups
    this->extractor->getSuperObjectGroups(items,this->usedOGBaseIDs);
    for (FFaListViewItem* item : items)
      if (this->verifyItem(item))
        verified.push_back(item);
    if (verified.empty()) return;

    std::vector<int> tls = this->ui->getChildren(-1);
    FFaListViewItem* lastTopLevel = tls.empty() ? NULL : this->getMapLVItem(tls.back());

    this->createUIItem(verified.front(),NULL,lastTopLevel);
    for (size_t i = 1; i < verified.size(); i++)
      this->createUIItem(verified[i],NULL,verified[i-1]);
  }

#ifdef LV_DEBUG
  clock_t stop = clock();
  double usedTime = (stop-start)/((double)CLOCKS_PER_SEC);
  std::cout << this->ui->getName()
	    <<" - FapUASimModelRDBListView::updateSession() used time: "<< usedTime
	    <<" nItems:"<< this->ui->getAllChildren(-1,true).size() << std::endl;
#endif
}
//----------------------------------------------------------------------------

bool FapUASimModelRDBListView::verifyItem(FFaListViewItem* item)
{
  if (dynamic_cast<FFrEntryBase*>(item))
    return FapUARDBListView::verifyItem(item);
  else if (dynamic_cast<FmMechanism*>(item))
    return true;
  else if (dynamic_cast<FmStructPropertyBase*>(item))
    return false;
  else if (dynamic_cast<FmSeaState*>(item))
    return false;
  else if (dynamic_cast<FmModelMemberBase*>(item))
    return FapUASimModelListView::verifyItem(item);
  else
    return false;
}
//----------------------------------------------------------------------------

void FapUASimModelRDBListView::getChildren(FFaListViewItem* parent,
					   std::vector<FFaListViewItem*>& children) const
{
  if (!parent) {
    // Top headers, get top-level variables
    if (this->extractor) {
      this->extractor->getTopLevelVars(children);
#ifdef LV_DEBUG
      std::cout <<"\nFapUASimModelRDBListView::getChildren() Top level vars from Extractor: "<< children.size();
      for (FFaListViewItem* child : children)
        std::cout <<"\n"<< child->getItemName() <<" ["<< child->getItemID()
                  <<"]{"<< child->getItemBaseID() <<"}";
      std::cout << std::endl;
#endif
    }
#ifdef LV_DEBUG
    size_t nTV = children.size();
#endif
    children.reserve(children.size()+this->childrenVecCap);
    this->FapUASimModelListView::getChildren(NULL,children);
#ifdef LV_DEBUG
    std::cout <<"FapUASimModelRDBListView::getChildren() Top level vars in model: "<< children.size()-nTV;
    for (size_t i = nTV; i < children.size(); i++)
      std::cout <<"\n"<< children[i]->getItemName() <<" ["<< children[i]->getItemID()
                <<"]{"<< children[i]->getItemBaseID() <<"}";
    std::cout << std::endl;
#endif
    return;
  }

  // Check for model member parent
  FmModelMemberBase* mmbparent = dynamic_cast<FmModelMemberBase*>(parent);
  if (mmbparent) {
#ifdef LV_DEBUG
    std::cout <<"FapUASimModelRDBListView::getChildren() Model member "<< mmbparent->getInfoString() << std::endl;
#endif
    if (mmbparent->isOfType(FmRingStart::getClassTypeID()) ||
        mmbparent->isOfType(FmSubAssembly::getClassTypeID()))
      // Model member parent is a header
      this->FapUASimModelListView::getChildren(parent,children);

    if (mmbparent->isOfType(FmSimulationModelBase::getClassTypeID()) ||
        mmbparent->isOfType(FmSubAssembly::getClassTypeID())) {
#ifdef LV_DEBUG
      std::cout <<"FapUASimModelRDBListView::getChildren() Simulation object {"<< mmbparent->getBaseID() <<"}";
      size_t nC = children.size();
#endif
      // Find result items for current model member parent
      if (this->extractor && !this->extractor->empty())
        if (this->extractor->getObjectGroupFields(mmbparent->getBaseID(),
                                                  mmbparent->getItemName(),
                                                  children)) {
          this->usedOGBaseIDs.insert(mmbparent->getBaseID());
#ifdef LV_DEBUG
          std::cout <<" with "<< children.size()-nC <<" result items";
#endif
        }
#ifdef LV_DEBUG
      std::cout << std::endl;
#endif
    }
    return;
  }

  // Check for result database item parent
  FFrFieldEntryBase* ffrparent = dynamic_cast<FFrFieldEntryBase*>(parent);
  if (ffrparent) {
    if (ffrparent->isSOG()) {
      // Superobject group parent -> not used OG's,
      for (FFrEntryBase* og : ffrparent->dataFields)
        if (this->usedOGBaseIDs.find(og->getBaseID()) == this->usedOGBaseIDs.end())
          children.push_back(og);
    }
    else for (FFrEntryBase* field : ffrparent->dataFields)
      children.push_back(field);
    return;
  }

#ifdef LV_DEBUG
  std::cout <<"FapUASimModelRDBListView::getChildren(): Ignoring "
            << parent->getItemName() <<" ["<< parent->getItemID() <<"]{"
            << parent->getItemBaseID() <<"}"<< std::endl;
#endif
}
//----------------------------------------------------------------------------

std::string FapUASimModelRDBListView::getItemText(FFaListViewItem* item)
{
  if (dynamic_cast<FFrEntryBase*>(item))
    return FapUARDBListView::getItemText(item);
  else if (dynamic_cast<FmModelMemberBase*>(item))
    return FapUASimModelListView::getItemText(item);

  return "";
}
//----------------------------------------------------------------------------

void FapUASimModelRDBListView::permTotSelectItems(std::vector<int>& totalSelection)
{
  // Make sure that result items are not selected in the event manager
  // and that an empty selection is passed when such items are hit.
  // Also filter out mechanism from the selection, since verifyItem does not
  // filter out that entry (because solver results are within Mechanism OG).
  std::vector<FFaViewItem*> totalItems = this->convertItems(totalSelection);
  for (FFaViewItem* item : totalItems)
    if (dynamic_cast<FFrEntryBase*>(item) || dynamic_cast<FmMechanism*>(item)) {
      this->permUnselectAll();
      return;
    }

  this->FapUASimModelListView::permTotSelectItems(totalSelection);
}
