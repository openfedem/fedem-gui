// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>

#include "vpmUI/vpmUIComponents/FuiFunctionProperties.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "vpmUI/vpmUIComponents/FuiParameterView.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuMemo.H"
#include "FFuLib/FFuFrame.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuFileBrowseField.H"
#include "FFuLib/FFuSpinBox.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuScrolledList.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuScrolledListDialog.H"
#include "FFuLib/FFuTabbedWidgetStack.H"
#include "FFaLib/FFaString/FFaStringExt.H"


FuiFunctionProperties::FuiFunctionProperties()
{
  IAmSensitive = IAmAllowingTopolChange = true;

  IAmShowingTypeSwitch = false;
  IAmShowingCurvePreview = false;
  IAmShowingArgument = false;
  IAmShowingOutput = false;
  IAmShowingLinkToFields = false;
  IAmShowingHelpPixmap = false;
  IAmShowingJonswap = false;

  myFrictionType = 0;
}


void FuiFunctionProperties::initWidgets()
{
  // Function type and linkage

  myTypeFrame->setLabel("Function Type");
  myTypeFrame->setMaxHeight(80);

  myTypeSwitch->setOptionSelectedCB(FFaDynCB1M(FFaDynCB1<int>,&myFuncTypeSwitchedCB,invoke,int));

  myEngineFunction->setRefSelectedCB(FFaDynCB1M(FuiFunctionProperties,this,
						onEngineFuncChanged,int));

  // Argument

  myInputSelector->setValuesChangedCB(FFaDynCB1M(FuiFunctionProperties,this,
						 onInputSelectorValuesChanged,
						 FuiInputSelectorValues&));
  myInputSelector->setSetDBValuesCB(FFaDynCB1M(FuiFunctionProperties,this,
					       onInputSelectorSetDBValues,
					       FuiInputSelectorValues&));
  myInputSelector->setPickCB(FFaDynCB0M(FuiFunctionProperties,this,
					onSensorPicked));
  myInputSelector->setMaxHeight(120);

  // Arguments for multi-variable functions

  myInputSelectors->setCBs(FFaDynCB1M(FuiFunctionProperties,this,
                                      onInputSelectorValuesChanged,
                                      FuiInputSelectorValues&),
                           FFaDynCB1M(FuiFunctionProperties,this,
                                      onInputSelectorSetDBValues,
                                      FuiInputSelectorValues&),
                           FFaDynCB1M(FFaDynCB1<int>,&myPickSensorCB,invoke,int));

  // Output sensor toggle

  myOutputToggle->setLabel("Use as output sensor");
  myOutputToggle->setToggleCB(FFaDynCB1M(FuiFunctionProperties,this,
                                         onOutputToggled,bool));

  // Threshold fields

  myThresholdFrame->setLabel("Event Threshold");

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

  // Parameter views

  for (FFuLabelFrame* frame : myParameterFrames)
    frame->setLabel("Parameters");

  myExtrapolationSwitch->setOptionSelectedCB(FFaDynCB1M(FuiFunctionProperties,this,
							onSelectionChanged,int));
  myExtrapolationSwitch->addOption("None");
  myExtrapolationSwitch->addOption("Flat");
  myExtrapolationSwitch->addOption("Linear");

  myParameterView = new FuiParameterView();
  myParameterView->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
                                            onParameterValueChanged,double));

  myExpandButton->setActivateCB(FFaDynCB0M(FuiFunctionProperties,this,
					   onExpandButtonActivated));

  // Multi points UI

  myParameterList->setBrowseSelectCB(FFaDynCB1M(FuiFunctionProperties,this,
						onParameterSelected,int));
  myParameterList->setDeleteButtonCB(FFaDynCB0M(FuiFunctionProperties,this,
						onDeleteButtonActivated));
  myParameterList->setClearCB(FFaDynCB0M(FFaDynCB0,&myClearAllCB,invoke));
  myParameterList->setToolTip("Use Ctrl+V to paste numbers into this function\n"
			      "Use Ctrl+X to clear all numbers");

  myXValueInputField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myXValueInputField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
                                               onXAccepted,double));
  myYValueInputField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myYValueInputField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
                                               onYAccepted,double));
  myAddButton->setLabel("+");
  myAddButton->setMaxWidth(20);
  myAddButton->setActivateCB(FFaDynCB0M(FuiFunctionProperties,this,
					onAddButtonActivated));
  myDeleteButton->setLabel("-");
  myDeleteButton->setMaxWidth(20);
  myDeleteButton->setActivateCB(FFaDynCB0M(FuiFunctionProperties,this,
					   onDeleteButtonActivated));

  // Math expression

  myExprLabel->setLabel("Enter a function of x");
  myExprApplyButton->setLabel("Apply");
  myExprApplyButton->setActivateCB(FFaDynCB0M(FuiFunctionProperties,this,
					      onExprApplyButtonActivated));

  myNumArgLabel->setLabel("Number of function arguments");
  myNumArgBox->setMinMax(1,4);
  myNumArgBox->setValueChangedCB(FFaDynCB1M(FuiFunctionProperties,this,
					    onEngineFuncChanged,int));

  // File function

  myChannelNameField->setSensitivity(false);

  myChannelBrowseButton->setLabel("...");
  myChannelBrowseButton->setMaxWidth(25);
  myChannelBrowseButton->setActivateCB(FFaDynCB0M(FFaDynCB0,&myGetChannelListCB,invoke));

  myChannelSelectUI->setTitle("Channel list");
  myChannelSelectUI->setApplyButtonClickedCB(FFaDynCB1M(FuiFunctionProperties,this,
							onChannelDialogApply,int));
  myChannelSelectUI->setOkButtonClickedCB(FFaDynCB1M(FuiFunctionProperties,this,
						     onChannelDialogOK,int));
  myChannelSelectUI->setCancelButtonClickedCB(FFaDynCB1M(FuiFunctionProperties,this,
							 onChannelDialogCancel,int));

  myScaleFactorField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myScaleFactorField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
					       onParameterValueChanged,double));

  myVerticalShiftField->setLabel("Additional shift  ");
  myVerticalShiftField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myVerticalShiftField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
						 onParameterValueChanged,double));

  myZeroAdjustToggle->setLabel("Shift function to zero out start value");
  myZeroAdjustToggle->setToggleCB(FFaDynCB1M(FuiFunctionProperties,this,
					     onToggleChanged,bool));

  // External function

  myExtFuncFields.front()->setSensitivity(false);
  for (int i = 1; i < 3; i++)
  {
    myExtFuncFields[i]->setInputCheckMode(FFuIOField::DOUBLECHECK);
    myExtFuncFields[i]->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
                                                 onParameterValueChanged,double));
  }

  // File reference

  myFileBrowseButton->setLabel("...");
  myFileBrowseButton->setMaxWidth(25);
  myFileBrowseButton->setActivateCB(FFaDynCB0M(FuiFunctionProperties,this,
					       onFileBrowseActivated));

  myFileRefQueryField->setChangedCB(FFaDynCB1M(FuiFunctionProperties,this,
					       onFileQIChanged,FuiQueryInputField*));
  myFileRefQueryField->setBehaviour(FuiQueryInputField::REF_TEXT,true);
  myFileRefQueryField->setEditSensitivity(false);

  // Help and

  myHelpFrame->setLook(FFuFrame::PANEL_SUNKEN);

  // Preview

