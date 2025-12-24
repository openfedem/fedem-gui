// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdTire.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdExtraGraphics.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdFEModelKit.H"
#include "vpmDB/FmTire.H"
#include "vpmDB/FmTriad.H"

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>


/**********************************************************************
 *
 * CLASS FdTire
 *
 **********************************************************************/

Fmd_SOURCE_INIT(FDTIRE,FdTire,FdObject);

FdTire::FdTire(FmTire* pt) : myColor({ 0.2f, 0.2f, 0.2f })
{
  Fmd_CONSTRUCTOR_INIT(FdTire);

  itsFmOwner = pt;

  FdFEModelKit* FEKit = new FdFEModelKit;
  myFEKit = FEKit;

  this->itsKit = FEKit;
  this->itsKit->ref();

  // Set up backpointer

  myFEKit->setFdPointer(this);

  this->highlightBoxId = NULL;
}


FdTire::~FdTire()
{
  this->fdDisconnect();
  itsKit->unref();
}


SoNodeKitListPart* FdTire::getListSw() const
{
  return SO_GET_PART(FdDB::getMechanismKit(),"tireListSw",SoNodeKitListPart);
}


bool FdTire::updateFdAll(bool updateChildrenDisplay)
{
  if (!itsFmOwner) return false;

  // Create Tire geometry :

  myFEKit->show(false);

  FmTire* fmTire = (FmTire*)itsFmOwner;
  fmTire->updateFromFile();

  double rr = fmTire->getRimRadius();
  double so = fmTire->spindelTriadOffset.getValue();
  double tr = fmTire->getUnloadedTireRadius();

  double sr = 0.5;
  double ar = 0.83;
  double aw = 0.04;

  const int nSegs = 20;

  std::vector< std::vector<int> > shapeIndexes;
  std::vector<FaVec3> vxes;
  std::vector<int>    line;

  vxes.push_back(FaVec3(     0,      0, 0 ));// 0

  vxes.push_back(FaVec3( sr*rr,      0, so));// 1
  vxes.push_back(FaVec3(     0,  sr*rr, so));// 2
  vxes.push_back(FaVec3(-sr*rr,      0, so));// 3
  vxes.push_back(FaVec3(     0, -sr*rr, so));// 4

  vxes.push_back(FaVec3(    rr,      0, so));// 5
  vxes.push_back(FaVec3(     0,     rr, so));// 6
  vxes.push_back(FaVec3(   -rr,      0, so));// 7
  vxes.push_back(FaVec3(     0,    -rr, so));// 8

  vxes.push_back(FaVec3( ar*rr,  aw*rr, so));// 9
  vxes.push_back(FaVec3( ar*rr, -aw*rr, so));// 10

  // "Spindel"

  line.push_back(2);
  line.push_back(0);
  line.push_back(1);
  shapeIndexes.push_back(line);

  line.clear();
  line.push_back(4);
  line.push_back(0);
  line.push_back(3);
  shapeIndexes.push_back(line);

  // Centerplane diamon

  line.clear();
  line.push_back(1);
  line.push_back(2);
  line.push_back(3);
  line.push_back(4);
  line.push_back(1);
  shapeIndexes.push_back(line);

  // Cross

  line.clear();
  line.push_back(5);
  line.push_back(7);
  shapeIndexes.push_back(line);

  line.clear();
  line.push_back(6);
  line.push_back(8);
  shapeIndexes.push_back(line);

  if (fmTire->hasValidVisData())
    {
      // Roll indicator arrow

      line.clear();
      line.push_back(5);
      line.push_back(9);
      line.push_back(10);
      line.push_back(5);
      shapeIndexes.push_back(line);

      // Rings

      FaVec3 X(1,0,0);
      FaVec3 Y(0,1,0);
      FaVec3 Z(0,0,1);

      // Rim ring

      int seg;
      line.clear();
      for (seg = 0; seg < nSegs; seg++)
        {
          double cl = (2*M_PI*seg)/nSegs;
          vxes.push_back(Z*so + rr*sin(cl)*Y + rr*cos(cl)*X);
          line.push_back(vxes.size()-1);
        }
      line.push_back(vxes.size() - seg);
      shapeIndexes.push_back(line);

      // Tire ring

      line.clear();
      for (seg = 0; seg < nSegs; seg++)
        {
          double cl = (2*M_PI*seg)/nSegs;
          vxes.push_back(Z*so + tr*sin(cl)*Y + tr*cos(cl)*X);
          line.push_back(vxes.size()-1);
        }
      line.push_back(vxes.size() - seg);
      shapeIndexes.push_back(line);
    }

  myFEKit->deleteVisualization();
  myFEKit->setVertexes(vxes);
  myFEKit->addLineShape(shapeIndexes);

  return this->FdObject::updateFdAll(updateChildrenDisplay);
}


bool FdTire::updateFdTopology(bool)
{
  SoTransform* transLocal;
  SoTransform* transLink;
  FmJointBase* bearing = ((FmTire*)itsFmOwner)->bearingJoint.getPointer();
  FmTriad*     slave   = bearing ? bearing->getSlaveTriad() : NULL;

  // Set slave transform and backPt connection
  if (slave)
    {
      transLink = SO_GET_PART(slave->getFdPointer()->getKit(),"firstTrans",SoTransform);
      transLocal = SO_GET_PART(slave->getFdPointer()->getKit(),"secondTrans",SoTransform);
    }
  else
    {
      // Joints should always have a slave triad
      std::cout <<"No slave triad in bearing joint of "
		<< itsFmOwner->getIdString(true) << std::endl;
      transLink = new SoTransform;
      transLocal= new SoTransform;
    }

  itsKit->setPart("transform",transLink);
  itsKit->setPart("transform2",transLocal);

  return true;
}


bool FdTire::updateFdApperance()
{
  // Tire Appearance:
  // This test makes sure we do not unhighlight something
  // when it is supposed to be highlighted
  if (this->highlightRefCount < 1)
    myFEKit->setLook(FFdLook(myColor,0.5f));

  return true;
}


void FdTire::setColor(const FdColor& color)
{
  myColor = color;
  this->updateFdApperance();
}


bool FdTire::updateFdDetails()
{
  FdDB::setAutoRedraw(false);

  myFEKit->setDrawStyle(FdFEVisControl::LINES);
  myFEKit->setDrawDetail(FdFEVisControl::OFF);
  myFEKit->setLineDetail(FdFEVisControl::FULL);
  myFEKit->show(true);
  myFEKit->myGroupParts.update();

  FdDB::setAutoRedraw(true);

  return true;
}


void FdTire::showHighlight()
{
  if (FdDB::usesLineHighlight)
  {
    myFEKit->highlight(true);
    itsKit->setPart("symbolMaterial",FdSymbolDefs::getHighlightMaterial());
  }
  else
  {
    if (this->highlightBoxId)
      FdExtraGraphics::removeBBox(this->highlightBoxId);
    this->highlightBoxId = FdExtraGraphics::showBBox(this->itsKit);
  }
}


void FdTire::hideHighlight()
{
  if (this->highlightBoxId)
    FdExtraGraphics::removeBBox(this->highlightBoxId);

  this->highlightBoxId = NULL;

  myFEKit->highlight(false);

  this->updateFdApperance();
}
