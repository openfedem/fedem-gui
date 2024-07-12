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
  FuiGraphView() {}
  virtual ~FuiGraphView() {}

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
  virtual ~FuiGraphViewTLS() {}

  static FuiGraphView* create(FFuComponentBase* parent,
                              int xpos=100,int ypos=100,
                              int width=100,int height=100,
                              const char* title= "Fedem Graph",
                              const char* name = "FuiGraphViewTLS");

  FuiGraphView* getGraphViewComp() { return this->graphView; }

  virtual bool onClose();

protected:
  virtual void placeWidgets(int width, int height);
  virtual void onPoppedUp() { ++numInstance; }

private:
  static short int numInstance;

protected:
  FuiGraphView* graphView;
};


class FuaGraphViewValues : public FFuaUIValues
{
public:
  FuaGraphViewValues()
  {
    this->fontSize = {10,8,8};
    this->showLegend = false;
    this->gridtype = -1;
    this->autoScaleOnLoadCurve = false;
    this->xAxisMin = this->xAxisMax = 0.0;
    this->yAxisMin = this->yAxisMax = 0.0;
  }

  virtual ~FuaGraphViewValues() {}

  struct CurveData
  {
    CurveData()
    {
      this->id = -1;
      this->color = {0,0,0};
      this->style = this->width = -1;
      this->symbols = this->symbolssize = this->numSymbols = -1;
    }

    int id;
    std::string legend;
    UColor color;
    int style;
    int width;
    int symbols;
    int symbolssize;
    int numSymbols;
  };

  std::vector<CurveData> curves;

  std::string title, subtitle;
  std::string xAxisLabel, yAxisLabel;
  std::array<int,3> fontSize;

  bool showLegend;

  int gridtype; // see FFu2DPlotter enum

  bool autoScaleOnLoadCurve;
  double xAxisMin, xAxisMax;
  double yAxisMin, yAxisMax;
};

#endif