#ifdef FT_HAS_PREVIEW
  myPreviewButton->setLabel("Show");
  myPreviewButton->setActivateCB(FFaDynCB0M(FFaDynCB0,&myPreviewCB,invoke));

  myX0Field->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myX0Field->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
				      onParameterValueChanged,double));
  myXNField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myXNField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
				      onParameterValueChanged,double));
  myDXField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myDXField->setAcceptedCB(FFaDynCB1M(FuiFunctionProperties,this,
				      onParameterValueChanged,double));

  myUseSmartPointsToggle->setLabel("Auto");
  myUseSmartPointsToggle->setToggleCB(FFaDynCB1M(FuiFunctionProperties,this,
                                                 onToggleChanged,bool));
#endif

  // Jonswap wave spectrum

  myJonswapA->initWidgets(FFaDynCB1M(FuiFunctionProperties,this,onParameterValueChanged,double),
                          FFaDynCB1M(FuiFunctionProperties,this,onSelectionChanged,int),
                          FFaDynCB1M(FuiFunctionProperties,this,onToggleChanged,bool));
  myJonswapB->initWidgets(FFaDynCB1M(FuiFunctionProperties,this,onParameterValueChanged,double),
                          FFaDynCB1M(FuiFunctionProperties,this,onToggleChanged,bool));

  // User-defined wave spectrum

  myWaveSpec->initWidgets(FFaDynCB1M(FuiFunctionProperties,this,onSelectionChanged,int),
                          FFaDynCB2M(FuiFunctionProperties,this,
                                     onBrowseFileOpened,const std::string&,int));

  FFuUAExistenceHandler::invokeCreateUACB(this);
}


