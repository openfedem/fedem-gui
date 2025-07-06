// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiWorkSpace.H"
#include "FFuLib/FFuBase/FFuMDIWindow.H"
#include "vpmApp/FapEventManager.H"


void FuiWorkSpace::sendWindowActivated()
{
  FFuMDIWindow* win = this->getActiveWindow();
  if (win) win->onWindowActivated();

  FapEventManager::setActiveWindow(win);
}
