// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_ANIMATION_DEFINE_H
#define FUI_ANIMATION_DEFINE_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "FFuLib/FFuLogicButtonGroup.H"
#include <string>
#include <vector>
#include <set>

class FuiFringeSheet;
class FuiTimeSheet;
class FuiModesSheet;
class FuiQueryInputField;
class FFuTabbedWidgetStack;
class FFuToggleButton;
class FFuRadioButton;
class FFuPushButton;
class FFuLabel;
class FFuLabelFrame;
class FFuIOField;
class FFuOptionMenu;
class FFuaQueryBase;
class FmModelMemberBase;


class FuiAnimationDefine : virtual public FFuComponentBase,
                           public FFuUAExistenceHandler,
                           public FFuUADataHandler
{
public:
  void setLoadAnimCB(const FFaDynCB0& dynCB) { loadAnimCB = dynCB; }

protected:
  void initWidgets();

private:
  // Inside callback functions
  void onLoadAnimButtonClicked()     { loadAnimCB.invoke(); }
  void onTabSheetChanged()           { this->updateDBValues(); }
  void onButtonToggled(bool)         { this->updateDBValues(); }
  void onRadioGroupToggled(int,bool) { this->updateDBValues(); }

  // From datahandler
  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);

protected:
  FFuRadioButton* timeRadio;
  FFuRadioButton* eigenRadio;
  FFuRadioButton* summaryRadio;
  FFuLogicButtonGroup typeRadioGroup;

  FFuToggleButton* loadFringeToggle;
  FFuToggleButton* loadLineFringeToggle;
  FFuToggleButton* loadDefToggle;

  FFuPushButton* loadAnimButton;

  FFuTabbedWidgetStack* tabStack;
  FuiFringeSheet* fringeSheet;
  FuiTimeSheet* timeSheet;
  FuiModesSheet* modesSheet;

private:
  FFaDynCB0 loadAnimCB;
};
/////////////////////////////////////////////////////////////////////////////

class FuiFringeSheet : virtual public FFuComponentBase
{
public:
  void setDataChangedCB(const FFaDynCB0& dynCB) { dataChangedCB = dynCB; }

  void setUIValues(const FFuaUIValues* values);
  void getUIValues(FFuaUIValues* values);

protected:
  void initWidgets();

private:
  // Inside callback functions
  void onMenuSelected(int)           { dataChangedCB.invoke(); }
  void onButtonToggled(bool)         { dataChangedCB.invoke(); }
  void onRadioGroupToggled(int,bool) { dataChangedCB.invoke(); }
  void onFieldValueChanged(char*)    { dataChangedCB.invoke(); }

protected:
  FFuOptionMenu* resultClassMenu;
  FFuOptionMenu* resultMenu;
  FFuOptionMenu* resultOperMenu;

  FFuRadioButton* setByOperRadio;
  FFuOptionMenu* setOperMenu;
  FFuRadioButton* setByNameRadio;
  FFuOptionMenu* setNameMenu;
  FFuLogicButtonGroup resultSetRadioGroup;

  FFuOptionMenu* averageOperMenu;
  FFuOptionMenu* averageOnMenu;
  FFuIOField* shellAngleField;
  FFuToggleButton* elementToggle;

  FFuLabel* elemGrpOperLabel;
  FFuOptionMenu* elemGrpOperMenu;

  FFuToggleButton* autoExportToggle;

private:
  FFaDynCB0 dataChangedCB;
};
/////////////////////////////////////////////////////////////////////////////

class FuiTimeSheet : virtual public FFuComponentBase
{
public:
  void setDataChangedCB(const FFaDynCB0& dynCB) { dataChangedCB = dynCB; }

  void setUIValues(const FFuaUIValues* values);
  void getUIValues(FFuaUIValues* values);

protected:
  void initWidgets();

private:
  // Inside callback functions
  void onButtonToggled(bool)         { dataChangedCB.invoke(); }
  void onFieldValueChanged(char*)    { dataChangedCB.invoke(); }
  void onRadioGroupToggled(int,bool) { dataChangedCB.invoke(); }

protected:
  FFuRadioButton* completeSimRadio;
  FFuRadioButton* timeIntRadio;
  FFuLogicButtonGroup timeRadioGroup;

