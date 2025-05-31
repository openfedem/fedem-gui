// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_PREFERENCES_H
#define FUI_QT_PREFERENCES_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiPreferences.H"


class FuiQtPreferences : public FFuQtTopLevelShell, public FuiPreferences
{
public:
  FuiQtPreferences(int xpos, int ypos, int width, int height,
                   const char* title, const char* name);

private:
  virtual void setSensitivity(bool sens)
  {
    this->FuiPreferences::setSensitivity(sens);
  }
};

#endif
