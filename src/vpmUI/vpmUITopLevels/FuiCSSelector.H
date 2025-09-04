// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_CS_SELECTOR_H
#define FUI_CS_SELECTOR_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"

class FuiCrossSectionListView;
class FuiMaterialListView;
class FFuNotes;
class FFuPushButton;


class FuiCSSelector : virtual public FFuTopLevelShell,
                      public FFuUAExistenceHandler
{
  Fmd_HEADER_INIT();

public:
  FuiCSSelector();

  static FuiCSSelector* create(int xpos = 100, int ypos  = 100,
                               int width= 100, int height= 100,
                               const char* title= "Cross Section Import",
                               const char* name = "FuiCSSelector");

  void setImportCB(const FFaDynCB0& dynCB) { importCB = dynCB; }

protected:
  void initWidgets();

public:
  FuiCrossSectionListView* lvCS;
  FuiMaterialListView*     lvMat;

protected:
  FFuNotes*      notes;
  FFuPushButton* importButton;
  FFuPushButton* closeButton;

private:
  FFaDynCB0 importCB;
};

#endif
