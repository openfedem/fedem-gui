// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_TRIAD_DOF_H
#define FUI_TRIAD_DOF_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiSimpleLoad.H"

class FuiMotionType;
class FuiVariableType;
class FFuLabelField;
class FFuToggleButton;


struct FuiTriadDOFValues
{
  int myMotionType = -1;
  bool freqDomain = false;
  double myInitVel = 0.0;
  FuiSimpleLoadValues myLoadVals;
};


class FuiTriadDOF : public virtual FFuComponentBase
{
public:
  FuiTriadDOF();

  void setChangedCB(const FFaDynCB0& aDynCB);

  void getValues(FuiTriadDOFValues& values) const;
  void setValues(const FuiTriadDOFValues& values,
                 bool canSwitchMotionType = true,
                 bool isSensitive = true);

  FuiMotionType*   motionType;
  FuiVariableType* variableType;
  FuiSimpleLoad*   loadField;
  FFuLabelField*   initialVel;
  FFuToggleButton* freqToggle;

  enum {
    FREE = 0,
    FIXED = 1,
    PRESCRIBED = 2,
    FREE_DYNAMICS = 3,
    PRESCRIBED_DISP = 6,
    PRESCRIBED_VEL = 7,
    PRESCRIBED_ACC = 8
  };

protected:
  void initWidgets();
};

#endif
