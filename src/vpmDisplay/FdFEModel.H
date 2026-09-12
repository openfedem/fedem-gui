// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_FE_MODEL_H
#define FD_FE_MODEL_H

#include <vector>
#include <string>

#include "vpmDisplay/FdFEVisControl.H"
#include "vpmDisplay/FdFEGroupPart.H"
#include "FFaLib/FFaOperation/FFaOperation.H"
#include "FFaLib/FFaAlgebra/FFaMat34.H"

class  FFlLinkHandler;
class  FFlGroupPartCreator;
struct FFlGroupPartData;
class  SoSeparator;


class FdFEModel
{
protected:
  using VertexVec = std::vector<FaVec3>;
  using IndexVec  = std::vector<unsigned int>;
  using IntVec    = std::vector<int>;

  struct VisParams
  {
    bool doShow = false;

    FdFEVisControl::DetailType detailLevel     = FdFEVisControl::OFF;
    FdFEVisControl::DetailType lineDetailLevel = FdFEVisControl::OFF;
    FdFEVisControl::DrawStyleType drawStyle    = FdFEVisControl::SOLID_LINES;

    FFdLook look;

    bool showResults       = false;
    bool showColorResults  = false;
    bool showVertexResults = false;

    bool showTransformResults = false;
  };

public:
  FdFEModel();
  virtual ~FdFEModel();

  void updateVertexes(FFlLinkHandler* FEModel);
  void updateGroupParts(FFlGroupPartCreator* FEModel);
  void updateElementVisibility();
  void addGroupPart(FdFEGroupPartSet::GroupPartType type,
                    FFlGroupPartData* groupPartData);
  void addLineShape(const std::vector<IntVec>& indices,
                    unsigned short int pattern = 0xffff);
  virtual void deleteGroupParts(FdFEGroupPartSet::GroupPartType type) = 0;

  void updateVisControl();

  // Visual parameters interface. Reimplemented to handle the
  // Group-GroupParts according to override status.

  virtual void deleteVisualization(bool keepSpecialGraphics = false) = 0;

  virtual void show(bool doShow);

  void setVizMode(FdFEGroupPart::FdGpVizModeEnum type);
  void setDrawDetail(FdFEVisControl::DetailType detailLevel);
  void setLineDetail(FdFEVisControl::DetailType detailLevel);
  void setDrawStyle(FdFEVisControl::DrawStyleType drawStyle);
  void setLook(const FFdLook& aLook);

  void highlight(bool isToHighlight,
                 FdFEVisControl::DetailType type = FdFEVisControl::OUTLINE,
                 const FdColor& color = { 1.0f, 0.0f, 0.0f });

  virtual void showResults(bool doShow);
  virtual void showColorResults(bool doShow);
  virtual void showVertexResults(bool doShow);
  virtual void showTransformResults(bool doShow) = 0;

  // 3D Lib dep. Methods :

  virtual void setFdPointer(FdObject* backPt) = 0;

  // Transformation :

  virtual void setTransform(const FaMat34& pos) = 0;
  virtual void setTempTransform(const FaMat34& pos) = 0;
  virtual void resetTempTransform() = 0;
  virtual FaMat34 getActiveTransform() = 0;

  // Link coordinate system
  virtual void setLinkCSToggle(bool doShow) = 0;
  virtual void setInternalCSToggle(bool doShow) = 0;
  virtual void addInternalCS(const FaMat34& localMx) = 0;

  // Beam coordinate system
  virtual void setBeamCSToggle(bool doShow) = 0;

  virtual void setCorotCSToggle(bool doShow) = 0;
  virtual void setCorotCS(const FaMat34& localMx) = 0;
  virtual void setCoGCSToggle(bool doShow) = 0;

  virtual const FaMat34& getTransform() const = 0;

  FFaOperation<FaMat34>* getPosMxReadOp() { return myPosMxReadOp; }

  void setPosMxReadOp(FFaOperation<FaMat34>* readOp)
  {
    if (readOp == myPosMxReadOp) return;
    if (readOp)        readOp->ref();
    if (myPosMxReadOp) myPosMxReadOp->unref();
    myPosMxReadOp = readOp;
  }

  // Result Frames :

  virtual void selectResultFrame(int frameIdx);

  // Labels

  virtual void* addLabel(const std::string& text, const FaVec3& position) = 0;
  virtual void removeLabels(void* id = NULL) = 0;

  // Vertexes :

  virtual void setVertexes(const std::vector<FaVec3*>& vertexes) = 0;
  virtual void setVertexes(const VertexVec& vertexes) = 0;

  virtual void resetTempVxes() = 0;

  //  Shared results :

  virtual void addResultFrame(int beforeFrame = -1); // frame = -1 => at end
  virtual void deleteResultFrame(int frameIdx = -1); // frameidx = -1 => ALL

  virtual bool hasResultTransform(unsigned int frameIdx) = 0;
  virtual void setResultTransform(unsigned int frameIdx,
                                  const FaMat34& pos) = 0;

  void setFringeLegendMapping(const FFaLegendMapper& mapping);

  virtual bool hasResultDeformation(unsigned int frameIdx) = 0;
  virtual void setDeformationScale(float scale) = 0;
  virtual void setResultDeformation(unsigned int frameIdx,
                                    const VertexVec& defs) = 0;

  virtual FdFEGroupPart* createGroupPart() = 0;

  bool isHighlighted() const { return IAmHighlighted; }

protected:
  void updateLook();

  virtual void expandFrameArrayIfNeccesary(int frameIdx);

  typedef void (FdFEGroupPart::*shapeRefMethodType)(int);
  typedef void (FdFEGroupPart::*shapeRefVoidMethod)();
  void forEachGroupPart(shapeRefMethodType shapeRefMethod, int frameidx);
  void forEachGroupPart(shapeRefVoidMethod shapeRefMethod);

  template<class T> void setVisParam(T& onePieceParam,
                                     void (FdFEVisControl::*setMethod)(T),
                                     T param);

public:
  //! The group parts used for the one-piece representation
  FdFEVisControl myGroupParts;

protected:
  VisParams myGroupWiseVisParams;
  VisParams myVisParams;

  bool IAmHighlighted;
  FdFEVisControl::DetailType myDetailHighlighted;
  FdColor myHighlightColor;
  int myCurrentResultsFrame;

  FFaOperation<FaMat34>* myPosMxReadOp;
};

#endif
