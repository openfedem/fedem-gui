// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QApplication>
#include <QWheelEvent>

#include "qwt_symbol.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_zoomer.h"
#include "qwt_plot_marker.h"
#include "qwt_legend.h"
#include "qwt_picker_machine.h"
#include "qwt_scale_widget.h"
#include "qwt_plot_panner.h"
#include "qwt_series_data.h"

#include "FFuLib/FFuQtComponents/FFuQt2DPlotter.H"


namespace
{
  class CurveDataSeries : public QwtSeriesData<QPointF>
  {
    const std::vector<double>& x_data;
    const std::vector<double>& y_data;

    double xMax, xMin;
    double yMax, yMin;

    double xScale, yScale;
    double xShift, yShift;

    bool zeroAdjustX, zeroAdjustY;

  public:
    CurveDataSeries(const std::vector<double>& x,
                    const std::vector<double>& y)
      : x_data(x), y_data(y)
    {
      if (x.empty())
        xMax = xMin = 0.0;
      else
        xMax = xMin = x.front();

      if (y.empty())
        yMax = yMin = 0.0;
      else
        yMax = yMin = y.front();

      for (double value : x)
        if (value > xMax)
          xMax = value;
        else if (value < xMin)
          xMin = value;

      for (double value : y)
        if (value > yMax)
          yMax = value;
        else if (value < yMin)
          yMin = value;

      xScale = yScale = 1.0;
      xShift = yShift = 0.0;

      zeroAdjustX = zeroAdjustY = false;
    }

    void setScaleAndOffset(double scaleX, double scaleY,
                           double offsetX, double offsetY,
                           bool adjustX, bool adjustY)
    {
      xScale = scaleX;
      yScale = scaleY;
      xShift = offsetX;
      yShift = offsetY;
      zeroAdjustX = adjustX;
      zeroAdjustY = adjustY;
    }

    virtual size_t size() const { return x_data.size(); }

    virtual QPointF sample(size_t i) const
    {
      double adjX = zeroAdjustX && x_data.size() > 1 ? x_data.front() : 0.0;
      double adjY = zeroAdjustY && y_data.size() > 1 ? y_data.front() : 0.0;

      return QPointF((x_data.at(i) - adjX)*xScale + xShift,
                     (y_data.at(i) - adjY)*yScale + yShift);
    }

    virtual QRectF boundingRect() const
    {
      double adjX = zeroAdjustX && x_data.size() > 1 ? x_data.front() : 0.0;
      double adjY = zeroAdjustY && y_data.size() > 1 ? y_data.front() : 0.0;
      double left = ((xScale > 0.0 ? xMin : xMax) - adjX)*xScale + xShift;
      double top  = ((yScale > 0.0 ? yMax : yMin) - adjY)*yScale + yShift;

      return QRectF(left, top,
                    (xMax-xMin)*fabs(xScale),
                    (yMax-yMin)*fabs(yScale));
    }
  };
}

//----------------------------------------------------------------------------

FFuQt2DPlotter::FFuQt2DPlotter(QWidget* parent, const char* name)
  : QwtPlot(QwtText(name), parent)
{
  this->setWidget(this);
  this->setContentsMargins(10, 10, 10, 10);

  QFrame* canvas = (QFrame*)this->canvas();
  canvas->setLineWidth(1);
  canvas->setMidLineWidth(2);
  canvas->setFrameShadow(QFrame::Plain);
  canvas->setFrameShape(QFrame::Box);

  QObject::connect(this, SIGNAL(curveHighlightChanged()),
                   this, SLOT(fwdCurveHighlightChanged()));
  QObject::connect(this, SIGNAL(graphSelected()),
                   this, SLOT(fwdGraphSelected()));

  // Initialize map for storing curveId to QwtPlotCurve connections
  QwtCurves.clear();
  QwtMarkers.clear();
  highlightedCurves.clear();

  zoomer = new QwtPlotZoomer(this->canvas());
  zoomer->setKeyPattern(QwtEventPattern::KeyHome, Qt::Key_Home);
  picker = new QwtPlotPicker(this->canvas());
  appendPicker = new QwtPlotPicker(this->canvas());
  picker->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
  appendPicker->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton, Qt::ControlModifier);
  picker->setStateMachine(new QwtPickerClickPointMachine());
  appendPicker->setStateMachine(new QwtPickerClickPointMachine());
  picker->setEnabled(true);
  appendPicker->setEnabled(true);
  zoomer->setEnabled(false);
  panner = new QwtPlotPanner(this->canvas());
  panner->setMouseButton(Qt::RightButton);

  QObject::connect(picker, SIGNAL(selected(const QPointF&)),
                   this, SLOT(onCurvePicked(const QPointF&)));

  QObject::connect(appendPicker, SIGNAL(selected(const QPointF&)),
                   this, SLOT(onCurvePicked(const QPointF&)));

  QObject::connect(zoomer, SIGNAL(zoomed(const QRectF&)),
                   this, SLOT(zoomComplete()));

  QObject::connect(panner, SIGNAL(panned(int,int)),
                   this, SLOT(panComplete(int,int)));

  this->canvas()->setCursor(Qt::ArrowCursor);

  for (int axis = 0; axis < QwtPlot::axisCnt; axis++)
  {
    this->axisScaleDraw(axis)->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    this->axisWidget(axis)->setMargin(0);
    this->axisWidget(axis)->setFont({ "Helvetica", 7 });
  }

  plotGrid = NULL;
  xViewMin = yViewMin = 0.0;
  xViewMax = yViewMax = 1.0;
}

