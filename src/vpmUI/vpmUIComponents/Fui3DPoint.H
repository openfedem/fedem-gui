// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_3D_POINT_H
#define FUI_3D_POINT_H

#include <array>

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuIOField;
class FFuOptionMenu;
class FaVec3;


/*!
  UI component to edit a 3D point relative to a local or global reference.
*/

class Fui3DPoint : virtual public FFuComponentBase
{
  using DynCB0 = FFaDynCB1<int>;
  using DynCB1 = FFaDynCB1<bool>;
  using DynCB2 = FFaDynCB2<const FaVec3&,bool>;

public:
  Fui3DPoint() { myFields.fill(NULL); myRefMenu = NULL; }

  virtual void setSensitivity(bool sensitive);
  virtual void showNodeField(int ID = -1) = 0;

  void setXvalue(double value);
  void setYvalue(double value);
  void setZvalue(double value);
  void setValue(double x, double y, double z);
  void setValue(const FaVec3& X);
  void setValue(int nodeNo);

  double getXvalue() const;
  double getYvalue() const;
  double getZvalue() const;
  FaVec3 getValue () const;
  int    getNodeNo() const;

  void setGlobal(bool globalOnly = false) { this->setCSRef(0,globalOnly); }
  void setLocal(bool localOnly = false)   { this->setCSRef(1,localOnly); }
  bool isGlobal() const;

  void setNodeChangedCB (const DynCB0& cb) { myNodeChangedCB = cb; }
  void setRefChangedCB  (const DynCB1& cb) { myRefChangedCB = cb; }
  void setPointChangedCB(const DynCB2& cb) { myPointChangedCB = cb; }

protected:
  void init();

  std::array<FFuIOField*,4> myFields;
  FFuOptionMenu* myRefMenu;

private:
  void setCSRef(int cs, bool only);
  void callNodeChangedCB(int);
  void callRefChangedCB(int);
  void callPointChangedCB(double);

  DynCB0 myNodeChangedCB;
  DynCB1 myRefChangedCB;
  DynCB2 myPointChangedCB;
};

#endif
