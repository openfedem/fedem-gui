// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_FUNCTION_PROPERTIES_H
#define FUI_FUNCTION_PROPERTIES_H

#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "FFuLib/FFuBase/FFuMultUIComponent.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "vpmUI/vpmUIComponents/FuiFunctionDomain.H"
#include "vpmUI/vpmUIComponents/FuiInputSelector.H"
#include "vpmUI/vpmUIComponents/FuiThreshold.H"

class FuiParameterView;
class FuiQueryInputField;
class FuiQueryInputFieldValues;
class FFuIOField;
class FFuOptionMenu;
class FFuPushButton;
class FFuLabelFrame;
class FFuTable;
class FFuMemo;
class FFuFrame;
class FFuLabel;
class FFuLabelField;
class FFuSpinBox;
class FFuScrolledList;
class FFuToggleButton;
class FFuScrolledListDialog;
class FFuTabbedWidgetStack;

class FFuaQueryBase;
class FmModelMemberBase;

class FuaFunctionPropertiesValues;


class FuiFunctionProperties : public virtual FFuMultUIComponent,
			      public FFuUAExistenceHandler,
			      public FFuUADataHandler
{
public:
  FuiFunctionProperties();
  virtual ~FuiFunctionProperties() {}

  void setUIValues(const FFuaUIValues* values);
  void buildDynamicWidgets(const FFuaUIValues* values);
  void setSensitivity(bool isSensitive);
  void setCBs(const FFuaUIValues* values);

  void setChannelList(const std::vector<std::string>& channels);
  void popUpChannelList();

  void getValues(FuaFunctionPropertiesValues& values);
#ifdef FT_HAS_PREVIEW
  void getPreviewDomain(FuaFunctionDomain& domain);
#endif
  int  getSelectedFunctionIndex() const;

protected:
  virtual void placeWidgets(int width, int height);
  virtual void setNoArgs(size_t narg) = 0;

  // Initialisation to be called by UILib-dependent subclass

  void initWidgets();

private:
  void setParameterSensitivity(bool isSensitive);
  void setJonswapSensitivity(bool isSensitive = true);

  // Internal callback forwarding:

  void onValuesChanged(bool widgetChanges = false);
  void onSelectionChanged(int) { this->onValuesChanged(); }
  void onEngineFuncChanged(int) { this->onValuesChanged(true); }
  void onParameterChanged(char*) { this->onValuesChanged(); }
  void onParameterValueChanged(double) { this->onValuesChanged(); }
  void onToggleChanged(bool) { this->onValuesChanged(); }
  void onOutputToggled(bool) { this->onValuesChanged(true); }
  void onSensorPicked()  { myPickSensorCB.invoke(0); }
  void onSensor1Picked() { myPickSensorCB.invoke(1); }
  void onSensor2Picked() { myPickSensorCB.invoke(2); }
  void onSensor3Picked() { myPickSensorCB.invoke(3); }
  void onSensor4Picked() { myPickSensorCB.invoke(4); }
  void onSensor5Picked() { myPickSensorCB.invoke(5); }
  void onSensor6Picked() { myPickSensorCB.invoke(6); }
  void onSensor7Picked() { myPickSensorCB.invoke(7); }
  void onSensor8Picked() { myPickSensorCB.invoke(8); }
  void onSensor9Picked() { myPickSensorCB.invoke(9); }
  void onXAccepted(double);
  void onYAccepted(double);
  void onAddButtonActivated();
  void onDeleteButtonActivated();
  void onParameterSelected(int);
  void onBrowseFileOpened(const std::string&, int);
  void onChannelDialogOK(int);
  void onChannelDialogCancel(int);
  void onChannelDialogApply(int);
  void onExprApplyButtonActivated();
  void onExpandButtonActivated();
  void onFileBrowseActivated();
  void onFileQIChanged(FuiQueryInputField*) { this->onValuesChanged(); }
  void onInputSelectorValuesChanged(FuiInputSelectorValues&) { this->onValuesChanged(); }
  void onInputSelectorSetDBValues(FuiInputSelectorValues&) { this->updateDBValues(false); }

protected:

  // Shadow pointers from subclass.
  // Subclass is responsible to set them up:

  FFuLabelFrame* myTypeFrame;
  FFuOptionMenu* myTypeSwitch;

  FuiQueryInputField* myEngineFunction;
  FuiInputSelector*   myInputSelector;

  FFuTable*                      myArgumentTable;
  std::vector<FuiInputSelector*> myArguments;

  FFuToggleButton* myOutputToggle;
  FFuLabelFrame*   myThresholdFrame;
  FuiThreshold*    myThresholds;
  FuiThreshold*    myThreshold2;

  FFuFrame      * myHelpFrame;
  FFuLabel      * myHelpLabel;

  FFuLabelFrame * myParameterFrame;

  FFuLabel      * myExprLabel;
  FFuMemo       * myExprMemo;
  FFuPushButton * myExprApplyButton;
  FFuLabel      * myNumArgLabel;
  FFuSpinBox    * myNumArgBox;

  FFuPushButton*      myFileBrowseButton;
  FuiQueryInputField* myFileRefQueryField;
  FFuLabel*           myFileBrowseLabel;
  FFuLabelField*      myActualFileInfoLabel;

  FFuLabelField * myChannelNameField;
  FFuPushButton * myChannelBrowseButton;

  FFuScrolledListDialog * myChannelSelectUI;

  FFuLabelField   * myScaleFactorField;
  FFuLabelFrame   * myVerticalShiftFrame;
  FFuLabelField   * myVerticalShiftField;
  FFuToggleButton * myZeroAdjustToggle;

  FuiParameterView* myParameterView;
  FFuPushButton*    myExpandButton;

  FFuScrolledList * myParameterList;
  FFuIOField    * myXValueInputField;
  FFuIOField    * myYValueInputField;
  FFuLabel      * myXLabel;
  FFuLabel      * myYLabel;
  FFuLabel      * myExtrapolationLabel;
  FFuOptionMenu * myExtrapolationSwitch;

  FFuPushButton * myAddButton;
  FFuPushButton * myDeleteButton;

#ifdef FT_HAS_PREVIEW
  FFuPushButton*   myPreviewButton;
  FFuToggleButton* myUseSmartPointsToggle;
  FFuLabel*        myX0Label;
  FFuLabel*        myDXLabel;
  FFuIOField*      myX0Field;
  FFuIOField*      myXNField;
  FFuIOField*      myDXField;
#endif

  // Jonswap wave spectrum
  FFuLabelFrame   * myJonswapAdvancedFrame;
  FFuLabelField   * myJonswapSpectralPeakednessField;
  FFuToggleButton * myJonswapSpectralPeakednessToggle;
  FFuLabelField   * myJonswapWaveComponentsField;
  FFuLabelField   * myJonswapRandomSeedField;
  FFuLabelField   * myJonswapWaveDirsField;
  FFuLabelField   * myJonswapSpreadExpField;
  FFuLabelFrame   * myJonswapBasicFrame;
  FFuLabelField   * myJonswapHsField;
  FFuLabelField   * myJonswapTpField;
  FFuLabelFrame   * myJonswapCutOffFrame;
  FFuToggleButton * myJonswapCutOffToggle;
  FFuLabelField   * myJonswapTLowField;
  FFuLabelField   * myJonswapTHighField;

  FFuTabbedWidgetStack* myTabStack;

private:
  char IAmShowingParameterList;
  char IAmShowingParameterView;
  char IAmShowingFileView;
  bool IAmShowingTypeSwitch;
  bool IAmShowingMathExpr;
  bool IAmShowingCurvePreview;
  char IAmShowingArgument;
  char IAmShowingOutput;
  bool IAmShowingLinkToFields;
  bool IAmShowingHelpPixmap;
  bool IAmShowingJonswap;
  int  myMinParameterWidth;

  bool IAmSensitive;
  bool IAmAllowingTopolChange;

  // Callbacks

  FFaDynCB1<int> myFuncTypeSwitchedCB;
  FFaDynCB1<double> myAddNumberCB;
  FFaDynCB2<double,double> myAddNumbersCB;
  FFaDynCB1<int> myDeleteCB;
  FFaDynCB0 myClearAllCB;
#ifdef FT_HAS_PREVIEW
  FFaDynCB0 myPreviewCB;
#endif
  FFaDynCB0 myGetChannelListCB;
  FFaDynCB0 myFilesBrowseCB;
  FFaDynCB0 myAcceptExprCB;
  FFaDynCB1<int> myPickSensorCB;
};


