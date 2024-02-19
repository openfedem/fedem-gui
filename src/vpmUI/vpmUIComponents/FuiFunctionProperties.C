// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>

#include "vpmUI/vpmUIComponents/FuiFunctionProperties.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "FFuLib/FFuTable.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuMemo.H"
#include "FFuLib/FFuFrame.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuSpinBox.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuTableView.H"
#include "FFuLib/FFuScrolledList.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuScrolledListDialog.H"
#include "FFuLib/FFuTabbedWidgetStack.H"
#include "FFaLib/FFaString/FFaStringExt.H"


FuiFunctionProperties::FuiFunctionProperties()
{
  IAmShowingParameterList = false;
  IAmShowingParameterView = false;
  IAmShowingFileView = false;
  IAmShowingTypeSwitch = false;
  IAmShowingMathExpr = false;
  IAmShowingCurvePreview = false;
  IAmShowingArgument = false;
  IAmShowingOutput = false;
  IAmShowingLinkToFields = false;
  IAmShowingHelpPixmap = false;
  IAmShowingJonswap = false;
  myMinParameterWidth = 0;

  IAmSensitive = IAmAllowingTopolChange = true;
}


void FuiFunctionProperties::initWidgets()
{
  // Function type and linkage

  myTypeFrame->setLabel("Function Type");
  myTypeSwitch->setOptionSelectedCB(FFaDynCB1M(FFaDynCB1<int>,&myFuncTypeSwitchedCB,invoke,int));

  myEngineFunction->setRefSelectedCB(FFaDynCB1M(FuiFunctionProperties,this,
						onEngineFuncChanged,int));
  myEngineFunction->popDown();

  // Argument

  myInputSelector->setValuesChangedCB(FFaDynCB1M(FuiFunctionProperties,this,
						 onInputSelectorValuesChanged,
						 FuiInputSelectorValues&));
  myInputSelector->setSetDBValuesCB(FFaDynCB1M(FuiFunctionProperties,this,
					       onInputSelectorSetDBValues,
					       FuiInputSelectorValues&));
  myInputSelector->setPickCB(FFaDynCB0M(FuiFunctionProperties,this,
					onSensorPicked));
  myInputSelector->popDown();

  // Arguments for multi-variable functions

  myArgumentTable->showRowHeader(false);
  myArgumentTable->showColumnHeader(false);
  myArgumentTable->setNumberColumns(1);
  myArgumentTable->setSelectionPolicy(FFuTable::NO_SELECTION);
  myArgumentTable->popDown();

  // Output sensor toggle

  myOutputToggle->setLabel("Use as output sensor");
  myOutputToggle->setToggleCB(FFaDynCB1M(FuiFunctionProperties,this,
                                         onOutputToggled,bool));
  myOutputToggle->popDown();

  // Threshold fields

  myThresholdFrame->setLabel("Event Threshold");
  myThresholdFrame->popDown();

  myThresholds->setToggleCB(FFaDynCB1M(FuiFunctionProperties,this,
                                       onToggleChanged,bool));
  myThresholds->setValueChangedCB(FFaDynCB1M(FuiFunctionProperties,this,
                                             onParameterChanged,char*));
  myThresholds->setSelectionChangedCB(FFaDynCB1M(FuiFunctionProperties,this,
                                                 onSelectionChanged,int));
  myThreshold2->setToggleCB(FFaDynCB1M(FuiFunctionProperties,this,
                                       onToggleChanged,bool));
  myThreshold2->setValueChangedCB(FFaDynCB1M(FuiFunctionProperties,this,
                                             onParameterChanged,char*));
  myThreshold2->setSelectionChangedCB(FFaDynCB1M(FuiFunctionProperties,this,
                                                 onSelectionChanged,int));
  myThreshold2->popDown();

  // Parameter view

  myParameterFrame->setLabel("Parameters");

  myExtrapolationLabel->setLabel("Extrapolation");
  myExtrapolationSwitch->setOptionSelectedCB(FFaDynCB1M(FuiFunctionProperties,this,
							onSelectionChanged,int));
  myExtrapolationSwitch->addOption("None");
  myExtrapolationSwitch->addOption("Flat");
  myExtrapolationSwitch->addOption("Linear");

  myParameterView->stretchContentsWidth(true);
  myParameterView->popDown();

  myExpandButton->setActivateCB(FFaDynCB0M(FuiFunctionProperties,this,
					   onExpandButtonActivated));
  myExpandButton->popDown();

  // Multi points UI

  myParameterList->setBrowseSelectCB(FFaDynCB1M(FuiFunctionProperties,this,
						onParameterSelected,int));
  myParameterList->setDeleteButtonCB(FFaDynCB0M(FuiFunctionProperties,this,
						onDeleteButtonActivated));
  myParameterList->setClearCB(FFaDynCB0M(FFaDynCB0,&myClearAllCB,invoke));
  myParameterList->setToolTip("Use Ctrl+V to paste numbers into this function\n"
			      "Use Ctrl+X to clear all numbers");

  myXLabel->setLabel("X");
  myXValueInputField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myXValueInputField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
                                               onXAccepted,double));
  myYLabel->setLabel("Y");
  myYValueInputField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myYValueInputField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
                                               onYAccepted,double));
  myAddButton->setLabel("+");
  myAddButton->setActivateCB(FFaDynCB0M(FuiFunctionProperties,this,
					onAddButtonActivated));
  myDeleteButton->setLabel("-");
  myDeleteButton->setActivateCB(FFaDynCB0M(FuiFunctionProperties,this,
					   onDeleteButtonActivated));
  myXLabel->popDown();
  myYLabel->popDown();

  myExtrapolationLabel->popDown();
  myExtrapolationSwitch->popDown();

  myXValueInputField->popDown();
  myYValueInputField->popDown();

  myAddButton->popDown();
  myDeleteButton->popDown();

  myParameterList->popDown();

  // Math expression

  myExprLabel->setLabel("Enter a function of x");
  myExprApplyButton->setLabel("Apply");
  myExprApplyButton->setActivateCB(FFaDynCB0M(FuiFunctionProperties,this,
					      onExprApplyButtonActivated));
  myNumArgLabel->setLabel("Number of function arguments");
  myNumArgBox->setMinMax(1,4);
  myNumArgBox->setValueChangedCB(FFaDynCB1M(FuiFunctionProperties,this,
					    onEngineFuncChanged,int));

  myExprLabel->popDown();
  myExprMemo->popDown();
  myExprApplyButton->popDown();
  myNumArgLabel->popDown();
  myNumArgBox->popDown();

  // File function

  myChannelNameField->setLabel("Channel");
  myChannelNameField->setSensitivity(false);
  myChannelNameField->setLabelMargin(0);

  myChannelBrowseButton->setLabel("...");
  myChannelBrowseButton->setActivateCB(FFaDynCB0M(FFaDynCB0,&myGetChannelListCB,invoke));

  myChannelSelectUI->setTitle("Channel list");
  myChannelSelectUI->setApplyButtonClickedCB(FFaDynCB1M(FuiFunctionProperties,this,
							onChannelDialogApply,int));
  myChannelSelectUI->setOkButtonClickedCB(FFaDynCB1M(FuiFunctionProperties,this,
						     onChannelDialogOK,int));
  myChannelSelectUI->setCancelButtonClickedCB(FFaDynCB1M(FuiFunctionProperties,this,
							 onChannelDialogCancel,int));

  myScaleFactorField->setLabel("Scale");
  myScaleFactorField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myScaleFactorField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
					       onParameterValueChanged,double));
  myScaleFactorField->setLabelMargin(0);

  myVerticalShiftFrame->setLabel("Vertical shift after scale");

  myVerticalShiftField->setLabel("Additional shift  ");
  myVerticalShiftField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myVerticalShiftField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
						 onParameterValueChanged,double));
  myVerticalShiftField->setLabelMargin(0);

  myZeroAdjustToggle->setLabel("Shift function to zero out start value");
  myZeroAdjustToggle->setToggleCB(FFaDynCB1M(FuiFunctionProperties,this,
					     onToggleChanged,bool));

  myChannelNameField->popDown();
  myScaleFactorField->popDown();
  myVerticalShiftFrame->popDown();
  myVerticalShiftField->popDown();
  myZeroAdjustToggle->popDown();

  // File reference

  myFileBrowseButton->setLabel("...");
  myFileBrowseButton->setActivateCB(FFaDynCB0M(FuiFunctionProperties,this,
					       onFileBrowseActivated));
  myFileBrowseButton->popDown();

  myFileBrowseLabel->setLabel("File/Reference");
  myFileBrowseLabel->popDown();

  myFileRefQueryField->setChangedCB(FFaDynCB1M(FuiFunctionProperties,this,
					       onFileQIChanged,FuiQueryInputField*));
  myFileRefQueryField->turnButtonOff(true);
  myFileRefQueryField->setBehaviour(FuiQueryInputField::REF_TEXT);
  myFileRefQueryField->setEditSensitivity(false);

  myActualFileInfoLabel->setLabel("Actual file");
  myActualFileInfoLabel->popDown();

  // Help and

  myHelpFrame->setLook(FFuFrame::PANEL_SUNKEN);

  // Preview

