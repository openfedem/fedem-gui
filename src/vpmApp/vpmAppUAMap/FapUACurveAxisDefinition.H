// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_CURVE_AXIS_DEFINITION_H
#define FAP_UA_CURVE_AXIS_DEFINITION_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "FFaLib/FFaDefinitions/FFaResultDescription.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FuiCurveAxisDefinition;


class FapUACurveAxisDefinition : public FapUAExistenceHandler
{
  Fmd_HEADER_INIT();

public:
  FapUACurveAxisDefinition(FuiCurveAxisDefinition* ui);
  virtual ~FapUACurveAxisDefinition() {}

  void setAxisData(const FFaResultDescription& data,
		   const std::string& oper);

  const FFaResultDescription& getAxisResult() const { return this->currentResult; }
  const std::string& getAxisOper() const { return this->currentOper; }

  void setAxisCompletedCB(const FFaDynCB0& dynCB)
  { this->axisCompletedCB = dynCB; }
  void setEditAxisCB(const FFaDynCB0& dynCB)
  { this->editAxisCB = dynCB; }

private:
  void editResult();
  void onOperSelected(std::string oper);

  FuiCurveAxisDefinition* ui;

  FFaResultDescription currentResult;
  std::string          currentOper;

  FFaDynCB0 axisCompletedCB;
  FFaDynCB0 editAxisCB;
};

#endif
