// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_GRAPH_VIEW_TLS_H
#define FAP_UA_GRAPH_VIEW_TLS_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAFinishHandler.H"
#include "vpmApp/FapEventManager.H"
#include "FFaLib/FFaDynCalls/FFaSwitchBoard.H"

class FuiGraphViewTLS;
class FmModelMemberBase;


class FapUAGraphViewTLS : public FapUAExistenceHandler, public FapUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FapUAGraphViewTLS(FuiGraphViewTLS* ui);
  virtual ~FapUAGraphViewTLS() {}

  FuiGraphViewTLS* getUI() { return this->ui; }

private:
  // from FapUAFinishHandler
  virtual void finishUI();

  // slots from DB
  void onModelMemberDisconnected(FmModelMemberBase* item);
  void onModelMemberChanged(FmModelMemberBase* item);

  void onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
			      const std::vector<FFaViewItem*>& selectedSinceLast,
			      const std::vector<FFaViewItem*>& unselectedSinceLast);

  FapPermSelChangedReceiver<FapUAGraphViewTLS> selSignalConnector;
  friend class FapPermSelChangedReceiver<FapUAGraphViewTLS>;

  class SignalConnector : public FFaSwitchBoardConnector
  {
  public:
    SignalConnector(FapUAGraphViewTLS* ua);
    virtual ~SignalConnector();

  private:
    // slots from DB
    void onModelMemberDisconnected(FmModelMemberBase* item)
    { this->owner->onModelMemberDisconnected(item); }
    void onModelMemberChanged(FmModelMemberBase* item)
    { this->owner->onModelMemberChanged(item); }

    FapUAGraphViewTLS* owner;
  };

  SignalConnector signalConnector;

  FuiGraphViewTLS* ui;
};

#endif