#ifdef FT_HAS_PREVIEW
  myPreviewButton->setLabel("Show");
  myPreviewButton->setActivateCB(FFaDynCB0M(FFaDynCB0,&myPreviewCB,invoke));
  myPreviewButton->popUp();

  myX0Label->setLabel("Domain");
  myDXLabel->setLabel("Increment ");
  myUseSmartPointsToggle->setLabel("Auto");

  myX0Field->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myX0Field->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
				      onParameterValueChanged,double));
  myXNField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myXNField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
				      onParameterValueChanged,double));
  myDXField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myDXField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
				      onParameterValueChanged,double));
  myUseSmartPointsToggle->setToggleCB(FFaDynCB1M(FuiFunctionProperties,this,
                                                 onToggleChanged,bool));
#endif

  // Jonswap wave spectrum

  myJonswapAdvancedFrame->setLabel("Advanced");

  myJonswapSpectralPeakednessField->setLabel("Spectral peakedness, <font face='Symbol'><font size='+1'>g</font></font>");
  myJonswapSpectralPeakednessField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myJonswapSpectralPeakednessField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
                                                             onParameterValueChanged,double));
  myJonswapSpectralPeakednessField->setLabelMargin(0);

  myJonswapSpectralPeakednessToggle->setLabel("Use DNV recommendation");
  myJonswapSpectralPeakednessToggle->setToggleCB(FFaDynCB1M(FuiFunctionProperties,this,
                                                            onToggleChanged,bool));

  myJonswapWaveComponentsField->setLabel("Number of wave components");
  myJonswapWaveComponentsField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  myJonswapWaveComponentsField->myField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,onSelectionChanged,int));
  myJonswapWaveComponentsField->setLabelMargin(0);

  myJonswapRandomSeedField->setLabel("Random seed");
  myJonswapRandomSeedField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  myJonswapRandomSeedField->myField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,onSelectionChanged,int));
  myJonswapRandomSeedField->setLabelMargin(2);

  myJonswapWaveDirsField->setLabel("Number of wave directions");
  myJonswapWaveDirsField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  myJonswapWaveDirsField->myField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,onSelectionChanged,int));
  myJonswapWaveDirsField->setLabelMargin(0);

  myJonswapSpreadExpField->setLabel("Spreading exp.");
  myJonswapSpreadExpField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  myJonswapSpreadExpField->myField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,onSelectionChanged,int));
  myJonswapSpreadExpField->setLabelMargin(2);

  myJonswapBasicFrame->setLabel("Basic");

  myJonswapHsField->setLabel("Significant wave height, Hs");
  myJonswapHsField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myJonswapHsField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
					       onParameterValueChanged,double));
  myJonswapHsField->setLabelMargin(0);

  myJonswapTpField->setLabel("Spectral peak period, Tp");
  myJonswapTpField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myJonswapTpField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
                                             onParameterValueChanged,double));
  myJonswapTpField->setLabelMargin(0);

  myJonswapCutOffFrame->setLabel("Period cut-off values");

  myJonswapCutOffToggle->setLabel("Auto calculate period cut-off values");
  myJonswapCutOffToggle->setToggleCB(FFaDynCB1M(FuiFunctionProperties,this,
                                                onToggleChanged,bool));

  myJonswapTLowField->setLabel("T<sub>low</sub>");
  myJonswapTLowField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myJonswapTLowField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
					       onParameterValueChanged,double));
  myJonswapTLowField->setLabelMargin(0);

  myJonswapTHighField->setLabel("T<sub>high</sub>");
  myJonswapTHighField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myJonswapTHighField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
					       onParameterValueChanged,double));
  myJonswapTHighField->setLabelMargin(0);

  myJonswapAdvancedFrame->popDown();
  myJonswapSpectralPeakednessField->popDown();
  myJonswapSpectralPeakednessToggle->popDown();
  myJonswapWaveComponentsField->popDown();
  myJonswapRandomSeedField->popDown();
  myJonswapWaveDirsField->popDown();
  myJonswapSpreadExpField->popDown();
  myJonswapBasicFrame->popDown();
  myJonswapHsField->popDown();
  myJonswapTpField->popDown();
  myJonswapCutOffFrame->popDown();
  myJonswapCutOffToggle->popDown();
  myJonswapTLowField->popDown();
  myJonswapTHighField->popDown();

  this->invokeCreateUACB(this);
}


