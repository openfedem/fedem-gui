// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_FUNCTION_PROPERTIES_H
#define FUI_QT_FUNCTION_PROPERTIES_H

#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "vpmUI/vpmUIComponents/FuiFunctionProperties.H"


class FuiQtFunctionProperties : public FFuQtWidget, public FuiFunctionProperties
{
public:
  FuiQtFunctionProperties(QWidget* parent);

  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiFunctionProperties::setSensitivity(isSensitive);
  }

protected:
  class FuiQtJonswapAdvanced : public FFuQtLabelFrame,
                               public FuiFunctionProperties::FuiJonswapAdvanced
  {
  public:
    FuiQtJonswapAdvanced(QWidget* parent);
  };

  class FuiQtJonswapBasic : public FFuQtWidget,
                            public FuiFunctionProperties::FuiJonswapBasic
  {
  public:
    FuiQtJonswapBasic(QWidget* parent);
  };

  class FuiQtWaveSpectrum : public FFuQtLabelFrame,
                            public FuiFunctionProperties::FuiWaveSpectrum
  {
  public:
    FuiQtWaveSpectrum(QWidget* parent);
  };
};

#endif
