// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_MODELLER_H
#define FUI_QT_MODELLER_H

#include "FFuLib/FFuQtBaseClasses/FFuQtMDIWindow.H"
#include "vpmUI/vpmUITopLevels/FuiModeller.H"


class FuiQtModeller : public FFuQtMDIWindow, public FuiModeller
{
public:
  FuiQtModeller(QWidget* parent, int xpos, int ypos, int width, int height,
                const char* title, const char* name);

protected:
  virtual void resizeEvent(QResizeEvent* e);
};

#endif
