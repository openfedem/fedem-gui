// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiInputSelector.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "FFuLib/FFuOptionMenu.H"


FuiInputSelector::FuiInputSelector(int iArg)
{
  IAmSensitive = true;
  IAmShowingDofList = IAmShowingVariableList = false;
  myArgNo = iArg;
}


void FuiInputSelector::initWidgets()
{
  myObjSelector->setRefSelectedCB(FFaDynCB1M(FuiInputSelector,this,
                                             onIntChanged,int));
  myObjSelector->setBehaviour(FuiQueryInputField::REF_NONE);
  myObjSelector->setButtonMeaning(FuiQueryInputField::SELECT);

  if (myArgNo >= 0)
    myObjSelector->setButtonCB(FFaDynCB0M(FuiInputSelector,this,
                                          onSensorPicked));

  myDofSelector->setOptionSelectedCB(FFaDynCB1M(FuiInputSelector,this,
                                                onIntChanged,int));
  myVarSelector->setOptionSelectedCB(FFaDynCB1M(FuiInputSelector,this,
                                                onIntChanged,int));
}


void FuiInputSelector::setPickCB(const FFaDynCB0& aDynCB)
{
  myObjSelector->setButtonCB(aDynCB);
}


void FuiInputSelector::setValues(const FuiInputSelectorValues& values)
{
  // Sensor selector :

  myObjSelector->setQuery(values.objectSelectorQuery);
  myObjSelector->setSelectedRef(values.selectedObject);

  // DOF Selector :

  IAmShowingDofList = values.showDofList;
  myDofSelector->setOptions(values.dofList);
  myDofSelector->selectOption(values.selectedDofIdx);

  // Variable Selector :

  IAmShowingVariableList = values.showVariableList;
  myVarSelector->setOptions(values.variableList);
  myVarSelector->selectOption(values.selectedVariableIdx);

  FuiInputSelectorValues tmp;
  this->getValues(tmp);
  mySetDBValuesCB.invoke(tmp);

  this->setSensitivity(IAmSensitive);
}


void FuiInputSelector::getValues(FuiInputSelectorValues& values)
{
  values.selectedObject = myObjSelector->getSelectedRef();

  if (IAmShowingDofList)
    values.selectedDofIdx = myDofSelector->getSelectedOption();
  else
    values.selectedDofIdx = -1;

  if (IAmShowingVariableList)
    values.selectedVariableIdx = myVarSelector->getSelectedOption();
  else
    values.selectedVariableIdx = -1;
}


void FuiInputSelector::onIntChanged(int)
{
  FuiInputSelectorValues values;
  this->getValues(values);
  myValuesChangedCB.invoke(values);
}


void FuiInputSelector::onSensorPicked()
{
  if (myArgNo >= 0)
    myPickSensorCB.invoke(myArgNo);
}


void FuiInputSelector::setSensitivity(bool isSensitive)
{
  myObjSelector->setSensitivity(IAmSensitive = isSensitive);
  myDofSelector->setSensitivity(IAmSensitive && IAmShowingDofList);
  myVarSelector->setSensitivity(IAmSensitive && IAmShowingVariableList);
}


void FuiInputSelectors::setCBs(const DynCB1& cb1, const DynCB1& cb2,
                               const FFaDynCB1<int>& cb3)
{
  myValuesChangedCB = cb1;
  mySetDBvaluessCB = cb2;
  mySensorPickedCB = cb3;
}


void FuiInputSelectors::setValues(const Values& vals)
{
  std::map<size_t,Sheet*>::iterator it = myLists.find(vals.size());
  if (it != myLists.end()) it->second->setValues(vals);
}


void FuiInputSelectors::getValues(Values& vals) const
{
  std::map<size_t,Sheet*>::const_iterator it = myLists.find(vals.size());
  if (it != myLists.end()) it->second->getValues(vals);
}


void FuiInputSelectors::setSensitivity(bool isSensitive)
{
  for (std::pair<const size_t,Sheet*>& list : myLists)
    list.second->setSensitivity(isSensitive);
}


void FuiInputSelectors::popUp(size_t numArg)
{
  std::map<size_t,Sheet*>::iterator it = myLists.find(numArg);
  if (it == myLists.end())
  {
    it = myLists.insert({ numArg, this->createSheet(numArg) }).first;
    it->second->initWidgets(myValuesChangedCB,mySetDBvaluessCB,
                            mySensorPickedCB);
  }

  for (std::pair<const size_t,Sheet*>& list : myLists)
    if (list.first == numArg)
      list.second->popUp();
    else
      list.second->popDown();
}


void FuiInputSelectors::popDown()
{
  for (std::pair<const size_t,Sheet*>& list : myLists)
    list.second->popDown();
}


void FuiInputSelectors::Sheet::initWidgets(const DynCB1& valChangedCB,
                                           const DynCB1& setDBvalsCB,
                                           const FFaDynCB1<int>& sensorPickedCB)
{
  for (size_t i = 0; i < myArgs.size(); i++)
  {
    char cArg = myArgs.size() > 4 ? '1'+i : (i < 3 ? 'x'+i : 't');
    myArgs[i]->setLabel(std::string("Argument ") + cArg);
    myArgs[i]->setValuesChangedCB(valChangedCB);
    myArgs[i]->setSetDBValuesCB(setDBvalsCB);
    myArgs[i]->setPickCB(sensorPickedCB);
  }
}


void FuiInputSelectors::Sheet::setValues(const Values& vals)
{
  for (size_t i = 0; i < vals.size() && i < myArgs.size(); i++)
    myArgs[i]->setValues(vals[i]);
}


void FuiInputSelectors::Sheet::getValues(Values& vals) const
{
  vals.resize(myArgs.size());
  for (size_t i = 0; i < myArgs.size(); i++)
    myArgs[i]->getValues(vals[i]);
}


void FuiInputSelectors::Sheet::setSensitivity(bool isSensitive)
{
  for (FuiInputSelector* arg : myArgs)
    arg->setSensitivity(isSensitive);
}
