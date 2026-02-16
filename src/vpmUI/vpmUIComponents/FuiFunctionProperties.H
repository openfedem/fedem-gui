// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_FUNCTION_PROPERTIES_H
#define FUI_FUNCTION_PROPERTIES_H

#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "vpmUI/vpmUIComponents/FuiFunctionDomain.H"
#include "vpmUI/vpmUIComponents/FuiInputSelector.H"
#include "vpmUI/vpmUIComponents/FuiThreshold.H"

class FuiParameterView;
class FuiQueryInputField;
class FuiQueryInputFieldValues;
class FFuIOField;
class FFuFileBrowseField;
class FFuOptionMenu;
class FFuPushButton;
class FFuLabelFrame;
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


class FuiFunctionProperties : public virtual FFuComponentBase,
                              public FFuUAExistenceHandler,
                              public FFuUADataHandler
{
public:
  FuiFunctionProperties();

  void setUIValues(const FFuaUIValues* values);
  void buildDynamicWidgets(const FFuaUIValues* values);
  void setSensitivity(bool isSensitive);
  void setCBs(const FFuaUIValues* values);
  void placeExpandButton();

  void setChannelList(const std::vector<std::string>& channels);
  void popUpChannelList();

  void getValues(FuaFunctionPropertiesValues& values);
#ifdef FT_HAS_PREVIEW
  void getPreviewDomain(FuaFunctionDomain& domain);
#endif
  int  getSelectedFunctionIndex() const;

protected:

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
  void onSensorPicked() { myPickSensorCB.invoke(0); }
  void onXAccepted(double);
  void onYAccepted(double);
  void onAddButtonActivated();
  void onDeleteButtonActivated();
  void onParameterSelected(int);
  void onBrowseFileOpened(const std::string&,int) { this->onValuesChanged(); }
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
  FuiInputSelectors*  myInputSelectors;

  FFuToggleButton* myOutputToggle;
  FFuLabelFrame*   myThresholdFrame;
  FuiThreshold*    myThresholds;
  FuiThreshold*    myThreshold2;

  FFuFrame* myHelpFrame;
  FFuLabel* myHelpLabel;

  enum { LIST, VIEW, FILE, UDWS, MATH, EXTF, NPRM };
  std::array<FFuLabelFrame*,NPRM> myParameterFrames;

  FFuMemo*       myExprMemo;
  FFuLabel*      myExprLabel;
  FFuPushButton* myExprApplyButton;

  FFuLabel*   myNumArgLabel;
  FFuSpinBox* myNumArgBox;

  FuiQueryInputField* myFileRefQueryField;
  FFuPushButton*      myFileBrowseButton;

  FFuIOField*    myChannelNameField;
  FFuPushButton* myChannelBrowseButton;

  FFuScrolledListDialog* myChannelSelectUI;

  FFuIOField*      myScaleFactorField;
  FFuLabelField*   myVerticalShiftField;
  FFuToggleButton* myZeroAdjustToggle;

  std::array<FFuIOField*,3> myExtFuncFields;

  FuiParameterView* myParameterView;
  FFuPushButton*    myExpandButton;

  FFuScrolledList* myParameterList;
  FFuIOField*      myXValueInputField;
  FFuIOField*      myYValueInputField;
  FFuPushButton*   myAddButton;
  FFuPushButton*   myDeleteButton;
  FFuOptionMenu*   myExtrapolationSwitch;

  class FuiJonswapAdvanced : public virtual FFuComponentBase
  {
  public:
    void initWidgets(const FFaDynCB1<double>& doubleCB,
                     const FFaDynCB1<int>& intCB,
                     const FFaDynCB1<bool>& boolCB);

    FFuLabelField*   mySpectralPeakednessField;
    FFuToggleButton* mySpectralPeakednessToggle;
    FFuLabelField*   myWaveComponentsField;
    FFuLabelField*   myRandomSeedField;
    FFuLabelField*   myWaveDirsField;
    FFuLabelField*   mySpreadExpField;
  };

  class FuiJonswapBasic : public virtual FFuComponentBase
  {
  public:
    void initWidgets(const FFaDynCB1<double>& doubleCB,
                     const FFaDynCB1<bool>& boolCB);

    FFuLabelField*   myHsField;
    FFuLabelField*   myTpField;
    FFuToggleButton* myCutOffToggle;
    FFuLabelField*   myTLowField;
    FFuLabelField*   myTHighField;
  };

  class FuiWaveSpectrum : public virtual FFuComponentBase
  {
  public:
    void initWidgets(const FFaDynCB1<int>& intCB,
                     const FFaDynCB2<const std::string&,int>& strCB);

    FFuFileBrowseField* myFileField;
    FFuLabelField*      myRandomSeedField;
    FFuLabelField*      myHsField;
    FFuLabelField*      myTpField;
  };

  FuiJonswapAdvanced* myJonswapA;
  FuiJonswapBasic*    myJonswapB;
  FuiWaveSpectrum*    myWaveSpec;

#ifdef FT_HAS_PREVIEW
  FFuPushButton*   myPreviewButton;
  FFuToggleButton* myUseSmartPointsToggle;
  FFuIOField*      myX0Field;
  FFuIOField*      myXNField;
  FFuIOField*      myDXField;
#endif

  FFuTabbedWidgetStack* myTabStack;

private:
  bool IAmSensitive;
  bool IAmAllowingTopolChange;

  std::array<char,NPRM> IAmShowingPrmView{};

  bool IAmShowingTypeSwitch;
  bool IAmShowingCurvePreview;
  char IAmShowingArgument;
  char IAmShowingOutput;
  bool IAmShowingLinkToFields;
  bool IAmShowingHelpPixmap;
  bool IAmShowingJonswap;

  int myFrictionType;

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
  bool allowTopolChange = true;

  // Type switch

  bool           showTypeSwitch = false;
  std::vector<std::string> myFunctionTypes;
  int            mySelectedFunctionTypeIdx = 0;
  FFaDynCB1<int> myFuncTypeSwitchedCB;

  // Argument

  bool           showArgument = false;
  std::vector<FuiInputSelectorValues> myArgumentValues;
  FFaDynCB1<int> myPickSensorCB;

  // Digital twin output sensor settings

  char            useAsOutputSensor = -1;
  ThresholdValues myThreshold;

  // Link to other function

  bool               showLinkToFields = false;
  FFuaQueryBase*     myLinkFunctionQuery = NULL;
  FmModelMemberBase* mySelectedLinkFunction = NULL;

  FFaDynCB1<FuiQueryInputFieldValues&> myEditLinkFunctionCB;

  // Parameter view

  bool                     showParameterView = false;
  std::vector<std::string> myParameterNames;
  std::vector<double>      myParameterValues;

  // Scrolled list of values

  char                showParameterList = false;
  std::vector<double> myXValues;
  std::vector<double> myYValues;
  int                 mySelectedValueIdx = 0;

  FFaDynCB1<double>        myAddNumberCB;
  FFaDynCB2<double,double> myAddNumbersCB;
  FFaDynCB1<const std::string&> myPasteCB;
  FFaDynCB1<int>           myDeleteCB;
  FFaDynCB0                myClearAllCB;

  // Extrapolation switch

  int            myExtrapolationType = 0;
  FFaDynCB1<int> myExtrapolationTypeSwitchedCB;

  // External device parameters

  bool        showFileView = false;
  std::string fileName;
  std::string modelFilePath;
  bool        isMultiChannel = false;
  std::string myChannelName = "Not set";
  double      myScaleFactor = 1.0;
  double      myVerticalShift = 0.0;
  bool        myZeroAdjust = false;
  int         myExtFunc = 0;

  FFuaQueryBase*     fileRefQuery = NULL;
  FmModelMemberBase* selectedFileRef = NULL;

  FFaDynCB0          myFilesBrowseCB;
  FFaDynCB0          myGetChannelListCB;

  // Math expression

  bool        showMathExpr = false;
  std::string myExpression;
  FFaDynCB0   myAcceptExprCB;

  // Jonswap wave spectrum

  bool   showJonswapView = false;
  double myJonswapHs = 0.0;
  double myJonswapTp = 0.0;
  bool   myJonswapAutoCalcCutoff = false;
  std::pair<double,double> myJonswapRange = { 0.0, 0.0 };
  bool   myJonswapAutoCalcSpectralPeakedness = false;
  double myJonswapSpectralPeakedness = 0.0;
  int    myJonswapNComp = 0;
  int    myJonswapRandomSeed = 0;
  int    myJonswapNDir = 1;
  int    myJonswapSprExp = 2;

  // Help picture

  const char** myHelpPixmap = NULL;

  // Function preview

  bool              showCurvePreview = false;
#ifdef FT_HAS_PREVIEW
  FuaFunctionDomain previewDomain;
  FFaDynCB0         myPreviewCB;
#endif
};

#endif
