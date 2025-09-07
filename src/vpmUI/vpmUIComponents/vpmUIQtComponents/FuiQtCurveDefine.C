// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtCurveDefine.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtCurveAxisDefinition.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtScaleShiftWidget.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtSNCurveSelector.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtFileBrowseField.H"
#include "FFuLib/FFuQtComponents/FFuQtScrolledListDialog.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtTabbedWidgetStack.H"
#include "FFuLib/FFuQtComponents/FFuQtSpinBox.H"
#include "FFuLib/FFuQtComponents/FFuQtRadioButton.H"
#include "FFuLib/FFuQtComponents/FFuQtColorComboBox.H"

//----------------------------------------------------------------------------

FuiQtCurveDefine::FuiQtCurveDefine(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  legendField = new FFuQtLabelField();
  legendButton = new FFuQtToggleButton();
  tabStack = new FFuQtTabbedWidgetStack(NULL);

  curveDefSheet = new FuiQtCurveDefSheet();
  curveAnalysisSheet = new FuiQtCurveAnalysisSheet();
  curveScaleSheet = new FuiQtCurveScaleSheet();
  appearanceSheet = new FuiQtCurveAppearanceSheet();
  infoSheet = new FuiQtCurveInfoSheet();
  fatigueSheet = new FuiQtCurveFatigueSheet();

  this->initWidgets();

  QWidget* qLegend = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qLegend);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(legendField->getQtWidget(),1);
  layout->addWidget(legendButton->getQtWidget());

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qLegend);
  layout->addWidget(tabStack->getQtWidget(),1);
}


void FuiQtCurveDefine::setVisible(bool visible)
{
  if (visible != this->isVisible())
    this->rememberSelectedTab(visible);
  this->QWidget::setVisible(visible);
}


class FuiQtCurveDomain : public FFuQtLabelFrame, public FuiCurveDomain
{
public:
  FuiQtCurveDomain(const char* title, const char* name = "FuiQtCurveDomain")
    : FFuQtLabelFrame(NULL)
  {
    this->setTitle(title);
    this->setObjectName(name);

    startField = new FFuQtLabelField();
    endField   = new FFuQtLabelField();
    entireBtn  = new FFuQtToggleButton();

    QBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(5,0,5,5);
    layout->setSpacing(20);
    layout->addWidget(startField->getQtWidget(),1);
    layout->addWidget(endField->getQtWidget(),1);
    layout->addWidget(entireBtn->getQtWidget());
  }
};


class FuiQtRDBCurveAxes : public FFuQtWidget
{
public:
  FuiQtRDBCurveAxes(const char* name) : FFuQtWidget(NULL,name) {}
};


