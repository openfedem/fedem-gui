// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_STRESS_OPTIONS_H
#define FUI_QT_STRESS_OPTIONS_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiStressOptions.H"


class FuiQtStressOptions : public FFuQtTopLevelShell, public FuiStressOptions
{
public:
  FuiQtStressOptions(int xpos, int ypos, int width, int height,
                     const char* title, const char* name);
 
private:
  virtual void setSensitivity(bool sens)
  {
    this->FuiStressOptions::setSensitivity(sens);
  }
};

#endif
