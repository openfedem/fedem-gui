// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_OUTPUT_LIST_H
#define FUI_QT_OUTPUT_LIST_H

#include "vpmUI/vpmUITopLevels/FuiOutputList.H"
#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"


class FuiQtOutputList : public FFuQtTopLevelShell, public FuiOutputList
{
public:
  FuiQtOutputList(int xpos, int ypos, int width, int height,
                  const char* title = "Output List",
                  const char* name  = "FuiOutputList");
  virtual ~FuiQtOutputList() {}
};

#endif
