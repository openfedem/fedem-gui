// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_LINK_H
#define FD_LINK_H

#include <iostream>

#include "vpmDisplay/FdBase.H"
#include "vpmDisplay/FdAnimatedBase.H"

class FmLink;
class FdFEModel;
class FdCadHandler;
class FdCadComponent;
class FaMat34;


class FdLink : public FdObject, public FdAnimatedBase
{
  Fmd_HEADER_INIT();

public:
  virtual bool updateFdAll(bool updateChildrenDisplay);
  virtual bool updateFdTopology(bool updateChildrenDisplay);
  virtual bool updateFdApperance();
  virtual bool updateFdCS();

  virtual void updateSimplifiedViz() = 0;
  virtual void updateElementVisibility();

  virtual FaVec3 findSnapPoint(const SbVec3f& pointOnObject,
                               const SbMatrix& objToWorld,
                               SoDetail* pickDetail, SoPickedPoint* pPoint);

  virtual void showCS(bool doShow);
  virtual void showCoGCS(bool doShow);
  virtual void showLinkMotion(bool doShow);
  virtual void showDeformations(bool doShow);
  virtual void setDeformationScale(double scale);
  virtual void showFringes(bool doShow);
  virtual void setFringeLegendMapping(const FFaLegendMapper& mapping);

  virtual void initAnimation();
  virtual void selectAnimationFrame(size_t frameNr);
  virtual void resetAnimation();
  virtual void deleteAnimationData();

  FaMat34 getActiveTransform() const;

  virtual void removeDisplayData();
  virtual void removeVisualizationData(bool removeCadDataToo = false);

  FdFEModel* getVisualModel() const { return myFEKit; }
  FdCadHandler* getCadHandler() const { return myCadHandler; }
  FdCadComponent* getCadComponent() const;
  void writeCad(std::ostream& out);
  bool readCad(std::istream& in);

  bool isUsingGenPartVis() const { return IAmUsingGenPartVis; }

  bool getGenPartBoundingBox(FaVec3& max, FaVec3& min) const;

protected:
  FdLink(FmLink* link);
  virtual ~FdLink();

  bool loadVrmlViz();
  bool createCadViz();

  virtual void showHighlight();
  virtual void hideHighlight();

protected:
  FdFEModel*    myFEKit;
  FdCadHandler* myCadHandler;

  void* highlightBoxId;
  bool  IAmUsingGenPartVis;
  bool  IHaveLoadedVrmlViz;
  bool  IHaveCreatedCadViz;
};

#endif