  FFuLabel* startLabel;
  FFuLabel* stopLabel;
  FFuIOField* startField;
  FFuIOField* stopField;

  FFuRadioButton* mostFramesRadio;
  FFuRadioButton* leastFramesRadio;
  FFuLogicButtonGroup framesRadioGroup;

private:
  FFaDynCB0 dataChangedCB;
};
/////////////////////////////////////////////////////////////////////////////

class FuiModesSheet : virtual public FFuComponentBase
{
public:
  void setDataChangedCB(const FFaDynCB0& dynCB) { dataChangedCB = dynCB; }

  void setUIValues(const FFuaUIValues* values);
  void getUIValues(FFuaUIValues* values);

protected:
  void initWidgets();

private:
  // Inside callback functions
  void onMenuSelected(int)           { dataChangedCB.invoke(); }
  void onButtonToggled(bool)         { dataChangedCB.invoke(); }
  void onFieldValueChanged(char*)    { dataChangedCB.invoke(); }
  void onRadioGroupToggled(int,bool) { dataChangedCB.invoke(); }

protected:
  FFuOptionMenu* typeMenu;
  FFuLabel* linkLabel;
  FuiQueryInputField* linkMenu;
  FFuLabel* timeLabel;
  FFuOptionMenu* timeMenu;
  FFuOptionMenu* modeMenu;
  FFuIOField* scaleField;
  FFuIOField* framesPrField;

  FFuLabelFrame* lengthFrame;
  FFuRadioButton* timeLengthRadio;
  FFuIOField* timeLengthField;
  FFuRadioButton* nCyclRadio;
  FFuIOField* nCyclField;
  FFuRadioButton* dampedRadio;
  FFuIOField* dampedField;
  FFuLogicButtonGroup lengthRadioGroup;

private:
  FFaDynCB0 dataChangedCB;
};
/////////////////////////////////////////////////////////////////////////////

class FuaAnimationDefineValues : public FFuaUIValues
{
public:
  // COMMON
  bool isModesAnim = false;
  bool isSummaryAnim = false;
  bool loadFringe = false;
  bool loadLineFringe = false;
  bool loadDef = false;

  // FRINGE
  // value
  std::vector<std::string> resultClasses; std::string selResultClass;
  std::set<std::string>    results;       std::string selResult;
  std::vector<std::string> resultOpers;   std::string selResultOper;

  bool resultSetByName = false;
  std::vector<std::string> resultSetOpers; std::string selResultSetOper;
  std::set<std::string>    resultSets;     std::string selResultSet;

  // Averaging
  std::vector<std::string> averOns;   std::string selAverOn;
  std::vector<std::string> averOpers; std::string selAverOper;

  double shellAngle = 0.0;
  bool acrossElemType = false;
  bool acrossMatBorder = false;
  bool acrossPropBorder = false;

  // multi face
  std::vector<std::string> elemGrps;     std::string selElemGrp;
  std::vector<std::string> elemGrpOpers; std::string selElemGrpOper;

  char autoExport = false;

  // TIME
  bool   complSimul = false;
  double start = 0.0;
  double stop = 0.0;
  bool   mostFrames = false;

  // MODES
  int modeTyp = 0;
  FFuaQueryBase*     linkQuery = NULL;
  FmModelMemberBase* selLink = NULL;
  std::vector<double> times; double selTime = 0.0;
  std::vector<int>    modes; int    selMode = 0;

  double modeScale = 0.0;
  int    framesPrCycle = 0;

  bool timeLength = false;  double timeLengthVal = 0.0;
  bool nCycles = false;     int nCyclesVal = 0;
  bool untilDamped = false; int untilDampedVal = 0;
};
/////////////////////////////////////////////////////////////////////////////

#endif
