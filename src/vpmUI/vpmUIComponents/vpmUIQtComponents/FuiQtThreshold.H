// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_THRESHOLD_H
#define FUI_QT_THRESHOLD_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiThreshold.H"


class FuiQtThreshold : public FFuQtWidget, public FuiThreshold
{
public:
  FuiQtThreshold(QWidget* parent, const char* name = "FuiQtThreshold");
};

#endif
