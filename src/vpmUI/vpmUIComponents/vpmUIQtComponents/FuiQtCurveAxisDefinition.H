// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_CURVE_AXIS_DEFINITION_H
#define FUI_QT_CURVE_AXIS_DEFINITION_H

#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "vpmUI/vpmUIComponents/FuiCurveAxisDefinition.H"


class FuiQtCurveAxisDefinition : public FFuQtLabelFrame,
                                 public FuiCurveAxisDefinition
{
public:
  FuiQtCurveAxisDefinition(QWidget* parent, const char* title,
			   const char* name = "FuiQtCurveAxisDefinition");

private:
  virtual void setSensitivity(bool) {}
};


class FuiQtCurveTimeRange : public FFuQtLabelFrame,
                            public FuiCurveTimeRange
{
public:
  FuiQtCurveTimeRange(QWidget* parent,
                      const char* name = "FuiQtCurveTimeRange");

private:
  virtual void setSensitivity(bool) {}
};

#endif
