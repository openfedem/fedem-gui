// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiBeamPropSummary.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "vpmUI/Pixmaps/pipe.xpm"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuToggleButton.H"


void FuiBeamPropSummary::initWidgets()
{
  // Combos

  myCrossSectionTypeLabel->setLabel("Cross section type");
  myCrossSectionTypeMenu->addOption("Pipe");
  myCrossSectionTypeMenu->addOption("Generic");

  myMaterialDefLabel->setLabel("Material");
  myMaterialDefField->setBehaviour(FuiQueryInputField::REF_NONE);
  myMaterialDefField->setButtonMeaning(FuiQueryInputField::EDIT);

  // Cross section values

  myDefinitionFrame->setLabel("Definition");

  myDoField->setLabel("Do");
  myDoField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myDoField->setLabelMargin(0);
  myDoField->setLabelWidth(15);

  myDiField->setLabel("Di");
  myDiField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myDiField->setLabelMargin(0);
  myDiField->setLabelWidth(15);

  myImage->setPixMap(pipe_xpm);

  myGEAField->setLabel("EA");
  myGEAField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myGEAField->setLabelMargin(0);
  myGEAField->setLabelWidth(25);

  myGEIyyField->setLabel("EIyy");
  myGEIyyField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myGEIyyField->setLabelMargin(0);
  myGEIyyField->setLabelWidth(25);

  myGEIzzField->setLabel("EIzz");
  myGEIzzField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myGEIzzField->setLabelMargin(0);
  myGEIzzField->setLabelWidth(25);

  myGItField->setLabel("GIt");
  myGItField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myGItField->setLabelMargin(0);
  myGItField->setLabelWidth(25);

  myGmlField->setLabel("<font face='Symbol'>r</font>A");
  myGmlField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myGmlField->setLabelMargin(0);
  myGmlField->setLabelWidth(25);

  myGIpField->setLabel("<font face='Symbol'>r</font>Ip");
  myGIpField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myGIpField->setLabelMargin(0);
  myGIpField->setLabelWidth(25);

  // Dependent properties

  myDependencyFrame->setLabel("Dependent properties");
  myDependencyButton->setLabel("Break dependency");

  myAField->setLabel("A");
  myAField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myAField->setLabelMargin(0);
  myAField->setLabelWidth(25);
  myAField->setSensitivity(false);

  myIyField->setLabel("Iyy");
  myIyField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myIyField->setLabelMargin(0);
  myIyField->setLabelWidth(25);
  myIyField->setSensitivity(false);

  myIzField->setLabel("Izz");
  myIzField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myIzField->setLabelMargin(0);
  myIzField->setLabelWidth(25);
  myIzField->setSensitivity(false);

  myIpField->setLabel("Ip");
  myIpField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myIpField->setLabelMargin(0);
  myIpField->setLabelWidth(25);
  myIpField->setSensitivity(false);

  // Shear

  myShearKFrame->setLabel("Shear reduction factors");

  myShearKYField->setLabel("ky");
  myShearKYField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myShearKYField->setLabelMargin(0);
  myShearKYField->setLabelWidth(25);

  myShearKZField->setLabel("kz");
  myShearKZField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myShearKZField->setLabelMargin(0);
  myShearKZField->setLabelWidth(25);

  myShearGAsyField->setLabel("GAs,y");
  myShearGAsyField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myShearGAsyField->setLabelMargin(0);
  myShearGAsyField->setLabelWidth(35);

  myShearGAszField->setLabel("GAs,z");
  myShearGAszField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myShearGAszField->setLabelMargin(0);
  myShearGAszField->setLabelWidth(35);

  myShearSFrame->setLabel("Shear center offset");

  myShearSYField->setLabel("sy");
  myShearSYField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myShearSYField->setLabelMargin(0);
  myShearSYField->setLabelWidth(25);

  myShearSZField->setLabel("sz");
  myShearSZField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myShearSZField->setLabelMargin(0);
  myShearSZField->setLabelWidth(25);
}


