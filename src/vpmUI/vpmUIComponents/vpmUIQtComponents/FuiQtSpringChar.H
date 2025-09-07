// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_SPRING_CHAR_H
#define FUI_QT_SPRING_CHAR_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiSpringChar.H"


class FuiQtSpringChar : public FFuQtWidget, public FuiSpringChar
{
public:
  FuiQtSpringChar(QWidget* parent, const char* name = "FuiQtSpringChar");

  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiSpringChar::setSensitivity(isSensitive);
  }
};

#endif
