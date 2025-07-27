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
  myToggle->setLabel("Enable hydrodynamic properties");

  for (int i = 0; i < NFIELD; i++)
  {
    myFields[i]->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
    myFields[i]->setLabelWidth(i <= CA ? 25 : 45);
  }

  myFields[DB]->setLabel("Db");
  myFields[DD]->setLabel("Dd");
  myFields[CD]->setLabel("Cd");
  myFields[CA]->setLabel("Ca");
  myFields[CM]->setLabel("Cm");
  myFields[CD_AXIAL]->setLabel("Cd_axial");
  myFields[CA_AXIAL]->setLabel("Ca_axial");
  myFields[CM_AXIAL]->setLabel("Cm_axial");
  myFields[CD_SPIN]->setLabel("Cd_spin");
  myFields[DI]->setLabel("Di");
}


void FuiBeamPropHydro::setToggledCB(const FFaDynCB1<bool>& aDynCB)
{
  myToggle->setToggleCB(aDynCB);
}


void FuiBeamPropHydro::setAcceptedCBs(const FFaDynCB1<double>& aDynCB)
{
  for (FFuLabelField* fld : myFields)
    fld->setAcceptedCB(aDynCB);
}


void FuiBeamPropHydro::onBeamHydroToggled(bool value)
{
  for (FFuLabelField* fld : myFields)
    fld->setSensitivity(value);
}


void FuiBeamPropHydro::setPropSensitivity(bool makeSensitive)
{
  myToggle->setSensitivity(makeSensitive);
  this->onBeamHydroToggled(makeSensitive && myToggle->getValue());
}


void FuiBeamPropHydro::setValues(bool onOrOff,
                                 const std::array<double,NFIELD>& values)
{
  myToggle->setValue(onOrOff);
  for (int i = 0; i < NFIELD; i++)
    myFields[i]->setValue(values[i]);
}


bool FuiBeamPropHydro::getValues(std::array<double,NFIELD>& values) const
{
  for (int i = 0; i < NFIELD; i++)
    values[i] = myFields[i]->getValue();

  return myToggle->getValue();
}

