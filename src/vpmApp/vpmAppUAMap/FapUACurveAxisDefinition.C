// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUACurveAxisDefinition.H"
#include "vpmUI/vpmUIComponents/FuiCurveAxisDefinition.H"
#include "FFaLib/FFaOperation/FFaOpUtils.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"

#include <algorithm>


Fmd_SOURCE_INIT(FAPUACURVEAXISDEFINITION, FapUACurveAxisDefinition, FapUAExistenceHandler);


//----------------------------------------------------------------------------

FapUACurveAxisDefinition::FapUACurveAxisDefinition(FuiCurveAxisDefinition* uic) : FapUAExistenceHandler(uic)
{
  Fmd_CONSTRUCTOR_INIT(FapUACurveAxisDefinition);

  this->ui = uic;

  this->ui->setEditResultCB(FFaDynCB0M(FapUACurveAxisDefinition,this,editResult));
  this->ui->setOperSelectedCB(FFaDynCB1M(FapUACurveAxisDefinition,this,onOperSelected,std::string));
}

//----------------------------------------------------------------------------

void FapUACurveAxisDefinition::setAxisData(const FFaResultDescription& axisData,
					   const std::string& axisOper)
{
  std::vector<std::string> opers;
  if (!axisData.varRefType.empty())
    opers = FFaOpUtils::findOpers(axisData.varRefType);

  if (!axisOper.empty())
    if (std::find(opers.begin(),opers.end(),axisOper) == opers.end())
      FFaMsg::dialog("Can't find the operation: " + axisOper,FFaMsg::ERROR);

  this->currentResult = axisData;
  this->currentOper = axisOper;
  this->ui->setUIValues(axisData.getText(),opers,axisOper);
}

//----------------------------------------------------------------------------

void FapUACurveAxisDefinition::editResult()
{
  this->editAxisCB.invoke();
}

//----------------------------------------------------------------------------

void FapUACurveAxisDefinition::onOperSelected(std::string oper)
{
  this->currentOper = oper;
  this->axisCompletedCB.invoke();
}

//----------------------------------------------------------------------------
