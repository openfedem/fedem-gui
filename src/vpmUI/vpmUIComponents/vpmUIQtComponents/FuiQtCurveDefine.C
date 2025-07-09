// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QSizePolicy>
#include <QHBoxLayout>

#include "FuiQtCurveAxisDefinition.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtScaleShiftWidget.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtSNCurveSelector.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtTable.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtFileBrowseField.H"
#include "FFuLib/FFuQtComponents/FFuQtScrolledListDialog.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtTabbedWidgetStack.H"
#include "FFuLib/FFuQtComponents/FFuQtSpinBox.H"
#include "FFuLib/FFuQtComponents/FFuQtRadioButton.H"
#include "FFuLib/FFuQtComponents/FFuQtColorComboBox.H"
#include "FFuLib/FFuQtComponents/FFuQtColorDialog.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtCurveDefine.H"

//----------------------------------------------------------------------------

FuiQtCurveDefine::FuiQtCurveDefine(QWidget* parent, const char* name)
  : FFuQtMultUIComponent(parent,name)
{
  this->legendField = new FFuQtLabelField(this);
  this->legendButton = new FFuQtToggleButton(this);

  this->tabStack = new FFuQtTabbedWidgetStack(this);
  this->curveDefSheet = new FuiQtCurveDefSheet(this);
  this->curveAnalysisSheet = new FuiQtCurveAnalysisSheet(this);
  this->curveScaleSheet = new FuiQtCurveScaleSheet(this);
  this->appearanceSheet = new FuiQtCurveAppearanceSheet(this);
  this->infoSheet = new FuiQtCurveInfoSheet(this);
  this->fatigueSheet = new FuiQtCurveFatigueSheet(this);

  this->FuiCurveDefine::initWidgets();
}


FuiQtCurveDefSheet::FuiQtCurveDefSheet(QWidget* parent, const char* name)
  : FFuQtMultUIComponent(parent, name)
{
  this->sourceFrame = new FFuQtLabelFrame(this);
  this->rdbResultPlotRadio = new FFuQtRadioButton(this);
  this->combCurvePlotRadio = new FFuQtRadioButton(this);
  this->extCurvePlotRadio = new FFuQtRadioButton(this);
  this->intFunctionPlotRadio = new FFuQtRadioButton(this);
  for (FFuLabelFrame*& frame : this->frames)
    frame = new FFuQtLabelFrame(this);
  for (FuiCurveAxisDefinition*& axis : axes)
    axis = new FuiQtCurveAxisDefinition(this);
  this->definitionFrame = new FFuQtLabelFrame(this);
  this->exprField = new FFuQtLabelField(this);
  this->compTable = new FFuQtTable(this);
  this->fileBrowseField = new FFuQtFileBrowseField(this);
  this->reloadCurveButton = new FFuQtPushButton(this);
  this->channelField = new FFuQtLabelField(this);
  this->channelBtn = new FFuQtPushButton(this);
  this->channelSelectUI = new FFuQtScrolledListDialog(this,true);
  this->startXField = new FFuQtLabelField(this);
  this->stopXField = new FFuQtLabelField(this);
  this->incXField = new FFuQtLabelField(this);
  this->functionLabel = new FFuQtLabel(this);
  this->functionMenu = new FuiQtQueryInputField(this);
  this->useSmartPointsBtn = new FFuQtToggleButton(this);

  this->timeRange = new FuiQtCurveTimeRange(this);
  this->spaceOper = new FFuQtOptionMenu(this);
  this->spaceObj = new FFuQtLabel(this);

  this->completeLabel = new FFuQtLabel(this);
  this->autoExportToggle = new FFuQtToggleButton(this);

  this->FuiCurveDefSheet::initWidgets();
}


void FuiQtCurveDefSheet::setNoComps(unsigned int nc)
{
  QWidget* qPtr = dynamic_cast<QWidget*>(this->compTable);

  unsigned int oc = this->curveComps.size();
  this->curveComps.resize(nc,NULL);

  for (unsigned int i = oc; i < nc; i++)
    this->curveComps[i] = new FuiQtQueryInputField(qPtr);
}


FuiQtCurveAnalysisSheet::FuiQtCurveAnalysisSheet(QWidget* parent, const char* name)
  : FFuQtMultUIComponent(parent, name)
{
  this->dftFrame = new FFuQtLabelFrame(this);
  this->doDftBtn = new FFuQtToggleButton(this);

  this->dftDomainFrame = new FFuQtLabelFrame(this);
  this->startField = new FFuQtLabelField(this);
  this->endField = new FFuQtLabelField(this);
  this->entireDomainBtn = new FFuQtToggleButton(this);

  this->removeCompBtn = new FFuQtToggleButton(this);
  this->resampleBtn = new FFuQtToggleButton(this);
  this->resampleRateField = new FFuQtIOField(this);

  this->diffBtn = new FFuQtToggleButton(this);
  this->intBtn = new FFuQtToggleButton(this);

  this->FuiCurveAnalysisSheet::initWidgets();
}


