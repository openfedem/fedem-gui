// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_SPLIT_BEAM_H
#define FUI_SPLIT_BEAM_H

#include "FFuLib/FFuBase/FFuModalDialog.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuLabelField;
class FFuDialogButtons;


class FuiSplitBeam : virtual public FFuModalDialog
{
  Fmd_HEADER_INIT();

protected:
  FuiSplitBeam();

  static FuiSplitBeam* create(int xpos = 100, int ypos = 100,
			      int width = 100, int height = 100,
			      const char* title = "Split Beam",
			      const char* name = "FuiSplitBeam");

public:
  static FuiSplitBeam* getUI(bool onScreen = true);

  void setDialogButtonClickedCB(const FFaDynCB1<int>& cb) { myClickedCB = cb; }

  int getValue();

protected:
  void initWidgets();

private:
  virtual void onDialogButtonClicked(int);

protected:
  FFuLabelField*    myNumField;
  FFuDialogButtons* myDialogButtons;

private:
  FFaDynCB1<int> myClickedCB;
};

#endif
