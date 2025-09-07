// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_CTRL_ELEM_PROPERTIES_H
#define FUI_QT_CTRL_ELEM_PROPERTIES_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiCtrlElemProperties.H"


class FuiQtCtrlElemProperties : public FFuQtWidget, public FuiCtrlElemProperties
{
public:
  FuiQtCtrlElemProperties(QWidget* parent);

  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiCtrlElemProperties::setSensitivity(isSensitive);
  }
};

#endif
