// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_CAD_FACE_H
#define FD_CAD_FACE_H

#include <vector>

#include <Inventor/nodes/SoIndexedFaceSet.h>

class FdCadEdge;
class FdCadEntityInfo;


class FdCadFace : public SoIndexedFaceSet
{
  SO_NODE_HEADER(FdCadFace);

public:
  FdCadFace();

  static void initClass();

  void setGeometryInfo(FdCadEntityInfo* info);
  FdCadEntityInfo* getGeometryInfo();

  std::vector<FdCadEdge*> myEdges;

private:
  virtual ~FdCadFace();

  FdCadEntityInfo* myGeometryInfo;
};

#endif
