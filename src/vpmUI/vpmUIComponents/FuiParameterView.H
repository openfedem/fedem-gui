// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_PARAMETER_VIEW_H
#define FUI_PARAMETER_VIEW_H

#include <string>
#include <vector>
#include <map>

#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "FFuLib/FFuBase/FFuMultUIComponent.H"


class FuiParameterView
{
public:
  class ParameterSheet : public virtual FFuMultUIComponent
  {
  public:
    virtual void setFields(const std::vector<std::string>& names,
                           const FFaDynCB1<double>& acceptCB) = 0;
    virtual void setValues(const std::vector<double>& values) = 0;
    virtual void getValues(std::vector<double>& values) const = 0;
  };

  static ParameterSheet* createSheet(FFuMultUIComponent* parent,
                                     const char* name = "ParameterSheet");

  void setFields(int idx, const std::vector<std::string>& names,
                 FFuMultUIComponent* parent = NULL);
  void setValues(int idx, const std::vector<double>& values);
  void getValues(int idx, std::vector<double>& values) const;
  int  getValues(std::vector<double>& values) const;

  void setEdgeGeometry(int v1, int v2, int h1, int h2);
  void setSensitivity(bool isSensitive);
  void popDown();

  void setAcceptedCB(const FFaDynCB1<double>& cb) { myAcceptedCB = cb; }

protected:
  std::map<int,ParameterSheet*> myViews;

  FFaDynCB1<double> myAcceptedCB;
};

#endif
