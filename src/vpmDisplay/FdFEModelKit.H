// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_FE_MODEL_KIT_H
#define FD_FE_MODEL_KIT_H

#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodes/SoVertexProperty.h>

#include "vpmDisplay/FdFEModel.H"
#include "vpmDisplay/FdFEGroupPart.H"

#ifdef win32
#include <SoWinLeaveScope.h>
#endif

class SoSeparator;


class FdFEModelKit : public SoBaseKit, public FdFEModel
{
public:
  FdFEModelKit();

  static void init();

  virtual void setFdPointer(FdObject* backPt);

  void addGroupPart(FdFEGroupPartSet::GroupPartType type,
                    SoSeparator* specialGraphics);
  void deleteGroupParts(FdFEGroupPartSet::GroupPartType type);

  virtual void selectResultFrame(int frameIdx);

  // Labels

  virtual void* addLabel(const std::string& text, const FaVec3& position);
  virtual void removeLabels(void* id);

  // Vertexes :

  virtual void setVertexes(const std::vector<FaVec3*>& vertexes);
  virtual void setVertexes(const VertexVec& vertexes);
  FaVec3  getVertex(int idx) const;
  bool    hasVertexes() const { return myVertexes->vertex.getNum() > 0; }

  // Transformation :

  virtual void setTransform    (const FaMat34& pos);
  virtual void setTempTransform(const FaMat34& pos);
  virtual void resetTempTransform();
  virtual FaMat34 getActiveTransform();

  // Link coordinate systems Visual control
  virtual void setLinkCSToggle(bool doShow);
  virtual void setInternalCSToggle(bool doShow);
  virtual void addInternalCS(const FaMat34& localMx);

  // Beam coordinate systems visual control
  virtual void setBeamCSToggle(bool doShow);

  virtual void setCorotCSToggle(bool doShow);
  virtual void setCorotCS(const FaMat34& localMx);
  virtual void setCoGCSToggle(bool doShow);

  virtual const FaMat34& getTransform() const { return myTransformMx; }

  //  results :

  virtual void addResultFrame   (int beforeFrame);
  virtual void deleteResultFrame(int frameIdx);

  virtual bool hasResultTransform(unsigned int frameIdx);
  virtual void setResultTransform(unsigned int frameIdx, const FaMat34& pos);

  virtual bool hasResultDeformation(unsigned int frameIdx);
  virtual void setDeformationScale (float scale);
  virtual void setResultDeformation(unsigned int frameIdx,
                                    const VertexVec& defs);

protected:
  virtual ~FdFEModelKit();

  virtual FdFEGroupPart* createGroupPart();
  virtual void show(bool doShow);
  virtual void deleteVisualization(bool keepSpecialGraphics);
  virtual void showTransformResults(bool doShow);
  virtual void showVertexResults(bool doShow);
  virtual void resetTempVxes();

  virtual void expandFrameArrayIfNeccesary(int frameIdx);

private:
  SoVertexProperty* myVertexes;
  FaMat34 myTransformMx;
  float myDeformationScale;

  // Result frame management :

  using Vec3f = std::array<float,3>;

  struct ResultsFrame
  {
    void eraseAll() { eraseMx(); eraseVxProp(); eraseDef(); }
    void eraseVxRes() { eraseVxProp(); eraseDef(); }
    void eraseMx() { delete mx; mx = NULL; }
    void eraseVxProp();
    void eraseDef();

    std::vector<Vec3f> deformation;
    SoVertexProperty* vxProp = NULL;
    FaMat34* mx = NULL;
  };

  std::vector<ResultsFrame> myResultsFrames;

  // Results Vertexes and look handling :

  void updateResultVertexes(const ResultsFrame& frame);

  void setTempVxes(SoVertexProperty* vxes);
  void setVxFrame(unsigned int frameIdx);

  bool IAmUsingMyVertexes;

  // Results Transformation handling

  void setTransformFrame(unsigned int frameIdx);

  bool IAmUsingMyTransform;

  // Node kit definitions :

  SO_KIT_HEADER(FdFEModelKit);

public:
  SO_KIT_CATALOG_ENTRY_HEADER(toggle);
  SO_KIT_CATALOG_ENTRY_HEADER(linkSep);
  SO_KIT_CATALOG_ENTRY_HEADER(transform);
  SO_KIT_CATALOG_ENTRY_HEADER(transform2);
  SO_KIT_CATALOG_ENTRY_HEADER(coords);
  SO_KIT_CATALOG_ENTRY_HEADER(groupParts);
  SO_KIT_CATALOG_ENTRY_HEADER(vrmlGraphics);
  SO_KIT_CATALOG_ENTRY_HEADER(backPt);

  SO_KIT_CATALOG_ENTRY_HEADER(labelMaterial);
  SO_KIT_CATALOG_ENTRY_HEADER(labels);

  SO_KIT_CATALOG_ENTRY_HEADER(symbolMaterial);
  SO_KIT_CATALOG_ENTRY_HEADER(spiderSwitch);
  SO_KIT_CATALOG_ENTRY_HEADER(cgSymbolSwitch);
  SO_KIT_CATALOG_ENTRY_HEADER(symbolSwitch);
  SO_KIT_CATALOG_ENTRY_HEADER(beamCSSwitch);
  SO_KIT_CATALOG_ENTRY_HEADER(symbolSep);
  SO_KIT_CATALOG_ENTRY_HEADER(symbol);

  SO_KIT_CATALOG_ENTRY_HEADER(corotCSSwitch);
  SO_KIT_CATALOG_ENTRY_HEADER(corotCSSep);
  SO_KIT_CATALOG_ENTRY_HEADER(corotCSSymbol);

  SO_KIT_CATALOG_ENTRY_HEADER(internalCSSwitch);
};

#ifdef win32
#include <SoWinEnterScope.h>
#endif

#endif
