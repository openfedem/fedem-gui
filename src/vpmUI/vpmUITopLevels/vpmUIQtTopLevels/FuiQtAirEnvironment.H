// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_AIR_ENVIRONMENT_H
#define FUI_QT_AIR_ENVIRONMENT_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiAirEnvironment.H"


class FuiQtAirEnvironment : public FFuQtTopLevelShell, public FuiAirEnvironment
{
public:
  FuiQtAirEnvironment(int xpos, int ypos, int width, int height,
                      const char* title, const char* name);

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiAirEnvironment::setSensitivity(isSensitive);
  }
};

#endif
