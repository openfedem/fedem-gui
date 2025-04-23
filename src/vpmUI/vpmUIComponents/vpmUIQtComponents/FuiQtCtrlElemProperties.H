// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_CTRL_ELEM_PROPERTIES_H
#define FUI_QT_CTRL_ELEM_PROPERTIES_H

#include "FFuLib/FFuQtBaseClasses/FFuQtMultUIComponent.H"
#include "vpmUI/vpmUIComponents/FuiCtrlElemProperties.H"


class FuiQtCtrlElemProperties : public virtual FFuQtMultUIComponent,
                                public FuiCtrlElemProperties
{
public:
  FuiQtCtrlElemProperties(QWidget* parent);
  virtual ~FuiQtCtrlElemProperties() {}

  virtual void setSensitivity(bool sensitive)
  { this->FuiCtrlElemProperties::setSensitivity(sensitive); }
};

#endif
