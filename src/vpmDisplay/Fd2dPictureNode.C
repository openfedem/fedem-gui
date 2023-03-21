// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

///////////////////
//
//        This node was made by
//  Morten Eriksen from Norsk Simulator Senter
//           4.Dec.1996
//
/////////////////

#include <QtOpenGL>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoElement.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>

#include "vpmDisplay/Fd2dPictureNode.H"

SO_NODE_SOURCE(Fd2dPictureNode);


void Fd2dPictureNode::init()
{
  SO_NODE_INIT_CLASS(Fd2dPictureNode, SoNode, "Node");
}


Fd2dPictureNode::Fd2dPictureNode()
{
  SO_NODE_CONSTRUCTOR(Fd2dPictureNode);

  SO_NODE_ADD_FIELD(position, (0.0, 0.0));
  SO_NODE_ADD_FIELD(scale, (1.0, 1.0));
  SO_NODE_ADD_FIELD(isOn, (1));

  myPixeldata = 0;
  myPicturewidth = 0;
  myPictureheight = 0;
}


void Fd2dPictureNode::setPixmapData(unsigned char* pixls, int width, int height)
{
  myPixeldata = pixls;
  myPicturewidth = width;
  myPictureheight = height;
}


void Fd2dPictureNode::flipBuffer(unsigned char* buffer, int width, int height)
{
  if (!(buffer && width > 0 && height > 0))
    return;

  // Flip all pixels around the y-axis to make the pixel buffer OpenGL-compliant

  long int j, w4 = width*4, topoffset, botoffset;
  for (int i = 0; i < height/2; i++) {
    topoffset = i*w4;
    botoffset = (height-i-1)*w4;
    for (j = 0; j < w4; j ++)
      std::swap(buffer[topoffset+j],buffer[botoffset+j]);
  }
}


void Fd2dPictureNode::computeBBox(SoAction* action, SbBox3f& box, SbVec3f& center)
{
  if(!this->isOn.getValue()) return;

  SoState* state = action->getState();
  const SbViewVolume& vv = SoViewVolumeElement::get(state);

  // Construct a tiny bounding box a long way behind  the camera, to
  // make sure the node is rendered after every transparent node in the graph.

  center = vv.getPlanePoint(-1.0f, SbVec2f(0.5f, 0.5f));
  box.extendBy(center);
  const float UNIT_SIZE = 0.001f;
  box.extendBy(center + SbVec3f(UNIT_SIZE, 0, 0));
  box.extendBy(center + SbVec3f(0, UNIT_SIZE, 0));
  box.extendBy(center + SbVec3f(0, 0, UNIT_SIZE));
  box.extendBy(center + SbVec3f(-UNIT_SIZE, 0, 0));
  box.extendBy(center + SbVec3f(0, -UNIT_SIZE, 0));
  box.extendBy(center + SbVec3f(0, 0, -UNIT_SIZE));
}


void Fd2dPictureNode::GLRender(SoGLRenderAction* action)
{
  if(!this->isOn.getValue()) return;
  if(!(myPixeldata && myPicturewidth>0 && myPictureheight>0))
    return;

  SoState* state = action->getState();
  SoCacheElement::invalidate(state);

  if (action->handleTransparency(true))
    return;

  GLboolean depthenabled, depthmask, blendenabled;
  GLint depthfunc, blendsrc, blenddest, matrixmode;
  GLfloat depthrange[2], xzoom, yzoom;

  // Save the Inventor state information.
  state->push();

  // Store GL state information for the variables that we might modify.

  glGetIntegerv(GL_MATRIX_MODE, &matrixmode);

  depthenabled = glIsEnabled(GL_DEPTH_TEST);
  glGetIntegerv(GL_DEPTH_FUNC, &depthfunc);
  glGetBooleanv(GL_DEPTH_WRITEMASK, &depthmask);
  glGetFloatv  (GL_DEPTH_RANGE, depthrange);

  blendenabled = glIsEnabled(GL_BLEND);
  glGetIntegerv(GL_BLEND_SRC, &blendsrc);
  glGetIntegerv(GL_BLEND_DST, &blenddest);

  glGetFloatv(GL_ZOOM_X, &xzoom);
  glGetFloatv(GL_ZOOM_Y, &yzoom);

  // Push identity matrices on top of the matrix stacks
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, 1, 0, 1, -1, 1);

  // Set up state for drawing 2D "sprite" graphics which will always
  // lay on top of the Inventor-triggered rendering of 3D graphics.

  glDepthFunc(GL_ALWAYS);
  glDepthMask(GL_TRUE);
  glDepthRange(0, 0);
  glEnable(GL_DEPTH_TEST);

  // Make the alpha values valid.
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  // Set the scale factor.
  SbVec2f s = this->scale.getValue();
  glPixelZoom(s[0], s[1]);

  // Set the bitmap position.
  SbVec2f p = this->position.getValue();
  glRasterPos2f(p[0], p[1]);

  // Draw the image
  glDrawPixels(myPicturewidth, myPictureheight, GL_RGBA, GL_UNSIGNED_BYTE,
	       myPixeldata);

  // Reset original state for OpenGL and Inventor.
  glPixelZoom(xzoom, yzoom);


  if(!blendenabled) glDisable(blendenabled);
  glBlendFunc(blendsrc, blenddest);

  if(!depthenabled) glDisable(GL_DEPTH_TEST);
  glDepthRange(depthrange[0], depthrange[1]);
  glDepthMask(depthmask);
  glDepthFunc(depthfunc);

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(matrixmode);

  state->pop();
}