void FuiFunctionProperties::placeWidgets(int width, int height)
{
  int textHeight  = myXLabel->getHeightHint();
  int fieldHeight = myTypeSwitch->getHeightHint();
  int border      = getGridLinePos(height,30);
  int vBorder     = 6;

  int columnSep;
  if (!IAmShowingTypeSwitch && IAmShowingArgument < 1)
    columnSep = vBorder;
  else if (width < 300)
    columnSep = getGridLinePos(width,500);
  else
    columnSep = 202 + 4*vBorder;

  int previewFrameWidth = 0;
  if (IAmShowingCurvePreview || IAmShowingHelpPixmap)
    previewFrameWidth = std::min(width-464,211); //getGridLinePos(width,320);

  int inParameterFrameWidth = width - columnSep - 3*vBorder - previewFrameWidth;
  int xFieldWidth = getGridLinePos(inParameterFrameWidth,300);
  int buttonWidth = getGridLinePos(inParameterFrameWidth,75);
  int extrapolateWidth = getGridLinePos(inParameterFrameWidth,250-vBorder);

  int h1 = textHeight + border;
  int h2 = h1 + fieldHeight;
  int h3 = h2 + border;
  int h4 = IAmShowingLinkToFields ? h3 + fieldHeight : h3;
  int h5 = h4 + border;
  int h8 = height - border;
  int h7 = h8 - fieldHeight;
  int h6 = h7 - textHeight;
  int h9 = height;
  if (IAmShowingOutput) h9 -= fieldHeight;
  int h0 = h9;
  if (IAmShowingMathExpr) h9 -= fieldHeight+border;

  int v1 = vBorder;
  int v2 = columnSep - 2*vBorder;
  int v3 = columnSep - vBorder;
  int v4 = columnSep;
  int v5 = columnSep + vBorder;
  int v6 = v5 + xFieldWidth;
  int v7 = v6 + xFieldWidth;
  int v8 = v7 + buttonWidth;
  int v9 = v8 + buttonWidth;
  int v91 = width - vBorder - previewFrameWidth - extrapolateWidth;
  int v10 = v91 + extrapolateWidth - vBorder;
  int v11 = v10 + vBorder;
  int v13 = myNumArgLabel->getWidthHint();

  myTypeFrame->setEdgeGeometry(0,v3,0,h5);
  myTypeSwitch->setEdgeGeometry(v1,v2,h1,h2);

  myEngineFunction->setEdgeGeometry(v1,v2,h3,h4);

  myInputSelector->setEdgeGeometry(0,v3,h5,h9);
  myArgumentTable->setEdgeGeometry(0,v3,h5,h9);
  myNumArgLabel->setEdgeGeometry(0,v13,h0-fieldHeight,h0);
  myNumArgBox->setEdgeGeometry(v13+border,v3,h0-fieldHeight,h0);
  myOutputToggle->setEdgeGeometry(0,v3,height-fieldHeight,height);
  myThresholdFrame->setEdgeGeometry(v5,width,0,height);
  myThresholds->setEdgeGeometry(vBorder,width-v5-vBorder,h1,height-border);

  // Add or remove the threshold fields as a third tab
  if (IAmShowingOutput == 3)
  {
#ifdef FT_HAS_PREVIEW
    if (myTabStack->addTabPage(myThreshold2, "Threshold"))
      myTabStack->renameTabPage(myHelpFrame, "Prm. Help");
#else
    // No preview tab - don't need to change tab label
    myTabStack->addTabPage(myThreshold2, "Threshold");
#endif
  }
  else if (IAmShowingCurvePreview || IAmShowingHelpPixmap)
  {
    if (myTabStack->removeTabPage(myThreshold2))
      myTabStack->renameTabPage(myHelpFrame, "Parameter Help");
  }

  int nRows = myArgumentTable->getNumberRows();
  int argWt = v3-4;
  int argHt = textHeight+3*(vBorder+fieldHeight);
  if (argHt*nRows > h9-h5-4)
    argWt -= 18;
  else if (nRows > 0)
    argHt = (h9-h5-4)/nRows;
  for (int i = 0; i < nRows; i++)
    myArgumentTable->setRowHeight(i,argHt);
  myArgumentTable->setColumnWidth(0,argWt);

  myTabStack->setEdgeGeometry(std::max(width-211,464),width,0,180);

  int helpWidth = myHelpFrame->getWidth();
  int helpHeight = myHelpFrame->getHeight();
  myHelpLabel->setEdgeGeometry(2, helpWidth-2, 2, helpHeight-2);

  myParameterFrame->setEdgeGeometry(v4,v11,0,height);

  if (v10-v5 < myMinParameterWidth) {
    myParameterView->stretchContentsWidth(false);
    myParameterView->setColumnWidth(myMinParameterWidth);
  }
  else
    myParameterView->stretchContentsWidth(true);

  myParameterView->setEdgeGeometry(v5,v10,h1,h8);
  myParameterList->setEdgeGeometry(v5,v10,h1,h6);
  myExpandButton->setEdgeGeometry(v10-6*textHeight/5,v10+1,0,textHeight+5);

  myXLabel->setEdgeGeometry(v5,v6,h6,h7);
  myYLabel->setEdgeGeometry(v6,v7,h6,h7);
  if (IAmShowingParameterList > 1)
    myXValueInputField->setEdgeGeometry(v5,v6,h7,h8);
  else
    myXValueInputField->setEdgeGeometry(v5,v7,h7,h8);
  myYValueInputField->setEdgeGeometry(v6,v7,h7,h8);

  myExtrapolationLabel->setEdgeGeometry(v91,v10,h6,h7);
  myExtrapolationSwitch->setEdgeGeometry(v91,v10,h7,h8);

  myAddButton->setEdgeGeometry(v7,v8,h7,h8);
  myDeleteButton->setEdgeGeometry(v8,v9,h7,h8);

  // Place external function parameters

  int labelWidth = myFileBrowseLabel->getFontWidth("File/Reference");
  int channelBrowseWidth = this->getFontWidth("X...X");

  int h_1  = textHeight + border;
  int h_2  = IAmShowingFileView ? h_1 + fieldHeight : h_1;
  int h_21 = IAmShowingFileView ? h_2 + border : h_1;
  int h_22 = h_21 + fieldHeight;
  int h_3  = h_22 + border;
  int h_4  = h_3 + fieldHeight;
  int h_5  = h_4 + border;
  int h_6  = h_5 + fieldHeight;
  int h_7  = h_6 + fieldHeight;
  int h_8  = h_7 + border;
  int h_9  = h_8 + fieldHeight;
  int h_10 = h_9 + border;

  int v_1 = columnSep + vBorder;
  int v_2 = v_1 + vBorder;
  int v_3 = v_1 + labelWidth + vBorder;
  int v_5 = v11 - vBorder;
  int v_6 = v_5 - channelBrowseWidth;
  int v_7 = v_6 - vBorder;
  int v_8 = v_5 - vBorder;

  myFileBrowseLabel->    setEdgeGeometry(v_1, v_3, h_1, h_2);
  myFileRefQueryField->  setEdgeGeometry(v_3, v_7, h_1, h_2);
  myFileBrowseButton->   setEdgeGeometry(v_6, v_5, h_1, h_2);
  myChannelNameField->   setEdgeGeometry(v_1, v_7, h_21, h_22);
  myChannelNameField->   setLabelWidth  (v_3-v_1);
  myChannelBrowseButton->setEdgeGeometry(v_6, v_5, h_21, h_22);
  myScaleFactorField->   setEdgeGeometry(v_2, v_7, h_3, h_4);
  myScaleFactorField->   setLabelWidth  (labelWidth);
  myVerticalShiftFrame-> setEdgeGeometry(v_1, v_5, h_5, h_10);
  myZeroAdjustToggle->   setEdgeGeometry(v_2, v_8, h_6, h_7);
  if (IAmShowingFileView)
    myVerticalShiftField->setEdgeGeometry(v_2, v_8, h_8, h_9);
  else // External Function
    myVerticalShiftField->setEdgeGeometry(v_2, v_7, h_5, h_6);

  // Place math expression

  int eh_1 = h6   + border;
  int eh_2 = eh_1 + myExprApplyButton->getHeightHint();
  int ev_1 = v5   + myExprLabel->getWidthHint();
  int ev_2 = v10  - myExprApplyButton->getFontWidth("XApplyX");

  myExprMemo->setEdgeGeometry(v5,v10,h1,h6);
  myExprLabel->setEdgeGeometry(v5,ev_1,eh_1,eh_2);
  myExprApplyButton->setEdgeGeometry(ev_2,v10,eh_1,eh_2);

  // Place preview fields (matching file/channel browse field heights)

#ifdef FT_HAS_PREVIEW
  int incLabelWidth = myDXLabel->getWidthHint();
  int vp1 = vBorder;
  int vp2 = vp1 + incLabelWidth;
  int vp3 = vp2 + vBorder;
  int vp4 = vp3 + 60;
  int vp5 = vp4 + vBorder;
  int vp6 = vp5 + 60;

  int ph1 = border;
  int ph2 = ph1 + fieldHeight;
  int ph3 = ph2 + border;
  int ph4 = ph3 + fieldHeight;
  int ph5 = ph4 + border;
  int ph6 = ph5 + fieldHeight + 4;

  myPreviewButton->setEdgeGeometry(vp5,vp6,ph5,ph6);

  myX0Label->setEdgeGeometry(vp1,vp2,ph1,ph2);
  myDXLabel->setEdgeGeometry(vp1,vp2,ph3,ph4);
  myX0Field->setEdgeGeometry(vp3,vp4,ph1,ph2);
  myXNField->setEdgeGeometry(vp5,vp6,ph1,ph2);
  myDXField->setEdgeGeometry(vp5,vp6,ph3,ph4);
  myUseSmartPointsToggle->setEdgeGeometry(vp3,vp4,ph3,ph4);
#endif

  // Jonswap wave spectrum

  int y = h5;
  myJonswapAdvancedFrame->setEdgeGeometry(0,v3,y,136+y);
  v3 -= vBorder;
  y += 16;
  myJonswapSpectralPeakednessField->setEdgeGeometry(v1,v3,y,20+y);
  myJonswapSpectralPeakednessField->setLabelWidth(150);
  y += 20;
  myJonswapSpectralPeakednessToggle->setEdgeGeometry(v1+10,v3,y,20+y);
  y += 24;
  myJonswapWaveComponentsField->setEdgeGeometry(v1,v3,y,20+y);
  myJonswapWaveComponentsField->setLabelWidth(150);
  y += 24;
  myJonswapWaveDirsField->setEdgeGeometry(v1,v3,y,20+y);
  myJonswapWaveDirsField->setLabelWidth(150);
  y += 24;
  int x = v1 + (v3-v1)/2;
  myJonswapSpreadExpField->setEdgeGeometry(v1,x,y,20+y);
  myJonswapBasicFrame->setEdgeGeometry(v5,v10,0,68);
  if (IAmShowingFileView == 'w') {
    y = h_21 + border;
    myJonswapRandomSeedField->setEdgeGeometry(v5,v10,y,20+y);
    myJonswapRandomSeedField->setLabelWidth(150-vBorder);
    y += 25;
  }
  else {
    myJonswapRandomSeedField->setEdgeGeometry(x+vBorder,v3,y,20+y);
    myJonswapRandomSeedField->setLabelWidth(myJonswapRandomSeedField->myLabel->getWidthHint());
    v5 += vBorder; v10 -= vBorder;
    y = 18;
  }
  myJonswapHsField->setEdgeGeometry(v5,v10,y,20+y);
  myJonswapHsField->setLabelWidth(150);
  y += 24;
  myJonswapTpField->setEdgeGeometry(v5,v10,y,20+y);
  myJonswapTpField->setLabelWidth(150);

  y += 34;
  v5 -= vBorder; v10 += vBorder;
  myJonswapCutOffFrame->setEdgeGeometry(v5,v10,y,68+y);
  v5 += vBorder; v10 -= vBorder;
  y += 18;
  myJonswapCutOffToggle->setEdgeGeometry(v5,v10,y,20+y);
  y += 24;
  x = v5 + (v10-v5)/2;
  myJonswapTLowField->setEdgeGeometry(v5,x-vBorder,y,20+y);
  myJonswapTLowField->setLabelWidth(30);
  myJonswapTHighField->setEdgeGeometry(x+vBorder,v10,y,20+y);
  myJonswapTHighField->setLabelWidth(30);
}


