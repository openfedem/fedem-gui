// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <cmath>

#include <QWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QWheelEvent>
#include <QtGui/QPixmap>

#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/fields/SoSFTime.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SbViewportRegion.h>

#include "Cursors/pan.xpm"
#include "Cursors/zoom.xpm"
#include "Cursors/crossHair.xpm"
#include "Cursors/rotateXY.xpm"
#include "Cursors/rotateZ.xpm"

#include "vpmDisplay/FdMultiplyTransforms.H"
#include "vpmDisplay/FdViewer.H"
#include "vpmDisplay/qtViewers/FdQtViewer.H"

SoSFTime* FdQtViewer::viewerRealTime = NULL;

SOQT_OBJECT_SOURCE(FdQtViewer);


#ifndef NO_FFU
FFuComponentBase* FdViewer::create(QWidget* parent, bool noRotation)
{
  FdQtViewer* viewer = new FdQtViewer(parent);
  if (noRotation)
  {
    viewer->parallellView();
    viewer->disableRotation(true);
  }
  parent->setFocusProxy(viewer->getGLWidget());
  return viewer;
}
#endif


// Constructor for the viewer
FdQtViewer::FdQtViewer(QWidget* parent, const char* name, SbBool, SoQtViewer::Type)
  : SoQtViewer(parent, name, TRUE, EDITOR, TRUE)
{
  // Init local vars
  this->mode = BASE_MODE;
  this->setViewing(FALSE);
  this->createdCursors = FALSE;
  this->setSize(SbVec2s(520, 360)); //def size
  this->IAmAllowedToRotate = true;
  this->myFocusStolenWidget = 0;
  this->myKeyRotationAngle = M_PI / 12;

  // Init animation variables :

  // called when the viewer becomes visible/hidden - when hidden, make sure to
  // temporary stop ongoing animation (and restart as soon as we become visible)
  this->addVisibilityChangeCallback([](void* p, SbBool visible)
                                    {
                                      FdQtViewer* me = (FdQtViewer*)p;
                                      // only do this if we are/were spinning...
                                      if (me->isAnimating())
                                      {
                                        if (visible) // reschedule timer sensor
                                          me->animationSensor->schedule();
                                        else // hidden, so unschedule the sensor
                                          // but don't change the animatingFlag
                                          // variable to let us know we need to
                                          // turn it back on when visible....
                                          me->animationSensor->unschedule();
                                      }
                                    }, this);

  this->animationEnabled = false;
  this->animatingZrotFlag = FALSE;
  this->animatingRotFlag = FALSE;

  animationSensor = new SoTimerSensor([](void* p, SoSensor*)
                                      {
                                        ((FdQtViewer*)p)->doSpinAnimation();
                                      }, this);
  animationSensor->setInterval(1.0/60.0); // animation frame rate

  seekAnimTime = 2.0;
  detailSeekFlag = true;
  seekAnimationSensor = new SoFieldSensor([](void* p, SoSensor*)
                                          {
                                            FdQtViewer* me = (FdQtViewer*)p;
                                            // get the time difference
                                            SbTime time = viewerRealTime->getValue();
                                            double sec = (time - me->seekStartTime).getValue();
                                            if (sec == 0.0) sec = 1.0/72.0; // at least one frame (needed for first call)
                                            double t = sec / me->seekAnimTime;
                                            // check to make sure the values are correctly clipped
                                            if (t > 0.9999) t = 1.0; // this is the last interval
                                            // call subclasses to interpolate the animation
                                            me->interpolateSeekAnimation(t);
                                            if (t == 1.0)
                                            {
                                              // stops seek since this was the last interval
                                              if (me->isViewing()) me->setViewing(FALSE);
                                              me->setSeekingMode(FALSE);
                                            }
                                          }, this);

  // Set (re)draw to back buffer, change do to Open Inventor 2.1.1 porting

  this->setDrawToFrontBufferEnable(FALSE);

  if ( !viewerRealTime )
    viewerRealTime = (SoSFTime *) SoDB::getGlobalField("realTime");

  // set title

  this->setTitle("Fedem Viewer");
  this->setSeekTime(0.7f);
  myRelXf = 0;
  myMultiplier = new FdMultiplyTransforms;
  myMultiplier->ref();

  // SoQt port - hmmmm...
#ifndef NO_FFU
  this->setWidget(getGLWidget());
#endif
  this->setAutoClippingStrategy(VARIABLE_NEAR_PLANE, 0.6f,
                                [](void* p, const SbVec2f& nearfar)
                                {
                                  SbVec2f retVal = nearfar;
                                  FdQtViewer* me = (FdQtViewer*)p;
                                  if (me && me->getCamera())
                                  {
                                    float dist = 0.25f * me->getCamera()->focalDistance.getValue();
                                    if (retVal[0] > dist) retVal[0] = dist;
                                  }
                                  return retVal;
                                }, this);
}

FdQtViewer::~FdQtViewer()
{
  delete animationSensor;
  myMultiplier->unref();
}


