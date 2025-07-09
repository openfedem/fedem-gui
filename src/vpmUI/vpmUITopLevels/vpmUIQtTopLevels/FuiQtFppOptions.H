// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_FPP_OPTIONS_H
#define FUI_QT_FPP_OPTIONS_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiFppOptions.H"


class FuiQtFppOptions : public FFuQtTopLevelShell, public FuiFppOptions
{
public:
  FuiQtFppOptions(int xpos, int ypos, int width, int height,
                  const char* title, const char* name);

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiFppOptions::setSensitivity(isSensitive);
  }
};

#endif
