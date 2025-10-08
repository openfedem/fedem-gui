// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef  _FD2DPOINTS_
#define  _FD2DPOINTS_
///////////////////
//
//        This node was made by 
//  Morten Eriksen from Norsk Simulator Senter
//           4.Dec.1996 
//
/////////////////

#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/SbColor.h>
#include <Inventor/SbLinear.h>

class SoState;
class SoGLRenderAction;
class FaVec3;

#ifdef win32
#include <SoWinLeaveScope.h>
#endif


class Fd2DPoints : public SoNode
{
  SO_NODE_HEADER(Fd2DPoints);

public:
  static void init();

  Fd2DPoints();

  // Override write() to avoid any output when this node is writing itself.
  void write(SoWriteAction*) override {}

  long addPoint(const FaVec3& position);
  void movePoint(long id, const FaVec3& position);
  void removePoint(long id);
  void removeAllPoints();
  void changeForgrColor(SbColor newcol);
  void changeBckgrColor(SbColor newcol);

  SoSFBool isOn;
  SoSFBool useFastMarkers;
  SoSFFloat scale;

protected:
  virtual ~Fd2DPoints();

  virtual void GLRender(SoGLRenderAction *action);
  virtual SbBool affectsState() const;

private:
  static void flipBuffer(unsigned char *buffer, int width, int height);
  void drawMarkers(SoState *state);
  void swapcols(unsigned char *from, unsigned char *to);

  struct pointnode *listhead, *listtail;
  long lastid;
  float backgrf[3];
  unsigned char backgr[3], foregr[3];
};

#ifdef win32
#include <SoWinEnterScope.h>
#endif


#endif