// Process the given event to change the camera
void FdQtViewer::processEvent(QEvent* qevent)
{
  // Check if cursors need to be defined (they can be
  // defined only after the window has been mapped).
  // Receiving events guarantees that the window has been mapped.
  if (!this->createdCursors)
  {
    this->defineCursors();
    this->getGLWidget()->setCursor(this->baseCursor);
  }

  bool eventHandeled = false;
  myQtEventCB.invoke(qevent,eventHandeled);
  if (!eventHandeled)
  {
    QKeyEvent*   keyEv = NULL;
    QMouseEvent* mouseEv = NULL;

    // Lambda function scaling the key-triggered rotation angle.
    auto&& keyScale = [&keyEv](double rotAngle)
    {
      rotAngle *= 0.01;
      if (keyEv->modifiers() & Qt::ControlModifier) rotAngle *= 0.01;
      if (keyEv->modifiers() & Qt::AltModifier)     rotAngle *= 0.01;
      return rotAngle;
    };

		QPoint pointerPos = this->getGLWidget()->mapFromGlobal( QCursor::pos() );

		SbVec2s windowSize = this->getGlxSize();
		int Xrad, Yrad; // Used to test what rotation to apply: Rotate or Zrot
		bool slowRot = false;

		switch ( qevent->type() )
		{
		case QEvent::FocusOut:
			// Make sure we release the mouse grab, or else we might be grabbing wo beeing able to release it again.
			eventHandeled = true;
			break;
		case QEvent::WindowDeactivate:
			eventHandeled = true;
			break;

		case QEvent::KeyPress:
			keyEv = (QKeyEvent *) qevent;
			if ( keyEv->isAutoRepeat() && ( keyEv->key() == Qt::Key_F1
				|| keyEv->key() == Qt::Key_F2 || keyEv->key() == Qt::Key_F3
				|| keyEv->key() == Qt::Key_F4 ) )
				break;

			this->mousePosKeyP.setValue(pointerPos.x(), windowSize[1]
			- pointerPos.y());

			switch ( keyEv->key() )
			{
			case Qt::Key_Up:
				this->stopAnimating();

				if (keyEv->modifiers() & Qt::ShiftModifier)
				  this->rotateCamera(keyScale(myKeyRotationAngle), 0);
				else if ( keyEv -> modifiers() & Qt::ControlModifier ){
					this->mousePosPrevMoNot.setValue(this->mousePosMoNot[0],
						this->mousePosMoNot[1]);
					this->mousePosMoNot[1] = this->mousePosMoNot[1] + 60;
					this->panCamera();

				}
				else
					this->rotateCamera(this->myKeyRotationAngle, 0);
				eventHandeled = true;
				break;

			case Qt::Key_Down:
				this->stopAnimating();

				if (keyEv->modifiers() & Qt::ShiftModifier)
				  this->rotateCamera(-keyScale(myKeyRotationAngle), 0);
				else if ( keyEv -> modifiers() & Qt::ControlModifier ){
					this->mousePosPrevMoNot.setValue(this->mousePosMoNot[0],
						this->mousePosMoNot[1]);
					this->mousePosMoNot[1] = this->mousePosMoNot[1] - 60;
					this->panCamera();

				}
				else
					this->rotateCamera(-this->myKeyRotationAngle, 0);
				eventHandeled = true;
				break;

			case Qt::Key_Left:
				this->stopAnimating();

				if (keyEv->modifiers() & Qt::ShiftModifier)
				  this->rotateCamera(0, keyScale(myKeyRotationAngle));
				else if ( keyEv -> modifiers() & Qt::ControlModifier ){
					this->mousePosPrevMoNot.setValue( this->mousePosMoNot[0],
						this->mousePosMoNot[1]
					);
					this->mousePosMoNot[0] = this->mousePosMoNot[0] - 60;
					this->panCamera();
				}
				else if (keyEv->modifiers() & Qt::AltModifier)
					this->ZrotCamera(-M_PI / 12.0);
				else
					this->rotateCamera(0, this->myKeyRotationAngle);

				eventHandeled = true;
				break;

			case Qt::Key_Right:
				this->stopAnimating();

				if (keyEv->modifiers() & Qt::ShiftModifier)
				  this->rotateCamera(0, -keyScale(myKeyRotationAngle));
				else if ( keyEv -> modifiers() & Qt::ControlModifier ){
					this->mousePosPrevMoNot.setValue(this->mousePosMoNot[0],
						this->mousePosMoNot[1]);
					this->mousePosMoNot[0] = this->mousePosMoNot[0] + 60;
					this->panCamera();
				}
				else if (keyEv->modifiers() & Qt::AltModifier)
					this->ZrotCamera( M_PI / 12.0);
				else
					this->rotateCamera(0, -this->myKeyRotationAngle);

				eventHandeled = true;
				break;

			case Qt::Key_Z:
				this->stopAnimating();
				this->mousePosPrevMoNot.setValue(this->mousePosMoNot[0],
					this->mousePosMoNot[1]);
				if ( keyEv -> modifiers() & Qt::ShiftModifier )
					this->mousePosMoNot[1] = this->mousePosMoNot[1] + 60;
				else
					this->mousePosMoNot[1] = this->mousePosMoNot[1] - 60;
				this->dollYCamera();
				break;

			case Qt::Key_F1:
				this->stopAnimating();
				this->interactiveCountInc();
				this->mode = PAN_MODE;
				this->setFocalPlane();
				this->getGLWidget()->setCursor(this->panCursor);

				if ( !isViewing() ){
					this->setViewing(TRUE);
				}
				eventHandeled = true;
				break;

			case Qt::Key_F2:
				this->stopAnimating();
				this->interactiveCountInc();
				this->mode = DOLLY_MODE;
				this->mousePosMoNot = this->mousePosKeyP;
				this->getGLWidget()->setCursor(this->dollYCursor);
				if ( !isViewing() ){
					this->setViewing(TRUE);
				}
				eventHandeled = true;
				break;

			case Qt::Key_F3:
				if ( IAmAllowedToRotate ){
					this->stopAnimating();
					this->interactiveCountInc();
					this->mousePosMoNot = this->mousePosKeyP;
					Xrad = this->mousePosKeyP[0] - windowSize[0] / 2;
					Yrad = this->mousePosKeyP[1] - windowSize[1] / 2;
					if ( !isViewing() ){
						this->setViewing(TRUE);
					}
					if ( ( Xrad * Xrad + Yrad * Yrad )
						< ( windowSize[0] * windowSize[0] / 16 + windowSize[1]
					* windowSize[1] / 16 ) ){
						this->mode = ROTATE_MODE;
						this->getGLWidget()->setCursor(this->rotateCursor);
					}
					else{
						this->mode = ZROT_MODE;
						this->getGLWidget()->setCursor(this->ZrotCursor);
					}
					eventHandeled = true;
				}
				break;

			case Qt::Key_F4:
				if ( IAmAllowedToRotate ){
					this->stopAnimating();
					this->mode = SEEK_MODE;
					this->getGLWidget()->setCursor(this->seekCursor);
					if ( !isViewing() ){
						this->setViewing(TRUE);
					}
					this->setSeekingMode(TRUE);
					eventHandeled = true;
				}
				break;

			case Qt::Key_F5:
        if ( this->getCamera() && this->getCamera()->isOfType(SoPerspectiveCamera::getClassTypeId()) ) {
          SoPerspectiveCamera* camera = (SoPerspectiveCamera*)this->getCamera();
          float ha = camera->heightAngle.getValue();
          if (ha < 1.57f && keyEv->modifiers() & Qt::ShiftModifier)
            ha *= 1.05f;
          else if (ha > 0.001f)
            ha /= 1.05f;
          camera->heightAngle.setValue(ha);
        }
        break;

			default:
				eventHandeled = false;
				break;
			}
			break;

		case QEvent::KeyRelease:
			keyEv = (QKeyEvent *) qevent;
			if ( keyEv->isAutoRepeat() )
				break;
			switch ( keyEv->key() )
			{
			case Qt::Key_F1:
				this->interactiveCountDec();
				this->mode = BASE_MODE;
				this->getGLWidget()->setCursor(this->baseCursor);
				if ( isViewing() ){
					this->setViewing(FALSE);
				}
				eventHandeled = true;
				break;

			case Qt::Key_F2:
				this->interactiveCountDec();
				this->mode = BASE_MODE;
				this->getGLWidget()->setCursor(this->baseCursor);
				if ( isViewing() ){
					this->setViewing(FALSE);
				}
				eventHandeled = true;
				break;

			case Qt::Key_F3:
				if ( IAmAllowedToRotate ){
					// if (this->animationEnabled
					//                  && ( (SbTime::getTimeOfDay().getMsecValue() -
					//                        this->lastMotionTime ) < 100) )
					if ( this->animationEnabled
						&& ( ( SbTime::getTimeOfDay().getValue()
						- this->lastMotionTime ) < 0.1 ) ){
						if ( this->mode == ZROT_MODE )
							this->animatingZrotFlag = TRUE;
						else if ( this->mode == ROTATE_MODE )
							this->animatingRotFlag = TRUE;
						this->animationSensor->schedule();
						this->interactiveCountInc();
					}
					this->interactiveCountDec();
					this->mode = BASE_MODE;
					this->getGLWidget()->setCursor(this->baseCursor);
					if ( isViewing() )
						this->setViewing(FALSE);
					eventHandeled = true;
				}
				break;

			case Qt::Key_F4:
				if ( IAmAllowedToRotate ){
					this->mode = BASE_MODE;
					this->getGLWidget()->setCursor(this->baseCursor);
					if ( isViewing() ){
						this->setViewing(FALSE);
					}
					eventHandeled = true;
				}
				break;
			default:
				eventHandeled = false;
				break;
			}
			break;
		case QEvent::Wheel:
			if ( !this->isAnimating() ){
				QWheelEvent * wheelEv = (QWheelEvent *) qevent;

				this->mousePosPrevMoNot.setValue(this->mousePosMoNot[0], this->mousePosMoNot[1]);
				this->mousePosMoNot[1] += wheelEv->angleDelta().y() / 4;

				this->dollYCamera();
				if ( wheelEv->angleDelta().y() < 0 ){
					//this->setFocalDistance(newFd);
					// Set previous mouse pos to mid of window
					this->mousePosPrevMoNot.setValue(windowSize[0] / 2,
						windowSize[1] / 2);
					// Set mouse pos to emulate a move from the middle of the window to the real mouse position.
					Xrad = ( wheelEv->position().x() - windowSize[0] / 2 ) * 3 / 11;
					Yrad = ( wheelEv->position().y() - windowSize[1] / 2 ) * 3 / 18;
					this->mousePosMoNot = ( this->mousePosPrevMoNot + SbVec2s(
						-Xrad, Yrad) );
					this->panCamera();
				}
				eventHandeled = true;
			}

			break;
		case QEvent::MouseMove:
			if ( !this->isAnimating() ){
				mouseEv = (QMouseEvent *) qevent;
				this->mousePosPrevMoNot.setValue(this->mousePosMoNot[0],
					this->mousePosMoNot[1]);
				this->mousePosMoNot[0] = mouseEv->position().x();
				this->mousePosMoNot[1] = windowSize[1] - mouseEv->position().y();

			}

			switch ( this->mode )
			{
			case PAN_MODE:
				this->panCamera();
				//Uncomment this to stop panning when cursor exits 3d-window
				/*
				mouseEv = (QMouseEvent *) qevent;
				if (this->getXPos() > mouseEv->position().x() ||
				    this->getXPos() + this->getWidth() < mouseEv->position().x() ||
				    this->getYPos() > mouseEv->position().y() ||
				    this->getYPos() + this->getHeight() < mouseEv->position().y()) {
				this->interactiveCountDec();
				this->mode = BASE_MODE;
				this->getGLWidget()->setCursor(this->baseCursor);
				if ( isViewing() )
				this->setViewing(FALSE);
				this->ungrab();
				}*/

				eventHandeled = true;
				break;
			case ROTATE_MODE:
			case MIDMOUSE_ROTATE_MODE:
				this->lastMotionTime = SbTime::getTimeOfDay().getValue();
				if (mouseEv)
				  slowRot = mouseEv->modifiers() & Qt::ShiftModifier;
				this->rotateCamera(slowRot);
				eventHandeled = true;
				break;
			case DOLLY_MODE:
				this->dollYCamera();
				eventHandeled = true;
				break;
			case ZROT_MODE:
			case MIDMOUSE_ZROT_MODE:
				this->lastMotionTime = SbTime::getTimeOfDay().getValue();
				if (mouseEv)
				  slowRot = mouseEv->modifiers() & Qt::ShiftModifier;
				this->ZrotCamera(slowRot);
				eventHandeled = true;
				break;
			case READY_TO_MIDMOUSE_ROTATE:
				if ( IAmAllowedToRotate ){
					this->stopAnimating();
					this->interactiveCountInc();

					// First set the focal distance to "point to" what the user points to.
					SoRayPickAction pickAction(SbViewportRegion(windowSize[0],
						windowSize[1]));
					pickAction.setPoint(mousePosMoNot);
					pickAction.setPickAll(false);
					pickAction.apply(this->getSceneGraph());
					float newFd = this->getFocalDistance();
					if ( pickAction.getPickedPoint() )
					{
						SbVec3f ppoint = pickAction.getPickedPoint()->getPoint();
						newFd
							= ( ppoint - this->getCamera()->position.getValue() ).length();
						this->setFocalDistance(newFd);
					}

					Xrad = this->mousePosMoNot[0] - windowSize[0] / 2;
					Yrad = this->mousePosMoNot[1] - windowSize[1] / 2;
					if ( !isViewing() )
						this->setViewing(TRUE);
					if ( ( Xrad * Xrad + Yrad * Yrad )
						< ( windowSize[0] * windowSize[0] / 16 + windowSize[1]
					* windowSize[1] / 16 ) )
					{
						this->mode = MIDMOUSE_ROTATE_MODE;
						this->getGLWidget()->setCursor(this->rotateCursor);
					}
					else
					{
						this->mode = MIDMOUSE_ZROT_MODE;
						this->getGLWidget()->setCursor(this->ZrotCursor);
					}
					eventHandeled = true;
				}
				break;
			default:
				eventHandeled = false;
				break;
			}
			break;

		case QEvent::MouseButtonRelease:
			if ( this->mode == SEEK_MODE ){
				mouseEv = (QMouseEvent *) qevent;
				if ( mouseEv->button() == Qt::LeftButton )
				{
					this->mousePosBut1P[0] = mouseEv->position().x();
					this->mousePosBut1P[1] = windowSize[1] - mouseEv->position().y();
					this->seekToThisPoint(this->mousePosBut1P);
					eventHandeled = true;
				}
			}
			else{
				mouseEv = (QMouseEvent *) qevent;
				eventHandeled = false;
				if ( this->mode == READY_TO_MIDMOUSE_ROTATE ){
					this->mode = BASE_MODE;
				}
				else if ( this->mode == MIDMOUSE_ROTATE_MODE || this->mode
					== MIDMOUSE_ZROT_MODE ){
					// clean up
					if ( IAmAllowedToRotate )
					{
						if ( this->animationEnabled
							&& ( ( SbTime::getTimeOfDay().getValue()
							- this->lastMotionTime ) < 0.1 ) )
						{
							if ( this->mode == MIDMOUSE_ZROT_MODE )
								this->animatingZrotFlag = TRUE;
							else if ( this->mode == MIDMOUSE_ROTATE_MODE )
								this->animatingRotFlag = TRUE;
							this->animationSensor->schedule();
							this->interactiveCountInc();
						}
						this->interactiveCountDec();
						this->mode = BASE_MODE;
						this->getGLWidget()->setCursor(this->baseCursor);
						if ( isViewing() )
							this->setViewing(FALSE);
						eventHandeled = true;
					}
				}
				else if( mouseEv->button() == Qt::RightButton){
					this->interactiveCountDec();
					this->mode = BASE_MODE;
					this->getGLWidget()->setCursor(this->baseCursor);
					if ( isViewing() ){
						this->setViewing(FALSE);
					}
					eventHandeled = true;
				}
			}

			break;
		case QEvent::MouseButtonPress:
			mouseEv = (QMouseEvent *) qevent;
			if ( this->mode == SEEK_MODE ){
				if ( mouseEv->button() == Qt::LeftButton ){
					eventHandeled = true;
				}
			}
			else if ( this->mode == BASE_MODE ){
				if ( mouseEv->button() == Qt::MiddleButton ){
					this->mode = READY_TO_MIDMOUSE_ROTATE;
				}
				if(mouseEv->button() == Qt::RightButton){
					this->stopAnimating();
					this->interactiveCountInc();
					this->mode = PAN_MODE;
					this->setFocalPlane();
					this->getGLWidget()->setCursor(this->panCursor);

					if ( !isViewing() )
						this->setViewing(TRUE);
					eventHandeled = true;
				}
			}
			break;

		default:
			eventHandeled = false;
			break;

		}
	}

	// Pass upwards
	if ( !eventHandeled )
		this->SoQtViewer::processEvent(qevent);
}