class FuaFunctionPropertiesValues : public FFuaUIValues
{
public:
  FuaFunctionPropertiesValues()
    {
      showTypeSwitch    = false;
      showArgument      = false;
      showLinkToFields  = false;
      showParameterList = false;
      showParameterView = false;
      showFileView      = false;
      showMathExpr      = false;
      showCurvePreview  = false;
      showJonswapView   = false;

      mySelectedFunctionTypeIdx = 0;
      useAsOutputSensor = -1;
      mySelectedLinkFunction = NULL;

      mySelectedValueIdx = 0;
      myExtrapolationType = 0;

      isMultiChannel  = false;
      myChannelName   = "Not set";
      myScaleFactor   = 1.0;
      myVerticalShift = 0.0;
      myZeroAdjust    = false;
      myExtFunc       = 0;

      fileRefQuery    = NULL;
      selectedFileRef = NULL;

      myHelpPixmap = NULL;

      allowTopolChange = true;

      myJonswapHs = myJonswapTp = 0.0;
      myJonswapAutoCalcCutoff = false;
      myJonswapRange = { 0.0, 0.0 };
      myJonswapAutoCalcSpectralPeakedness = false;
      myJonswapSpectralPeakedness = 0.0;
      myJonswapNComp = 0;
      myJonswapRandomSeed = 0;
      myJonswapNDir = 1;
      myJonswapSprExp = 2;
    }

