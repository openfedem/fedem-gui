// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

////////////////////
//
//        This node was made by
//  Morten Eriksen from Norsk Simulator Senter
//           26.Feb.1997
//
/////////////////

#include <QtOpenGL/qgl.h>
#include <QApplication>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoElement.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>

#ifdef win32
#include <windows.h>
#else
#include <GL/glx.h>
#endif

#include "vpmDisplay/FdAnimationInfo.H"


SO_NODE_SOURCE(FdAnimationInfo);


FdAnimationInfo::FdAnimationInfo() : fontlist(NULL), fontlistcontext(-1)
{
  SO_NODE_CONSTRUCTOR(FdAnimationInfo);

  SO_NODE_ADD_FIELD(isOn, (0));
  SO_NODE_ADD_FIELD(isStepOn, (1));
  SO_NODE_ADD_FIELD(isTimeOn, (1));
  SO_NODE_ADD_FIELD(isProgressOn, (1));
  SO_NODE_ADD_FIELD(corner, (UPPERRIGHT));
  SO_NODE_ADD_FIELD(timeColor, (1.0, 1.0, 1.0));
  SO_NODE_ADD_FIELD(stepColor, (1.0, 1.0, 1.0));
  SO_NODE_ADD_FIELD(progressColor, (1.0, 1.0, 0.0));
  SO_NODE_ADD_FIELD(shadowColor, (0.1f, 0.1f, 0.1f));
  SO_NODE_ADD_FIELD(time, (0.0));
  SO_NODE_ADD_FIELD(step, (0));
  SO_NODE_ADD_FIELD(progress, (0.0));

  fieldWidth = fieldHeight = 0;
}


FdAnimationInfo::~FdAnimationInfo()
{
  if (fontlist) fontlist->unref();
}


void FdAnimationInfo::init()
{
  SO_NODE_INIT_CLASS(FdAnimationInfo, SoNode, "Node");
}