void FuiFunctionProperties::setUIValues(const FFuaUIValues* values)
{
  const FuaFunctionPropertiesValues* fv = dynamic_cast<const FuaFunctionPropertiesValues*>(values);
  if (!fv) return;

  IAmAllowingTopolChange = fv->allowTopolChange;

  if (fv->showArgument)
  {
    if (fv->myArgumentValues.size() == 1)
      myInputSelector->setValues(fv->myArgumentValues.front());
    else for (size_t j = 0; j < fv->myArgumentValues.size(); j++)
      myArguments[j]->setValues(fv->myArgumentValues[j]);
  }

  if (fv->showLinkToFields)
  {
    myEngineFunction->setSelectedRef(fv->mySelectedLinkFunction);
    this->setParameterSensitivity(false);
  }
  else
    this->setParameterSensitivity(IAmSensitive);

  myTypeSwitch->selectOption(fv->mySelectedFunctionTypeIdx);

  if (fv->showFileView)
  {
    myFileRefQueryField->setSelectedRef(fv->selectedFileRef);
    myFileRefQueryField->setText(fv->fileName);
    myChannelNameField->myField->setValue(fv->myChannelName);
    myChannelBrowseButton->setSensitivity(fv->isMultiChannel && IAmSensitive);
    myZeroAdjustToggle->setValue(fv->myZeroAdjust);
    myScaleFactorField->setValue(fv->myScaleFactor);
    myVerticalShiftField->setValue(fv->myVerticalShift);
  }

  else if (fv->showParameterView)
  {
    FFuLabelField* field;
    for (size_t i = 0; i < fv->myParameterValues.size(); i++)
      if ((field = dynamic_cast<FFuLabelField*>(myParameterView->getCell(i,0))))
	field->setValue(fv->myParameterValues[i]);
  }

  else if (fv->showMathExpr)
  {
    myExprMemo->setAllText(fv->myExpression.c_str());
    if (fv->showArgument)
    {
      myNumArgBox->setIntValue(fv->myArgumentValues.size());
      myNumArgBox->setSensitivity(IAmSensitive && !fv->showLinkToFields);
    }
  }

  else if (fv->myExtFunc > 0) {
    myChannelNameField->myField->setValue(fv->myExtFunc);
    myScaleFactorField->setValue(fv->myScaleFactor);
    myVerticalShiftField->setValue(fv->myVerticalShift);
  }

  if (fv->showParameterList)
  {
    std::vector<std::string> items;
    for (size_t i = 0; i < fv->myXValues.size(); i++)
      if (fv->showParameterList > 1 && i < fv->myYValues.size())
        items.push_back(FFaNumStr(fv->myXValues[i],1,8,1e+3,1e-3) + ", " +
                        FFaNumStr(fv->myYValues[i],1,8,1e+3,1e-3));
      else
        items.push_back(FFaNumStr(fv->myXValues[i],1,8,1e+3,1e-3));

    myParameterList->setItems(items);
    myExtrapolationSwitch->selectOption(fv->myExtrapolationType);
  }

  if ((IAmShowingOutput = fv->useAsOutputSensor+1) > 0)
  {
    myOutputToggle->setValue(fv->useAsOutputSensor > 0);
    if (IAmShowingOutput == 3)
      myThreshold2->setValues(fv->myThreshold);
    else
      myThresholds->setValues(fv->myThreshold);
  }

  this->placeWidgets(this->getWidth(), this->getHeight());

#ifdef FT_HAS_PREVIEW
  myX0Field->setValue(fv->previewDomain.X.first);
  myXNField->setValue(fv->previewDomain.X.second);
  myDXField->setValue(fv->previewDomain.dX);
  myDXField->setSensitivity(fv->previewDomain.autoInc <= 0);
  myUseSmartPointsToggle->setValue(fv->previewDomain.autoInc > 0);
  myUseSmartPointsToggle->setSensitivity(fv->previewDomain.autoInc >= 0);
#endif

  if (fv->showJonswapView)
  {
    myJonswapTpField->setLabel("Spectral peak period, Tp");
    myJonswapHsField->setValue(fv->myJonswapHs);
    myJonswapTpField->setValue(fv->myJonswapTp);
    myJonswapCutOffToggle->setValue(fv->myJonswapAutoCalcCutoff);
    myJonswapTLowField->setValue(fv->myJonswapRange.first);
    myJonswapTHighField->setValue(fv->myJonswapRange.second);
    myJonswapSpectralPeakednessToggle->setValue(fv->myJonswapAutoCalcSpectralPeakedness);
    myJonswapSpectralPeakednessField->setValue(fv->myJonswapSpectralPeakedness);
    myJonswapWaveComponentsField->myField->setValue(fv->myJonswapNComp);
    myJonswapRandomSeedField->myField->setValue(fv->myJonswapRandomSeed);
    myJonswapWaveDirsField->myField->setValue(fv->myJonswapNDir);
    myJonswapSpreadExpField->myField->setValue(fv->myJonswapSprExp);
    this->setJonswapSensitivity();
  }
  else if (IAmShowingFileView == 'w')
  {
    // These values are calculated from file
    myJonswapTpField->setLabel("Mean wave period, Tz");
    myJonswapHsField->setValue(fv->myJonswapHs);
    myJonswapTpField->setValue(fv->myJonswapTp);
    myJonswapRandomSeedField->myField->setValue(fv->myJonswapRandomSeed);
  }
}


