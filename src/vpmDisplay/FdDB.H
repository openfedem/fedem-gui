// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_DB_H
#define FD_DB_H

#include "vpmDisplay/FaDOF.H"

#include <array>

using FdColor = std::array<float,3>;

class FmIsRenderedBase;
class FmCamJoint;

class FdAxisCross;
class FdAnimationInfo;
class FdMechanismKit;
class FdObject;
class FdQtViewer;
class Fd2dPictureNode;
struct cameraData;

class SoPath;
class SoSeparator;
class SoExtSelection;
class SoEnvironment;
class SoDetail;
class SoLightModel;
class SoShapeHints;
class SoEventCallback;
class SbMatrix;
class LegendKit;


class FdDB
{
public:
  static void init();  // Inventor data structure initialization
  static void start(); // Creating the scenegraph etc.

  static FdQtViewer      *getViewer()           { return viewer;}
  static SoSeparator     *getExtraGraphicsSep() { return ourExtraGraphicsRoot; }
  static FdMechanismKit  *getMechanismKit()     { return mechanismKit; }
  static LegendKit       *getLegend()           { return legend; }
  static SoExtSelection  *getExtSelectionRoot() { return selectionRoot; }
  // Returns the pointer to the animation info display node in the scene graph.
  static FdAnimationInfo *getAnimInfoNode(void) { return animationInfo; }
  static void             enableDemoWarning(bool enable);

  // Follow me camera

  static void setLinkToFollow(FmIsRenderedBase* link);

  // Interactive mode Methods:

  static void updateMode();
  static void updateState(int newState);
  static void cancel();

  // Methods for communication with GUI

  static FaVec3 getPPoint(int idx, bool GlobalYesOrNo);
  static bool setPPoint(int idx, bool GlobalYesOrNo, const FaVec3& fromPoint);

  // Get and set methods:

  static cameraData getView();
  static void setView(const cameraData& cd);
  static double getCameraDistance(const FaVec3& from);
  static bool getOrthographicFlag();
  static void updateGDirection(const FaVec3& gDir);

  // View Methods:

  static void redraw();
  static void setAutoRedraw(bool setOn);

  static void zoomTo(FmIsRenderedBase* obj);
  static void zoomAll();
  static void perspectiveView();
  static void parallellView();
  static void isometricView();
  static void XYpZpYView();
  static void XYnZpYView();
  static void XZpYpZView();
  static void XZnYpZView();
  static void YZpXpZView();
  static void YZnXpZView();
  static void RotateView(double xRot, double yRot);

  static void setFogOn(bool yesOrNo);
  static void setFogVisibility(double visibility);
  static void setAntialiazingOn(bool yesOrNo);
  static void setNiceTransparency(bool nice);
  static void setSolidView(bool YesOrNo);
  static void setViewerBackground(const FdColor& color);
  static void setShading(bool on);
  static bool isShading();
  static void setFrontFaceLightOnly(bool doIt);
  static bool isFrontFaceLightOnly();
  static void setLineWidth(int width);

  // ViewFilter Methods:

  static void showRevoluteJoints(bool show);
  static void showBallJoints(bool show);
  static void showRigidJoints(bool show);
  static void showFreeJoints(bool show);
  static void showPrismaticJoints(bool show);
  static void showCylindricJoints(bool show);
  static void showCamJoints(bool show);
  static void showParts(bool show);
  static void showBeams(bool show);
  static void showPartCS(bool show);
  static void showBeamCS(bool show);
  static void showInternalPartCSs(bool show);
  static void showPartCoGCSs(bool show);
  static void showBeamTriads(bool show);
  static void showTriads(bool show);
  static void showJoints(bool show);
  static void showSprDas(bool show);
  static void showHPs(bool show);
  static void showLoads(bool show);
  static void showFeedbacks(bool show);
  static void showStickers(bool show);
  static void showRefPlanes(bool show);
  static void showSeaStates(bool show);
  static void showTires(bool show);
  static void showStrainRosettes(bool show);
  static void showContactSurfaces(bool show);

  static void setTireColor(const FdColor& color);
  static void setFEBeamSysScale(float scale);

  // Export Graphics methods:

  static bool exportEPS(const char* filename);  // Export function for EPS picture
  static bool exportIV(const char* filename);   // Export function for Inventor file
  static bool exportRGB(const char* filename);  // Export function for RGB image file
  static bool exportJPEG(const char* filename); // Export function for JPEG image file
  static bool exportBMP(const char* filename);
  static bool exportPNG(const char* filename);
  static bool exportGIF(const char* filename);
  static bool exportTIFF(const char* filename);
  static bool exportAsPicture(const char* filename, // Export function for any format,
			      const char* extension); // based on extension

