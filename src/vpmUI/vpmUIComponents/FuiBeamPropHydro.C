// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiBeamPropHydro.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuIOField.H"


void FuiBeamPropHydro::initWidgets()
{
  myBeamHydroToggleButton->setLabel("Enable hydrodynamic properties");

  myBeamHydroDbField->setLabel("Db");
  myBeamHydroDbField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myBeamHydroDbField->setLabelWidth(25);

  myBeamHydroDdField->setLabel("Dd");
  myBeamHydroDdField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myBeamHydroDdField->setLabelWidth(25);

  myBeamHydroCdField->setLabel("Cd");
  myBeamHydroCdField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myBeamHydroCdField->setLabelWidth(25);

  myBeamHydroCaField->setLabel("Ca");
  myBeamHydroCaField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myBeamHydroCaField->setLabelWidth(25);

  myBeamHydroCmField->setLabel("Cm");
  myBeamHydroCmField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myBeamHydroCmField->setLabelWidth(45);

  myBeamHydroCdAxialField->setLabel("Cd_axial");
  myBeamHydroCdAxialField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myBeamHydroCdAxialField->setLabelWidth(45);

  myBeamHydroCaAxialField->setLabel("Ca_axial");
  myBeamHydroCaAxialField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myBeamHydroCaAxialField->setLabelWidth(45);

  myBeamHydroCmAxialField->setLabel("Cm_axial");
  myBeamHydroCmAxialField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myBeamHydroCmAxialField->setLabelWidth(45);

  myBeamHydroCdSpinField->setLabel("Cd_spin");
  myBeamHydroCdSpinField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myBeamHydroCdSpinField->setLabelWidth(45);

  myBeamHydroDiField->setLabel("Di");
  myBeamHydroDiField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  myBeamHydroDiField->setLabelWidth(45);

  this->placeWidgets(this->getWidth(), this->getHeight());
}


void FuiBeamPropHydro::setAcceptedCBs(const FFaDynCB1<double>& aDynCB)
{
  myBeamHydroDbField->setAcceptedCB(aDynCB);
  myBeamHydroDdField->setAcceptedCB(aDynCB);
  myBeamHydroCdField->setAcceptedCB(aDynCB);
  myBeamHydroCaField->setAcceptedCB(aDynCB);
  myBeamHydroCmField->setAcceptedCB(aDynCB);
  myBeamHydroCdAxialField->setAcceptedCB(aDynCB);
  myBeamHydroCaAxialField->setAcceptedCB(aDynCB);
  myBeamHydroCmAxialField->setAcceptedCB(aDynCB);
  myBeamHydroCdSpinField->setAcceptedCB(aDynCB);
  myBeamHydroDiField->setAcceptedCB(aDynCB);
}


void FuiBeamPropHydro::placeWidgets(int, int)
{
  int y = 4;

  myBeamHydroToggleButton->setEdgeGeometry(10, 10+200, y, y+20);
  y += 20 + 4;

  int y1 = y;
  myBeamHydroDbField->setEdgeGeometry(10, 110, y, y+20);
  y += 20 + 4;
  myBeamHydroDdField->setEdgeGeometry(10, 110, y, y+20);
  y += 20 + 4;
  myBeamHydroCdField->setEdgeGeometry(10, 110, y, y+20);
  y += 20 + 4;
  myBeamHydroCaField->setEdgeGeometry(10, 110, y, y+20);

  y = y1;
  int x1 = 130;
  myBeamHydroCmField->setEdgeGeometry(10+x1, 130+x1, y, y+20);
  y += 20 + 4;
  myBeamHydroCdAxialField->setEdgeGeometry(10+x1, 130+x1, y, y+20);
  y += 20 + 4;
  myBeamHydroCaAxialField->setEdgeGeometry(10+x1, 130+x1, y, y+20);
  y += 20 + 4;
  myBeamHydroCmAxialField->setEdgeGeometry(10+x1, 130+x1, y, y+20);

  y = y1;
  int x2 = 280;
  myBeamHydroCdSpinField->setEdgeGeometry(10+x2, 130+x2, y, y+20);
  y += 20 + 4;
  myBeamHydroDiField->setEdgeGeometry(10+x2, 130+x2, y, y+20);
}


void FuiBeamPropHydro::onBeamHydroToggled(bool value)
{
  myBeamHydroDbField->setSensitivity(value);
  myBeamHydroDdField->setSensitivity(value);
  myBeamHydroCdField->setSensitivity(value);
  myBeamHydroCaField->setSensitivity(value);
  myBeamHydroCmField->setSensitivity(value);
  myBeamHydroCdAxialField->setSensitivity(value);
  myBeamHydroCaAxialField->setSensitivity(value);
  myBeamHydroCmAxialField->setSensitivity(value);
  myBeamHydroCdSpinField->setSensitivity(value);
  myBeamHydroDiField->setSensitivity(value);
}


void FuiBeamPropHydro::setPropSensitivity(bool makeSensitive)
{
  myBeamHydroToggleButton->setSensitivity(makeSensitive);

  if (myBeamHydroToggleButton->getValue())
    this->onBeamHydroToggled(makeSensitive);
  else
    this->onBeamHydroToggled(false);
}


void FuiBeamPropHydro::setValues(const std::array<double,10>& values)
{
  myBeamHydroDbField->setValue(values[0]);
  myBeamHydroDdField->setValue(values[1]);
  myBeamHydroCdField->setValue(values[2]);
  myBeamHydroCaField->setValue(values[3]);
  myBeamHydroCmField->setValue(values[4]);
  myBeamHydroCdAxialField->setValue(values[5]);
  myBeamHydroCaAxialField->setValue(values[6]);
  myBeamHydroCmAxialField->setValue(values[7]);
  myBeamHydroCdSpinField->setValue(values[8]);
  myBeamHydroDiField->setValue(values[9]);
}


void FuiBeamPropHydro::getValues(std::array<double,10>& values) const
{
  values = {
    myBeamHydroDbField->getValue(),
    myBeamHydroDdField->getValue(),
    myBeamHydroCdField->getValue(),
    myBeamHydroCaField->getValue(),
    myBeamHydroCmField->getValue(),
    myBeamHydroCdAxialField->getValue(),
    myBeamHydroCaAxialField->getValue(),
    myBeamHydroCmAxialField->getValue(),
    myBeamHydroCdSpinField->getValue(),
    myBeamHydroDiField->getValue()
  };
}