void FuiFunctionProperties::buildDynamicWidgets(const FFuaUIValues* values)
{
  const FuaFunctionPropertiesValues* fv = dynamic_cast<const FuaFunctionPropertiesValues*>(values);
  if (!fv) return;

  // Store the visibility for placeWidgets etc.

  IAmShowingParameterList = fv->showParameterList;
  IAmShowingParameterView = fv->showParameterView;
  if (!fv->showFileView)
    IAmShowingFileView = false;
  else if (fv->mySelectedFunctionTypeIdx == 2)
    IAmShowingFileView = 'w'; // User defined wave spectrum view
  else
    IAmShowingFileView = fv->isMultiChannel ? 2 : 1; // Other file views
  IAmShowingMathExpr = fv->showMathExpr;
  IAmShowingCurvePreview = fv->showCurvePreview;
  unsigned char nArg = fv->myArgumentValues.size();
  IAmShowingArgument = fv->showArgument ? nArg : -nArg;
  IAmShowingLinkToFields = fv->showLinkToFields;
  IAmShowingHelpPixmap = fv->myHelpPixmap ? true : false;
  IAmShowingTypeSwitch = fv->showTypeSwitch && fv->myFunctionTypes.size() > 1;
  IAmShowingJonswap = fv->showJonswapView;
  IAmShowingOutput = fv->useAsOutputSensor+1;

  // Function type menu

  if (IAmShowingTypeSwitch) {
    myTypeFrame->popUp();
    myTypeSwitch->popUp();
    myTypeSwitch->setOptions(fv->myFunctionTypes);
  }
  else {
    myTypeFrame->popDown();
    myTypeSwitch->popDown();
  }

  // Arguments

  if (IAmShowingArgument == 1) {
    myInputSelector->popUp();
    myArgumentTable->popDown();
  }
  else if (IAmShowingArgument > 1) {
    size_t narg = fv->myArgumentValues.size();
    size_t nrow = myArgumentTable->getNumberRows();
    for (size_t r = narg; r < nrow; r++)
      myArgumentTable->clearCellContents(r,0);
    this->setNoArgs(narg);
    myArgumentTable->setNumberRows(narg);
    for (size_t i = 0; i < narg; i++) {
      if (narg > 4)
	myArguments[i]->setLabel(std::string("Argument ") + char('1'+i));
      else
	myArguments[i]->setLabel(std::string("Argument ") + (i < 3 ? char('x'+i) : 't'));
      myArguments[i]->setValuesChangedCB(FFaDynCB1M(FuiFunctionProperties,this,
						    onInputSelectorValuesChanged,
						    FuiInputSelectorValues&));
      myArguments[i]->setSetDBValuesCB(FFaDynCB1M(FuiFunctionProperties,this,
						  onInputSelectorSetDBValues,
						  FuiInputSelectorValues&));
      if (i >= nrow) myArgumentTable->insertWidget(i,0,myArguments[i]);
      switch (i) {
      case 0: myArguments[i]->setPickCB(FFaDynCB0M(FuiFunctionProperties,this,onSensorPicked)); break;
      case 1: myArguments[i]->setPickCB(FFaDynCB0M(FuiFunctionProperties,this,onSensor1Picked)); break;
      case 2: myArguments[i]->setPickCB(FFaDynCB0M(FuiFunctionProperties,this,onSensor2Picked)); break;
      case 3: myArguments[i]->setPickCB(FFaDynCB0M(FuiFunctionProperties,this,onSensor3Picked)); break;
      case 4: myArguments[i]->setPickCB(FFaDynCB0M(FuiFunctionProperties,this,onSensor4Picked)); break;
      case 5: myArguments[i]->setPickCB(FFaDynCB0M(FuiFunctionProperties,this,onSensor5Picked)); break;
      case 6: myArguments[i]->setPickCB(FFaDynCB0M(FuiFunctionProperties,this,onSensor6Picked)); break;
      case 7: myArguments[i]->setPickCB(FFaDynCB0M(FuiFunctionProperties,this,onSensor7Picked)); break;
      case 8: myArguments[i]->setPickCB(FFaDynCB0M(FuiFunctionProperties,this,onSensor8Picked)); break;
      case 9: myArguments[i]->setPickCB(FFaDynCB0M(FuiFunctionProperties,this,onSensor9Picked)); break;
      }
    }
    myInputSelector->popDown();
    myArgumentTable->popUp();
  }
  else {
    myInputSelector->popDown();
    myArgumentTable->popDown();
  }

  // Output sensor toggle

  if (IAmShowingOutput)
    myOutputToggle->popUp();
  else
    myOutputToggle->popDown();

  // Threshold fields

  if (IAmShowingOutput == 2)
    myThresholdFrame->popUp();
  else
    myThresholdFrame->popDown();

  // Link to fields

  if (IAmShowingLinkToFields) {
    myEngineFunction->setBehaviour(FuiQueryInputField::REF_NONE);
    myEngineFunction->setTextForNoRefSelected("None");
    myEngineFunction->popUp();
    myEngineFunction->setQuery(fv->myLinkFunctionQuery);
  }
  else
    myEngineFunction->popDown();

  // Curve preview

  if (IAmShowingCurvePreview || IAmShowingHelpPixmap) {
#ifdef FT_HAS_PREVIEW
    myTabStack->setTabSensitivity(1,IAmShowingCurvePreview);
#endif
    myTabStack->popUp();
  }
  else
    myTabStack->popDown();

  // Help picture

  myHelpLabel->setPixMap(fv->myHelpPixmap);

  // Parameter view

  myParameterView->deleteRow(-1);

  if (IAmShowingParameterView) {

    int maxLabelWidth = 0;
    int maxFieldWidth = 0;
    std::vector<FFuLabelField*> fields;
    fields.reserve(fv->myParameterNames.size());

    for (const std::string& paraName : fv->myParameterNames) {
      FFuLabelField* field = FFuLabelField::create(NULL);

      field->setLabelMargin(3);
      field->setLabel(paraName.c_str());
      field->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
      field->myField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
					       onParameterChanged,char*));

      if (maxLabelWidth < field->myLabel->getWidthHint())
	maxLabelWidth = field->myLabel->getWidthHint();
      if (maxFieldWidth < field->myField->getWidthHint())
	maxFieldWidth = field->myField->getWidthHint();

      myParameterView->addRow({field});
      fields.push_back(field);
    }

    for (FFuLabelField* field : fields)
      field->setLabelWidth(maxLabelWidth);

    myMinParameterWidth = maxLabelWidth + maxFieldWidth + 9;

    if (myParameterView->getWidth() < myMinParameterWidth) {
      myParameterView->stretchContentsWidth(false);
      myParameterView->setColumnWidth(myMinParameterWidth);
    }
    else
      myParameterView->stretchContentsWidth(true);

    myParameterView->popUp();

    myExpandButton->setLabel(">");
    myExpandButton->setToolTip("Expand the Parameters view");
    myExpandButton->popUp();
  }
  else {
    myParameterView->popDown();
    myExpandButton->popDown();
  }

  // File view

  if (IAmShowingFileView) {
    myFileRefQueryField->setQuery(fv->fileRefQuery);
    myFileRefQueryField->popUp();
    myFileBrowseLabel->popUp();
    myFileBrowseButton->popUp();

    if (IAmShowingFileView == 'w') {
      // User defined wave spectrum view
      myJonswapHsField->popUp();
      myJonswapTpField->popUp();
      myJonswapRandomSeedField->popUp();
      myChannelNameField->popDown();
      myChannelBrowseButton->popDown();
      myScaleFactorField->popDown();
      myVerticalShiftFrame->popDown();
      myVerticalShiftField->popDown();
      myZeroAdjustToggle->popDown();
    }
    else {
      // Other file view
      myChannelNameField->popUp();
      myChannelBrowseButton->popUp();
      myScaleFactorField->popUp();
      myVerticalShiftFrame->popUp();
      myVerticalShiftField->popUp();
      myZeroAdjustToggle->popUp();
    }
  }
  else {
    myFileRefQueryField->popDown();
    myFileBrowseLabel->popDown();
    myFileBrowseButton->popDown();
    if (fv->myExtFunc > 0) {
      myChannelNameField->popUp();
      myScaleFactorField->popUp();
      myVerticalShiftField->popUp();
    }
    else {
      myChannelNameField->popDown();
      myScaleFactorField->popDown();
      myVerticalShiftField->popDown();
    }
    myChannelBrowseButton->popDown();
    myVerticalShiftFrame->popDown();
    myZeroAdjustToggle->popDown();
  }

  // Parameter list

  if (IAmShowingParameterList) {
    myXLabel->popUp();
    myAddButton->popUp();
    myDeleteButton->popUp();
    myExtrapolationLabel->popUp();
    myExtrapolationSwitch->popUp();
    myXValueInputField->setValue("");
    myXValueInputField->popUp();
    myParameterList->popUp();

    if (IAmShowingParameterList > 1) {
      myYLabel->popUp();
      myYValueInputField->setValue("");
      myYValueInputField->popUp();
    }
    else {
      myYLabel->popDown();
      myYValueInputField->popDown();
    }
  }
  else {
    myXLabel->popDown();
    myAddButton->popDown();
    myDeleteButton->popDown();
    myExtrapolationLabel->popDown();
    myExtrapolationSwitch->popDown();
    myXValueInputField->popDown();
    myParameterList->popDown();
    myYLabel->popDown();
    myYValueInputField->popDown();
  }

  // Math expression

  if (IAmShowingMathExpr) {
    switch ( std::abs((int)IAmShowingArgument) ) {
    case 1: myExprLabel->setLabel("Enter a function of x"); break;
    case 2: myExprLabel->setLabel("Enter a function of x,y"); break;
    case 3: myExprLabel->setLabel("Enter a function of x,y,z"); break;
    case 4: myExprLabel->setLabel("Enter a function of x,y,z,t"); break;
    }
    myExprMemo->popUp();
    myExprLabel->popUp();
    myExprApplyButton->popUp();
    if (IAmShowingArgument > 0) {
      myNumArgLabel->popUp();
      myNumArgBox->popUp();
    }
    else {
      myNumArgLabel->popDown();
      myNumArgBox->popDown();
    }
  }
  else {
    myExprMemo->popDown();
    myExprLabel->popDown();
    myExprApplyButton->popDown();
    myNumArgLabel->popDown();
    myNumArgBox->popDown();
  }

  if (IAmShowingParameterList || IAmShowingParameterView || IAmShowingFileView || IAmShowingMathExpr)
    myParameterFrame->popUp();
  else
    myParameterFrame->popDown();

  // Jonswap wave spectrum

  if (IAmShowingJonswap) {
    myJonswapAdvancedFrame->popUp();
    myJonswapSpectralPeakednessField->popUp();
    myJonswapSpectralPeakednessToggle->popUp();
    myJonswapWaveComponentsField->popUp();
    myJonswapRandomSeedField->popUp();
    myJonswapWaveDirsField->popUp();
    myJonswapSpreadExpField->popUp();
    myJonswapBasicFrame->popUp();
    myJonswapHsField->popUp();
    myJonswapTpField->popUp();
    myJonswapCutOffFrame->popUp();
    myJonswapCutOffToggle->popUp();
    myJonswapTLowField->popUp();
    myJonswapTHighField->popUp();
    myParameterFrame->popDown();
    myParameterView->popDown();
    myExpandButton->popDown();
  }
  else {
    myJonswapAdvancedFrame->popDown();
    myJonswapSpectralPeakednessField->popDown();
    myJonswapSpectralPeakednessToggle->popDown();
    myJonswapWaveComponentsField->popDown();
    myJonswapWaveDirsField->popDown();
    myJonswapSpreadExpField->popDown();
    myJonswapBasicFrame->popDown();
    if (IAmShowingFileView != 'w') {
      myJonswapRandomSeedField->popDown();
      myJonswapHsField->popDown();
      myJonswapTpField->popDown();
    }
    myJonswapCutOffFrame->popDown();
    myJonswapCutOffToggle->popDown();
    myJonswapTLowField->popDown();
    myJonswapTHighField->popDown();
  }
}


