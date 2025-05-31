// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_GAGE_OPTIONS_H
#define FUI_GAGE_OPTIONS_H

#include "vpmUI/vpmUITopLevels/FuiTopLevelDialog.H"
#include "vpmUI/vpmUIComponents/FuiTimeInterval.H"

class FFuIOField;
class FFuToggleButton;
class FFuPushButton;


class FuiGageOptions : public FuiTopLevelDialog
{
  Fmd_HEADER_INIT();

public:
  FuiGageOptions();

  static FuiGageOptions* create(int xpos = 100, int ypos  = 100,
				int width= 100, int height= 100,
				const char* title= "Strain Rosette Recovery Setup",
				const char* name = "FuiGageOptions");

  void setTimeUIValues(const FuaTimeIntervalValues* timeValues);
  void setResetTimeCB(const FFaDynCB0& dynCB) { resetTimeCB = dynCB; }
  void setImportRosetteFileCB(const FFaDynCB0& dynCB);

  virtual void setSensitivity(bool sens);

protected:
  virtual void initWidgets();

private:
  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);

protected:
  FuiTimeInterval* time;

  FFuToggleButton* autoDacExportToggle;
  FFuIOField*      dacSampleRateField;
  FFuToggleButton* rainflowToggle;
  FFuIOField*      binSizeField;
  FFuPushButton*   importRosFileButton;

private:
  FFaDynCB0 resetTimeCB;
  FFaDynCB0 importRosFileCB;
};


class FuaGageOptionsValues : public FFuaUIValues
{
public:
  FuaTimeIntervalValues timeValues;

  bool   autoDacSwitch  = false;
  double dacSampleRate  = 0.0;
  bool   rainflowSwitch = false;
  double binSize        = 0.0;
  bool   isSensitive    = true;
};

#endif
