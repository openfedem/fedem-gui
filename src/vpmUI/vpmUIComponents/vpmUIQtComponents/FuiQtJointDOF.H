// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_JOINT_DOF_H
#define FUI_QT_JOINT_DOF_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiJointDOF.H"


class FuiQtJointDOF : public FFuQtWidget, public FuiJointDOF
{
public:
  FuiQtJointDOF(QWidget* parent, const char* name = "FuiQtJointDOF");
};

#endif