void FuiFunctionProperties::FuiJonswapAdvanced::initWidgets(const FFaDynCB1<double>& doubleCB,
                                                            const FFaDynCB1<int>& intCB,
                                                            const FFaDynCB1<bool>& boolCB)
{
  mySpectralPeakednessField->setLabel("Spectral peakedness, <font face='Symbol'><font size='+1'>g</font></font>");
  mySpectralPeakednessField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  mySpectralPeakednessField->setAcceptedCB(doubleCB);

  mySpectralPeakednessToggle->setLabel("Use DNV recommendation");
  mySpectralPeakednessToggle->setToggleCB(boolCB);

  myWaveComponentsField->setLabel("Number of wave components");
  myWaveComponentsField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  myWaveComponentsField->myField->setAcceptedCB(intCB);

  myRandomSeedField->setLabel("Rnd. seed");
  myRandomSeedField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  myRandomSeedField->myField->setAcceptedCB(intCB);

  myWaveDirsField->setLabel("Number of wave directions");
  myWaveDirsField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  myWaveDirsField->myField->setAcceptedCB(intCB);

  int labelWidth = myWaveComponentsField->myLabel->getWidthHint();
  myWaveDirsField->setLabelWidth(labelWidth);
  mySpectralPeakednessField->setLabelWidth(labelWidth);
}


void FuiFunctionProperties::FuiJonswapBasic::initWidgets(const FFaDynCB1<double>& doubleCB,
                                                         const FFaDynCB1<bool>& boolCB)
{
  myHsField->setLabel("Significant wave height, Hs");
  myHsField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myHsField->setAcceptedCB(doubleCB);

  myTpField->setLabel("Spectral peak period, Tp");
  myTpField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myTpField->setAcceptedCB(doubleCB);
  myTpField->setLabelWidth(myHsField->myLabel->getWidthHint());

  myCutOffToggle->setLabel("Auto calculate period cut-off values");
  myCutOffToggle->setToggleCB(boolCB);

  myTLowField->setLabel("T<sub>low</sub>");
  myTLowField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myTLowField->setAcceptedCB(doubleCB);

  myTHighField->setLabel("T<sub>high</sub>");
  myTHighField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myTHighField->setAcceptedCB(doubleCB);
}


