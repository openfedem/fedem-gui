// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_CREATE_BEAMSTRING_PAIR_H
#define FUI_QT_CREATE_BEAMSTRING_PAIR_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiCreateBeamstringPair.H"


class FuiQtCreateBeamstringPair : public FFuQtTopLevelShell,
                                  public FuiCreateBeamstringPair
{
public:
  FuiQtCreateBeamstringPair(int xpos, int ypos, int width, int height,
                            const char* title, const char* name);

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiCreateBeamstringPair::setSensitivity(isSensitive);
  }
};

#endif
