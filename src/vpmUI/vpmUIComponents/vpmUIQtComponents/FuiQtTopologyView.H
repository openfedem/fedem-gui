// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_TOPOLOGY_VIEW_H
#define FUI_QT_TOPOLOGY_VIEW_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiTopologyView.H"


class FuiQtTopologyView : public FFuQtWidget, public FuiTopologyView
{
public:
  FuiQtTopologyView(QWidget* parent, const char* name = "FuiTopologyView");
};

#endif