void FuiBeamPropSummary::setAcceptedCBs(const FFaDynCB1<double>& aDynCB)
{
  myDoField->setAcceptedCB(aDynCB);
  myDiField->setAcceptedCB(aDynCB);
  myGEAField->setAcceptedCB(aDynCB);
  myGEIyyField->setAcceptedCB(aDynCB);
  myGEIzzField->setAcceptedCB(aDynCB);
  myGItField->setAcceptedCB(aDynCB);
  myGmlField->setAcceptedCB(aDynCB);
  myGIpField->setAcceptedCB(aDynCB);
  myAField->setAcceptedCB(aDynCB);
  myIyField->setAcceptedCB(aDynCB);
  myIzField->setAcceptedCB(aDynCB);
  myIpField->setAcceptedCB(aDynCB);
  myShearKYField->setAcceptedCB(aDynCB);
  myShearKZField->setAcceptedCB(aDynCB);
  myShearGAsyField->setAcceptedCB(aDynCB);
  myShearGAszField->setAcceptedCB(aDynCB);
  myShearSYField->setAcceptedCB(aDynCB);
  myShearSZField->setAcceptedCB(aDynCB);
}


void FuiBeamPropSummary::placeWidgets(int width, int)
{
  int fontHeight = this->getFontHeigth();
  int y = 4;

  // menus

  myCrossSectionTypeLabel->setEdgeGeometry(10, 110, y, fontHeight);
  myMaterialDefLabel->setEdgeGeometry(120, 310, y, fontHeight);
  y += fontHeight + 4;
  myCrossSectionTypeMenu->setEdgeGeometry(10, 110, y, y+20);
  myMaterialDefField->setEdgeGeometry(120, 310, y, y+20);
  y += 25;

  // definition frame

  myDefinitionFrame->setEdgeGeometry(0, 310, y, y+103);
  y += 20;

  int y1 = y;
  myDoField->setEdgeGeometry(10, 100, y, y+20);
  y += 24;
  myDiField->setEdgeGeometry(10, 100, y, y+20);

  myImage->setEdgeGeometry(160, 285, y1, y1+65);

  y = y1;
  myGEAField->setEdgeGeometry(10, 100, y, y+20);
  y += 24;
  myGEIyyField->setEdgeGeometry(10, 100, y, y+20);
  y += 24;
  myGEIzzField->setEdgeGeometry(10, 100, y, y+20);

  y = y1;
  myGItField->setEdgeGeometry(110, 200, y, y+20);
  y += 24;
  myGmlField->setEdgeGeometry(110, 200, y, y+20);
  y += 24;
  myGIpField->setEdgeGeometry(110, 200, y, y+20);

  // dependent properties

  int x = width - 135;
  y = 4;
  myDependencyFrame->setEdgeGeometry(x, x+130, y, 146);
  x += 10;
  y += 20;
  myDependencyButton->setEdgeGeometry(x, x+115, y, y+20);
  y += 24;
  myAField->setEdgeGeometry(x, x+115, y, y+20);
  y += 24;
  myIyField->setEdgeGeometry(x, x+115, y, y+20);
  y += 24;
  myIzField->setEdgeGeometry(x, x+115, y, y+20);
  y += 24;
  myIpField->setEdgeGeometry(x, x+115, y, y+20);

  // shear

  x = 320;
  y = 4;
  int w = width-x < 265 ? width-x-135 : 130;
  myShearKFrame->setEdgeGeometry(x, x+w, y, 72);
  x += 10;
  y += 20;
  w -= 15;
  myShearKYField->setEdgeGeometry(x, x+w, y, y+20);
  myShearGAsyField->setEdgeGeometry(x, x+w, y, y+20);
  y += 24;
  myShearKZField->setEdgeGeometry(x, x+w, y, y+20);
  myShearGAszField->setEdgeGeometry(x, x+w, y, y+20);
  y += 30;

  myShearSFrame->setEdgeGeometry(x-10, x+w+5, y, y+70);
  y += 20;
  myShearSYField->setEdgeGeometry(x, x+w, y, y+20);
  y += 24;
  myShearSZField->setEdgeGeometry(x, x+w, y, y+20);
}