void FuiFunctionProperties::FuiWaveSpectrum::initWidgets(const FFaDynCB1<int>& intCB,
                                                         const FFaDynCB2<const std::string&,int>& strCB)
{
  myFileField->setLabel("Wave spectrum file");
  myFileField->setButtonLabel("...",25);
  myFileField->setAbsToRelPath("yes");
  myFileField->setFileOpenedCB(strCB);
  static std::vector<std::string> ascii({ "asc", "txt" });
  myFileField->addDialogFilter("ASCII file",ascii,true);
  myFileField->setDialogRememberKeyword("WaveSpectrum");

  myRandomSeedField->setLabel("Random seed");
  myRandomSeedField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  myRandomSeedField->myField->setAcceptedCB(intCB);

  myHsField->setLabel("Significant wave height, Hs");
  myHsField->setSensitivity(false);

  myTpField->setLabel("Mean wave period, Tz");
  myTpField->setSensitivity(false);

  int labelWidth = myHsField->myLabel->getWidthHint();
  myTpField->setLabelWidth(labelWidth);
  myRandomSeedField->setLabelWidth(labelWidth);
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
    else if (!fv->myArgumentValues.empty())
      myInputSelectors->setValues(fv->myArgumentValues);
  }

  if (fv->showLinkToFields)
  {
    myEngineFunction->setSelectedRef(fv->mySelectedLinkFunction);
    this->setParameterSensitivity(false);
  }
  else
    this->setParameterSensitivity(IAmSensitive);

  // Issue #130: Friction objects are identified by TypeIdx > 1000
  if (fv->mySelectedFunctionTypeIdx > 1000)
    myFrictionType = fv->mySelectedFunctionTypeIdx;
  else if (IAmShowingTypeSwitch)
    myTypeSwitch->selectOption(fv->mySelectedFunctionTypeIdx);

  if (fv->showFileView)
  {
    myFileRefQueryField->setSelectedRef(fv->selectedFileRef);
    myFileRefQueryField->setText(fv->fileName);
    myChannelNameField->setValue(fv->myChannelName);
    myChannelBrowseButton->setSensitivity(fv->isMultiChannel && IAmSensitive);
    myZeroAdjustToggle->setValue(fv->myZeroAdjust);
    myScaleFactorField->setValue(fv->myScaleFactor);
    myVerticalShiftField->setValue(fv->myVerticalShift);
  }

  else if (fv->showParameterView)
    myParameterView->setValues(fv->mySelectedFunctionTypeIdx,
                               fv->myParameterValues);

  else if (fv->showMathExpr)
  {
    myExprMemo->setAllText(fv->myExpression.c_str());
    if (fv->showArgument)
    {
      myNumArgBox->setIntValue(fv->myArgumentValues.size());
      myNumArgBox->setSensitivity(IAmSensitive && !fv->showLinkToFields);
    }
  }

  else if (fv->myExtFunc > 0)
  {
    myExtFuncFields[0]->setValue(fv->myExtFunc);
    myExtFuncFields[1]->setValue(fv->myScaleFactor);
    myExtFuncFields[2]->setValue(fv->myVerticalShift);
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
    myChannelBrowseButton->setSensitivity(fv->isMultiChannel && IAmSensitive);
    myYValueInputField->setSensitivity(IAmSensitive && fv->showParameterList > 1);
  }

  if ((IAmShowingOutput = fv->useAsOutputSensor+1) > 0)
  {
    myOutputToggle->setValue(fv->useAsOutputSensor > 0);
    if (IAmShowingOutput == 3)
    {
      myThreshold2->setValues(fv->myThreshold);
      // Add the threshold fields as a (second or) third tab
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
      // Use the threshold frame instead
      myThresholds->setValues(fv->myThreshold);
      // Remove the threshold fields tab
      if (myTabStack->removeTabPage(myThreshold2))
        myTabStack->renameTabPage(myHelpFrame, "Parameter Help");
    }
    else // No tab widget - use the threshold frame instead
      myThresholds->setValues(fv->myThreshold);
  }

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
    myJonswapB->myHsField->setValue(fv->myJonswapHs);
    myJonswapB->myTpField->setValue(fv->myJonswapTp);
    myJonswapB->myCutOffToggle->setValue(fv->myJonswapAutoCalcCutoff);
    myJonswapB->myTLowField->setValue(fv->myJonswapRange.first);
    myJonswapB->myTHighField->setValue(fv->myJonswapRange.second);
    myJonswapA->mySpectralPeakednessToggle->setValue(fv->myJonswapAutoCalcSpectralPeakedness);
    myJonswapA->mySpectralPeakednessField->setValue(fv->myJonswapSpectralPeakedness);
    myJonswapA->myWaveComponentsField->myField->setValue(fv->myJonswapNComp);
    myJonswapA->myRandomSeedField->myField->setValue(fv->myJonswapRandomSeed);
    myJonswapA->myWaveDirsField->myField->setValue(fv->myJonswapNDir);
    myJonswapA->mySpreadExpField->myField->setValue(fv->myJonswapSprExp);
    this->setJonswapSensitivity();
  }
  else if (IAmShowingPrmView[UDWS])
  {
    myWaveSpec->myFileField->setFileName(fv->fileName);
    myWaveSpec->myFileField->setAbsToRelPath(fv->modelFilePath);
    myWaveSpec->myRandomSeedField->myField->setValue(fv->myJonswapRandomSeed);
    myWaveSpec->myHsField->setValue(fv->myJonswapHs);
    myWaveSpec->myTpField->setValue(fv->myJonswapTp);
  }
}


