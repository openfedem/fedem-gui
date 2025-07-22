// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_MOTION_TYPE_H
#define FUI_MOTION_TYPE_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFuLib/FFuLogicButtonGroup.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <vector>
#include <array>

class FFuToggleButton;
class FFuRadioButton;


class FuiMotionType : public virtual FFuComponentBase
{
public:
  FuiMotionType();

  // Setting up:

  virtual void setSensitivity(bool isSensitive);
  void setSensitivity(unsigned int button, bool isSensitive);

  // Callbacks:

  void setChangedCB(const FFaDynCB0& aDynCB);
  void setValuesChangedCB(const FFaDynCB1<int>& aDynCB);

  // Setting values:

  void setValue(unsigned int motionType);

  // Getting values:

  unsigned int getValue() const;

protected:
  void initWidgets();

  // Shadow pointers from subclass,
  // Subclass is responsible to set them up:

  FFuToggleButton*             myAddButton;
  FFuLogicButtonGroup          myMotionTypeToggleGroup;
  std::vector<FFuRadioButton*> myMotionTypeButtons;
  std::array<const char*,4>    myMotionTypeLabels;

private:

  // Internal methods for rerouting of Callbacks

  void onMotionTypeChanged(int, bool) { this->onValuesChanged(); }
  void onButtonToggled(bool)          { this->onValuesChanged(); }
  void onValuesChanged();

  FFaDynCB1<int> myValuesChangedCB;
  FFaDynCB0      myChangedCB;

  bool IAmSensitive;
};

#endif
