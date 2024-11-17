// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAModMemListView.H"
#include "vpmUI/vpmUIComponents/FuiItemsListView.H"
#include "vpmDB/FmModelMemberBase.H"
#include "FFaLib/FFaDefinitions/FFaListViewItem.H"


//----------------------------------------------------------------------------

FapUAModMemListView::FapUAModMemListView(FuiItemsListView* ui)
  : FapUAItemsListView(ui), signalConnector(this)
{
  this->showModelPermSelectionAsTopLevelItem = false;
  this->selectionInMyself = false;
}
//----------------------------------------------------------------------------

void FapUAModMemListView::onModelMemberConnected(FmModelMemberBase* item)
{
  this->onListViewItemConnected(item);
}

void FapUAModMemListView::onModelMemberDisconnected(FmModelMemberBase* item)
{
  this->onListViewItemDisconnected(item);
}

void FapUAModMemListView::onModelMemberChanged(FmModelMemberBase* item)
{
  this->onListViewItemChanged(item);
}
//----------------------------------------------------------------------------

void FapUAModMemListView::permTotSelectItems(std::vector<int>& totalSelection)
{
  this->selectionInMyself = true;
  FapUAItemsViewHandler::permTotSelectItems(totalSelection);
  this->selectionInMyself = false;
}
//----------------------------------------------------------------------------

void FapUAModMemListView::onPermTotSelectionChanged(const std::vector<FFaViewItem*>& totalSelection)
{
  if (this->showModelPermSelectionAsTopLevelItem && this->isUIPoppedUp())
    if (!this->selectionInMyself) {
      if (totalSelection.size() != 1)
	this->setTopLevelItem(NULL);
      else if (dynamic_cast<FmModelMemberBase*>(totalSelection.front())) {
	this->setTopLevelItem((FFaListViewItem*)totalSelection.front(),true);
	this->ui->permSelectItem(this->getMapItem(totalSelection.front()));
      }
    }
}
//----------------------------------------------------------------------------

FapUAModMemListView::SignalConnector::SignalConnector(FapUAModMemListView* ua) : owner(ua)
{
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_CONNECTED,
			  FFaSlot1M(SignalConnector,this,onModelMemberConnected,FmModelMemberBase*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_DISCONNECTED,
			  FFaSlot1M(SignalConnector,this,onModelMemberDisconnected,FmModelMemberBase*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_CHANGED,
			  FFaSlot1M(SignalConnector,this,onModelMemberChanged,FmModelMemberBase*));
}
//----------------------------------------------------------------------------

FapUAModMemListView::SignalConnector::~SignalConnector()
{
  FFaSwitchBoard::removeAllOwnerConnections(this);
}
//----------------------------------------------------------------------------