void FuiFunctionProperties::buildDynamicWidgets(const FFuaUIValues* values)
{
  const FuaFunctionPropertiesValues* fv = dynamic_cast<const FuaFunctionPropertiesValues*>(values);
  if (!fv) return;

  IAmShowingPrmView.fill(false);
  IAmShowingPrmView[LIST] = fv->showParameterList;
  IAmShowingPrmView[VIEW] = fv->showParameterView;
  if (fv->showFileView)
  {
    if (fv->mySelectedFunctionTypeIdx == 2)
      IAmShowingPrmView[UDWS] = true; // User-defined wave spectrum
    else
      IAmShowingPrmView[FILE] = fv->isMultiChannel ? 2 : 1; // Other file views
  }
  IAmShowingPrmView[MATH] = fv->showMathExpr;
  IAmShowingPrmView[EXTF] = fv->myExtFunc > 0;
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
    myTypeSwitch->setOptions(fv->myFunctionTypes);
  }
  else
    myTypeFrame->popDown();

  // Arguments

  if (IAmShowingArgument == 1) {
    myInputSelector->popUp();
    myInputSelectors->popDown();
  }
  else if (IAmShowingArgument > 1) {
    myInputSelector->popDown();
    myInputSelectors->popUp(fv->myArgumentValues.size());
  }
  else {
    myInputSelector->popDown();
    myInputSelectors->popDown();
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
    myEngineFunction->setQuery(fv->myLinkFunctionQuery);
    myEngineFunction->popUp();
  }
  else
    myEngineFunction->popDown();

  // Parameter frames

  for (size_t i = 0; i < myParameterFrames.size(); i++)
    if (IAmShowingPrmView[i])
      myParameterFrames[i]->popUp();
    else
      myParameterFrames[i]->popDown();

  // Parameter view

  if (IAmShowingPrmView[VIEW]) {
    myParameterView->setFields(fv->mySelectedFunctionTypeIdx,
                               fv->myParameterNames,
			       myParameterFrames[VIEW]);
    myExpandButton->setLabel(">");
    myExpandButton->setToolTip("Expand the Parameters view");
    this->placeExpandButton();
  }

  // File view

  if (IAmShowingPrmView[FILE])
    myFileRefQueryField->setQuery(fv->fileRefQuery);

  // Math expression

  if (IAmShowingPrmView[MATH])
    switch (std::abs(static_cast<int>(IAmShowingArgument))) {
    case 1: myExprLabel->setLabel("Enter a function of x"); break;
    case 2: myExprLabel->setLabel("Enter a function of x,y"); break;
    case 3: myExprLabel->setLabel("Enter a function of x,y,z"); break;
    case 4: myExprLabel->setLabel("Enter a function of x,y,z,t"); break;
    }

  if (IAmShowingPrmView[MATH] && IAmShowingArgument > 0) {
    myNumArgLabel->popUp();
    myNumArgBox->popUp();
  }
  else {
    myNumArgLabel->popDown();
    myNumArgBox->popDown();
  }

  // Jonswap wave spectrum

  if (IAmShowingJonswap) {
    myJonswapA->popUp();
    myJonswapB->popUp();
  }
  else {
    myJonswapA->popDown();
    myJonswapB->popDown();
  }

  // Help picture

  myHelpLabel->setPixMap(fv->myHelpPixmap);

  // Curve preview

  if (IAmShowingCurvePreview || IAmShowingHelpPixmap) {
#ifdef FT_HAS_PREVIEW
    myTabStack->setTabSensitivity(1,IAmShowingCurvePreview);
#endif
    myTabStack->popUp();
  }
  else
    myTabStack->popDown();
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
  if (IAmShowingTypeSwitch)
    values.mySelectedFunctionTypeIdx = myTypeSwitch->getSelectedOption();
  else // Issue #130: Friction objects are identified by TypeIdx > 1000
    values.mySelectedFunctionTypeIdx = myFrictionType;

  values.showParameterView = IAmShowingPrmView[VIEW] > 0;
  values.showFileView      = IAmShowingPrmView[FILE] > 0;
  values.showMathExpr      = IAmShowingPrmView[MATH] > 0;
  values.showParameterList = IAmShowingPrmView[LIST] > 0;

  if (IAmShowingPrmView[FILE]) {
    if (myFileRefQueryField->isAConstant()) {
      values.fileName = myFileRefQueryField->getText();
      values.selectedFileRef = NULL;
    }
    else {
      values.fileName = "";
      values.selectedFileRef = myFileRefQueryField->getSelectedRef();
    }
    values.myChannelName = myChannelNameField->getValue();
    values.myScaleFactor = myScaleFactorField->getDouble();
    values.myVerticalShift = myVerticalShiftField->getValue();
    values.myZeroAdjust = myZeroAdjustToggle->getValue();
  }
  else if (IAmShowingPrmView[UDWS]) {
    values.fileName = myWaveSpec->myFileField->getFileName();
    values.myJonswapRandomSeed = myWaveSpec->myRandomSeedField->myField->getInt();
    values.myJonswapHs = myWaveSpec->myHsField->getValue();
    values.myJonswapTp = myWaveSpec->myTpField->getValue();
  }
  else if (IAmShowingJonswap) {
    values.myJonswapHs = myJonswapB->myHsField->getValue();
    values.myJonswapTp = myJonswapB->myTpField->getValue();
    values.myJonswapAutoCalcCutoff = myJonswapB->myCutOffToggle->getValue();
    values.myJonswapRange = { myJonswapB->myTLowField->getValue(), myJonswapB->myTHighField->getValue() };
    values.myJonswapAutoCalcSpectralPeakedness = myJonswapA->mySpectralPeakednessToggle->getValue();
    values.myJonswapSpectralPeakedness = myJonswapA->mySpectralPeakednessField->getValue();
    values.myJonswapNComp = myJonswapA->myWaveComponentsField->myField->getInt();
    values.myJonswapRandomSeed = myJonswapA->myRandomSeedField->myField->getInt();
    values.myJonswapNDir = myJonswapA->myWaveDirsField->myField->getInt();
    values.myJonswapSprExp = myJonswapA->mySpreadExpField->myField->getInt();
  }
  else if (IAmShowingPrmView[VIEW])
    myParameterView->getValues(values.mySelectedFunctionTypeIdx,
                               values.myParameterValues);

  else if (IAmShowingPrmView[LIST])
    values.myExtrapolationType = myExtrapolationSwitch->getSelectedOption();

  else if (IAmShowingPrmView[MATH])
    myExprMemo->getText(values.myExpression);

  else if (IAmShowingPrmView[EXTF]) {
    values.myExtFunc = myExtFuncFields[0]->getInt();
    values.myScaleFactor = myExtFuncFields[1]->getDouble();
    values.myVerticalShift = myExtFuncFields[2]->getDouble();
  }
  else {
    values.myScaleFactor = myScaleFactorField->getDouble();
    values.myVerticalShift = myVerticalShiftField->getValue();
  }

  if (IAmShowingArgument > 0)
    values.myArgumentValues.resize(IAmShowingArgument);
  else
    values.myArgumentValues.clear();
  if (IAmShowingArgument == 1)
    myInputSelector->getValues(values.myArgumentValues.front());
  else
    myInputSelectors->getValues(values.myArgumentValues);

  if (IAmShowingPrmView[MATH])
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
  myChannelSelectUI->myItemSelector->setItems(channels);
  myChannelSelectUI->myItemSelector->selectItem(0);
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
  if (IAmShowingPrmView[LIST] < 2)
    myAddNumberCB.invoke(d);
}


