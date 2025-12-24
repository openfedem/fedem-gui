// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_PICKED_POINTS_H
#define FD_PICKED_POINTS_H

#include <vector>
#include <utility>
#include <cstddef>

class SoNode;
class FaVec3;
class FaMat34;


namespace FdPickedPoints
{
  using FdNode = std::pair<long int,FaVec3>;

  void init();

  SoNode* getHighlighter();

  // Methods used to add and remove points that are not picked points

  long int add2DPoint(const FaVec3& point);
  void remove2DPoint(long int id);

  // Node selection, used by strain rosette and cylinder spider commands

  void selectNode(size_t number, int nodeID, const FaVec3& worldNodePos);
  void deselectNode(int number = -1);
  int getSelectedNode();
  std::vector<int> getSelectedNodes();

  // Node set selection, used by general spider commands

  enum { ADD_SELECT, TOGGLE_SELECT, REMOVE_SELECT };
  bool selectNodeSet(int nodeID, const FaVec3& worldNodePos, int selectionType);
  void clearNodeSet();
  void getNodeSet(std::vector<FdNode>& nodeSet);
  bool hasNodeSet();

  // Used by FdDB:

  void setPP(size_t idx, const FaVec3& point);
  void setPP(size_t idx, const FaVec3& point, const FaMat34& objToWorld);
  void delPP(size_t idx);
  void resetPPs();

  inline void setFirstPP (const FaVec3& p) { setPP(0,p); }
  inline void setSecondPP(const FaVec3& p) { setPP(1,p); }
  inline void setThirdPP (const FaVec3& p) { setPP(2,p); }

  inline void setFirstPP (const FaVec3& p, const FaMat34& T) { setPP(0,p,T); }
  inline void setSecondPP(const FaVec3& p, const FaMat34& T) { setPP(1,p,T); }
  inline void setThirdPP (const FaVec3& p, const FaMat34& T) { setPP(2,p,T); }

  inline void removeFirstPP()  { delPP(0); }
  inline void removeSecondPP() { delPP(1); }
  inline void removeThirdPP()  { delPP(2); }

  // Used by Fui through FdDB:

  size_t hasPickedPoints();
  FaVec3 getPickedPoint(size_t idx, bool global);
  void   setPickedPoint(size_t idx, bool global, const FaVec3& point);
  void   getAllPickedPointsGlobal(std::vector<FaVec3>& globalPoints);
  const FaVec3& getFirstPickedPoint();
  const FaVec3& getSecondPickedPoint();
}

#endif
