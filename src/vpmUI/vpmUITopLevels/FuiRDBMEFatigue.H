// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_RDB_ME_FATIGUE_H
#define FUI_RDB_ME_FATIGUE_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"

class FFuLabel;
class FFuTable;
class FFuIOField;
class FFuToggleButton;
class FFuPushButton;
class FFuDialogButtons;


class FuiRDBMEFatigue : virtual public FFuTopLevelShell,
                        public FFuUAExistenceHandler, public FFuUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FuiRDBMEFatigue();

  static FuiRDBMEFatigue* create(int xpos = 100, int ypos  = 100,
				 int width= 100, int height= 100,
				 const char* title= "Fatigue summary",
				 const char* name = "FuiRDBMEFatigue");

protected:
  void initWidgets();
  virtual bool onClose();

private:
  virtual void onCopyDataClicked() = 0;
  void onDialogButtonClicked(int button);

public:
  FFuTable*         tableMain;
  FFuToggleButton*  useProbToggle;
  FFuPushButton*    copyDataButton;
  FFuIOField*       startTimeField;
  FFuIOField*       stopTimeField;
  FFuIOField*       standardField;
  FFuIOField*       snCurveField;
  FFuLabel*         notesText;
  FFuDialogButtons* dialogButtons;
};

#endif
