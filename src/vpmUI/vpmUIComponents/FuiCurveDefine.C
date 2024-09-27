// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiCurveDefine.H"
#include "vpmUI/vpmUIComponents/FuiCurveAxisDefinition.H"
#include "vpmUI/vpmUIComponents/FuiCurveTimeRange.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "vpmUI/vpmUIComponents/FuiScaleShiftWidget.H"
#include "vpmUI/vpmUIComponents/FuiSNCurveSelector.H"
#include "vpmUI/Fui.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuTable.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuColorChooser.H"
#include "FFuLib/FFuColorDialog.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuRadioButton.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuFileBrowseField.H"
#include "FFuLib/FFuScrolledList.H"
#include "FFuLib/FFuScrolledListDialog.H"
#include "FFuLib/FFuTabbedWidgetStack.H"
#include "FFuLib/FFuSpinBox.H"
#include "vpmDB/FmGraph.H"
#include "vpmDB/FmColor.H"

////////////////////////////////////////////////////////////////////////////////

FuiCurveDefine::FuiCurveDefine()
{
  this->legendField = NULL;
  this->legendButton = NULL;

  this->tabStack = NULL;
  this->curveDefSheet = NULL;
  this->curveAnalysisSheet = NULL;
  this->curveScaleSheet = NULL;
  this->appearanceSheet = NULL;
  this->infoSheet = NULL;
  this->fatigueSheet = NULL;

  this->selectedTab = 0;
}

//----------------------------------------------------------------------------

void FuiCurveDefine::initWidgets()
{
  this->legendButton->setToggleCB(FFaDynCB1M(FuiCurveDefine,this,
					     onLegendButtonToggled,bool));
  this->legendField->myField->setAcceptedCB(FFaDynCB1M(FuiCurveDefine,this,
						       onFieldValueChanged,char*));

  this->legendField->setLabel("Legend");
  this->legendButton->setLabel("Auto legend");

  this->tabStack->addTabPage(this->curveDefSheet, "Data");
  this->tabStack->addTabPage(this->appearanceSheet, "Appearance");
  this->tabStack->addTabPage(this->infoSheet, "Curve Statistics");
  this->tabStack->addTabPage(this->curveScaleSheet, "Scale and Shift");
  this->tabStack->addTabPage(this->curveAnalysisSheet, "Analysis");
  this->tabStack->addTabPage(this->fatigueSheet, "Fatigue");

  this->curveAnalysisSheet->rainflowBtn = this->fatigueSheet->doRainflowBtn;

  this->tabStack->setTabSelectedCB(FFaDynCB1M(FuiCurveFatigueSheet,this->fatigueSheet,
					      onTabSelected,FFuComponentBase*));

  this->curveDefSheet->setDataChangedCB(FFaDynCB0M(FuiCurveDefine, this, onValuesChanged));
  this->appearanceSheet->setDataChangedCB(FFaDynCB0M(FuiCurveDefine, this, onValuesChanged));
  this->curveScaleSheet->setDataChangedCB(FFaDynCB0M(FuiCurveDefine, this, onValuesChanged));
  this->curveAnalysisSheet->setDataChangedCB(FFaDynCB0M(FuiCurveDefine, this, onValuesChanged));
  this->fatigueSheet->setDataChangedCB(FFaDynCB0M(FuiCurveDefine, this, onValuesChanged));

  // create ui's UA object
  FFuUAExistenceHandler::invokeCreateUACB(this);
}

//----------------------------------------------------------------------------

void FuiCurveDefine::setGetCurveStatisticsCB(const FFaDynCB4<bool,bool,double,double>& cb)
{
  this->infoSheet->setGetCurveStatisticsCB(cb);
}

void FuiCurveDefine::setApplyVerticalMarkersCB(const FFaDynCB2<double,double>& aCB)
{
  this->infoSheet->setApplyVerticalMarkersCB(aCB);
}

void FuiCurveDefine::setRemoveVerticalMarkersCB(const FFaDynCB0& aCB)
{
  this->infoSheet->setRemoveVerticalMarkersCB(aCB);
}

void FuiCurveDefine::setCurveStatistics(double rms, double avg, double stdDev,
					double integral, double min, double max)
{
  this->infoSheet->setCurveStatistics(rms, avg, stdDev, integral, min, max);
}

//----------------------------------------------------------------------------

void FuiCurveDefine::setGetDamageFromCurveCB(const FFaDynCB4<bool,bool,double,double>& cb)
{
  this->fatigueSheet->setCalculateCurveDamageCB(cb);
}

void FuiCurveDefine::setDamageFromCurve(double damage, double interval)
{
  this->fatigueSheet->setCurveDamage(damage, interval);
}

//----------------------------------------------------------------------------

void FuiCurveDefine::onValuesChanged()
{
  this->updateDBValues();
}

//----------------------------------------------------------------------------

void FuiCurveDefine::setCompleteSign(bool complete)
{
  this->curveDefSheet->setCompleteSign(complete);
}

//----------------------------------------------------------------------------

void FuiCurveDefine::setLegend(const std::string& legend)
{
  this->legendField->setValue(legend.c_str());
}

//----------------------------------------------------------------------------

void FuiCurveDefine::setChannelList(const std::vector<std::string>& channels)
{
  this->curveDefSheet->setChannelList(channels);
}

//----------------------------------------------------------------------------

FmModelMemberBase* FuiCurveDefine::getSelectedFunction() const
{
  return this->curveDefSheet->getSelectedFunction();
}

//----------------------------------------------------------------------------

FuiCurveAxisDefinition* FuiCurveDefine::getAxisUI(int dir) const
{
  return this->curveDefSheet->getAxisUI(dir);
}

//----------------------------------------------------------------------------

const char* FuiCurveDefine::getCurrentTabName() const
{
  if (!tabStack) return NULL;

  static std::string tabName;
  tabName = tabStack->getCurrentTabName();
  return tabName.c_str();
}

//----------------------------------------------------------------------------

void FuiCurveDefine::placeWidgets(int width, int height)
{
  int fieldHeight = 3*height/25;
  int left = 0;
  int right = width;
  int top = 0;
  int bottom = height;
  int legendCenterY = (fieldHeight + top)/2;
  int legendBtnLeft = right - this->legendButton->getWidthHint();
  int legendFieldWidth = legendBtnLeft - 7 - left;
  int separator = 3;

  this->legendField->setCenterYGeometry(left, legendCenterY,
					legendFieldWidth, fieldHeight),

  this->legendButton->setCenterYGeometry(legendBtnLeft, legendCenterY,
					 this->legendButton->getWidthHint(),
					 this->legendButton->getHeightHint());

  this->tabStack->setEdgeGeometry(left, right, top + fieldHeight + separator, bottom);
}

//----------------------------------------------------------------------------

FFuaUIValues* FuiCurveDefine::createValuesObject()
{
  return new FuaCurveDefineValues();
}

//----------------------------------------------------------------------------

void FuiCurveDefine::setUIValues(const FFuaUIValues* values)
{
  const FuaCurveDefineValues* v = (const FuaCurveDefineValues*)values;

  this->legendField->setValue(v->legend.c_str());
  this->legendField->setSensitivity(!v->autoLegend);
  this->legendButton->setValue(v->autoLegend);

  this->curveDefSheet->setUIValues(v);
  this->appearanceSheet->setUIValues(v);
  this->infoSheet->setUIValues(v);
  this->curveScaleSheet->setUIValues(v);

  this->tabStack->popDown();
  std::string tmpSel = this->tabStack->getCurrentTabName();
  this->tabStack->removeTabPage(this->curveAnalysisSheet);
  this->tabStack->removeTabPage(this->fatigueSheet);

  bool haveAnalysisTab = false;
  bool haveFatigueTab = false;
  switch (v->inputMode) {
  case FuiCurveDefSheet::TEMPORAL_RESULT:
  case FuiCurveDefSheet::COMB_CURVE:
  case FuiCurveDefSheet::EXT_CURVE:
    haveFatigueTab = true;
  case FuiCurveDefSheet::INT_FUNCTION:
  case FuiCurveDefSheet::PREVIEW_FUNCTION:
    haveAnalysisTab = true;
  }
  if (haveAnalysisTab) {
    this->tabStack->addTabPage(this->curveAnalysisSheet, "Fourier Analysis and Differentiation");
    this->curveAnalysisSheet->setUIValues(v);
  }
  if (haveFatigueTab) {
    // Recalculate curve damage only if the Fatigue sheet is visible
    this->tabStack->addTabPage(this->fatigueSheet, "Rainflow and Fatigue");
    this->fatigueSheet->setUIValues(v, tmpSel=="Rainflow and Fatigue");
  }

  this->tabStack->popUp();
  this->tabStack->setWidth(this->tabStack->getWidth()+1);

  this->tabStack->setCurrentTab(tmpSel);
  this->placeWidgets(this->getWidth(), this->getHeight());
}

//----------------------------------------------------------------------------

