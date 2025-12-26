// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdObjParser.H"
#include <iostream>


int main (int argc, char** argv)
{
  if (argc < 2)
  {
    std::cout <<"usage: "<< argv[0] <<" <objfile>\n";
    return 0;
  }

  FdObjParser obj(argv[1]);
  if (obj.vertexIndices.empty())
    return 1;

  std::cout <<"Vertices: "<< obj.vertices.size() << std::endl;
  std::cout <<"Indices: "<< obj.vertexIndices.size() << std::endl;
  return 0;
}
