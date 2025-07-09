// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_TIME_INTERVAL_H
#define FUI_QT_TIME_INTERVAL_H

#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "vpmUI/vpmUIComponents/FuiTimeInterval.H"


class FuiQtTimeInterval : public FFuQtLabelFrame, public FuiTimeInterval
{
public:
  FuiQtTimeInterval(QWidget* parent, const char* name = "FuiQtTimeInterval");

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiTimeInterval::setSensitivity(isSensitive);
  }
};

#endif