void FuiFunctionProperties::setCBs(const FFuaUIValues* values)
{
  const FuaFunctionPropertiesValues* pv = dynamic_cast<const FuaFunctionPropertiesValues*>(values);
  if (!pv) return;

  myFuncTypeSwitchedCB = pv->myFuncTypeSwitchedCB;
  myEngineFunction->setButtonCB(pv->myEditLinkFunctionCB);
  myPickSensorCB = pv->myPickSensorCB;

  myAddNumberCB = pv->myAddNumberCB;
  myAddNumbersCB = pv->myAddNumbersCB;
  myParameterList->setPasteCB(pv->myPasteCB);
  myDeleteCB = pv->myDeleteCB;
  myClearAllCB = pv->myClearAllCB;
  myFilesBrowseCB = pv->myFilesBrowseCB;
  myGetChannelListCB = pv->myGetChannelListCB;
  myAcceptExprCB = pv->myAcceptExprCB;

#ifdef FT_HAS_PREVIEW
  myPreviewCB = pv->myPreviewCB;
#endif
}


void FuiFunctionProperties::getValues(FuaFunctionPropertiesValues& values)
{
  values.mySelectedFunctionTypeIdx = myTypeSwitch->getSelectedOption();

  values.showParameterView = IAmShowingParameterView > 0;
  values.showFileView = IAmShowingFileView > 0;
  values.showMathExpr = IAmShowingMathExpr;
  values.showParameterList = IAmShowingParameterList;

  if (IAmShowingFileView) {
    if (myFileRefQueryField->isAConstant()) {
      values.fileName = myFileRefQueryField->getText();
      values.selectedFileRef = NULL;
    }
    else {
      values.fileName = "";
      values.selectedFileRef = myFileRefQueryField->getSelectedRef();
    }
    values.myChannelName = myChannelNameField->myField->getValue();
    values.myScaleFactor = myScaleFactorField->getValue();
    values.myVerticalShift = myVerticalShiftField->getValue();
    values.myZeroAdjust = myZeroAdjustToggle->getValue();
    values.myJonswapRandomSeed = myJonswapRandomSeedField->myField->getInt();
  }
  else if (IAmShowingJonswap) {
    values.myJonswapHs = myJonswapHsField->getValue();
    values.myJonswapTp = myJonswapTpField->getValue();
    values.myJonswapAutoCalcCutoff = myJonswapCutOffToggle->getValue();
    values.myJonswapRange = { myJonswapTLowField->getValue(), myJonswapTHighField->getValue() };
    values.myJonswapAutoCalcSpectralPeakedness = myJonswapSpectralPeakednessToggle->getValue();
    values.myJonswapSpectralPeakedness = myJonswapSpectralPeakednessField->getValue();
    values.myJonswapNComp = myJonswapWaveComponentsField->myField->getInt();
    values.myJonswapRandomSeed = myJonswapRandomSeedField->myField->getInt();
    values.myJonswapNDir = myJonswapWaveDirsField->myField->getInt();
    values.myJonswapSprExp = myJonswapSpreadExpField->myField->getInt();
  }
  else if (IAmShowingParameterView) {
    FFuLabelField* field;
    int nRows = myParameterView->getRowCount();
    values.myParameterValues.clear();
    for (int i = 0; i < nRows; i++)
      if ((field = dynamic_cast<FFuLabelField*>(myParameterView->getCell(i,0))))
	values.myParameterValues.push_back(field->getValue());
  }
  else if (IAmShowingParameterList)
    values.myExtrapolationType = myExtrapolationSwitch->getSelectedOption();

  else if (IAmShowingMathExpr)
    myExprMemo->getText(values.myExpression);

  else {
    values.myScaleFactor = myScaleFactorField->getValue();
    values.myVerticalShift = myVerticalShiftField->getValue();
  }

  if (IAmShowingArgument > 0)
    values.myArgumentValues.resize(IAmShowingArgument);
  else
    values.myArgumentValues.clear();
  if (IAmShowingArgument == 1)
    myInputSelector->getValues(values.myArgumentValues.front());
  else for (char i = 0; i < IAmShowingArgument; i++)
    myArguments[i]->getValues(values.myArgumentValues[i]);

  if (IAmShowingMathExpr)
    values.myArgumentValues.resize(myNumArgBox->getIntValue());

  if (IAmShowingLinkToFields)
    values.mySelectedLinkFunction = myEngineFunction->getSelectedRef();

  if (IAmShowingOutput) {
    values.useAsOutputSensor = myOutputToggle->getValue();
    if (IAmShowingOutput == 3)
      myThreshold2->getValues(values.myThreshold);
    else
      myThresholds->getValues(values.myThreshold);
  }
  else
    values.useAsOutputSensor = -1;

#ifdef FT_HAS_PREVIEW
  this->getPreviewDomain(values.previewDomain);
}


