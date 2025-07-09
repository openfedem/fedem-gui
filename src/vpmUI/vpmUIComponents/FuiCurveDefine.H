// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_CURVE_DEFINE_H
#define FUI_CURVE_DEFINE_H

#include "FFuLib/FFuBase/FFuMultUIComponent.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "FFuLib/FFuColor.H"
#include "FFuLib/FFuLogicButtonGroup.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "vpmUI/vpmUIComponents/FuiFunctionDomain.H"
#include <string>
#include <vector>
#include <array>

class FFuLabelFrame;
class FFuIOField;
class FFuLabel;
class FFuTable;
class FFuToggleButton;
class FFuPushButton;
class FFuLabelField;
class FFuFileBrowseField;
class FFuScrolledListDialog;
class FFuTabbedWidgetStack;
class FFuSpinBox;
class FFuRadioButton;
class FFuOptionMenu;
class FFuColorDialog;
class FFuColorChooser;
class FFuaQueryBase;
class FuiQueryInputField;
class FuiCurveAppearance;
class FuiCurveAxisDefinition;
class FuiCurveTimeRange;
class FuiCurveDefSheet;
class FuiCurveAnalysisSheet;
class FuiCurveScaleSheet;
class FuiCurveAppearanceSheet;
class FuiCurveInfoSheet;
class FuiCurveFatigueSheet;
class FuiSNCurveSelector;
class FuiScaleShiftWidget;
class FmModelMemberBase;


class FuiCurveDefine : virtual public FFuMultUIComponent,
		       public FFuUAExistenceHandler,
		       public FFuUADataHandler
{
public:
  FuiCurveDefine();
  virtual ~FuiCurveDefine() {}

  enum {X, Y, NDIRS};

  void setCompleteSign(bool complete);
  void setLegend(const std::string& legend);
  void setChannelList(const std::vector<std::string>& channels);

  FmModelMemberBase* getSelectedFunction() const;
  FuiCurveAxisDefinition* getAxisUI(int dir) const;
  const char* getCurrentTabName() const;

  // Setting of outside callbacks
  void setAutoLegendCB(const FFaDynCB1<bool>& dynCB);
  void setProvideChannelListCB(const FFaDynCB1<const std::string&>& dynCB);
  void setReloadCurveCB(const FFaDynCB0& dynCB);

  void setGetCurveStatisticsCB(const FFaDynCB4<bool,bool,double,double>& cb);
  void setApplyVerticalMarkersCB(const FFaDynCB2<double,double>& cb);
  void setRemoveVerticalMarkersCB(const FFaDynCB0& cb);

  void setGetDamageFromCurveCB(const FFaDynCB4<bool,bool,double,double>& cb);

  void setCurveStatistics(double rms, double avg, double stdDev,
			  double integral, double min, double max);
  void setDamageFromCurve(double damage, double interval);

protected:
  void initWidgets();
  virtual void placeWidgets(int width, int height);

private:
  // Inside callback functions
  void onLegendButtonToggled(bool toggle);
  void onFieldValueChanged(char* dummy = 0);
  void onValuesChanged();

  // From FFuMultUIComponent
  virtual void onPoppedDownToMem();
  virtual void onPoppedUpFromMem();

  // From datahandler
  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);
  virtual void buildDynamicWidgets(const FFuaUIValues* values);

protected:
  FFuLabelField* legendField;
  FFuToggleButton* legendButton;

  FFuTabbedWidgetStack* tabStack;
  FuiCurveDefSheet* curveDefSheet;
  FuiCurveAnalysisSheet* curveAnalysisSheet;
  FuiCurveScaleSheet* curveScaleSheet;
  FuiCurveAppearanceSheet* appearanceSheet;
  FuiCurveInfoSheet* infoSheet;
  FuiCurveFatigueSheet* fatigueSheet;

private:
  int selectedTab;

  FFaDynCB1<bool> autoLegendCB;
};
/////////////////////////////////////////////////////////////////////////////


