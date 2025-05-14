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
class FFuLabelFrame;
class FFuLabelField;
class FFuPushButton;
class FFuOptionMenu;
class FuiQueryInputField;
class FuiJointDOFValues;


class FuiJointSummary : public FuiSummaryTable
{
public:
  void setRowLabel(int dof, const FuiJointDOFValues& jval, const char** labels);
  void setSummary(int dof, const FuiJointDOFValues& jval);
  void showFixFreeAll(bool yesOrNo);
  void showSwapJoint(int jType);
  void showTzToggle(bool yesOrNo);
  void showFriction(char yesOrNo);
  void showScrew(bool yesOrNo);
  void showCamVars(bool yesOrNo);

  FFuToggleButton    * myDOF_TZ_Toggle;
  FFuLabel           * myDefDamperLabel;
  FFuPushButton      * mySetAllFreeButton;
  FFuPushButton      * mySetAllFixedButton;
  FFuPushButton      * mySwapJointButton;
  FFuLabelFrame      * myFrictionFrame;
  FuiQueryInputField * myFriction;
  FFuLabel           * myFrictionLabel;
  FFuOptionMenu      * myFrictionDof;
  FFuLabelFrame      * myScrewFrame;
  FFuToggleButton    * myScrewToggle;
  FFuLabelField      * myScrewRatioField;
  FFuLabelField      * myCamThicknessField;
  FFuLabelField      * myCamWidthField;
  FFuToggleButton    * myRadialToggle;

protected:
  virtual void placeWidgets(int width, int height);
  virtual void initWidgets();

private:
  bool IAmShowingFixFreeAll;
  bool IAmShowingSwapJoint;
  bool IAmShowingTzToggle;
  char IAmShowingFriction;
  bool IAmShowingScrew;
  bool IAmShowingCamVars;
  char IAmShowingFootNotes;
};

#endif
