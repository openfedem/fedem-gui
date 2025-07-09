// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_LINK_RAM_SETTINGS_H
#define FUI_QT_LINK_RAM_SETTINGS_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiLinkRamSettings.H"


class FuiQtLinkRamSettings : public FFuQtTopLevelShell,
                             public FuiLinkRamSettings
{
public:
  FuiQtLinkRamSettings(int xpos, int ypos, int width, int height,
                       const char* title, const char* name);

protected:
  virtual void resizeEvent(QResizeEvent* e);

private:
  virtual void setSensitivity(bool) {}
};

#endif
