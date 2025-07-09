// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuBase/FFuTopLevelShell.H"

std::vector<FFuTopLevelShell*> FFuTopLevelShell::ourTopLevelDialogs;

Fmd_SOURCE_INIT(FUI_TOPLEVELSHELL, FFuTopLevelShell, FFuTopLevelShell);

//----------------------------------------------------------------------------

FFuTopLevelShell::FFuTopLevelShell()
{
  Fmd_CONSTRUCTOR_INIT(FFuTopLevelShell);

  ourTopLevelDialogs.push_back(this);
}

//----------------------------------------------------------------------------

FFuTopLevelShell::~FFuTopLevelShell()
{
  size_t runner = 0;
  for (FFuTopLevelShell* tls : ourTopLevelDialogs)
    if (tls == this)
    {
      ourTopLevelDialogs.erase(ourTopLevelDialogs.begin()+runner);
      break;
    }
    else
      ++runner;
}

//----------------------------------------------------------------------------

void FFuTopLevelShell::manage(bool onScreen, bool inMem)
{
  if (onScreen) // Requested on screen
  {
    this->popUp();
    this->showTLSNormal();
  }
  else if (inMem) // Requested off screen but in memory
  {
    this->popDown();
  }
  else // Requested off screen and removed from mem
  {
    // Pop down before we delete so that onPoppedDown events are being given
    this->popDown();
    delete this;
  }
}

//----------------------------------------------------------------------------

bool FFuTopLevelShell::onClose()
{
  this->popDown();
  return false;
}

//----------------------------------------------------------------------------

FFuTopLevelShell* FFuTopLevelShell::getInstanceByType(int classType)
{
  for (FFuTopLevelShell* tls : ourTopLevelDialogs)
    if (tls->isOfType(classType)) return tls;

  return NULL;
}

//----------------------------------------------------------------------------