//--------------------------- texts ------------------------------------------
//----------------------------------------------------------------------------

void
FFuQt2DPlotter::setPlotterTitle(const std::string& title, int fontSize)
{
  QwtText titleText(title.c_str());
  titleText.setFont({ "Helvetica", fontSize, QFont::Bold });
  this->setTitle(titleText);

  this->updateLayout();
  replotAllPlotterCurves();
}

//----------------------------------------------------------------------------

std::string
FFuQt2DPlotter::getPlotterTitle()
{
	return this->title().text().toStdString();
}

//----------------------------------------------------------------------------

void
FFuQt2DPlotter::setPlotterSubTitle( const std::string& subtitle )
{
  if ( subtitle == this->getPlotterSubTitle() )
    return;

  //TODOthis->setSub setPlotterSubTitle(subtitle);
 // this->update();
  //this->backgroundPixmap.update();
  //Updated QWT
  /*
  this->setSubTitle(subtitle.c_str());
  this->updatePlot();
  this->updatePixmap();
  */
}

//----------------------------------------------------------------------------

std::string
FFuQt2DPlotter::getPlotterSubTitle()
{
	return "NOT IMPLEMENTED";
	//TODOreturn this->getPlotterSubTitle();
	//Updated QWT
	/*
    return this->subTitleCopy().toStdString();
	*/
}


//----------------------------------------------------------------------------

void
FFuQt2DPlotter::clearPlotterText()
{
  this->setTitle("");
  this->setPlotterSubTitle("");
  this->setAxisTitle(yLeft, "");
  this->setAxisTitle(xBottom, "");

  this->updateLayout();
  replotAllPlotterCurves();

}

//----------------------------- axes -----------------------------------------
//----------------------------------------------------------------------------

bool
FFuQt2DPlotter::setPlotterAxisScale( int axis, double min, double max, bool notify )
{
  if ( axis == X_AXIS )
  {
    if ( ( min - 1e-3 < this->getPlotterXAxisMin()
        && this->getPlotterXAxisMin() < min + 1e-3 )
        && ( max - 1e-3 < this->getPlotterXAxisMax()
            && this->getPlotterXAxisMax() < max + 1e-3 ) )
      return false;
  }
  else if ( axis == Y_AXIS )
    if ( ( min - 1e-3 < this->getPlotterYAxisMin()
        && this->getPlotterYAxisMin() < min + 1e-3 )
        && ( max - 1e-3 < this->getPlotterYAxisMax()
            && this->getPlotterYAxisMax() < max + 1e-3 ) )
      return false;

  bool wasblocked = this->areLibSignalsBlocked();
  if ( !notify )
    this->blockLibSignals(true);

  if ( axis == X_AXIS )
    this->setAxisScale(xBottom, min, max);
  else if ( axis == Y_AXIS )
    this->setAxisScale(yLeft, min, max);
  else
  {
    this->blockLibSignals(wasblocked);
    return false;
  }

  this->updateLayout();
  replotAllPlotterCurves();
  this->blockLibSignals(wasblocked);
  return true;
}

//----------------------------------------------------------------------------

bool
FFuQt2DPlotter::setPlotterAxisTitle( int axis, const std::string& title, int fontSize )
{
  QwtText titleText(title.c_str());
  titleText.setFont({ "Helvetica", fontSize, QFont::Bold });

  if (axis == X_AXIS){
    this->setAxisTitle(xBottom, titleText);
    this->axisWidget(xBottom)->setFont({ "Helvetica", fontSize });
  }
  else if (axis == Y_AXIS){
    this->setAxisTitle(yLeft, titleText);
    this->axisWidget(yLeft)->setFont({ "Helvetica", fontSize });
  }
  else
    return false;

  this->updateLayout();
  replotAllPlotterCurves();
  return true;
}


//----------------------------------------------------------------------------

std::string
FFuQt2DPlotter::getPlotterAxisTitle( int axis )
{
  if ( axis == X_AXIS )
    return this->axisTitle(xBottom).text().toStdString();
  else if ( axis == Y_AXIS )
    return this->axisTitle(yLeft).text().toStdString();
  else
    return "";
}

//----------------------------------------------------------------------------

double FFuQt2DPlotter::getPlotterXAxisMax()
{
  double max = 0.0;
  bool first = true;
  for (const std::pair<const int,QwtPlotCurve*>& curve : QwtCurves)
  {
    QRectF boundRect = curve.second->data()->boundingRect();
    double currValue = boundRect.x() + boundRect.width();
    if (first)
    {
      max = currValue;
      first = false;
    }
    else if (currValue > max)
      max = currValue;
  }
  return max;
}

//----------------------------------------------------------------------------

double FFuQt2DPlotter::getPlotterXAxisMin()
{
  double min = 0.0;
  bool first = true;
  for (const std::pair<const int,QwtPlotCurve*>& curve : QwtCurves)
  {
    double currValue = curve.second->data()->boundingRect().x();
    if (first)
    {
      min = currValue;
      first = false;
    }
    else if (currValue < min)
      min = currValue;
  }
  return min;
}

//----------------------------------------------------------------------------

double FFuQt2DPlotter::getPlotterYAxisMax()
{
  double max = 0.0;
  bool first = true;
  for (const std::pair<const int,QwtPlotCurve*>& curve : QwtCurves)
  {
    double currValue = curve.second->data()->boundingRect().y();
    if (first)
    {
      max = currValue;
      first = false;
    }
    else if (currValue > max)
      max = currValue;
  }
  return max;
}