////////////////////////
//
//     Viewsets :
//
/////////////////////////


void
FdQtViewer::perspectiveView()
{
  SoCamera* camera = this->getCamera();
  if ( camera && camera->isOfType(SoOrthographicCamera::getClassTypeId()) )
    {
      toggleCameraType();
      
      // Note (OIH 2012-08-30): A hard coded heightAngle of 0.61f is hardly the
      // way to go. We should enable adjusting it in General Appearance.
      ( (SoPerspectiveCamera*) ( this->getCamera() ) )->heightAngle.setValue(
          0.61f);

      // TEST (Tests the use of anisotropic view)
      //anisotropicView(false, 5.0);
    }
}

void
FdQtViewer::parallellView()
{
  SoCamera* camera = this->getCamera();
  if ( camera && camera->isOfType(SoPerspectiveCamera::getClassTypeId()) )
    toggleCameraType();
}

// Note (OIH 2012-08-30): Anisotropic viewing will basically stretch the viewport
// in the X- or Y- direction. This is useful for visualizing risers that are very
// long. Ratio is max 5.0f. This feature is currently not used anywhere in Fedem.
void FdQtViewer::anisotropicView(bool horz, float ratio)
{
  SoSceneManager* pSM = getSceneManager();
  if (pSM == NULL)
    return;
  SbViewportRegion vr(pSM->getViewportRegion());
  SbVec2s o = vr.getViewportOriginPixels();
  SbVec2s s = vr.getViewportSizePixels();
  
  // Adjust origin and size (see implementation of 
  //  SbViewportRegion::scaleWidth/Height()). I just
  //  removed the code that makes sure the viewport
  //  remains in the window.
  if (horz)
  {
    float halfWidth   = s[0] / 2.0;
    float widthCenter = o[0] + halfWidth;
    o[0] = widthCenter - ratio * halfWidth;
    s[0] *= ratio;
  }
  else
  {
    float halfHeight   = s[1] / 2.0;
    float heightCenter = o[1] + halfHeight;
    o[1] = heightCenter - ratio * halfHeight;
    s[1] *= ratio;
  }
  
  vr.setViewportPixels(o,s);
  pSM->setViewportRegion(vr);

  SoCamera* camera = this->getCamera();
  if (camera) camera->viewportMapping = SoCamera::LEAVE_ALONE;
}

