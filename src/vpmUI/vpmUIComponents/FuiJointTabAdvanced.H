// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_JOINT_TAB_ADVANCED_H
#define FUI_JOINT_TAB_ADVANCED_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuOptionMenu;


class FuiJointTabAdvanced : public virtual FFuComponentBase
{
public:
  void setOptionChangedCB(const FFaDynCB1<std::string>& cb);
  void setSensitivity(bool isSensitive);

  virtual void showRotFormulation(bool yesOrNo) = 0;
  virtual void showSpringCpl(int rotOrYesOrNo) = 0;
  virtual void setRotExplain(const char* text) = 0;

  FFuOptionMenu* myRotFormulationMenu;
  FFuOptionMenu* myRotSequenceMenu;
  FFuOptionMenu* myTranSpringCplMenu;
  FFuOptionMenu* myRotSpringCplMenu;
};

#endif
