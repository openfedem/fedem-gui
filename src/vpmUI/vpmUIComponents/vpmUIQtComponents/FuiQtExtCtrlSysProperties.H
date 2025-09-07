// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_EXT_CTRL_SYS_PROPERTIES_H
#define FUI_QT_EXT_CTRL_SYS_PROPERTIES_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiExtCtrlSysProperties.H"


class FuiQtExtCtrlSysProperties : public FFuQtWidget,
                                  public FuiExtCtrlSysProperties
{
public:
  FuiQtExtCtrlSysProperties(QWidget* parent);

  void makeNewFields();

  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiExtCtrlSysProperties::setSensitivity(isSensitive);
  }
};

#endif
