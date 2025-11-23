// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_CAD_HANDLER_H
#define FD_CAD_HANDLER_H

#include <iostream>
#include <vector>
#include <string>

#include "FFaLib/FFaAlgebra/FFaMat34.H"
#include "FFdCadModel/FFdUtils/FFdLook.H"

class FmTriad;
class FdCadSolid;
class FdCadSolidWire;

using FdSolidWirePair = std::pair<FdCadSolid*,FdCadSolidWire*>;


class FdCadComponent
{
public:
  FdCadComponent() {}
  virtual ~FdCadComponent() {}

  virtual void deleteCadData() = 0;
  virtual void write(std::ostream& out, const std::string& indent) = 0;
  virtual void read(std::istream& in) = 0;

  FFdLook myVisProp;
  FaMat34 myPartCS;
};


class FdCadAssembly : public FdCadComponent
{
public:
  virtual ~FdCadAssembly() { this->deleteCadData(); }

  virtual void deleteCadData();
  virtual void write(std::ostream& out, const std::string& indent);
  virtual void read(std::istream& in);

  std::vector<FdCadComponent*> myComponents;
};


class FdCadPart : public FdCadComponent
{
public:
  virtual ~FdCadPart() { this->deleteCadData(); }

  virtual void deleteCadData();
  virtual void write(std::ostream& out, const std::string& indent);
  virtual void read(std::istream& in);

  void addSolid(FdCadSolid* solid, FdCadSolidWire* wire);
  const FdSolidWirePair& getSolid(size_t i) const { return mySolids[i]; }
  size_t size() const { return mySolids.size(); }

private:
  std::vector<FdSolidWirePair> mySolids;
};


class FdCadHandler
{
public:
  FdCadHandler() : myCadData(NULL) {}
  ~FdCadHandler() { this->deleteCadData(); }

  static void initFdCad();
  void deleteCadData();

  FdCadComponent* getCadComponent() { return myCadData; }
  FdCadPart*      getCadPart();
  FdCadAssembly*  getCadAssembly();

  bool hasPart();
  bool hasAssembly();

  void write(std::ostream& out);
  bool read(std::istream& in);

  // Generate beam visualizations
  bool createBeamViz_Pipe(const FaVec3& v1, const FaVec3& v2,
                          double Do, double Di, int startAngle, int stopAngle);
  bool createBeamViz_Blade(const FmTriad* v1, const FmTriad* v2,
                           void* pBladeProperty, void* pBladePropertyPrev,
                           void* pBladePropertyNext, int segmentPart);

private:
  FdCadComponent* myCadData;
};

#endif