  enum FileTypes {
    FD_UNKNOWN_FILE,
    FD_VRML_FILE,
    FD_IGES_FILE,
    FD_STEP_FILE,
    FD_BREP_FILE,
    FD_FCAD_FILE,
    FD_OBJ_FILE
  };
  static FileTypes getCadFileType(const std::string& fileName);

  static void showPointSelectionUI(bool isEdge, int type,
                                   const FaVec3& pickedPoint,
                                   const FaVec3& vxSnappedPPoint,
                                   const FaVec3& ppNormal,
                                   const FaVec3& axis, bool isAxisDefined,
                                   const FaVec3& origin, bool isOriginDefined,
                                   bool showDirection, int ppointIdx = 0);
  static void showDirFlipUI(const FaVec3& pickedPoint);
  static void hidePointSelectionUI();

  static FaVec3 getLineDir(SoPath* path, SoDetail* pickDetail, FdObject* pickedObject, const SbMatrix& SbObjToWorld);

private:

  //
  //   Variables :
  //////////////////////

  static FdQtViewer      * viewer;             //!< Fedem viewer
  static SoExtSelection  * selectionRoot;      //!< This node is used by window select. Placed above the mechanism kit
  static FdAxisCross     * axisCross;
  static FdAnimationInfo * animationInfo;
  static SoSeparator     * ourRoot;              //!< Root pointer
  static SoSeparator     * ourExtraGraphicsRoot; //!< Separator used to add temporary graphics under
  static FdMechanismKit  * mechanismKit;         //!< Root of all the model entities visualization
  static SoEnvironment   * fogNode;
  static LegendKit       * legend;
  static SoLightModel    * lightModel;
  static SoShapeHints    * ourShapeHint;
  static Fd2dPictureNode * demoWarning;

  // Create mode vars:
  static FdObject * firstObjectToCreateNear;
  static bool       firstCreateDirDefined;

  static FdObject * secondObjectToCreateNear;
  static bool       secondCreateDirDefined;

  static FdObject * thirdObjectToCreateNear;
  static bool       thirdCreateDirDefined;

public:
  static FmIsRenderedBase * firstPickedFmObject;
  static FmIsRenderedBase * secondPickedFmObject;
  static FmIsRenderedBase * thirdPickedFmObject;
  static FaVec3     firstCreateDirection;
  static FaVec3     secondCreateDirection;
  static FaVec3     thirdCreateDirection;

  // Box or line highligt:

  static bool usesLineHighlight;

private:
  static FmCamJoint* tempCam;
  static bool        ourAllowCompleteCamCurveSelection;

  // Smart move mode vars

  static FdObject *objectToMove;
  static FdObject *objectToMoveTo;
  static FaDOF     smartMoveDOF;
  static int       DOF;

  // Attach mode Vars

  static FdObject *objectToAttach;
  static FdObject *linkToAttachTo;

  // Detach mode vars

  static FdObject *objectToDetach;

  //
  //   Methods:
  //////////////////////

  // Event handle functions:

  static void onePickCreateEventCB(void *,SoEventCallback *eventCallbackNode);
  static void twoPickCreateEventCB(void *,SoEventCallback *eventCallbackNode);
  static void threePickCreateEventCB(void *, SoEventCallback *eventCallbackNode);
  static void createHPEventCB(void *,SoEventCallback *eventCallbackNode);
  static void appearanceEventCB(void *,SoEventCallback *eventCallbackNode);
  static void smartMoveEventCB(void *,SoEventCallback *eventCallbackNode);
  static void dragMoveEventCB(void *,SoEventCallback *eventCallbackNode);
  static void attachEventCB(void *,SoEventCallback *eventCallbackNode);
  static void detachEventCB(void *,SoEventCallback *eventCallbackNode);
  static void pickLoadPointEventCB(void *,SoEventCallback *eventCallbackNode);
  static void createSimpleSensorEventCB(void *,SoEventCallback *eventCallbackNode);
  static void createTireEventCB(void *,SoEventCallback *eventCallbackNode);
  static void createRelativeSensorEventCB(void *,SoEventCallback *eventCallbackNode);
  static void makeCamJointEventCB(void *,SoEventCallback *eventCallbackNode);
  static void pickMeasurePointEventCB(void *,SoEventCallback *eventCallbackNode);

  // utility functions

  static void objectCreator(int mode);
  static void showParts(const char* partList, bool show);
  static void showJoints(int classTypeID, bool show);
};

#endif
