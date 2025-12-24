// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_CTRL_GRID_H
#define FD_CTRL_GRID_H

class SbVec3f;


namespace FdCtrlGrid
{
  void addGrid();
  void removeGrid();

  void setGridMaterial(float r, float g, float b);

  void setGridSize(float sizeX, float sizeY);
  float getGridSizeX();
  float getGridSizeY();

  bool getGridState();
  void setGridState(bool state);

  bool getSnapState();
  void setSnapState(bool s);

  void setSnapDistance(float X, float Y);
  float getSnapDistanceX();
  float getSnapDistanceY();

  void snapToNearestPoint(SbVec3f& vec);
};

#endif
