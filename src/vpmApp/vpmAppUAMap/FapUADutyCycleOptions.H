// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_DUTY_CYCLE_OPTIONS_H
#define FAP_UA_DUTY_CYCLE_OPTIONS_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUADataHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAFinishHandler.H"
#include "FFaLib/FFaDynCalls/FFaSwitchBoard.H"
#include <vector>
#include <string>

class FuiDutyCycleOptions;
class FmModelMemberBase;


class FapUADutyCycleOptions : public FapUAExistenceHandler,
			      public FapUADataHandler,
			      public FapUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FapUADutyCycleOptions(FuiDutyCycleOptions* ui);
  virtual ~FapUADutyCycleOptions() {}

private:
  // from datahandler
  virtual FFuaUIValues* createValuesObject();
  virtual void setDBValues(FFuaUIValues* values);
  virtual void getDBValues(FFuaUIValues* values);

  // from finishhandler
  virtual void finishUI();

  // callbacks
  void addEvent();

  void onModelMemberChanged(FmModelMemberBase* item);
  void onModelMemberConnected(FmModelMemberBase* item);
  void onModelMemberDisconnected(FmModelMemberBase* item);

  std::vector< std::pair<std::string,bool> > getUILinks();

private:
  std::vector<FmModelMemberBase*> allLinks;

  FuiDutyCycleOptions* ui;

  // Signal receiver
  class SignalConnector : public FFaSwitchBoardConnector
  {
  public:
    SignalConnector(FapUADutyCycleOptions* ua);
    virtual ~SignalConnector();

  private:
    void onModelMemberChanged(FmModelMemberBase* item)
    {this->owner->onModelMemberChanged(item);}
    void onModelMemberConnected(FmModelMemberBase* item)
    {this->owner->onModelMemberConnected(item);}
    void onModelMemberDisconnected(FmModelMemberBase* item)
    {this->owner->onModelMemberDisconnected(item);}

    FapUADutyCycleOptions* owner;
  };

  SignalConnector signalConnector;
};

#endif
