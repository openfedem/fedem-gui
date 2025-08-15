// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiCurveAxisDefinition.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuLabel.H"

//----------------------------------------------------------------------------

FuiCurveAxisDefinition::FuiCurveAxisDefinition()
{
  resultLabel = NULL;
  this->editButton = NULL;
  this->operMenu = NULL;
}
//----------------------------------------------------------------------------

void FuiCurveAxisDefinition::initWidgets()
{
  operMenu->setOptionChangedCB(FFaDynCB1M(FFaDynCBstr,&operSelectedCB,invoke,
                                          std::string));
  editButton->setActivateCB(FFaDynCB0M(FFaDynCB0,&editResultCB,invoke));
  editButton->setLabel("Edit...");

  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//----------------------------------------------------------------------------

void FuiCurveAxisDefinition::setUIValues(const std::string& result,
					 const Strings& opers,
					 const std::string& selOper)
{
  resultLabel->setLabel(result.c_str());

  operMenu->clearOptions();
  for (const std::string& oper : opers)
    operMenu->addOption(oper.c_str());

  if (!selOper.empty())
    operMenu->selectOption(selOper);

  operMenu->setSensitivity(!opers.empty());
}
//-----------------------------------------------------------------------------

FuiCurveTimeRange::FuiCurveTimeRange()
{
  spaceObj = NULL;
  minField = maxField = NULL;
  operMenu = NULL;
}
//----------------------------------------------------------------------------

void FuiCurveTimeRange::initWidgets()
{
  minField->setMaxWidth(80);
  minField->setAcceptedCB(FFaDynCB1M(FuiCurveTimeRange,this,
                                     onDoubleChanged,double));
  maxField->setMaxWidth(80);
  maxField->setAcceptedCB(FFaDynCB1M(FuiCurveTimeRange,this,
                                     onDoubleChanged,double));

  operMenu->addOption("None");
  operMenu->addOption("Min");
  operMenu->addOption("Max");
  operMenu->addOption("Absolute Max");
  operMenu->addOption("Mean");
  operMenu->addOption("RMS");
  operMenu->setOptionChangedCB(FFaDynCB1M(FuiCurveTimeRange,this,
                                          onOperSelected,std::string));
}
//----------------------------------------------------------------------------

void FuiCurveTimeRange::setObjLabel(const std::string& label)
{
  spaceObj->setLabel(label.c_str());
}
//----------------------------------------------------------------------------

void FuiCurveTimeRange::setUIValues(double tmin, double tmax,
                                    const std::string& selOper)
{
  minField->setValue(tmin);
  maxField->setValue(tmax);

  if (!selOper.empty())
    operMenu->selectOption(selOper);

  maxField->setSensitivity(selOper != "None");
}
//-----------------------------------------------------------------------------

void FuiCurveTimeRange::getUIValues(double& tmin, double& tmax,
                                    std::string& selOper)
{
  tmin    = minField->getDouble();
  tmax    = maxField->getDouble();
  selOper = operMenu->getSelectedOptionStr();
}
//-----------------------------------------------------------------------------