class FuaCurveDefineValues : public FFuaUIValues
{
public:
  FuaCurveDefineValues() {
    this->autoLegend = false;
    this->axesComplete = false;
    this->manualReload = false;
    this->autoExport = false;
    this->selectedTabIdx = 0;

    this->inputMode = 0;
    this->isMultiChannelFile = false;
    this->channel = "Not set";

    this->timeRange = {0.0, 0.0};

    this->curveType = 0;
    this->curveWidth = 0;
    this->curveSymbol = 0;
    this->symbolSize = 0;
    this->numSymbols = 10;

    this->scaleX = 1.0;
    this->offsetX = 0.0;
    this->zeroAdjustX = false;
    this->scaleY = 1.0;
    this->offsetY = 0.0;
    this->zeroAdjustY = false;

    this->analysis = false;
    this->dftDomain = {0.0, 1.0};
    this->dftFeasable = false;
    this->dftEntireDomain = false;
    this->dftRemoveComp = false;
    this->dftResample = false;
    this->dftResampleRate = 0.0;
    this->fatigueDomain = {0.0, 1.0};
    this->fatigueEntireDomain = true;
    this->fatigueGateValue = 1.0;
    this->fatigueFeasable = 0;
    this->fatigueSNCurve = 0;
    this->fatigueSNStd = 0;
    this->fatigueLifeUnit = 0;

    this->firstTriad = this->secondTriad = NULL;
    this->functionRef = NULL;
    this->functionQuery = this->curveQuery = NULL;
  }
  virtual ~FuaCurveDefineValues() {}

  std::string legend;
  bool autoLegend;
  bool axesComplete;
  bool manualReload;
  bool autoExport;
  int selectedTabIdx;

  int inputMode;
  std::string expression;
  std::vector<bool> activeComps;
  std::string modelFilePath;
  std::string filePath;
  bool isMultiChannelFile;
  std::string channel;

  std::pair<double,double> timeRange;
  std::string timeOper;
  std::string spaceOper;

  int curveType;
  int curveWidth;
  int curveSymbol;
  int symbolSize;
  int numSymbols;
  FFuColor color;

  double scaleX;
  double offsetX;
  bool zeroAdjustX;
  double scaleY;
  double offsetY;
  bool zeroAdjustY;

  char analysis;
  std::pair<double,double> dftDomain;
  bool dftFeasable;
  bool dftEntireDomain;
  bool dftRemoveComp;
  bool dftResample;
  double dftResampleRate;
  std::pair<double,double> fatigueDomain;
  bool fatigueEntireDomain;
  double fatigueGateValue;
  char fatigueFeasable;
  int fatigueSNCurve;
  int fatigueSNStd;
  int fatigueLifeUnit;

  FmModelMemberBase* firstTriad;
  FmModelMemberBase* secondTriad;

  FmModelMemberBase* functionRef;
  FFuaQueryBase*     functionQuery;
  FuaFunctionDomain  functionDomain;

  std::vector<FmModelMemberBase*> curveComps;
  FFuaQueryBase*     curveQuery;
};
/////////////////////////////////////////////////////////////////////////////


class FuiCurveDefSheet : virtual public FFuMultUIComponent
{
public:
  FuiCurveDefSheet();
  virtual ~FuiCurveDefSheet() {}

  enum {
    TEMPORAL_RESULT = 0,
    SPATIAL_RESULT = 1,
    COMB_CURVE = 3,
    EXT_CURVE = 4,
    INT_FUNCTION = 5,
    PREVIEW_FUNCTION = 6
  };

  void setDataChangedCB(const FFaDynCB0& dynCB);
  void setProvideChannelsCB(const FFaDynCB1<const std::string&>& dynCB);
  void setReloadCurveCB(const FFaDynCB0& dynCB);

  void setChannelList(const std::vector<std::string>& channels);
  void setCompleteSign(bool complete, bool preview = false);

  void setUIValues(const FuaCurveDefineValues* values);
  void getUIValues(FuaCurveDefineValues* values) const;
  void buildDynamicWidgets(const FuaCurveDefineValues* values);

  FmModelMemberBase* getSelectedFunction() const;
  FuiCurveAxisDefinition* getAxisUI(int dir) const;

protected:
  void initWidgets();
  virtual void placeWidgets(int width, int height);

  virtual void setNoComps(unsigned int nc) = 0;

private:
  virtual void onPoppedDownToMem();

  void onInputModeToggled(int, bool);
  void onFileSelected(const std::string&, int);
  void onChannelBtnClicked();
  void onChannelSelectOK(int index);
  void onChannelSelectApply(int index);
  void onChannelSelectCancel(int);
  void onReloadCurveClicked();
  void onExpressionChanged(char*);
  void onFieldValueChanged(double);
  void onRefSelected(int);
  void onButtonToggled(bool);

  void setDefineMode(int inputMode);

protected:
  FFuLabelFrame* sourceFrame;
  FFuRadioButton* rdbResultPlotRadio;
  FFuRadioButton* combCurvePlotRadio;
  FFuRadioButton* extCurvePlotRadio;
  FFuRadioButton* intFunctionPlotRadio;
  FFuLogicButtonGroup inputGroup;

