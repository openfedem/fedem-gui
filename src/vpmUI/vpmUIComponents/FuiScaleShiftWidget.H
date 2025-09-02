// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_SCALE_SHIFT_WIDGET_H
#define FUI_SCALE_SHIFT_WIDGET_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuIOField;
class FFuToggleButton;


class FuiScaleShiftWidget : public virtual FFuComponentBase
{
public:
  FuiScaleShiftWidget();

  void getValues(double& scale, bool& onOff, double& shift);
  void setValues(double  scale, bool  onOff, double  shift);

  void setDataChangedCB(const FFaDynCB0& cb) { dataChangedCB = cb; }

private:
  void dblChanged(double) { dataChangedCB.invoke(); }
  // Invoke the data changed call-back only when the user actually toggles the
  // button - not when selecting an object for which the toggle already was on
  void btnToggled(bool) { if (!readingDBonly) dataChangedCB.invoke(); }

protected:
  void initWidgets();

  FFuIOField*      scaleField;
  FFuIOField*      shiftField;
  FFuToggleButton* zeroOutBtn;

private:
  FFaDynCB0 dataChangedCB;
  bool      readingDBonly;
};

#endif
