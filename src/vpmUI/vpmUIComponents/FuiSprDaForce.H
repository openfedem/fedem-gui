// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_SPR_DA_FORCE_H
#define FUI_SPR_DA_FORCE_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <string>

class FFuLabel;
class FFuToggleButton;
class FFuaQueryBase;
class FuiQueryInputField;
class FuiQueryInputFieldValues;
class FmModelMemberBase;


struct FuiSprDaForceValues
{
  bool isDefDamper   = false;
  char showDefDamper = false; // -1: visible but insensitive, 0: invisible, 1: sensitive

  FFuaQueryBase*     functionQuery = NULL;
  bool               isConstant = true;
  FmModelMemberBase* selectedFunction = NULL;
  std::string        functionUIString;
  double             constFunction = 0.0;
  FFaDynCB1<FuiQueryInputFieldValues&> myFunctionQIFieldButtonCB;

  FFuaQueryBase*     engineQuery = NULL;
  FmModelMemberBase* selectedScaleEngine = NULL;
  std::string        scaleEngUIString;
  double             constScale = 0.0;
  FFaDynCB1<FuiQueryInputFieldValues&> myEngineQIFieldButtonCB;
};


class FuiSprDaForce : public virtual FFuComponentBase
{
public:
  FuiSprDaForce() { IAmSensitive = IAmSpringUI = false; }

  // Setting Up :

  void setAsSpring(bool yesOrNo);

  virtual void setLabel(const char* text) = 0;
  virtual void setSensitivity(bool isSensitive);

  // Callbacks :

  void setChangedCB(const FFaDynCB0& aDynCB);
  void setValuesChangedCB(const FFaDynCB1<FuiSprDaForceValues&>& aDynCB);

  // Setting and getting values:

  void buildDynamicWidgets(const FuiSprDaForceValues& values);
  void setCBs(const FuiSprDaForceValues& values);
  void setValues(const FuiSprDaForceValues& values);
  void getValues(FuiSprDaForceValues& values);

protected:
  void initWidgets();

  // Shadow pointers from subclass.
  // Subclass is responsible to set them up:

  FFuLabel*           myFunctionLabel;
  FuiQueryInputField* myFunctionField;
  FuiQueryInputField* myScaleField;
  FFuToggleButton*    myDmpToggle;

private:

  // Internal methods for rerouting of Callbacks

  void onQIFieldChanged(FuiQueryInputField*);
  void onBoolChanged(bool);

  // Callback Objects

  FFaDynCB1<FuiSprDaForceValues&> myValuesChangedCB;
  FFaDynCB0                       myChangedCB;

  // Internal Vars

  bool IAmSensitive;
  bool IAmSpringUI;
};

#endif
