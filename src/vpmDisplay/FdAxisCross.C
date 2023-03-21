// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

///////////////////
//
//        This node was made by
//  Morten Eriksen from Systems in Motion, <mortene@sim.no>
//           1997/2001
//
/////////////////

#include <QtOpenGL>
#include <Inventor/SbRotation.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoCacheElement.h>

#include <math.h>
#include "vpmDisplay/FdAxisCross.H"


const GLdouble FIELD_OF_VIEW = 45;

static GLubyte xbmp[] = {
  0x11,0x11,0x0a,0x04,0x0a,0x11,0x11,
};
static GLubyte ybmp[] = {
  0x04,0x04,0x04,0x04,0x0a,0x11,0x11,
};
static GLubyte zbmp[] = {
  0x1f,0x10,0x08,0x04,0x02,0x01,0x1f,
};


SO_NODE_SOURCE(FdAxisCross);


void FdAxisCross::init()
{
  SO_NODE_INIT_CLASS(FdAxisCross, SoShape, "Shape");
}


FdAxisCross::FdAxisCross()
{
  SO_NODE_CONSTRUCTOR(FdAxisCross);

  SO_NODE_ADD_FIELD(isCrossOn, (1));
  SO_NODE_ADD_FIELD(isGravityOn, (1));
  SO_NODE_ADD_FIELD(gravityVector, (0, 0, -1));
  SO_NODE_ADD_FIELD(pixelArea, (100));
  SO_NODE_ADD_FIELD(corner, (4));
  SO_NODE_ADD_FIELD(crossColor, (1.0, 1.0, 1.0));
  SO_NODE_ADD_FIELD(textColor, (1.0, 1.0, 1.0));
  SO_NODE_ADD_FIELD(gravityColor, (0.125, 0.3125, 0.5));
}


/*!
  Sets the viewing frustum for the camera to the wanted perspective projection.
*/

static void setPerspective(GLdouble vertFOV, GLdouble aspectratio,
                           GLdouble nearPlane, GLdouble farPlane)
{
  GLdouble minx, maxx, miny, maxy;
  maxy = nearPlane * tan(vertFOV*M_PI/360.0);
  miny = -maxy;
  minx = miny * aspectratio;
  maxx = maxy * aspectratio;
  glFrustum(minx, maxx, miny, maxy, nearPlane, farPlane);
}


/*!
  Transforms a point by a matrix.
*/

static void transform_point(GLdouble out[4], const GLdouble m[16], const GLdouble in[4])
{
#define M(row,col) m[col*4+row]
  for (int i = 0; i < 4; i++)
    out[i] = M(i,0)*in[0] + M(i,1)*in[1] + M(i,2)*in[2] + M(i,3)*in[3];
#undef M
}


/*!
  Projects a point from object coordinates to window coordinates.
*/

static void doProject(GLdouble objx, GLdouble objy, GLdouble objz,
                      const GLdouble model[16], const GLdouble proj[16],
                      const GLint viewport[4],
                      GLdouble* winx, GLdouble* winy, GLdouble* winz)
{
  GLdouble in[4] = { objx, objy, objz, 1.0 };
  GLdouble out[4];

  transform_point(out,model,in);
  transform_point(in,proj,out);

  *winx = viewport[0] + 0.5*(1.0+in[0]/in[3])*viewport[2];
  *winy = viewport[1] + 0.5*(1.0+in[1]/in[3])*viewport[3];
  *winz =               0.5*(1.0+in[2]/in[3]);
}


