// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/Fui3DPoint.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFaLib/FFaAlgebra/FFaVec3.H"


////////////////////////////////////////////////////////////////////////
//
// Initialisation to be called by UILib dep. subclass
//

void Fui3DPoint::init()
{
  int fieldNo = 0;
  for (FFuIOField*& fld : myFields)
    if (fld)
    {
      fld->setAcceptPolicy(FFuIOField::ENTERONLY);
      if (++fieldNo < 4)
      {
        fld->setInputCheckMode(FFuIOField::DOUBLECHECK);
        fld->setDoubleDisplayMode(FFuIOField::AUTO);
        fld->setAcceptedCB(FFaDynCB1M(Fui3DPoint,this,callPointChangedCB,double));
      }
      else
      {
        fld->setInputCheckMode(FFuIOField::INTEGERCHECK);
        fld->setAcceptedCB(FFaDynCB1M(Fui3DPoint,this,callNodeChangedCB,int));
      }
    }

  if (!this->myRefMenu) return;

  this->myRefMenu->addOption("Global");
  this->myRefMenu->addOption("Local");
  this->myRefMenu->setOptionSelectedCB(FFaDynCB1M(Fui3DPoint,this,callRefChangedCB,int));
}


void Fui3DPoint::setSensitivity(bool sensitive)
{
  for (FFuIOField* fld : myFields)
    if (fld) fld->setSensitivity(sensitive);
}


////////////////////////////////////////////////////////////////////////
//
// Setting and getting coordinate values
//

void Fui3DPoint::setXvalue(double value)
{
  myFields[0]->setValue(value);
}

void Fui3DPoint::setYvalue(double value)
{
  myFields[1]->setValue(value);
}

void Fui3DPoint::setZvalue(double value)
{
  myFields[2]->setValue(value);
}

void Fui3DPoint::setValue(double x, double y, double z)
{
  myFields[0]->setValue(x);
  myFields[1]->setValue(y);
  myFields[2]->setValue(z);
}

void Fui3DPoint::setValue(const FaVec3& X)
{
  this->setValue(X.x(),X.y(),X.z());
}


void Fui3DPoint::setValue(int nodeNo)
{
  if (myFields[3])
  {
    this->showNodeField(nodeNo);
    if (nodeNo > 0)
      myFields[3]->setValue(nodeNo);
    for (int i = 0; i < 3; i++)
      myFields[i]->setSensitivity(nodeNo < 0);
  }
}


double Fui3DPoint::getXvalue() const
{
  return myFields[0]->getDouble();
}

double Fui3DPoint::getYvalue() const
{
  return myFields[1]->getDouble();
}

double Fui3DPoint::getZvalue() const
{
  return myFields[2]->getDouble();
}

FaVec3 Fui3DPoint::getValue() const
{
  return FaVec3(this->getXvalue(),this->getYvalue(),this->getZvalue());
}

int Fui3DPoint::getNodeNo() const
{
  return myFields[3] ? myFields[3]->getInt() : -1;
}


////////////////////////////////////////////////////////////////////////
//
// Setting and getting coordinate references
//

void Fui3DPoint::setCSRef(int cs, bool only)
{
  if (!myRefMenu) return;

  myRefMenu->selectOption(cs);
  myRefMenu->setSensitivity(!only);
}

bool Fui3DPoint::isGlobal() const
{
  return myRefMenu ? myRefMenu->getSelectedOption() == 0 : 1;
}


/*!
  Internal Callbacks
*/

void Fui3DPoint::callNodeChangedCB(int)
{
  this->myNodeChangedCB.invoke(this->getNodeNo());
}

void Fui3DPoint::callRefChangedCB(int)
{
  this->myRefChangedCB.invoke(this->isGlobal());
}

void Fui3DPoint::callPointChangedCB(double)
{
  this->myPointChangedCB.invoke(this->getValue(),this->isGlobal());
}
