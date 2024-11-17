// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_MODMEM_LISTVIEW_H
#define FAP_UA_MODMEM_LISTVIEW_H

#include "vpmApp/vpmAppUAMap/FapUAItemsListView.H"
#include "FFaLib/FFaDynCalls/FFaSwitchBoard.H"


class FapUAModMemListView : virtual public FapUAItemsListView
{
public:
  FapUAModMemListView(FuiItemsListView* ui);
  virtual ~FapUAModMemListView() {}

  void setShowModelPermSelectionAsTopLevelItem(bool show)
  { this->showModelPermSelectionAsTopLevelItem = show; }

private:
  // slots from db
  void onModelMemberConnected(FmModelMemberBase* item);
  void onModelMemberDisconnected(FmModelMemberBase* item);
  void onModelMemberChanged(FmModelMemberBase* item);

protected:
  // from FapUAItemsViewHandler
  virtual void onPermTotSelectionChanged(const std::vector<FFaViewItem*>& totalSelection);
  virtual void permTotSelectItems(std::vector<int>& totalSelection);

private:
  // Attributes
  bool showModelPermSelectionAsTopLevelItem;
  bool selectionInMyself;

  // Signal receiver
  class SignalConnector : public FFaSwitchBoardConnector
  {
  public:
    SignalConnector(FapUAModMemListView* ua);
    virtual ~SignalConnector();

  private:
    // Slots from db
    void onModelMemberConnected(FmModelMemberBase* item)
    {this->owner->onModelMemberConnected(item);}
    void onModelMemberDisconnected(FmModelMemberBase* item)
    {this->owner->onModelMemberDisconnected(item);}
    void onModelMemberChanged(FmModelMemberBase* item)
    {this->owner->onModelMemberChanged(item);}

    FapUAModMemListView* owner;
  };

  SignalConnector signalConnector;
};

#endif
