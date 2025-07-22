// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_VARIABLE_TYPE_H
#define FUI_QT_VARIABLE_TYPE_H

#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "vpmUI/vpmUIComponents/FuiVariableType.H"


class FuiQtVariableType : public FFuQtLabelFrame, public FuiVariableType
{
public:
  FuiQtVariableType(QWidget* parent, const char* name = "FuiQtVariableType");

  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiVariableType::setSensitivity(isSensitive);
  }
};

#endif
