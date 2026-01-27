// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_GROUP_PART_H
#define FD_GROUP_PART_H

#include <vector>
#include <map>
#include <stack>

#include "FFdCadModel/FFdUtils/FFdLook.H"

class  FdFEModel;
class  FdObject;
class  FFaLegendMapper;
struct FFlGroupPartData;


class FdFEGroupPart
{
public:
  FdFEGroupPart();

  // Pointer for picking reference

  virtual void setFdPointer(FdObject* backPt) = 0;

  // On/Off :

  void addRenderRef();
  void removeRenderRef();

  enum FdGpVizModeEnum { NORMAL, SPECIAL, BOTH };
  virtual void setVizMode(FdGpVizModeEnum mode) = 0;
  virtual bool hasSpecialGraphics() const = 0;

  // Shape :

  using IntVec = std::vector<int>;
  void setFaceIndexes(bool isFace, const std::vector<IntVec>& idxs);

  void setGroupPartData(FFlGroupPartData* groupPartData,
                        unsigned short int linePattern = 0xffff);
  void updateElementVisibility();

  // Appearance :

  void updateOverallLook(void* id, const FFdLook& look);

  void pushOverallOverrideLook(const FFdLook& look);
  FFdLook popOverallOverrideLook();

  void pushContrastOverrideLook(const FFdLook& look);
  FFdLook popContrastOverrideLook();

  void pushHighlightOverrideLook(const FFdLook& look);
  FFdLook popHighlightOverrideLook();

  void setLineWidth(int nPoints);
  void setLinePattern(unsigned short pattern);

  // Results :

  void showResults(bool resultIsOn);

  virtual void selectResultFrame  ( int frameIdx )         = 0;
  virtual void addResultFrame     ( int beforeFrame = -1 ) = 0; // frame = -1 => at end
  virtual void deleteResultFrame  ( int frameIdx = -1  )   = 0; // frameidx = -1 => ALL
  virtual void expandFrameArrayIfNeccesary(int frameIdx)   = 0;

  void showResultLook( bool resultsIsOn );

  virtual void remapLookResults   (const FFaLegendMapper& mapping) = 0;
  virtual void remapLookResults   ()                       = 0;

  virtual bool hasResultLook      ( unsigned int frameIdx ) const = 0;

  enum lookPolicy {OVERALL, PR_FACE, PR_VERTEX , PR_FACE_VERTEX};
  virtual void setResultLook      (unsigned int frameIdx,
                                   lookPolicy lookBinding,
                                   std::vector<double>& lookValues,
                                   const FFaLegendMapper& mapping) = 0;
  virtual void deleteResultLook   ( int  frameIdx = -1 )   = 0;// frameIdx = -1 => all

protected:
  virtual ~FdFEGroupPart() {}

  FFlGroupPartData* myGroupPartData;

  // Method to be called in 3D lib dep. subclass constructor

  void initWhenConstructed();

  //
  // Protected Methods to be implemented in 3D lib dep. subclass
  //
  /////////

  virtual void setShapeIndexes(bool face, const std::vector<IntVec>& idxs) = 0;
  virtual void generateShapeIndexes() = 0;

  virtual void toggleOn          ( bool  turnOn )     = 0;
  virtual void setLook           ( const FFdLook& aLook ) = 0;
  virtual void setGouradOn       ( bool  setOn )      = 0;
  virtual void set3DLLineWidth   ( int   lineWidth )  = 0;
  virtual void set3DLLinePattern ( unsigned short pattern ) = 0;
  virtual void setLineOffsetOn   ( bool  turnOn )     = 0;

  virtual void setResultLookOn   ( bool  turnOn )     = 0;
  virtual void setResultsOn      ( bool  resultIsOn ) = 0;

private:
  void updateOverrideLook();

  std::map<void*,FFdLook> myAccumulatedLooks;
  FFdLook myAccumulatedLook;

  std::stack<FFdLook> myOverallOverrideLooks;
  std::stack<FFdLook> myContrastOverrideLooks;
  std::stack<FFdLook> myHighlightOverrideLooks;

  int myRenderRefCount;
  int myResultsOnRefCount;
  int myResultsLookOnRefCount;
  int myResultsVertexOnRefCount;

  bool IAmAFaceShape;
};

#endif