void FuiFunctionProperties::onYAccepted(double)
{
  this->onAddButtonActivated();
  myXValueInputField->giveKeyboardFocus();
}


void FuiFunctionProperties::onAddButtonActivated()
{
  if (IAmShowingPrmView[LIST] > 1)
    myAddNumbersCB.invoke(myXValueInputField->getDouble(),myYValueInputField->getDouble());
  else
    myAddNumberCB.invoke(myXValueInputField->getDouble());
}


void FuiFunctionProperties::onDeleteButtonActivated()
{
  int currItem = myParameterList->getSelectedItemIndex();
  if (currItem > -1)
    myDeleteCB.invoke(currItem);

  if (myParameterList->selectItem(currItem))
    this->onParameterSelected(currItem);
}


void FuiFunctionProperties::onChannelDialogOK(int index)
{
  this->onChannelDialogApply(index);
  myChannelSelectUI->popDown();
}


void FuiFunctionProperties::onChannelDialogApply(int index)
{
  if (myChannelSelectUI->myItemSelector->isItemSelected(index))
    myChannelNameField->setValue(myChannelSelectUI->myItemSelector->getItemText(index));

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

  if (IAmShowingPrmView[LIST] > 1) {
    sscanf(txt.c_str(), "%lf, %lf", &x,&y);
    myXValueInputField->setValue(x);
    myYValueInputField->setValue(y);
  }
  else {
    sscanf(txt.c_str(), "%lf", &x);
    myXValueInputField->setValue(x);
    myYValueInputField->setValue("");
  }
}


