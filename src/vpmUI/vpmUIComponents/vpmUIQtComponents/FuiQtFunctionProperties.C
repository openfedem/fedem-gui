// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

#include "FuiQtFunctionProperties.H"
#include "FuiQtQueryInputField.H"
#include "FuiQtInputSelector.H"
#include "FuiQtThreshold.H"

#include "FFuLib/FFuQtComponents/FFuQtMemo.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtFileBrowseField.H"
#include "FFuLib/FFuQtComponents/FFuQtFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtSpinBox.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtScrolledList.H"
#include "FFuLib/FFuQtComponents/FFuQtScrolledListDialog.H"
#include "FFuLib/FFuQtComponents/FFuQtTabbedWidgetStack.H"


class FuiQtPrmViewFrame : public FFuQtLabelFrame
{
public:
  FuiQtPrmViewFrame(FuiFunctionProperties* own) : owner(own) {}

protected:
  virtual void resizeEvent(QResizeEvent* e)
  {
    this->QWidget::resizeEvent(e);
    owner->placeExpandButton();
  }

private:
  FuiFunctionProperties* owner;
};


FuiQtFunctionProperties::FuiQtFunctionProperties(QWidget* parent)
  : FFuQtWidget(parent,"FuiQtFunctionProperties")
{
  myTypeFrame = new FFuQtLabelFrame();
  myTypeSwitch = new FFuQtOptionMenu();
  myEngineFunction = new FuiQtQueryInputField(NULL);

  QWidget* qLeft = new QWidget();
  myInputSelector = new FuiQtInputSelector(NULL);
  myInputSelectors = new FuiQtInputSelectors(qLeft);

  myOutputToggle = new FFuQtToggleButton();
  myThresholdFrame = new FFuQtLabelFrame();
  myThresholds = new FuiQtThreshold(NULL);
  myThreshold2 = new FuiQtThreshold(this);

  for (int i = 0; i < NPRM; i++)
    if (i == VIEW)
      myParameterFrames[i] = new FuiQtPrmViewFrame(this);
    else if (i != UDWS)
      myParameterFrames[i] = new FFuQtLabelFrame();

  myParameterList = new FFuQtScrolledList();
  myExpandButton = new FFuQtPushButton(myParameterFrames[VIEW]->getQtWidget());

  myExtrapolationSwitch = new FFuQtOptionMenu();

  myXValueInputField = new FFuQtIOField();
  myYValueInputField = new FFuQtIOField();

  myAddButton    = new FFuQtPushButton();
  myDeleteButton = new FFuQtPushButton();

  myExprLabel       = new FFuQtLabel();
  myExprMemo        = new FFuQtMemo();
  myExprApplyButton = new FFuQtPushButton();
  myNumArgLabel     = new FFuQtLabel();
  myNumArgBox       = new FFuQtSpinBox();

  myFileRefQueryField = new FuiQtQueryInputField(NULL);
  myFileBrowseButton  = new FFuQtPushButton();

  myChannelNameField    = new FFuQtIOField();
  myChannelBrowseButton = new FFuQtPushButton();

  myChannelSelectUI = new FFuQtScrolledListDialog(this);

  myScaleFactorField   = new FFuQtIOField();
  myVerticalShiftField = new FFuQtLabelField();
  myZeroAdjustToggle   = new FFuQtToggleButton();

  for (FFuIOField*& fld : myExtFuncFields)
    fld = new FFuQtIOField();

  myJonswapA = new FuiQtJonswapAdvanced(NULL);
  myJonswapB = new FuiQtJonswapBasic(NULL);
  myWaveSpec = new FuiQtWaveSpectrum(NULL);
  myParameterFrames[UDWS] = static_cast<FuiQtWaveSpectrum*>(myWaveSpec);

  const int helpImgWidth = 200;
  const int helpImgHeight = 150;
#ifdef linux64
  const int border = 5;
#else
  const int border = 2;
#endif
  myHelpFrame = new FFuQtFrame();
  myHelpLabel = new FFuQtLabel(myHelpFrame->getQtWidget());
  myHelpLabel->setSizeGeometry(2,2,helpImgWidth,helpImgHeight);

#ifdef FT_HAS_PREVIEW
  myX0Field = new FFuQtIOField();
  myXNField = new FFuQtIOField();
  myDXField = new FFuQtIOField();
  myUseSmartPointsToggle = new FFuQtToggleButton();
  myPreviewButton = new FFuQtPushButton();

  FFuQtFrame* preview = new FFuQtFrame();
  QGridLayout* pgl = new QGridLayout(preview);
  pgl->setContentsMargins(0,0,0,0);
  pgl->setColumnStretch(0,1);
  pgl->addWidget(new QLabel("Domain"), 0,0);
  pgl->addWidget(new QLabel("Increment"), 1,0);
  pgl->addWidget(myX0Field->getQtWidget(), 0,1);
  pgl->addWidget(myXNField->getQtWidget(), 0,2);
  pgl->addWidget(myDXField->getQtWidget(), 1,2);
  pgl->addWidget(myUseSmartPointsToggle->getQtWidget(), 1,1);
  pgl->addWidget(myPreviewButton->getQtWidget(), 2,2);
#endif

  FFuQtTabbedWidgetStack* qTabStack;
  myTabStack = qTabStack = new FFuQtTabbedWidgetStack(NULL);
  myTabStack->setMaxWidth(helpImgWidth+3+border);
  qTabStack->addTab(myHelpFrame->getQtWidget(),"Parameter Help");
#ifdef FT_HAS_PREVIEW
  qTabStack->addTab(preview,"Preview");
#endif

  this->initWidgets();

  QBoxLayout* layout = new QVBoxLayout(myTypeFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myTypeSwitch->getQtWidget(),0,Qt::AlignTop);
  layout->addWidget(myEngineFunction->getQtWidget());

  QWidget* qNumArgs = new QWidget();
  layout = new QHBoxLayout(qNumArgs);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(myNumArgLabel->getQtWidget());
  layout->addWidget(myNumArgBox->getQtWidget());

  layout = new QVBoxLayout(qLeft);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(1);
  layout->addWidget(myTypeFrame->getQtWidget());
  layout->addWidget(myInputSelector->getQtWidget(),1);
  layout->addWidget(qNumArgs);
  layout->addWidget(myJonswapA->getQtWidget());
  layout->addWidget(myOutputToggle->getQtWidget());

  QWidget* qXYfields = new QWidget();
  QGridLayout* gl = new QGridLayout(qXYfields);
  gl->setContentsMargins(0,0,0,0);
  gl->setVerticalSpacing(0);
  gl->addWidget(new QLabel("X"), 0,0);
  gl->addWidget(new QLabel("Y"), 0,1);
  gl->addWidget(new QLabel("Extrapolation"), 0,4);
  gl->addWidget(myXValueInputField->getQtWidget(), 1,0);
  gl->addWidget(myYValueInputField->getQtWidget(), 1,1);
  gl->addWidget(myAddButton->getQtWidget(), 1,2);
  gl->addWidget(myDeleteButton->getQtWidget(), 1,3);
  gl->addWidget(myExtrapolationSwitch->getQtWidget(), 1,4);

  layout = new QVBoxLayout(myParameterFrames[LIST]->getQtWidget());
  layout->setContentsMargins(3,0,3,3);
  layout->addWidget(myParameterList->getQtWidget());
  layout->addWidget(qXYfields);

  QGroupBox* qVerticalShiftFrame = new QGroupBox("Vertical shift after scale");
  layout = new QVBoxLayout(qVerticalShiftFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myZeroAdjustToggle->getQtWidget());
  layout->addWidget(myVerticalShiftField->getQtWidget());

  gl = new QGridLayout(myParameterFrames[FILE]->getQtWidget());
  gl->setContentsMargins(5,0,5,5);
  gl->setColumnStretch(1,1);
  gl->addWidget(new QLabel("File/Reference"), 0,0);
  gl->addWidget(myFileRefQueryField->getQtWidget(), 0,1);
  gl->addWidget(myFileBrowseButton->getQtWidget(), 0,2);
  gl->addWidget(new QLabel("Channel"), 1,0);
  gl->addWidget(myChannelNameField->getQtWidget(), 1,1);
  gl->addWidget(myChannelBrowseButton->getQtWidget(), 1,2);
  gl->addWidget(new QLabel("Scale"), 2,0);
  gl->addWidget(myScaleFactorField->getQtWidget(), 2,1);
  gl->addWidget(qVerticalShiftFrame, 3,0,1,3);

  gl = new QGridLayout(myParameterFrames[EXTF]->getQtWidget());
  gl->setContentsMargins(5,0,5,5);
  gl->setColumnStretch(1,1);
  gl->addWidget(new QLabel("Channel"),          0,0);
  gl->addWidget(new QLabel("Scale"),            1,0);
  gl->addWidget(new QLabel("Additional shift"), 2,0);
  for (int row = 0; row < 3; row++)
    gl->addWidget(myExtFuncFields[row]->getQtWidget(), row,1);

  QWidget* qExprBtn = new QWidget();
  layout = new QHBoxLayout(qExprBtn);
  layout->setContentsMargins(2,0,2,0);
  layout->addWidget(myExprLabel->getQtWidget(),1);
  layout->addWidget(myExprApplyButton->getQtWidget());

  layout = new QVBoxLayout(myParameterFrames[MATH]->getQtWidget());
  layout->setContentsMargins(3,0,3,3);
  layout->addWidget(myExprMemo->getQtWidget(),1);
  layout->addWidget(qExprBtn);

  layout = new QVBoxLayout(myThresholdFrame->getQtWidget());
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(myThresholds->getQtWidget());

  layout = new QHBoxLayout(this);
  layout->addWidget(qLeft,1);
  layout->addWidget(myJonswapB->getQtWidget(),2);
  for (FFuLabelFrame*& frame : myParameterFrames)
    layout->addWidget(frame->getQtWidget(),2);
  layout->addWidget(myThresholdFrame->getQtWidget());
  layout->addWidget(qTabStack);
}


