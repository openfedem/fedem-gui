// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_GRAPH_DEFINE_H
#define FUI_GRAPH_DEFINE_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include <string>
#include <array>

class FFuSpinBox;
class FFuOptionMenu;
class FFuToggleButton;
class FFuLabelField;


class FuiGraphDefine : virtual public FFuComponentBase,
                       public FFuUAExistenceHandler,
                       public FFuUADataHandler
{
public:
  FuiGraphDefine();

protected:
  void initWidgets();

private:
  // Inside callback functions
  void onFieldValueChanged(char*)  { this->updateDBValues(); }
  void onFieldValueChanged(double) { this->updateDBValues(); }
  void onFieldValueChanged(int)    { this->updateDBValues(); }
  void onButtonToggled(bool)       { this->updateDBValues(); }

  // from datahandler
  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);

protected:
  FFuLabelField* titleField;
  FFuLabelField* subtitleField;
  FFuLabelField* xAxisField;
  FFuLabelField* yAxisField;

  std::array<FFuSpinBox*,3> fontSizeBox;

  FFuOptionMenu* gridMenu;

  FFuToggleButton* legendButton;
  FFuToggleButton* autoscaleButton;

  FFuLabelField*   tminField;
  FFuLabelField*   tmaxField;
  FFuToggleButton* timeButton;
};

/////////////////////////////////////////////////////////////////////////////

class FuaGraphDefineValues : public FFuaUIValues
{
public:
  std::string title;
  std::string subtitle;
  std::string xAxisLabel;
  std::string yAxisLabel;
  std::array<int,3> fontSize = { 10, 8, 8 };

  int  gridType = -1;
  bool showLegend = false;
  bool autoScaleOnLoadCurve = false;
  char useTimeInterval = false;
  double tmin = 0.0;
  double tmax = 0.0;
};

/////////////////////////////////////////////////////////////////////////////

#endif
