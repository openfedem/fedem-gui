// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_MOTION_TYPE_H
#define FUI_QT_MOTION_TYPE_H

#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "vpmUI/vpmUIComponents/FuiMotionType.H"


class FuiQtMotionType : public FFuQtLabelFrame, public FuiMotionType
{
public:
  FuiQtMotionType(QWidget* parent, int nButton = 4,
                  const char* name = "FuiQtMotionType");

  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiMotionType::setSensitivity(isSensitive);
  }
};

#endif