//----------------------------------------------------------------------------

double FFuQt2DPlotter::getPlotterYAxisMin()
{
  double min = 0.0;
  bool first = true;
  for (const std::pair<const int,QwtPlotCurve*>& curve : QwtCurves)
  {
    QRectF boundRect = curve.second->data()->boundingRect();
    double currValue = boundRect.y() - boundRect.height();
    if (first)
    {
      min = currValue;
      first = false;
    }
    else if (currValue < min)
      min = currValue;
  }
  return min;
}

//-------------------------- curves ------------------------------------------
//----------------------------------------------------------------------------

int
FFuQt2DPlotter::loadNewPlotterCurve(std::vector<double>* const x_data,
				    std::vector<double>* const y_data,
    const UColor& color, int style, int width,
    int symbol, int symbolsize, int numSymbols, const std::string& legend,
    double scaleX, double offsetX, bool zeroAdjustX, double scaleY,
    double offsetY, bool zeroAdjustY )
{
  this->setCanvasBackground(QColor(Qt::white));

  // Find largest curve index already in use
  int curveId = 0;
  for (const std::pair<const int,QwtPlotCurve*>& curve : QwtCurves)
    if (curveId < curve.first)
      curveId = curve.first;

  QwtPlotCurve* newCurve = new QwtPlotCurve(legend.c_str());
  QwtCurves[++curveId] = newCurve;

  newCurve->setAxes(xBottom,yLeft);
  newCurve->setSamples(new CurveDataSeries(*x_data,*y_data));
  newCurve->attach(this);

  this->setPlotterCurveStyle(curveId, style, width, color, false);
  this->setPlotterCurveSymbol(curveId, symbol, symbolsize, numSymbols);
  this->setPlotterScaleAndOffset(curveId, scaleX, offsetX, zeroAdjustX,
                                 scaleY, offsetY, zeroAdjustY, false);

  if (autoScaleOnLoadCurve)
    this->autoScalePlotter();

  return curveId;
}

//----------------------------------------------------------------------------

bool FFuQt2DPlotter::loadPlotterCurveData(int curveid,
    std::vector<double>* const x, std::vector<double>* const y, bool,
    const UColor& color, int style, int width, int symbol, int symbolsize,
    int numSymbols, const std::string& legend, double scaleX, double offsetX,
    bool zeroAdjustX, double scaleY, double offsetY, bool zeroAdjustY)
{
  QwtPlotCurve* activeCurve = this->GetCurveFromID(curveid);
  if (!activeCurve) return false;

  activeCurve->setTitle(legend.c_str());
  activeCurve->setSamples(new CurveDataSeries(*x,*y));

  this->setPlotterCurveStyle(curveid, style, width, color, false);
  this->setPlotterCurveSymbol(curveid, symbol, symbolsize, numSymbols);
  this->setPlotterScaleAndOffset(curveid, scaleX, offsetX, zeroAdjustX,
                                 scaleY, offsetY, zeroAdjustY, false);

  if (autoScaleOnLoadCurve)
    this->autoScalePlotter();
  else
    this->replotAllPlotterCurves();

  return true;
}

//----------------------------------------------------------------------------

void
FFuQt2DPlotter::setPlotterScaleAndOffset( int curveid, double scaleX,
    double offsetX, bool zeroAdjustX, double scaleY, double offsetY,
    bool zeroAdjustY, bool doUpdate )
{
  QwtPlotCurve* curve = this->GetCurveFromID(curveid);
  if (!curve) return;

  ((CurveDataSeries*)curve->data())->setScaleAndOffset(scaleX,scaleY,
                                                       offsetX,offsetY,
                                                       zeroAdjustX,zeroAdjustY);
  if (doUpdate)
  {
    if (autoScaleOnLoadCurve)
      this->autoScalePlotter();
    else
      this->replotAllPlotterCurves();
  }
}

//----------------------------------------------------------------------------

void
FFuQt2DPlotter::removePlotterCurve( int curveid )
{
	QwtPlotCurve* activeCurve = GetCurveFromID(curveid);
	if (activeCurve != NULL)
	{
		activeCurve->detach();
		delete activeCurve;
		QwtCurves.erase(curveid);

		replotAllPlotterCurves();
	}
}

//----------------------------------------------------------------------------

void FFuQt2DPlotter::removePlotterCurves()
{
  for (const std::pair<const int,QwtPlotCurve*>& curve : QwtCurves)
  {
    curve.second->detach();
    delete curve.second;
  }

  QwtCurves.clear();

/*TODO
  this->removeCurves();
  this->updatePlot();
  this->updatePixmap();
*/
}

//----------------------------------------------------------------------------

void
FFuQt2DPlotter::replotAllPlotterCurves()
{
	//this->autoScalePlotter();
	this->setAxisScale(xBottom, xViewMin, xViewMax);
	this->setAxisScale(yLeft, yViewMin, yViewMax);
	this->replot();
}

//----------------------------------------------------------------------------

std::vector<int> FFuQt2DPlotter::getPlotterCurves()
{
  std::vector<int> vec;
  vec.reserve(QwtCurves.size());
  for (const std::pair<const int,QwtPlotCurve*>& curve : QwtCurves)
    vec.push_back(curve.first);

  return vec;
}

//----------------------------------------------------------------------------