FuiQtCurveDefSheet::FuiQtCurveDefSheet(const char* name)
  : FFuQtWidget(NULL,name)
{
  rdbResultPlotRadio = new FFuQtRadioButton();
  combCurvePlotRadio = new FFuQtRadioButton();
  extCurvePlotRadio = new FFuQtRadioButton();
  intFunctionPlotRadio = new FFuQtRadioButton();

  rdbAxes = new FuiQtRDBCurveAxes("RDB curve axes");
  axes[0] = new FuiQtCurveAxisDefinition(NULL,"X Axis");
  axes[1] = new FuiQtCurveAxisDefinition(NULL,"Y Axis");
  xAxisFrame = new FFuQtLabelFrame();
  xAxisFrame->setLabel("X Axis");
  spaceOper = new FFuQtOptionMenu();
  timeRange = new FuiQtCurveTimeRange(NULL);

  combFrame = new FFuQtLabelFrame();
  exprField = new FFuQtLabelField();

  fileFrame = new FFuQtLabelFrame();
  fileBrowseField = new FFuQtFileBrowseField(NULL);
  channelField = new FFuQtLabelField();
  channelBtn = new FFuQtPushButton();

  channelSelectUI = new FFuQtScrolledListDialog(NULL,true);

  functionFrame = new FFuQtLabelFrame();
  startXField = new FFuQtLabelField();
  stopXField = new FFuQtLabelField();
  incXField = new FFuQtLabelField();
  functionMenu = new FuiQtQueryInputField(NULL,"Function");
  useSmartPointsBtn = new FFuQtToggleButton();

  completeLabel = new FFuQtLabel();
  reloadCurveButton = new FFuQtPushButton();
  autoExportToggle = new FFuQtToggleButton();

  this->initWidgets();

  QGroupBox* qSourceFrame = new QGroupBox("Source");
  QBoxLayout* layout = new QVBoxLayout(qSourceFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(rdbResultPlotRadio->getQtWidget());
  layout->addWidget(combCurvePlotRadio->getQtWidget());
  layout->addWidget(extCurvePlotRadio->getQtWidget());
  layout->addWidget(intFunctionPlotRadio->getQtWidget());

  layout = new QVBoxLayout(xAxisFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  layout->addStretch(1);
  layout->addWidget(spaceOper->getQtWidget());

  QWidget* qAxes = new QWidget();
  layout = new QHBoxLayout(qAxes);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(xAxisFrame->getQtWidget(),1);
  layout->addWidget(axes[0]->getQtWidget(),1);
  layout->addWidget(axes[1]->getQtWidget(),1);

  layout = new QVBoxLayout(rdbAxes->getQtWidget());
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qAxes,1);
  layout->addWidget(timeRange->getQtWidget());

  layout = new QVBoxLayout(combFrame->getQtWidget());
  layout->setContentsMargins(5,5,5,5);
  layout->addWidget(exprField->getQtWidget());

  QWidget* qChannel = new QWidget();
  layout = new QHBoxLayout(qChannel);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(channelField->getQtWidget(),1);
  layout->addWidget(channelBtn->getQtWidget());

  layout = new QVBoxLayout(fileFrame->getQtWidget());
  layout->setContentsMargins(5,5,5,5);
  layout->addWidget(fileBrowseField->getQtWidget());
  layout->addWidget(qChannel);

  QWidget* qFuncDomain = new QWidget();
  layout = new QHBoxLayout(qFuncDomain);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(startXField->getQtWidget());
  layout->addWidget(stopXField->getQtWidget());
  layout->addWidget(incXField->getQtWidget());
  layout->addWidget(useSmartPointsBtn->getQtWidget());

  layout = new QVBoxLayout(functionFrame->getQtWidget());
  layout->setContentsMargins(5,5,5,5);
  layout->addWidget(qFuncDomain);
  layout->addWidget(functionMenu->getQtWidget());

  QWidget* qSource = new QWidget();
  layout = new QHBoxLayout(qSource);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qSourceFrame);
  layout->addWidget(rdbAxes->getQtWidget(),1);
  layout->addWidget(combFrame->getQtWidget(),1);
  layout->addWidget(fileFrame->getQtWidget(),1);
  layout->addWidget(functionFrame->getQtWidget(),1);

  QWidget* qBottom = new QWidget();
  layout = new QHBoxLayout(qBottom);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(completeLabel->getQtWidget(),1);
  layout->addWidget(reloadCurveButton->getQtWidget());
  layout->addWidget(autoExportToggle->getQtWidget());

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(5,5,5,5);
  layout->addWidget(qSource,1);
  layout->addWidget(qBottom);
}


void FuiQtCurveDefSheet::setNoComps(unsigned int nc)
{
  char label[3] = "A:";
  if (!curveComps.empty())
    label[0] += curveComps.size()-1;

  QLayout* layout = combFrame->getQtWidget()->layout();

  curveComps.reserve(nc);
  while (curveComps.size() < nc)
  {
    curveComps.push_back(new FuiQtQueryInputField(NULL,label));
    layout->addWidget(curveComps.back()->getQtWidget());
    ++label[0];
  }
}


void FuiQtCurveDefSheet::setVisible(bool visible)
{
  bool wasvisible = this->isVisible();
  this->QWidget::setVisible(visible);

  if (wasvisible && !visible)
    channelSelectUI->popDown();
}


