// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_CURVE_DEFINE_H
#define FUI_CURVE_DEFINE_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "FFuLib/FFuColor.H"
#include "FFuLib/FFuLogicButtonGroup.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "vpmUI/vpmUIComponents/FuiFunctionDomain.H"
#include <string>
#include <vector>
#include <array>

class FFuFrame;
class FFuLabelFrame;
class FFuIOField;
class FFuLabel;
class FFuToggleButton;
class FFuPushButton;
class FFuLabelField;
class FFuFileBrowseField;
class FFuScrolledListDialog;
class FFuTabbedWidgetStack;
class FFuSpinBox;
class FFuRadioButton;
class FFuOptionMenu;
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


class FuiCurveDefine : virtual public FFuComponentBase,
		       public FFuUAExistenceHandler,
		       public FFuUADataHandler
{
public:
  FuiCurveDefine();

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
  void rememberSelectedTab(bool show);

private:
  // Inside callback functions
  void onLegendButtonToggled(bool toggle);
  void onFieldValueChanged(char*) { this->updateDBValues(false); }
  void onValuesChanged() { this->updateDBValues(true); }

  // From datahandler
  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);
  virtual void buildDynamicWidgets(const FFuaUIValues* values);

protected:
  FFuLabelField*        legendField;
  FFuToggleButton*      legendButton;
  FFuTabbedWidgetStack* tabStack;

  FuiCurveDefSheet*        curveDefSheet;
  FuiCurveAnalysisSheet*   curveAnalysisSheet;
  FuiCurveScaleSheet*      curveScaleSheet;
  FuiCurveAppearanceSheet* appearanceSheet;
  FuiCurveInfoSheet*       infoSheet;
  FuiCurveFatigueSheet*    fatigueSheet;

private:
  int selectedTab;

  FFaDynCB1<bool> autoLegendCB;
};
/////////////////////////////////////////////////////////////////////////////


class FuaCurveDefineValues : public FFuaUIValues
{
public:
  std::string legend;
  bool autoLegend = false;
  bool axesComplete = false;
  bool manualReload = false;
  bool autoExport = false;
  int selectedTabIdx = 0;

  int inputMode = 0;
  std::string expression;
  std::vector<bool> activeComps;
  std::string modelFilePath;
  std::string filePath;
  bool isMultiChannelFile = false;
  std::string channel = "Not set";

  std::pair<double,double> timeRange = { 0.0, 0.0 };
  std::string timeOper;
  std::string spaceOper;

  int curveType   = 0;
  int curveWidth  = 0;
  int curveSymbol = 0;
  int symbolSize  = 0;
  int numSymbols  = 10;
  FFuColor color;

  double scaleX    = 1.0;
  double offsetX   = 0.0;
  bool zeroAdjustX = false;
  double scaleY    = 1.0;
  double offsetY   = 0.0;
  bool zeroAdjustY = false;
 
  char analysis = false;
  std::pair<double,double> dftDomain = { 0.0, 1.0 };
  bool dftFeasable = false;
  bool dftEntireDomain = false;
  bool dftRemoveComp = false;
  bool dftResample = false;
  double dftResampleRate = 0.0;
  std::pair<double,double> fatigueDomain = { 0.0, 1.0 };
  bool fatigueEntireDomain = false;
  double fatigueGateValue = 1.0;
  char fatigueFeasable = 0;
  int fatigueSNCurve = 0;
  int fatigueSNStd = 0;
  int fatigueLifeUnit = 0;

  FmModelMemberBase* firstTriad = NULL;
  FmModelMemberBase* secondTriad = NULL;

  FmModelMemberBase* functionRef = NULL;
  FFuaQueryBase*     functionQuery = NULL;
  FuaFunctionDomain  functionDomain;

  std::vector<FmModelMemberBase*> curveComps;
  FFuaQueryBase*     curveQuery = NULL;
};
/////////////////////////////////////////////////////////////////////////////


class FuiCurveDomain : virtual public FFuComponentBase
{
public:
  FuiCurveDomain() { entireBtn = NULL; startField = endField = NULL; }

