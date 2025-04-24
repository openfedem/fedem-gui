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
#include "vpmApp/vpmAppUAMap/FapUAModeller.H"
#include "vpmUI/Fui.H"
#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtMainWindow.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"

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
    FdDB::firstCreateDirection = isFlipDirectionOn ? -ppNormal : ppNormal;
    FdExtraGraphics::showDirection(FdPickedPoints::getPickedPoint(pPointIdx,true),FdDB::firstCreateDirection);
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
    FdDB::firstCreateDirection = isFlipDirectionOn ? -createDir : createDir;

    if (createDirDefined && showDirection)
      FdExtraGraphics::showDirection(createPos,FdDB::firstCreateDirection);
    else
      FdExtraGraphics::hideDirection();
  }
  else
  {
    FdDB::secondCreateDirection = isFlipDirectionOn ? -createDir : createDir;

    if (createDirDefined && showDirection)
      FdExtraGraphics::showDirection(createPos,FdDB::secondCreateDirection);
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

  myGroupBox = new QGroupBox("Position options");
  QVBoxLayout* myLayout = new QVBoxLayout();
  QHBoxLayout* layout = NULL;

  myOnCenterButton = new FFuQtPushButton();
  myOnCenterButton->setToggleAble(true);
  myOnCenterButton->setPixMap(center_xpm);
  myOnCenterButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  myOnCenterButton->setToggleCB(FFaDynCB1M(FdDBPointSelectionData,this,onButtonToggled,bool));

  layout = new QHBoxLayout();
  layout->addWidget(myOnCenterButton);
  layout->addWidget(new QLabel("On Center"));
  centerBox = new QWidget();
  centerBox->setLayout(layout);
  myLayout->addWidget(centerBox);

  myOnCircumfButton = new FFuQtPushButton();
  myOnCircumfButton->setToggleAble(true);
  myOnCircumfButton->setPixMap(circumference_xpm);
  myOnCircumfButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  layout = new QHBoxLayout();
  layout->addWidget(myOnCircumfButton);
  layout->addWidget(new QLabel("On Cicumference"));
  circumfBox = new QWidget();
  circumfBox->setLayout(layout);
  myLayout->addWidget(circumfBox);

  mySnapOnButton = new FFuQtPushButton();
  mySnapOnButton->setToggleCB(FFaDynCB1M(FdDBPointSelectionData,this,onButtonToggled,bool));
  mySnapOnButton->setPixMap(SnapToEdge_xpm);
  mySnapOnButton->setToggleAble(true);
  mySnapOnButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  layout = new QHBoxLayout();
  layout->addWidget(mySnapOnButton);
  layout->addWidget(snapLabel = new QLabel("Snap to edge"));
  snapOnBox = new QWidget();
  snapOnBox->setLayout(layout);
  myLayout->addWidget(snapOnBox);

  myFlipDirectionButton = new FFuQtPushButton();
  myFlipDirectionButton->setToggleAble(true);
  myFlipDirectionButton->setPixMap(FlipDirection_xpm);
  myFlipDirectionButton->setToggleCB(FFaDynCB1M(FdDBPointSelectionData,this,onButtonToggled,bool));
  myFlipDirectionButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  layout = new QHBoxLayout();
  layout->addWidget(myFlipDirectionButton);
  layout->addWidget(new QLabel("Flip direction"));
  flipDirBox = new QWidget();
  flipDirBox->setLayout(layout);
  myLayout->addWidget(flipDirBox);

  myGroupBox->setLayout(myLayout);

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
