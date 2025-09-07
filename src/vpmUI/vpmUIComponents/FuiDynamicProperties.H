// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_DYNAMIC_PROPERTIES_H
#define FUI_DYNAMIC_PROPERTIES_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <array>

class FFuLabelField;


class FuiDynamicProperties : virtual public FFuComponentBase
{
public:
  void initWidgets(const FFaDynCB1<double>& acceptedCB);

  void setValues(double alpha1, double alpha2, double sclK, double sclM,
                 int blankFlags = 0);
  int getValues(double& alpha1, double& alpha2, double& sclK, double& sclM);

  virtual void setSensitivity(bool iSensitive);

protected:
  std::array<FFuLabelField*,4> myFields{};
};

#endif
