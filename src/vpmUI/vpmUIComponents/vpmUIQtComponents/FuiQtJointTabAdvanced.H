// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_JOINT_TAB_ADVANCED_H
#define FUI_QT_JOINT_TAB_ADVANCED_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiJointTabAdvanced.H"

class QGroupBox;
class QLabel;


class FuiQtJointTabAdvanced : public FFuQtWidget, public FuiJointTabAdvanced
{
public:
  FuiQtJointTabAdvanced(QWidget* parent,
                        const char* name = "FuiQtJointAdvanced");

  virtual void showRotFormulation(bool yesOrNo);
  virtual void showSpringCpl(int rotOrYesOrNo);
  virtual void setRotExplain(const char* text);

  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiJointTabAdvanced::setSensitivity(isSensitive);
  }

private:
  QGroupBox* myRotFormulationFrame;
  QGroupBox* mySpringCouplingFrame;
  QLabel*    myRotExplainLabel;
};

#endif
