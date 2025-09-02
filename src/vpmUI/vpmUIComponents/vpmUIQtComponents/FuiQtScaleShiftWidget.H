// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_SCALE_SHIFT_WIDGET_H
#define FUI_QT_SCALE_SHIFT_WIDGET_H

#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "vpmUI/vpmUIComponents/FuiScaleShiftWidget.H"


class FuiQtScaleShiftWidget : public FFuQtLabelFrame, public FuiScaleShiftWidget
{
public:
  FuiQtScaleShiftWidget(QWidget* parent, const char axis);
};

#endif
