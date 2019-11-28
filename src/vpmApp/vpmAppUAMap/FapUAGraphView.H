// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_GRAPH_VIEW_H
#define FAP_UA_GRAPH_VIEW_H

#include <array>
#include <set>

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAItemsViewHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUADataHandler.H"
#include "vpmApp/vpmAppDisplay/FapGraphDataMap.H"
#include "FFaLib/FFaDynCalls/FFaSwitchBoard.H"

class FuiGraphView;
class FmGraph;
class FmCurveSet;
class FmModelMemberBase;
class FFrExtractor;


class FapUAGraphView : public FapUAExistenceHandler,
                       public FapUAItemsViewHandler,
                       public FapUADataHandler
{
  Fmd_HEADER_INIT();

public:
  FapUAGraphView(FuiGraphView* ui);
  virtual ~FapUAGraphView() { ourSelfSet.erase(this); }

  // Operations
  FmGraph* getDBPointer() { return dbgraph; }

  // from FapUAItemsViewHandler
  virtual void updateSession();
  virtual void clearSession();        // clear all curves in plotter
  virtual void clearSessionSpecial(); // clear curves with results from rdb

  static void insertVerticalMarkers(FmCurveSet* curve, double min, double max);
  static void removeVerticalMarkers(FmCurveSet* curve);

  // Animation marker interface used from FdAnimateModel
  // Distributes the calls to all the active graphs.
  static void initAnimationAllGraphs();
  static void setAnimationTimeAllGraphs(double time);
  static void resetAnimationAllGraphs();

  static FapGraphDataMap* getGraphDataMap(FmCurveSet* curve);
  static FapUAGraphView* getUAGraphView(FmCurveSet* curve);
  static FapUAGraphView* getUAGraphView(FmGraph* graph);

private:
  void insertVerticalMarkers(double min, double max);
  void removeVerticalMarkers();
  int myMinMarkerID;
  int myMaxMarkerID;

  // Animation marker interface
  void initAnimation();
  void setAnimationTime(double time);
  void resetAnimation();
  std::array<int,2> myAnimMarkerID;

  bool removeUICurve(FmCurveSet* curve);

  // from FapUADataHandler
  virtual FFuaUIValues* createValuesObject();
  virtual void setDBValues(FFuaUIValues* values);
  virtual void getDBValues(FFuaUIValues* values);

  // callbacks from plotter
  void onAxisScaleChanged();
  void onGraphSelected();

  // slots from db
  void onModelMemberConnected(FmModelMemberBase* item);
  void onModelMemberDisconnected(FmModelMemberBase* item);
  void onModelMemberChanged(FmModelMemberBase* item);

  // slots from rdb extr man
  void onNewModelExtr(FFrExtractor* extr);
  void onModelExtrDeleted(FFrExtractor* extr);
  void onModelExtrDataChanged(FFrExtractor* extr);
  void onModelExtrHeaderChanged(FFrExtractor* extr);

  // miscellaneous
  void loadCurveInViewer(FmCurveSet* curve);
  void loadCurvesInViewer(const std::vector<FmCurveSet*>& curves, bool append);
  void loadCurveDataInViewer(FmCurveSet* curve, bool append);

private:
  FuiGraphView*   ui;
  FmGraph*        dbgraph;
  FapGraphDataMap graphData;

  static std::set<FapUAGraphView*> ourSelfSet; // for animation time markers

  // Signal Receiver
  class SignalConnector : public FFaSwitchBoardConnector
  {
  public:
    SignalConnector(FapUAGraphView* ua);
    virtual ~SignalConnector();

  private:
    // Slots eventmanager
    // Slots from db
    void onModelMemberConnected(FmModelMemberBase* item)
    {
      owner->onModelMemberConnected(item);
    }
    void onModelMemberDisconnected(FmModelMemberBase* item)
    {
      owner->onModelMemberDisconnected(item);
    }
    void onModelMemberChanged(FmModelMemberBase* item)
    {
      owner->onModelMemberChanged(item);
    }

    // Slots from ffr
    // Slots from rdb extr man
    void onNewModelExtr(FFrExtractor* extr)
    {
      owner->onNewModelExtr(extr);
    }
    void onModelExtrDeleted(FFrExtractor* extr)
    {
      owner->onModelExtrDeleted(extr);
    }
    void onModelExtrDataChanged(FFrExtractor* extr)
    {
      owner->onModelExtrDataChanged(extr);
    }
    void onModelExtrHeaderChanged(FFrExtractor* extr)
    {
      owner->onModelExtrHeaderChanged(extr);
    }

    FapUAGraphView* owner;
  };

  SignalConnector signalConnector;
};

#endif
