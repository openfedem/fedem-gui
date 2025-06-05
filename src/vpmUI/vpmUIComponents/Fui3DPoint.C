// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/Fui3DPoint.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"


////////////////////////////////////////////////////////////////////////
//
// Initialisation to be called by UILib dep. subclass
//

void Fui3DPoint::init()
{
  this->myXField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->myXField->setAcceptPolicy(FFuIOField::ENTERONLY);
  this->myXField->setDoubleDisplayMode(FFuIOField::AUTO);
  this->myXField->setAcceptedCB(FFaDynCB1M(Fui3DPoint,this,callPointChangedCB,double));

  this->myYField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->myYField->setAcceptPolicy(FFuIOField::ENTERONLY);
  this->myYField->setDoubleDisplayMode(FFuIOField::AUTO);
  this->myYField->setAcceptedCB(FFaDynCB1M(Fui3DPoint,this,callPointChangedCB,double));

  this->myZField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->myZField->setAcceptPolicy(FFuIOField::ENTERONLY);
  this->myZField->setDoubleDisplayMode(FFuIOField::AUTO);
  this->myZField->setAcceptedCB(FFaDynCB1M(Fui3DPoint,this,callPointChangedCB,double));

  if (!this->myRefMenu) return;

  this->myRefMenu->addOption("Global");
  this->myRefMenu->addOption("Local");
  this->myRefMenu->setOptionSelectedCB(FFaDynCB1M(Fui3DPoint,this,callRefChangedCB,int));
}


void Fui3DPoint::setSensitivity(bool sensitive)
{
  this->myZField->setSensitivity(sensitive);
  this->myYField->setSensitivity(sensitive);
  this->myXField->setSensitivity(sensitive);
}


////////////////////////////////////////////////////////////////////////
//
// Setting and getting coordinate values
//

void Fui3DPoint::setXvalue(double value)
{
  this->myXField->setValue(value);
}

void Fui3DPoint::setYvalue(double value)
{
  this->myYField->setValue(value);
}

void Fui3DPoint::setZvalue(double value)
{
  this->myZField->setValue(value);
}

void Fui3DPoint::setValue(double x, double y, double z)
{
  this->myXField->setValue(x);
  this->myYField->setValue(y);
  this->myZField->setValue(z);
}


double Fui3DPoint::getXvalue() const
{
  return this->myXField->getDouble();
}

double Fui3DPoint::getYvalue() const
{
  return this->myYField->getDouble();
}

double Fui3DPoint::getZvalue() const
{
  return this->myZField->getDouble();
}

FaVec3 Fui3DPoint::getValue() const
{
  return FaVec3(this->getXvalue(),this->getYvalue(),this->getZvalue());
}


////////////////////////////////////////////////////////////////////////
//
// Setting and getting coordinate references
//

void Fui3DPoint::setGlobal()
{
  if (!this->myRefMenu) return;

  this->myRefMenu->selectOption(0);
}

void Fui3DPoint::setGlobalOnly()
{
  if (!this->myRefMenu) return;

  this->myRefMenu->selectOption(0);
  this->myRefMenu->setSensitivity(false);
}

void Fui3DPoint::setLocal()
{
  if (!this->myRefMenu) return;

  this->myRefMenu->selectOption(1);
  this->myRefMenu->setSensitivity(true);
}

void Fui3DPoint::enableLocal()
{
  if (!this->myRefMenu) return;

  this->myRefMenu->setSensitivity(true);
}

bool Fui3DPoint::isGlobal() const
{
  if (!this->myRefMenu) return true;

  return !this->myRefMenu->getSelectedOption();
}


/*!
  Callback to notify new coords and isGlobal()
*/

void Fui3DPoint::setPointChangedCB(const FFaDynCB2<const FaVec3&,bool>& aDynCB)
{
  this->myPointChangedCB = aDynCB;
}


/*!
  Callback to notify when ref changed. Called with isGlobal()
*/

void Fui3DPoint::setRefChangedCB(const FFaDynCB1<bool>& aDynCB)
{
  this->myRefChangedCB = aDynCB;
}


/*!
  Internal Callbacks
*/

void Fui3DPoint::callPointChangedCB(double)
{
  this->myPointChangedCB.invoke(this->getValue(),this->isGlobal());
}

void Fui3DPoint::callRefChangedCB(int)
{
  this->myRefChangedCB.invoke(this->isGlobal());
}
