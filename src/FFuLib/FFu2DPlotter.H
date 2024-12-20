// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_2D_PLOTTER_H
#define FFU_2D_PLOTTER_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <string>
#include <vector>
#include <array>

typedef std::array<int,3> UColor;


class FFu2DPlotter : virtual public FFuComponentBase
{
public:
  FFu2DPlotter() { autoScaleOnLoadCurve = true; d_shiftFactor = 0.1; }
  virtual ~FFu2DPlotter() {}


  // =========================== GENERAL ===============================
  // -------------------------------------------------------------------
  virtual void setPlotterTitle(const std::string& title, int fontSize) = 0;
  virtual void setPlotterSubTitle(const std::string& subtitle) = 0;
  virtual std::string getPlotterTitle() = 0;
  virtual std::string getPlotterSubTitle() = 0;


  // ============================ CLEAR ================================
  // -------------------------------------------------------------------

  // Removes titles, axestitles and legends
  virtual void clearPlotterText() = 0;


  // ============================= AXES ===============================
  // -------------------------------------------------------------------

  enum {X_AXIS, Y_AXIS, N_AXES};
  // These methods return false if the axis is invalid.

  // Fixes scale of specified axis
  virtual bool setPlotterAxisScale(int axis, double min, double max,
                                   bool notify = false) = 0;
  virtual bool setPlotterAxisTitle(int axis, const std::string& title, int fontSize) = 0;

  virtual std::string getPlotterAxisTitle(int axis) = 0;

  virtual double getPlotterXAxisMax() = 0;
  virtual double getPlotterXAxisMin() = 0;
  virtual double getPlotterYAxisMax() = 0;
  virtual double getPlotterYAxisMin() = 0;


  // =========================== CURVES ===============================
  // -------------------------------------------------------------------

  virtual int  loadNewPlotterCurve(std::vector<double>* const x,
				   std::vector<double>* const y,
				   const UColor& color = {0,0,0},
				   int style=LINES, int width=0,
				   int symbol=NONE, int symSize=7, int numSym=0,
				   const std::string& legend="",
				   double scaleX=1.0, double offsetX=0.0,
				   bool zeroAdjustX=false,
				   double scaleY=1.0, double offsetY=0.0,
				   bool zeroAdjustY=false) = 0;

  // set new data to an existing curve
  virtual bool loadPlotterCurveData(int curveid,
				    std::vector<double>* const x,
				    std::vector<double>* const y,
				    bool append=false,
				    const UColor& color = {0,0,0},
				    int style=LINES, int width=0,
				    int symbol=NONE, int symSize=7, int numSym=0,
				    const std::string& legend="",
				    double scaleX=1.0, double offsetX=0.0,
				    bool zeroAdjustX=false,
				    double scaleY=1.0, double offsetY=0.0,
				    bool zeroAdjustY=false) = 0;

  // Removes one curve
  virtual void removePlotterCurve(int curveid) = 0;
  // Removes all curves
  virtual void removePlotterCurves() = 0;
  // Replots curves with current pointArray and pen
  virtual void replotAllPlotterCurves() = 0;
  virtual std::vector<int> getPlotterCurves() = 0;

  enum {LINES, DOTS, NO_CURVE};

  // Draws lines, dots or no curve at all (default is LINES).
  // LINES connects points with line segments.
  // Dots draws dots at the points.
  // Width is a number in range 0...4
  // Uses RGB-values to set colors.
  // Each value in the UColor must be in the range 0...1
  virtual void   setPlotterCurveStyle(int curveid, int style, int width,
                                      const UColor& color, bool redraw=true) = 0;
  virtual int    getPlotterCurveStyle(int curveid) = 0;
  virtual int    getPlotterCurveWidth(int curveid) = 0;
  virtual UColor getPlotterCurveColor(int curveid) = 0;

  enum {NONE, PLUS_CROSS, X_CROSS, CIRCLE, DIAMOND, RECT,
	U_TRIANGLE, D_TRIANGLE, L_TRIANGLE, R_TRIANGLE};

  // Draws the selected symbol in each point (default is NONE)
  virtual void   setPlotterCurveSymbol(int curveid, int symbol,
				       int size=7, int num=10) = 0;
  virtual int    getPlotterCurveSymbol(int curveid) = 0;
  virtual int    getPlotterCurveSymbolSize(int curveid) = 0;
  virtual int    getPlotterCurveNumSymbols(int curveid) = 0;



  // ============================ GRID =================================
  // -------------------------------------------------------------------

  enum {NOGRID, COARSEGRID, FINEGRID};

  virtual void showPlotterGridX(int gridtype) = 0;
  virtual void showPlotterGridY(int gridtype) = 0;

  virtual int  getPlotterGridTypeX() = 0; // svenev: Needs testing
  virtual int  getPlotterGridTypeY() = 0; // but should work...

  virtual void setPlotterGridMajPen(const std::string& color="black",
				    unsigned int width=2) = 0;
  // Width 0 equals one pixel
  virtual void setPlotterGridMinPen(const std::string& color="black",
				    unsigned int width=0) = 0;


  // =========================== LEGEND ================================
  // -------------------------------------------------------------------

  virtual void   showPlotterLegend(bool show) = 0;
  virtual bool   getPlotterLegendEnabled() = 0;

  enum {LEFT = 1, RIGHT = 2, TOP = 4, BOTTOM = 8, CENTER = 16};