FuiQtCurveAppearanceSheet::FuiQtCurveAppearanceSheet(QWidget* parent, const char* name)
  : FFuQtMultUIComponent(parent, name)
{
  this->genAppearanceFrame = new FFuQtLabelFrame(this);
  this->curveTypeLabel = new FFuQtLabel(this);
  this->curveTypeMenu = new FFuQtOptionMenu(this);
  this->curveWidthLabel = new FFuQtLabel(this);
  this->curveWidthBox = new FFuQtSpinBox(this);
  this->colorLabel = new FFuQtLabel(this);
  this->colorChooser = new FFuQtColorComboBox(this);
  this->colorDialog =  new FFuQtColorDialog();

  this->symbolFrame = new FFuQtLabelFrame(this);
  this->curveSymbolLabel = new FFuQtLabel(this);
  this->curveSymbolMenu = new FFuQtOptionMenu(this);
  this->symbolSizeLabel = new FFuQtLabel(this);
  this->symbolSizeBox = new FFuQtSpinBox(this);
  this->numSymbolsLabel = new FFuQtLabel(this);
  this->numSymbolsBox = new FFuQtSpinBox(this);
  this->allSymbolsButton = new FFuQtToggleButton(this);

  this->FuiCurveAppearanceSheet::initWidgets();
}

FuiQtCurveAppearanceSheet::~FuiQtCurveAppearanceSheet()
{
  delete this->colorDialog;
}


FuiQtCurveInfoSheet::FuiQtCurveInfoSheet(QWidget* parent, const char* name)
  : FFuQtMultUIComponent(parent, name)
{
  this->rmsField = new FFuQtLabelField(this);
  this->avgField = new FFuQtLabelField(this);
  this->stdDevField = new FFuQtLabelField(this);
  this->integralField = new FFuQtLabelField(this);
  this->maxField = new FFuQtLabelField(this);
  this->minField = new FFuQtLabelField(this);

  this->calculateBtn = new FFuQtPushButton(this);
  this->useScaleShiftBtn = new FFuQtToggleButton(this);

  this->domainFrame = new FFuQtLabelFrame(this);
  this->startField = new FFuQtLabelField(this);
  this->stopField = new FFuQtLabelField(this);
  this->entireDomainBtn = new FFuQtToggleButton(this);

  this->FuiCurveInfoSheet::initWidgets();
}


FuiQtCurveScaleSheet::FuiQtCurveScaleSheet(QWidget* parent, const char* name)
: FFuQtMultUIComponent(parent, name)
{
  this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  FuiQtScaleShiftWidget *x, *y;
  XScale = x = new FuiQtScaleShiftWidget(this, "XScale");
  YScale = y = new FuiQtScaleShiftWidget(this, "YScale");

  QHBoxLayout* aLayout = new QHBoxLayout(this);
  aLayout->setContentsMargins(3, 3, 3, 3);
  aLayout->addWidget(x);
  aLayout->addSpacing(3);
  aLayout->addWidget(y);

  aLayout->setStretchFactor(x, 1);
  aLayout->setStretchFactor(y, 1);

  this->FuiCurveScaleSheet::initWidgets();
}


FuiQtCurveFatigueSheet::FuiQtCurveFatigueSheet(QWidget* parent, const char* name)
  : FFuQtMultUIComponent(parent, name)
{
  this->snSelector = new FuiQtSNCurveSelector(this);
  this->gateValueField = new FFuQtLabelField(this);

  this->resultFrame = new FFuQtLabelFrame(this);
  this->damageField = new FFuQtLabelField(this);
  this->lifeField = new FFuQtLabelField(this);
  this->unitLabel = new FFuQtLabel(this);
  this->unitTypeMenu = new FFuQtOptionMenu(this);
  this->calculateBtn = new FFuQtPushButton(this);

  this->doRainflowBtn = new FFuQtToggleButton(this);

  this->domainFrame = new FFuQtLabelFrame(this);
  this->startField = new FFuQtLabelField(this);
  this->stopField = new FFuQtLabelField(this);
  this->entireDomainBtn = new FFuQtToggleButton(this);

  this->FuiCurveFatigueSheet::initWidgets();
}
