// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_3D_POINT_H
#define FUI_3D_POINT_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "FFaLib/FFaAlgebra/FFaVec3.H"

class FFuIOField;
class FFuOptionMenu;


/*!
  UI component to edit a 3D point relative to a local or global reference.
*/

class Fui3DPoint : virtual public FFuComponentBase
{
public:
  virtual void setSensitivity(bool sensitive);

  void setXvalue(double value);
  void setYvalue(double value);
  void setZvalue(double value);
  void setValue(double x, double y, double z);
  void setValue(const FaVec3& X) { this->setValue(X[0],X[1],X[2]); }

  double getXvalue() const;
  double getYvalue() const;
  double getZvalue() const;
  FaVec3 getValue () const;

  void setGlobal();
  void setGlobalOnly();
  void setLocal();
  void enableLocal();
  bool isGlobal() const;

  void setPointChangedCB(const FFaDynCB2<const FaVec3&,bool>& aDynCB);
  void setRefChangedCB  (const FFaDynCB1<bool>&               aDynCB);

protected:
  void init();

  void callPointChangedCB(double d);
  void callRefChangedCB(int choice);

  FFuIOField*    myZField;
  FFuIOField*    myYField;
  FFuIOField*    myXField;
  FFuOptionMenu* myRefMenu;

private:
  FFaDynCB2<const FaVec3&,bool> myPointChangedCB;
  FFaDynCB1<bool>               myRefChangedCB;
};

#endif
