// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_GRAPH_DEFINE_H
#define FUI_QT_GRAPH_DEFINE_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiGraphDefine.H"


class FuiQtGraphDefine : public FFuQtWidget, public FuiGraphDefine
{
public:
  FuiQtGraphDefine(QWidget* parent, const char* name = "FuiQtGraphDefine");

private:
  virtual void setSensitivity(bool) {}
};

#endif