FuiQtCurveAnalysisSheet::FuiQtCurveAnalysisSheet(const char* name)
  : FFuQtWidget(NULL,name)
{
  curveDomain = new FuiQtCurveDomain("Time domain");
  doDftBtn = new FFuQtToggleButton();
  removeCompBtn = new FFuQtToggleButton();
  resampleBtn = new FFuQtToggleButton();
  resampleRateField = new FFuQtIOField();

  diffBtn = new FFuQtToggleButton();
  intBtn  = new FFuQtToggleButton();

  this->initWidgets();

  QWidget* qResample = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qResample);
  layout->setContentsMargins(0,5,0,0);
  layout->addWidget(removeCompBtn->getQtWidget());
  layout->addSpacing(20);
  layout->addWidget(resampleBtn->getQtWidget());
  layout->addWidget(resampleRateField->getQtWidget(),1);

  QGroupBox* qLeft = new QGroupBox("Fourier transform");
  layout = new QVBoxLayout(qLeft);
  layout->setContentsMargins(5,5,5,5);
  layout->addWidget(doDftBtn->getQtWidget());
  layout->addWidget(curveDomain->getQtWidget());
  layout->addWidget(qResample);

  QWidget* qRight = new QWidget();
  layout = new QVBoxLayout(qRight);
  layout->addWidget(diffBtn->getQtWidget());
  layout->addWidget(intBtn->getQtWidget());

  layout = new QHBoxLayout(this);
  layout->setContentsMargins(5,3,0,0);
  layout->addWidget(qLeft,1);
  layout->addWidget(qRight);
}


FuiQtCurveAppearanceSheet::FuiQtCurveAppearanceSheet(const char* name)
  : FFuQtWidget(NULL,name)
{
  curveTypeMenu = new FFuQtOptionMenu();
  curveWidthBox = new FFuQtSpinBox();
  colorChooser  = new FFuQtColorComboBox(NULL);

  curveSymbolMenu  = new FFuQtOptionMenu();
  symbolSizeBox    = new FFuQtSpinBox();
  numSymbolsBox    = new FFuQtSpinBox();
  allSymbolsButton = new FFuQtToggleButton();

  this->initWidgets();

  QGroupBox* qLeft = new QGroupBox("General appearance");
  QGridLayout* layout = new QGridLayout(qLeft);
  layout->setHorizontalSpacing(20);
  layout->setColumnStretch(1,1);
  layout->addWidget(new QLabel("Curve type"), 0,0);
  layout->addWidget(new QLabel("Curve thickness"), 1,0);
  layout->addWidget(new QLabel("Curve color"), 2,0);
  layout->addWidget(curveTypeMenu->getQtWidget(), 0,1);
  layout->addWidget(curveWidthBox->getQtWidget(), 1,1);
  layout->addWidget(colorChooser->getQtWidget(), 2,1);

  QWidget* qNumSymbols = new QWidget();
  QBoxLayout* aLayout = new QHBoxLayout(qNumSymbols);
  aLayout->setContentsMargins(0,0,0,0);
  aLayout->addWidget(numSymbolsBox->getQtWidget(),1);
  aLayout->addWidget(allSymbolsButton->getQtWidget());
  qNumSymbols->setMaximumHeight(numSymbolsBox->getHeightHint());

  QGroupBox* qRight = new QGroupBox("Symbols");
  layout = new QGridLayout(qRight);
  layout->setHorizontalSpacing(20);
  layout->setColumnStretch(1,1);
  layout->addWidget(new QLabel("Symbol type"), 0,0);
  layout->addWidget(new QLabel("Symbol size"), 1,0);
  /* Number of symbols feature not (yet) supported with Qwt 6.1.2.
     The associated widgets are therefore hidden.
  layout->addWidget(new QLabel("Number of symbols"), 2,0);
  */
  layout->addWidget(curveSymbolMenu->getQtWidget(), 0,1);
  layout->addWidget(symbolSizeBox->getQtWidget(), 1,1);
  layout->addWidget(qNumSymbols, 2,1);

  aLayout = new QHBoxLayout(this);
  aLayout->setContentsMargins(5,3,5,0);
  aLayout->addWidget(qLeft,1);
  aLayout->addWidget(qRight,1);
}


