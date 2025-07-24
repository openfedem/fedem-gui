// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_TRIAD_DOF_H
#define FUI_QT_TRIAD_DOF_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiTriadDOF.H"


class FuiQtTriadDOF : public FFuQtWidget, public FuiTriadDOF
{
public:
  FuiQtTriadDOF(QWidget* parent, const char* name="FuiQtTriadDOF");
};

#endif