void FdAxisCross::GLRender(SoGLRenderAction* action)
{
  SoCacheElement::invalidate(action->getState());

  // The following element-access statements _must_ be present, to
  // make sure rendering *always* happen.  If we don't set up
  // dependencies to all camera-related elements like this, the node
  // might not be traversed under certain circumstances due to
  // view-frustum culling or rendercaching.
  SoState* state = action->getState();
  SoViewVolumeElement::get(state);
  SoViewportRegionElement::get(state);

  if (!this->isCrossOn.getValue() && !this->isGravityOn.getValue())
    return;

  GLfloat depthrange[2];
  GLdouble projectionmatrix[16];
  GLdouble xpos[3], ypos[3], zpos[3];
  GLdouble proj[16];
  GLint view[4];
  GLdouble model[16];

  // Store GL state information for the variables that we might modify.
  glPushAttrib(GL_CURRENT_BIT|GL_LIGHTING_BIT|GL_DEPTH_BUFFER_BIT|GL_TRANSFORM_BIT|
               GL_VIEWPORT_BIT|GL_LINE_BIT|GL_ENABLE_BIT);
  glGetFloatv(GL_DEPTH_RANGE, depthrange);
  glGetDoublev(GL_PROJECTION_MATRIX, projectionmatrix);

  glDepthFunc(GL_ALWAYS);
  glDepthMask(GL_TRUE);
  glDepthRange(0, 0);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);

  glGetIntegerv(GL_VIEWPORT, view);

  unsigned short pixarea = this->pixelArea.getValue();

  switch (this->corner.getValue()) {
  case UPPERLEFT:
    glViewport(0, view[3]-pixarea-1, pixarea, pixarea);
    break;
  case LOWERLEFT:
    glViewport(0, 0, pixarea, pixarea);
    break;
  case UPPERRIGHT:
    glViewport(view[2]-pixarea-1, view[3]-pixarea-1, pixarea, pixarea);
    break;
  case LOWERRIGHT:
    glViewport(view[2]-pixarea-1, 0, pixarea, pixarea);
    break;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // NB! Need a check if we want to use orthographic projection, which
  // in case we have to write a "setOrthogonal" method aswell.
  setPerspective(FIELD_OF_VIEW, 1.0, 0.1, 10.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glGetDoublev(GL_MODELVIEW_MATRIX, model);

  model[12] = 0;
  model[13] = -0.2;
  model[14] = -3.5;

  glLoadMatrixd(model);

  // Gravity vector.
  SbVec3f gvec = this->gravityVector.getValue();

  if(!gvec.equals(SbVec3f(0, 0, 0), 0.01f)) {
    glPushMatrix();
    SbRotation grot(SbVec3f(1,0,0), gvec);
    SbVec3f gaxis; float radians, angle;
    grot.getValue(gaxis, radians);
    angle = 180*radians/M_PI;

    glRotatef(angle, gaxis[0], gaxis[1], gaxis[2]);
    glScalef(2, 1, 1);

    if(this->isGravityOn.getValue()) {
      glLineWidth(3.0);
      SbColor gravitycol = this->gravityColor.getValue();
      glColor3f(gravitycol[0], gravitycol[1], gravitycol[2]);
      glBegin(GL_LINES);
      glVertex3f(0, 0, 0);
      glVertex3f(0.5, 0, 0);
      glEnd();
      glDisable(GL_CULL_FACE);
      glBegin(GL_TRIANGLES);
      glVertex3f(0.5f, 0, 0);
      glVertex3f(0.5f-0.5f/3, +0.5f/4, 0);
      glVertex3f(0.5f-0.5f/3, -0.5f/4, 0);
      glVertex3f(0.5f, 0, 0);
      glVertex3f(0.5f-0.5f/3, 0, +0.5f/4);
      glVertex3f(0.5f-0.5f/3, 0, -0.5f/4);
      glEnd();
      glBegin(GL_QUADS);
      glVertex3f(0.5f-0.5f/3, +0.5f/4, 0);
      glVertex3f(0.5f-0.5f/3, 0, +0.5f/4);
      glVertex3f(0.5f-0.5f/3, -0.5f/4, 0);
      glVertex3f(0.5f-0.5f/3, 0, -0.5f/4);
      glEnd();
    }

    glPopMatrix();
  }

  // Axes.
  if(this->isCrossOn.getValue()) {
    glLineWidth(1.0);
    SbColor crosscol = this->crossColor.getValue();
    glColor3f(crosscol[0], crosscol[1], crosscol[2]);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);
    glEnd();
  }

  // Axis notation letters.
  glGetDoublev(GL_PROJECTION_MATRIX, proj);
  doProject(1,0,0, model, proj, view, &xpos[0], &xpos[1], &xpos[2]);
  doProject(0,1,0, model, proj, view, &ypos[0], &ypos[1], &ypos[2]);
  doProject(0,0,1, model, proj, view, &zpos[0], &zpos[1], &zpos[2]);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0, view[2], 0, view[3], -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLint unpack;
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  SbColor textcol = this->textColor.getValue();
  glColor3f(textcol[0], textcol[1], textcol[2]);

  if(this->isCrossOn.getValue()) {
    glRasterPos2d(xpos[0], xpos[1]);
    glBitmap(8, 7, 0, 0, 0, 0, xbmp);
    glRasterPos2d(ypos[0], ypos[1]);
    glBitmap(8, 7, 0, 0, 0, 0, ybmp);
    glRasterPos2d(zpos[0], zpos[1]);
    glBitmap(8, 7, 0, 0, 0, 0, zbmp);
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);
  glPopMatrix();

  // Reset original state for OpenGL and Inventor.
  glDepthRange(depthrange[0], depthrange[1]);
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(projectionmatrix);
  glMatrixMode(GL_MODELVIEW);
  glPopAttrib();
}


void FdAxisCross::computeBBox(SoAction* action, SbBox3f& box, SbVec3f& center)
{
  SoCacheElement::invalidate(action->getState());

  if (!(isCrossOn.getValue() && isGravityOn.getValue()))
    return;

  // The following element-access statements _must_ be present, to
  // make sure rendering *always* happen.  If we don't set up
  // dependencies to all camera-related elements like this, the node
  // might not be traversed under certain circumstances due to
  // view-frustum culling or bbox-caching.
  SoState* state = action->getState();
  const SbViewVolume& vv = SoViewVolumeElement::get(state);
  SoViewportRegionElement::get(state);

  // Construct a tiny bounding box right in front of the camera, to
  // avoid the node being view-frustum culled.
  center = vv.getPlanePoint(1.0f, SbVec2f(0.5f, 0.5f));
  box.extendBy(center);
  const float UNIT_SIZE = 0.001f;
  box.extendBy(center + SbVec3f(UNIT_SIZE, 0, 0));
  box.extendBy(center + SbVec3f(0, UNIT_SIZE, 0));
  box.extendBy(center + SbVec3f(0, 0, UNIT_SIZE));
  box.extendBy(center + SbVec3f(-UNIT_SIZE, 0, 0));
  box.extendBy(center + SbVec3f(0, -UNIT_SIZE, 0));
  box.extendBy(center + SbVec3f(0, 0, -UNIT_SIZE));
}


void FdAxisCross::generatePrimitives(SoAction*)
{
  // This is an abstract method in our immediate superclass; SoShape,
  // so it must be overridden. It is empty because conceptually no
  // "common" primitives are used for rendering this node -- it is to
  // be viewed as a specific "native" primitive by itself.
}
