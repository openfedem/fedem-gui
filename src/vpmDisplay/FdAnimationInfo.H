// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_ANIMATION_INFO_H
#define FD_ANIMATION_INFO_H

///////////////////
//
//        This node was made by
//  Morten from Norsk Simulator Senter
//           26.Feb.1997
//
/////////////////

#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFFloat.h>

class SoGLDisplayList;


#ifdef win32
#include <SoWinLeaveScope.h>
#endif

class FdAnimationInfo : public SoNode
{
  SO_NODE_HEADER(FdAnimationInfo);

public:
  FdAnimationInfo();

  static void init();

  // Dummy overloading of write() to avoid any output to file
  virtual void write(SoWriteAction*) {}

  // Turn indicators on off (individually or as a group).

  SoSFBool isOn;
  SoSFBool isStepOn;
  SoSFBool isTimeOn;
  SoSFBool isProgressOn;

  // Decides which corner of the window the animation info is drawn to.

  SoSFEnum corner;
  enum {UPPERLEFT = 1, LOWERLEFT, UPPERRIGHT, LOWERRIGHT};

  // Set color values.

  SoSFColor timeColor;
  SoSFColor stepColor;
  SoSFColor progressColor;
  SoSFColor shadowColor;

  // Panel values.

  SoSFFloat time;
  SoSFEnum  step;
  SoSFFloat progress;

protected:
  virtual ~FdAnimationInfo();

  virtual void GLRender(SoGLRenderAction* action);
  virtual SbBool affectsState() const { return FALSE; }

private:
  SoGLDisplayList* fontlist;
  int fontlistcontext;
  int fieldWidth, fieldHeight;
};

#ifdef win32
#include <SoWinEnterScope.h>
#endif
#endif
