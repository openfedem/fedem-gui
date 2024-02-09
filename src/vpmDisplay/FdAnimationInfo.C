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

#include <QtOpenGL>
#include <QApplication>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoElement.h>
#include <Inventor/elements/SoCacheElement.h>

#ifndef win32
#include <GL/glx.h>
#endif


#include "vpmDisplay/FdAnimationInfo.H"


SO_NODE_SOURCE(FdAnimationInfo);

void
FdAnimationInfo::init()
{
  SO_NODE_INIT_CLASS(FdAnimationInfo, SoNode, "Node");
}

FdAnimationInfo::FdAnimationInfo()
{
  SO_NODE_CONSTRUCTOR(FdAnimationInfo);

  SO_NODE_ADD_FIELD(isOn, (0));
  SO_NODE_ADD_FIELD(isStepOn, (1));
  SO_NODE_ADD_FIELD(isTimeOn, (1));
  SO_NODE_ADD_FIELD(isProgressOn, (1));
  SO_NODE_ADD_FIELD(corner, (3));
  SO_NODE_ADD_FIELD(timeColor, (1.0, 1.0, 1.0));
  SO_NODE_ADD_FIELD(stepColor, (1.0, 1.0, 1.0));
  SO_NODE_ADD_FIELD(progressColor, (1.0, 1.0, 0.0));
  SO_NODE_ADD_FIELD(shadowColor, (0.1f, 0.1f, 0.1f));
  SO_NODE_ADD_FIELD(time, (0.0));
  SO_NODE_ADD_FIELD(step, (0));
  SO_NODE_ADD_FIELD(progress, (0.0));

  this->fontlist = NULL;
  this->fontlistcontext = -1;
}

FdAnimationInfo::~FdAnimationInfo()
{
  if (this->fontlist) this->fontlist->unref();
}

void
FdAnimationInfo::write(SoWriteAction *)//writeAction)
{
  // This is a dummy function made to avoid any output when this node
  // is writing itself to a file.
  //writeAction;
}

void 
FdAnimationInfo::buildFontList(SoState* state, int& fldWidth, int& fldHeight) 
{
  // Generating displaylist from windowsystem font :
  int context = SoGLCacheContextElement::get(state); 
  char timeStr[40] = "Time: 000123.456"; 
 
  if (this->fontlist && context == this->fontlistcontext) { 
    QFontMetrics fm((qApp->font())); 
    fldWidth = fm.horizontalAdvance(timeStr);
    fldHeight = fm.height(); 
    return; 
  } 

  if (this->fontlist && context != this->fontlistcontext) { 
    this->fontlist->unref(); 
  } 
   
  this->fontlist = 
    new SoGLDisplayList(state, SoGLDisplayList::DISPLAY_LIST, 96); 
  this->fontlist->ref(); 
   
  this->fontlistcontext = context; 
  
#ifdef win32
  //  wglUseFontBitmaps(qApp->font().handle(), 32, 96, this->fontlistBase);

 //wglUseFontBitmaps(qt_display_dc(), 32, 96, this->fontlist->getFirstIndex()); 
  wglUseFontBitmaps(GetDC(0), 32, 96, this->fontlist->getFirstIndex()); 
#else
  // BUG: For the time being I disable this, which does not exists on Qt6
  // anymore
  // glXUseXFont(qApp->font().handle(), 32, 96, this->fontlist->getFirstIndex()); 
#endif
  // Find text field dimensions.
  QFontMetrics fm((qApp->font()));
  fldWidth = fm.horizontalAdvance(timeStr);
  fldHeight = fm.height();
}


