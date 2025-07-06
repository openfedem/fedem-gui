// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_WORK_SPACE_H
#define FUI_WORK_SPACE_H

#include "FFuLib/FFuBase/FFuComponentBase.H"

class FFuMDIWindow;


class FuiWorkSpace : virtual public FFuComponentBase
{
public:
  void sendWindowActivated();

  virtual void cascadeWins() = 0;
  virtual void tileWins() = 0;
  virtual void tabWins() = 0;
  virtual void subWins() = 0;
 
protected:
  virtual FFuMDIWindow* getActiveWindow() = 0;
};

#endif
