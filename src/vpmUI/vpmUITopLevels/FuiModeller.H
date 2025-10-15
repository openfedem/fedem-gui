// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_MODELLER_H
#define FUI_MODELLER_H

#include "FFuLib/FFuBase/FFuMDIWindow.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"

class FuiPlayPanel;
class Fui3DPoint;


/*!
  \brief The 3D viewer of Fedem.

  Contains an animation control panel and a 3D-point editor.
*/

class FuiModeller : public virtual FFuMDIWindow, public FFuUAExistenceHandler
{
  Fmd_HEADER_INIT();

public:
  FuiModeller();

  // Static create method to use instead of constructor :
  // Implementation in GUILib-dependent implementation file

  static FuiModeller* create(FFuComponentBase* parent,
			     int xpos  = 100, int ypos   = 100,
			     int width = 100, int height = 100,
			     const char* title = "Modeler",
			     const char* name  = "FuiModeller");

  // Member UI Interface :

  FuiPlayPanel*     getPlayPanelUIC()   { return myPlayPanel; }
  Fui3DPoint*       getPickedPointUIC() { return my3DpointUI; }
  FFuComponentBase* activeViewer()      { return myViewer; }

  // Direct interface :

  void mapAnimControls(bool yesOrNo);

protected:

  // Init method to be called by constructor of GUILib-dependent subclass

  void initWidgets();

  // Reimplemented methods :

  virtual bool onClose();
  virtual void onPoppedDownToMem();

  // Widgets :

  FuiPlayPanel*     myPlayPanel;
  Fui3DPoint*       my3DpointUI;
  FFuComponentBase* myViewer;
};

#endif
