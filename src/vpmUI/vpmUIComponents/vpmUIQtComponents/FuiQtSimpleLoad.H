// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_SIMPLE_LOAD_H
#define FUI_QT_SIMPLE_LOAD_H

#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "vpmUI/vpmUIComponents/FuiSimpleLoad.H"


class FuiQtSimpleLoad : public FFuQtLabelFrame, public FuiSimpleLoad
{
public:
  FuiQtSimpleLoad(QWidget* parent, const char* name = "FuiQtSimpleLoad");

  virtual void setLabel(const char* text)
  {
    this->FFuQtLabelFrame::setLabel(text);
  }

  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiSimpleLoad::setSensitivity(isSensitive);
  }
};

#endif
