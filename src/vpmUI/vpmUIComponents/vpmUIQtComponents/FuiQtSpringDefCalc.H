// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_SPRING_DEF_CALC_H
#define FUI_QT_SPRING_DEF_CALC_H

#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "vpmUI/vpmUIComponents/FuiSpringDefCalc.H"


class FuiQtSpringDefCalc : public FFuQtLabelFrame, public FuiSpringDefCalc
{
public:
  FuiQtSpringDefCalc(QWidget* parent, const char* name = "FuiQtSpringDefCalc");

  virtual void setLabel(const char* text)
  {
    this->FFuQtLabelFrame::setLabel(text);
  }

  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiSpringDefCalc::setSensitivity(isSensitive);
  }
};

#endif