void FFuQt2DPlotter::setPlotterCurveStyle(int curveid, int style, int width,
                                          const UColor& color, bool redraw)
{
  QwtPlotCurve* activeCurve = this->GetCurveFromID(curveid);
  if (activeCurve)
  {
    if (style != (int)activeCurve->style())
      switch (style) {
      case LINES:
        activeCurve->setStyle(QwtPlotCurve::Lines);
        break;
      case DOTS:
        activeCurve->setStyle(QwtPlotCurve::Dots);
        break;
      case NO_CURVE:
        activeCurve->setStyle(QwtPlotCurve::NoCurve);
        break;
      }

    QPen newPen = QPen(QColor(color[0],color[1],color[2]),width);

    std::map<int,QPen>::iterator curve = highlightedCurves.find(curveid);
    if (curve != highlightedCurves.end())
      curve->second = newPen;
    else
      activeCurve->setPen(newPen);

    if (redraw)
      this->replotAllPlotterCurves();
  }
}


//----------------------------------------------------------------------------

int
FFuQt2DPlotter::getPlotterCurveStyle( int curveid )
{
	QwtPlotCurve* activeCurve = GetCurveFromID(curveid);
	if (activeCurve != NULL)
	{
		switch (activeCurve->style())
		{
		case QwtPlotCurve::NoCurve:
			return NO_CURVE;
		case QwtPlotCurve::Dots:
			return DOTS;
		case QwtPlotCurve::Lines:
			return LINES;
		default:
			return -1;
		}
	}

	return -1;
}

//----------------------------------------------------------------------------

UColor FFuQt2DPlotter::getPlotterCurveColor(int curveid)
{
  UColor color = {0,0,0};
  QwtPlotCurve* activeCurve = this->GetCurveFromID(curveid);
  if (activeCurve)
  {
    QPen pen = activeCurve->pen();
    color[0] = pen.color().red();
    color[1] = pen.color().green();
    color[2] = pen.color().blue();
  }

  return color;
}

//----------------------------------------------------------------------------

int
FFuQt2DPlotter::getPlotterCurveWidth( int curveid )
{
	QwtPlotCurve* activeCurve = GetCurveFromID(curveid);
	if (activeCurve != NULL)
	{
		return activeCurve->pen().width();
	}

	return 0;
}

//----------------------------------------------------------------------------

void FFuQt2DPlotter::setPlotterCurveSymbol(int curveid, int symbol, int size, int)
{
	QwtPlotCurve* activeCurve = GetCurveFromID(curveid);
	if (activeCurve != NULL)
	{
		if (symbol !=  this->getPlotterCurveSymbol(curveid) || size
			!= this->getPlotterCurveSymbolSize(curveid))
		{
			QwtSymbol* s = new QwtSymbol();
			s->setSize(size);

			switch (symbol)
			{
			case NONE:
				s->setStyle(QwtSymbol::NoSymbol);
				break;
			case PLUS_CROSS:
				s->setStyle(QwtSymbol::Cross);
				break;
			case X_CROSS:
				s->setStyle(QwtSymbol::XCross);
				break;
			case CIRCLE:
				s->setStyle(QwtSymbol::Ellipse);
				break;
			case DIAMOND:
				s->setStyle(QwtSymbol::Diamond);
				break;
			case RECT:
				s->setStyle(QwtSymbol::Rect);
				break;
			case U_TRIANGLE:
				s->setStyle(QwtSymbol::UTriangle);
				break;
			case D_TRIANGLE:
				s->setStyle(QwtSymbol::DTriangle);
				break;
			case L_TRIANGLE:
				s->setStyle(QwtSymbol::LTriangle);
				break;
			case R_TRIANGLE:
				s->setStyle(QwtSymbol::RTriangle);
				break;

			default:
				delete s;
				s = 0;
			}

			if (s)
			{
				activeCurve->setSymbol(s);
			}
		}
		//this->repaint();
		//this->autoScalePlotter();
		replotAllPlotterCurves();//this->replot();

		//TODOthis->setCurveNumSymbols(curveid, num);
		//this->clearFramePixmap();
		//this->updatePlot();
		//this->redrawPixmap();
		//this->updateFramePixmap();
	}
}

//----------------------------------------------------------------------------

int
FFuQt2DPlotter::getPlotterCurveSymbol( int curveid )
{
	QwtPlotCurve* activeCurve = GetCurveFromID(curveid);
	if (activeCurve != NULL)
	{
		if (activeCurve->symbol() != NULL){
			switch (activeCurve->symbol()->style())
			{
			case QwtSymbol::NoSymbol:
				return NONE;
			case QwtSymbol::Ellipse:
				return CIRCLE;
			case QwtSymbol::Rect:
				return RECT;
			case QwtSymbol::Diamond:
				return DIAMOND;
			case QwtSymbol::DTriangle:
				return D_TRIANGLE;
			case QwtSymbol::UTriangle:
				return U_TRIANGLE;
			case QwtSymbol::LTriangle:
				return L_TRIANGLE;
			case QwtSymbol::RTriangle:
				return R_TRIANGLE;
			case QwtSymbol::Cross:
				return PLUS_CROSS;
			case QwtSymbol::XCross:
				return X_CROSS;
			default:
				return -1;
			}
		}
	}
	return -1;
}

//----------------------------------------------------------------------------

int
FFuQt2DPlotter::getPlotterCurveSymbolSize( int curveid )
{
	QwtPlotCurve* activeCurve = GetCurveFromID(curveid);
	if (activeCurve != NULL)
	{
		if (activeCurve->symbol() != NULL){
			return activeCurve->symbol()->size().width();
		}
	}
	return 0;
}

