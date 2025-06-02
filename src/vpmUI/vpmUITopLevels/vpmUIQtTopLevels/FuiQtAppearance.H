// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_APPERANCE_H
#define FUI_QT_APPERANCE_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiAppearance.H"


class FuiQtAppearance : public FFuQtTopLevelShell, public FuiAppearance
{
public:
  FuiQtAppearance(int xpos, int ypos, int width, int height,
                  const char* title, const char* name);
};

#endif