void
FdQtViewer::isometricView()
{
  // if( camera->isOfType(SoPerspectiveCamera::getClassTypeId()))
  //  toggleCameraType();

  SbRotation directionRot;
  SbRotation zRot;
  SbRotation totalRot;

  directionRot.setValue(SbVec3f(-1, 1, 0), (float) M_PI_4); // pi/4
  zRot.setValue(SbVec3f(0, 0, 1), 3 * (float) M_PI_4); // 3pi/4
  totalRot = zRot * directionRot;

  SbMatrix mx;
  mx.setRotate(totalRot);

  this->setView(mx);
  //myMultiplier->rot2.setValue(totalRot); 
  //viewAll();
}

void
FdQtViewer::frontView()
{
  SoCamera* camera = this->getCamera();
  if ( camera && camera->isOfType(SoPerspectiveCamera::getClassTypeId()) )
    toggleCameraType();
  myMultiplier->rot2.setValue(SbRotation::identity());
  viewAll();
}

void
FdQtViewer::sideView()
{
  SoCamera* camera = this->getCamera();
  if ( camera && camera->isOfType(SoPerspectiveCamera::getClassTypeId()) )
    toggleCameraType();
  myMultiplier->rot2.setValue(SbRotation(SbVec3f(0, 0, 1), SbVec3f(1, 0, 0)));
  viewAll();
}

void
FdQtViewer::topView()
{
  SoCamera* camera = this->getCamera();
  if ( camera->isOfType(SoPerspectiveCamera::getClassTypeId()) )
    toggleCameraType();
  myMultiplier->rot2.setValue(SbRotation(SbVec3f(0, 0, 1), SbVec3f(0, 1, 0)));
  viewAll();
}

void
FdQtViewer::bottomView()
{
  SoCamera* camera = this->getCamera();
  if ( camera->isOfType(SoPerspectiveCamera::getClassTypeId()) )
    toggleCameraType();
  myMultiplier->rot2.setValue(SbRotation(SbVec3f(0, 0, 1), SbVec3f(0, -1, 0)));
  viewAll();
}

/*!
 View names are coded as:
 Plane to view, Which side of plane, witch direction is up.
 So XYpZpY is viewing the XY plane from above (positive Z) and with poitive Y upwards
 While XYnZnY would be from underneath, with negative Y upwards.

 Remember that the unit std::vectors of the SbMatrix are rows as written below.

 Top
 XYpZpX
 pY
 nX
 nY
 Bottom
 nZpX
 pY
 nX
 nY
 Right
 YZpXpY
 pZ
 nY
 nZ
 Left
 nXpY
 pZ
 nY
 nZ
 Back
 XZpYpX
 pZ
 nX
 nZ
 Front
 nYpX
 pZ
 nX
 nZ

 */