  void initWidgets(bool internalToggleCB = true);
  void initWidgets(const FFaDynCB1<double>& doubleCB,
                   const FFaDynCB1<bool>& boolCB);
  void setValues(const std::pair<double,double>& domain,
                 bool entire, bool isSensitive = true);
  bool getValues(std::pair<double,double>& domain) const;

private:
  void onButtonToggled(bool toggle);

protected:
  FFuLabelField*   startField;
  FFuLabelField*   endField;
  FFuToggleButton* entireBtn;
};
/////////////////////////////////////////////////////////////////////////////


class FuiCurveDefSheet : virtual public FFuComponentBase
{
public:
  FuiCurveDefSheet();

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
  FuiCurveAxisDefinition* getAxisUI(int dir) const { return axes[dir]; }

protected:
  void initWidgets();

  virtual void setNoComps(unsigned int nc) = 0;

private:
  void onInputModeToggled(int, bool);
  void onFileSelected(const std::string&, int) { dataChangedCB.invoke(); }
  void onChannelBtnClicked();
  void onChannelSelectOK(int index);
  void onChannelSelectApply(int index);
  void onChannelSelectCancel(int);
  void onReloadCurveClicked() { reloadCurveCB.invoke(); }
  void onExpressionChanged(char*) { dataChangedCB.invoke(); }
  void onFieldValueChanged(double) { dataChangedCB.invoke(); }
  void onRefSelected(int) { dataChangedCB.invoke(); }
  void onButtonToggled(bool) { dataChangedCB.invoke(); }

  void setDefineMode(int inputMode);

protected:
  FFuRadioButton* rdbResultPlotRadio;
  FFuRadioButton* combCurvePlotRadio;
  FFuRadioButton* extCurvePlotRadio;
  FFuRadioButton* intFunctionPlotRadio;
  FFuLogicButtonGroup inputGroup;

  FFuComponentBase* rdbAxes;
  std::array<FuiCurveAxisDefinition*,FuiCurveDefine::NDIRS> axes;
  FFuLabelFrame* xAxisFrame;
  FFuOptionMenu* spaceOper;
  FuiCurveTimeRange* timeRange;

  FFuLabelFrame* combFrame;
  FFuLabelField* exprField;
  std::vector<FuiQueryInputField*> curveComps;

  FFuLabelFrame* fileFrame;
  FFuFileBrowseField* fileBrowseField;
  FFuLabelField* channelField;
  FFuPushButton* channelBtn;
  FFuScrolledListDialog* channelSelectUI;

  FFuLabelFrame* functionFrame;
  FuiQueryInputField* functionMenu;
  FFuLabelField* startXField;
  FFuLabelField* stopXField;
  FFuLabelField* incXField;
  FFuToggleButton* useSmartPointsBtn;

  FFuLabel* completeLabel;
  FFuPushButton* reloadCurveButton;
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


class FuiCurveAppearanceSheet : virtual public FFuComponentBase
{
public:
  FuiCurveAppearanceSheet();

  void setDataChangedCB(const FFaDynCB0& aCB) { dataChangedCB = aCB; }

  void setUIValues(const FuaCurveDefineValues* values);
  void getUIValues(FuaCurveDefineValues* values) const;

protected:
  void initWidgets();

private:
  void onIntValueChanged(int) { dataChangedCB.invoke(); }
  void onSymbolsToggled(bool) { dataChangedCB.invoke(); }
  void onColorChanged(const FFuColor&) { dataChangedCB.invoke(); }

protected:
  FFuOptionMenu*   curveTypeMenu;
  FFuSpinBox*      curveWidthBox;
  FFuColorChooser* colorChooser;

  FFuOptionMenu*   curveSymbolMenu;
  FFuSpinBox*      symbolSizeBox;
  FFuSpinBox*      numSymbolsBox;
  FFuToggleButton* allSymbolsButton;

private:
  FFaDynCB0 dataChangedCB;
};
/////////////////////////////////////////////////////////////////////////////


class FuiCurveAnalysisSheet : virtual public FFuComponentBase
{
public:
  void setDataChangedCB(const FFaDynCB0& aCB) { dataChangedCB = aCB; }

