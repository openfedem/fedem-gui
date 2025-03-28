// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiParameterView.H"


void FuiParameterView::setFields(int idx,
                                 const std::vector<std::string>& names,
                                 FFuMultUIComponent* parent)
{
  if (myViews.find(idx) == myViews.end())
  {
    myViews[idx] = createSheet(parent);
    myViews[idx]->setFields(names,myAcceptedCB);
  }

  for (std::pair<const int,ParameterSheet*>& view : myViews)
    if (view.first == idx)
      view.second->popUp();
    else
      view.second->popDown();
}


void FuiParameterView::setValues(int idx, const std::vector<double>& values)
{
  std::map<int,ParameterSheet*>::iterator it = myViews.find(idx);
  if (it != myViews.end()) it->second->setValues(values);
}


void FuiParameterView::getValues(int idx, std::vector<double>& values) const
{
  std::map<int,ParameterSheet*>::const_iterator it = myViews.find(idx);
  if (it != myViews.end()) it->second->getValues(values);
}


int FuiParameterView::getValues(std::vector<double>& values) const
{
  for (const std::pair<const int,ParameterSheet*>& view : myViews)
    if (view.second->isPoppedUp())
    {
      view.second->getValues(values);
      return view.first;
    }

  return -1;
}


void FuiParameterView::setEdgeGeometry(int v1, int v2, int h1, int h2)
{
  for (std::pair<const int,ParameterSheet*>& view : myViews)
    view.second->setEdgeGeometry(v1,v2,h1,h2);
}


void FuiParameterView::setSensitivity(bool isSensitive)
{
  for (std::pair<const int,ParameterSheet*>& view : myViews)
    view.second->setSensitivity(isSensitive);
}


void FuiParameterView::popDown()
{
  for (std::pair<const int,ParameterSheet*>& view : myViews)
    view.second->popDown();
}
