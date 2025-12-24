// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdDBPointSelectionData.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdPickedPoints.H"
#include "vpmDisplay/FdExtraGraphics.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmApp/vpmAppUAMap/FapUAModeller.H"
#include "vpmUI/Fui.H"
#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtMainWindow.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFdCadModel/FdCadInfo.H"

#include <QButtonGroup>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "vpmUI/Icons/center.xpm"
#include "vpmUI/Icons/circumference.xpm"
#include "vpmUI/Icons/SnapToEdge.xpm"
#include "vpmUI/Icons/FlipDirection.xpm"


FdDBPointSelectionData::FdDBPointSelectionData()
{
  pPointIdx = 0;
  isUnknownShape = isEdge = showDirection = true;
  isAxisDefined = isDirOnly = false;

  myOnCenterButton = myOnCircumfButton = mySnapOnButton = myFlipDirectionButton = NULL;
  centerBox = circumfBox = flipDirBox = snapOnBox = NULL;
  snapLabel = NULL;
  myGroupBox = NULL;
}


void FdDBPointSelectionData::onButtonToggled(bool)
{
  bool isFlipDirectionOn = myFlipDirectionButton->getToggle();
  if (isDirOnly)
  {
    FdDB::setCreateDirection(isFlipDirectionOn ? -ppNormal : ppNormal);
    FdExtraGraphics::showDirection(FdPickedPoints::getPickedPoint(pPointIdx,true),FdDB::getCreateDirection());
    FapUAModeller::updatePointUI();
    return;
  }

  bool isOnCenter = myOnCenterButton->getToggle();
  bool isSnapOn = mySnapOnButton->getToggle();
  bool createDirDefined = true;

  FaVec3 createDir;
  FaVec3 createPos;

  if (isUnknownShape)
  {
    createPos = isSnapOn ? vxSnappedPPoint : pickedPoint;
    createDir = axis;
  }

  // Has some revolved attributes
  else if (isOnCenter)
  {
    if (isAxisDefined)
    {
      // Position
      // Use ppoint and project to axis.
      FaVec3 Ea = axis;
      Ea.normalize();
      FaVec3 oPP = (isSnapOn ? vxSnappedPPoint : pickedPoint) - origin;
      createPos = (oPP*Ea)*Ea + origin;
    }
    else
    {
      // Sphere
      createPos = origin;
      createDirDefined = false;
    }
    createDir = axis;
  }
  else if (isEdge) // On circumference
  {
    // Position
    createPos = isSnapOn ? vxSnappedPPoint : pickedPoint; // TODO: Radius corrected ?

    // Direction
    // Points towards axis
    FaVec3 Ea = axis;
    Ea.normalize();
    FaVec3 oPP = createPos - origin;
    createDir = ((oPP*Ea)*Ea - oPP).normalize();
  }
  else // On circumference of Surface
  {
    // Position
    createPos = isSnapOn ? vxSnappedPPoint : pickedPoint; // TODO: Radius corrected ?
    // Direction
    FaVec3 oPP = createPos - origin;
    if (isAxisDefined)
    {
      // Point towards axis
      FaVec3 Ea = axis;
      Ea.normalize();
      createDir = ((oPP*Ea)*Ea - oPP).normalize();
      if (!ppNormal.isParallell(createDir,0.1))
      {
        // Not cylinder use ppnormal projected into origin-ppoint-axis plane
        FaVec3 Epa = createDir;
        FaVec3 En = ppNormal;
        En.normalize();
        createDir = (En*Epa)*Epa + (En*Ea)*Ea;
      }
    }
    else // Sphere
      createDir = -oPP.normalize();
  }

  if (pPointIdx == 0)
  {
    FdDB::setCreateDirection(isFlipDirectionOn ? -createDir : createDir);
    if (createDirDefined && showDirection)
      FdExtraGraphics::showDirection(createPos,FdDB::getCreateDirection());
    else
      FdExtraGraphics::hideDirection();
  }
  else
  {
    FdDB::setCreateDirection(isFlipDirectionOn ? -createDir : createDir, 2);
    if (createDirDefined && showDirection)
      FdExtraGraphics::showDirection(createPos,FdDB::getCreateDirection(2));
    else
      FdExtraGraphics::hideDirection();
  }

  FdPickedPoints::setPickedPoint(pPointIdx,true,createPos);
  FapUAModeller::updatePointUI();
}


