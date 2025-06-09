// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_CREATE_TURBINE_TOWER_H
#define FUI_QT_CREATE_TURBINE_TOWER_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiCreateTurbineTower.H"


class FuiQtCreateTurbineTower : public FFuQtTopLevelShell, public FuiCreateTurbineTower
{
public:
  FuiQtCreateTurbineTower(int xpos, int ypos, int width, int height,
                          const char* title, const char* name);

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiCreateTurbineTower::setSensitivity(isSensitive);
  }
};

#endif