void
FdQtViewer::XYpZpYView()
{
  SbMatrix mx(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
  this->setView(mx);
}

void
FdQtViewer::XYnZpYView()
{
  SbMatrix mx(-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1);
  this->setView(mx);
}

void
FdQtViewer::YZpXpZView()
{
  SbMatrix mx(0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1);
  this->setView(mx);
}

void
FdQtViewer::YZnXpZView()
{
  SbMatrix mx(0, -1, 0, 0, 0, 0, 1, 0, -1, 0, 0, 0, 0, 0, 0, 1);
  this->setView(mx);
}

void
FdQtViewer::XZnYpZView()
{
  SbMatrix mx(1, 0, 0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1);
  this->setView(mx);
}

void
FdQtViewer::XZpYpZView()
{
  SbMatrix mx(-1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1);
  this->setView(mx);
}

/*!
 Calculate new trans position for the camera, based on
 the focal distance.
 The camera looks in negative Z-direction, with Y up, and X to the right.
 */

void
FdQtViewer::setView( const SbMatrix & mx )
{

  SbMatrix oldRot;
  SoCamera* camera = this->getCamera();
  camera->orientation.getValue().getValue(oldRot);

  SbVec3f oldFwd(-oldRot[2][0], -oldRot[2][1], -oldRot[2][2]);
  SbVec3f newFwd(-mx[2][0], -mx[2][1], -mx[2][2]);

  // Coin2: Changed from double to float, also removes need for cast double->float
  // in the next statement
  float fd = camera->focalDistance.getValue();

  SbVec3f newT = camera->position.getValue() + fd * oldFwd - fd * newFwd;

  // Take into account the effect of a possible pre transform :

  SbRotation newRot;
  SbVec3f newTrans;

  if ( myRelXf )
    {
      FdQtViewer::multXf1InvXf2(newT, SbRotation(mx),
          myRelXf->translation.getValue(), myRelXf->rotation.getValue(),
          newTrans, newRot);
    }
  else
    {
      newTrans = newT;
      newRot = SbRotation(mx);
    }

  myMultiplier->trans2.setValue(newTrans);
  myMultiplier->rot2.setValue(newRot);
}

void
FdQtViewer::setPosition( const SbMatrix & mx )
{
  /* This gave incorrect rotation when using the "Right View" button, TT #2838
   Anyway, the IV warning does not seem to be present any longer
   SbRotation rot(SbVec3f(1,0,0), 0);

   // Stepping around a warning from IV :

   SbVec3f src(1,1,1);
   SbVec3f dst;
   mx.multDirMatrix(src,dst);
   if (src != dst)
   rot.setValue(mx);
   */
  SbRotation rot(mx);

  myMultiplier->rot2.setValue(rot);
  myMultiplier->trans2.setValue(mx[3][0], mx[3][1], mx[3][2]);
}

/*!
 Translation manipulation used when used as control system viewer
 */

void
FdQtViewer::setPosition( const SbVec3f & trans )
{
  float maxX = 50;
  float maxY = 60;
  float maxZ = 70;

  SbVec3f trans2;

  // Limit the camera position.

  trans2[0]
      = ( trans[0] > maxX ? maxX : ( trans[0] < -maxX ? -maxX : trans[0] ) );
  trans2[1]
      = ( trans[1] > maxY ? maxY : ( trans[1] < -maxY ? -maxY : trans[1] ) );
  trans2[2]
      = ( trans[2] > maxZ ? maxZ : ( trans[2] < -maxY ? -maxY : trans[2] ) );

  myMultiplier->rot2.setValue(SbVec3f(0, 0, -1), 0);
  myMultiplier->trans2.setValue(trans2);
}

void
FdQtViewer::setFocalDistance( double distance )
{
  this->getCamera()->focalDistance.setValue(distance);
}

void
FdQtViewer::setOHeightOrHAngle( double height )
{
  SoCamera* camera = this->getCamera();
  if ( camera->isOfType(SoPerspectiveCamera::getClassTypeId()) )
    ( (SoPerspectiveCamera*) camera )->heightAngle.setValue(height);
  else if ( camera->isOfType(SoOrthographicCamera::getClassTypeId()) )
    ( (SoOrthographicCamera*) camera )->height.setValue(height);
}

void
FdQtViewer::setTransparencyType( TransparencyType type )
{
  SoGLRenderAction * ra = this->getGLRenderAction();
  if ( type == NICE )
    {
      ra->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_BLEND);
    }
  else
    {
      ra->setTransparencyType(SoGLRenderAction::SCREEN_DOOR);
    }
}

bool
FdQtViewer::isOrthographicView()
{
  return this->getCamera()->isOfType(SoOrthographicCamera::getClassTypeId());
}

void
FdQtViewer::getOrient(SbMatrix& mx)
{
  this->getCamera()->orientation.getValue().getValue(mx);
}

SbVec3f
FdQtViewer::getPos()
{
  return this->getCamera()->position.getValue();
}

SbViewVolume
FdQtViewer::getViewVolume()
{
  return this->getCamera()->getViewVolume();
}

SbVec2s
FdQtViewer::getWindowSize()
{
  return this->getGlxSize();
}

double
FdQtViewer::getFocalDistance()
{
  return getCamera()->focalDistance.getValue();
}

double
FdQtViewer::getOHeightOrHAngle()
{
  SoCamera* camera = this->getCamera();
  if ( camera->isOfType(SoOrthographicCamera::getClassTypeId()) )
    return ( (SoOrthographicCamera*) camera )->height.getValue();
  else
    return ( (SoPerspectiveCamera*) camera )->heightAngle.getValue();
}

void
FdQtViewer::setSceneGraph( SoNode *newScene )
{
  SoQtViewer::setSceneGraph(newScene);

}

void
FdQtViewer::setCamera( SoCamera *cam )
{
  SoCamera* oldCamera = this->getCamera();
  if ( oldCamera )
    {
      oldCamera->orientation.disconnect(&myMultiplier->rotation);
      oldCamera->position.disconnect(&myMultiplier->translation);
    }

  if ( cam )
    {
      cam->orientation.connectFrom(&myMultiplier->rotation);
      cam->position.connectFrom(&myMultiplier->translation);
    }
  SoQtViewer::setCamera(cam);
}

void
FdQtViewer::setRelativeTransform( SoTransform * xf )
{
  if ( xf == myRelXf )
    return;

  SbRotation newRot;
  SbVec3f newTrans;

  SoCamera* camera = this->getCamera();

  if ( xf && myRelXf )
    {
      FdQtViewer::multXf1InvXf2(camera->position.getValue(),
          camera->orientation.getValue(), xf->translation.getValue(),
          xf->rotation.getValue(), newTrans, newRot);

      myMultiplier->trans1.disconnect(&myRelXf->translation);
      myMultiplier->rot1.disconnect(&myRelXf->rotation);
      myMultiplier->trans1.connectFrom(&xf->translation);
      myMultiplier->rot1.connectFrom(&xf->rotation);

      myMultiplier->trans2.setValue(newTrans);
      myMultiplier->rot2.setValue(newRot);

      myRelXf = xf;
    }
  else if ( xf && !myRelXf )
    {
      FdQtViewer::multXf1InvXf2(camera->position.getValue(),
          camera->orientation.getValue(), xf->translation.getValue(),
          xf->rotation.getValue(), newTrans, newRot);

      myMultiplier->trans1.connectFrom(&xf->translation);
      myMultiplier->rot1.connectFrom(&xf->rotation);

      myMultiplier->trans2.setValue(newTrans);
      myMultiplier->rot2.setValue(newRot);

      myRelXf = xf;
    }
  else if ( !xf && myRelXf )
    {
      newTrans = camera->position.getValue();
      newRot = camera->orientation.getValue();

      myMultiplier->trans1.disconnect(&myRelXf->translation);
      myMultiplier->rot1.disconnect(&myRelXf->rotation);
      myMultiplier->trans1.setValue(SbVec3f(0, 0, 0));
      myMultiplier->rot1.setValue(SbRotation::identity());

      myMultiplier->trans2.setValue(newTrans);
      myMultiplier->rot2.setValue(newRot);

      myRelXf = 0;
    }
}

