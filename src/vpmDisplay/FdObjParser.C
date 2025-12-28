// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdObjParser.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#ifdef FD_DEBUG
#include <iostream>
#endif
#include <cstring>
#include <cstdio>
#include <cctype>


FdObjParser::FdObjParser(const char* fName, int gid)
{
  groupId = 0;
  nFace = 0;

  FILE* file = fopen(fName,"r");
  if (!file)
  {
    perror(fName);
    return;
  }

  // Lambda function reading next word from the file.
  auto&& getWord = [file](char* word, int n)
  {
    int i, c;
    for (i = 0; i < n; i++)
      if ((c = fgetc(file)) < 0)
        return false; // end-of-file reached
      else if (isspace(c))
        break;
      else
        word[i] = c;

    word[i == n ? --i : i] = 0;
    return true;
  };

  char lineHeader[256];
  float x, y, z;
  size_t igroup = 0;
  using GeoGroup = std::pair<std::string,size_t>;
  std::vector<GeoGroup> geometryGroups;

  // Read the first word of the line
  while (getWord(lineHeader,256))
    if (strcmp(lineHeader,"v") == 0)
    {
      if (fscanf(file,"%f %f %f\n",&x,&y,&z) >= 0)
        vertices.push_back({x,y,z});
#if FD_DEBUG > 1
      std::cout <<"Read vertex "<< vertices.size()
                <<": "<< x <<" "<< y <<" "<< z << std::endl;
#endif
    }
    else if (strcmp(lineHeader,"vt") == 0)
    {
      if (fscanf(file,"%f %f\n",&x,&y) >= 0)
        uvs.push_back({x,y,0.0f});
#if FD_DEBUG > 1
      std::cout <<"Read texture "<< uvs.size()
                <<": "<< x <<" "<< y << std::endl;
#endif
    }
    else if (strcmp(lineHeader,"vn") == 0)
    {
      if (fscanf(file,"%f %f %f\n",&x,&y,&z) >= 0)
        normals.push_back({x,y,z});
#if FD_DEBUG > 1
      std::cout <<"Read normal "<< normals.size()
                <<": "<< x <<" "<< y <<" "<< z << std::endl;
#endif
    }
    else if (strcmp(lineHeader,"g") == 0 ||
             strcmp(lineHeader,"o") == 0)
    {
      if (!fgets(lineHeader,256,file))
        perror("fgets");
      else
      {
        lineHeader[strlen(lineHeader)-1] = 0; // replace newline by 0
        geometryGroups.emplace_back(lineHeader,igroup);
#if FD_DEBUG > 1
        std::cout <<"Read group "<< geometryGroups.size() <<": \""
                  << geometryGroups.back().first <<"\" "
                  << geometryGroups.back().second << std::endl;
#endif
      }
    }
    else if (strcmp(lineHeader,"f") == 0)
    {
      if (!fgets(lineHeader,256,file))
        perror("fgets");
      else
        lineHeader[strlen(lineHeader)-1] = 0; // replace newline by 0

      std::vector<int> ints;
      size_t j = 0;
      for (size_t i = 0; i < strlen(lineHeader); i++)
      {
        char c = lineHeader[i];
        if (c == '#') // comment line
          break;
        else if (isdigit(c) || c == '-' || c == '+')
          continue;
        else if (i > j)
        {
          ints.push_back(atoi(lineHeader+j));
          j = i+1;
        }
        else
          j++;
      }
      // Add final number
      if (j < strlen(lineHeader))
        ints.push_back(atoi(lineHeader+j));

#if FD_DEBUG > 1
      std::cout <<"Face indices \""<< lineHeader <<"\": #"<< ints.size();
      for (int i : ints) std::cout <<" "<< i;
      std::cout << std::endl;
#endif
      switch (ints.size())
        {
        case 3:
          vertexIndices.push_back(ints[0]);
          vertexIndices.push_back(ints[1]);
          vertexIndices.push_back(ints[2]);
          break;
        case 4:
          vertexIndices.push_back(ints[0]);
          vertexIndices.push_back(ints[1]);
          vertexIndices.push_back(ints[2]);
          vertexIndices.push_back(ints[3]);
          break;
        case 6:
          vertexIndices.push_back(ints[0]);
          vertexIndices.push_back(ints[2]);
          vertexIndices.push_back(ints[4]);
          normalIndices.push_back(ints[1]);
          normalIndices.push_back(ints[3]);
          normalIndices.push_back(ints[5]);
          break;
        case 8:
          vertexIndices.push_back(ints[0]);
          vertexIndices.push_back(ints[2]);
          vertexIndices.push_back(ints[4]);
          vertexIndices.push_back(ints[6]);
          normalIndices.push_back(ints[1]);
          normalIndices.push_back(ints[3]);
          normalIndices.push_back(ints[5]);
          normalIndices.push_back(ints[7]);
          break;
        case 9:
          vertexIndices.push_back(ints[0]);
          vertexIndices.push_back(ints[3]);
          vertexIndices.push_back(ints[6]);
          uvIndices.push_back(ints[1]);
          uvIndices.push_back(ints[4]);
          uvIndices.push_back(ints[7]);
          normalIndices.push_back(ints[2]);
          normalIndices.push_back(ints[5]);
          normalIndices.push_back(ints[8]);
          break;
        case 12:
          vertexIndices.push_back(ints[0]);
          vertexIndices.push_back(ints[3]);
          vertexIndices.push_back(ints[6]);
          vertexIndices.push_back(ints[9]);
          uvIndices.push_back(ints[1]);
          uvIndices.push_back(ints[4]);
          uvIndices.push_back(ints[7]);
          uvIndices.push_back(ints[10]);
          normalIndices.push_back(ints[2]);
          normalIndices.push_back(ints[5]);
          normalIndices.push_back(ints[8]);
          normalIndices.push_back(ints[11]);
          break;
        default:
          nFace = 0;
          vertexIndices.clear();
          normalIndices.clear();
          uvIndices.clear();
          ListUI <<"Could not parse obj-file "<< fName
                 <<" ("<< ints.size() <<").\n";
          return;
        }

      vertexIndices.push_back(-1);
      if (ints.size() >= 6)
        normalIndices.push_back(-1);
      if (ints.size() >= 9)
        uvIndices.push_back(-1);

      ++nFace;

      igroup = vertexIndices.size();
    }

  fclose(file);

  groupId = geometryGroups.size();
#ifdef FD_DEBUG
  std::cout <<"("<< vertices.size() <<" vertices";
  if (!uvs.empty()) std::cout <<", "<< uvs.size() <<" uvs";
  if (!normals.empty()) std::cout <<", "<< normals.size() <<" normals";
  std::cout <<", "<< groupId <<" groups, "<< nFace <<" faces)"<< std::endl;
#endif

  // Transform to 0-based indices
  for (int& idx : vertexIndices) --idx;
  for (int& idx : normalIndices) --idx;
  for (int& idx : uvIndices)     --idx;

  if (groupId <= 1)
  {
    groupId = 0;
    return;
  }
  else if (gid == groupId)
    return;
  else if (gid >= 0 && gid < groupId)
    groupId = gid;
  else if (FFaMsg::dialog("Multiple geometry groups in obj-file. "
                          "Import all?",FFaMsg::FFaDialogType::YES_NO) > 0)
    return;
  else
  {
    std::vector<std::string> buttonText = { "Select" };
    std::vector<std::string> selectionList(groupId);
    for (int k = 0; k < groupId; k++)
      selectionList[k] = std::to_string(1+k) + ": " + geometryGroups[k].first;
    FFaMsg::dialog(groupId,"Multiple geometry groups in obj-file. "
                   "Please select group.",FFaMsg::FFaDialogType::GENERIC,
                   buttonText,selectionList);
  }

  // We have multiple groups, but only want one of them
  igroup = static_cast<size_t>(groupId+1);
  if (igroup < geometryGroups.size())
  {
    igroup = geometryGroups[igroup].second;
    vertexIndices.resize(igroup);
    if (!normalIndices.empty())
      normalIndices.resize(igroup);
    if (!uvIndices.empty())
      uvIndices.resize(igroup);
  }
  if (groupId > 0)
  {
    igroup = geometryGroups[groupId].second;
    vertexIndices.erase(vertexIndices.begin(),vertexIndices.begin()+igroup);
    if (!normalIndices.empty())
      normalIndices.erase(normalIndices.begin(),normalIndices.begin()+igroup);
    if (!uvIndices.empty())
      uvIndices.erase(uvIndices.begin(),uvIndices.begin()+igroup);
  }
}