void FuiFunctionProperties::onExprApplyButtonActivated()
{
  this->onValuesChanged();
  myAcceptExprCB.invoke();
}


void FuiFunctionProperties::onExpandButtonActivated()
{
  if (IAmShowingPrmView[VIEW] > 1)
  {
    IAmShowingCurvePreview  = IAmShowingPrmView[VIEW]%2 > 0;
    IAmShowingHelpPixmap    = IAmShowingPrmView[VIEW] > 2;
    IAmShowingPrmView[VIEW] = 1;
    myExpandButton->setLabel(">");
    myExpandButton->setToolTip("Expand the Parameters view");
    if (IAmShowingCurvePreview || IAmShowingHelpPixmap)
      myTabStack->popUp();
  }
  else
  {
    if (IAmShowingCurvePreview) IAmShowingPrmView[VIEW] += 1;
    if (IAmShowingHelpPixmap)   IAmShowingPrmView[VIEW] += 2;
    IAmShowingCurvePreview = IAmShowingHelpPixmap = false;
    myExpandButton->setLabel("<");
    myExpandButton->setToolTip("Contract the Parameters view");
    myTabStack->popDown();
  }
  this->placeExpandButton();
}


void FuiFunctionProperties::placeExpandButton()
{
  int w = myParameterFrames[VIEW]->getWidth();
  myExpandButton->setSizeGeometry(w-22,0,20,18);
  myExpandButton->toFront();
}


