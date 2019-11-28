// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_CURVE_DEFINE_H
#define FAP_UA_CURVE_DEFINE_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUADataHandler.H"
#include "vpmApp/vpmAppDisplay/FapGraphDataMap.H"
#include "vpmApp/FapEventManager.H"

class FapUACurveAxisDefinition;
class FuaCurveDefineValues;
class FuiCurveDefine;
class FmCurveSet;


class FapUACurveDefine : public FapUAExistenceHandler, public FapUADataHandler
{
  Fmd_HEADER_INIT();

public:
  FapUACurveDefine(FuiCurveDefine* ui);
  virtual ~FapUACurveDefine() {}

  // multi-event and preview curve cache clearing
  static void clearCachedDBCurve(FmCurveSet* curve = NULL);

private:
  void resetCachedDBCurve(FmCurveSet* curve);

  // slots from EventManager
  void onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
			      const std::vector<FFaViewItem*>& selectedSinceLast,
			      const std::vector<FFaViewItem*>& unselectedSinceLast);
  void onPermSelectionStackChanged(bool pushed);

  // from datahandler
  virtual FFuaUIValues* createValuesObject();
  virtual void updateUIValues();
  virtual void setDBValues(FFuaUIValues* values);
  virtual void getDBValues(FFuaUIValues* values);

  static void setMultiselectionDBValues(FmCurveSet* curve,
					FuaCurveDefineValues* curveValues);

  // from ui
  void setAutoLegend(bool autoLegend);
  void getChannelList(const std::string& fileName);
  void reloadCurve();

  void fetchStatistics(bool scaled, bool entire, double start, double stop);
  void applyVerticalMarkers(double min, double max);
  void removeVerticalMarkers();

  void fetchCurveDamage(bool weighted, bool entire, double start, double stop);

  // from axis ua
  void onAxisCompleted();
  void onEditXAxis();
  void onEditYAxis();

  // from signal connector
  void onModelMemberChanged(FmModelMemberBase* curve);

  // verification call-backs
  void verifySpatialCurveCB(bool& isOK, FmModelMemberBase* item);
  void verifyTemporalCurveCB(bool& isOK, FmModelMemberBase* item);

private:
  FuiCurveDefine* ui;
  FmCurveSet* dbcurve;
  FmCurveSet* cachedDBCurve;
  std::vector<FmCurveSet*> mySelectedCurves;
  FapUACurveAxisDefinition* axesUA[2];

  FapGraphDataMap curveData; // cached data for statistics and damage calculation

  bool wait;
  bool ignoreEverythingButCurves;
  bool IAmEditingCurveAxis;

  FapPermSelChangedReceiver<FapUACurveDefine> selSignalConnector;
  friend class FapPermSelChangedReceiver<FapUACurveDefine>;

  FapPermSelStackChangedReceiver<FapUACurveDefine> stackSignalConnector;
  friend class FapPermSelStackChangedReceiver<FapUACurveDefine>;

  // Signal receiver
  class SignalConnector : public FFaSwitchBoardConnector
  {
  public:
    SignalConnector(FapUACurveDefine* ua);
    virtual ~SignalConnector();

  private:
    void onModelMemberChanged(FmModelMemberBase* item)
    { this->owner->onModelMemberChanged(item); }

    FapUACurveDefine* owner;
  };

  SignalConnector signalConnector;
};

#endif