  void setUIValues(const FuaCurveDefineValues* values);
  void getUIValues(FuaCurveDefineValues* values) const;

protected:
  void initWidgets();

private:
  void onDftToggled(bool toggle);
  void onDiffToggled(bool toggle);
  void onIntToggled(bool toggle);
  void onButtonToggled(bool) { dataChangedCB.invoke(); }
  void onFieldValueChanged(double) { dataChangedCB.invoke(); }

protected:
  FFuToggleButton* doDftBtn;
  FFuToggleButton* removeCompBtn;
  FFuToggleButton* resampleBtn;
  FFuIOField* resampleRateField;
  FuiCurveDomain* curveDomain;

  FFuToggleButton* diffBtn;
  FFuToggleButton* intBtn;

public: // short-cut to FuiCurveFatigueSheet::doRainflowBtn
  FFuToggleButton* rainflowBtn;

private:
  FFaDynCB0 dataChangedCB;
};
/////////////////////////////////////////////////////////////////////////////


class FuiCurveInfoSheet : virtual public FFuComponentBase
{
public:
  FuiCurveInfoSheet() { probablyHasMarkers = false; }

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

private:
  void onBtnClicked();

protected:
  FFuLabelFrame* domainFrame;
  FFuPushButton* calculateBtn;
  FFuToggleButton* useScaleShiftBtn;
  FFuLabelField* rmsField;
  FFuLabelField* avgField;
  FFuLabelField* stdDevField;
  FFuLabelField* integralField;
  FFuLabelField* maxField;
  FFuLabelField* minField;
  FuiCurveDomain* xDomain;

private:
  FFaDynCB4<bool,bool,double,double> getCurveStatisticsCB;
  FFaDynCB2<double,double> applyVerticalMarkersCB;
  FFaDynCB0 removeVerticalMarkersCB;

  bool probablyHasMarkers;
};
/////////////////////////////////////////////////////////////////////////////


class FuiCurveScaleSheet : virtual public FFuComponentBase
{
public:
  void setDataChangedCB(const FFaDynCB0& aCB) { dataChangedCB = aCB; }

  void setUIValues(const FuaCurveDefineValues* values);
  void getUIValues(FuaCurveDefineValues* values) const;

protected:
  void initWidgets();

  FuiScaleShiftWidget* XScale;
  FuiScaleShiftWidget* YScale;

private:
  FFaDynCB0 dataChangedCB;
};
/////////////////////////////////////////////////////////////////////////////


class FuiCurveFatigueSheet : virtual public FFuComponentBase
{
public:
  FuiCurveFatigueSheet() { myInterval = 0.0; delayDamage=showCalcBtn = false; }

  enum {REPEATS, HOURS, DAYS};

  void setDataChangedCB(const FFaDynCB0& aCB);
  void setCalculateCurveDamageCB(const FFaDynCB4<bool,bool,double,double>& aCB);

  void setCurveDamage(double damage, double interval);

  void setUIValues(const FuaCurveDefineValues* values, bool recalculateDamage);
  void getUIValues(FuaCurveDefineValues* values) const;
  void buildDynamicWidgets(const FuaCurveDefineValues* values);

  void onTabSelected(FFuComponentBase* sheet);

protected:
  void initWidgets();

private:
  void onButtonClicked() { this->calculateCurveDamage(true); }
  void onButtonToggled(bool) { dataChangedCB.invoke(); }
  void onFieldValueChanged(double) { dataChangedCB.invoke(); }

  void calculateCurveDamage(bool eventWeighted = false);
  void recalculateLife(int unitType);

protected:
  FuiSNCurveSelector* snSelector;
  FFuLabelField* gateValueField;

  FFuLabelField* damageField;
  FFuLabelField* lifeField;
  FFuOptionMenu* unitTypeMenu;
  FFuPushButton* calculateBtn;

  FuiCurveDomain* timeDomain;

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
