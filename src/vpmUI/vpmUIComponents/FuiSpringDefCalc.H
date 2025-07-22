// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_SPRING_DEF_CALC_H
#define FUI_SPRING_DEF_CALC_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFuLib/FFuLogicButtonGroup.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuLabel;
class FFuIOField;
class FFuRadioButton;
class FFuaQueryBase;
class FuiQueryInputField;
class FuiQueryInputFieldValues;
class FmModelMemberBase;


struct FuiSpringDefCalcValues
{
  char useAngularLabels = false;
  bool useSpringLabels = true;

  double variable = 0.0;
  bool   variableIsEditable = false;
  bool   fixedOrFree = false;

  double initLengtOrDeflection = 0.0;
  bool   useAsDeflection = true;

  FFuaQueryBase*     engineQuery = NULL;
  FmModelMemberBase* selectedLengthEngine = NULL;

  FFaDynCB1<FuiQueryInputFieldValues&> myQIFieldButtonCB;

  double getInitDeflection() const
  {
    if (useAsDeflection)
      return initLengtOrDeflection;
    else
      return variable - initLengtOrDeflection;
  }
};


class FuiSpringDefCalc : public virtual FFuComponentBase
{
public:
  FuiSpringDefCalc();

  // Setting Up :

  void setLabels(char isAngle, bool isSpring);

  virtual void setLabel(const char* text) = 0;
  virtual void setSensitivity(bool isSensitive);

  // Callbacks :

  void setChangedCB(const FFaDynCB0& aDynCB);
  void setValuesChangedCB(const FFaDynCB1<FuiSpringDefCalcValues&>& aDynCB);

  // Setting values:

  void setValues(const FuiSpringDefCalcValues& values);
  void buildDynamicWidgets(const FuiSpringDefCalcValues& values);
  void setCBs(const FuiSpringDefCalcValues& values);

  // Getting values:

  void getValues(FuiSpringDefCalcValues& values);

protected:
  void initWidgets();

  // Shadow pointers from subclass.
  // Subclass is responsible to set them up:

  FFuLabel*           myVariableLabel;
  FFuIOField*         myVariableField;

  FFuIOField*         myILengthField;
  FFuIOField*         myIDeflField;

  FFuLogicButtonGroup myILDToggleGroup;
  FFuRadioButton*     myILToggle;
  FFuRadioButton*     myIDToggle;

  FFuLabel*           myLengthChangeLabel;
  FuiQueryInputField* myLengthEngineField;

private:

  // Internal methods for rerouting of Callbacks

  void onDoubleChanged(double value);
  void onIntBoolChanged(int button, bool value);
  void onEngineChanged(int idx);
  void onValuesChanged();

  // Callback Objects

  FFaDynCB1<FuiSpringDefCalcValues&> myValuesChangedCB;
  FFaDynCB0                          myChangedCB;

  // Internal Vars

  bool IAmSpring;
  char IAmAngular;
  bool IAmDeflection;
  bool IAmSensitive;
  bool IAmBlockingVariable;
};

#endif
