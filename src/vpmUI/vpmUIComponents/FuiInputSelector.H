// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_INPUT_SELECTOR_H
#define FUI_INPUT_SELECTOR_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <vector>
#include <string>
#include <map>

class FuiQueryInputField;
class FFuOptionMenu;
class FFuaQueryBase;
class FmModelMemberBase;


struct FuiInputSelectorValues
{
  FFuaQueryBase*     objectSelectorQuery = NULL;
  FmModelMemberBase* selectedObject = NULL;

  bool showDofList = false;
  int  selectedDofIdx = -1;
  std::vector<std::string> dofList;

  bool showVariableList = false;
  int  selectedVariableIdx = -1;
  std::vector<std::string> variableList;
};


class FuiInputSelector : public virtual FFuComponentBase
{
public:
  using DynCB1 = FFaDynCB1<FuiInputSelectorValues&>;

  FuiInputSelector(int iArg = -1);

  // Callbacks:

  void setValuesChangedCB(const DynCB1& aDynCB) { myValuesChangedCB = aDynCB; }
  void setSetDBValuesCB(const DynCB1&   aDynCB) { mySetDBValuesCB   = aDynCB; }
  void setPickCB(const FFaDynCB1<int>&  aDynCB) { myPickSensorCB    = aDynCB; }
  void setPickCB(const FFaDynCB0& aDynCB);

  // Setting values:

  void setValues(const FuiInputSelectorValues& values);

  // Getting values:

  void getValues(FuiInputSelectorValues& values);

  // Look

  virtual void setLabel(const std::string& text) = 0;
  virtual void setSensitivity(bool isSensitive);

protected:
  void initWidgets();

  // Shadow pointers from subclass.
  // Subclass is responsible to set them up:

  FuiQueryInputField* myObjSelector;
  FFuOptionMenu*      myDofSelector;
  FFuOptionMenu*      myVarSelector;

private:

  // Internal methods for rerouting of callbacks

  void onIntChanged(int);
  void onSensorPicked();

  // Callback objects

  DynCB1         myValuesChangedCB;
  DynCB1         mySetDBValuesCB;
  FFaDynCB1<int> myPickSensorCB;

  // Internal variables

  bool IAmSensitive;
  bool IAmShowingDofList;
  bool IAmShowingVariableList;
  int  myArgNo;
};


class FuiInputSelectors
{
public:
  using DynCB1 = FuiInputSelector::DynCB1;
  using Values = std::vector<FuiInputSelectorValues>;

  class Sheet : public virtual FFuComponentBase
  {
  public:
    void initWidgets(const DynCB1& valChangedCB,
                     const DynCB1& setDBvalsCB,
                     const FFaDynCB1<int>& sensorPickedCB);

    void setValues(const Values& vals);
    void getValues(Values& vals) const;

    virtual void setSensitivity(bool isSensitive);

  protected:
    std::vector<FuiInputSelector*> myArgs;
  };

  virtual Sheet* createSheet(int numArg,
                             const char* name = "FuiInputSelectors") const = 0;

  void setCBs(const DynCB1& cb1, const DynCB1& cb2, const FFaDynCB1<int>& cb3);

  void setValues(const Values& vals);
  void getValues(Values& vals) const;

  void setSensitivity(bool isSensitive);

  void popUp(size_t numArg);
  void popDown();

protected:
  std::map<size_t,Sheet*> myLists;

  DynCB1         myValuesChangedCB;
  DynCB1         mySetDBvaluessCB;
  FFaDynCB1<int> mySensorPickedCB;
};

#endif