void FuiFunctionProperties::getPreviewDomain(FuaFunctionDomain& domain)
{
  domain.X = { myX0Field->getDouble(), myXNField->getDouble() };
  domain.dX = myDXField->getDouble();
  domain.autoInc = myUseSmartPointsToggle->getValue();
#endif
}


int FuiFunctionProperties::getSelectedFunctionIndex() const
{
  return myTypeSwitch->getSelectedOption();
}


void FuiFunctionProperties::setChannelList(const std::vector<std::string>& channels)
{
  myChannelSelectUI->myItemSelector->deleteAllItems();
  for (const std::string& chn : channels)
    myChannelSelectUI->myItemSelector->addItem(chn.c_str());
}


void FuiFunctionProperties::popUpChannelList()
{
  myChannelSelectUI->popUp();
}


void FuiFunctionProperties::onValuesChanged(bool widgetChanges)
{
  FuaFunctionPropertiesValues values;
  this->getValues(values);
  this->invokeSetAndGetDBValuesCB(&values);
  if (widgetChanges)
    this->buildDynamicWidgets(&values);
  this->setUIValues(&values);
}


void FuiFunctionProperties::onXAccepted(double d)
{
  if (IAmShowingParameterList < 2)
    myAddNumberCB.invoke(d);
}


void FuiFunctionProperties::onYAccepted(double)
{
  this->onAddButtonActivated();
  myXValueInputField->giveKeyboardFocus();
}


