// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdPickedPoints.H"
#include "vpmDisplay/Fd2DPoints.H"
#include "FFaLib/FFaAlgebra/FFaMat34.H"
#include "FFaLib/FFaAlgebra/FFaVec3.H"
#include <map>


namespace
{
  using FdInts = std::pair<int,long int>;
  using FdNode = FdPickedPoints::FdNode;

  struct EditablePickedPoint
  {
    long int highlightId = -1;
    FaVec3   globPoint;
    FaMat34  objectToWorldMatrix;
    FaMat34  worldToObjectMatrix;
  };

  // Picked points, used when creating and moving objects:
  std::vector<EditablePickedPoint> ourEditablePPoints;

  std::vector<FdInts>  ourSelectedNodes; // NodeID, HighlightID
  std::map<int,FdNode> ourNodeSet;       // NodeID, HighlightID, globPos

  Fd2DPoints* ourHighlighter = NULL;
}


void FdPickedPoints::init()
{
  // Set up the point highlight node
  ourHighlighter = new Fd2DPoints();
  ourHighlighter->changeForgrColor(SbColor(0.5f, 0.5f, 0.5f));
  ourHighlighter->changeBckgrColor(SbColor(1.0f, 1.0f, 1.0f));
  ourHighlighter->scale.setValue(1);

  resetPPs();
}


SoNode* FdPickedPoints::getHighlighter()
{
  return ourHighlighter;
}


long int FdPickedPoints::add2DPoint(const FaVec3& point)
{
  if (ourHighlighter)
    return ourHighlighter->addPoint(point);
  else
    return -1;
}

void FdPickedPoints::remove2DPoint(long int id)
{
  if (ourHighlighter && id > 0)
    ourHighlighter->removePoint(id);
}


size_t FdPickedPoints::hasPickedPoints()
{
  return ourEditablePPoints.size();
}


const FaVec3& FdPickedPoints::getFirstPickedPoint()
{
  if (ourEditablePPoints.size() > 0)
    return ourEditablePPoints.front().globPoint;

  static FaVec3 Null;
  return Null;
}


const FaVec3& FdPickedPoints::getSecondPickedPoint()
{
  if (ourEditablePPoints.size() > 1)
    return ourEditablePPoints[1].globPoint;

  static FaVec3 Null;
  return Null;
}


FaVec3 FdPickedPoints::getPickedPoint(size_t idx, bool global)
{
  if (idx >= ourEditablePPoints.size())
    return FaVec3();
  else if (global)
    return ourEditablePPoints[idx].globPoint;
  else
    return ourEditablePPoints[idx].worldToObjectMatrix * ourEditablePPoints[idx].globPoint;
}

void FdPickedPoints::getAllPickedPointsGlobal(std::vector<FaVec3>& globalPoints)
{
  globalPoints.clear();
  for (size_t i = 0; i < ourEditablePPoints.size(); i++)
    globalPoints.push_back(ourEditablePPoints[i].globPoint);
}

void FdPickedPoints::setPickedPoint(size_t idx, bool global,
                                    const FaVec3& point)
{
  if (idx >= ourEditablePPoints.size())
    ourEditablePPoints.resize(idx+1);

  EditablePickedPoint& newPoint = ourEditablePPoints[idx];
  FaVec3& globPoint = newPoint.globPoint;
  if (global)
    globPoint = point;
  else
    globPoint = newPoint.objectToWorldMatrix * point;

  if (ourHighlighter)
  {
    if (newPoint.highlightId == -1)
      newPoint.highlightId = ourHighlighter->addPoint(globPoint);
    else
      ourHighlighter->movePoint(newPoint.highlightId,globPoint);
  }
}


void FdPickedPoints::setPP(size_t idx, const FaVec3& point)
{
  if (idx >= ourEditablePPoints.size())
    ourEditablePPoints.resize(idx+1);

  EditablePickedPoint& newPoint = ourEditablePPoints[idx];

  newPoint.globPoint = point;
  newPoint.objectToWorldMatrix.setIdentity();

  if (ourHighlighter)
  {
    if (newPoint.highlightId == -1)
      newPoint.highlightId = ourHighlighter->addPoint(point);
    else
      ourHighlighter->movePoint(newPoint.highlightId,point);
  }
}