//----------------------------------------------------------------------------

int FFuQt2DPlotter::getPlotterCurveNumSymbols(int /*curveid*/)
{
//TODO return this->getCurveNumSymbols(curveid);
  return 0;
}

//----------------------------------------------------------------------------




//------------------------------ grid ----------------------------------------
//----------------------------------------------------------------------------

void
FFuQt2DPlotter::showPlotterGridX( int gridtype )
{
	if (plotGrid == NULL)
	{
		plotGrid = new QwtPlotGrid();
		plotGrid->setMajorPen(QColor("darkGray"));
		plotGrid->setMinorPen(QColor("lightGray"));
		plotGrid->attach(this);
	}
	switch (gridtype)
	{
	case NOGRID:
		plotGrid->enableX(false);
		plotGrid->enableXMin(false);
		break;

	case FINEGRID:
		plotGrid->enableX(true);
		plotGrid->enableXMin(true);
		break;

	case COARSEGRID:
		plotGrid->enableX(true);
		plotGrid->enableXMin(false);
		break;
	}

	replotAllPlotterCurves();
}

//----------------------------------------------------------------------------

void
FFuQt2DPlotter::showPlotterGridY( int gridtype )
{
	if (plotGrid == NULL)
	{
		plotGrid = new QwtPlotGrid();
		plotGrid->setMajorPen(QColor("darkGray"));
		plotGrid->setMinorPen(QColor("lightGray"));
		plotGrid->attach(this);
	}
	switch (gridtype)
	{
	case NOGRID:
		plotGrid->enableY(false);
		plotGrid->enableYMin(false);
		break;

	case FINEGRID:
		plotGrid->enableY(true);
		plotGrid->enableYMin(true);
		break;

	case COARSEGRID:
		plotGrid->enableY(true);
		plotGrid->enableYMin(false);
		break;
	}

	replotAllPlotterCurves();
}

//----------------------------------------------------------------------------

int FFuQt2DPlotter::getPlotterGridTypeX()
{
  if (!plotGrid || !plotGrid->xEnabled())
    return NOGRID;

  return plotGrid->xMinEnabled() ? FINEGRID : COARSEGRID;
}

//----------------------------------------------------------------------------

int FFuQt2DPlotter::getPlotterGridTypeY()
{
  if (!plotGrid || !plotGrid->yEnabled())
    return NOGRID;

  return plotGrid->yMinEnabled() ? FINEGRID : COARSEGRID;
}

//----------------------------------------------------------------------------

void FFuQt2DPlotter::setPlotterGridMajPen(const std::string& color, unsigned int width)
{
	if (plotGrid == NULL)
	{
		plotGrid = new QwtPlotGrid();
		plotGrid->attach(this);
	}

	QPen p = plotGrid->majorPen();
	p.setColor(QColor(color.c_str()));
	p.setWidth(width);
	plotGrid->setMajorPen(p);

	replotAllPlotterCurves();
}

//----------------------------------------------------------------------------

void
FFuQt2DPlotter::setPlotterGridMinPen( const std::string& color,
    unsigned int width )
{
	if (plotGrid == NULL)
	{
		plotGrid = new QwtPlotGrid();
		plotGrid->attach(this);
	}

	QPen p = plotGrid->minorPen();
	p.setColor(QColor(color.c_str()));
	p.setWidth(width);
	plotGrid->setMinorPen(p);

	replotAllPlotterCurves();
}

//--------------------------- legend -----------------------------------------
//----------------------------------------------------------------------------

void
FFuQt2DPlotter::showPlotterLegend( bool show )
{
	QwtAbstractLegend* legend = this->legend();
	if (show)
	{
		if (legend == NULL)
		{
			legend = new QwtLegend(this);
			this->insertLegend(legend, QwtPlot::BottomLegend);
		}
	}
	else
	{
		this->insertLegend(NULL);
	}

	replotAllPlotterCurves();
}

//----------------------------------------------------------------------------

