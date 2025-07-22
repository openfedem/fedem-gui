// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiVariableType.H"
#include "FFuLib/FFuOptionMenu.H"


void FuiVariableType::initWidgets()
{
  myTypeMenu->addOption("Deflection");
  myTypeMenu->addOption("Velocity");
  myTypeMenu->addOption("Acceleration");
  myTypeMenu->setOptionSelectedCB(FFaDynCB1M(FuiVariableType,this,onMenuSelected,int));
}


void FuiVariableType::setChangedCB(const FFaDynCB0& aDynCB)
{
  myChangedCB = aDynCB;
}


void FuiVariableType::onMenuSelected(int)
{
  myChangedCB.invoke();
}


void FuiVariableType::setValue(int selection)
{
  myTypeMenu->selectOption(selection);
}


int FuiVariableType::getValue() const
{
  return myTypeMenu->getSelectedOption();
}


void FuiVariableType::setSensitivity(bool isSensitive)
{
  myTypeMenu->setSensitivity(isSensitive);
}
