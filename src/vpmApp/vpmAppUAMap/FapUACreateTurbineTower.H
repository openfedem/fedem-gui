// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_CREATE_TURBINE_TOWER_H
#define FAP_UA_CREATE_TURBINE_TOWER_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUADataHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAFinishHandler.H"
#include "FFaLib/FFaDynCalls/FFaSwitchBoard.H"

class FuiCreateTurbineTower;
class FmModelMemberBase;


class FapUACreateTurbineTower : public FapUAExistenceHandler,
                                public FapUADataHandler,
                                public FapUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FapUACreateTurbineTower(FuiCreateTurbineTower* ui);
  virtual ~FapUACreateTurbineTower() {}

private:
  void onMaterialSelected(FmModelMemberBase*);

  // from datahandler
  virtual FFuaUIValues* createValuesObject();
  virtual void setDBValues(FFuaUIValues* values);
  virtual void getDBValues(FFuaUIValues* values);

  // from finishhandler
  virtual void finishUI();

  FuiCreateTurbineTower* ui;

  // Signal Receiver
  class SignalConnector : public FFaSwitchBoardConnector
  {
  public:
    SignalConnector(FapUACreateTurbineTower* ua);
    virtual ~SignalConnector();

  private:
    void onModelMemberDisconnected(FmModelMemberBase* item);

    FapUACreateTurbineTower* owner;
  };

  SignalConnector signalConnector;
};

#endif
