// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_CREATE_TURBINE_ASSEMBLY_H
#define FAP_UA_CREATE_TURBINE_ASSEMBLY_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUADataHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAFinishHandler.H"
#include "FFaLib/FFaDynCalls/FFaSwitchBoard.H"

class FuiCreateTurbineAssembly;
class FmModelMemberBase;


class FapUACreateTurbineAssembly : public FapUAExistenceHandler,
                                   public FapUADataHandler,
                                   public FapUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FapUACreateTurbineAssembly(FuiCreateTurbineAssembly* ui);
  virtual ~FapUACreateTurbineAssembly() {}

private:
  // from datahandler
  virtual FFuaUIValues* createValuesObject();
  virtual void setDBValues(FFuaUIValues* values);
  virtual void getDBValues(FFuaUIValues* values);

  // from finishhandler
  virtual void finishUI();

  FuiCreateTurbineAssembly* ui;
  bool IAmErasingTurbine;

  // Signal Receiver
  class SignalConnector : public FFaSwitchBoardConnector
  {
  public:
    SignalConnector(FapUACreateTurbineAssembly* ua);
    virtual ~SignalConnector();

  private:
    void onModelMemberConnected(FmModelMemberBase* item);
    void onModelMemberDisconnected(FmModelMemberBase* item);

    FapUACreateTurbineAssembly* owner;
  };

  SignalConnector signalConnector;
};

#endif