void FdPickedPoints::setPP(size_t idx, const FaVec3& point,
                           const FaMat34& objToWorld)
{
  if (idx >= ourEditablePPoints.size())
    ourEditablePPoints.resize(idx+1);

  EditablePickedPoint& newPoint = ourEditablePPoints[idx];

  newPoint.globPoint = point;
  newPoint.objectToWorldMatrix = objToWorld;
  for (int i = 0; i < 3; i++)
    newPoint.objectToWorldMatrix[i].normalize();
  newPoint.worldToObjectMatrix = newPoint.objectToWorldMatrix.inverse();

  if (ourHighlighter)
  {
    if (newPoint.highlightId == -1)
      newPoint.highlightId = ourHighlighter->addPoint(point);
    else
      ourHighlighter->movePoint(newPoint.highlightId,point);
  }
}


void FdPickedPoints::delPP(size_t idx)
{
  if (idx >= ourEditablePPoints.size()) return;

  remove2DPoint(ourEditablePPoints[idx].highlightId);

  ourEditablePPoints[idx] = EditablePickedPoint();
}


void FdPickedPoints::resetPPs()
{
  if (ourHighlighter)
    ourHighlighter->removeAllPoints();

  ourEditablePPoints.clear();
}


/*!
  Highlights and stores the node numbers for the strain gage command.
*/

void FdPickedPoints::selectNode(size_t number, int nodeID,
                                const FaVec3& worldNodePos)
{
  if (number < ourSelectedNodes.size())
    remove2DPoint(ourSelectedNodes[number].second);
  else
    ourSelectedNodes.resize(number+1, { -1, -1 });

  ourSelectedNodes[number] = { nodeID, add2DPoint(worldNodePos) };
}

/*!
  Removes node selections and highlights.
  Passing -1 will remove all selections.
*/

void FdPickedPoints::deselectNode(int number)
{
  if (number < 0)
  {
    for (const FdInts& node : ourSelectedNodes)
      remove2DPoint(node.second);
    ourSelectedNodes.clear();
  }
  else if (number < static_cast<int>(ourSelectedNodes.size()))
  {
    remove2DPoint(ourSelectedNodes[number].second);
    ourSelectedNodes[number] = { -1, -1 };
  }
}


int FdPickedPoints::getSelectedNode()
{
  return ourSelectedNodes.empty() ? -1 : ourSelectedNodes.front().first;
}


std::vector<int> FdPickedPoints::getSelectedNodes()
{
  std::vector<int> nodes;
  nodes.reserve(ourSelectedNodes.size());

  for (const FdInts& node : ourSelectedNodes)
    if (node.first > 0)
      nodes.push_back(node.first);

  return nodes;
}


/*!
  Selects or deselects the provided nodeID,
  and highligths it at position worldNodePos if needed.
  If the node is previously selected it will be deselected,
  if TOGGLE_SELECT or REMOVE_SELECT.
  If the node is not selected, it will be selected unless REMOVE_SELECT.
  Returns whether the node now is a part of the selection.
*/

bool FdPickedPoints::selectNodeSet(int nodeID, const FaVec3& worldNodePos,
                                   int selectionType)
{
  std::map<int,FdNode>::iterator it = ourNodeSet.find(nodeID);
  if (it != ourNodeSet.end())
  {
    // Previously selected.
    // Deselect if we are toggle or removing from selection, else nothing.
    if (selectionType == TOGGLE_SELECT || selectionType == REMOVE_SELECT)
    {
      remove2DPoint(it->second.first);
      ourNodeSet.erase(it);
      return false;
    }
  }
  else if (selectionType != REMOVE_SELECT)
    ourNodeSet[nodeID] = { add2DPoint(worldNodePos), worldNodePos };
  else
    return false;

  return true;
}

void FdPickedPoints::clearNodeSet()
{
  for (const std::pair<const int,FdNode>& node : ourNodeSet)
    remove2DPoint(node.second.first);

  ourNodeSet.clear();
}

void FdPickedPoints::getNodeSet(std::vector<FdNode>& nodeSet)
{
  nodeSet.clear();
  nodeSet.reserve(ourNodeSet.size());

  for (const std::pair<const int,FdNode>& node : ourNodeSet)
    nodeSet.emplace_back(node.first,node.second.second);
}

bool FdPickedPoints::hasNodeSet()
{
  return !ourNodeSet.empty();
}
