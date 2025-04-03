// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FuiQtFunctionProperties.H"
#include "FuiQtQueryInputField.H"
#include "FuiQtInputSelector.H"
#include "FuiQtThreshold.H"

#include "FFuLib/FFuQtComponents/FFuQtMemo.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtTable.H"
#include "FFuLib/FFuQtComponents/FFuQtSpinBox.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtTableView.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQ3ScrolledList.H"
#include "FFuLib/FFuQtComponents/FFuQtScrolledListDialog.H"
#include "FFuLib/FFuQtComponents/FFuQtTabbedWidgetStack.H"


FuiQtFunctionProperties::FuiQtFunctionProperties(QWidget* parent,
						 int xpos, int ypos,
						 int width, int height,
						 const char* name)
  : FFuQtMultUIComponent(parent,xpos,ypos,width,height,name)
{
  myTypeFrame = new FFuQtLabelFrame(this);
  myTypeSwitch = new FFuQtOptionMenu(this);
  myEngineFunction = new FuiQtQueryInputField(this);
  myInputSelector = new FuiQtInputSelector(this);
  myArgumentTable = new FFuQtTable(this);

  FFuQtLabelFrame* qlf;
  myOutputToggle = new FFuQtToggleButton(this);
  myThresholdFrame = qlf = new FFuQtLabelFrame(this);
  myThresholds = new FuiQtThreshold(qlf);
  myThreshold2 = new FuiQtThreshold(this);

  myParameterFrame = new FFuQtLabelFrame(this);
  myParameterView = new FFuQtTableView(this);
  myParameterList = new FFuQ3ScrolledList(this);
  myExpandButton = new FFuQtPushButton(this);

  myExtrapolationLabel = new FFuQtLabel(this);
  myExtrapolationSwitch = new FFuQtOptionMenu(this);

  myXValueInputField = new FFuQtIOField(this);
  myYValueInputField = new FFuQtIOField(this);
  myXLabel = new FFuQtLabel(this);
  myYLabel = new FFuQtLabel(this);

  myAddButton    = new FFuQtPushButton(this);
  myDeleteButton = new FFuQtPushButton(this);

  myExprLabel       = new FFuQtLabel(this);
  myExprMemo        = new FFuQtMemo(this);
  myExprApplyButton = new FFuQtPushButton(this);
  myNumArgLabel     = new FFuQtLabel(this);
  myNumArgBox       = new FFuQtSpinBox(this);

  myChannelNameField = new FFuQtLabelField(this);
  myChannelBrowseButton = new FFuQtPushButton(this);

  myChannelSelectUI = new FFuQtScrolledListDialog(this);

  myScaleFactorField   = new FFuQtLabelField(this);
  myVerticalShiftFrame = new FFuQtLabelFrame(this);
  myVerticalShiftField = new FFuQtLabelField(this);
  myZeroAdjustToggle   = new FFuQtToggleButton(this);

  myFileBrowseButton = new FFuQtPushButton(this);
  myFileRefQueryField = new FuiQtQueryInputField(this);
  myFileBrowseLabel = new FFuQtLabel(this);
  myActualFileInfoLabel = new FFuQtLabelField(this);

  FFuQtFrame* qhlab;
  myHelpFrame = qhlab = new FFuQtFrame(this);
  myHelpLabel = new FFuQtLabel(qhlab);

#ifdef FT_HAS_PREVIEW
  FFuQtMultUIComponent* preview = new FFuQtMultUIComponent();
  myPreviewButton = new FFuQtPushButton(preview);
  myX0Label = new FFuQtLabel(preview);
  myDXLabel = new FFuQtLabel(preview);
  myX0Field = new FFuQtIOField(preview);
  myXNField = new FFuQtIOField(preview);
  myDXField = new FFuQtIOField(preview);
  myUseSmartPointsToggle = new FFuQtToggleButton(preview);
#endif

  myTabStack = new FFuQtTabbedWidgetStack(this);
  myTabStack->addTabPage(myHelpFrame, "Parameter Help");
#ifdef FT_HAS_PREVIEW
  myTabStack->addTabPage(preview, "Preview");
#endif

  // Jonswap wave spectrum
  myJonswapAdvancedFrame = new FFuQtLabelFrame(this);
  myJonswapSpectralPeakednessField = new FFuQtLabelField(this);
  myJonswapSpectralPeakednessToggle = new FFuQtToggleButton(this);
  myJonswapWaveComponentsField = new FFuQtLabelField(this);
  myJonswapRandomSeedField = new FFuQtLabelField(this);
  myJonswapWaveDirsField = new FFuQtLabelField(this);
  myJonswapSpreadExpField = new FFuQtLabelField(this);
  myJonswapBasicFrame = new FFuQtLabelFrame(this);
  myJonswapHsField = new FFuQtLabelField(this);
  myJonswapTpField = new FFuQtLabelField(this);
  myJonswapCutOffFrame = new FFuQtLabelFrame(this);
  myJonswapCutOffToggle = new FFuQtToggleButton(this);
  myJonswapTLowField = new FFuQtLabelField(this);
  myJonswapTHighField = new FFuQtLabelField(this);

  this->initWidgets();
}


void FuiQtFunctionProperties::setNoArgs(size_t narg)
{
  QWidget* qPtr = dynamic_cast<QWidget*>(myArgumentTable);

  size_t oarg = myArguments.size();
  myArguments.resize(narg,NULL);

  for (size_t i = oarg; i < narg; i++)
    myArguments[i] = new FuiQtInputSelector(qPtr);
}