void FuiFunctionProperties::setSensitivity(bool isSensitive)
{
  IAmSensitive = isSensitive;

  myTypeSwitch->setSensitivity(isSensitive && IAmAllowingTopolChange);
  myEngineFunction->setSensitivity(isSensitive && IAmAllowingTopolChange);
  myInputSelector->setSensitivity(isSensitive && IAmAllowingTopolChange);
  myInputSelectors->setSensitivity(isSensitive && IAmAllowingTopolChange);

  this->setParameterSensitivity(isSensitive && !IAmShowingLinkToFields);
}


void FuiFunctionProperties::setParameterSensitivity(bool isSensitive)
{
  myParameterView->setSensitivity(isSensitive);
  myParameterList->setSensitivity(isSensitive);
  myXValueInputField->setSensitivity(isSensitive);
  myYValueInputField->setSensitivity(isSensitive && IAmShowingPrmView[LIST] > 1);
  myAddButton->setSensitivity(isSensitive);
  myDeleteButton->setSensitivity(isSensitive);
  myFileRefQueryField->setSensitivity(isSensitive);
  myFileBrowseButton->setSensitivity(isSensitive);
  myChannelBrowseButton->setSensitivity(isSensitive && IAmShowingPrmView[FILE] == 2);
  myScaleFactorField->setSensitivity(isSensitive);
  myVerticalShiftField->setSensitivity(isSensitive);
  myZeroAdjustToggle->setSensitivity(isSensitive);
  myExprMemo->setSensitivity(isSensitive);
  myExprApplyButton->setSensitivity(isSensitive);
  myExtrapolationSwitch->setSensitivity(isSensitive);
  myExtFuncFields[1]->setSensitivity(isSensitive);
  myExtFuncFields[2]->setSensitivity(isSensitive);

  myJonswapA->mySpectralPeakednessToggle->setSensitivity(isSensitive);
  myJonswapA->myWaveComponentsField->setSensitivity(isSensitive);
  myJonswapA->myRandomSeedField->setSensitivity(isSensitive);
  myJonswapA->myWaveDirsField->setSensitivity(isSensitive);
  myJonswapB->myHsField->setSensitivity(isSensitive);
  myJonswapB->myTpField->setSensitivity(isSensitive);
  myJonswapB->myCutOffToggle->setSensitivity(isSensitive);
  this->setJonswapSensitivity(isSensitive);
  myWaveSpec->myFileField->setSensitivity(isSensitive);
  myWaveSpec->myRandomSeedField->setSensitivity(isSensitive);
}


void FuiFunctionProperties::setJonswapSensitivity(bool isSensitive)
{
  // Disable Tmin and Tmax based on CutOffToggle
  bool bVal = isSensitive && !myJonswapB->myCutOffToggle->getValue();
  myJonswapB->myTLowField->setSensitivity(bVal);
  myJonswapB->myTHighField->setSensitivity(bVal);
  // Disable SpectralPeakedness based on SpectralPeakednessToggle
  bVal = isSensitive && !myJonswapA->mySpectralPeakednessToggle->getValue();
  myJonswapA->mySpectralPeakednessField->setSensitivity(bVal);
  bVal = isSensitive && myJonswapA->myWaveDirsField->myField->getInt() > 1;
  // Disable spreading exponent field if only one wave direction
  myJonswapA->mySpreadExpField->setSensitivity(bVal);
}


void FuiFunctionProperties::onFileBrowseActivated()
{
  myFileRefQueryField->setSelectedRef(NULL);
  myFilesBrowseCB.invoke();

  this->onValuesChanged();
}
