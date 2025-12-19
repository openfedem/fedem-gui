// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_DB_H
#define FD_DB_H

#include <array>
#include <string>

class FaVec3;
class FmIsRenderedBase;
class FdAnimationInfo;
class FdMechanismKit;
class FdQtViewer;
struct cameraData;

class SoSeparator;
class SoExtSelection;
class LegendKit;


namespace FdDB
{
  using FdColor = std::array<float,3>;

  extern bool usesLineHighlight;

  void init();  // Inventor data structure initialization
  void start(); // Creating the scene graph, etc.

  FdQtViewer*      getViewer();
  SoSeparator*     getExtraGraphicsSep();
  FdMechanismKit*  getMechanismKit();
  LegendKit*       getLegend();
  SoExtSelection*  getExtSelectionRoot();
  FdAnimationInfo* getAnimInfoNode();

  void enableDemoWarning(bool enable);

  // Follow me camera

  void setLinkToFollow(FmIsRenderedBase* link);

  // Interactive mode methods

  void updateMode();
  void updateState(int newState);
  void cancel();

  // Methods for communication with GUI

  FaVec3 getPPoint(int idx, bool GlobalYesOrNo);
  bool setPPoint(int idx, bool GlobalYesOrNo, const FaVec3& fromPoint);

  // Get and set methods

  cameraData getView();
  void setView(const cameraData& cd);
  double getCameraDistance(const FaVec3& from);
  bool getOrthographicFlag();
  void updateGDirection(const FaVec3& gDir);
  const FaVec3& getCreateDirection(int i = 0);
  void setCreateDirection(const FaVec3& cDir, int i = 0);

  // View methods

  void redraw();
  void setAutoRedraw(bool setOn);

  void zoomTo(FmIsRenderedBase* obj);
  void zoomAll();
  void perspectiveView();
  void parallellView();
  void isometricView();
  void XYpZpYView();
  void XYnZpYView();
  void XZpYpZView();
  void XZnYpZView();
  void YZpXpZView();
  void YZnXpZView();
  void RotateView(double xRot, double yRot);

  void setFogOn(bool yesOrNo);
  void setFogVisibility(double visibility);
  void setAntialiazingOn(bool yesOrNo);
  void setNiceTransparency(bool nice);
  void setSolidView(bool YesOrNo);
  void setViewerBackground(const FdColor& color);
  void setShading(bool on);
  bool isShading();
  void setFrontFaceLightOnly(bool doIt);
  bool isFrontFaceLightOnly();
  void setLineWidth(int width);

  // View filter methods

  void showRevoluteJoints(bool show);
  void showBallJoints(bool show);
  void showRigidJoints(bool show);
  void showFreeJoints(bool show);
  void showPrismaticJoints(bool show);
  void showCylindricJoints(bool show);
  void showCamJoints(bool show);
  void showParts(bool show);
  void showBeams(bool show);
  void showPartCS(bool show);
  void showBeamCS(bool show);
  void showInternalPartCSs(bool show);
  void showPartCoGCSs(bool show);
  void showBeamTriads(bool show);
  void showTriads(bool show);
  void showJoints(bool show);
  void showSprDas(bool show);
  void showHPs(bool show);
  void showLoads(bool show);
  void showFeedbacks(bool show);
  void showStickers(bool show);
  void showRefPlanes(bool show);
  void showSeaStates(bool show);
  void showTires(bool show);
  void showStrainRosettes(bool show);
  void showContactSurfaces(bool show);

  void setTireColor(const FdColor& color);
  void setFEBeamSysScale(float scale);

  // Export graphics methods

  bool exportEPS(const char* filename);  // Export function for EPS picture
  bool exportIV(const char* filename);   // Export function for Inventor file
  bool exportRGB(const char* filename);  // Export function for RGB image file
  bool exportJPEG(const char* filename); // Export function for JPEG image file
  bool exportBMP(const char* filename);
  bool exportPNG(const char* filename);
  bool exportGIF(const char* filename);
  bool exportTIFF(const char* filename);
  bool exportAsPicture(const char* filename, // Export function for any format,
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

  FileTypes getCadFileType(const std::string& fileName);
}

#endif
