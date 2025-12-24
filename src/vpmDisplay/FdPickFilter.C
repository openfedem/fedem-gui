// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <algorithm>

#include <Inventor/SoPath.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/VRMLnodes/SoVRMLCoordinate.h>
#include <Inventor/VRMLnodes/SoVRMLVertexShape.h>
#include <Inventor/nodes/SoVertexShape.h>

#include "vpmDisplay/FdPickFilter.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdPart.H"


namespace
{
  bool isFdObjInteresting(FdObject* obj,
                         const std::vector<int>& types, bool isInteresting)
  {
    for (int itype : types)
      if (obj->isOfType(itype))
        return isInteresting;

    return !isInteresting;
  }
}


FdObject* FdPickFilter::findFdObject(SoPath* path)
{
  if (!path) return NULL;

  // Finds first kit with a backPt and returns its Fdpointer
  SoFullPath* p = (SoFullPath*)path;
  int pathLength = p->getLength();
  for (int i = 0; i < pathLength; i++)
  {
    SoNode* n = p->getNodeFromTail(i);
    if (n->isOfType(SoBaseKit::getClassTypeId()))
      if ((n = ((SoBaseKit*)n)->getPart("backPt",false)))
        if (n->isOfType(FdBackPointer::getClassTypeId()))
          return ((FdBackPointer*)n)->getPointer();
  }

  return NULL;
}


FdObject* FdPickFilter::getCyceledInterestingPObj(const SoPickedPointList* ppl,
						  const std::vector<int>& types,
						  bool typesIsInteresting,
						  long& indexToInterestingPP)
{
  static size_t cycleIndex = 0;
  static FdObject* lastReturnedFdObject = NULL;
  static long int previousPPListLength = 0;

  std::vector<FdObject*> pickedInterestingFdObjects;
  std::vector<long int>  ppIndexesForEachFdObject;

  bool lastRetObjIsInPPL = false;

  FdObject* obj     = NULL;
  FdObject* lastObj = NULL;

  // Build array of picked objects:
  long int i, nPickedPoints = ppl->getLength();
  for (i = 0; i < nPickedPoints; i++)
    if ((obj = findFdObject(((*ppl)[i])->getPath())) && obj != lastObj)
    {
      // Do this only if the new object is different from the previous one
      // else skip because it is a multiple hit
      if (isFdObjInteresting(obj,types,typesIsInteresting))
      {
        pickedInterestingFdObjects.push_back(obj);
        ppIndexesForEachFdObject.push_back(i);
        if (obj == lastReturnedFdObject)
          lastRetObjIsInPPL = true;
      }
      lastObj = obj;
    }

  size_t nObjects = pickedInterestingFdObjects.size();
  if (!lastRetObjIsInPPL || cycleIndex >= nObjects || nPickedPoints != previousPPListLength)
    cycleIndex = 0;

  if (nObjects > 0)
  {
    indexToInterestingPP = ppIndexesForEachFdObject[cycleIndex];
    lastReturnedFdObject = pickedInterestingFdObjects[cycleIndex];
    cycleIndex++;
  }
  else
  {
    indexToInterestingPP = - 1;
    lastReturnedFdObject = NULL;
    cycleIndex = 0;
  }

  previousPPListLength = nPickedPoints;
  return lastReturnedFdObject;
}


