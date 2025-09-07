// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_SIMPLE_LOAD_H
#define FUI_SIMPLE_LOAD_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuaQueryBase;
class FuiQueryInputField;
class FuiQueryInputFieldValues;
class FmModelMemberBase;


struct FuiSimpleLoadValues
{
  bool   isConstant = true;
  double constValue = 0.0;

  FFuaQueryBase*     engineQuery = NULL;
  FmModelMemberBase* selectedEngine = NULL;

  FFaDynCB1<FuiQueryInputFieldValues&> myEngineQIFieldButtonCB;
};


class FuiSimpleLoad : public virtual FFuComponentBase
{
public:

  // Setting Up :

  virtual void setLabel(const char* text) = 0;
  virtual void setSensitivity(bool isSensitive);

  // Callbacks :

  void setChangedCB(const FFaDynCB0& aDynCB);
  void setValuesChangedCB(const FFaDynCB1<FuiSimpleLoadValues&>& aDynCB);

  // Setting values:

  void setValues(const FuiSimpleLoadValues& values);
  void setCBs(const FuiSimpleLoadValues& values);

  // Getting values:

  void getValues(FuiSimpleLoadValues& values);

protected:
  void initWidgets();

  FuiQueryInputField* myEngineField;

private:

  // Internal methods for rerouting of Callbacks

  void onQIFieldChanged(FuiQueryInputField* changedField);
  void onValuesChanged();

  FFaDynCB1<FuiSimpleLoadValues&> myValuesChangedCB;
  FFaDynCB0                       myChangedCB;
};

#endif