bool
FFuQt2DPlotter::getPlotterLegendEnabled()
{
	QwtAbstractLegend* legend = this->legend();
	if (legend == NULL)
	{
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------

void
FFuQt2DPlotter::setPlotterLegendLabel( int curveid, const std::string& label )
{
  if ( label == this->getPlotterLegendLabel(curveid) )
    return;

  QwtPlotCurve* activeCurve = GetCurveFromID(curveid);
  if (activeCurve != NULL)
  {
	  activeCurve->setTitle(QString(label.c_str()));

  }

  replotAllPlotterCurves();
}

//----------------------------------------------------------------------------

std::string
FFuQt2DPlotter::getPlotterLegendLabel( int curveid )
{
	QwtPlotCurve* activeCurve = GetCurveFromID(curveid);
	if (activeCurve != NULL)
	{
		return activeCurve->title().text().toStdString();
	}
	return "";
}

//------------------------------ markers -------------------------------------
//----------------------------------------------------------------------------

int
FFuQt2DPlotter::insertPlotterLineMarker( const std::string& label, int axis,
    double pos )
{
  // Find largest marker index already in use
  int markerId = 0;
  for (const std::pair<const int,QwtPlotMarker*>& marker : QwtMarkers)
    if (markerId < marker.first)
      markerId = marker.first;

  QwtPlotMarker* newMarker = new QwtPlotMarker(label.c_str());
  QwtMarkers[++markerId] = newMarker;

  newMarker->setLinePen(QColor(0,0,180));

  if (axis == X_AXIS)
  {
    newMarker->setLineStyle(QwtPlotMarker::VLine);
    newMarker->setValue(pos, 0);
  }
  else
  {
    newMarker->setLineStyle(QwtPlotMarker::HLine);
    newMarker->setValue(0, pos);
  }

  newMarker->attach(this);

  this->replotAllPlotterCurves();

  return markerId;
}

//----------------------------------------------------------------------------

bool
FFuQt2DPlotter::setPlotterMarkerPos( int id, double x, double y )
{
	QwtPlotMarker* activeMarker = GetMarkerFromID(id);
	if (activeMarker != NULL)
	{
	     activeMarker->setValue( x, y);
		 replotAllPlotterCurves();
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

bool
FFuQt2DPlotter::setPlotterMarkerXPos( int id, double x )
{
	QwtPlotMarker* activeMarker = GetMarkerFromID(id);
	if (activeMarker != NULL)
	{
		activeMarker->setXValue(x);
		replotAllPlotterCurves();
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

bool
FFuQt2DPlotter::setPlotterMarkerYPos( int id, double y )
{
	QwtPlotMarker* activeMarker = GetMarkerFromID(id);
	if (activeMarker != NULL)
	{
		activeMarker->setYValue(y);
		replotAllPlotterCurves();
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

bool
FFuQt2DPlotter::removePlotterMarker( int id )
{
	QwtPlotMarker* activeMarker = GetMarkerFromID(id);
	if (activeMarker != NULL)
	{
		activeMarker->detach();
		delete activeMarker;
		QwtMarkers.erase(id);
		replotAllPlotterCurves();
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

void FFuQt2DPlotter::removePlotterMarkers()
{
  for (const std::pair<const int,QwtPlotCurve*>& curve : QwtCurves)
  {
    curve.second->detach();
    delete curve.second;
  }

  QwtCurves.clear();
}

//------------------------- zoom & shift -------------------------------------
//----------------------------------------------------------------------------

void
FFuQt2DPlotter::autoScalePlotter()
{
	xViewMin = this->getPlotterXAxisMin();
	xViewMax = this->getPlotterXAxisMax();
	yViewMin = this->getPlotterYAxisMin();
	yViewMax = this->getPlotterYAxisMax();

	if (yViewMax - yViewMin == 0)
	{
		if (yViewMax == 0)
		{
			yViewMin = -1;
			yViewMax = 1;
		}
		else
		{
			double delta = 0.02*fabs(yViewMax);
			yViewMax += delta;
			yViewMin -= delta;
		}
	}

	this->replotAllPlotterCurves();
}

//----------------------------------------------------------------------------

void
FFuQt2DPlotter::zoomPlotterWindow()
{
		zoomer->setZoomBase();
		this->canvas()->setCursor(Qt::CrossCursor);
		this->zoomer->setEnabled(true);
		this->picker->setEnabled(false);
		this->appendPicker->setEnabled(false);
}

//----------------------------------------------------------------------------

void
FFuQt2DPlotter::zoomPlotterAllInWindow()
{
//	this->unsetCursor();
	this->canvas()->setCursor(Qt::CrossCursor);
	this->zoomer->setEnabled(true);
//TODO  this->setZoomAllInRectMode(true);
}

//----------------------------------------------------------------------------

void
FFuQt2DPlotter::zoomPlotterOut()
{
  autoScaleOnLoadCurve = false;
  this->zoomPlotter(1.25);
}

//----------------------------------------------------------------------------

void
FFuQt2DPlotter::zoomPlotterIn()
{
  autoScaleOnLoadCurve = false;
  this->zoomPlotter(0.8);
}

//----------------------------------------------------------------------------

void FFuQt2DPlotter::zoomPlotter(double zoomFactor)
{
	QwtInterval intervalBottom = this->axisInterval(xBottom);
	QwtInterval intervalLeft = this->axisInterval(yLeft);
	double minX = intervalBottom.minValue();
	double maxX = intervalBottom.maxValue();
	double minY = intervalLeft.minValue();
	double maxY = intervalLeft.maxValue();
	double xWidth = intervalBottom.width();
	double yWidth = intervalLeft.width();

	double xNewWidth = xWidth*(zoomFactor);
	double yNewWidth = yWidth*(zoomFactor);

	minX += (xWidth - xNewWidth) / 2;
	maxX -= (xWidth - xNewWidth) / 2;
	minY += (yWidth - yNewWidth) / 2;
	maxY -= (yWidth - yNewWidth) / 2;

	xViewMin = minX;
	xViewMax = maxX;
	yViewMin = minY;
	yViewMax = maxY;

	replotAllPlotterCurves();
}

//----------------------------------------------------------------------------

void
FFuQt2DPlotter::shiftPlotterLeft()
{
	autoScaleOnLoadCurve = false;
	double min = xViewMin;
	double max = xViewMax;
	double dx = (max - min)*(d_shiftFactor);

	min += dx;
	max += dx;

	xViewMin = min;
	xViewMax = max;
	replotAllPlotterCurves();
}

//----------------------------------------------------------------------------

void
FFuQt2DPlotter::shiftPlotterRight()
{
	autoScaleOnLoadCurve = false;
	double min = xViewMin;
	double max = xViewMax;
	double dx = (max - min)*(d_shiftFactor);

	min -= dx;
	max -= dx;

	xViewMin = min;
	xViewMax = max;
	replotAllPlotterCurves();
}

//----------------------------------------------------------------------------

void
FFuQt2DPlotter::shiftPlotterUp()
{
	autoScaleOnLoadCurve = false;
	double  min = yViewMin;
	double  max = yViewMax;
	double  dy = (max - min)*(d_shiftFactor);
	min -= dy;
	max -= dy;

	yViewMin = min;
	yViewMax = max;
	replotAllPlotterCurves();
}

//----------------------------------------------------------------------------

void
FFuQt2DPlotter::shiftPlotterDown()
{
	autoScaleOnLoadCurve = false;
	double  min = yViewMin;
	double  max = yViewMax;
	double  dy = (max - min)*(d_shiftFactor);
	min += dy;
	max += dy;

	yViewMin = min;
	yViewMax = max;
	replotAllPlotterCurves();
}

//----------------------------------------------------------------------------

void FFuQt2DPlotter::enablePlotterDynamicView(bool /*yesOrNo*/)
{
	//TODO  this->enableDynamicView(yesOrNo);
}

//----------------------------------------------------------------------------

bool
FFuQt2DPlotter::getPlotterDynamicViewEnabled()
{
	//TODO return this->dynamicViewEnabled();
	return true;
}

//------------------------- highlight ----------------------------------------
//----------------------------------------------------------------------------

void FFuQt2DPlotter::enablePlotterPickCurve(bool /*yesOrNo*/)
{
	//TODO this->setPickCurveMode(yesOrNo);
}

//----------------------------------------------------------------------------

void FFuQt2DPlotter::enablePlotterHighlightCurveOnPick(bool /*yesOrNo*/)
{
	//TODO this->setPickCurveMode(yesOrNo);
	//TODO this->setHighlightCurveOnPickMode(yesOrNo);
}

//----------------------------------------------------------------------------

bool
FFuQt2DPlotter::getPlotterPickCurve()
{
	//TODO return this->getPickCurveEnabled();
	return true;
}

//----------------------------------------------------------------------------

bool
FFuQt2DPlotter::getPlotterHighlightCurveOnPick()
{
	//TODOreturn this->getHighlightCurveOnPick();
	return true;
}

//----------------------------------------------------------------------------

bool FFuQt2DPlotter::isPlotterCurvehighlighted(int curveid)
{
  return highlightedCurves.find(curveid) != highlightedCurves.end();
}

//----------------------------------------------------------------------------

void FFuQt2DPlotter::highlightPlotterCurve(int curveid, bool highlight, bool notify)
{
  bool wasblocked = this->areLibSignalsBlocked();
  if (!notify) this->blockLibSignals(true);

  if (highlight)
  {
    QwtPlotCurve* c = this->GetCurveFromID(curveid);
    QPen pen = c->pen();
    highlightedCurves[curveid] = pen;
    pen.setColor(QColor("red"));
    c->setPen(pen);
    c->setZ(21);
  }
  else
  {
    std::map<int,QPen>::iterator it = highlightedCurves.find(curveid);
    if (it != highlightedCurves.end()) {
      QwtPlotCurve* c = this->GetCurveFromID(curveid);
      c->setPen(it->second);
      c->setZ(20);
      highlightedCurves.erase(it);
    }
  }

  this->blockLibSignals(wasblocked);
}

//----------------------------------------------------------------------------

void FFuQt2DPlotter::highlightAllPlotterCurves(bool highlight, bool notify)
{
  bool wasblocked = this->areLibSignalsBlocked();
  if (!notify) this->blockLibSignals(true);

  if (highlight)
    for (const std::pair<const int,QwtPlotCurve*>& curve : QwtCurves)
    {
      QPen pen = curve.second->pen();
      highlightedCurves[curve.first] = pen;
      pen.setColor(QColor("red"));
      curve.second->setPen(pen);
      curve.second->setZ(21);
    }
  else
  {
    QwtPlotCurve* c;
    for (const std::pair<const int,QPen>& curve : highlightedCurves)
      if ((c = this->GetCurveFromID(curve.first))) {
        c->setPen(curve.second);
        c->setZ(20);
      }
    highlightedCurves.clear();
  }

  this->blockLibSignals(wasblocked);
}

//----------------------------------------------------------------------------

std::vector<int> FFuQt2DPlotter::getPlotterHighlightedCurves()
{
  std::vector<int> vec;
  vec.reserve(highlightedCurves.size());
  for (const std::pair<const int,QPen>& curve : highlightedCurves)
    vec.push_back(curve.first);
  return vec;
}

//----------------------------------------------------------------------------

void FFuQt2DPlotter::setPlotterHighlightColor(const UColor& /*color*/)
{
  /* TODO
  if (color != this->getPlotterHighlightColor())
    this->setCurveHighlightColor(QColor(color[0],color[1],color[2]));
  */
}

//----------------------------------------------------------------------------

UColor FFuQt2DPlotter::getPlotterHighlightColor()
{
  /* TODO
  QColor col = this->getCurveHighlightColor();
  return {col.red(), col.green(), col.blue()};
  */
  return {0,0,0};
}

//----------------------------------------------------------------------------

void FFuQt2DPlotter::setPlotterHighlightPenWidth(int /*width*/)
{
	//TODO  this->setCurveHighlightPenWidth(width);
}

//----------------------------------------------------------------------------

int
FFuQt2DPlotter::getPlotterHighlightPenWidth()
{
	//TODO return this->getCurveHighlightPenWidth();
	return 0;
}

//----------------------------------------------------------------------------

bool FFuQt2DPlotter::savePlotterAsImage(const std::string& fName, const char* format)
{
  return this->grab().save(fName.c_str(), format, 100);
}

//----------------------------------------------------------------------------

void FFuQt2DPlotter::fwdCurveHighlightChanged()
{
  this->invokeCurveHighlightChangedCB();
  this->onCurveHighlightChanged();
}

//----------------------------------------------------------------------------

void FFuQt2DPlotter::fwdGraphSelected()
{
  this->invokeGraphSelectedCB();
}

void FFuQt2DPlotter::zoomComplete()
{
	zoomer->setZoomBase();
	autoScaleOnLoadCurve = false;

	this->zoomer->setEnabled(false);
	this->picker->setEnabled(true);
	this->appendPicker->setEnabled(true);
	this->canvas()->setCursor(Qt::ArrowCursor);

	yViewMin = this->axisScaleDiv(yLeft).lowerBound();
	yViewMax = this->axisScaleDiv(yLeft).upperBound();
	xViewMin = this->axisScaleDiv(xBottom).lowerBound();
	xViewMax = this->axisScaleDiv(xBottom).upperBound();
}

void FFuQt2DPlotter::panComplete(int, int)
{
	yViewMin = this->axisScaleDiv(yLeft).lowerBound();
	yViewMax = this->axisScaleDiv(yLeft).upperBound();
	xViewMin = this->axisScaleDiv(xBottom).lowerBound();
	xViewMax = this->axisScaleDiv(xBottom).upperBound();

	autoScaleOnLoadCurve = false;
}

//----------------------------------------------------------------------------
QwtPlotCurve* FFuQt2DPlotter::GetCurveFromID(int curveID)
{
  std::map<int,QwtPlotCurve*>::const_iterator it = QwtCurves.find(curveID);
  return it == QwtCurves.end() ? NULL : it->second;
}

QwtPlotMarker* FFuQt2DPlotter::GetMarkerFromID(int markerID)
{
  std::map<int,QwtPlotMarker*>::const_iterator it = QwtMarkers.find(markerID);
  return it == QwtMarkers.end() ? NULL : it->second;
}


void FFuQt2DPlotter::onCurvePicked(const QPointF& point)
{
	bool d_ctrlPressed = QApplication::keyboardModifiers() & Qt::ControlModifier;

	double xMax = xViewMax;
	double xMin = xViewMin;
	double yMax = yViewMax;
	double yMin = yViewMin;
	double xTol = (xMax - xMin) / 100;
	double yTol = (yMax - yMin) / 100;

	long curveid = closestCurveFaster(point.x(), point.y(), xTol, yTol);

	// If no close curve was found -1 is returned
	// In that case, unhighlight all curves
	if (curveid == -1) {
		if (highlightedCurves.size() > 0)
		{
			highlightAllPlotterCurves(false, true);
			replotAllPlotterCurves();
			emit curveHighlightChanged();
		}

		emit graphSelected();
		return;
	}

  if (!this->GetCurveFromID(curveid)) return;

	bool changed = false;
	bool highlighted = isPlotterCurvehighlighted(curveid);

	if (!d_ctrlPressed) {        // !ctrlPressed
		if (!highlighted || highlightedCurves.size() > 1) {
			highlightAllPlotterCurves(false, true);
			highlightPlotterCurve(curveid, true, true);
			replotAllPlotterCurves();
			changed = true;
		}
	}
	else {                    // ctrlPressed
		if (!highlighted) { // unhighlighted
			highlightPlotterCurve(curveid, true, true);
			replotAllPlotterCurves();
			changed = true;
		}
		else {                  // highlighted
			highlightPlotterCurve(curveid, false, true);
			replotAllPlotterCurves();
			changed = true;
		}
	}

	if (changed)
		emit curveHighlightChanged();

}

void FFuQt2DPlotter::wheelEvent(QWheelEvent* event)
{
  double zoomFactor;
  int numSteps = event->angleDelta().y() / 120;
  if (numSteps < 0)
    zoomFactor = 0.8 / static_cast<double>(-numSteps);
  else if (numSteps > 0)
    zoomFactor = 1.2 * static_cast<double>(numSteps);
  else
    return;

  autoScaleOnLoadCurve = false;
  this->zoomPlotter(zoomFactor);
}


long FFuQt2DPlotter::closestCurveFaster(double xpos, double ypos, double distX, double distY) const
{
	long curveid = -1;

	double dminX = 1.0e10;
	double dminY = 1.0e10;
	double cx, cy;
	double xMin, xMax, yMin, yMax;

	QwtSeriesData<QPointF> *points;

	// Going into loop over curves
	for (const std::pair<const int,QwtPlotCurve*>& curve : QwtCurves){
		if (curve.second->isVisible()) {
			points = curve.second->data();
			xMin = xpos - distX;
			xMax = xpos + distX;
			yMin = ypos - distY;
			yMax = ypos + distY;

			// Going into loop over individual points
			for (size_t i = 0; i < points->size(); i++) {
				QPointF sample = points->sample(i);
				cx = sample.x();
				cy = sample.y();

				if (cx > xMin  &&  cx < xMax  &&  cy > yMin  &&  cy < yMax) {
					cx -= xpos;
					cy -= ypos;
					if (cx < dminX && cy < dminY){

						dminX = cx;
						dminY = cy;
						curveid = curve.first;
					}
				}
			}
		}
	}
	return curveid;
}
