// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_DYNAMIC_PROPERTIES_H
#define FUI_QT_DYNAMIC_PROPERTIES_H

#include "vpmUI/vpmUIComponents/FuiDynamicProperties.H"
#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"


class FuiQtDynamicProperties : public FFuQtWidget, public FuiDynamicProperties
{
public:
  FuiQtDynamicProperties(QWidget* parent = NULL,
                         const char* name = "FuiQtDynamicProperties");

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiDynamicProperties::setSensitivity(isSensitive);
  }
};

#endif