void
FdQtViewer::viewAll()
{
  if ( this->getCamera() != NULL )
    {
      SoNode * n = SoNode::getByName("FdQtViewerIgnoreViewAllSwitch");

      if ( n && n->isOfType(SoSwitch::getClassTypeId()) )
        ( (SoSwitch *) n )->whichChild.setValue(SO_SWITCH_NONE);

      //camera->viewAll(sceneGraph,SbViewportRegion(getGlxSize()));
      //this->viewAll(sceneGraph);
      this->viewAll(this->getSceneGraph());
      // Or this->SoQtViewer::viewAll() ?

      if ( n && n->isOfType(SoSwitch::getClassTypeId()) )
      {
        ( (SoSwitch *) n )->whichChild.setValue(SO_SWITCH_ALL);
      }
    }
}

void
FdQtViewer::viewAll( SoNode * sceneRoot )
{
  SoCamera* camera = this->getCamera();
  if ( camera != NULL )
    {
      // Check that the scene is healthy, and that a view all will not
      // destroy the camera matrices beyond repair.

      SoGetBoundingBoxAction bboxAction(this->getViewportRegion());
      bboxAction.apply(sceneRoot);
      SbBox3f box = bboxAction.getBoundingBox();
      float x, y, z;
      box.getSize(x, y, z);

      if ( x > 1e15 || y > 1e15 || z > 1e15 )
        return;

      camera->viewAll(sceneRoot, SbViewportRegion(getGlxSize()), 0.8f);

      if ( !myRelXf )
        {
          myMultiplier->trans2.setValue(camera->position.getValue());
          myMultiplier->rot2.setValue(camera->orientation.getValue());
        }
      else
        {
          SbRotation rotRes;
          SbVec3f transRes;

          FdQtViewer::multXf1InvXf2(camera->position.getValue(),
              camera->orientation.getValue(), myMultiplier->trans1.getValue(),
              myMultiplier->rot1.getValue(), transRes, rotRes);

          myMultiplier->trans2.setValue(transRes);
          myMultiplier->rot2.setValue(rotRes);
        }
    }
}

void
FdQtViewer::multXf1InvXf2( const SbVec3f & p1, const SbRotation & r1,
    const SbVec3f & p2, const SbRotation & r2, SbVec3f & pos, SbRotation & rot )
{
  SbRotation dummy1;
  SbVec3f dummy2;

  SbMatrix mx1;
  SbMatrix mx2;
  SbMatrix mxRes;

  mx1.setTransform(p1, r1, SbVec3f(1, 1, 1));
  mx2.setTransform(p2, r2, SbVec3f(1, 1, 1));

  mxRes = mx1 * mx2.inverse();

  mxRes.getTransform(pos, rot, dummy2, dummy1);
}

void
multInvXf1Xf2( const SbVec3f & p1, const SbRotation & r1, const SbVec3f & p2,
    const SbRotation & r2, SbVec3f & pos, SbRotation & rot )
{
  SbRotation dummy1;
  SbVec3f dummy2;

  SbMatrix mx1;
  SbMatrix mx2;
  SbMatrix mxRes;

  mx1.setTransform(p1, r1, SbVec3f(1, 1, 1));
  mx2.setTransform(p2, r2, SbVec3f(1, 1, 1));

  mxRes = mx1.inverse() * mx2;

  mxRes.getTransform(pos, rot, dummy2, dummy1);
}

/////////////////////////////////////
//
//    Methods for interactivly 
//    manipulating camera position:
//
/////////////////////////////////////
/*!
 Pans the camera according to mouse motion. Called while
 F1 is held down and mouse is moved.

 Uses : mousePosMoNot, mousePosPrevMoNot, windowSize, camera, focalplane
 */
void
FdQtViewer::panCamera()
{
  SoCamera* camera = this->getCamera();

  if ( camera == NULL )
    return;

  SbVec2f normMousePosMoNot;
  SbVec2f normMousePosPrevMoNot;
  SbVec2s windowSize = getGlxSize();
  SbLine line;
  SbLine prevLine;
  SbVec3f MousePos3D;
  SbVec3f prevMousePos3D;

  SbViewVolume cameraVolume;

  normMousePosMoNot.setValue(this->mousePosMoNot[0] / float(windowSize[0]),
      this->mousePosMoNot[1] / float(windowSize[1]));
  normMousePosPrevMoNot.setValue(this->mousePosPrevMoNot[0] / float(
      windowSize[0]), this->mousePosPrevMoNot[1] / float(windowSize[1]));

  // Map new mouse location into the camera focal plane

  cameraVolume = camera->getViewVolume();
  this->setFocalPlane();

  cameraVolume.projectPointToLine(normMousePosMoNot, line);
  this->focalplane.intersect(line, MousePos3D);

  cameraVolume.projectPointToLine(normMousePosPrevMoNot, prevLine);
  this->focalplane.intersect(prevLine, prevMousePos3D);

  // Move the camera by the delta 3D position amount

  SbVec3f newCameraPosition = camera->position.getValue() + ( prevMousePos3D
      - MousePos3D );

  SbMatrix mx1;
  mx1.setTransform(myMultiplier->trans1.getValue(),
      myMultiplier->rot1.getValue(), SbVec3f(1, 1, 1));

  SbVec3f newManipPos;
  mx1.inverse().multVecMatrix(newCameraPosition, newManipPos);
  float x, y, z;
  newManipPos.getValue(x, y, z);
  if ( ( fabs(x) < 1e9 ) && ( fabs(y) < 1e9 ) && ( fabs(z) < 1e9 ) )
    this->myMultiplier->trans2 = newManipPos;
}