FuiQtFunctionProperties::FuiQtJonswapAdvanced::FuiQtJonswapAdvanced(QWidget* parent)
  : FFuQtLabelFrame(parent)
{
  this->setObjectName("FuiQtJonswapAdvanced");
  this->setLabel("Advanced");

  mySpectralPeakednessField = new FFuQtLabelField();
  mySpectralPeakednessToggle = new FFuQtToggleButton();
  myWaveComponentsField = new FFuQtLabelField();
  myWaveDirsField = new FFuQtLabelField();
  mySpreadExpField = new FFuQtLabelField();
  myRandomSeedField = new FFuQtLabelField();

  QWidget* qAdvanced = new QWidget();
  QLayout* layout = new QHBoxLayout(qAdvanced);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(mySpreadExpField->getQtWidget());
  layout->addWidget(myRandomSeedField->getQtWidget());

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(mySpectralPeakednessField->getQtWidget());
  layout->addWidget(mySpectralPeakednessToggle->getQtWidget());
  layout->addWidget(myWaveComponentsField->getQtWidget());
  layout->addWidget(myWaveDirsField->getQtWidget());
  layout->addWidget(qAdvanced);
}


FuiQtFunctionProperties::FuiQtJonswapBasic::FuiQtJonswapBasic(QWidget* parent)
  : FFuQtWidget(parent,"FuiQtJonswapBasic")
{
  myHsField = new FFuQtLabelField();
  myTpField = new FFuQtLabelField();

  myCutOffToggle = new FFuQtToggleButton();
  myTLowField = new FFuQtLabelField();
  myTHighField = new FFuQtLabelField();

  QGroupBox* qBasicFrame = new QGroupBox("Basic");
  QLayout* layout = new QVBoxLayout(qBasicFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myHsField->getQtWidget());
  layout->addWidget(myTpField->getQtWidget());

  QWidget* qCutOff = new QWidget();
  layout = new QHBoxLayout(qCutOff);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(myTLowField->getQtWidget());
  layout->addWidget(myTHighField->getQtWidget());

  QGroupBox* qCutOffFrame = new QGroupBox("Period cut-off values");
  layout = new QVBoxLayout(qCutOffFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myCutOffToggle->getQtWidget());
  layout->addWidget(qCutOff);

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qBasicFrame);
  layout->addWidget(qCutOffFrame);
}


FuiQtFunctionProperties::FuiQtWaveSpectrum::FuiQtWaveSpectrum(QWidget* parent)
  : FFuQtLabelFrame(parent)
{
  this->setObjectName("FuiQtWaveSpectrum");

  myFileField = new FFuQtFileBrowseField(NULL);
  myRandomSeedField = new FFuQtLabelField();
  myHsField = new FFuQtLabelField();
  myTpField = new FFuQtLabelField();

  QLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(myFileField->getQtWidget());
  layout->addWidget(myRandomSeedField->getQtWidget());
  layout->addWidget(myHsField->getQtWidget());
  layout->addWidget(myTpField->getQtWidget());
}
