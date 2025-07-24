// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_RESULT_TABS_H
#define FUI_RESULT_TABS_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <vector>
#include <array>

class FFuToggleButton;


class FuiTriadResults : public virtual FFuComponentBase
{
public:
  FuiTriadResults() { myToggles.fill(NULL); }

  void setToggleCB(const FFaDynCB1<bool>& cb);
  void setValues(const std::vector<bool>& vals);
  void getValues(std::vector<bool>& vals) const;

protected:
  void initWidgets();

  std::array<FFuToggleButton*,7> myToggles;
};


class FuiJointResults : public virtual FFuComponentBase
{
public:
  FuiJointResults() { myToggles.fill(NULL); }

  void setToggleCB(const FFaDynCB1<bool>& cb);
  void setValues(const std::vector<bool>& vals);
  void getValues(std::vector<bool>& vals) const;

protected:
  void initWidgets();

  virtual void setSpringDamper(bool onOff) = 0;

  std::array<FFuToggleButton*,15> myToggles;
};

#endif
