// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_EXTRA_GRAPHICS_H
#define FD_EXTRA_GRAPHICS_H

#include <vector>
#include <array>

class SoNode;
class FaVec3;
class FaMat34;
class FFa3DLocation;
class FFaGeometryBase;
class FFaCylinderGeometry;


namespace FdExtraGraphics
{
  using FdColor = std::array<float,3>;

  void  init();
  void* showBBox(SoNode* rootOfObjectToSurround);
  void* showBBox(const FaVec3& min, const FaVec3& max, const FaMat34& where);
  void  removeBBox(void* bBoxId);

  void showDirection(const FaVec3& from, const FaVec3& direction);
  void hideDirection();
  void moveDirection(const FaVec3& newFromPoint);

  void showLine(const FaVec3& from, const FaVec3& to,
                const FdColor& color = {1,0,0});
  void hideLine();

  void showCS(const FaMat34& globalCS);
  void hideCS();

  void showDOFVisualizing(int DOF, const FaVec3& center, const FaVec3& direct);
  void hideDOFVisualizing();

  void show3DLocation(const FaMat34& posRefCS, const FaMat34& rotRefCS,
                      const FFa3DLocation& loc);
  void hide3DLocation();

  void showCylinder(const FaMat34& matrix,
                    const std::vector<FaVec3>& points, bool arcOnly = false);
  void showCylinder(const FFaCylinderGeometry& cylinder,
                    const FaMat34& matrix, bool highlight = false);
  void hideCylinder();

  void highlight(const FFaGeometryBase* geo, const FaMat34& m, bool onOff);
}

#endif
