// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_PICKED_POINTS_H
#define FD_PICKED_POINTS_H

#include "FFaLib/FFaAlgebra/FFaMat34.H"
#include "FFaLib/FFaAlgebra/FFaVec3.H"

#include <vector>
#include <map>

typedef std::pair<long,FaVec3> FdNode;
typedef std::pair<int,long>    FdInts;

class Fd2DPoints;


class FdPickedPoints
{
public:
  static void init();

  static Fd2DPoints* getHighlighter() { return pointHighlighter; }

  // Methods used to add and remove points that are not picked points

  static long add2DPoint(const FaVec3& point);
  static void remove2DPoint(long id);

  // Node selection, used by strain rosette and cylinder spider commands

  static void selectNode(unsigned int number, int nodeID, const FaVec3& worldNodePos);
  static void deselectNode(int number = -1);
  static int getSelectedNode(size_t i = 0) { return i < ourSelectedNodes.size() ? ourSelectedNodes[i].first : -1; }
  static std::vector<int> getSelectedNodes();

  // Node set selection, used by general spider commands

  enum { ADD_SELECT, TOGGLE_SELECT, REMOVE_SELECT };
  static bool selectNodeSet(int nodeID, const FaVec3& worldNodePos,
                            int selectionType = TOGGLE_SELECT);
  static void clearNodeSet();
  static void getNodeSet(std::vector<FdNode>& nodeSet);
  static bool hasNodeSet() { return !ourNodeSet.empty(); }

  // Used by FdDB:

  static void setPP(unsigned int idx, const FaVec3& point, const FaMat34& objToWorld);
  static void setFirstPP (const FaVec3& pp, const FaMat34& objToWorld) { setPP(0,pp,objToWorld); }
  static void setSecondPP(const FaVec3& pp, const FaMat34& objToWorld) { setPP(1,pp,objToWorld); }
  static void setThirdPP (const FaVec3& pp, const FaMat34& objToWorld) { setPP(2,pp,objToWorld); }
  static void removePP(unsigned int idx);
  static void removeFirstPP()  { removePP(0); }
  static void removeSecondPP() { removePP(1); }
  static void removeThirdPP()  { removePP(2); }
  static void resetPPs();

  // Used by Fui through FdDB:

  static FaVec3 getPickedPoint(unsigned int idx, bool global);
  static void   setPickedPoint(unsigned int idx, bool global, const FaVec3& point);
  static void   getAllPickedPointsGlobal(std::vector<FaVec3>& globalPoints);
  static bool   hasPickedPoints() { return !ourEditablePPoints.empty(); }
  static const FaVec3& getFirstPickedPoint();
  static const FaVec3& getSecondPickedPoint();
  static void   setFirstPickedPoint (bool global, const FaVec3& pp) { setPickedPoint(0,global,pp); }
  static void   setSecondPickedPoint(bool global, const FaVec3& pp) { setPickedPoint(1,global,pp); }

private:

  // Picked points, used when creating and moving objects:

  static Fd2DPoints* pointHighlighter;

  struct EditablePickedPoint
  {
    EditablePickedPoint() : highlightId(-1) {}
    long    highlightId;
    FaVec3  globPoint;
    FaMat34 objectToWorldMatrix;
    FaMat34 worldToObjectMatrix;
  };

  static std::vector<EditablePickedPoint> ourEditablePPoints;

  static std::vector<FdInts> ourSelectedNodes; // NodeID, HighlightID

  static std::map<int,FdNode> ourNodeSet; // NodeID, HighlightID, WorldPosition
};

#endif