  std::array<FFuLabelFrame*,FuiCurveDefine::NDIRS> frames;
  std::array<FuiCurveAxisDefinition*,FuiCurveDefine::NDIRS> axes;
  FFuLabelFrame* definitionFrame;
  FFuLabelField* exprField;
  FFuTable* compTable;
  std::vector<FuiQueryInputField*> curveComps;
  FFuFileBrowseField* fileBrowseField;
  FFuPushButton* reloadCurveButton;
  FFuLabelField* channelField;
  FFuPushButton* channelBtn;
  FFuScrolledListDialog* channelSelectUI;
  FFuLabel* functionLabel;
  FuiQueryInputField* functionMenu;
  FFuLabelField* startXField;
  FFuLabelField* stopXField;
  FFuLabelField* incXField;
  FFuToggleButton* useSmartPointsBtn;

  FuiCurveTimeRange* timeRange;
  FFuLabel*          spaceObj;
  FFuOptionMenu*     spaceOper;

  FFuLabel* completeLabel;
  FFuToggleButton* autoExportToggle;

private:
  FFaDynCB0 dataChangedCB;
  FFaDynCB0 reloadCurveCB;
  FFaDynCB1<const std::string&> channelBtnClickedCB;

  bool isSpatial;
  bool isCompCurve;
  char isFuncPreview;
};
/////////////////////////////////////////////////////////////////////////////


class FuiCurveAppearanceSheet : virtual public FFuMultUIComponent
{
public:
  FuiCurveAppearanceSheet();
  virtual ~FuiCurveAppearanceSheet() {}

  void setDataChangedCB(const FFaDynCB0& aCB) { this->dataChangedCB = aCB; }

  void setUIValues(const FuaCurveDefineValues* values);
  void getUIValues(FuaCurveDefineValues* values) const;

protected:
  void initWidgets();
  virtual void placeWidgets(int width, int height);

private:
  virtual void onPoppedDownToMem();

  void onIntValueChanged(int);
  void onAllSymbolsToggled(bool);

  void onColorDialogOk(const FFuColor&);
  void onColorDialogCancel(const FFuColor&);
  void onColorChanged(const FFuColor&);

protected:
  FFuLabelFrame* genAppearanceFrame;
  FFuLabelFrame* symbolFrame;

  FFuLabel*      curveTypeLabel;
  FFuOptionMenu* curveTypeMenu;

  FFuLabel*   curveWidthLabel;
  FFuSpinBox* curveWidthBox;

  FFuLabel*        colorLabel;
  FFuColorChooser* colorChooser;
  FFuColorDialog*  colorDialog;

  FFuLabel*      curveSymbolLabel;
  FFuOptionMenu* curveSymbolMenu;

  FFuLabel*   symbolSizeLabel;
  FFuSpinBox* symbolSizeBox;

  FFuLabel*        numSymbolsLabel;
  FFuSpinBox*      numSymbolsBox;
  FFuToggleButton* allSymbolsButton;

private:
  FFaDynCB0 dataChangedCB;
};
/////////////////////////////////////////////////////////////////////////////


class FuiCurveAnalysisSheet : virtual public FFuMultUIComponent
{
public:
  FuiCurveAnalysisSheet() {}
  virtual ~FuiCurveAnalysisSheet() {}

  void setDataChangedCB(const FFaDynCB0& aCB) { this->dataChangedCB = aCB; }

  void setUIValues(const FuaCurveDefineValues* values);
  void getUIValues(FuaCurveDefineValues* values) const;

protected:
  void initWidgets();
  virtual void placeWidgets(int width, int height);

private:
  void onDftToggled(bool toggle);
  void onDiffToggled(bool toggle);
  void onIntToggled(bool toggle);
  void onButtonToggled(bool toggle);
  void onFieldValueChanged(double value);

protected:
  FFuLabelFrame* dftFrame;
  FFuToggleButton* doDftBtn;
  FFuToggleButton* entireDomainBtn;
  FFuToggleButton* removeCompBtn;
  FFuToggleButton* resampleBtn;
  FFuLabelFrame* dftDomainFrame;
  FFuLabelField* startField;
  FFuLabelField* endField;
  FFuIOField* resampleRateField;

  FFuToggleButton* diffBtn;
  FFuToggleButton* intBtn;

public: // short-cut to FuiCurveFatigueSheet::doRainflowBtn
  FFuToggleButton* rainflowBtn;

private:
  FFaDynCB0 dataChangedCB;
};
/////////////////////////////////////////////////////////////////////////////


class FuiCurveInfoSheet : virtual public FFuMultUIComponent
{
public:
  FuiCurveInfoSheet() { probablyHasMarkers = false; }
  virtual ~FuiCurveInfoSheet() {}