FdObject* FdPickFilter::getInterestingPObj(const SoPickedPointList* ppl,
					   const std::vector<FdObject*>& selectedObjects,
					   const std::vector<int>& types,
					   bool typesIsInteresting,
					   long& indexToInterestingPP,
					   bool& wasASelectedObjInPPList)
{
  FdObject* firstInterestingObj = NULL;
  FdObject* obj     = NULL;
  FdObject* lastObj = NULL;

  bool foundIntPoint = false;
  bool firstRun = true;

  bool isASelectedObjInPPList  = false; // Vars used to acumulate
  bool isAnInterestingObjInPPL = false; // the returned result

  long int i, nPickedPoints = ppl->getLength();
  for (i = 0; !foundIntPoint && i < nPickedPoints;)
  {
    // Continue through the list until an new interesting and/or a selected object is reached.
    bool isSelected = false, isInteresting = false;
    for (; i < nPickedPoints && !isSelected && !isInteresting; i++)
      if ((obj = findFdObject(((*ppl)[i])->getPath())) && obj != lastObj)
      {
        // Do this only if the new object is different from the previous one
        isSelected = std::find(selectedObjects.begin(),selectedObjects.end(),obj) != selectedObjects.end();
        isInteresting = isFdObjInteresting(obj,types,typesIsInteresting);
        lastObj = obj;
      }

    // After the for loop, we've come
    // EITHER to a interesting and/or selected object
    // OR to the end of the pp-list

    if (firstRun)
    {
      // This is first stop
      if (isInteresting)
      {
        // Remember the first interesting point
        indexToInterestingPP = i-1;
        firstInterestingObj = obj;
      }
      firstRun = false;
    }
    else if (isInteresting && (!isAnInterestingObjInPPL || (!isSelected && isASelectedObjInPPList)))
    {
      // We update the object and index to return when:
      // we come to an interesting object and there is no interesting object registered before
      // we come to an interesting object after a selected one
      // when the object itself is not selected

      indexToInterestingPP = i-1;
      firstInterestingObj = obj;
      if (!isSelected)
        // We know that when this is entered there IS a selected object in the PPlist
        // ( Because we have stopped before, and that was not because of an interesting object. OR
        //   the other test in the if was true..... :-} Also see comment under method.)
        // We also know that the new interesting object is not selected.
        // that means that we should go no further.
        foundIntPoint = true; // stop the while loop
    }

    if (isSelected)    isASelectedObjInPPList = true;
    if (isInteresting) isAnInterestingObjInPPL = true;
  }

  wasASelectedObjInPPList = isASelectedObjInPPList;
  return firstInterestingObj;
}


/*
  The following scheme visualizes the different possibilities
  and the actions to be done for each.
  The if-test under the for-loop IN the while-loop is built
  using this scheme

     I  short for isInteresting  \  Temp Vars
     S  short for isSelected     /

     FS short for isAnSelectedObjInPPList \ Vars acumulating
     FI short for isAnInterestingObjInPPL / the final result

     I &&  S                I && !S                 !I && S
     -------------------------------------------------------
     FI && FS               FI &&  FS               FI && FS
     continue               Upate vars pointing     continue
     to interesting point
     return

     FI && !FS              FI && !FS               FI && !FS
     Set FS = true                                  Set FS = true
     continue               continue                continue

     !FI && FS              !FI &&  FS              !FI &&  FS
     Upate vars pointing    Upate vars pointing     continue
     to interesting point   to interesting point
     Set FI = true          return
     continue
*/

/*!
  Scans through the picked points until the first hit on partToFind,
  or on first part if partToFind == 0.
*/

FdPart* FdPickFilter::findFirstPartHit(int& pplistIndex, const SoPickedPointList& ppl, FdObject* partToFind)
{
  int nPickedPoints = ppl.getLength();
  FdObject* pickedObject;
  for (int i = 0; i < nPickedPoints; i++)
    if ((pickedObject = findFdObject(ppl[i]->getPath())))
      if (pickedObject->isOfType(FdPart::getClassTypeID()))
        if (partToFind == NULL || pickedObject == partToFind)
        {
          pplistIndex = i;
          return static_cast<FdPart*>(pickedObject);
        }

  // We came to the end without finding a good hit
  pplistIndex = -1;
  return NULL;
}


FdPart* FdPickFilter::findNodeHit(int& nodeID, FaVec3& nodePos, const SoPickedPointList& ppl, FdObject* partToFind)
{
  int pplIndex = -1;
  FdPart* part = FdPickFilter::findFirstPartHit(pplIndex,ppl,partToFind);
  if (part && part->findNode(nodeID,nodePos,ppl[pplIndex]->getPoint()))
    return part;
  else
    return NULL;
}


static SoPath* findLastNodeOfType(SoPath* path, const SoType& type)
{
  if (!path) return NULL;

  SoSearchAction search;
  search.setType(type);
  search.setInterest(SoSearchAction::LAST);
  search.setSearchingAll(true);
  SbBool isKitsOpen = SoBaseKit::isSearchingChildren();
  SoBaseKit::setSearchingChildren(true);
  search.apply(path);
  SoBaseKit::setSearchingChildren(isKitsOpen);

  if ((path = search.getPath()))
    path->ref();

  return path;
}


