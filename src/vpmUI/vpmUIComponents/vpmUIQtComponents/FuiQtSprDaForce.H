// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_SPR_DA_FORCE_H
#define FUI_QT_SPR_DA_FORCE_H

#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "vpmUI/vpmUIComponents/FuiSprDaForce.H"


class FuiQtSprDaForce : public FFuQtLabelFrame, public FuiSprDaForce
{
public:
  FuiQtSprDaForce(QWidget* parent, const char* name = "FuiQtSprDaForce");

  virtual void setLabel(const char* text)
  {
    this->FFuQtLabelFrame::setLabel(text);
  }

  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiSprDaForce::setSensitivity(isSensitive);
  }
};

#endif
