// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_THRESHOLD_H
#define FUI_THRESHOLD_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <string>

class FFuToggleButton;
class FFuLabelField;
class FFuOptionMenu;


struct ThresholdValues
{
  bool   active = false;
  double threshold = 0.0;
  int    min = 0;
  int    skip = 0;
  int    severity = 0;
  std::string description;
};


class FuiThreshold : public virtual FFuComponentBase
{
public:
  FuiThreshold();

  void setToggleCB(const FFaDynCB1<bool>& cb);
  void setValueChangedCB(const FFaDynCB1<char*>& cb);
  void setSelectionChangedCB(const FFaDynCB1<int>& cb);

  void setValues(const ThresholdValues& vals);
  void getValues(ThresholdValues& vals) const;

protected:
  void initWidgets();

  FFuToggleButton* myToggle;
  FFuLabelField*   myDescription;
  FFuLabelField*   myValueField;
  FFuLabelField*   myMinField;
  FFuLabelField*   mySkipField;
  FFuOptionMenu*   mySeverityMenu;
};

#endif