void FuiCurveDefine::getUIValues(FFuaUIValues* values)
{
  FuaCurveDefineValues* v = (FuaCurveDefineValues*)values;

  v->selectedTabIdx = this->tabStack->getCurrentTabPosIdx();

  v->legend = this->legendField->getText();
  v->autoLegend = this->legendButton->getValue();

  this->curveDefSheet->getUIValues(v);
  this->appearanceSheet->getUIValues(v);
  this->curveScaleSheet->getUIValues(v);
  switch (v->inputMode) {
  case FuiCurveDefSheet::TEMPORAL_RESULT:
  case FuiCurveDefSheet::COMB_CURVE:
  case FuiCurveDefSheet::EXT_CURVE:
    this->fatigueSheet->getUIValues(v);
  case FuiCurveDefSheet::INT_FUNCTION:
  case FuiCurveDefSheet::PREVIEW_FUNCTION:
    this->curveAnalysisSheet->getUIValues(v);
  }
}

//----------------------------------------------------------------------------

void FuiCurveDefine::buildDynamicWidgets(const FFuaUIValues* values)
{
  this->curveDefSheet->buildDynamicWidgets((const FuaCurveDefineValues*)values);
  this->fatigueSheet->buildDynamicWidgets((const FuaCurveDefineValues*)values);
}

//----------------------------------------------------------------------------

void FuiCurveDefine::setAutoLegendCB(const FFaDynCB1<bool>& dynCB)
{
  this->autoLegendCB = dynCB;
}

void FuiCurveDefine::setProvideChannelListCB(const FFaDynCB1<const std::string&>& dynCB)
{
  this->curveDefSheet->setProvideChannelsCB(dynCB);
}

void FuiCurveDefine::setReloadCurveCB(const FFaDynCB0& dynCB)
{
  this->curveDefSheet->setReloadCurveCB(dynCB);
}

//-----------------------------------------------------------------------------

void FuiCurveDefine::onFieldValueChanged(char*)
{
  this->updateDBValues(false);
}

//----------------------------------------------------------------------------

void FuiCurveDefine::onLegendButtonToggled(bool toggle)
{
  this->autoLegendCB.invoke(toggle);
  this->updateDBValues();
}

//----------------------------------------------------------------------------

void FuiCurveDefine::onPoppedUpFromMem()
{
  this->tabStack->setCurrentTab(selectedTab);
}

void FuiCurveDefine::onPoppedDownToMem()
{
  this->selectedTab = this->tabStack->getCurrentTabPosIdx();
}


////////////////////////////////////////////////////////////////////////////////

FuiCurveAppearanceSheet::FuiCurveAppearanceSheet()
{
  this->genAppearanceFrame = 0;
  this->curveTypeLabel = 0;
  this->curveTypeMenu = 0;
  this->curveWidthLabel = 0;
  this->curveWidthBox = 0;
  this->colorLabel = 0;
  this->colorChooser = 0;
  this->colorDialog = 0;

  this->symbolFrame = 0;
  this->curveSymbolLabel = 0;
  this->curveSymbolMenu = 0;
  this->symbolSizeLabel = 0;
  this->symbolSizeBox = 0;
  this->numSymbolsLabel = 0;
  this->numSymbolsBox = 0;
  this->allSymbolsButton = 0;
}

//----------------------------------------------------------------------------

void FuiCurveAppearanceSheet::initWidgets()
{
  this->curveTypeMenu->setOptionSelectedCB(FFaDynCB1M(FuiCurveAppearanceSheet,this,
						      onIntValueChanged,int));

  this->curveWidthBox->setMinMax(0, 4);
  this->curveWidthBox->setIntValue(0);
  this->curveWidthBox->setValueChangedCB(FFaDynCB1M(FuiCurveAppearanceSheet,this,
						    onIntValueChanged,int));

  this->colorDialog->setOkButtonClickedCB(FFaDynCB2M(FuiCurveAppearanceSheet,this,
						     onColorDialogOk,FFuColor,FFuComponentBase*));
  this->colorDialog->setCancelButtonClickedCB(FFaDynCB2M(FuiCurveAppearanceSheet,this,
							 onColorDialogCancel,FFuColor,FFuComponentBase*));
  this->colorChooser->setSelectionChangedCB(FFaDynCB2M(FuiCurveAppearanceSheet,this,
						       onColorChanged,FFuColor,FFuComponentBase*));

  this->curveSymbolMenu->setOptionSelectedCB(FFaDynCB1M(FuiCurveAppearanceSheet,this,
							onIntValueChanged,int));

  this->symbolSizeBox->setMinMax(1, 25);
  this->symbolSizeBox->setValueChangedCB(FFaDynCB1M(FuiCurveAppearanceSheet,this,
						    onIntValueChanged,int));

  this->numSymbolsBox->setMinMax(0, 100000);
  this->numSymbolsBox->setIntValue(10);
  this->numSymbolsBox->setStepSize(10);
  this->numSymbolsBox->setValueChangedCB(FFaDynCB1M(FuiCurveAppearanceSheet,this,
						    onIntValueChanged,int));

  this->allSymbolsButton->setLabel("All");
  this->allSymbolsButton->setToggleCB(FFaDynCB1M(FuiCurveAppearanceSheet,this,
						 onAllSymbolsToggled,bool));

  this->curveTypeLabel->setLabel("Curve type");
  this->curveWidthLabel->setLabel("Curve thickness");
  this->colorLabel->setLabel("Curve color");
  this->curveSymbolLabel->setLabel("Symbol type");
  this->symbolSizeLabel->setLabel("Symbol size");
  this->numSymbolsLabel->setLabel("Number of symbols");

  for (const FmCurveColor& color : FmGraph::getCurveDefaultColors())
    this->colorChooser->insertCol(color.first,color.second);
  this->colorChooser->enableColorDialog(true);

  this->curveTypeMenu->addOption("Lines");   // 0 refer FFu2DPlotter enum
  this->curveTypeMenu->addOption("Dots");    // 1
  this->curveTypeMenu->addOption("Invisible");// 2

  this->curveSymbolMenu->addOption("None");   // 0 refer FFu2DPlotter enum
  this->curveSymbolMenu->addOption("+ Cross");// 1
  this->curveSymbolMenu->addOption("X Cross");// 2
  this->curveSymbolMenu->addOption("Circle");// 3
  this->curveSymbolMenu->addOption("Diamond");// 4
  this->curveSymbolMenu->addOption("Rectangle");// 5
  this->curveSymbolMenu->addOption("Up triangle");// 6
  this->curveSymbolMenu->addOption("Down triangle");// 7
  this->curveSymbolMenu->addOption("Left triangle");// 8
  this->curveSymbolMenu->addOption("Right triangle");// 9

  this->symbolFrame->setLabel("Symbols");
  this->genAppearanceFrame->setLabel("General appearance");

  // These features are (not yet) supported with Qwt 6.1.2, so hide the widgets in the GUI
  this->numSymbolsLabel->popDown();
  this->numSymbolsBox->popDown();
  this->allSymbolsButton->popDown();
}

//----------------------------------------------------------------------------

void FuiCurveAppearanceSheet::placeWidgets(int width, int height)
{
  int border = this->getBorder();
  int left = border;
  int right = width - border;
  int top = border;
  int bottom = height - border;
  int fieldHeight = 4*height/25;
  int textHeight = this->colorLabel->getHeightHint();
  int centerX = width/2;

  int frameTop = top;
  int frameBottom = bottom;
  int frame1Left = left;
  int frame1Right = centerX - border/2;
  int frame2Left = centerX + border/2;
  int frame2Right = right;

  int frameHeightAvail = frameBottom - frameTop - border;
  int vertSpacing = frameHeightAvail/4;

  int line1 = frameTop + border + vertSpacing;
  int line2 = line1 + vertSpacing;
  int line3 = line2 + vertSpacing;

  int frame1CenterX = frame1Left + (frame1Right - frame1Left)/2;
  int frame2CenterX = frame2Left + (frame2Right - frame2Left)/2;

  int col1Left = frame1Left + border;
  int col2Left = frame1CenterX;
  int col2Right = frame1Right - border;
  int col2Width = col2Right - col2Left;
  int col3Left = frame2Left + border;
  int col4Left = frame2CenterX;
  int col4Right = frame2Right - border;
  int col4Width = col4Right - col4Left;

  int allSymbolsWidth = this->allSymbolsButton->getWidthHint();
  int allSymbolsLeft = col4Right - allSymbolsWidth;

  this->genAppearanceFrame->setEdgeGeometry(frame1Left, frame1Right, frameTop, frameBottom);

  this->curveTypeLabel->setCenterYGeometryWidthHint(col1Left, line1, textHeight);
  this->curveTypeMenu->setCenterYGeometry(col2Left, line1, col2Width, fieldHeight);

  this->curveWidthLabel->setCenterYGeometryWidthHint(col1Left, line2, textHeight);
  this->curveWidthBox->setCenterYGeometry(col2Left, line2, col2Width, fieldHeight);

  this->colorLabel->setCenterYGeometryWidthHint(col1Left, line3, textHeight);
  this->colorChooser->setCenterYGeometry(col2Left, line3, col2Width, fieldHeight);

  this->symbolFrame->setEdgeGeometry(frame2Left, frame2Right, frameTop, frameBottom);

  this->curveSymbolLabel->setCenterYGeometryWidthHint(col3Left, line1, textHeight);
  this->curveSymbolMenu->setCenterYGeometry(col4Left, line1, col4Width, fieldHeight);

  this->symbolSizeLabel->setCenterYGeometryWidthHint(col3Left, line2, textHeight);
  this->symbolSizeBox->setCenterYGeometry(col4Left, line2, col4Width, fieldHeight);

  this->numSymbolsLabel->setCenterYGeometryWidthHint(col3Left, line3, textHeight);
  this->numSymbolsBox->setCenterYGeometry(col4Left, line3, col4Width - allSymbolsWidth - border, fieldHeight);
  this->allSymbolsButton->setCenterYGeometry(allSymbolsLeft, line3, allSymbolsWidth, fieldHeight);
}

