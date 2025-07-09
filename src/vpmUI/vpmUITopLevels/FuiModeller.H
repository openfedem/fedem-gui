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
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FuiPlayPanel;
class Fui3DPoint;
class FaVec3;


/*!
  \brief The 3D viewer of Fedem.

  Contains an animation control panel and a 3D-point editor.
*/

class FuiModeller : public virtual FFuMDIWindow, public FFuUAExistenceHandler
{
  Fmd_HEADER_INIT();

public:
  FuiModeller();
  virtual ~FuiModeller() {}

  // Static create method to use instead of constructor :
  // Implementation in GUILib-dependent implementation file

  static FuiModeller* create(FFuComponentBase* parent,
			     int xpos  = 100, int ypos   = 100,
			     int width = 100, int height = 100,
			     const char* title = "Modeler",
			     const char* name  = "FuiModeller");

  // Member UI Interface :

  Fui3DPoint*       getPickedPointUIC() { return my3DpointUI; }
  FFuComponentBase* activeViewer()      { return myViewer; }

  // Direct interface :

  void mapAnimControls(bool yesOrNo);

  // Set external Callbacks :

  void setPointChangedCB    (const FFaDynCB2<const FaVec3&,bool>& aDynCB);
  void setRefChangedCB      (const FFaDynCB1<bool>& aDynCB);
  void setAnimTypeChangedCB (const FFaDynCB1<int>&  aDynCB);
  void setAnimationPressCB  (const FFaDynCB1<int>&  aDynCB);
  void setRealTimeCB        (const FFaDynCB0&       aDynCB);
  void setSpeedScaleChangeCB(const FFaDynCB1<int>&  aDynCB);
  void setCloseAnimCB       (const FFaDynCB0&       aDynCB);
  void setAllFramesCB       (const FFaDynCB1<bool>& aDynCB);

  // Virtual mode and state handling

  virtual void updateMode();
  virtual void updateState(int newState);
  virtual void cancel();

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
