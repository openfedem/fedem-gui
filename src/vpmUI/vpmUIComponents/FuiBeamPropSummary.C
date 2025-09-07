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
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuToggleButton.H"


void FuiBeamPropSummary::initWidgets()
{
  myCrossSectionTypeMenu->addOption("Pipe");
  myCrossSectionTypeMenu->addOption("Generic");

  myMaterialDefField->setBehaviour(FuiQueryInputField::REF_NONE);
  myMaterialDefField->setButtonMeaning(FuiQueryInputField::EDIT);

  myImage->setPixMap(pipe_xpm);
  myDependencyButton->setLabel("Break dependency");

  for (FFuLabelField* fld : myFields)
    fld->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  for (int i = 0; i < NFIELD; i++)
    if (i <= DI)
      myFields[i]->setLabelWidth(15);
    else if (i <= KZ || i >= SY)
      myFields[i]->setLabelWidth(25);
    else
      myFields[i]->setLabelWidth(35);

  myFields[DO]->setLabel("Do");
  myFields[DI]->setLabel("Di");
  myFields[GEA]->setLabel("EA");
  myFields[GEIYY]->setLabel("EIyy");
  myFields[GEIZZ]->setLabel("EIzz");
  myFields[GIT]->setLabel("GIt");
  myFields[GML]->setLabel("<font face='Symbol'>r</font>A");
  myFields[GIP]->setLabel("<font face='Symbol'>r</font>Ip");
  myFields[AREA]->setLabel("A");
  myFields[IY]->setLabel("Iyy");
  myFields[IZ]->setLabel("Izz");
  myFields[IP]->setLabel("Ip");
  myFields[KY]->setLabel("ky");
  myFields[KZ]->setLabel("kz");
  myFields[GASY]->setLabel("GAs,y");
  myFields[GASZ]->setLabel("GAs,z");
  myFields[SY]->setLabel("sy");
  myFields[SZ]->setLabel("sz");
}


void FuiBeamPropSummary::setAcceptedCBs(const FFaDynCB1<double>& aDynCB)
{
  for (FFuLabelField* fld : myFields)
    fld->setAcceptedCB(aDynCB);
}


void FuiBeamPropSummary::onBreakDependencyToggled(bool value)
{
  for (int i = AREA; i <= IP; i++)
    myFields[i]->setSensitivity(value);
}


void FuiBeamPropSummary::setPropSensitivity(bool makeSensitive)
{
  myCrossSectionTypeMenu->setSensitivity(makeSensitive);
  myMaterialDefField->setSensitivity(makeSensitive);

  for (int i = DO; i <= GIP; i++)
    myFields[i]->setSensitivity(makeSensitive);

  myDependencyButton->setSensitivity(makeSensitive);
  if (myDependencyButton->getValue())
    this->onBreakDependencyToggled(makeSensitive);
  else
    this->onBreakDependencyToggled(false);

  for (int i = KY; i < NFIELD; i++)
    myFields[i]->setSensitivity(makeSensitive);
}


void FuiBeamPropSummary::setValues(const std::array<double,NFIELD>& values)
{
  for (int i = 0; i < NFIELD; i++)
    myFields[i]->setValue(values[i]);
}


void FuiBeamPropSummary::getValues(std::array<double,NFIELD>& values) const
{
  for (int i = 0; i < NFIELD; i++)
    values[i] = myFields[i]->getValue();
}
