// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiCurveDefine.H"
#include "vpmUI/vpmUIComponents/FuiCurveAxisDefinition.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "vpmUI/vpmUIComponents/FuiScaleShiftWidget.H"
#include "vpmUI/vpmUIComponents/FuiSNCurveSelector.H"
#include "vpmUI/Fui.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuColorChooser.H"
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

void FuiCurveDefine::setLegend(const std::string& legend)
{
  this->legendField->setValue(legend.c_str());
}

//----------------------------------------------------------------------------

void FuiCurveDefine::setCompleteSign(bool complete)
{
  this->curveDefSheet->setCompleteSign(complete);
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
  this->tabStack->setCurrentTab(tmpSel);
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

//----------------------------------------------------------------------------

void FuiCurveDefine::onLegendButtonToggled(bool toggle)
{
  this->autoLegendCB.invoke(toggle);
  this->updateDBValues();
}

//----------------------------------------------------------------------------

void FuiCurveDefine::rememberSelectedTab(bool show)
{
  if (show)
    tabStack->setCurrentTab(selectedTab);
  else
    selectedTab = tabStack->getCurrentTabPosIdx();
}


////////////////////////////////////////////////////////////////////////////////

void FuiCurveDomain::initWidgets(bool internalToggleCB)
{
  startField->setLabel("Start");
  endField->setLabel("Stop");
  entireBtn->setLabel("Entire");

  startField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  startField->myField->setDoubleDisplayMode(FFuIOField::AUTO,6,1);

  endField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  endField->myField->setDoubleDisplayMode(FFuIOField::AUTO,6,1);

  if (internalToggleCB)
    entireBtn->setToggleCB(FFaDynCB1M(FuiCurveDomain,this,
                                      onButtonToggled,bool));
}

void FuiCurveDomain::initWidgets(const FFaDynCB1<double>& doubleCB,
				 const FFaDynCB1<bool>& boolCB)
{
  this->initWidgets(false);

  startField->setAcceptedCB(doubleCB);
  endField->setAcceptedCB(doubleCB);
  entireBtn->setToggleCB(boolCB);
}

//----------------------------------------------------------------------------

void FuiCurveDomain::setValues(const std::pair<double,double>& domain,
                               bool entire, bool isSensitive)
{
  entireBtn->setValue(entire);
  startField->setValue(domain.first);
  endField->setValue(domain.second);

  entireBtn->setSensitivity(isSensitive);
  startField->setSensitivity(isSensitive && !entire);
  endField->setSensitivity(isSensitive && !entire);
}

//----------------------------------------------------------------------------

bool FuiCurveDomain::getValues(std::pair<double,double>& domain) const
{
  domain = { startField->getValue(), endField->getValue() };
  return entireBtn->getValue();
}

//----------------------------------------------------------------------------

void FuiCurveDomain::onButtonToggled(bool toggle)
{
  startField->myField->setSensitivity(!toggle);
  endField->myField->setSensitivity(!toggle);
}


////////////////////////////////////////////////////////////////////////////////

FuiCurveAppearanceSheet::FuiCurveAppearanceSheet()
{
  this->curveTypeMenu = 0;
  this->curveWidthBox = 0;
  this->colorChooser = 0;

  this->curveSymbolMenu = 0;
  this->symbolSizeBox = 0;
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

  colorChooser->setSelectionChangedCB(FFaDynCB1M(FuiCurveAppearanceSheet,this,
                                                 onColorChanged,const FFuColor&));

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
						 onSymbolsToggled,bool));

  for (const FmCurveColor& color : FmGraph::getCurveDefaultColors())
    this->colorChooser->insertCol(color.first,color.second);

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

  // These features are (not yet) supported with Qwt 6.1.2, so hide the widgets in the GUI
  this->numSymbolsBox->popDown();
  this->allSymbolsButton->popDown();
}

//----------------------------------------------------------------------------

