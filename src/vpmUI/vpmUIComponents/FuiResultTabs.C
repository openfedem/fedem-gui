// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiResultTabs.H"
#include "FFuLib/FFuToggleButton.H"


void FuiTriadResults::initWidgets()
{
  myToggles[0]->setLabel("Velocity");
  myToggles[1]->setLabel("Acceleration");
  myToggles[2]->setLabel("Force");
  myToggles[6]->setLabel("Deformation");

  myToggles[3]->setLabel("Velocity");
  myToggles[4]->setLabel("Acceleration");
  myToggles[5]->setLabel("Force");
}


void FuiTriadResults::setToggleCB(const FFaDynCB1<bool>& cb)
{
  for (FFuToggleButton* toggle : myToggles)
    toggle->setToggleCB(cb);
}


void FuiTriadResults::setValues(const std::vector<bool>& vals)
{
  for (size_t i = 0; i < myToggles.size(); i++)
    myToggles[i]->setValue(i < vals.size() ? vals[i] : false);
}


void FuiTriadResults::getValues(std::vector<bool>& vals) const
{
  vals.clear();
  vals.reserve(myToggles.size());
  for (FFuToggleButton* toggle : myToggles)
    vals.push_back(toggle->getValue());
}


void FuiJointResults::initWidgets()
{
  myToggles[0]->setLabel("Deflection");
  myToggles[1]->setLabel("Velocity");
  myToggles[2]->setLabel("Acceleration");
  myToggles[3]->setLabel("Friction/Reaction force");
  myToggles[4]->setLabel("Friction energy");

  myToggles[5]->setLabel("Stiffness");
  myToggles[6]->setLabel("Length");
  myToggles[7]->setLabel("Deflection");
  myToggles[8]->setLabel("Force");
  myToggles[9]->setLabel("Energies");

  myToggles[10]->setLabel("Coefficient");
  myToggles[11]->setLabel("Length");
  myToggles[12]->setLabel("Velocity");
  myToggles[13]->setLabel("Force");
  myToggles[14]->setLabel("Energies");
}


void FuiJointResults::setToggleCB(const FFaDynCB1<bool>& cb)
{
  for (FFuToggleButton* toggle : myToggles)
    toggle->setToggleCB(cb);
}


void FuiJointResults::setValues(const std::vector<bool>& vals)
{
  this->setSpringDamper(vals.size() > 5);

  for (size_t i = 0; i < myToggles.size(); i++)
    myToggles[i]->setValue(i < vals.size() ? vals[i] : false);
}


void FuiJointResults::getValues(std::vector<bool>& vals) const
{
  vals.clear();
  vals.reserve(myToggles.size());
  for (FFuToggleButton* toggle : myToggles)
    vals.push_back(toggle->getValue());
}