SoVertexProperty* FdPickFilter::findLastVxPropNode(SoPath* path)
{
  SoPath* p = findLastNodeOfType(path,SoVertexProperty::getClassTypeId());
  if (!p) return NULL;

  SoVertexProperty* tail = (SoVertexProperty*)((SoFullPath*)p)->getTail();
  p->unref();
  return tail;
}


SoCoordinate3* FdPickFilter::findLastCoordNode(SoPath* path)
{
  SoPath* p = findLastNodeOfType(path,SoCoordinate3::getClassTypeId());
  if (!p) return NULL;

  SoCoordinate3* tail = (SoCoordinate3*)((SoFullPath*)p)->getTail();
  p->unref();
  return tail;
}


SoVRMLCoordinate* FdPickFilter::findLastVRMLCoordNode(SoPath* path)
{
  SoPath* p = findLastNodeOfType(path,SoVRMLCoordinate::getClassTypeId());
  if (!p) return NULL;

  SoVRMLCoordinate* tail = (SoVRMLCoordinate*)((SoFullPath*)p)->getTail();
  p->unref();
  return tail;
}


SoMFVec3f* FdPickFilter::findLastVertexes(SoPath* p)
{
  SoFullPath* path = (SoFullPath*)p;
  if (path->getTail()->isOfType(SoVRMLVertexShape::getClassTypeId()))
  {
    SoVRMLVertexShape* vrmlVertShape = (SoVRMLVertexShape*)path->getTail();
    if (vrmlVertShape->coord.isOfType(SoVRMLCoordinate::getClassTypeId()))
    {
      SoVRMLCoordinate* vrmlCoords = (SoVRMLCoordinate*)&vrmlVertShape->coord;
      if (vrmlCoords && vrmlCoords->point.getNum() > 0)
        return &(vrmlCoords->point);
    }
  }
  else if (path->getTail()->isOfType(SoVertexShape::getClassTypeId()))
  {
    SoVertexShape* vertShape = (SoVertexShape*)path->getTail();
    if (vertShape->vertexProperty.isOfType(SoVertexShape::getClassTypeId()))
    {
      SoVertexProperty* vxProp = (SoVertexProperty*)&vertShape->vertexProperty;
      if (vxProp && vxProp->vertex.getNum() > 0)
        return &(vxProp->vertex);
    }
  }

  SoFullPath* vrmlCoordsPath = (SoFullPath*)findLastNodeOfType(path,SoVRMLCoordinate::getClassTypeId());
  SoFullPath* coordsPath = (SoFullPath*)findLastNodeOfType(path,SoCoordinate3::getClassTypeId());
  SoFullPath* vxPropPath = (SoFullPath*)findLastNodeOfType(path,SoVertexProperty::getClassTypeId());

  SoMFVec3f* field = NULL;
  if (!vrmlCoordsPath && !coordsPath && !vxPropPath)
    return field;

  int vrmlCoordsIdx = -1;
  int coordsIdx = -1;
  int vxPropIdx = -1;

  if (vrmlCoordsPath) vrmlCoordsIdx = path->findFork(vrmlCoordsPath);
  if (coordsPath) coordsIdx = path->findFork(coordsPath);
  if (vxPropPath) vxPropIdx = path->findFork(vxPropPath);

  if (vrmlCoordsIdx > coordsIdx && vrmlCoordsIdx > vxPropIdx)
    field = &( ((SoVRMLCoordinate*)(vrmlCoordsPath->getTail()))->point);
  else if (coordsIdx > vrmlCoordsIdx && coordsIdx > vxPropIdx)
    field =  &( ((SoCoordinate3*)(coordsPath->getTail()))->point);
  else if (vxPropIdx > vrmlCoordsIdx && vxPropIdx > coordsIdx)
    field = &( ((SoVertexProperty*)(vxPropPath->getTail()))->vertex);
  else // two or more has equal and the largest index
    std::cerr <<" *** FdPickFilter::findLastVertexesUnhandled coordinate case."<< std::endl;

  if (vrmlCoordsPath) vrmlCoordsPath->unref();
  if (coordsPath) coordsPath->unref();
  if (vxPropPath) vxPropPath->unref();

  return field;
}
