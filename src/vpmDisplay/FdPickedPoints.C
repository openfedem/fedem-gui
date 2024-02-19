// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdPickedPoints.H"
#include "vpmDisplay/Fd2DPoints.H"
#include "vpmDisplay/qtViewers/FdQtViewer.H"
#include "vpmDisplay/FdConverter.H"


Fd2DPoints* FdPickedPoints::pointHighlighter = NULL;
FdQtViewer* FdPickedPoints::viewer = NULL;

std::vector<FdPickedPoints::EditablePickedPoint> FdPickedPoints::ourEditablePPoints;

std::vector<FdInts> FdPickedPoints::ourSelectedNodes; // NodeID, HighlightID
std::map<int,FdNode> FdPickedPoints::ourNodeSet; // NodeID, HighlightID, globPos


void FdPickedPoints::init(Fd2DPoints* highlighter, FdQtViewer* v)
{
  FdPickedPoints::pointHighlighter = highlighter;
  FdPickedPoints::viewer = v;
  FdPickedPoints::resetPPs();
}


long FdPickedPoints::add2DPoint(const FaVec3& point)
{
  if (pointHighlighter)
    return pointHighlighter->addPoint(FdConverter::toSbVec3f(point));
  else
    return -1;
}

void FdPickedPoints::remove2DPoint(long id)
{
  if (pointHighlighter)
    pointHighlighter->removePoint(id);
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


FaVec3 FdPickedPoints::getPickedPoint(unsigned int idx, bool global)
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

void FdPickedPoints::setPickedPoint(unsigned int idx, bool global, const FaVec3& point)
{
  if (idx >= ourEditablePPoints.size())
    ourEditablePPoints.resize(idx+1);

  FaVec3& globPoint = ourEditablePPoints[idx].globPoint;
  if (global)
    globPoint = point;
  else
    globPoint = ourEditablePPoints[idx].objectToWorldMatrix * point;

  if (pointHighlighter)
  {
    if (ourEditablePPoints[idx].highlightId == -1)
      ourEditablePPoints[idx].highlightId = pointHighlighter->addPoint(FdConverter::toSbVec3f(globPoint));
    else
      pointHighlighter->movePoint(ourEditablePPoints[idx].highlightId, FdConverter::toSbVec3f(globPoint));
  }
}


void FdPickedPoints::setPP(unsigned int idx, const FaVec3& point, const FaMat34& objToWorld)
{
  if (idx >= ourEditablePPoints.size())
    ourEditablePPoints.resize(idx+1);

  ourEditablePPoints[idx].globPoint = point;
  ourEditablePPoints[idx].objectToWorldMatrix = objToWorld;
  for (int i = 0; i < 3; i++)
    ourEditablePPoints[idx].objectToWorldMatrix[i].normalize();
  ourEditablePPoints[idx].worldToObjectMatrix = ourEditablePPoints[idx].objectToWorldMatrix.inverse();

  if (pointHighlighter)
  {
    if (ourEditablePPoints[idx].highlightId == -1)
      ourEditablePPoints[idx].highlightId = pointHighlighter->addPoint(FdConverter::toSbVec3f(point));
    else
      pointHighlighter->movePoint(ourEditablePPoints[idx].highlightId, FdConverter::toSbVec3f(point));
  }
}

void FdPickedPoints::removePP(unsigned int idx)
{
  if (idx >= ourEditablePPoints.size()) return;

  if (pointHighlighter && ourEditablePPoints[idx].highlightId != -1)
    pointHighlighter->removePoint(ourEditablePPoints[idx].highlightId);

  ourEditablePPoints[idx] = EditablePickedPoint();
}

void FdPickedPoints::resetPPs()
{
  if (pointHighlighter)
    pointHighlighter->removeAllPoints();

  ourEditablePPoints.clear();
}


/*!
  Highlights and stores the node numbers for the strain gage command.
*/

void FdPickedPoints::selectNode(unsigned int number, int nodeID, const FaVec3& worldNodePos)
{
  if (number >= ourSelectedNodes.size())
    ourSelectedNodes.resize(number+1,std::make_pair(-1,-1));

  if (ourSelectedNodes[number].second != -1)
    FdPickedPoints::remove2DPoint(ourSelectedNodes[number].second);

  ourSelectedNodes[number] = std::make_pair(nodeID,FdPickedPoints::add2DPoint(worldNodePos));
}

/*!
  Removes node selections and highlights.
  Passing -1 will remove all selections.
*/

void FdPickedPoints::deselectNode(int number)
{
  if (number < 0)
  {
    for (size_t i = 0; i < ourSelectedNodes.size(); i++)
      if (ourSelectedNodes[i].second != -1)
        FdPickedPoints::remove2DPoint(ourSelectedNodes[i].second);
    ourSelectedNodes.clear();
  }
  else if (number < (int)ourSelectedNodes.size())
  {
    if (ourSelectedNodes[number].second != -1)
      FdPickedPoints::remove2DPoint(ourSelectedNodes[number].second);
    ourSelectedNodes[number] = std::make_pair(-1,-1);
  }
}


/*!
  Selects or deselects the provided nodeID, and highligths it at position worldNodePos if needed.
  If the node is previously selected it will be deselected, if TOGGLE_SELECT or REMOVE_SELECT.
  If the node is not selected, it will be selected unless REMOVE_SELECT.
  Returns whether the node now is a part of the selection.
*/

bool FdPickedPoints::selectNodeSet(int nodeID, const FaVec3& worldNodePos, int selectionType)
{
  std::map<int,FdNode>::iterator it = ourNodeSet.find(nodeID);
  if (it != ourNodeSet.end())
  {
    // Previously selected.
    // Deselect if we are toggle or removing from selection, else nothing.
    if (selectionType == TOGGLE_SELECT || selectionType == REMOVE_SELECT)
    {
      FdPickedPoints::remove2DPoint(it->second.first);
      ourNodeSet.erase(it);
      return false;
    }
  }
  else if (selectionType != REMOVE_SELECT)
    ourNodeSet[nodeID] = std::make_pair(FdPickedPoints::add2DPoint(worldNodePos),worldNodePos);
  else
    return false;

  return true;
}

void FdPickedPoints::clearNodeSet()
{
  std::map<int,FdNode>::const_iterator it;
  for (it = ourNodeSet.begin(); it != ourNodeSet.end(); ++it)
    FdPickedPoints::remove2DPoint(it->second.first);

  ourNodeSet.clear();
}

void FdPickedPoints::getNodeSet(std::vector<FdNode>& nodeSet)
{
  nodeSet.clear();

  std::map<int,FdNode>::const_iterator it;
  for (it = ourNodeSet.begin(); it != ourNodeSet.end(); ++it)
    nodeSet.push_back(std::make_pair(it->first,it->second.second));
}
