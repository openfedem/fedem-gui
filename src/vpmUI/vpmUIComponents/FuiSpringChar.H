// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_SPRING_CHAR_H
#define FUI_SPRING_CHAR_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <array>

class FFuLabel;
class FFuIOField;
class FFuToggleButton;
class FFuaQueryBase;
class FuiQueryInputField;
class FuiQueryInputFieldValues;
class FmModelMemberBase;


struct FuiSpringCharValues
{
  bool isTranslationalSpring = true;
  double constantStiffness = 0.0;
  FmModelMemberBase* springFunction = NULL;
  FFuaQueryBase* springFunctionQuery = NULL;
  FFaDynCB1<FuiQueryInputFieldValues&> springFunctionFieldButtonCB;

  bool   useDeflectionMax = false;
  bool   useDeflectionMin = false;
  bool   useForceMax = false;
  bool   useForceMin = false;
  double deflectionMax = 0.0;
  double deflectionMin = 0.0;
  double forceMax = 0.0;
  double forceMin = 0.0;

  bool   useYieldForceMax = false;
  bool   useYieldForceMin = false;
  double constantYieldForceMax = 0.0;
  double constantYieldForceMin = 0.0;
  FmModelMemberBase* yieldForceMaxEngine = NULL;
  FmModelMemberBase* yieldForceMinEngine = NULL;
  FFuaQueryBase* yieldForceEngineQuery = NULL;
  FFaDynCB1<FuiQueryInputFieldValues&> yieldForceFieldMaxButtonCB;
  FFaDynCB1<FuiQueryInputFieldValues&> yieldForceFieldMinButtonCB;

  bool   useYieldDeflectionMax = false;
  double yieldDeflectionMax = 0.0;
};


class FuiSpringChar : public virtual FFuComponentBase
{
public:
  FuiSpringChar();

  virtual void setSensitivity(bool isSensitive);

  // Callbacks :

  void setChangedCB(const FFaDynCB0& aDynCB);
  void setValuesChangedCB(const FFaDynCB1<FuiSpringCharValues&>& aDynCB);

  // Setting values:

  void setValues(const FuiSpringCharValues& values);
  void setCBs(const FuiSpringCharValues& values);

  // Getting values:

  void getValues(FuiSpringCharValues& values);

protected:
  void initWidgets();

  enum {
    MAX_DEFL, MIN_DEFL,
    MAX_FORCE, MIN_FORCE,
    MAX_YIELD_FORCE, MIN_YIELD_FORCE,
    MAX_YIELD_DEFL,
    NUM_FIELDS
  };

  // Shadow pointers from subclass.
  // Subclass is responsible to set them up:

  FFuLabel*           mySpringFunctionDescription;
  FuiQueryInputField* mySpringFunctionField;
  FuiQueryInputField* myYieldForceMaxEngineField;
  FuiQueryInputField* myYieldForceMinEngineField;

  std::array<FFuToggleButton*,NUM_FIELDS> myToggles;
  std::array<FFuIOField*,NUM_FIELDS>      myFields;

private:

  // Internal methods for rerouting of Callbacks

  void onBoolChanged(bool);
  void onBoolChanged(int,bool);
  void onQIFieldChanged(FuiQueryInputField*);
  void onValuesChanged(double = 0.0);

  // Callback Objects

  FFaDynCB1<FuiSpringCharValues&> myValuesChangedCB;
  FFaDynCB0                       myChangedCB;

  // Internal Vars

  bool IAmSensitive;
};

#endif
