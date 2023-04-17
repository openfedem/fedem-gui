// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_FUNCTION_PROPERTIES_H
#define FUI_QT_FUNCTION_PROPERTIES_H

#include "FFuLib/FFuQtBaseClasses/FFuQtMultUIComponent.H"
#include "vpmUI/vpmUIComponents/FuiFunctionProperties.H"


class FuiQtFunctionProperties : public virtual FFuQtMultUIComponent,
                                public FuiFunctionProperties
{
public:
  FuiQtFunctionProperties(QWidget* parent);
  virtual ~FuiQtFunctionProperties() {}

  virtual void setSensitivity(bool sensitive)
  { this->FuiFunctionProperties::setSensitivity(sensitive); }

protected:
  virtual void setNoArgs(size_t narg);
};

#endif