  // Sets a lengend label  NB: same as the curve's name!
  virtual void   setPlotterLegendLabel(int curveid,const std::string& label) = 0;
  virtual std::string getPlotterLegendLabel(int curveid) = 0;


  // =========================== MARKER ================================
  // -------------------------------------------------------------------

  // A linemarker draws a vertical or horizontal line in the plottingarea
  virtual int    insertPlotterLineMarker(const std::string& label, int axis, double pos) = 0;
  virtual bool   setPlotterMarkerPos(int id, double x, double y) = 0;

  virtual bool   setPlotterMarkerXPos(int id, double x) = 0;
  virtual bool   setPlotterMarkerYPos(int id, double y) = 0;
  // Remove one marker
  virtual bool   removePlotterMarker(int id) = 0;
  // Remove all markers
  virtual void   removePlotterMarkers() = 0;


  // ============================= ZOOM ================================
  // -------------------------------------------------------------------

  void setAutoScaleOnLoadCurve(bool a) { autoScaleOnLoadCurve = a; }
  bool getAutoScaleOnLoadCurve() const { return autoScaleOnLoadCurve; }

  // Scales axes so that all points are visible
  virtual void   autoScalePlotter() = 0;

  // Zooms in/out by a certain factor
  virtual void   zoomPlotterIn() = 0;
  virtual void   zoomPlotterOut() = 0;

  // Enables windowzoom
  virtual void   zoomPlotterWindow() = 0;
  virtual void   zoomPlotterAllInWindow() = 0;

  // Dynamic viewing is enabled by default in QwtPlot
  virtual void   enablePlotterDynamicView(bool yesOrNo) = 0;
  virtual bool   getPlotterDynamicViewEnabled() = 0;


  // ============================= SHIFT ===============================
  // -------------------------------------------------------------------

  // Shifts up/down/left/right by a certain factor
  virtual void   shiftPlotterLeft() = 0;
  virtual void   shiftPlotterRight() = 0;
  virtual void   shiftPlotterUp() = 0;
  virtual void   shiftPlotterDown() = 0;
  // Should probably be around 0.1 - 0.3
  // No restrictions apply. Default is 0.07
  void   setPlotterShiftFactor(double f) { d_shiftFactor = f; }
  double getPlotterShiftFactor() const { return d_shiftFactor; }


  // =========================== HIGHLIGHT =============================
  // -------------------------------------------------------------------

  // Enables highlighting of curves. Is active until turned off.
  virtual void   enablePlotterPickCurve(bool yesOrNo) = 0;
  virtual void   enablePlotterHighlightCurveOnPick(bool yesOrNo) = 0;
  virtual bool   getPlotterPickCurve() = 0;
  virtual bool   getPlotterHighlightCurveOnPick() = 0;
  // Highlights or unhighlights the specified curve
  // Note that if you highlight a curve that is already highlighted,
  // nothing happens, and no signal is emitted
  virtual bool   isPlotterCurvehighlighted(int curveid) = 0;
  virtual void   highlightPlotterCurve(int curveid, bool highlight,
				       bool notify = false) = 0;
  // Highlights or unhighlights all curves
  virtual void   highlightAllPlotterCurves(bool highlight,
					   bool notify = false) = 0;
  virtual std::vector<int> getPlotterHighlightedCurves() = 0;

  // Sets the color with which highlighted curves are drawn
  // The values in the vector must be in [0,1]
  virtual void   setPlotterHighlightColor(const UColor& color) = 0;
  virtual UColor getPlotterHighlightColor() = 0;

  virtual void   setPlotterHighlightPenWidth(int width) = 0;
  virtual int    getPlotterHighlightPenWidth() = 0;


  // ============================= MISC ================================
  // -------------------------------------------------------------------
  // Available formats:
  // BMP PBM PGM PNG PPM XBM XPM - JPEG might be available
  // See https://doc.qt.io/qt-6.5/qpixmap.html#reading-and-writing-image-files
  virtual bool savePlotterAsImage(const std::string& fName, const char* format) = 0;

  virtual void setPlotterScaleAndOffset(int curveid, double scaleX, double offsetX,
					bool zeroAdjustX, double scaleY, double offsetY,
					bool zeroAdjustY, bool doUpdate=true) = 0;

  virtual void onCurveHighlightChanged() {}

  void setCurveHighlightChangedCB(const FFaDynCB0& aDynCB)
    {this->curveHighlightChangedCB = aDynCB;}
  void setAxisScaleChangedCB(const FFaDynCB0& aDynCB)
    {this->axisScaleChangedCB = aDynCB;}
  void setGraphSelectedCB(const FFaDynCB0& aDynCB)
    {this->graphSelectedCB = aDynCB;}

protected:
  void invokeCurvePickedCB(int curvid) { this->curvePickedCB.invoke(curvid); }
  void invokeCurveHighlightChangedCB() { this->curveHighlightChangedCB.invoke(); }
  void invokeAxisScaleChangedCB()      { this->axisScaleChangedCB.invoke(); }
  void invokeGraphSelectedCB()         { this->graphSelectedCB.invoke(); }

private:
  FFaDynCB1<int> curvePickedCB;
  FFaDynCB0 curveHighlightChangedCB;
  FFaDynCB0 axisScaleChangedCB;
  FFaDynCB0 graphSelectedCB;

protected:
  bool   autoScaleOnLoadCurve;
  double d_shiftFactor;
};

#endif
