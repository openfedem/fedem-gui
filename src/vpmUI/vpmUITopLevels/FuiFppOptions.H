// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_FPP_OPTIONS_H
#define FUI_FPP_OPTIONS_H

#include "vpmUI/vpmUITopLevels/FuiTopLevelDialog.H"
#include "vpmUI/vpmUIComponents/FuiTimeInterval.H"

class FFuLabel;
class FFuLabelFrame;
class FFuToggleButton;
class FFuIOField;
class FFuOptionMenu;


class FuiFppOptions : public FuiTopLevelDialog
{
  Fmd_HEADER_INIT();

public:
  FuiFppOptions();

  // Static create method to use instead of constructor
  // Implementation in GUIlib-dependent implementation file
  static FuiFppOptions* create(int xpos = 100, int ypos  = 100,
			       int width= 100, int height= 100,
			       const char* title= "Strain Coat Recovery Summary Setup",
			       const char* name = "FuiFppOptions");

  void setTimeUIValues(const FuaTimeIntervalValues* timeValues);
  void setResetTimeCB(const FFaDynCB0& dynCB) { resetTimeCB = dynCB; }

protected:
  virtual void initWidgets();

private:
  void onAnalysisTypeChanged(int type);
  void onRainflowButtonToggled(bool toggle);

  // From datahandler
  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);

protected:
  FuiTimeInterval* time;

  FFuLabelFrame*   rainflowFrame;
  FFuToggleButton* rainflowButton;

  FFuLabel*   maxElemsLabel;
  FFuIOField* maxElemsField;
  FFuLabel*   biaxGateLabel;
  FFuIOField* biaxGateField;
  FFuLabel*   pvxGateLabel;
  FFuIOField* pvxGateField;

  FFuLabel*      typeLabel;
  FFuOptionMenu* typeMenu;

  FFuLabelFrame* histFrame;
  FFuIOField*    maxField;
  FFuIOField*    minField;
  FFuIOField*    nBinsField;

private:
  FFaDynCB0 resetTimeCB;
  bool showNCodeFields;
};


class FuaFppOptionsValues : public FFuaUIValues
{
public:
  FuaTimeIntervalValues timeValues;

  int    nElemsTogether = 0;
  bool   performRainflow = true;
  int    histType = 0;
  double biaxGate = 0.0;
  double pvxGate = 0.0;
  double max = 0.0;
  double min = 0.0;
  int    nBins = 0;
};

#endif
