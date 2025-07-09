// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_GRAPH_VIEW_H
#define FUI_GRAPH_VIEW_H

#include "FFuLib/FFu2DPlotter.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUAItemsViewHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"
#include "FFuLib/FFuBase/FFuMDIWindow.H"


class FuiGraphView : virtual public FFu2DPlotter,
                     public FFuUAExistenceHandler,
                     public FFuUAItemsViewHandler,
                     public FFuUADataHandler
{
public:
  //see FFu2DPlotter for format
  bool saveAsImage(const std::string& fileName, const char* format)
  {
    return this->savePlotterAsImage(fileName,format);
  }

  //zoom & shift
  void zoomAll();
  void zoomIn() {this->zoomPlotterIn();}
  void zoomOut() {this->zoomPlotterOut();}
  void zoomWindow() {this->zoomPlotterWindow();}
  void zoomAllInWindow() {this->zoomPlotterAllInWindow();}
  void shiftLeft() {this->shiftPlotterLeft();}
  void shiftRight() {this->shiftPlotterRight();}
  void shiftUp() {this->shiftPlotterUp();}
  void shiftDown() {this->shiftPlotterDown();}

  // from FFuUAItemsViewHandler
  virtual void deleteItem(int item);
  virtual void deleteItems();

private:
  virtual void permTotSelectItems(const std::vector<int>& totalSelection);

  // from FFuUADataHandler
  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);
  virtual void clearUIValues();

  // inside callback functions from FFu2DPlotter
  virtual void onCurveHighlightChanged();
  virtual void onAxisScaleChanged();	

protected:
  void initWidgets();

  // These methods are called every time a view pops up/down, also
  // from an iconified position
  virtual void onPoppedUp();
  virtual void onPoppedDown() {}

  // to/from memory only
  // called every time a ui appears on the screen (not from icon)
  virtual void onPoppedUpFromMem() { this->onPoppedUpFromMemory(); }
  // called when a ui is on its way to the memory (hide)
  virtual void onPoppedDownToMem() { this->onPoppedDownToMemory(); }
};


class FuiGraphViewTLS : virtual public FFuMDIWindow,
                        public FFuUAExistenceHandler,
                        public FFuUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FuiGraphViewTLS();

  static FuiGraphView* create(FFuComponentBase* parent,
                              int xpos=100,int ypos=100,
                              int width=100,int height=100,
                              const char* title= "Fedem Graph",
                              const char* name = "FuiGraphViewTLS");

  FuiGraphView* getGraphViewComp() { return myView; }

  virtual bool onClose();

protected:
  virtual void onPoppedUp() { ++numInstance; }

private:
  static short int numInstance;

protected:
  FuiGraphView* myView;
};


class FuaGraphViewValues : public FFuaUIValues
{
public:
  struct CurveData
  {
    int id = -1;
    std::string legend;
    std::array<int,3> color = {0,0,0};
    int style = -1;
    int width = -1;
    int symbols = -1;
    int symbolssize = -1;
    int numSymbols = -1;
  };

  std::vector<CurveData> curves;

  std::string title, subtitle;
  std::string xAxisLabel, yAxisLabel;

  std::array<int,3> fontSize = {10,8,8};

  bool showLegend = false;
  bool autoScaleOnLoadCurve = false;

  int gridtype = -1; // see FFu2DPlotter enum

  double xAxisMin = 0.0;
  double xAxisMax = 0.0;
  double yAxisMin = 0.0;
  double yAxisMax = 0.0;
};

#endif
