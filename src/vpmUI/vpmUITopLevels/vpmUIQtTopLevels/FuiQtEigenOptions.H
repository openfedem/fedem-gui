// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_EIGEN_OPTIONS_H
#define FUI_QT_EIGEN_OPTIONS_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiEigenOptions.H"


class FuiQtEigenOptions : public FFuQtTopLevelShell, public FuiEigenOptions
{
public:
  FuiQtEigenOptions(int xpos, int ypos, int width, int height,
                    const char* title, const char* name);

private:
  virtual void setSensitivity(bool sens)
  {
    this->FuiEigenOptions::setSensitivity(sens);
  }
};

#endif