void FuiBeamPropSummary::onCrossSectionTypeChanged(int value)
{
  if (value == 0)
  {
    myMaterialDefLabel->popUp();
    myMaterialDefField->popUp();

    myDoField->popUp();
    myDiField->popUp();
    myImage->popUp();

    myGEAField->popDown();
    myGEIyyField->popDown();
    myGEIzzField->popDown();
    myGItField->popDown();
    myGmlField->popDown();
    myGIpField->popDown();

    myDependencyFrame->popUp();
    myDependencyButton->popUp();
    myAField->popUp();
    myIyField->popUp();
    myIzField->popUp();
    myIpField->popUp();

    myShearKFrame->setLabel("Shear reduction factors");
    myShearKYField->popUp();
    myShearKZField->popUp();

    myShearGAsyField->popDown();
    myShearGAszField->popDown();
  }
  else
  {
    myMaterialDefLabel->popDown();
    myMaterialDefField->popDown();

    myDoField->popDown();
    myDiField->popDown();
    myImage->popDown();

    myGEAField->popUp();
    myGEIyyField->popUp();
    myGEIzzField->popUp();
    myGItField->popUp();
    myGmlField->popUp();
    myGIpField->popUp();

    myDependencyFrame->popDown();
    myDependencyButton->popDown();
    myAField->popDown();
    myIyField->popDown();
    myIzField->popDown();
    myIpField->popDown();

    myShearKFrame->setLabel("Shear stiffness");
    myShearKYField->popDown();
    myShearKZField->popDown();

    myShearGAsyField->popUp();
    myShearGAszField->popUp();
  }
}


void FuiBeamPropSummary::onBreakDependencyToggled(bool value)
{
  myAField->setSensitivity(value);
  myIyField->setSensitivity(value);
  myIzField->setSensitivity(value);
  myIpField->setSensitivity(value);
}


void FuiBeamPropSummary::setPropSensitivity(bool makeSensitive)
{
  myCrossSectionTypeMenu->setSensitivity(makeSensitive);
  myMaterialDefField->setSensitivity(makeSensitive);
  myDoField->setSensitivity(makeSensitive);
  myDiField->setSensitivity(makeSensitive);
  myGEAField->setSensitivity(makeSensitive);
  myGEIyyField->setSensitivity(makeSensitive);
  myGEIzzField->setSensitivity(makeSensitive);
  myGItField->setSensitivity(makeSensitive);
  myGmlField->setSensitivity(makeSensitive);
  myGIpField->setSensitivity(makeSensitive);

  // Dependent properties
  myDependencyButton->setSensitivity(makeSensitive);
  if (myDependencyButton->getValue())
    this->onBreakDependencyToggled(makeSensitive);
  else
    this->onBreakDependencyToggled(false);

  // Shear
  myShearKYField->setSensitivity(makeSensitive);
  myShearKZField->setSensitivity(makeSensitive);
  myShearGAsyField->setSensitivity(makeSensitive);
  myShearGAszField->setSensitivity(makeSensitive);
  myShearSYField->setSensitivity(makeSensitive);
  myShearSZField->setSensitivity(makeSensitive);
}


void FuiBeamPropSummary::setValues(const std::array<double,18>& values)
{
  std::array<double,18>::const_iterator it = values.begin();
  myDoField->setValue(*(it++));
  myDiField->setValue(*(it++));
  myGEAField->setValue(*(it++));
  myGEIyyField->setValue(*(it++));
  myGEIzzField->setValue(*(it++));
  myGItField->setValue(*(it++));
  myGmlField->setValue(*(it++));
  myGIpField->setValue(*(it++));
  myAField->setValue(*(it++));
  myIyField->setValue(*(it++));
  myIzField->setValue(*(it++));
  myIpField->setValue(*(it++));
  myShearKYField->setValue(*(it++));
  myShearKZField->setValue(*(it++));
  myShearGAsyField->setValue(*(it++));
  myShearGAszField->setValue(*(it++));
  myShearSYField->setValue(*(it++));
  myShearSZField->setValue(*(it++));
}


void FuiBeamPropSummary::getValues(std::array<double,18>& values) const
{
  values = {
    myDoField->getValue(),
    myDiField->getValue(),
    myGEAField->getValue(),
    myGEIyyField->getValue(),
    myGEIzzField->getValue(),
    myGItField->getValue(),
    myGmlField->getValue(),
    myGIpField->getValue(),
    myAField->getValue(),
    myIyField->getValue(),
    myIzField->getValue(),
    myIpField->getValue(),
    myShearKYField->getValue(),
    myShearKZField->getValue(),
    myShearGAsyField->getValue(),
    myShearGAszField->getValue(),
    myShearSYField->getValue(),
    myShearSZField->getValue()
  };
}
