// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuIOField.H"
#include "vpmUI/vpmUIComponents/FuiParameterView.H"


void FuiParameterView::PrmSheet::setValues(const std::vector<double>& values)
{
  for (size_t row = 0; row < myFields.size() && row < values.size(); row++)
    myFields[row]->setValue(values[row]);
}


void FuiParameterView::PrmSheet::getValues(std::vector<double>& values) const
{
  values.clear();
  values.reserve(myFields.size());
  for (const FFuIOField* fld : myFields)
    values.push_back(fld->getDouble());
}


void FuiParameterView::PrmSheet::setSensitivity(bool isSensitive)
{
  IAmSensitive = isSensitive;
  for (FFuIOField* fld : myFields)
    fld->setSensitivity(isSensitive);
}


void FuiParameterView::setFields(int idx, const std::vector<std::string>& names,
                                 FFuComponentBase* parent)
{
  if (myViews.find(idx) == myViews.end())
  {
    myViews[idx] = createSheet(parent);
    myViews[idx]->setFields(names,myAcceptedCB);
  }

  for (std::pair<const int,PrmSheet*>& view : myViews)
    if (view.first == idx)
      view.second->popUp();
    else
      view.second->popDown();
}


void FuiParameterView::setValues(int idx, const std::vector<double>& values)
{
  std::map<int,PrmSheet*>::iterator it = myViews.find(idx);
  if (it != myViews.end()) it->second->setValues(values);
}


void FuiParameterView::getValues(int idx, std::vector<double>& values) const
{
  std::map<int,PrmSheet*>::const_iterator it = myViews.find(idx);
  if (it != myViews.end()) it->second->getValues(values);
}


int FuiParameterView::getValues(std::vector<double>& values) const
{
  for (const std::pair<const int,PrmSheet*>& view : myViews)
    if (view.second->isPoppedUp())
    {
      view.second->getValues(values);
      return view.first;
    }

  return -1;
}


void FuiParameterView::setSensitivity(bool isSensitive)
{
  for (std::pair<const int,PrmSheet*>& view : myViews)
    view.second->setSensitivity(isSensitive);
}


void FuiParameterView::popDown()
{
  for (std::pair<const int,PrmSheet*>& view : myViews)
    view.second->popDown();
}
