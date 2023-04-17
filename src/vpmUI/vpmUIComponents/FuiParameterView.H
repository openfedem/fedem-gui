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

#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "FFuLib/FFuBase/FFuMultUIComponent.H"


class FuiParameterView : public virtual FFuMultUIComponent
{
public:
  virtual void clear() = 0;
  virtual void setFields(const std::vector<std::string>& names) = 0;
  virtual void setValues(const std::vector<double>& values) = 0;
  virtual void getValues(std::vector<double>& values) const = 0;

  void setAcceptedCB(const FFaDynCB1<double>& cb) { myAcceptedCB = cb; }

protected:
  FFaDynCB1<double> myAcceptedCB;
};

#endif