FuiQtCurveInfoSheet::FuiQtCurveInfoSheet(const char* name)
  : FFuQtWidget(NULL,name)
{
  rmsField = new FFuQtLabelField();
  avgField = new FFuQtLabelField();
  stdDevField = new FFuQtLabelField();
  integralField = new FFuQtLabelField();
  maxField = new FFuQtLabelField();
  minField = new FFuQtLabelField();

  calculateBtn = new FFuQtPushButton(this);
  useScaleShiftBtn = new FFuQtToggleButton(this);

  xDomain = new FuiQtCurveDomain("X Axis Domain");

  this->initWidgets();

  QGridLayout* layout = new QGridLayout(this);
  layout->setHorizontalSpacing(20);
  layout->addWidget(rmsField->getQtWidget(), 0,0);
  layout->addWidget(avgField->getQtWidget(), 1,0);
  layout->addWidget(stdDevField->getQtWidget(), 0,1);
  layout->addWidget(integralField->getQtWidget(), 1,1);
  layout->addWidget(maxField->getQtWidget(), 0,2);
  layout->addWidget(minField->getQtWidget(), 1,2);
  layout->addWidget(calculateBtn->getQtWidget(), 0,3);
  layout->addWidget(useScaleShiftBtn->getQtWidget(), 1,3);
  layout->addWidget(xDomain->getQtWidget(), 2,0,1,4);
}


FuiQtCurveScaleSheet::FuiQtCurveScaleSheet(const char* name)
  : FFuQtWidget(NULL,name)
{
  XScale = new FuiQtScaleShiftWidget(NULL,'X');
  YScale = new FuiQtScaleShiftWidget(NULL,'Y');

  this->initWidgets();

  QLayout* layout = new QHBoxLayout(this);
  layout->setContentsMargins(3,3,3,3);
  layout->addWidget(XScale->getQtWidget());
  layout->addWidget(YScale->getQtWidget());
}


FuiQtCurveFatigueSheet::FuiQtCurveFatigueSheet(const char* name)
  : FFuQtWidget(NULL,name)
{
  snSelector = new FuiQtSNCurveSelector(NULL);
  gateValueField = new FFuQtLabelField();

  damageField = new FFuQtLabelField();
  lifeField = new FFuQtLabelField();
  unitTypeMenu = new FFuQtOptionMenu();
  calculateBtn = new FFuQtPushButton();
  timeDomain = new FuiQtCurveDomain("Time interval");
  doRainflowBtn = new FFuQtToggleButton();

  this->initWidgets();

  QWidget* qSelector = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qSelector);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(25);
  layout->addWidget(snSelector->getQtWidget(),2);
  layout->addWidget(gateValueField->getQtWidget(),1);

  QGroupBox* qResults = new QGroupBox("Fatigue results");
  layout = new QHBoxLayout(qResults);
  layout->setContentsMargins(5,3,5,5);
  layout->addWidget(damageField->getQtWidget());
  layout->addSpacing(15);
  layout->addWidget(lifeField->getQtWidget());
  layout->addSpacing(15);
  layout->addWidget(new QLabel("Life unit"));
  layout->addWidget(unitTypeMenu->getQtWidget());

  QWidget* qFatigue = new QWidget();
  layout = new QHBoxLayout(qFatigue);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(25);
  layout->addWidget(qResults,1);
  layout->addWidget(calculateBtn->getQtWidget());

  QWidget* qTime = new QWidget();
  layout = new QHBoxLayout(qTime);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(25);
  layout->addWidget(timeDomain->getQtWidget(),1);
  layout->addWidget(doRainflowBtn->getQtWidget());

  layout = new QVBoxLayout(this);
  layout->addWidget(qSelector);
  layout->addWidget(qFatigue);
  layout->addWidget(qTime);
}
