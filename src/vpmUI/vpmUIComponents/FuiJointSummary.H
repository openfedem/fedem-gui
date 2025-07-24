// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_JOINT_SUMMARY_H
#define FUI_JOINT_SUMMARY_H

#include "FuiSummaryTable.H"

class FFuToggleButton;
class FFuIOField;
class FFuPushButton;
class FFuOptionMenu;
class FuiQueryInputField;
struct FuiJointDOFValues;


class FuiJointSummary : public FuiSummaryTable
{
public:
  void setSummary(int dof, const FuiJointDOFValues& jval);

  void showFixFreeAll(bool yesOrNo);
  void showSwapJoint(int jType);
  void showTzToggle(bool yesOrNo);
  virtual void showCamVars(bool yesOrNo) = 0;
  virtual void showScrew(bool yesOrNo) = 0;
  virtual void showFriction(char yesOrNo) = 0;

  FFuLabel*           myDefDamperLabel;
  FFuToggleButton*    myDOF_TZ_Toggle;
  FFuPushButton*      mySetAllFreeButton;
  FFuPushButton*      mySetAllFixedButton;
  FFuPushButton*      mySwapJointButton;
  FuiQueryInputField* myFriction;
  FFuOptionMenu*      myFrictionDof;
  FFuToggleButton*    myScrewToggle;
  FFuIOField*         myScrewRatioField;
  FFuIOField*         myCamThicknessField;
  FFuIOField*         myCamWidthField;
  FFuToggleButton*    myRadialToggle;

protected:
  virtual void initWidgets();
};

#endif
