// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_QT_VIEWER_H
#define FD_QT_VIEWER_H

#include <QtCore/QPointer>
#include <QtGui/QCursor>

#include <Inventor/Qt/viewers/SoQtViewer.h>
#include <Inventor/SbLinear.h>

#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#ifndef NO_FFU
#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#endif

class SoTransform;
class SoTimerSensor;
class SoFieldSensor;
class SoSFTime;

class FdMultiplyTransforms;

class FdQtViewer : public SoQtViewer
#ifndef NO_FFU
, public FFuQtComponentBase
#endif
{
  SOQT_OBJECT_HEADER(FdQtViewer,SoQtViewer);

public:
  enum ViewerModes {
    BASE_MODE,
    PAN_MODE,
    ROTATE_MODE,
    AUTROT_MODE,
    DOLLY_MODE,
    ZROT_MODE,
    SEEK_MODE,
    READY_TO_MIDMOUSE_ROTATE,
    MIDMOUSE_ZROT_MODE,
    MIDMOUSE_ROTATE_MODE
  };

  // Constructor/destructor

  FdQtViewer(QWidget* parent  = NULL,
             const char* name = NULL,
             SbBool buildInsideParent = TRUE,
             SoQtViewer::Type = BROWSER);
  virtual ~FdQtViewer();

  // Spin control :

  void   setAnimationEnabled(SbBool onOrOff);
  SbBool isAnimationEnabled() { return animationEnabled;}

  void disableRotation(bool doDisable){IAmAllowedToRotate = !doDisable;};

  // Set views :

  void perspectiveView();
  void parallellView();

  void anisotropicView(bool horz, float ratio = 5.0);

  void isometricView();
  void frontView();
  void sideView();
  void topView();
  void bottomView();

  void XYpZpYView();
  void XYnZpYView();
  void XZnYpZView();
  void XZpYpZView();
  void YZpXpZView();
  void YZnXpZView();

  void RotateView(double xRot, double yRot) { rotateCamera(xRot, yRot); }

  void setPosition(const SbMatrix & mx);
  void setPosition(const SbVec3f & trans);

  void setFocalDistance(double distance);
  void setOHeightOrHAngle(double heigth);

  void setRelativeTransform(SoTransform * xf);

  void setKeyRotationAngle(double rads) {myKeyRotationAngle = rads;}
  double getKeyRotationAngle() const {return myKeyRotationAngle;}

  enum TransparencyType{SIMPLE,NICE};
  void setTransparencyType( TransparencyType type);

  virtual void viewAll();
  void viewAll(SoNode * sceneRoot);

  // Get view params:

  bool     isOrthographicView();
  SbVec3f  getPos();
  void     getOrient(SbMatrix& mx);
  double   getFocalDistance();
  double   getOHeightOrHAngle();

  SbViewVolume getViewVolume();
  SbVec2s      getWindowSize();

  // User event CB :
  
  void setQtEventCB(const FFaDynCB2<QEvent*,bool&>& aDynCB) { myQtEventCB = aDynCB; }

  // Reimplementations for internal reasons :

  virtual void setSceneGraph(SoNode *newScene);
  virtual void setCamera(SoCamera *cam);

 protected:
  // SoQt port
  virtual void processEvent(QEvent* e);

  //virtual bool processEvent(QEvent *e);
  void setMouseCursor(QCursor cur);

  // Copied from fedem version SoQtViewer
  //virtual void    interpolateSeekAnimation2(float t);
  //virtual void    computeSeekFinalOrientation();

  void setSeekingMode(bool tf);

  float	      seekDistance;
  SbBool	    seekDistAsPercentage; // percentage/absolute flag
  SbBool	    computeSeekVariables;
  SbVec3f	    seekPoint, seekNormal;
  SbRotation  oldCamOrientation, newCamOrientation;
  SbVec3f	    oldCamPosition, newCamPosition;

  QPointer<QWidget>  myFocusStolenWidget;

 private:
 // Viewer state variables
  int     mode;
  SbBool  createdCursors;

  // Cursors For the viewer :
  QCursor baseCursor; 
  QCursor rotateCursor;
  QCursor dollYCursor;
  QCursor panCursor;
  QCursor ZrotCursor;
  QCursor seekCursor;

  // Variables used to compute camera movement :

  SbVec2s mousePosBut1P;     // Mouse position on LeftMousePressed
  SbVec2s mousePosKeyP;      // Mouse position on keyPressed 
  SbVec2s mousePosMoNot;     // Mouse position on mouse move
  SbVec2s mousePosPrevMoNot; // Mouse position on Previus mouse move

  FdMultiplyTransforms * myMultiplier;

  static void multXf1InvXf2( const SbVec3f & p1, const SbRotation & r1,
                             const SbVec3f & p2, const SbRotation & r2,
                             SbVec3f & pos, SbRotation & rot );

  void setView(const SbMatrix & mx);

  // Camera translation variables
  
  SbPlane focalplane;
  bool    IAmAllowedToRotate;
  double  myKeyRotationAngle;

  // Camera Movement methods :

  void setFocalPlane();
  void defineCursors();
  void panCamera();
  void rotateCamera(bool slow);
  void rotateCamera(double xRot, double yRot);
  void dollYCamera();
  void ZrotCamera(bool slow);
  void ZrotCamera(float angle);

  void interpolateSeekAnimation(float t);
  bool seekToThisPoint(const SbVec2s &mouseLocation);
  // Animation state Variables and conv. methods:
 
  SbBool	  animationEnabled, animatingZrotFlag, animatingRotFlag;
  SoTimerSensor   *animationSensor;
  double	  lastMotionTime;
  SoTransform     * myRelXf;


  SoFieldSensor	*seekAnimationSensor;
  bool   seekModeFlag;
  SbTime seekStartTime;
  float	 seekAnimTime;
  bool detailSeekFlag;
  static SoSFTime	*viewerRealTime;

  // Animation Methods :
  
  void   doSpinAnimation();
  void   stopAnimating();
  SbBool isAnimating() const { return animatingZrotFlag || animatingRotFlag; }

  // User event CB :

  FFaDynCB2<QEvent*,bool&> myQtEventCB;
};

#endif
