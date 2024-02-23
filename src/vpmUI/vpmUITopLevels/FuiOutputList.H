// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_OUTPUT_LIST_H
#define FUI_OUTPUT_LIST_H

#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"
#include "FFuLib/FFuBase/FFuTopLevelShell.H"

class FFuMemo;


class FuiOutputList : public virtual FFuTopLevelShell,
                      public FFuUAExistenceHandler, public FFuUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FuiOutputList();
  virtual ~FuiOutputList() {}

  // Static create method to use instead of constructor :
  // Implementation in GUILib-dependent implementation file
  static FuiOutputList* create(int xpos, int ypos, int width, int height);

  void addText(const char* text);

  // Slots from commands
  void clearList();
  void selectAll();
  void copySelected();
  bool hasText();
  bool hasSelectedText();

protected:
  void initWidgets();

  virtual void placeWidgets(int width,int height);

  virtual bool onClose();

  FFuMemo* myMemo;
};

#endif
