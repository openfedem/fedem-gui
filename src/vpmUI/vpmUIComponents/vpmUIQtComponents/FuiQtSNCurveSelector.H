// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_SN_CURVE_SELCTOR_H
#define FUI_QT_SN_CURVE_SELCTOR_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiSNCurveSelector.H"


class FuiQtSNCurveSelector : public FFuQtWidget, public FuiSNCurveSelector
{
public:
  FuiQtSNCurveSelector(QWidget* parent,
                       const char* name = "FuiQtSNCurveSelector");

  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiSNCurveSelector::setSensitivity(isSensitive);
  }
};

#endif
