// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_REPEAT_CURVE_H
#define FUI_REPEAT_CURVE_H

#include "FFuLib/FFuBase/FFuModalDialog.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuLabel;
class FFuLabelField;
class FFuDialogButtons;


class FuiRepeatCurve : virtual public FFuModalDialog
{
  Fmd_HEADER_INIT();

protected:
  FuiRepeatCurve();

  static FuiRepeatCurve* create(int xpos = 100, int ypos = 100,
				int width = 100, int height = 100,
				const char* title = "Repeat Curve",
				const char* name = "FuiRepeatCurve");

public:
  static FuiRepeatCurve* getUI(bool onScreen = true);

  void setDialogButtonClickedCB(const FFaDynCB1<int>& cb) { myClickedCB = cb; }

  void getValues(int& from, int& to);
  void setValues(int from, int to);
  void setLabel(const std::string& newLabel);

protected:
  void initWidgets();

private:
  virtual void onDialogButtonClicked(int);

protected:
  FFuLabel*         myLabel;
  FFuLabelField*    myFromField;
  FFuLabelField*    myToField;
  FFuDialogButtons* myDialogButtons;

private:
  FFaDynCB1<int> myClickedCB;
};

#endif
