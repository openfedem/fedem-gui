// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_RDB_SELECTOR_H
#define FAP_UA_RDB_SELECTOR_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAFinishHandler.H"
#include "FFaLib/FFaDynCalls/FFaSwitchBoard.H"

class FuiRDBSelector;
class FapUASimModelRDBListView;
class FapUARDBListView;
class FapUAItemsViewHandler;
class FFrExtractor;
class FFaResultDescription;
class FmCurveSet;
class FmModelMemberBase;


class FapUARDBSelector : public FapUAExistenceHandler, public FapUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FapUARDBSelector(FuiRDBSelector* ui);
  virtual ~FapUARDBSelector() {}

  void edit(FmCurveSet* curve, int axis);
  FFaResultDescription getSelectedResultDescr() const;
  int getCurrentAxis() const { return myAxis; }

private:
  // from FapUA...ListView
  void onRDBItemSelected(FapUAItemsViewHandler* lv);
  void onPosItemSelected(FapUAItemsViewHandler* lv);

  // from FuiRDBSelector
  void onResultApplied();
  void onAppearance(bool popup);

  // from FapUAFinishHandler
  virtual void finishUI();

  // slots from FpRDBExtractorManager
  void onModelExtrDeleted(FFrExtractor* extr);
  void onModelExtrHeaderChanged(FFrExtractor* extr);
  void onPosExtrDeleted(FFrExtractor* extr);
  void onPosExtrHeaderChanged(FFrExtractor* extr);
  void doNewModelExtr(FFrExtractor* extr);
  void doNewPosExtr(FFrExtractor* extr);

  // slot from FmModelMemberBase
  void onModelMemberChanged(FmModelMemberBase* item);

  // miscellaneous
  void updateApplyable();
  void setAxisText();
  void selectResult(const FFaResultDescription& result);

private:
  FuiRDBSelector* ui;

  bool        IAmEditingCurve;
  FmCurveSet* myCurve;
  int         myAxis;

  FapUASimModelRDBListView* resUA;
  FapUARDBListView*         posUA;

private:
  class SignalConnector : public FFaSwitchBoardConnector
  {
  public:
    SignalConnector(FapUARDBSelector* ua);
    virtual ~SignalConnector();

  private:
    void onModelExtrDeleted(FFrExtractor* extr)
      {this->owner->onModelExtrDeleted(extr);}
    void onModelExtrHeaderChanged(FFrExtractor* extr)
      {this->owner->onModelExtrHeaderChanged(extr);}
    void onPosExtrDeleted(FFrExtractor* extr)
      {this->owner->onPosExtrDeleted(extr);}
    void onPosExtrHeaderChanged(FFrExtractor* extr)
      {this->owner->onPosExtrHeaderChanged(extr);}
    void onModelMemberChanged(FmModelMemberBase* item)
      {this->owner->onModelMemberChanged(item);}

    FapUARDBSelector* owner;
  };

private:
  SignalConnector signalConnector;
};

#endif
