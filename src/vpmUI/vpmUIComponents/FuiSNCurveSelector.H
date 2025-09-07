// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_SN_CURVE_SELECTOR_H
#define FUI_SN_CURVE_SELECTOR_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <string>

class FFuOptionMenu;


class FuiSNCurveSelector : virtual public FFuComponentBase
{
public:
  FuiSNCurveSelector() { stdTypeMenu = curveTypeMenu = NULL; }

  virtual void setSensitivity(bool isSensitive);

  void setDataChangedCB(const FFaDynCB0& aCB) { this->dataChangedCB = aCB; }

  std::string getSelectedStd() const;
  std::string getSelectedCurve() const;

  void getValues(int& stdIdx, int& curveIdx);
  void setValues(int stdIdx, int curveIdx);

protected:
  void initWidgets();

  virtual void onPoppedUpFromMem();

private:
  void onStdValueChanged(int value);
  void onCurveValueChanged(int value);
  void populateCurveMenu(const std::string& stdName);

protected:
  FFuOptionMenu* stdTypeMenu;
  FFuOptionMenu* curveTypeMenu;

private:
  FFaDynCB0 dataChangedCB;
};

#endif