  // Type switch

  bool           showTypeSwitch;
  std::vector<std::string> myFunctionTypes;
  int            mySelectedFunctionTypeIdx;
  FFaDynCB1<int> myFuncTypeSwitchedCB;

  // Argument

  bool           showArgument;
  std::vector<FuiInputSelectorValues> myArgumentValues;
  FFaDynCB1<int> myPickSensorCB;

  // Digital twin output sensor settings

  char            useAsOutputSensor;
  ThresholdValues myThreshold;

  // Link to other function

  bool               showLinkToFields;
  FFuaQueryBase*     myLinkFunctionQuery;
  FmModelMemberBase* mySelectedLinkFunction;

  FFaDynCB1<FuiQueryInputFieldValues&> myEditLinkFunctionCB;

  // Parameter view

  bool                     showParameterView;
  std::vector<std::string> myParameterNames;
  std::vector<double>      myParameterValues;

  // Scrolled list of values

  char                showParameterList;
  std::vector<double> myXValues;
  std::vector<double> myYValues;
  int                 mySelectedValueIdx;

  FFaDynCB1<double>        myAddNumberCB;
  FFaDynCB2<double,double> myAddNumbersCB;
  FFaDynCB1<const std::string&> myPasteCB;
  FFaDynCB1<int>           myDeleteCB;
  FFaDynCB0                myClearAllCB;

  // Extrapolation switch

  int            myExtrapolationType;
  FFaDynCB1<int> myExtrapolationTypeSwitchedCB;

  // External device parameters

  bool        showFileView;
  std::string fileName;
  bool        isMultiChannel;
  std::string myChannelName;
  double      myScaleFactor;
  double      myVerticalShift;
  bool        myZeroAdjust;
  int         myExtFunc;

  FFuaQueryBase*     fileRefQuery;
  FmModelMemberBase* selectedFileRef;

  FFaDynCB0          myFilesBrowseCB;
  FFaDynCB0          myGetChannelListCB;

  // Math expression

  bool        showMathExpr;
  std::string myExpression;
  FFaDynCB0   myAcceptExprCB;

  // Help picture

  const char** myHelpPixmap;

  // Function preview
  bool              showCurvePreview;
#ifdef FT_HAS_PREVIEW
  FuaFunctionDomain previewDomain;
  FFaDynCB0         myPreviewCB;
#endif

  bool allowTopolChange;

  // Jonswap wave spectrum

  bool   showJonswapView;
  double myJonswapHs;
  double myJonswapTp;
  bool   myJonswapAutoCalcCutoff;
  std::pair<double,double> myJonswapRange;
  bool   myJonswapAutoCalcSpectralPeakedness;
  double myJonswapSpectralPeakedness;
  int    myJonswapNComp;
  int    myJonswapRandomSeed;
  int    myJonswapNDir;
  int    myJonswapSprExp;
};

#endif
