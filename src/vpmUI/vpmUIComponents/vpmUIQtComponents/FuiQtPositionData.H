// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_POSITION_DATA_H
#define FUI_QT_POSITION_DATA_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiPositionData.H"


class FuiQtPositionData : public FFuQtWidget, public FuiPositionData
{
public:
  FuiQtPositionData(QWidget* parent = NULL, const char* name = "PositionData");

  virtual void setSensitivity(bool onOff)
  {
    this->FuiPositionData::setSensitivity(onOff);
  }
};

#endif