void
FdQtViewer::rotateCamera(bool slow)
{
  // Uses : mousePosMoNot, mousePosPrevMoNot, camera

  SoCamera* camera = this->getCamera();

  if ( camera == NULL )
    return;

  if ( !IAmAllowedToRotate )
    return;

  SbVec2f movment;
  SbRotation cameraXRotation;
  SbRotation cameraYRotation;
  SbMatrix cameraRotMatrix;
  SbVec3f rotPoint;
  SbVec3f forward;
  SbVec3f cameraXAxis;
  SbVec3f cameraYAxis;
  SbVec2s windowSize = getGlxSize();
  static SbTime previousTime = viewerRealTime->getValue();
  static bool isInit = true;
  SbTime currentTime = viewerRealTime->getValue();
  float sec = float(( currentTime - previousTime ).getValue());

  movment.setValue(this->mousePosMoNot[0] - this->mousePosPrevMoNot[0],
      this->mousePosMoNot[1] - this->mousePosPrevMoNot[1]);

  double vel;
  if ( !isInit && sec != 0 )
    vel = movment.length() / sec;
  else
    {
      vel = 1;
      isInit = false;
    }

  if ( vel > 100 )
    vel = 100;

  double acc;
  acc = 0.0001 * vel * vel;
  if (slow)
    acc *= 0.01;

  movment[0] = acc * 2 * M_PI * movment[0] / windowSize[0];
  movment[1] = acc * 2 * M_PI * movment[1] / windowSize[1];

  cameraRotMatrix = this->myMultiplier->rot2.getValue();

  forward.setValue(-cameraRotMatrix[2][0], -cameraRotMatrix[2][1],
      -cameraRotMatrix[2][2]);
  cameraXAxis.setValue(cameraRotMatrix[0][0], cameraRotMatrix[0][1],
      cameraRotMatrix[0][2]);
  cameraYAxis.setValue(cameraRotMatrix[1][0], cameraRotMatrix[1][1],
      cameraRotMatrix[1][2]);

  rotPoint = this->myMultiplier->trans2.getValue() + ( forward
      * camera->focalDistance.getValue() );

  cameraXRotation.setValue(cameraXAxis, movment[1]);
  cameraYRotation.setValue(cameraYAxis, -movment[0]);

  this->myMultiplier->rot2 = this->myMultiplier->rot2.getValue()
      * cameraXRotation * cameraYRotation;

  cameraRotMatrix = this->myMultiplier->rot2.getValue();
  forward.setValue(-cameraRotMatrix[2][0], -cameraRotMatrix[2][1],
      -cameraRotMatrix[2][2]);

  this->myMultiplier->trans2 = rotPoint - forward
      * camera->focalDistance.getValue();

  previousTime = viewerRealTime->getValue();
}

void
FdQtViewer::rotateCamera( double xRot, double yRot )
{
  // Uses : mousePosMoNot, mousePosPrevMoNot, camera

  SoCamera* camera = this->getCamera();

  if ( camera == NULL )
    return;

  if ( !IAmAllowedToRotate )
    return;

  SbRotation cameraXRotation;
  SbRotation cameraYRotation;
  SbMatrix cameraRotMatrix;
  SbVec3f rotPoint;
  SbVec3f forward;
  SbVec3f cameraXAxis;
  SbVec3f cameraYAxis;

  cameraRotMatrix = this->myMultiplier->rot2.getValue();

  forward.setValue(-cameraRotMatrix[2][0], -cameraRotMatrix[2][1],
      -cameraRotMatrix[2][2]);
  cameraXAxis.setValue(cameraRotMatrix[0][0], cameraRotMatrix[0][1],
      cameraRotMatrix[0][2]);
  cameraYAxis.setValue(cameraRotMatrix[1][0], cameraRotMatrix[1][1],
      cameraRotMatrix[1][2]);

  rotPoint = this->myMultiplier->trans2.getValue() + ( forward
      * camera->focalDistance.getValue() );

  cameraXRotation.setValue(cameraXAxis, xRot);
  cameraYRotation.setValue(cameraYAxis, yRot);

  this->myMultiplier->rot2 = this->myMultiplier->rot2.getValue()
      * cameraXRotation * cameraYRotation;

  cameraRotMatrix = this->myMultiplier->rot2.getValue();
  forward.setValue(-cameraRotMatrix[2][0], -cameraRotMatrix[2][1],
      -cameraRotMatrix[2][2]);

  this->myMultiplier->trans2 = rotPoint - forward
      * camera->focalDistance.getValue();
}

// Moves the camera closer/further away from the plane 
// of interest, which is defined by the viewing normal 
// and the camera focalDistance field value.

void
FdQtViewer::dollYCamera()
{
  // uses : camera, mousePosMoNot, mousePosPrevMoNot
  SoCamera* camera = this->getCamera();
  if ( camera == NULL )
    return;

  SbMatrix cameraRotMatrix;
  SbVec3f forward;

  if ( camera->isOfType(SoOrthographicCamera::getClassTypeId()) )
    {
      SoOrthographicCamera* cam = (SoOrthographicCamera*) camera;
      cam->height = cam->height.getValue() * pow(1.005,
          ( ( this->mousePosMoNot[1] - this->mousePosPrevMoNot[1] ) ));
    }

  float focalDistance = camera->focalDistance.getValue();
  float newFocalDist = focalDistance * pow(1.005, ( ( this->mousePosMoNot[1]
      - this->mousePosPrevMoNot[1] ) ));

  cameraRotMatrix = this->myMultiplier->rot2.getValue();
  forward.setValue(-cameraRotMatrix[2][0], -cameraRotMatrix[2][1],
      -cameraRotMatrix[2][2]);

  this->myMultiplier->trans2 = this->myMultiplier->trans2.getValue()
      + ( focalDistance - newFocalDist ) * forward;

  camera->focalDistance = newFocalDist;
}

void
FdQtViewer::ZrotCamera(bool slow)
{
  // uses : windowSize, mousePosMoNot, mousePosPrevMoNot

  SbVec2s windowCenter = this->getGlxSize() / 2;

  SbVec2f centerToMPos(this->mousePosMoNot[0] - windowCenter[0],
      this->mousePosMoNot[1] - windowCenter[1]);

  SbVec2f centerToPrevMpos(this->mousePosPrevMoNot[0] - windowCenter[0],
      this->mousePosPrevMoNot[1] - windowCenter[1]);

  centerToMPos.normalize();
  centerToPrevMpos.normalize();

  double angle = (centerToMPos[0]*centerToPrevMpos[1] -
                  centerToMPos[1]*centerToPrevMpos[0]);
  if (slow)
    angle *= 0.01;

  this->ZrotCamera(angle);
}

void
FdQtViewer::ZrotCamera(double angle)
{
  SbMatrix cameraRotMatrix;

  cameraRotMatrix = myMultiplier->rot2.getValue();
  SbVec3f cameraZAxis(cameraRotMatrix[2]);

  SbRotation cameraRotIncr(cameraZAxis,angle);
  myMultiplier->rot2 = myMultiplier->rot2.getValue() * cameraRotIncr;
}