void
FdAnimationInfo::GLRender(SoGLRenderAction * action) 
{
  SoCacheElement::invalidate(action->getState());

  char timeStr[40] = "Time: 000123.456";
  char stepStr[40] = "Step:      1";
  char space[] = "       ";

  if(!this->isOn.getValue()) return;
  unsigned int lines = 0;
  if(this->isStepOn.getValue()) lines++;
  if(this->isTimeOn.getValue()) lines++;
  if(this->isProgressOn.getValue()) lines++;
  if(!lines) return;
  
  this->buildFontList(action->getState(), this->fieldWidth, this->fieldHeight); 

  unsigned int nrChars = strlen(timeStr);

  GLfloat depthrange[2];
  GLint oldbase;

  // Store GL state information for the variables that we might modify.
  glPushAttrib(GL_CURRENT_BIT|GL_LIGHTING_BIT|GL_DEPTH_BUFFER_BIT|GL_TRANSFORM_BIT|
	       GL_VIEWPORT_BIT|GL_ENABLE_BIT);
  glGetFloatv(GL_DEPTH_RANGE, depthrange);
  glGetIntegerv(GL_LIST_BASE, &oldbase);

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

  glListBase(this->fontlist->getFirstIndex()-32); 

  // Find initial raster position.
  unsigned int rasterX = 0, rasterY = 0;
  if(this->corner.getValue() == FdAnimationInfo::UPPERLEFT) {
    rasterX = this->fieldWidth / nrChars;
    rasterY = view[3] - this->fieldHeight;
  }
  else if(this->corner.getValue() == FdAnimationInfo::LOWERLEFT) {
    rasterX = this->fieldWidth / nrChars;
    rasterY = lines * (this->fieldHeight + this->fieldHeight/2);
  }
  else if(this->corner.getValue() == FdAnimationInfo::UPPERRIGHT) {
    rasterX = view[2] - this->fieldWidth - this->fieldWidth/nrChars;
    rasterY = view[3] - this->fieldHeight;
  }
  else if(this->corner.getValue() == FdAnimationInfo::LOWERRIGHT) {
    rasterX = view[2] - this->fieldWidth - this->fieldWidth/nrChars;
    rasterY = lines * (this->fieldHeight + this->fieldHeight/2);
  }
     
  SbColor shadowcol = this->shadowColor.getValue();

  // Draw "Time: ..." text.
  if(this->isTimeOn.getValue()) {
    char buf[40];
    sprintf(buf, "%.6f", fabsf(this->time.getValue()));
    strcpy(&timeStr[strlen("Time:")], space);
    if(this->time.getValue() < 0)
      timeStr[strlen("Time:") + strlen(space)-(strlen(buf)-4) - 1] = '-';
    strcpy(&timeStr[strlen("Time:") + strlen(space)-(strlen(buf)-4)], buf);

    // Draw shadow.
    glColor3f(shadowcol[0], shadowcol[1], shadowcol[2]);
    glRasterPos2i(rasterX + (this->fieldWidth/nrChars)/4,
		  rasterY - this->fieldHeight/5);
    glCallLists(strlen(timeStr), GL_UNSIGNED_BYTE, (GLubyte *)timeStr);

    // Draw text.
    SbColor timecol = this->timeColor.getValue();
    glColor3f(timecol[0], timecol[1], timecol[2]);
    glRasterPos2i(rasterX, rasterY);
    glCallLists(strlen(timeStr), GL_UNSIGNED_BYTE, (GLubyte *)timeStr);

    rasterY -= this->fieldHeight + this->fieldHeight/2;
  }

  // Draw "Step: ..." text.
  if(this->isStepOn.getValue()) {
    char buf[40];
    sprintf(buf, "%d", this->step.getValue());
    strcpy(&stepStr[strlen("Step:")], space);
    strcpy(&stepStr[strlen("Step:") + strlen(space)-strlen(buf)], buf);

    // Draw shadow.
    glColor3f(shadowcol[0], shadowcol[1], shadowcol[2]);
    glRasterPos2i(rasterX + (this->fieldWidth/nrChars)/4,
		  rasterY - this->fieldHeight/5);
    glCallLists(strlen(stepStr), GL_UNSIGNED_BYTE, (GLubyte *)stepStr);

    // Draw text.
    SbColor stepcol = this->stepColor.getValue();
    glColor3f(stepcol[0], stepcol[1], stepcol[2]);
    glRasterPos2i(rasterX, rasterY);
    glCallLists(strlen(stepStr), GL_UNSIGNED_BYTE, (GLubyte *)stepStr);

    rasterY -= this->fieldHeight + this->fieldHeight/2;
  }

  // Draw progress indicator.
  if(this->isProgressOn.getValue()) {
    glColor3f(shadowcol[0], shadowcol[1], shadowcol[2]);
    rasterX += (this->fieldWidth/nrChars)/4;
    rasterY -= this->fieldHeight/5;
    // Draw outline shadow.
    glBegin(GL_LINE_LOOP);
    glVertex2i(rasterX, rasterY-this->fieldHeight);
    glVertex2i(rasterX+this->fieldWidth, rasterY-this->fieldHeight);
    glVertex2i(rasterX+this->fieldWidth, rasterY);
    glVertex2i(rasterX, rasterY);
    glEnd();
    // Draw full shadow.
    glBegin(GL_POLYGON);
    glVertex2i(rasterX, rasterY-this->fieldHeight);
    glVertex2i((int)(rasterX+this->fieldWidth * this->progress.getValue()),
	       rasterY-this->fieldHeight);
    glVertex2i((int)(rasterX+this->fieldWidth * this->progress.getValue()),
	       rasterY);
    glVertex2i(rasterX, rasterY);
    glEnd();

    SbColor progresscol = this->progressColor.getValue();
    glColor3f(progresscol[0], progresscol[1], progresscol[2]);
    rasterX -= (this->fieldWidth/nrChars)/4;
    rasterY += this->fieldHeight/5;
    // Draw outline.
    glBegin(GL_LINE_LOOP);
    glVertex2i(rasterX, rasterY-this->fieldHeight);
    glVertex2i(rasterX+this->fieldWidth, rasterY-this->fieldHeight);
    glVertex2i(rasterX+this->fieldWidth, rasterY);
    glVertex2i(rasterX, rasterY);
    glEnd();
    // Draw full progress indicator.
    glBegin(GL_POLYGON);
    glVertex2i(rasterX, rasterY-this->fieldHeight);
    glVertex2i((int)(rasterX+this->fieldWidth * this->progress.getValue()),
	       rasterY-this->fieldHeight);
    glVertex2i((int)(rasterX+this->fieldWidth * this->progress.getValue()), 
	       rasterY);
    glVertex2i(rasterX, rasterY);
    glEnd();
  }


  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  // Reset original state for OpenGL and Inventor.
  glListBase(oldbase);
  glDepthRange(depthrange[0], depthrange[1]);
  glPopAttrib();
}

SbBool
FdAnimationInfo::affectsState() const
{
  return FALSE;
}

