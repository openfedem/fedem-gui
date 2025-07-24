// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_JOINT_DOF_H
#define FUI_JOINT_DOF_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiSimpleLoad.H"
#include "vpmUI/vpmUIComponents/FuiSpringDefCalc.H"
#include "vpmUI/vpmUIComponents/FuiSprDaForce.H"

class FuiMotionType;
class FuiVariableType;
class FFuLabelField;
class FFuToggleButton;


struct FuiJointDOFValues
{
  int myDofNo;
  int myMotionType = -1;
  bool freqDomain  = false;
  double myInitVel = 0.0;
  FuiSimpleLoadValues    myLoadVals;
  FuiSpringDefCalcValues mySpringDCVals;
  FuiSprDaForceValues    mySpringFSVals;
  FuiSprDaForceValues    myDamperFCVals;

  FuiJointDOFValues(int dof = -1) : myDofNo(dof) {}
};


class FuiJointDOF : public virtual FFuComponentBase
{
public:
  FuiJointDOF();

  void setChangedCB(const FFaDynCB0& aDynCB);

  void getValues(FuiJointDOFValues& values) const;
  void setValues(const FuiJointDOFValues& values,
                 bool isSensitive = true);

  int dofNo;
  FuiMotionType*    motionType;
  FuiVariableType*  variableType;
  FuiSimpleLoad*    simpleLoad;
  FuiSpringDefCalc* springDC;
  FuiSprDaForce*    springFS;
  FuiSprDaForce*    damperFS;
  FFuLabelField*    initialVel;
  FFuToggleButton*  freqToggle;

  enum {
    FREE,
    FIXED,
    PRESCRIBED,
    FREE_DYNAMICS,
    SPRING_CONSTRAINED,
    SPRING_DYNAMICS,
    PRESCRIBED_DISP,
    PRESCRIBED_VEL,
    PRESCRIBED_ACC
  };

protected:
  void initWidgets();
};

#endif
