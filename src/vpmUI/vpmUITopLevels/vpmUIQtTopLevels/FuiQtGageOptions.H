// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_GAGE_OPTIONS_H
#define FUI_QT_GAGE_OPTIONS_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiGageOptions.H"


class FuiQtGageOptions : public FFuQtTopLevelShell, public FuiGageOptions
{
public:
  FuiQtGageOptions(int xpos, int ypos, int width, int height,
                   const char* title, const char* name);

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiGageOptions::setSensitivity(isSensitive);
  }
};

#endif
