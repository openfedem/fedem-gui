// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_TIME_INTERVAL_H
#define FUI_TIME_INTERVAL_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuIOField;
class FFuToggleButton;
class FFuPushButton;


struct FuaTimeIntervalValues
{
  double start = 0.0;
  double stop = 0.0;
  double incr = 0.0;
  bool allSteps = false;
  bool dontTouchIncr = false;
};


class FuiTimeInterval : virtual public FFuComponentBase
{
public:
  void setResetCB(const FFaDynCB0& dynCB) { resetCB = dynCB; }

  void setUIValues(const FuaTimeIntervalValues* timeValues);
  void getUIValues(FuaTimeIntervalValues* timeValues);

  virtual void setSensitivity(bool isSensitive);

protected:
  void initWidgets();

  FFuIOField*      startField = NULL;
  FFuIOField*      stopField = NULL;
  FFuIOField*      incrField = NULL;
  FFuToggleButton* allStepsToggle = NULL;
  FFuPushButton*   resetButton = NULL;

private:
  FFaDynCB0 resetCB;
};

#endif
