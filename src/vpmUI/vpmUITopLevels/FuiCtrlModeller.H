// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_CTRL_MODELLER_H
#define FUI_CTRL_MODELLER_H

#include "FFuLib/FFuBase/FFuMDIWindow.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"

class FuiCtrlGridAttributes;


class FuiCtrlModeller : public virtual FFuMDIWindow,
                        public FFuUAExistenceHandler, public FFuUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FuiCtrlModeller();
  virtual ~FuiCtrlModeller();

  // Static create method to use instead of constructor :
  // Implementation in GUILib-dependent implementation file

  static FuiCtrlModeller* create(FFuComponentBase* parent,
                                 int xpos  = 100, int ypos   = 100,
                                 int width = 100, int height = 100,
                                 const char* title = "Control Editor",
                                 const char* name = "FuiCtrlModeller");

  FFuComponentBase*      activeViewer()        { return myViewer; }
  FuiCtrlGridAttributes* getGridAttributesUI() { return myCtrlGridAttributes; }

  void showGridUI(bool yes);

protected:
  void initWidgets();

  virtual bool onClose();
  virtual void onPoppedDownToMem();

  FuiCtrlGridAttributes* myCtrlGridAttributes;
  FFuComponentBase*      myViewer;
};

#endif