void FdAnimationInfo::GLRender(SoGLRenderAction* action)
{
  SoCacheElement::invalidate(action->getState());

  if (!isOn.getValue()) return;

  unsigned short int lines = 0;
  if (isStepOn.getValue()) lines++;
  if (isTimeOn.getValue()) lines++;
  if (isProgressOn.getValue()) lines++;
  if (lines == 0) return;

  char timeStr[40] = "Time: 000123.45678901234";
  char stepStr[40] = "Step:      1";

  // Generating display list from the windows system font.
  int context = SoGLCacheContextElement::get(action->getState());
  if (!fontlist || context != fontlistcontext)
  {
    if (fontlist)
      fontlist->unref();

    fontlist = new SoGLDisplayList(action->getState(),
                                   SoGLDisplayList::DISPLAY_LIST,96);
    fontlist->ref();

    fontlistcontext = context;

#ifdef win32
    wglUseFontBitmaps(GetDC(0), 32, 96, fontlist->getFirstIndex());
#else
    // For the time being I disable this, QFont::handle() does not exist in Qt6
    // glXUseXFont(qApp->font().handle(), 32, 96, fontlist->getFirstIndex());
#endif

    // Find text field dimensions.
    QFontMetrics fm(qApp->font());
    fieldHeight = fm.height();
    fieldWidth  = fm.horizontalAdvance(timeStr);
  }

  // Store GL state information for the variables that we might modify.
  glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT |
               GL_DEPTH_BUFFER_BIT | GL_TRANSFORM_BIT |
               GL_VIEWPORT_BIT | GL_ENABLE_BIT);

  GLfloat depthrange[2];
  glGetFloatv(GL_DEPTH_RANGE, depthrange);
  GLint oldbase[1];
  glGetIntegerv(GL_LIST_BASE, oldbase);

  glDepthFunc(GL_ALWAYS);
  glDepthMask(GL_TRUE);
  glDepthRange(0, 0.00001);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);

  GLint view[4];
  glGetIntegerv(GL_VIEWPORT, view);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(view[0], view[2], view[1], view[3], -1.0, 1.0);

  glListBase(fontlist->getFirstIndex()-32);

  // Find initial raster position.
  unsigned int charWidth = fieldWidth / strlen(timeStr);
  unsigned int rasterX = 0, rasterY = 0;
  switch (corner.getValue()) {
  case UPPERLEFT:
    rasterX = charWidth;
    rasterY = view[3] - 3*fieldHeight/2;
    break;
  case LOWERLEFT:
    rasterX = charWidth;
    rasterY = lines * 3*fieldHeight/2;
    break;
  case UPPERRIGHT:
    rasterX = view[2] - fieldWidth - charWidth;
    rasterY = view[3] - 3*fieldHeight/2;
    break;
  case LOWERRIGHT:
    rasterX = view[2] - fieldWidth - charWidth;
    rasterY = lines * 3*fieldHeight/2;
    break;
  }

  SbColor shadowcol = shadowColor.getValue();

  if (isTimeOn.getValue())
  {
    // Draw "Time: ..." text.
    char buf[40];
    sprintf(buf, "%.6f", time.getValue());
    size_t i = strlen(buf); // remove trailing zeroes in decimals
    while (i > 1 && buf[i-2] != '.' && buf[--i] == '0') buf[i] = '\0';
    memset(timeStr+5, ' ', 10);
    strcpy(timeStr+(time.getValue() < 0 ? 15 : 16)-strlen(buf), buf);

    rasterX += charWidth/4;
    rasterY -= fieldHeight/5;
    glColor3f(shadowcol[0], shadowcol[1], shadowcol[2]);

    // Draw shadow.
    glRasterPos2i(rasterX, rasterY);
    glCallLists(strlen(timeStr), GL_UNSIGNED_BYTE, (GLubyte*)timeStr);

    rasterX -= charWidth/4;
    rasterY += fieldHeight/5;
    SbColor timecol = timeColor.getValue();
    glColor3f(timecol[0], timecol[1], timecol[2]);

    // Draw text.
    glRasterPos2i(rasterX, rasterY);
    glCallLists(strlen(timeStr), GL_UNSIGNED_BYTE, (GLubyte*)timeStr);
  }

  if (isStepOn.getValue())
  {
    rasterY -= 3*fieldHeight/2;

    // Draw "Step: ..." text.
    char buf[40];
    sprintf(buf, "%d", step.getValue());
    memset(stepStr+5, ' ', 7);
    strcpy(stepStr+12-strlen(buf), buf);

    rasterX += charWidth/4;
    rasterY -= fieldHeight/5;
    glColor3f(shadowcol[0], shadowcol[1], shadowcol[2]);

    // Draw shadow.
    glRasterPos2i(rasterX, rasterY);
    glCallLists(strlen(stepStr), GL_UNSIGNED_BYTE, (GLubyte*)stepStr);

    rasterX -= charWidth/4;
    rasterY += fieldHeight/5;
    SbColor stepcol = stepColor.getValue();
    glColor3f(stepcol[0], stepcol[1], stepcol[2]);

    // Draw text.
    glRasterPos2i(rasterX, rasterY);
    glCallLists(strlen(stepStr), GL_UNSIGNED_BYTE, (GLubyte*)stepStr);
  }

  if (isProgressOn.getValue())
  {
    rasterY -= 6*fieldHeight/5;

    // Draw progress indicator.
    int progressWidth = fieldWidth*progress.getValue();

    rasterX += charWidth/4;
    rasterY -= fieldHeight/5;
    glColor3f(shadowcol[0], shadowcol[1], shadowcol[2]);

    // Draw outline shadow.
    glBegin(GL_LINE_LOOP);
    glVertex2i(rasterX, rasterY-fieldHeight);
    glVertex2i(rasterX+fieldWidth, rasterY-fieldHeight);
    glVertex2i(rasterX+fieldWidth, rasterY);
    glVertex2i(rasterX, rasterY);
    glEnd();

    // Draw full shadow.
    glBegin(GL_POLYGON);
    glVertex2i(rasterX, rasterY-fieldHeight);
    glVertex2i(rasterX+progressWidth, rasterY-fieldHeight);
    glVertex2i(rasterX+progressWidth, rasterY);
    glVertex2i(rasterX, rasterY);
    glEnd();

    rasterX -= charWidth/4;
    rasterY += fieldHeight/5;
    SbColor progresscol = progressColor.getValue();
    glColor3f(progresscol[0], progresscol[1], progresscol[2]);

    // Draw outline.
    glBegin(GL_LINE_LOOP);
    glVertex2i(rasterX, rasterY-fieldHeight);
    glVertex2i(rasterX+fieldWidth, rasterY-fieldHeight);
    glVertex2i(rasterX+fieldWidth, rasterY);
    glVertex2i(rasterX, rasterY);
    glEnd();

    // Draw full progress indicator.
    glBegin(GL_POLYGON);
    glVertex2i(rasterX, rasterY-fieldHeight);
    glVertex2i(rasterX+progressWidth, rasterY-fieldHeight);
    glVertex2i(rasterX+progressWidth, rasterY);
    glVertex2i(rasterX, rasterY);
    glEnd();
  }

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  // Reset original state for OpenGL and Inventor.
  glListBase(oldbase[0]);
  glDepthRange(depthrange[0], depthrange[1]);
  glPopAttrib();
}