void FdDBPointSelectionData::createUI()
{
  if (myGroupBox)
    return;

  myOnCenterButton = new FFuQtPushButton();
  myOnCenterButton->setToggleAble(true);
  myOnCenterButton->setPixMap(center_xpm);
  myOnCenterButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  myOnCenterButton->setToggleCB(FFaDynCB1M(FdDBPointSelectionData,this,onButtonToggled,bool));

  myOnCircumfButton = new FFuQtPushButton();
  myOnCircumfButton->setToggleAble(true);
  myOnCircumfButton->setPixMap(circumference_xpm);
  myOnCircumfButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  mySnapOnButton = new FFuQtPushButton();
  mySnapOnButton->setToggleCB(FFaDynCB1M(FdDBPointSelectionData,this,onButtonToggled,bool));
  mySnapOnButton->setPixMap(SnapToEdge_xpm);
  mySnapOnButton->setToggleAble(true);
  mySnapOnButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  myFlipDirectionButton = new FFuQtPushButton();
  myFlipDirectionButton->setToggleAble(true);
  myFlipDirectionButton->setPixMap(FlipDirection_xpm);
  myFlipDirectionButton->setToggleCB(FFaDynCB1M(FdDBPointSelectionData,this,onButtonToggled,bool));
  myFlipDirectionButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  centerBox = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(centerBox);
  layout->addWidget(myOnCenterButton);
  layout->addWidget(new QLabel("On Center"));

  circumfBox = new QWidget();
  layout = new QHBoxLayout(circumfBox);
  layout->addWidget(myOnCircumfButton);
  layout->addWidget(new QLabel("On Cicumference"));

  snapOnBox = new QWidget();
  layout = new QHBoxLayout(snapOnBox);
  layout->addWidget(mySnapOnButton);
  layout->addWidget(snapLabel = new QLabel("Snap to edge"));

  flipDirBox = new QWidget();
  layout = new QHBoxLayout(flipDirBox);
  layout->addWidget(myFlipDirectionButton);
  layout->addWidget(new QLabel("Flip direction"));

  myGroupBox = new QGroupBox("Position options");
  layout = new QVBoxLayout(myGroupBox);
  layout->addWidget(centerBox);
  layout->addWidget(circumfBox);
  layout->addWidget(snapOnBox);
  layout->addWidget(flipDirBox);

  QButtonGroup* buttonGroup = new QButtonGroup(myGroupBox);
  buttonGroup->setExclusive(true);
  buttonGroup->addButton(myOnCenterButton);
  buttonGroup->addButton(myOnCircumfButton);

  myOnCenterButton->toggleOn(true);
  mySnapOnButton->toggleOn(true);
  myFlipDirectionButton->toggleOn(false);
}


void FdDBPointSelectionData::updateUI()
{
  if (isDirOnly)
  {
    flipDirBox->show();
    circumfBox->hide();
    centerBox->hide();
    snapOnBox->hide();
  }
  else
  {
    snapOnBox->show();
    snapLabel->setText("Snap to vertex");

    if (isUnknownShape)
    {
      circumfBox->hide();
      centerBox->hide();
    }
    else
    {
      circumfBox->show();
      centerBox->show();
    }

    if (showDirection)
      flipDirBox->show();
    else
      flipDirBox->hide();
  }
}


void FdDBPointSelectionData::fillUI(FdCadEntityInfo* info, bool edge,
                                    const SbVec3f& pickedPt,
                                    const FaVec3& snappedPt,
                                    const SbVec3f& pickedPn,
                                    const FaMat34& objToWorld,
                                    bool showDir, int ppIdx)
{
  isDirOnly = false;
  pPointIdx = ppIdx;
  isEdge = edge;
  pickedPoint = FdConverter::toFaVec3(pickedPt);
  vxSnappedPPoint = snappedPt;
  ppNormal = FdConverter::toFaVec3(pickedPn),
  axis = objToWorld.direction()*info->axis;
  isAxisDefined = info->myAxisIsValid;
  origin = objToWorld*info->origin;
  showDirection = showDir;
  isUnknownShape = true;

  FaVec3 PP = pickedPoint - origin;
  FaVec3 Ea = axis;
  Ea.normalize();

  const double epsTol = 1.0e-7; // Todo set tolerance more intelligently

  if (isEdge)
  {
    // Find whether PP is on axis
    if (info->type == FdCadEntityInfo::CIRCLE) // Circle or ellipsis
      if ((Ea*(PP*Ea)-PP).sqrLength() > epsTol*epsTol) // PP is not on the axis
	isUnknownShape = false;
  }
  else // Surface
    switch (info->type) {
    case FdCadEntityInfo::SPHERE:
      isUnknownShape = isAxisDefined;
      break;
    case FdCadEntityInfo::CYLINDER:
    case FdCadEntityInfo::CONE:
    case FdCadEntityInfo::TORUS:
    case FdCadEntityInfo::SREV:
      isUnknownShape = false;
      break;
    default: // Plane
      if (!ppNormal.isParallell(axis,0.001) || PP*Ea > epsTol)
        isUnknownShape = false;
    }

  this->createAndUpdateUI();
}


void FdDBPointSelectionData::fillUI(const SbVec3f& pickedPt,
                                    const FaVec3& pickedPn)
{
  isDirOnly = true;
  pickedPoint = FdConverter::toFaVec3(pickedPt);
  ppNormal = pickedPn;

  this->createAndUpdateUI();
}


void FdDBPointSelectionData::createAndUpdateUI()
{
  this->createUI();
  this->updateUI();
  dynamic_cast<FuiQtMainWindow*>(Fui::getMainWindow())->addDefinitionWidget(myGroupBox);
  this->onButtonToggled(true);
}


void FdDBPointSelectionData::hideUI()
{
  if (myGroupBox)
    dynamic_cast<FuiQtMainWindow*>(Fui::getMainWindow())->removeDefinitionWidget(myGroupBox);
}