void FuiFunctionProperties::onAddButtonActivated()
{
  if (IAmShowingParameterList > 1)
    myAddNumbersCB.invoke(myXValueInputField->getDouble(),myYValueInputField->getDouble());
  else
    myAddNumberCB.invoke(myXValueInputField->getDouble());
}


void FuiFunctionProperties::onDeleteButtonActivated()
{
  int currItem = myParameterList->getSelectedItemIndex();
  if (currItem > -1)
    myDeleteCB.invoke(currItem);

  if (currItem > -1 && currItem < myParameterList->getNumberOfItems())
    myParameterList->selectItem(currItem, true);
}


void FuiFunctionProperties::onBrowseFileOpened(const std::string&, int)
{
  myChannelNameField->myField->setValue("Not set");
  this->onValuesChanged();
}


void FuiFunctionProperties::onChannelDialogOK(int index)
{
  this->onChannelDialogApply(index);
  myChannelSelectUI->popDown();
}


void FuiFunctionProperties::onChannelDialogApply(int index)
{
  if (myChannelSelectUI->myItemSelector->isItemSelected(index))
    myChannelNameField->myField->setValue(myChannelSelectUI->myItemSelector->getItemText(index));

  this->onValuesChanged();
}


void FuiFunctionProperties::onChannelDialogCancel(int)
{
  myChannelSelectUI->popDown();
}


void FuiFunctionProperties::onParameterSelected(int i)
{
  std::string txt = myParameterList->getItemText(i);
  double x,y;

  if (IAmShowingParameterList > 1) {
    sscanf(txt.c_str(), "%lf, %lf", &x,&y);
    myXValueInputField->setValue(x);
    myYValueInputField->setValue(y);
  }
  else {
    sscanf(txt.c_str(), "%lf", &x);
    myXValueInputField->setValue(x);
  }
}


void FuiFunctionProperties::onExprApplyButtonActivated()
{
  this->onValuesChanged();
  myAcceptExprCB.invoke();
}


void FuiFunctionProperties::onExpandButtonActivated()
{
  if (IAmShowingParameterView > 1)
  {
    IAmShowingCurvePreview = IAmShowingParameterView%2 > 0;
    IAmShowingHelpPixmap = IAmShowingParameterView > 2;
    IAmShowingParameterView = 1;
    myExpandButton->setLabel(">");
    myExpandButton->setToolTip("Expand the Parameters view");
    if (IAmShowingCurvePreview || IAmShowingHelpPixmap)
      myTabStack->popUp();
  }
  else
  {
    if (IAmShowingCurvePreview) IAmShowingParameterView += 1;
    if (IAmShowingHelpPixmap) IAmShowingParameterView += 2;
    IAmShowingCurvePreview = IAmShowingHelpPixmap = false;
    myExpandButton->setLabel("<");
    myExpandButton->setToolTip("Contract the Parameters view");
    myTabStack->popDown();
  }

  this->placeWidgets(this->getWidth(), this->getHeight());
}


void FuiFunctionProperties::setSensitivity(bool isSensitive)
{
  IAmSensitive = isSensitive;

  myTypeSwitch->setSensitivity(isSensitive && IAmAllowingTopolChange);
  myEngineFunction->setSensitivity(isSensitive && IAmAllowingTopolChange);
  myInputSelector->setSensitivity(isSensitive && IAmAllowingTopolChange);
  for (FuiInputSelector* arg : myArguments)
    arg->setSensitivity(isSensitive && IAmAllowingTopolChange);

  this->setParameterSensitivity(isSensitive && !IAmShowingLinkToFields);
}


void FuiFunctionProperties::setParameterSensitivity(bool isSensitive)
{
  myParameterView->setSensitivity(isSensitive);
  myParameterList->setSensitivity(isSensitive);
  myXValueInputField->setSensitivity(isSensitive);
  myYValueInputField->setSensitivity(isSensitive);
  myAddButton->setSensitivity(isSensitive);
  myDeleteButton->setSensitivity(isSensitive);
  myFileRefQueryField->setSensitivity(isSensitive);
  myFileBrowseButton->setSensitivity(isSensitive);
  myChannelBrowseButton->setSensitivity(isSensitive && IAmShowingFileView == 2);
  myScaleFactorField->setSensitivity(isSensitive);
  myVerticalShiftField->setSensitivity(isSensitive);
  myZeroAdjustToggle->setSensitivity(isSensitive);
  myExprMemo->setSensitivity(isSensitive);
  myExprApplyButton->setSensitivity(isSensitive);
  myExtrapolationSwitch->setSensitivity(isSensitive);

  myJonswapSpectralPeakednessToggle->setSensitivity(isSensitive);
  myJonswapWaveComponentsField->setSensitivity(isSensitive);
  myJonswapRandomSeedField->setSensitivity(isSensitive);
  myJonswapWaveDirsField->setSensitivity(isSensitive);
  myJonswapHsField->setSensitivity(isSensitive && IAmShowingFileView != 'w');
  myJonswapTpField->setSensitivity(isSensitive && IAmShowingFileView != 'w');
  myJonswapCutOffToggle->setSensitivity(isSensitive);
  this->setJonswapSensitivity(isSensitive);
}


void FuiFunctionProperties::setJonswapSensitivity(bool isSensitive)
{
  // Disable Tmin and Tmax based on CutOffToggle
  bool bVal = isSensitive && !this->myJonswapCutOffToggle->getValue();
  this->myJonswapTLowField->setSensitivity(bVal);
  this->myJonswapTHighField->setSensitivity(bVal);
  // Disable SpectralPeakedness based on SpectralPeakednessToggle
  bVal = isSensitive && !this->myJonswapSpectralPeakednessToggle->getValue();
  this->myJonswapSpectralPeakednessField->setSensitivity(bVal);
  bVal = isSensitive && this->myJonswapWaveDirsField->myField->getInt() > 1;
  // Disable spreading exponent field if only one wave direction
  this->myJonswapSpreadExpField->setSensitivity(bVal);
}


void FuiFunctionProperties::onFileBrowseActivated()
{
  myFileRefQueryField->setSelectedRef(NULL);
  myFilesBrowseCB.invoke();

  this->onValuesChanged();
}
