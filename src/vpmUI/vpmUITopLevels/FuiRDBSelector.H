// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_RDB_SELECTOR_H
#define FUI_RDB_SELECTOR_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"

class FuiSimModelRDBListView;
class FuiRDBListView;
class FFuLabel;
class FFuDialogButtons;


class FuiRDBSelector : virtual public FFuTopLevelShell,
                       public FFuUAExistenceHandler, public FFuUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FuiRDBSelector();

  static FuiRDBSelector* create(int xpos = 100, int ypos  = 100,
				int width= 100, int height= 100,
				const char* title= "Curve Data Value",
				const char* name = "FuiRDBSelector");

  void setResultAppliedCB(const FFaDynCB0& dynCB) { resultAppliedCB = dynCB; }
  void setOkCancelDialog(bool yesOrNo);
  void setApplyable(bool able);

protected:
  void initWidgets();
  virtual void placeWidgets(int width, int height);
  virtual bool onClose();

private:
  void onDialogButtonClicked(int button);

public:
  FuiSimModelRDBListView* lvRes;
  FuiRDBListView*         lvPos;

protected:
  FFuLabel* resLabel;
  FFuLabel* posLabel;
  FFuLabel* notesImage;
  FFuLabel* notesLabel;
  FFuLabel* notesText;

  FFuDialogButtons* dialogButtons;

private:
  FFaDynCB0 resultAppliedCB;
};

#endif
