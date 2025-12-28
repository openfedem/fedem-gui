// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FT_OBJ_PARSER_H
#define FT_OBJ_PARSER_H

#include <array>
#include <vector>
#include <cstddef>

struct FdObjParser
{
  int groupId = 0;
  size_t nFace = 0;

  using XYZ = std::array<float,3>;

  std::vector<XYZ> vertices;
  std::vector<XYZ> uvs;
  std::vector<XYZ> normals;
  std::vector<int> vertexIndices;
  std::vector<int> uvIndices;
  std::vector<int> normalIndices;

  FdObjParser(const char* fName, int gid = -1);
};

#endif