  void setGetCurveStatisticsCB(const FFaDynCB4<bool,bool,double,double>& aCB)
  { this->getCurveStatisticsCB = aCB; }

  void setApplyVerticalMarkersCB(const FFaDynCB2<double,double>& aCB)
  { this->applyVerticalMarkersCB = aCB; }

  void setRemoveVerticalMarkersCB(const FFaDynCB0& aCB)
  { this->removeVerticalMarkersCB = aCB; }

  void setCurveStatistics(double rms, double avg, double stdDev,
			  double integral, double min, double max);

  void setUIValues(const FuaCurveDefineValues* values);

protected:
  void initWidgets();
  virtual void placeWidgets(int width, int height);

private:
  void onBtnClicked();
  void onEntireDomainToggled(bool toggle);

protected:
  FFuLabelFrame* domainFrame;
  FFuPushButton* calculateBtn;
  FFuToggleButton* useScaleShiftBtn;
  FFuLabelField* startField;
  FFuLabelField* stopField;
  FFuLabelField* rmsField;
  FFuLabelField* avgField;
  FFuLabelField* stdDevField;
  FFuLabelField* integralField;
  FFuLabelField* maxField;
  FFuLabelField* minField;
  FFuToggleButton* entireDomainBtn;

private:
  FFaDynCB4<bool,bool,double,double> getCurveStatisticsCB;
  FFaDynCB2<double,double> applyVerticalMarkersCB;
  FFaDynCB0 removeVerticalMarkersCB;

  bool probablyHasMarkers;
};
/////////////////////////////////////////////////////////////////////////////


class FuiCurveScaleSheet : virtual public FFuMultUIComponent
{
public:
  FuiCurveScaleSheet() {}
  virtual ~FuiCurveScaleSheet() {}

  void setDataChangedCB(const FFaDynCB0& aCB) { this->dataChangedCB = aCB; }

  void setUIValues(const FuaCurveDefineValues* values);
  void getUIValues(FuaCurveDefineValues* values) const;

protected:
  void initWidgets();

private:
  void invokeDataChangedCB() { this->dataChangedCB.invoke(); }

protected:
  FuiScaleShiftWidget* XScale;
  FuiScaleShiftWidget* YScale;

private:
  FFaDynCB0 dataChangedCB;
};
/////////////////////////////////////////////////////////////////////////////


class FuiCurveFatigueSheet : virtual public FFuMultUIComponent
{
public:
  FuiCurveFatigueSheet() { myInterval = 0.0; delayDamage=showCalcBtn = false; }
  virtual ~FuiCurveFatigueSheet() {}

  enum {REPEATS, HOURS, DAYS};

  void setDataChangedCB(const FFaDynCB0& aCB);

  void setCalculateCurveDamageCB(const FFaDynCB4<bool,bool,double,double>& aCB)
  { this->calculateCurveDamageCB = aCB; }

  void setCurveDamage(double damage, double interval);

  void setUIValues(const FuaCurveDefineValues* values, bool recalculateDamage);
  void getUIValues(FuaCurveDefineValues* values) const;
  void buildDynamicWidgets(const FuaCurveDefineValues* values);

  void onTabSelected(FFuComponentBase* sheet);

protected:
  void initWidgets();
  virtual void placeWidgets(int width, int height);

private:
  void onButtonClicked() { this->calculateCurveDamage(true); }
  void onButtonToggled(bool toggle);
  void onFieldValueChanged(double value);
  void onUnitValueChanged(int value);

  void calculateCurveDamage(bool eventWeighted = false);
  void recalculateLife(int unitType);

protected:
  FuiSNCurveSelector* snSelector;

  FFuLabelField* gateValueField;

  FFuLabelFrame* resultFrame;
  FFuLabelField* damageField;
  FFuLabelField* lifeField;
  FFuPushButton* calculateBtn;

  FFuLabel*      unitLabel;
  FFuOptionMenu* unitTypeMenu;

  FFuLabelFrame*   domainFrame;
  FFuLabelField*   startField;
  FFuLabelField*   stopField;
  FFuToggleButton* entireDomainBtn;

public: // because FuiCurveAnalysisSheet needs to access this button too
  FFuToggleButton* doRainflowBtn;

private:
  FFaDynCB0                          dataChangedCB;
  FFaDynCB4<bool,bool,double,double> calculateCurveDamageCB;

  double myInterval;  // length of time interval to calculate damage over
  bool   delayDamage; // delay damage calculations until Fatigue tab is selected
  bool   showCalcBtn; // display button of explicit weighted damage calculation
};

#endif