//-----------------------------------------------------------------------------

void FuiCurveAppearanceSheet::onPoppedDownToMem()
{
  this->colorDialog->popDown();
}

//-----------------------------------------------------------------------------

void FuiCurveAppearanceSheet::setUIValues(const FuaCurveDefineValues* curveValues)
{
  this->colorDialog->popDown();

  this->curveTypeMenu->selectOption(curveValues->curveType);
  this->curveWidthBox->setIntValue(curveValues->curveWidth);
  this->curveSymbolMenu->selectOption(curveValues->curveSymbol);
  this->symbolSizeBox->setIntValue(curveValues->symbolSize);

  if (curveValues->numSymbols == -1) {
    this->allSymbolsButton->setValue(true);
    this->numSymbolsBox->setSensitivity(false);
  }
  else {
    this->allSymbolsButton->setValue(false);
    this->numSymbolsBox->setIntValue(curveValues->numSymbols);
    this->numSymbolsBox->setSensitivity(true);
  }

  this->colorChooser->setCurrentCol(curveValues->color);
}

//----------------------------------------------------------------------------

void FuiCurveAppearanceSheet::getUIValues(FuaCurveDefineValues* curveValues) const
{
  curveValues->curveType = this->curveTypeMenu->getSelectedOption();
  curveValues->curveWidth = this->curveWidthBox->getIntValue();
  curveValues->curveSymbol = this->curveSymbolMenu->getSelectedOption();
  curveValues->symbolSize = this->symbolSizeBox->getIntValue();

  if (allSymbolsButton->getToggle())
    curveValues->numSymbols = -1;
  else
    curveValues->numSymbols = this->numSymbolsBox->getIntValue();

  curveValues->color = this->colorChooser->getCurrentCol();
}

//----------------------------------------------------------------------------

void FuiCurveAppearanceSheet::onAllSymbolsToggled(bool)
{
  this->dataChangedCB.invoke();
}

//----------------------------------------------------------------------------

void FuiCurveAppearanceSheet::onIntValueChanged(int)
{
  this->dataChangedCB.invoke();
}

//----------------------------------------------------------------------------

void FuiCurveAppearanceSheet::onColorDialogOk(FFuColor, FFuComponentBase*)
{
  this->colorDialog->popDown();
  this->dataChangedCB.invoke();
}

//----------------------------------------------------------------------------

void FuiCurveAppearanceSheet::onColorDialogCancel(FFuColor, FFuComponentBase*)
{
  this->colorDialog->popDown();
}

//----------------------------------------------------------------------------

void FuiCurveAppearanceSheet::onColorChanged(FFuColor, FFuComponentBase*)
{
  this->dataChangedCB.invoke();
}


////////////////////////////////////////////////////////////////////////////////

