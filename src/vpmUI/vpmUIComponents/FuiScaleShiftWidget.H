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

  virtual void setFrameTitles(const char* main, const char* shift) = 0;

  void getValues(double& scale, bool& onOff, double& shift);
  void setValues(double  scale, bool  onOff, double  shift);

  void setDataChangedCB(const FFaDynCB0& cb) { dataChangedCB = cb; }

protected:
  FFuIOField*      scaleField;
  FFuIOField*      shiftField;
  FFuToggleButton* zeroOutBtn;

  FFaDynCB0 dataChangedCB;
  bool      readingDBonly;
};

#endif