void FuiCurveAppearanceSheet::setUIValues(const FuaCurveDefineValues* curveValues)
{
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


////////////////////////////////////////////////////////////////////////////////

FuiCurveDefSheet::FuiCurveDefSheet()
{
  axes.fill(NULL);

  rdbResultPlotRadio   = 0;
  combCurvePlotRadio   = 0;
  extCurvePlotRadio    = 0;
  intFunctionPlotRadio = 0;

  exprField         = 0;
  fileBrowseField   = 0;
  reloadCurveButton = 0;
  channelField      = 0;
  channelBtn        = 0;
  channelSelectUI   = 0;
  functionMenu      = 0;
  startXField       = 0;
  stopXField        = 0;
  incXField         = 0;
  useSmartPointsBtn = 0;

  completeLabel     = 0;
  autoExportToggle  = 0;

  timeRange = 0;
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
  if (aInputMode == SPATIAL_RESULT && v->firstTriad && v->secondTriad)
    this->timeRange->setObjLabel(v->firstTriad->getIdString() + " - " +
                                 v->secondTriad->getIdString());
  else
    this->timeRange->setObjLabel("");
  this->timeRange->setUIValues(v->timeRange.first,v->timeRange.second,
                               v->timeOper);
  this->spaceOper->selectOption(v->spaceOper,false);
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
      this->curveComps[i]->popUp();
    }
    else {
      this->curveComps[i]->setSelectedRef(NULL);
      this->curveComps[i]->setSensitivity(true);
      this->curveComps[i]->popDown();
    }

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

  if (v->curveComps.empty()) return;

  this->setNoComps(v->curveComps.size());
  for (FuiQueryInputField* comp : curveComps)
  {
    comp->setBehaviour(FuiQueryInputField::REF_NONE,true);
    comp->setRefSelectedCB(FFaDynCB1M(FuiCurveDefSheet,this,onRefSelected,int));
    comp->setQuery(v->curveQuery);
  }
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::setChannelList(const std::vector<std::string>& channels)
{
  this->channelSelectUI->myItemSelector->setItems(channels);
  this->channelSelectUI->myItemSelector->selectItem(0);
}

//----------------------------------------------------------------------------

FmModelMemberBase* FuiCurveDefSheet::getSelectedFunction() const
{
  return this->functionMenu->getSelectedRef();
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::setProvideChannelsCB(const FFaDynCB1<const std::string&>& dynCB)
{
  this->channelBtnClickedCB = dynCB;
}

//----------------------------------------------------------------------------

void FuiCurveDefSheet::initWidgets()
{
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

  this->combFrame->setLabel("Definition");
  this->exprField->setLabel("Function of  A, B, C, ...");
  this->exprField->setToolTip("Enter a math expression in the variables A, B, ..., J\n"
			      "and select the Curve corresponding to each variable");
  this->exprField->myField->setAcceptedCB(FFaDynCB1M(FuiCurveDefSheet,this,
						     onExpressionChanged,char*));

  this->fileFrame->setLabel("Definition");
  this->fileBrowseField->setAbsToRelPath("yes");
  this->fileBrowseField->setFileOpenedCB(FFaDynCB2M(FuiCurveDefSheet,this,
						    onFileSelected,const std::string&,int));
  this->fileBrowseField->addDialogFilter("ASCII Curve Definition File",FmGraph::asc(),true);
  this->fileBrowseField->addDialogFilter("nCode DAC File",FmGraph::dac());
  this->fileBrowseField->addDialogFilter("MTS RPC Time history file",FmGraph::rpc());
  this->fileBrowseField->setDialogRememberKeyword("ExternalCurveDefinition");

  this->functionFrame->setLabel("Definition");
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

  this->functionMenu->setBehaviour(FuiQueryInputField::REF_NONE,true);
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
  this->channelSelectUI->setNotesText("Column 1 is the first <u>after</u> the argument column.");
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

void FuiCurveDefSheet::setDefineMode(int inputMode)
{
  if (inputMode == TEMPORAL_RESULT || inputMode == SPATIAL_RESULT) {
    rdbAxes->popUp();
    for (int a = 0; a < FuiCurveDefine::NDIRS; a++)
      if (inputMode == TEMPORAL_RESULT || a == FuiCurveDefine::Y)
        axes[a]->popUp();
      else
        axes[a]->popDown();

    if (inputMode == SPATIAL_RESULT) {
      xAxisFrame->popUp();
      timeRange->popUp();
    }
    else {
      xAxisFrame->popDown();
      timeRange->popDown();
    }
  }
  else
    rdbAxes->popDown();

  if (inputMode == COMB_CURVE)
    combFrame->popUp();
  else
    combFrame->popDown();

  if (inputMode == EXT_CURVE)
    fileFrame->popUp();
  else
    fileFrame->popDown();

  if (inputMode >= INT_FUNCTION)
    functionFrame->popUp();
  else
    functionFrame->popDown();
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
  curveDomain->initWidgets(FFaDynCB1M(FuiCurveAnalysisSheet,this,
                                      onFieldValueChanged,double),
                           FFaDynCB1M(FuiCurveAnalysisSheet,this,
                                      onButtonToggled,bool));

  this->doDftBtn->setLabel("On/Off");
  this->removeCompBtn->setLabel("No 0 Hz component");
  this->resampleBtn->setLabel("Use sample rate");

  this->resampleRateField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->resampleRateField->setAcceptedCB(FFaDynCB1M(FuiCurveAnalysisSheet,this,
						    onFieldValueChanged,double));

  this->doDftBtn->setToggleCB(FFaDynCB1M(FuiCurveAnalysisSheet,this,
					 onDftToggled,bool));
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

void FuiCurveAnalysisSheet::setUIValues(const FuaCurveDefineValues* v)
{
  bool doDft = v->dftFeasable && v->analysis == 3;
  diffBtn->setValue(v->analysis == 1);
  intBtn->setValue(v->analysis == 2);
  doDftBtn->setValue(doDft);
  removeCompBtn->setValue(v->dftRemoveComp);
  resampleBtn->setValue(v->dftResample);
  resampleRateField->setValue(v->dftResampleRate);
  curveDomain->setValues(v->dftDomain,v->dftEntireDomain,doDft);

  doDftBtn->setSensitivity(v->dftFeasable);
  removeCompBtn->setSensitivity(doDft);
  resampleBtn->setSensitivity(doDft);
  resampleRateField->setSensitivity(doDft && v->dftResample);
}

//----------------------------------------------------------------------------

void FuiCurveAnalysisSheet::getUIValues(FuaCurveDefineValues* v) const
{
  v->dftFeasable = doDftBtn->getSensitivity();
  if (diffBtn->getValue())  v->analysis = 1;
  if (intBtn->getValue())   v->analysis = 2;
  if (doDftBtn->getValue()) v->analysis = 3;
  v->dftRemoveComp   = removeCompBtn->getValue();
  v->dftResample     = resampleBtn->getValue();
  v->dftResampleRate = resampleRateField->getDouble();
  v->dftEntireDomain = curveDomain->getValues(v->dftDomain);
}

//----------------------------------------------------------------------------

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

  std::pair<double,double> domain;
  if (!xDomain->getValues(domain)) {
    this->applyVerticalMarkersCB.invoke(domain.first,domain.second);
    this->probablyHasMarkers = true;
  }
}


void FuiCurveInfoSheet::initWidgets()
{
  this->calculateBtn->setLabel("Calculate");
  this->calculateBtn->setActivateCB(FFaDynCB0M(FuiCurveInfoSheet,this,
					       onBtnClicked));

  this->useScaleShiftBtn->setLabel("Use scaled/shifted");

  xDomain->initWidgets();
  xDomain->setValues({ 0.0, 1.0 }, true);

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


void FuiCurveInfoSheet::onBtnClicked()
{
  std::pair<double,double> domain;
  bool entireDomain = xDomain->getValues(domain);
  bool useScaled = this->useScaleShiftBtn->getValue();

  if (this->probablyHasMarkers)
    this->removeVerticalMarkersCB.invoke();
  this->probablyHasMarkers = false;

  Fui::noUserInputPlease();
  this->getCurveStatisticsCB.invoke(useScaled, entireDomain,
                                    domain.first, domain.second);
  Fui::okToGetUserInput();
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
  XScale->setFrameTitles("X Axis Scale and Shift",
                         "Horizontal shift after scale");
  YScale->setFrameTitles("Y Axis Scale and Shift",
                         "Vertical shift after scale");

  XScale->setDataChangedCB(FFaDynCB0M(FFaDynCB0,&dataChangedCB,invoke));
  YScale->setDataChangedCB(FFaDynCB0M(FFaDynCB0,&dataChangedCB,invoke));
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

  this->damageField->setLabel("Damage");
  this->damageField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->damageField->myField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  this->damageField->setValue(0.0);
  this->damageField->myField->setSensitivity(false);

  this->lifeField->setLabel("Life");
  this->lifeField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->lifeField->myField->setDoubleDisplayMode(FFuIOField::AUTO, 6, 1);
  this->lifeField->setValue(0.0);
  this->lifeField->myField->setSensitivity(false);

  this->unitTypeMenu->addOption("Repeats");
  this->unitTypeMenu->addOption("Hours");
  this->unitTypeMenu->addOption("Days");
  this->unitTypeMenu->setOptionSelectedCB(FFaDynCB1M(FuiCurveFatigueSheet,this,
						     recalculateLife,int));

  this->calculateBtn->setLabel("Calculate\nweighted life");
  this->calculateBtn->setActivateCB(FFaDynCB0M(FuiCurveFatigueSheet,this,
					       onButtonClicked));

  this->doRainflowBtn->setLabel("Show rainflow");
  this->doRainflowBtn->setToggleCB(FFaDynCB1M(FuiCurveFatigueSheet,this,
					      onButtonToggled,bool));
  calculateBtn->setMinWidth(doRainflowBtn->getWidthHint()+2);

  timeDomain->initWidgets(FFaDynCB1M(FuiCurveFatigueSheet,this,
                                     onFieldValueChanged,double),
                          FFaDynCB1M(FuiCurveFatigueSheet,this,
                                     onButtonToggled,bool));
  timeDomain->setValues({ 0.0, 1.0 }, true);
}


void FuiCurveFatigueSheet::calculateCurveDamage(bool eventWeighted)
{
  std::pair<double,double> domain;
  bool entireDomain = timeDomain->getValues(domain);

  Fui::noUserInputPlease();
  this->calculateCurveDamageCB.invoke(eventWeighted,entireDomain,
                                      domain.first,domain.second);
  Fui::okToGetUserInput();

  if (!eventWeighted)
    this->delayDamage = false;
}


void FuiCurveFatigueSheet::onTabSelected(FFuComponentBase* selectedSheet)
{
  if (selectedSheet == this && this->delayDamage)
    this->calculateCurveDamage();
}


void FuiCurveFatigueSheet::setDataChangedCB(const FFaDynCB0& aCB)
{
  this->dataChangedCB = aCB;
  this->snSelector->setDataChangedCB(aCB);
}


void FuiCurveFatigueSheet::setCalculateCurveDamageCB(const FFaDynCB4<bool,bool,double,double>& aCB)
{
  this->calculateCurveDamageCB = aCB;
}


void FuiCurveFatigueSheet::getUIValues(FuaCurveDefineValues* v) const
{
  this->snSelector->getValues(v->fatigueSNStd,v->fatigueSNCurve);
  v->fatigueGateValue = this->gateValueField->getValue();
  if (this->doRainflowBtn->getValue()) v->analysis = 4;
  v->fatigueFeasable = this->snSelector->getSensitivity();
  v->fatigueLifeUnit = this->unitTypeMenu->getSelectedOption();
  v->fatigueEntireDomain = timeDomain->getValues(v->fatigueDomain);
}


void FuiCurveFatigueSheet::setUIValues(const FuaCurveDefineValues* v,
				       bool recalculateDamage)
{
  this->snSelector->setValues(v->fatigueSNStd,v->fatigueSNCurve);
  this->gateValueField->setValue(v->fatigueGateValue);
  this->doRainflowBtn->setValue(v->analysis == 4);
  this->unitTypeMenu->selectOption(v->fatigueLifeUnit);
  timeDomain->setValues(v->fatigueDomain,v->fatigueEntireDomain);

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
