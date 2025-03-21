// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAItemsViewHandler.H"
#include "FFuLib/FFuBase/FFuUAItemsViewHandler.H"
#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDefinitions/FFaViewItem.H"
#include <iostream>


//----------------------------------------------------------------------------

FapUAItemsViewHandler::FapUAItemsViewHandler(FFuUAItemsViewHandler* uic)
  : permSelReceiver(this), tmpSelReceiver(this)
{
  this->ui = uic;
  this->updateOnChangesWhenInMem = false;
  this->hasApplIndependentSelection = false;
  this->ensureItemVisibleOnPermSelection = false;

  // Call-backs from UI
  this->ui->setSetUpdateOnChangesWhenInMemCB(FFaDynCB1M(FapUAItemsViewHandler,this,
							setUpdateOnChangesWhenInMem,bool));
  this->ui->setGetUpdateOnChangesWhenInMemCB(FFaDynCB1M(FapUAItemsViewHandler,this,
							getUpdateOnChangesWhenInMem,bool&));
  this->ui->setUpdateSessionCB(FFaDynCB0M(FapUAItemsViewHandler,this,
					  updateSession));
  this->ui->setClearSessionCB(FFaDynCB0M(FapUAItemsViewHandler,this,
					 clearSession));
  this->ui->setPermTotSelectItemsCB(FFaDynCB1M(FapUAItemsViewHandler,this,
					       permTotSelectItems,std::vector<int>&));
}
//----------------------------------------------------------------------------

void FapUAItemsViewHandler::clearSession()
{
  this->ui->deleteItems();
  this->clearMaps();
}
//----------------------------------------------------------------------------

std::vector<FFaViewItem*> FapUAItemsViewHandler::getUISelectedItems() const
{
  return this->convertItems(this->ui->getSelectedItems());
}
//----------------------------------------------------------------------------

bool FapUAItemsViewHandler::isItemUISelected(FFaViewItem* item) const
{
  return this->ui->isItemSelected(this->getMapItem(item));
}
//----------------------------------------------------------------------------

void FapUAItemsViewHandler::permTotSelectUIItems(const std::vector<FFaViewItem*>& totalSelection)
{
  // doesnt tolerate mult sel if ui is sgl sel
  if (totalSelection.size() > 1 && this->ui->isSglSelectionMode())
    this->ui->permTotSelectItems(std::vector<int>());
  else {
    std::vector<FFaViewItem*> selectables;
    for (FFaViewItem* sel : totalSelection)
      if (this->getItemSelectAble(sel))
        selectables.push_back(sel);

    this->ui->permTotSelectItems(this->convertItems(selectables));
  }
}
//----------------------------------------------------------------------------

void FapUAItemsViewHandler::deleteUIItem(FFaViewItem* item)
{
  int uiitem = this->getMapItem(item);
  if (uiitem < 0) return;

  this->ui->deleteItem(uiitem);
  this->eraseMapItem(uiitem);
}
//----------------------------------------------------------------------------

bool FapUAItemsViewHandler::shouldIUpdateOnChanges() const
{
  if (this->updateOnChangesWhenInMem)
    return true;
  else if (dynamic_cast<FFuComponentBase*>(this->ui)->isPoppedUp())
    return true;
  else
    return false;
}
//----------------------------------------------------------------------------

std::vector<FFaViewItem*> FapUAItemsViewHandler::convertItems(const std::vector<int>& in) const
{
  std::vector<FFaViewItem*> out;
  FFaViewItem* item;
  for (int i : in)
    if ((item = this->getMapItem(i)))
      out.push_back(item);
  return out;
}
//----------------------------------------------------------------------------

std::vector<int> FapUAItemsViewHandler::convertItems(const std::vector<FFaViewItem*>& in) const
{
  std::vector<int> out;
  int id;
  for (FFaViewItem* item : in)
    if ((id = this->getMapItem(item)) > -1)
      out.push_back(id);
  return out;
}
//----------------------------------------------------------------------------

void FapUAItemsViewHandler::permTotSelectItems(std::vector<int>& totalSelection)
{
  if (!hasApplIndependentSelection)
    FapEventManager::permTotalSelect(this->convertItems(totalSelection));

  permTotUISelectionChangedCB.invoke(this);
}
//----------------------------------------------------------------------------

void FapUAItemsViewHandler::permUnselectAll()
{
  if (!hasApplIndependentSelection)
    FapEventManager::permUnselectAll();

  permTotUISelectionChangedCB.invoke(this);
}
//----------------------------------------------------------------------------

void FapUAItemsViewHandler::onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
						   const std::vector<FFaViewItem*>&,
						   const std::vector<FFaViewItem*>&)
{
  if (!this->shouldIUpdateOnChanges()) return;
  if (this->hasApplIndependentSelection) return;

  this->permTotSelectUIItems(totalSelection);

  if (this->ensureItemVisibleOnPermSelection)
    for (FFaViewItem* item : this->getUISelectedItems())
      this->ensureItemVisible(item);

  this->onPermTotSelectionChanged(totalSelection);
  this->permSelectionChangedEvent();

  permTotUISelectionChangedCB.invoke(this);
}

void FapUAItemsViewHandler::onTmpSelectionChanged(FFaViewItem*, FFaViewItem*,
						  const std::vector<FFaViewItem*>&)
{
  this->tmpSelectionChangedEvent();
}
//----------------------------------------------------------------------------

int FapUAItemsViewHandler::getMapItem(FFaViewItem* item) const
{
  std::map<FFaViewItem*,int>::const_iterator it = this->ptrMap.find(item);

  return it == this->ptrMap.end() ? -1 : it->second;
}

FFaViewItem* FapUAItemsViewHandler::getMapItem(int item) const
{
  std::map<int,FFaViewItem*>::const_iterator it = this->intMap.find(item);

  return it == this->intMap.end() ? NULL : it->second;
}
//----------------------------------------------------------------------------

void FapUAItemsViewHandler::putMapItem(int intItem, FFaViewItem* ptrItem)
{
  if (this->getMapItem(intItem) || this->getMapItem(ptrItem) > -1)
    std::cerr <<"FapUAItemsViewHandler::putMapItem: Item is already there "
	      << ptrItem->getItemName() <<" "<< ptrItem->getItemID()
	      << std::endl;
  else if (intItem > -1 && ptrItem)
  {
    this->intMap[intItem] = ptrItem;
    this->ptrMap[ptrItem] = intItem;
  }
}
//----------------------------------------------------------------------------

void FapUAItemsViewHandler::eraseMapItem(int item)
{
  this->ptrMap.erase(this->getMapItem(item));
  this->intMap.erase(item);
}

void FapUAItemsViewHandler::eraseMapItem(FFaViewItem* item)
{
  this->intMap.erase(this->getMapItem(item));
  this->ptrMap.erase(item);
}
//----------------------------------------------------------------------------

void FapUAItemsViewHandler::clearMaps()
{
  this->intMap.clear();
  this->ptrMap.clear();
}
//----------------------------------------------------------------------------
