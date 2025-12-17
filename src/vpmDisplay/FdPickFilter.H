// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_PICK_FILTER_H
#define FD_PICK_FILTER_H

#include <vector>

class FdObject;
class FdPart;
class FaVec3;
class SoPath;
class SoPickedPointList;
class SoCoordinate3;
class SoVertexProperty;
class SoVRMLCoordinate;
class SoMFVec3f;


namespace FdPickFilter
{
  FdObject* getInterestingPObj(const SoPickedPointList* ppl,
                               // To select objects behind the selected one:
                               const std::vector<FdObject*>& selectedObjects,
                               // Filter variables:
                               const std::vector<int>& types,
                               bool typesIsInteresting,
                               // Variables returning values:
                               long int& indexToInterestingPP,
                               bool& wasASelectedObjInPPList);

  FdObject* getCyceledInterestingPObj(const SoPickedPointList* ppl,
                                      // Filter variables:
                                      const std::vector<int>& types,
                                      bool typesIsInteresting,
                                      // Variables returning values:
                                      long int& indexToInterestingPP);

  FdObject* findFdObject(SoPath* path);

  FdPart* findFirstPartHit(int& pplistIndex,
                           const SoPickedPointList& ppl,
                           FdObject* partToFind = NULL);
  FdPart* findNodeHit(int& nodeID, FaVec3& nodePos,
                      const SoPickedPointList& ppl,
                      FdObject* partToFind);

  SoCoordinate3*    findLastCoordNode(SoPath* path);
  SoVertexProperty* findLastVxPropNode(SoPath* path);
  SoVRMLCoordinate* findLastVRMLCoordNode(SoPath* path);
  SoMFVec3f*        findLastVertexes(SoPath* path);
}

#endif