FuiCurveDefSheet::FuiCurveDefSheet()
{
  frames.fill(NULL);
  axes.fill(NULL);

  sourceFrame          = 0;
  rdbResultPlotRadio   = 0;
  combCurvePlotRadio   = 0;
  extCurvePlotRadio    = 0;
  intFunctionPlotRadio = 0;

  definitionFrame   = 0;
  exprField         = 0;
  compTable         = 0;
  fileBrowseField   = 0;
  reloadCurveButton = 0;
  channelField      = 0;
  channelBtn        = 0;
  channelSelectUI   = 0;
  functionLabel     = 0;
  functionMenu      = 0;
  startXField       = 0;
  stopXField        = 0;
  incXField         = 0;
  useSmartPointsBtn = 0;

  completeLabel     = 0;
  autoExportToggle  = 0;

  timeRange = 0;
  spaceObj  = 0;
  spaceOper = 0;

  isFuncPreview = isSpatial = isCompCurve = false;
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::setDataChangedCB(const FFaDynCB0& dynCB)
{
  this->timeRange->setChangeCB(dynCB);
  this->dataChangedCB = dynCB;
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::setReloadCurveCB(const FFaDynCB0& dynCB)
{
  this->reloadCurveCB = dynCB;
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::setUIValues(const FuaCurveDefineValues* v)
{
  int aInputMode = abs(v->inputMode);
  this->setDefineMode(aInputMode);

  this->isCompCurve = aInputMode == COMB_CURVE;
  this->isSpatial = v->inputMode == SPATIAL_RESULT || v->inputMode < 0;
  this->timeRange->setUIValues(v->timeRange.first,v->timeRange.second,v->timeOper);
  this->spaceOper->selectOption(v->spaceOper,false);
  if (aInputMode == SPATIAL_RESULT)
  {
    std::string domain("Spatial domain: ");
    if (v->firstTriad && v->secondTriad)
      domain += v->firstTriad->getIdString() + " - " + v->secondTriad->getIdString();
    this->spaceObj->setLabel(domain);
  }

  this->setCompleteSign(v->axesComplete, aInputMode == PREVIEW_FUNCTION);

  if (v->manualReload) {
    this->reloadCurveButton->popUp();
    this->reloadCurveButton->setSensitivity(v->axesComplete);
  }
  else
    this->reloadCurveButton->popDown();

  this->autoExportToggle->setValue(v->autoExport && aInputMode <= COMB_CURVE);
  this->autoExportToggle->setSensitivity(aInputMode <= COMB_CURVE);

  this->exprField->setValue(v->expression);

  for (size_t i = 0; i < this->curveComps.size(); i++)
    if (i < v->curveComps.size()) {
      this->curveComps[i]->setSelectedRef(v->curveComps[i]);
      this->curveComps[i]->setSensitivity(v->activeComps[i]);
    }
    else {
      this->curveComps[i]->setSelectedRef(0);
      this->curveComps[i]->setSensitivity(true);
    }

  if (v->curveComps.empty())
    this->compTable->popDown();

  this->fileBrowseField->setAbsToRelPath(v->modelFilePath);
  this->fileBrowseField->setFileName(v->filePath);
  this->channelField->setValue(v->channel);
  this->channelBtn->setSensitivity(v->isMultiChannelFile);

  this->functionMenu->setSelectedRef(v->functionRef);
  this->startXField->setValue(v->functionDomain.X.first);
  this->stopXField->setValue(v->functionDomain.X.second);
  this->incXField->setValue(v->functionDomain.dX);
  this->useSmartPointsBtn->setValue(v->functionDomain.autoInc > 0);

  switch (aInputMode) {
  case TEMPORAL_RESULT:
  case SPATIAL_RESULT:
    this->rdbResultPlotRadio->setValue(true);
    break;
  case COMB_CURVE:
    this->combCurvePlotRadio->setValue(true);
    break;
  case EXT_CURVE:
    this->extCurvePlotRadio->setValue(true);
    break;
  default:
    this->intFunctionPlotRadio->setValue(true);
  }

  if (aInputMode == PREVIEW_FUNCTION) {
    // Block the change of data source for function preview curves (TT #2656)
    this->inputGroup.setSensitivity(false);
    this->functionMenu->setSensitivity(false);
    this->isFuncPreview = true;
  }
  else {
    this->inputGroup.setSensitivity(true);
    this->functionMenu->setSensitivity(true);
    this->isFuncPreview = aInputMode == INT_FUNCTION ? 2 : 0;
  }

  if (v->functionDomain.autoInc >= 0) {
    this->incXField->setSensitivity(!this->useSmartPointsBtn->getValue());
    this->useSmartPointsBtn->setSensitivity(true);
  }
  else {
    this->incXField->setSensitivity(true);
    this->useSmartPointsBtn->setSensitivity(false);
  }
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::getUIValues(FuaCurveDefineValues* v) const
{
  if (rdbResultPlotRadio->getValue())
    v->inputMode = this->isSpatial ? SPATIAL_RESULT : TEMPORAL_RESULT;
  else if (combCurvePlotRadio->getValue())
    v->inputMode = COMB_CURVE;
  else if (extCurvePlotRadio->getValue())
    v->inputMode = EXT_CURVE;
  else
    v->inputMode = this->isFuncPreview == 1 ? PREVIEW_FUNCTION : INT_FUNCTION;

  v->expression = this->exprField->getText();
  v->curveComps.clear();
  for (FuiQueryInputField* ccomp : this->curveComps)
    v->curveComps.push_back(ccomp->getSelectedRef());

  v->filePath = this->fileBrowseField->getFileName();
  v->channel = this->channelField->getText();
  v->functionRef = this->functionMenu->getSelectedRef();
  v->functionDomain = FuaFunctionDomain(this->startXField->getValue(),
                                        this->stopXField->getValue(),
                                        this->incXField->getValue(),
                                        this->useSmartPointsBtn->getValue());
  v->autoExport = this->autoExportToggle->getValue() && this->autoExportToggle->getSensitivity();

  this->timeRange->getUIValues(v->timeRange.first,v->timeRange.second,v->timeOper);
  v->spaceOper = this->spaceOper->getSelectedOptionStr();
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::buildDynamicWidgets(const FuaCurveDefineValues* v)
{
  this->functionMenu->setQuery(v->functionQuery);

  // Don't touch if no curve components at all
  int numComp = v->curveComps.size();
  if (numComp == 0) return;

  int numCols = this->compTable->getNumberColumns();
  int numRows = this->compTable->getNumberRows();
  for (int row = numComp; row < numRows; row++)
    for (int col = 0; col < numCols; col++)
      this->compTable->clearCellContents(row, col);

  this->setNoComps(numComp);
  this->compTable->setNumberRows(numComp);

  std::string label("A:");
  for (int i = 0; i < numComp; i++) {
    this->curveComps[i]->turnButtonOff(true);
    this->curveComps[i]->setBehaviour(FuiQueryInputField::REF_NONE);
    this->curveComps[i]->setRefSelectedCB(FFaDynCB1M(FuiCurveDefSheet,this,onRefSelected,int));
    this->curveComps[i]->setQuery(v->curveQuery);
    if (i >= numRows)
      this->compTable->insertWidget(i, 0, this->curveComps[i]);
    this->compTable->setRowLabel(i, label.c_str());
    this->compTable->stretchRowHeight(i, true);
    label[0]++;
  }
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::setChannelList(const std::vector<std::string>& channels)
{
  this->channelSelectUI->myItemSelector->deleteAllItems();
  this->channelSelectUI->myItemSelector->setItems(channels);
  this->channelSelectUI->myItemSelector->selectItem(0, false);
}

//----------------------------------------------------------------------------

FmModelMemberBase* FuiCurveDefSheet::getSelectedFunction() const
{
  return this->functionMenu->getSelectedRef();
}

//----------------------------------------------------------------------------

FuiCurveAxisDefinition* FuiCurveDefSheet::getAxisUI(int dir) const
{
  return axes[dir];
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::setProvideChannelsCB(const FFaDynCB1<const std::string&>& dynCB)
{
  this->channelBtnClickedCB = dynCB;
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::initWidgets()
{
  this->sourceFrame->setLabel("Source");
  this->rdbResultPlotRadio->setLabel("From RDB");
  this->combCurvePlotRadio->setLabel("Combined curve");
  this->extCurvePlotRadio->setLabel("From file");
  this->intFunctionPlotRadio->setLabel("Internal function");
  inputGroup.insert(rdbResultPlotRadio);
  inputGroup.insert(combCurvePlotRadio);
  inputGroup.insert(extCurvePlotRadio);
#ifdef FT_HAS_PREVIEW
  inputGroup.insert(intFunctionPlotRadio);
#else
  this->intFunctionPlotRadio->popDown();
#endif
  inputGroup.setExclusive(true);
  inputGroup.setGroupToggleCB(FFaDynCB2M(FuiCurveDefSheet,this,
					 onInputModeToggled,int,bool));

  this->frames[FuiCurveDefine::X]->setLabel("X Axis");
  this->frames[FuiCurveDefine::Y]->setLabel("Y Axis");

  this->exprField->setLabel("Function of  A, B, C, ...");
  this->exprField->setToolTip("Enter a math expression in the variables A, B, ..., J\n"
			      "and select the Curve corresponding to each variable");
  this->exprField->myField->setAcceptedCB(FFaDynCB1M(FuiCurveDefSheet,this,
						     onExpressionChanged,char*));

  this->compTable->showRowHeader(true);
  this->compTable->showColumnHeader(false);
  this->compTable->setNumberColumns(1);
  this->compTable->setSelectionPolicy(FFuTable::NO_SELECTION);

  this->fileBrowseField->setAbsToRelPath("yes");
  this->fileBrowseField->setFileOpenedCB(FFaDynCB2M(FuiCurveDefSheet,this,
						    onFileSelected,const std::string&,int));
  this->fileBrowseField->addDialogFilter("ASCII Curve Definition File",FmGraph::asc(),true);
  this->fileBrowseField->addDialogFilter("nCode DAC File",FmGraph::dac());
  this->fileBrowseField->addDialogFilter("MTS RPC Time history file",FmGraph::rpc());
  this->fileBrowseField->setDialogRememberKeyword("ExternalCurveDefinition");

  this->startXField->setLabel("Start x");
  this->stopXField->setLabel("Stop x");
  this->incXField->setLabel("Increment");
  this->startXField->setAcceptedCB(FFaDynCB1M(FuiCurveDefSheet,this,
					      onFieldValueChanged,double));
  this->stopXField->setAcceptedCB(FFaDynCB1M(FuiCurveDefSheet,this,
					     onFieldValueChanged,double));
  this->incXField->setAcceptedCB(FFaDynCB1M(FuiCurveDefSheet,this,
					    onFieldValueChanged,double));
  this->incXField->setSensitivity(false);
  this->useSmartPointsBtn->setLabel("Auto Increment");
  this->useSmartPointsBtn->setToggleCB(FFaDynCB1M(FuiCurveDefSheet,this,
						  onButtonToggled,bool));

  this->functionLabel->setLabel("Function");
  this->functionMenu->turnButtonOff(true);
  this->functionMenu->setBehaviour(FuiQueryInputField::REF_NONE);
  this->functionMenu->setRefSelectedCB(FFaDynCB1M(FuiCurveDefSheet,this,
						  onRefSelected,int));

  this->completeLabel->setLabel("Incomplete");
  FFuaPalette pal;
  this->completeLabel->getColors(pal);
  pal.setTextOnStdBackground(255,0,0);
  this->completeLabel->setColors(pal);

  this->autoExportToggle->setLabel("Export curve automatically");
  this->autoExportToggle->setToggleCB(FFaDynCB1M(FuiCurveDefSheet,this,
						 onButtonToggled,bool));

  this->reloadCurveButton->setLabel("Reload");
  this->reloadCurveButton->setActivateCB(FFaDynCB0M(FuiCurveDefSheet,this,
						    onReloadCurveClicked));

  this->channelField->setLabel("Column/channel");
  this->channelField->setSensitivity(false);
  this->channelBtn->setLabel("Select...");
  this->channelBtn->setActivateCB(FFaDynCB0M(FuiCurveDefSheet,this,
					     onChannelBtnClicked));
  this->channelSelectUI->setTitle("Channel list");
  this->channelSelectUI->showNotes = true;
  this->channelSelectUI->labNotesText->setLabel("Column 1 is the first <u>after</u> the argument column.");
  this->channelSelectUI->labNotesImage->popUp();
  this->channelSelectUI->labNotesLabel->popUp();
  this->channelSelectUI->labNotesText->popUp();
  this->channelSelectUI->setCancelButtonClickedCB(FFaDynCB1M(FuiCurveDefSheet,this,
							     onChannelSelectCancel,int));
  this->channelSelectUI->setOkButtonClickedCB(FFaDynCB1M(FuiCurveDefSheet,this,
							 onChannelSelectOK,int));
  this->channelSelectUI->setApplyButtonClickedCB(FFaDynCB1M(FuiCurveDefSheet,this,
							    onChannelSelectApply,int));

  this->spaceOper->addOption("Length");
  this->spaceOper->addOption("Position X");
  this->spaceOper->addOption("Position Y");
  this->spaceOper->addOption("Position Z");
  this->spaceOper->addOption("Initial Length");
  this->spaceOper->addOption("Initial Position X");
  this->spaceOper->addOption("Initial Position Y");
  this->spaceOper->addOption("Initial Position Z");
  this->spaceOper->setOptionSelectedCB(FFaDynCB1M(FuiCurveDefSheet,this,onRefSelected,int));
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::placeWidgets(int width, int height)
{
  int border = this->getBorder();
  int textHeight = this->completeLabel->getHeightHint();
  int top = border;
  int bottom = height - border/2;
  int left = border;
  int right = width - border;

  // Radios
  int intFunWidth = this->intFunctionPlotRadio->getWidthHint();

  int frameTop = top/2;
  int frameBottom = bottom - border - textHeight;

  int sourceLeft = left;
  int sourceRight = intFunWidth + 3*border;

  int centerRadio = (frameBottom - frameTop - 2*border)/4;
  int rdbTop     = frameTop + border - textHeight/2 + centerRadio;
  int rdbBottom  = rdbTop + textHeight/2 + border/2;
  int combTop    = rdbTop + centerRadio;
  int combBottom = combTop + textHeight/2 + border/2;
  int extTop     = combTop + centerRadio;
  int extBottom  = extTop + textHeight/2 + border/2;
  int intTop     = extTop + centerRadio;
  int intBottom  = intTop + textHeight/2 + border/2;

  int radioLeft  = sourceLeft + border;
  int radioRight = radioLeft + intFunWidth;

  // Frames
  int sFrameTop = frameTop;
#ifdef FT_HAS_PREVIEW
  int sFrameBottom = frameBottom;
#else
  int sFrameBottom = extBottom + border;
#endif
  int bFrameTop = 5*frameTop/2;
  int bFrameBottom = frameBottom;
  int xFrameLeft = sourceRight + border/2;
  int centerX = (width - xFrameLeft)/2 + xFrameLeft;
  int xFrameRight = centerX - border/4;
  int yFrameRight = right;
  int yFrameLeft = centerX + border/4;

  if (this->isSpatial && rdbResultPlotRadio->getValue()) {
    bFrameTop = bFrameBottom - 5*textHeight/3 - border;
    frameBottom = bFrameTop - border/2;
  }

  // Axes
  int axisTop = frameTop + 3*border/2;
  int axisBottom = frameBottom - border/2;
  int xAxisLeft = xFrameLeft + border;
  int xAxisRight = xFrameRight - border;
  int yAxisLeft = yFrameLeft + border;
  int yAxisRight = yFrameRight - border;
  int xOperTop = axisBottom - textHeight-border;
  if (xOperTop < axisTop) xOperTop = axisTop;
  int tRangeLeft = (3*xAxisLeft+2*yAxisRight)/5;

  // Placing widgets
  this->sourceFrame->setEdgeGeometry(sourceLeft, sourceRight, sFrameTop, sFrameBottom);
  this->rdbResultPlotRadio->setEdgeGeometry(radioLeft,radioRight,rdbTop,rdbBottom);
  this->combCurvePlotRadio->setEdgeGeometry(radioLeft,radioRight,combTop,combBottom);
  this->extCurvePlotRadio->setEdgeGeometry(radioLeft,radioRight,extTop,extBottom);
  this->intFunctionPlotRadio->setEdgeGeometry(radioLeft,radioRight,intTop,intBottom);

  this->frames[FuiCurveDefine::X]->setEdgeGeometry(xFrameLeft, xFrameRight, frameTop, frameBottom);
  this->frames[FuiCurveDefine::Y]->setEdgeGeometry(yFrameLeft, yFrameRight, frameTop, frameBottom);
  this->axes[FuiCurveDefine::X]->setEdgeGeometry(xAxisLeft, xAxisRight, axisTop, axisBottom);
  this->axes[FuiCurveDefine::Y]->setEdgeGeometry(yAxisLeft, yAxisRight, axisTop, axisBottom);

  this->definitionFrame->toBack();
  this->definitionFrame->setEdgeGeometry(xFrameLeft, yFrameRight, bFrameTop, bFrameBottom);
  this->timeRange->setEdgeGeometry(tRangeLeft, yAxisRight, bFrameTop+border/2, bFrameBottom-border/2);
  this->spaceObj->setEdgeGeometry(xAxisLeft, tRangeLeft-border, bFrameTop+border/2, bFrameBottom-border/2);
  this->spaceOper->setEdgeGeometry(xAxisLeft, xAxisRight, xOperTop, axisBottom);

  // place combined curve fields
  int fieldH = 20;
  int fieldTop = bFrameTop + border;
  int fieldBottom = fieldTop + fieldH;
  int fieldRight = yAxisRight;
  int reloadWidth = this->reloadCurveButton->getWidthHint();
  if (this->isCompCurve && this->reloadCurveButton->isPoppedUp())
    fieldRight -= border+reloadWidth;
  this->exprField->setEdgeGeometry(xAxisLeft,fieldRight,fieldTop,fieldBottom);

  int nRows = this->compTable->getNumberRows();
  if (nRows > 0)
  {
    int fieldW = yAxisRight - xAxisLeft - 36;
    int tableTop = fieldBottom + border;
    int tableH = border + nRows*fieldH;
    if (tableTop+tableH+border > frameBottom)
    {
      tableH = frameBottom-tableTop-border;
      fieldW -= 17;
    }
    int tableBottom = tableTop + tableH;
    this->compTable->setEdgeGeometry(xAxisLeft,yAxisRight,tableTop,tableBottom);

    for (int i = 0; i < nRows; i++)
      this->compTable->setRowHeight(i,fieldH);
    this->compTable->setColumnWidth(0,fieldW);
  }
  else
    this->compTable->popDown();

  // external curve params
  int fieldHeight = 4*height/25;
  int channelBtnWidth = this->channelBtn->getWidthHint();

  int v0 = top + 3*border/2;
  int v1 = v0 + fieldHeight;
  int v2 = v1 + border/2;
  int v3 = v2 + fieldHeight;

  int browseFieldLeft = sourceRight + 3*border/2;
  int channelFieldRight = right-3*border/2-channelBtnWidth;

  // place external curve fields
  this->fileBrowseField->setEdgeGeometry(browseFieldLeft, right-border-reloadWidth-4, v0, v1);
  if (this->isSpatial && rdbResultPlotRadio->getValue()) // special placing of reload button for beam diagrams
    this->reloadCurveButton->setEdgeGeometry(right-border-reloadWidth, right-border, axisTop, axisTop*0.55 + axisBottom*0.45);
  else if (this->isFuncPreview)
    this->reloadCurveButton->setEdgeGeometry(right-border-reloadWidth, right-border, v2, v3);
  else if (this->isCompCurve)
    this->reloadCurveButton->setEdgeGeometry(right-border-reloadWidth, right-border, fieldTop-2, fieldBottom+2);
  else
    this->reloadCurveButton->setEdgeGeometry(right-border-reloadWidth, right-border, v0, v1);
  this->channelField->setEdgeGeometry(browseFieldLeft, channelFieldRight, v2, v3);
  this->channelBtn->setEdgeGeometry(channelFieldRight+border/2, right-border, v2, v3);

  // internal function params
  int funclWid = this->functionLabel->getWidthHint();
  int strtlWid = this->startXField->getWidthHint();
  int stoplWid = this->stopXField->getWidthHint();
  int inclWid  = this->incXField->getWidthHint();
  int smPtsWid = this->useSmartPointsBtn->getWidthHint();
  int paramFieldWidth = (width - sourceRight-10*border-strtlWid-stoplWid-inclWid-smPtsWid)/3;

  int h0  = sourceRight + 3*border/2;
  int h01 = h0 + funclWid;
  int h02 = h01 + border;
  int h3  = h0 + strtlWid + border + paramFieldWidth;
  int h4  = h3 + border;
  int h7  = h4 + stoplWid + border/2 + paramFieldWidth;
  int h8  = h7 + border;
  int h11 = h8 + inclWid + border/2 + paramFieldWidth;
  int h12 = h11 + border;
  int h13 = h12 + smPtsWid;

  // place internal function fields
  this->startXField->setEdgeGeometry(h0,h3,v0,v1);
  this->stopXField->setEdgeGeometry(h4,h7,v0,v1);
  this->incXField->setEdgeGeometry(h8,h11,v0,v1);
  this->useSmartPointsBtn->setEdgeGeometry(h12,h13,v0,v1);
  if (this->isFuncPreview && this->reloadCurveButton->isPoppedUp())
    h13 = right-2*border-reloadWidth;
  this->functionLabel->setEdgeGeometry(h0,h01,v2,v3);
  this->functionMenu->setEdgeGeometry(h02,h13,v2,v3);

  int h2 = height - border/2;
  int autoXWidth = this->autoExportToggle->getWidthHint();
  this->completeLabel->setEdgeGeometry(sourceLeft,sourceRight,h2-textHeight,h2);
  this->autoExportToggle->setEdgeGeometry(yAxisRight-autoXWidth,yAxisRight,
					  h2-textHeight,h2);
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::onPoppedDownToMem()
{
  this->channelSelectUI->popDown();
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::onReloadCurveClicked()
{
  this->reloadCurveCB.invoke();
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::onChannelSelectOK(int index)
{
  this->channelSelectUI->popDown();
  this->onChannelSelectApply(index);
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::onChannelSelectApply(int index)
{
  if (channelSelectUI->myItemSelector->isItemSelected(index))
    channelField->myField->setValue(channelSelectUI->myItemSelector->getItemText(index));

  this->dataChangedCB.invoke();
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::onChannelSelectCancel(int)
{
  this->channelSelectUI->popDown();
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::onChannelBtnClicked()
{
  this->channelBtnClickedCB.invoke(this->fileBrowseField->getFileName());
  this->channelSelectUI->popUp();
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::onFileSelected(const std::string&, int)
{
  this->dataChangedCB.invoke();
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::onExpressionChanged(char*)
{
  this->dataChangedCB.invoke();
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::onFieldValueChanged(double)
{
  this->dataChangedCB.invoke();
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::onButtonToggled(bool)
{
  this->dataChangedCB.invoke();
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::onRefSelected(int)
{
  this->dataChangedCB.invoke();
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::setDefineMode(int inputMode)
{
  if (inputMode == TEMPORAL_RESULT)
    this->definitionFrame->popDown();
  else
    this->definitionFrame->popUp();

  for (int a = 0; a < FuiCurveDefine::NDIRS; a++)
  {
    if (inputMode == TEMPORAL_RESULT || inputMode == SPATIAL_RESULT)
      this->frames[a]->popUp();
    else
      this->frames[a]->popDown();

    if (inputMode == TEMPORAL_RESULT)
      this->axes[a]->popUp();
    else if (inputMode == SPATIAL_RESULT && a == FuiCurveDefine::Y)
      this->axes[a]->popUp();
    else
      this->axes[a]->popDown();
  }

  if (inputMode == SPATIAL_RESULT) {
    timeRange->popUp();
    spaceObj->popUp();
    spaceOper->popUp();
  }
  else {
    timeRange->popDown();
    spaceObj->popDown();
    spaceOper->popDown();
  }

  if (inputMode == COMB_CURVE) {
    this->exprField->popUp();
    this->compTable->popUp();
  }
  else {
    this->exprField->popDown();
    this->compTable->popDown();
  }

  if (inputMode == EXT_CURVE) {
    this->fileBrowseField->popUp();
    this->channelField->popUp();
    this->channelBtn->popUp();
  }
  else {
    this->fileBrowseField->popDown();
    this->channelField->popDown();
    this->channelBtn->popDown();
  }

  if (inputMode >= INT_FUNCTION) {
    this->startXField->popUp();
    this->stopXField->popUp();
    this->incXField->popUp();
    this->useSmartPointsBtn->popUp();
    this->functionLabel->popUp();
    this->functionMenu->popUp();
  }
  else {
    this->startXField->popDown();
    this->stopXField->popDown();
    this->incXField->popDown();
    this->useSmartPointsBtn->popDown();
    this->functionLabel->popDown();
    this->functionMenu->popDown();
  }
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::onInputModeToggled(int, bool)
{
  if (this->rdbResultPlotRadio->getValue())
    this->setDefineMode(this->isSpatial ? SPATIAL_RESULT : TEMPORAL_RESULT);
  else if (this->combCurvePlotRadio->getValue())
    this->setDefineMode(COMB_CURVE);
  else if (this->extCurvePlotRadio->getValue())
    this->setDefineMode(EXT_CURVE);
  else
    this->setDefineMode(INT_FUNCTION);

  this->dataChangedCB.invoke();
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::setCompleteSign(bool complete, bool preview)
{
  if (complete)
    this->completeLabel->setLabel(preview ? "Function preview" : "Complete");
  else
    this->completeLabel->setLabel("Incomplete");

  FFuaPalette pal;
  this->completeLabel->getColors(pal);
  pal.setTextOnStdBackground(complete ? 0 : 255, 0, 0);
  this->completeLabel->setColors(pal);
}


////////////////////////////////////////////////////////////////////////////////

void FuiCurveAnalysisSheet::initWidgets()
{
  this->dftFrame->setLabel("Fourier transform");
  this->startField->setLabel("Start");
  this->endField->setLabel("Stop");
  this->entireDomainBtn->setLabel("Entire");
  this->doDftBtn->setLabel("On/Off");
  this->removeCompBtn->setLabel("No 0 Hz component");
  this->resampleBtn->setLabel("Use sample rate");
  this->dftDomainFrame->setLabel("Time domain");

  this->startField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->endField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->resampleRateField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  this->startField->setAcceptedCB(FFaDynCB1M(FuiCurveAnalysisSheet,this,
					     onFieldValueChanged,double));
  this->endField->setAcceptedCB(FFaDynCB1M(FuiCurveAnalysisSheet,this,
					   onFieldValueChanged,double));
  this->resampleRateField->setAcceptedCB(FFaDynCB1M(FuiCurveAnalysisSheet,this,
						    onFieldValueChanged,double));
  this->doDftBtn->setToggleCB(FFaDynCB1M(FuiCurveAnalysisSheet,this,
					 onDftToggled,bool));
  this->entireDomainBtn->setToggleCB(FFaDynCB1M(FuiCurveAnalysisSheet,this,
						onButtonToggled,bool));
  this->removeCompBtn->setToggleCB(FFaDynCB1M(FuiCurveAnalysisSheet,this,
					      onButtonToggled,bool));
  this->resampleBtn->setToggleCB(FFaDynCB1M(FuiCurveAnalysisSheet,this,
  					   onButtonToggled,bool));

  this->diffBtn->setLabel("Differentiate");
  this->diffBtn->setToggleCB(FFaDynCB1M(FuiCurveAnalysisSheet,this,
					onDiffToggled,bool));

  this->intBtn->setLabel("Integrate");
  this->intBtn->setToggleCB(FFaDynCB1M(FuiCurveAnalysisSheet,this,
				       onIntToggled,bool));
}

//----------------------------------------------------------------------------

void FuiCurveAnalysisSheet::placeWidgets(int width, int height)
{
  int border = this->getBorder();
  int top = border/2;
  int left = border;
  int diffWidth = this->diffBtn->getWidthHint();
  int frameWidth = width - diffWidth - 4*border;
  int frameHeight = height - border;
  int fieldHeight = 4*height/25;

  int h0 = left;
  int h1 = h0 + frameWidth;
  int v0 = top;
  int v1 = v0 + frameHeight;
  this->dftFrame->setEdgeGeometry(h0,h1,v0,v1);

  int v01 = v0 + 2*border;
  int v02 = v01 + fieldHeight;
  int v021 = v02 + border/8;
  int v03 = v021 + 2*border;
  int v04 = v03 + fieldHeight;

  int labelWidth1 = this->startField->getWidthHint();
  int labelWidth2 = this->endField->getWidthHint();
  int labelWidth3 = this->entireDomainBtn->getWidthHint();
  int labelWidth4 = this->removeCompBtn->getWidthHint();
  int labelWidth5 = this->resampleBtn->getWidthHint();
  int fieldWidth2 = (frameWidth - labelWidth1 - labelWidth2 - labelWidth3 - 6*border)/2;

  int h21 = h0 + border/2;
  int h212 = h21 + labelWidth4;
  int h213 = h212 + 5*border/4;
  int h214 = h213 + labelWidth5;
  int h215 = h214 + border/4;
  int h22 = h21 + border/2;
  int h25 = h22 + labelWidth1 + border/4 + fieldWidth2;
  int h26 = h25 + border;
  int h29 = h26 + labelWidth2 + border/4 + fieldWidth2;
  int h291 = h29 + 5*border/2;
  int h292 = h291 + labelWidth3;
  int h30 = h1 - border/2;

  int v11 = v04 + border/2;
  int v12 = v11 + border/4;
  int v121 = v12 + fieldHeight;

  if (h214 > h30) h214 = h30;

  this->doDftBtn->setEdgeGeometry(h21,h21+this->doDftBtn->getWidthHint(),v01,v02);
  this->dftDomainFrame->setEdgeGeometry(h21,h30,v021,v11);
  this->startField->setEdgeGeometry(h22,h25,v03,v04);
  this->endField->setEdgeGeometry(h26,h29,v03,v04);
  this->entireDomainBtn->setEdgeGeometry(h291,h292,v03,v04);
  this->removeCompBtn->setEdgeGeometry(h21,h212,v12,v121);
  this->resampleBtn->setEdgeGeometry(h213,h214,v12,v121);
  this->resampleRateField->setEdgeGeometry(h215,h30,v12,v121);

  int h4 = h1 + 2*border;
  this->diffBtn->setEdgeGeometry(h4,h4+diffWidth,v01,v02);
  this->intBtn->setEdgeGeometry(h4,h4+diffWidth,v03,v04);
}

//----------------------------------------------------------------------------

void FuiCurveAnalysisSheet::setUIValues(const FuaCurveDefineValues* v)
{
    bool doDft = v->dftFeasable && v->analysis == 3;
    this->diffBtn->setValue(v->analysis == 1);
    this->intBtn->setValue(v->analysis == 2);
    this->doDftBtn->setValue(doDft);
    this->entireDomainBtn->setValue(v->dftEntireDomain);
    this->startField->setValue(v->dftDomain.first);
    this->endField->setValue(v->dftDomain.second);
    this->removeCompBtn->setValue(v->dftRemoveComp);
    this->resampleBtn->setValue(v->dftResample);
    this->resampleRateField->setValue(v->dftResampleRate);

    this->doDftBtn->setSensitivity(v->dftFeasable);
    this->entireDomainBtn->setSensitivity(doDft);
    this->startField->setSensitivity(doDft && !v->dftEntireDomain);
    this->endField->setSensitivity(doDft && !v->dftEntireDomain);
    this->removeCompBtn->setSensitivity(doDft);
    this->resampleBtn->setSensitivity(doDft);
    this->resampleRateField->setSensitivity(doDft && v->dftResample);
}

//----------------------------------------------------------------------------

void FuiCurveAnalysisSheet::getUIValues(FuaCurveDefineValues* v) const
{
  v->dftFeasable = this->doDftBtn->getSensitivity();
  if (this->diffBtn->getValue()) v->analysis = 1;
  if (this->intBtn->getValue()) v->analysis = 2;
  if (this->doDftBtn->getValue()) v->analysis = 3;
  v->dftDomain = { this->startField->getValue(), this->endField->getValue() };
  v->dftResampleRate = this->resampleRateField->getDouble();
  v->dftEntireDomain = this->entireDomainBtn->getValue();
  v->dftRemoveComp = this->removeCompBtn->getValue();
  v->dftResample = this->resampleBtn->getValue();
}

//----------------------------------------------------------------------------

void FuiCurveAnalysisSheet::onFieldValueChanged(double)
{
  this->dataChangedCB.invoke();
}

//----------------------------------------------------------------------------

void FuiCurveAnalysisSheet::onButtonToggled(bool)
{
  this->dataChangedCB.invoke();
}

void FuiCurveAnalysisSheet::onDftToggled(bool toggle)
{
  if (toggle)
  {
    diffBtn->setValue(false);
    intBtn->setValue(false);
    rainflowBtn->setValue(false);
  }
  this->dataChangedCB.invoke();
}

void FuiCurveAnalysisSheet::onDiffToggled(bool toggle)
{
  if (toggle)
  {
    doDftBtn->setValue(false);
    intBtn->setValue(false);
    rainflowBtn->setValue(false);
  }
  this->dataChangedCB.invoke();
}

void FuiCurveAnalysisSheet::onIntToggled(bool toggle)
{
  if (toggle)
  {
    doDftBtn->setValue(false);
    diffBtn->setValue(false);
    rainflowBtn->setValue(false);
  }
  this->dataChangedCB.invoke();
}


////////////////////////////////////////////////////////////////////////////////

void FuiCurveInfoSheet::setCurveStatistics(double rms, double avg,
					   double stdDev, double integral,
					   double min, double max)
{
  this->rmsField->setValue(rms);
  this->avgField->setValue(avg);
  this->stdDevField->setValue(stdDev);
  this->integralField->setValue(integral);
  this->minField->setValue(min);
  this->maxField->setValue(max);

  // not neccessary?
  if (this->probablyHasMarkers) {
    this->removeVerticalMarkersCB.invoke();
    this->probablyHasMarkers = false;
  }

  if (!this->entireDomainBtn->getValue()) {
    this->applyVerticalMarkersCB.invoke(this->startField->getValue(), this->stopField->getValue());
    this->probablyHasMarkers = true;
  }
}


void FuiCurveInfoSheet::initWidgets()
{
  this->calculateBtn->setLabel("Calculate");
  this->calculateBtn->setActivateCB(FFaDynCB0M(FuiCurveInfoSheet,this,
					       onBtnClicked));

  this->useScaleShiftBtn->setLabel("Use scaled/shifted");

  this->startField->setLabel("Start");
  this->startField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->startField->myField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  this->startField->setValue(0.0);
  this->startField->myField->setSensitivity(false);

  this->stopField->setLabel("Stop");
  this->stopField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->stopField->myField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  this->stopField->setValue(1.0);
  this->stopField->myField->setSensitivity(false);

  this->rmsField->setLabel("RMS");
  this->rmsField->myField->setSensitivity(false);
  this->rmsField->myField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  this->rmsField->setValue(0.0);
  this->rmsField->setToolTip("Root Mean Square");

  this->avgField->setLabel("Mean");
  this->avgField->myField->setSensitivity(false);
  this->avgField->myField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  this->avgField->setValue(0.0);
  this->avgField->setToolTip("Mean value");

  this->stdDevField->setLabel("Std.dev");
  this->stdDevField->myField->setSensitivity(false);
  this->stdDevField->myField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  this->stdDevField->setValue(0.0);
  this->stdDevField->setToolTip("Standard deviation");

  this->integralField->setLabel("Integral");
  this->integralField->myField->setSensitivity(false);
  this->integralField->myField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  this->integralField->setValue(0.0);
  this->integralField->setToolTip("Area under curve");

  this->minField->setLabel("Min");
  this->minField->myField->setSensitivity(false);
  this->minField->myField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  this->minField->setValue(0.0);
  this->minField->setToolTip("Curve minimum");

  this->maxField->setLabel("Max");
  this->maxField->myField->setSensitivity(false);
  this->maxField->myField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  this->maxField->setValue(0.0);
  this->maxField->setToolTip("Curve maximum");

  this->entireDomainBtn->setLabel("Entire");
  this->entireDomainBtn->setValue(true);
  this->entireDomainBtn->setToggleCB(FFaDynCB1M(FuiCurveInfoSheet,this,
						onEntireDomainToggled,bool));

  this->domainFrame->setLabel("X Axis Domain");

  int labelW = this->avgField->myLabel->getWidthHint();
  this->rmsField->setLabelWidth(labelW);
  this->avgField->setLabelWidth(labelW);

  labelW = this->integralField->myLabel->getWidthHint();
  this->stdDevField->setLabelWidth(labelW);
  this->integralField->setLabelWidth(labelW);

  labelW = this->maxField->myLabel->getWidthHint();
  this->minField->setLabelWidth(labelW);
  this->maxField->setLabelWidth(labelW);
}


void FuiCurveInfoSheet::placeWidgets(int width, int height)
{
  int border = this->getBorder();

  int left = border;
  int right = width - border/2;
  int top = 0;
  int bottom = height - border/2;

  int domainFrameBtm = bottom;
  int startStopBtm = domainFrameBtm - border;
  int lineHeight = 20;
  int startStopTp = startStopBtm - lineHeight;

  int domainFrameTp = startStopTp - 2*border;

  int toggleBtnLeft = right - this->entireDomainBtn->getWidthHint() - border;
  this->entireDomainBtn->setEdgeGeometry(toggleBtnLeft, right - border, startStopTp, startStopBtm);

  int available = toggleBtnLeft - left - border;

  this->startField->setEdgeGeometry(left + border, left + available/2, startStopTp, startStopBtm);
  this->stopField->setEdgeGeometry(this->startField->getXRightPos() + border, toggleBtnLeft - border,
				   startStopTp, startStopBtm);

  this->domainFrame->setEdgeGeometry(left, right, domainFrameTp, domainFrameBtm);

  available = domainFrameTp - top;
  int rmsCenterY = top + border + lineHeight/2;
  int avgCenterY = rmsCenterY + lineHeight + border/2;

  int calcRight = right;

  int btnWidth;
  if (this->calculateBtn->getWidthHint() > this->useScaleShiftBtn->getWidthHint())
    btnWidth = this->calculateBtn->getWidthHint();
  else
    btnWidth = this->useScaleShiftBtn->getWidthHint();

  int calcLeft = calcRight - btnWidth;

  available = calcLeft - left;
  int fieldWidth = (available - 3*border)/3;

  int rmsLeft = left;
  int rmsRight = left + fieldWidth;
  int stdDevLeft = rmsRight + border;
  int stdDevRight = stdDevLeft + fieldWidth;
  int maxLeft = stdDevRight + border;
  int maxRight = maxLeft + fieldWidth;

  this->rmsField->setEdgeGeometry(rmsLeft, rmsRight, rmsCenterY - lineHeight/2, rmsCenterY + lineHeight/2);
  this->avgField->setEdgeGeometry(rmsLeft, rmsRight, avgCenterY - lineHeight/2, avgCenterY + lineHeight/2);
  this->stdDevField->setEdgeGeometry(stdDevLeft, stdDevRight, rmsCenterY - lineHeight/2, rmsCenterY + lineHeight/2);
  this->integralField->setEdgeGeometry(stdDevLeft, stdDevRight, avgCenterY - lineHeight/2, avgCenterY + lineHeight/2);
  this->maxField->setEdgeGeometry(maxLeft, maxRight, rmsCenterY - lineHeight/2, rmsCenterY + lineHeight/2);
  this->minField->setEdgeGeometry(maxLeft, maxRight, avgCenterY - lineHeight/2, avgCenterY + lineHeight/2);

  this->calculateBtn->setEdgeGeometry(calcLeft, calcRight, rmsCenterY - lineHeight/2, rmsCenterY + lineHeight/2);
  this->useScaleShiftBtn->setEdgeGeometry(calcLeft, calcRight, avgCenterY - lineHeight/2, avgCenterY + lineHeight/2);
}


void FuiCurveInfoSheet::onBtnClicked()
{
  double start = this->startField->getValue();
  double stop  = this->stopField->getValue();
  bool entireDomain = this->entireDomainBtn->getValue();
  bool useScaled = this->useScaleShiftBtn->getValue();

  if (this->probablyHasMarkers)
    this->removeVerticalMarkersCB.invoke();
  this->probablyHasMarkers = false;

  Fui::noUserInputPlease();
  this->getCurveStatisticsCB.invoke(useScaled, entireDomain, start, stop);
  Fui::okToGetUserInput();
}


void FuiCurveInfoSheet::onEntireDomainToggled(bool toggle)
{
  this->startField->myField->setSensitivity(!toggle);
  this->stopField->myField->setSensitivity(!toggle);
}


void FuiCurveInfoSheet::setUIValues(const FuaCurveDefineValues*)
{
  this->rmsField->setValue(0.0);
  this->avgField->setValue(0.0);
  this->stdDevField->setValue(0.0);
  this->integralField->setValue(0.0);
  this->maxField->setValue(0.0);
  this->minField->setValue(0.0);
  if (this->probablyHasMarkers)
    this->removeVerticalMarkersCB.invoke();
  this->probablyHasMarkers = false;
}


////////////////////////////////////////////////////////////////////////////////

void FuiCurveScaleSheet::setUIValues(const FuaCurveDefineValues* v)
{
  XScale->setValues(v->scaleX, v->zeroAdjustX, v->offsetX);
  YScale->setValues(v->scaleY, v->zeroAdjustY, v->offsetY);
}


void FuiCurveScaleSheet::getUIValues(FuaCurveDefineValues* v) const
{
  XScale->getValues(v->scaleX, v->zeroAdjustX, v->offsetX);
  YScale->getValues(v->scaleY, v->zeroAdjustY, v->offsetY);
}


void FuiCurveScaleSheet::initWidgets()
{
  XScale->setFrameTitles("X Axis Scale and Shift", "Horizontal shift after scale");
  YScale->setFrameTitles("Y Axis Scale and Shift", "Vertical shift after scale");

  XScale->setDataChangedCB(FFaDynCB0M(FuiCurveScaleSheet,this,invokeDataChangedCB));
  YScale->setDataChangedCB(FFaDynCB0M(FuiCurveScaleSheet,this,invokeDataChangedCB));
}


////////////////////////////////////////////////////////////////////////////////

void FuiCurveFatigueSheet::initWidgets()
{
  this->gateValueField->setLabel("Stress range threshold");
  this->gateValueField->setToolTip("Stress ranges below this value will be ignored");
  this->gateValueField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->gateValueField->myField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  this->gateValueField->setValue(0.0);
  this->gateValueField->myField->setAcceptedCB(FFaDynCB1M(FuiCurveFatigueSheet,this,
							  onFieldValueChanged,double));

  this->resultFrame->setLabel("Fatigue results");

  this->damageField->setLabel("Damage");
  this->damageField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->damageField->myField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  this->damageField->setValue(0.0);
  this->damageField->myField->setSensitivity(false);

  this->unitLabel->setLabel("Life unit");
  this->lifeField->setLabel("Life");
  this->lifeField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->lifeField->myField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  this->lifeField->setValue(0.0);
  this->lifeField->myField->setSensitivity(false);

  this->unitTypeMenu->addOption("Repeats");
  this->unitTypeMenu->addOption("Hours");
  this->unitTypeMenu->addOption("Days");
  this->unitTypeMenu->setOptionSelectedCB(FFaDynCB1M(FuiCurveFatigueSheet,this,
						     onUnitValueChanged,int));

  this->calculateBtn->setLabel("Calculate\nweighted life");
  this->calculateBtn->setActivateCB(FFaDynCB0M(FuiCurveFatigueSheet,this,
					       onButtonClicked));

  this->doRainflowBtn->setLabel("Show rainflow");
  this->doRainflowBtn->setToggleCB(FFaDynCB1M(FuiCurveFatigueSheet,this,
					      onButtonToggled,bool));

  this->domainFrame->setLabel("Time Interval");

  this->startField->setLabel("Start");
  this->startField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->startField->myField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  this->startField->setValue(0.0);
  this->startField->myField->setSensitivity(false);
  this->startField->myField->setAcceptedCB(FFaDynCB1M(FuiCurveFatigueSheet,this,
						      onFieldValueChanged,double));

  this->stopField->setLabel("Stop");
  this->stopField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->stopField->myField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  this->stopField->setValue(1.0);
  this->stopField->myField->setSensitivity(false);
  this->stopField->myField->setAcceptedCB(FFaDynCB1M(FuiCurveFatigueSheet,this,
						     onFieldValueChanged,double));

  this->entireDomainBtn->setLabel("Entire");
  this->entireDomainBtn->setValue(true);
  this->entireDomainBtn->setToggleCB(FFaDynCB1M(FuiCurveFatigueSheet,this,
						onButtonToggled,bool));
}


void FuiCurveFatigueSheet::placeWidgets(int width, int height)
{
  int border = this->getBorder();

  int left = border;
  int right = width - border/2;
  int lineHeight = (height-8*border)/3;

  int gateValRight = right;
  int gateValLeft = gateValRight - (right-left)/3;
  int snRight = gateValLeft - 2*border;
  int snLeft = left;
  int snTop = border/2;
  int snBottom = snTop + lineHeight;

  this->snSelector->setEdgeGeometry(snLeft, snRight, snTop, snBottom);
  this->gateValueField->setEdgeGeometry(gateValLeft, gateValRight, snTop, snBottom);

  int resultRight = right - border;
  if (this->showCalcBtn)
    resultRight -= this->calculateBtn->getWidthHint() + border;
  int labelWidth = this->unitLabel->getWidthHint();
  int fieldWidth = (resultRight - labelWidth - 6*border)/3;
  int damageLeft = left + border;
  int damageRight = damageLeft + fieldWidth;
  int lifeLeft = damageRight + border;
  int lifeRight = lifeLeft + fieldWidth;
  int unitLeft = lifeRight + border;
  int unitRight = unitLeft + labelWidth;
  int unitTypeLeft = unitRight + border;

  int domainFrameTp = snBottom;
  int damageTp = domainFrameTp + 2*border;
  int damageBtm = damageTp + lineHeight;
  int domainFrameBtm = damageBtm + border;

  this->damageField->setEdgeGeometry(damageLeft, damageRight, damageTp, damageBtm);
  this->lifeField->setEdgeGeometry(lifeLeft, lifeRight, damageTp, damageBtm);
  this->unitLabel->setEdgeGeometry(unitLeft, unitRight, damageTp, damageBtm);
  this->unitTypeMenu->setCenterYGeometry(unitTypeLeft, damageBtm - lineHeight/2,
					 fieldWidth, lineHeight);
  this->resultFrame->setEdgeGeometry(left, right, domainFrameTp, domainFrameBtm);

  int calcLeft = unitTypeLeft + fieldWidth + border;
  int calcRight = right - border;
  int calcTop = domainFrameTp + 3*border/2;
  int calcBtm = domainFrameBtm - border/2;
  this->calculateBtn->setEdgeGeometry(calcLeft, calcRight, calcTop, calcBtm);

  domainFrameTp = domainFrameBtm + border;
  int startStopTp = domainFrameTp + 2*border;
  int startStopBtm = startStopTp + lineHeight;
  domainFrameBtm = startStopBtm + border;

  int toggleBtnLeft = right - this->doRainflowBtn->getWidthHint();
  this->doRainflowBtn->setEdgeGeometry(toggleBtnLeft, right, startStopTp, startStopBtm);
  int domainRight = toggleBtnLeft - border;

  toggleBtnLeft = domainRight - this->entireDomainBtn->getWidthHint() - border;
  this->entireDomainBtn->setEdgeGeometry(toggleBtnLeft, domainRight - border,
					 startStopTp, startStopBtm);

  int startRight = left + (toggleBtnLeft - left - border)/2;
  this->startField->setEdgeGeometry(left + border, startRight, startStopTp, startStopBtm);
  this->stopField->setEdgeGeometry(startRight + border, toggleBtnLeft - border,
				   startStopTp, startStopBtm);

  this->domainFrame->setEdgeGeometry(left, domainRight, domainFrameTp, domainFrameBtm);
}


void FuiCurveFatigueSheet::calculateCurveDamage(bool eventWeighted)
{
  double start = this->startField->getValue();
  double stop  = this->stopField->getValue();
  bool entireDomain = this->entireDomainBtn->getValue();

  Fui::noUserInputPlease();
  this->calculateCurveDamageCB.invoke(eventWeighted,entireDomain,start,stop);
  Fui::okToGetUserInput();

  if (!eventWeighted)
    this->delayDamage = false;
}


void FuiCurveFatigueSheet::onTabSelected(FFuComponentBase* selectedSheet)
{
  if (selectedSheet == this && this->delayDamage)
    this->calculateCurveDamage();
}

void FuiCurveFatigueSheet::onButtonToggled(bool)
{
  this->dataChangedCB.invoke();
}

void FuiCurveFatigueSheet::onFieldValueChanged(double)
{
  this->dataChangedCB.invoke();
}

void FuiCurveFatigueSheet::onUnitValueChanged(int value)
{
  this->recalculateLife(value);
}


void FuiCurveFatigueSheet::setDataChangedCB(const FFaDynCB0& aCB)
{
  this->dataChangedCB = aCB;
  this->snSelector->setDataChangedCB(aCB);
}


void FuiCurveFatigueSheet::getUIValues(FuaCurveDefineValues* v) const
{
  this->snSelector->getValues(v->fatigueSNStd,v->fatigueSNCurve);
  v->fatigueGateValue = this->gateValueField->getValue();
  if (this->doRainflowBtn->getValue()) v->analysis = 4;
  v->fatigueFeasable = this->snSelector->getSensitivity();
  v->fatigueLifeUnit = this->unitTypeMenu->getSelectedOption();
  v->fatigueDomain = { this->startField->myField->getDouble(), this->stopField->myField->getDouble() };
  v->fatigueEntireDomain = this->entireDomainBtn->getValue();
}


void FuiCurveFatigueSheet::setUIValues(const FuaCurveDefineValues* v,
				       bool recalculateDamage)
{
  this->snSelector->setValues(v->fatigueSNStd,v->fatigueSNCurve);
  this->gateValueField->setValue(v->fatigueGateValue);
  this->doRainflowBtn->setValue(v->analysis == 4);
  this->unitTypeMenu->selectOption(v->fatigueLifeUnit);
  this->startField->setValue(v->fatigueDomain.first);
  this->stopField->setValue(v->fatigueDomain.second);
  this->entireDomainBtn->setValue(v->fatigueEntireDomain);

  this->snSelector->setSensitivity(v->fatigueFeasable > 0);
  this->unitTypeMenu->setSensitivity(v->fatigueFeasable > 0);

  this->showCalcBtn = v->fatigueFeasable > 1;
  if (recalculateDamage && v->fatigueFeasable)
    this->calculateCurveDamage();
  else if (v->fatigueFeasable)
    this->delayDamage = true; // delay calculation until Fatigue tab is selected
  else if (recalculateDamage)
  {
    // Fatigue tab selected, but damage calculation is not feasable
    this->damageField->setValue(0.0);
    this->lifeField->setValue(0.0);
  }

  this->startField->myField->setSensitivity(!v->fatigueEntireDomain);
  this->stopField->myField->setSensitivity(!v->fatigueEntireDomain);
}


void FuiCurveFatigueSheet::buildDynamicWidgets(const FuaCurveDefineValues* v)
{
  if (v->fatigueFeasable > 1)
    this->calculateBtn->popUp();
  else
    this->calculateBtn->popDown();
}


void FuiCurveFatigueSheet::setCurveDamage(double damage, double interval)
{
  this->myInterval = interval;

  this->damageField->setValue(damage);
  this->recalculateLife(this->unitTypeMenu->getSelectedOption());
}


void FuiCurveFatigueSheet::recalculateLife(int unitType)
{
  double damage = this->damageField->getValue();
  double life = damage > 0.0 ? 1.0/damage : 0.0;

  if (unitType == HOURS)
    life *= this->myInterval / 3600.0;
  else if (unitType == DAYS)
    life *= this->myInterval / 86400.0;

  this->lifeField->setValue(life);
}
