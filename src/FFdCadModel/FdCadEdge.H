// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_CAD_EDGE_H
#define FD_CAD_EDGE_H

#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/fields/SoMFNode.h>

class FdCadEntityInfo;


class FdCadEdge : public SoIndexedLineSet
{
  SO_NODE_HEADER(FdCadFace);

public:
  FdCadEdge();

  static void initClass();

  void setGeometryInfo(FdCadEntityInfo* info);
  FdCadEntityInfo* getGeometryInfo();

  SoMFNode faces;

private:
  virtual ~FdCadEdge();

  FdCadEntityInfo* myGeometryInfo;
};

#endif
