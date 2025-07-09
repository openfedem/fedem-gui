// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiGraphView.H"
#include "vpmUI/vpmUITopLevels/FuiMainWindow.H"
#include "vpmUI/Fui.H"


Fmd_SOURCE_INIT(FUI_GRAPHVIEWTLS, FuiGraphViewTLS, FFuMDIWindow);

short int FuiGraphViewTLS::numInstance = 0;

//----------------------------------------------------------------------------

FuiGraphViewTLS::FuiGraphViewTLS()
{
  Fmd_CONSTRUCTOR_INIT(FuiGraphViewTLS);

  myView = NULL;
}
//----------------------------------------------------------------------------

void FuiGraphView::initWidgets()
{
  // plotter settings
  this->enablePlotterHighlightCurveOnPick(true);

  // create ui's UA object
  FFuUAExistenceHandler::invokeCreateUACB(this);

  // viewSession init
  this->setUpdateOnChangesWhenInMem(false);
}
//----------------------------------------------------------------------------

FFuaUIValues* FuiGraphView::createValuesObject()
{
  return new FuaGraphViewValues();
}
//----------------------------------------------------------------------------

void FuiGraphView::setUIValues(const FFuaUIValues* values)
{
  FuaGraphViewValues* graphvalues = (FuaGraphViewValues*)values;

  this->setPlotterTitle(graphvalues->title, graphvalues->fontSize.front());
  this->setPlotterSubTitle(graphvalues->subtitle);
  this->setPlotterAxisTitle(FFu2DPlotter::X_AXIS,graphvalues->xAxisLabel, graphvalues->fontSize[1]);
  this->setPlotterAxisTitle(FFu2DPlotter::Y_AXIS,graphvalues->yAxisLabel, graphvalues->fontSize[2]);

  this->showPlotterLegend(graphvalues->showLegend);

  this->showPlotterGridX(graphvalues->gridtype);
  this->showPlotterGridY(graphvalues->gridtype);

  this->setAutoScaleOnLoadCurve(graphvalues->autoScaleOnLoadCurve);
  this->setPlotterAxisScale(FFu2DPlotter::X_AXIS,
			    graphvalues->xAxisMin,graphvalues->xAxisMax,false);
  this->setPlotterAxisScale(FFu2DPlotter::Y_AXIS,
			    graphvalues->yAxisMin,graphvalues->yAxisMax,false);

  for (const FuaGraphViewValues::CurveData& curve : graphvalues->curves) {
    this->setPlotterLegendLabel(curve.id,curve.legend);
    this->setPlotterCurveStyle (curve.id,curve.style,curve.width,curve.color);
    this->setPlotterCurveSymbol(curve.id,curve.symbols,curve.symbolssize,curve.numSymbols);
  }
}
//----------------------------------------------------------------------------

void FuiGraphView::getUIValues(FFuaUIValues* values)
{
  FuaGraphViewValues*  graphvalues = (FuaGraphViewValues*) values;

  graphvalues->title = this->getPlotterTitle();
  graphvalues->subtitle = this->getPlotterSubTitle();
  graphvalues->xAxisLabel = this->getPlotterAxisTitle(FFu2DPlotter::X_AXIS);
  graphvalues->yAxisLabel = this->getPlotterAxisTitle(FFu2DPlotter::Y_AXIS);

  graphvalues->showLegend = this->getPlotterLegendEnabled();

  graphvalues->gridtype = this->getPlotterGridTypeX();//assume xgrid = ygrid

  graphvalues->autoScaleOnLoadCurve = this->getAutoScaleOnLoadCurve();

  graphvalues->xAxisMin = this->getPlotterXAxisMin();
  graphvalues->xAxisMax = this->getPlotterXAxisMax();

  graphvalues->yAxisMin = this->getPlotterYAxisMin();
  graphvalues->yAxisMax = this->getPlotterYAxisMax();

  graphvalues->curves.clear();
  FuaGraphViewValues::CurveData curve;

  for (int curveId : this->getPlotterCurves()) {
    curve.id = curveId;
    curve.legend = this->getPlotterLegendLabel(curveId);
    curve.style = this->getPlotterCurveStyle(curveId);
    curve.width = this->getPlotterCurveWidth(curveId);
    curve.color = this->getPlotterCurveColor(curveId);
    curve.symbols = this->getPlotterCurveSymbol(curveId);
    curve.symbolssize = this->getPlotterCurveSymbolSize(curveId);
    curve.numSymbols = this->getPlotterCurveNumSymbols(curveId);
    graphvalues->curves.push_back(curve);
  }
}
//----------------------------------------------------------------------------

void FuiGraphView::zoomAll()
{
  this->setAutoScaleOnLoadCurve(true);
  this->autoScalePlotter();
}
//----------------------------------------------------------------------------

void FuiGraphView::clearUIValues()
{
  this->clearPlotterText();
}
//----------------------------------------------------------------------------

void FuiGraphView::deleteItem(int item)
{
  this->removePlotterCurve(item);
}
//----------------------------------------------------------------------------

void FuiGraphView::deleteItems()
{
  this->removePlotterCurves();
}
//----------------------------------------------------------------------------

void FuiGraphView::permTotSelectItems(const std::vector<int>& totalSelection)
{
  this->highlightAllPlotterCurves(false,true);

  for (int curveId : totalSelection)
    this->highlightPlotterCurve(curveId,true,true);

  this->replotAllPlotterCurves();
}
//----------------------------------------------------------------------------

void FuiGraphView::onCurveHighlightChanged()
{
  this->invokeCurveHighlightChangedCB();
  this->invokePermTotSelectItemsCB(this->getPlotterHighlightedCurves());
}
//----------------------------------------------------------------------------

void FuiGraphView::onAxisScaleChanged()
{
  this->invokeAxisScaleChangedCB();
}
//----------------------------------------------------------------------------

void FuiGraphView::onPoppedUp()
{
  FuiMainWindow* mainWindow = Fui::getMainWindow();
  bool haveZoomOnly = mainWindow->isToolBarShown(FuiMainWindow::VIEWCTRL1);
  mainWindow->showToolBar(FuiMainWindow::VIEWCTRL1, false);
  if (haveZoomOnly)
    mainWindow->showToolBar(FuiMainWindow::VIEWCTRL, true);
  else if (!mainWindow->isToolBarShown(FuiMainWindow::VIEWCTRL))
    mainWindow->showToolBar(FuiMainWindow::VIEWCTRL2, true);
}
//----------------------------------------------------------------------------

bool FuiGraphViewTLS::onClose()
{
  this->invokeFinishedCB();

  if (--numInstance < 1)
  {
    FuiMainWindow* mainWindow = Fui::getMainWindow();
    bool haveZoomAndPan = mainWindow->isToolBarShown(FuiMainWindow::VIEWCTRL);
    mainWindow->showToolBar(FuiMainWindow::VIEWCTRL, false);
    mainWindow->showToolBar(FuiMainWindow::VIEWCTRL2, false);
    if (haveZoomAndPan)
      mainWindow->showToolBar(FuiMainWindow::VIEWCTRL1, true);
  }

  return false;
}
