// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_PLAY_PANEL_H
#define FUI_QT_PLAY_PANEL_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiPlayPanel.H"


class FuiQtPlayPanel : public FFuQtWidget, public  FuiPlayPanel
{
public:
  FuiQtPlayPanel(QWidget* parent = NULL, const char* name="PlayPanel");

protected:
  virtual void showEvent(QShowEvent* e);
};

#endif