void
FdQtViewer::interpolateSeekAnimation(double t)
{

  // check if camera new and old position/orientation have already
  // been computed.

  SoCamera* camera = this->getCamera();

  if ( !camera )
    return;

  // SoQt port - what do we do here??
  //if (this->computeSeekVariables) 
  if ( true )
    {

      // save camera starting point

      this->oldCamOrientation = myMultiplier->rot2.getValue();

      // now compute new camera position and rotation

      if ( isDetailSeek() )
        {
          // Get the camera forward view std::vector

          SbMatrix mx;
          mx = camera->orientation.getValue();
          SbVec3f viewVector(-mx[2][0], -mx[2][1], -mx[2][2]);

          //get the new forward std::vector

          SbVec3f newViewVector = camera->position.getValue() - this->seekPoint;
          camera->focalDistance.setValue(newViewVector.length());

          newViewVector.normalize();

          // calculate camera's orientation change

          SbRotation changeOrient(viewVector, -newViewVector);
          if ( myRelXf )
            {
              SbRotation newGlobCamOrientation = camera->orientation.getValue()
                  * changeOrient;
              SbVec3f dummy;

              FdQtViewer::multXf1InvXf2(SbVec3f(0, 0, 0),
                  newGlobCamOrientation, myMultiplier->trans1.getValue(),
                  myMultiplier->rot1.getValue(), dummy, this->newCamOrientation);
            }
          else
            {
              this->newCamOrientation = this->oldCamOrientation * changeOrient;
            }
        }
      else
        {
          this->newCamOrientation = this->oldCamOrientation;
        }

      // SoQt port -> ???
      // this->computeSeekVariables = FALSE;
    }

  // Rotate the camera according to the animation time
  // use and ease-in ease-out approach

  double cos_t = 0.5 - 0.5*cos(t*M_PI);

  // Set camera new rotation

  myMultiplier->rot2 = SbRotation::slerp(this->oldCamOrientation,
                                         this->newCamOrientation, cos_t);
}

/*!
  Figure out and set the focal plane
*/

void
FdQtViewer::setFocalPlane()
{
  SbMatrix cameraRotMatrix;

  SoCamera* camera = this->getCamera();

  cameraRotMatrix = camera->orientation.getValue();
  SbVec3f forward(cameraRotMatrix[2]);
  forward *= -1.0f;

  SbVec3f focalPoint(camera->position.getValue());
  focalPoint += forward * camera->focalDistance.getValue();

  focalplane = SbPlane(forward, focalPoint);
}

//////////////////////////////////////
//
//    Mouse cursor control methods:
//
//////////////////////////////////////


// This routine is used to define cursors, which can 
// only be called after the window has been realized.

void
FdQtViewer::defineCursors()
{
  baseCursor = this->getGLWidget()->cursor();
  panCursor = QCursor(QPixmap(fd_pan_xpm), 8, 8);
  rotateCursor = QCursor(QPixmap(rotateXY_xpm), 8, 8);
  dollYCursor = QCursor(QPixmap(fd_zoom_xpm), 7, 7);
  ZrotCursor = QCursor(QPixmap(rotateZ_xpm), 7, 7);
  seekCursor = QCursor(QPixmap(crossHair_xpm), 13, 13);
  createdCursors = TRUE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Routine which animates the ball spinning (called by sensor).
//
// Use: private

void
FdQtViewer::doSpinAnimation()
//
////////////////////////////////////////////////////////////////////////
{
  if ( animatingRotFlag )
    rotateCamera(false);
  if ( animatingZrotFlag )
    ZrotCamera(false);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Enable/disable the animation feature of the viewer
//
// Use: publicseekAnimationSensor = new SoFieldSensor(SoQtViewer::seekAnimationSensorCB, this);
void
FdQtViewer::setAnimationEnabled( SbBool flag )
//
////////////////////////////////////////////////////////////////////////
{
  if ( animationEnabled == flag )
    return;

  animationEnabled = flag;
  if ( !animationEnabled && isAnimating() )
    stopAnimating();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Stops ongoing animation (if any)
//
// Use: public
void
FdQtViewer::stopAnimating()
//
////////////////////////////////////////////////////////////////////////
{
  if ( isAnimating() )
    {
      animatingZrotFlag = FALSE;
      animatingRotFlag = FALSE;
      animationSensor->unschedule();
      interactiveCountDec();
    }
}

/////////////////////////////
//
//  Handeling iconisation:
//
/////////////////////////////

void
FdQtViewer::setSeekingMode( bool flag )
{
  if ( flag && !isViewing() )
    return;

  // check if seek is being turned off while seek animation is happening
  if ( !flag && seekAnimationSensor->getAttachedField() )
    {
      seekAnimationSensor->detach();
      if ( seekAnimationSensor->isScheduled() )
        seekAnimationSensor->unschedule();
      interactiveCountDec();
    }

  seekModeFlag = flag;
}

bool
FdQtViewer::seekToThisPoint( const SbVec2s &mouseLocation )
{
  SoCamera* camera = this->getCamera();
  if ( camera == NULL )
    {
      setSeekingMode(false);
      return false;
    }
  // do the picking
  SbViewportRegion vpreg(getGlxSize());
  SoRayPickAction pick(vpreg);
  pick.setPoint(mouseLocation);
  pick.setRadius(3.0);
  pick.setPickAll(FALSE); // pick only the closest object
  //pick.apply(sceneRoot);
  pick.apply(this->getSceneGraph());

  // makes sure something got picked
  SoPickedPoint *pp = pick.getPickedPoint();
  if ( pp == NULL )
    {
      setSeekingMode(false);
      return false;
    }

  //
  // Get picked point and normal if detailtSeek
  //
  if ( detailSeekFlag )
    {

      seekPoint = pp->getPoint();
      seekNormal = pp->getNormal();

      // check to make sure normal points torward the camera, else
      // flip the normal around
      if ( seekNormal.dot(camera->position.getValue() - seekPoint) < 0 )
        seekNormal.negate();
    }
  //
  // else get object bounding box as the seek point and the camera
  // orientation as the normal.
  //
  else
    {
      // get center of object's bounding box
      SbViewportRegion vpreg(getGlxSize());
      SoGetBoundingBoxAction bba(vpreg);
      bba.apply(pp->getPath());
      SbBox3f bbox = bba.getBoundingBox();
      seekPoint = bbox.getCenter();

      // keep the camera oriented the same way
      SbMatrix mx;
      mx = camera->orientation.getValue();
      seekNormal.setValue(mx[2][0], mx[2][1], mx[2][2]);
    }

  //
  // now check if animation sensor needs to be scheduled
  //

  computeSeekVariables = TRUE;
  if ( seekAnimTime == 0 )
    {

      // jump to new location, no animation needed
      interpolateSeekAnimation(1.0);
    }
  else
    {
      // schedule sensor and call viewer start callbacks
      if ( !seekAnimationSensor->getAttachedField() )
        {
          seekAnimationSensor->attach(viewerRealTime);
          seekAnimationSensor->schedule();
          interactiveCountInc();
        }

      seekStartTime = viewerRealTime->getValue();
    }

  return true; // successfull
}
