// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_2D_PLOTTER_H
#define FFU_QT_2D_PLOTTER_H

#include <string>
#include <vector>
#include <map>

#include <QPen>
#include <QWidget>

#include "qwt_plot.h"

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "FFuLib/FFu2DPlotter.H"

class QWheelEvent;
class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotGrid;
class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;


class FFuQt2DPlotter : public QwtPlot, virtual public FFu2DPlotter, public FFuQtComponentBase
{
  Q_OBJECT

public:
  FFuQt2DPlotter(QWidget* parent, const char* name = "FFuQt2DPlotter" );
  virtual ~FFuQt2DPlotter() {}

  // texts
  virtual void
  setPlotterTitle( const std::string& title, int fontSize );
  virtual void
  setPlotterSubTitle( const std::string& subtitle );
  virtual std::string
  getPlotterTitle();
  virtual std::string
  getPlotterSubTitle();

  // clear
  virtual void
  clearPlotterText();

  // axes
  virtual bool
  setPlotterAxisScale( int axis, double min, double max, bool notify = false );
  virtual bool
  setPlotterAxisTitle( int axis, const std::string& title, int fontSize );
  virtual std::string
  getPlotterAxisTitle( int axis );
  virtual double
  getPlotterXAxisMax();
  virtual double
  getPlotterXAxisMin();
  virtual double
  getPlotterYAxisMax();
  virtual double
  getPlotterYAxisMin();

  // curves
  virtual int
  loadNewPlotterCurve(std::vector<double>* const x, std::vector<double>* const y,
      const UColor& color, int style = LINES, int width = 0,
      int symbol = NONE, int symSize = 7, int numSym = 0,
      const std::string& legend = "", double scaleX = 1.0,
      double offsetX = 0.0, bool zeroAdjustX = false, double scaleY = 1.0,
      double offsetY = 0.0, bool zeroAdjustY = false );

  virtual bool
  loadPlotterCurveData(int curveid, std::vector<double>* const x,
      std::vector<double>* const y, bool append,
      const UColor& color, int style = LINES, int width = 0,
      int symbol = NONE, int symSize = 7, int numSym = 0,
      const std::string& legend = "", double scaleX = 1.0,
      double offsetX = 0.0, bool zeroAdjustX = false, double scaleY = 1.0,
      double offsetY = 0.0, bool zeroAdjustY = false );

  virtual void
  setPlotterScaleAndOffset( int curveid, double scaleX, double offsetX,
      bool zeroAdjustX, double scaleY, double offsetY, bool zeroAdjustY,
      bool doUpdate = true );

  virtual void
  removePlotterCurve( int curveid );
  virtual void
  removePlotterCurves();
  virtual void
  replotAllPlotterCurves();
  virtual std::vector<int>
  getPlotterCurves();

  virtual void
  setPlotterCurveStyle( int curveid, int style, int width, const UColor& color, bool redraw = true);
  virtual int
  getPlotterCurveStyle( int curveid );
  virtual int
  getPlotterCurveWidth( int curveid );
  virtual UColor
  getPlotterCurveColor( int curveid );

  virtual void
  setPlotterCurveSymbol( int curveid, int symbol, int size = 6, int num = 10 );
  virtual int
  getPlotterCurveSymbol( int curveid );
  virtual int
  getPlotterCurveSymbolSize( int curveid );
  virtual int
  getPlotterCurveNumSymbols( int curveid );

  long closestCurveFaster(double xpos, double ypos, double distX, double distY) const;

  // grid
  virtual void
  showPlotterGridX( int gridtype );
  virtual void
  showPlotterGridY( int gridtype );
  virtual int
  getPlotterGridTypeX();
  virtual int
  getPlotterGridTypeY();
  virtual void
  setPlotterGridMajPen(const std::string& color = "black", unsigned int width = 2);
  virtual void
  setPlotterGridMinPen(const std::string& color = "black", unsigned int width = 0);

  // legend
  virtual void
  showPlotterLegend( bool show );
  virtual bool
  getPlotterLegendEnabled();
  virtual void
  setPlotterLegendLabel( int curveid, const std::string& label );
  virtual std::string
  getPlotterLegendLabel( int curveid );

  // markers
  virtual int
  insertPlotterLineMarker( const std::string& label, int axis, double pos );
  virtual bool
  setPlotterMarkerPos( int id, double x, double y );
  virtual bool
  setPlotterMarkerXPos( int id, double x );
  virtual bool
  setPlotterMarkerYPos( int id, double y );
  virtual bool
  removePlotterMarker( int id );
  virtual void
  removePlotterMarkers();

  // zoom & shift
  virtual void
  autoScalePlotter();
  virtual void
  zoomPlotterIn();
  virtual void
  zoomPlotterOut();
  virtual void
  zoomPlotterWindow();
  virtual void
  zoomPlotterAllInWindow();
  void
  zoomPlotter(double zoomFactor);

  virtual void
  shiftPlotterLeft();
  virtual void
  shiftPlotterRight();
  virtual void
  shiftPlotterUp();
  virtual void
  shiftPlotterDown();
  virtual void
  enablePlotterDynamicView( bool yesOrNo );
  virtual bool
  getPlotterDynamicViewEnabled();

  // highlight
  virtual void
  enablePlotterPickCurve( bool yesOrNo );
  virtual void
  enablePlotterHighlightCurveOnPick( bool yesOrNo );
  virtual bool
  getPlotterPickCurve();
  virtual bool
  getPlotterHighlightCurveOnPick();


  virtual bool
  isPlotterCurvehighlighted( int curveid );
  virtual void
  highlightPlotterCurve( int curveid, bool highlight, bool notify = false );
  virtual void
  highlightAllPlotterCurves( bool highlight, bool notify = false );
  virtual std::vector<int>
  getPlotterHighlightedCurves();

  virtual void
  setPlotterHighlightColor( const UColor& color );
  virtual UColor
  getPlotterHighlightColor();

  virtual void
  setPlotterHighlightPenWidth( int width );
  virtual int
  getPlotterHighlightPenWidth();

  virtual bool savePlotterAsImage(const std::string& fName, const char* format);

protected:
  virtual void wheelEvent(QWheelEvent* event) override;

private slots:
  void fwdCurveHighlightChanged();
  void fwdGraphSelected();
  void zoomComplete();
  void onCurvePicked(const QPointF& point);
  void panComplete(int dx, int dy);

private:
  QwtPlotCurve* GetCurveFromID(int curveID);
  QwtPlotMarker* GetMarkerFromID(int markerID);

  std::map<int,QwtPlotCurve*> QwtCurves;
  QwtPlotGrid* plotGrid;
  std::map<int,QwtPlotMarker*> QwtMarkers;
  QwtPlotZoomer* zoomer;
  QwtPlotPicker* picker;
  QwtPlotPicker* appendPicker;
  QwtPlotPanner* panner;
  std::map<int,QPen> highlightedCurves;

  double xViewMin, yViewMin;
  double xViewMax, yViewMax;

signals:
  void graphSelected();
  void curveHighlightChanged();
};

#endif
