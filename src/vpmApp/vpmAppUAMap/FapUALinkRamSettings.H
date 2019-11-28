// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_LINK_RAM_SETTINGS_H
#define FAP_UA_LINK_RAM_SETTINGS_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUADataHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAFinishHandler.H"
#include "FFaLib/FFaDynCalls/FFaSwitchBoard.H"
#include "vpmDB/FmPart.H"

class FuiLinkRamSettings;


class FapUALinkRamSettings : public FapUAExistenceHandler,
                             public FapUADataHandler,
                             public FapUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FapUALinkRamSettings(FuiLinkRamSettings* ui);
  virtual ~FapUALinkRamSettings() {}

  static void changeRamUsageLevel(FmPart* part, const FmPart::RamLevel& level);

private:
  // from datahandler
  virtual FFuaUIValues* createValuesObject();
  virtual void setDBValues(FFuaUIValues* values);
  virtual void getDBValues(FFuaUIValues* values);

  // from finishhandler
  virtual void finishUI();

  void onModelMemberChanged(FmModelMemberBase* item);
  void onModelMemberConnected(FmModelMemberBase* item);
  void onModelMemberDisconnected(FmModelMemberBase* item);

  class SignalConnector : public FFaSwitchBoardConnector
  {
  public:
    SignalConnector(FapUALinkRamSettings* lrs);
    virtual ~SignalConnector();

  private:
    void onModelMemberChanged(FmModelMemberBase* item)
    { owner->onModelMemberChanged(item); }
    void onModelMemberConnected(FmModelMemberBase* item)
    { owner->onModelMemberConnected(item); }
    void onModelMemberDisconnected(FmModelMemberBase* item)
    { owner->onModelMemberDisconnected(item); }

    FapUALinkRamSettings* owner;
  };

  SignalConnector signalConnector;

  FuiLinkRamSettings* ui;
};

#endif
