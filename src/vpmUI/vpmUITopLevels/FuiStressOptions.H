// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_STRESS_OPTIONS_H
#define FUI_STRESS_OPTIONS_H

#include "vpmUI/vpmUITopLevels/FuiTopLevelDialog.H"
#include "vpmUI/vpmUIComponents/FuiTimeInterval.H"

class FFuLabelFrame;
class FFuToggleButton;
class FFuFileBrowseField;
class FFuIOField;


class FuiStressOptions : public FuiTopLevelDialog
{
  Fmd_HEADER_INIT();

public:
  FuiStressOptions();

  // Static create method to use instead of constructor
  // Implementation in GUIlib-dependent implementation file
  static FuiStressOptions* create(int xpos = 100, int ypos  = 100,
				  int width= 100, int height= 100,
				  const char* title= "Stress Recovery Setup",
				  const char* name = "FuiStressOptions");

  void setTimeUIValues(const FuaTimeIntervalValues* timeValues);
  void setResetTimeCB(const FFaDynCB0& dynCB) { resetTimeCB = dynCB; }

protected:
  virtual void initWidgets();

private:
  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);

  void setVTFLabel(int);
  void onAutoVTFToggled(bool);
  void onAutoVTFFileChanged(const std::string&, int);

protected:
  FuiTimeInterval* time;

  FFuLabelFrame* outputFrame;
  FFuLabelFrame* vtfFrame;

  std::array<FFuToggleButton*,6> strainToggle;
  std::array<FFuToggleButton*,6> stressToggle;

  FFuToggleButton*    autoVTFToggle;
  FFuFileBrowseField* autoVTFField;

  FFuIOField* minFringeField;
  FFuIOField* maxFringeField;

private:
  FFaDynCB0 resetTimeCB;
  bool      showVTFfield;
};


class FuaStressOptionsValues : public FFuaUIValues
{
public:
  FuaTimeIntervalValues timeValues;

  bool deformationOutput = false;
  bool stressResOutput = false;
  bool stressOutput[5] = { false, false, false, false, false };
  bool strainOutput[5] = { false, false, false, false, false };

  char autoVTFSwitch = false;
  int  autoVTFFileType = 0;

  std::string autoVTFFileName;
  std::string modelFilePath;

  double vtfFringeMin = 0.0;
  double vtfFringeMax = 0.0;

  bool isSensitive = true;
};

#endif
