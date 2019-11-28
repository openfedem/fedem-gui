// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUACreateTurbineTower.H"
#include "vpmApp/vpmAppUAMap/FapUAQuery.H"
#include "vpmUI/vpmUITopLevels/FuiCreateTurbineTower.H"
#include "vpmUI/Fui.H"
#include "vpmPM/FpPM.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmTurbine.H"
#include "vpmDB/FmMaterialProperty.H"


Fmd_SOURCE_INIT(FAPUACREATETURBINETOWER, FapUACreateTurbineTower, FapUAExistenceHandler);


//----------------------------------------------------------------------------

FapUACreateTurbineTower::FapUACreateTurbineTower(FuiCreateTurbineTower* uic)
  : FapUAExistenceHandler(uic), FapUADataHandler(uic), FapUAFinishHandler(uic),
    signalConnector(this)
{
  Fmd_CONSTRUCTOR_INIT(FapUACreateTurbineTower);

  this->ui = uic;
  this->ui->setCB(FFaDynCB1M(FapUACreateTurbineTower,this,onMaterialSelected,FmModelMemberBase*));
}
//----------------------------------------------------------------------------

FFuaUIValues* FapUACreateTurbineTower::createValuesObject()
{
  return new FuaCreateTurbineTowerValues();
}
//----------------------------------------------------------------------------

void FapUACreateTurbineTower::setDBValues(FFuaUIValues* values)
{
  FuaCreateTurbineTowerValues* twrValues = (FuaCreateTurbineTowerValues*) values;

  // Don't create a turbine object here, if we don't have any yet
  FmTower* tower = NULL;
  FmTurbine* turbine = FmDB::getTurbineObject(0);
  if (!turbine) return;

  std::vector<FmModelMemberBase*> allAss;
  FmDB::getAllOfType(allAss,FmSubAssembly::getClassTypeID(),turbine);
  for (size_t i = 0; i < allAss.size() && !tower; i++)
    tower = dynamic_cast<FmTower*>(allAss[i]);
  if (!tower) return;

  double H2 = tower->Height.getValue() - twrValues->H1;
  std::vector<FmTwrSegment> newData; newData.reserve(2);
  newData.push_back(FmTwrSegment(twrValues->H1,twrValues->D1,twrValues->N1));
  newData.push_back(FmTwrSegment(H2,twrValues->D2,twrValues->N2));
  tower->segments.getValue().swap(newData);
  tower->Thick.setValue(twrValues->wallThickness);
  tower->material.setRef(static_cast<FmMaterialProperty*>(twrValues->materialObject));

  FpPM::touchModel(); // Indicate that the model needs save
}
//----------------------------------------------------------------------------

void FapUACreateTurbineTower::getDBValues(FFuaUIValues* values)
{
  FuaCreateTurbineTowerValues* twrValues = (FuaCreateTurbineTowerValues*) values;

  // Don't create a turbine object here, if we don't have any yet
  FmTower* tower = NULL;
  FmTurbine* turbine = FmDB::getTurbineObject(0);
  twrValues->haveTurbine = turbine != NULL;
  if (twrValues->haveTurbine)
  {
    std::vector<FmModelMemberBase*> allAss;
    FmDB::getAllOfType(allAss,FmSubAssembly::getClassTypeID(),turbine);
    for (size_t i = 0; i < allAss.size() && !tower; i++)
      tower = dynamic_cast<FmTower*>(allAss[i]);
    if (!tower) return; // Should not happen
  }
  else
    tower = new FmTower(); // Temporary tower object, to get default values

  std::vector<FmTwrSegment> tdata(tower->segments.getValue());
  tdata.resize(2);

  twrValues->M1 = tower->Height.getValue();
  twrValues->H1 = tdata[0].H;
  twrValues->D1 = tdata[0].D;
  twrValues->D2 = tdata[1].D;
  twrValues->N1 = tdata[0].N;
  twrValues->N2 = tdata[1].N;
  twrValues->wallThickness = tower->Thick.getValue();
  twrValues->materialObject = tower->material.getPointer();
  twrValues->materialQuery = FapUAMaterialPropQuery::instance();
  twrValues->editCB = FFaDynCB1S(FapUAQuery::onQIFieldButtonCB,FuiQueryInputFieldValues&);

  // Delete the temporary object
  if (!twrValues->haveTurbine)
    tower->erase();

  twrValues->isSensitive = FpPM::isModelEditable();
}
//----------------------------------------------------------------------------

void FapUACreateTurbineTower::finishUI()
{
  Fui::turbineTowerUI(false,true);
}
//----------------------------------------------------------------------------

void FapUACreateTurbineTower::onMaterialSelected(FmModelMemberBase* current)
{
  FmMaterialProperty* mat = dynamic_cast<FmMaterialProperty*>(current);
  if (mat)
    this->ui->setValues(mat->Rho.getValue(),mat->E.getValue(),mat->nu.getValue(),mat->G.getValue());
  else
    this->ui->setValues(0.0,0.0,0.0,0.0);
}
//----------------------------------------------------------------------------

FapUACreateTurbineTower::SignalConnector::SignalConnector(FapUACreateTurbineTower* ua) : owner(ua)
{
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_DISCONNECTED,
			  FFaSlot1M(SignalConnector,this,
				    onModelMemberDisconnected,FmModelMemberBase*));
}
//----------------------------------------------------------------------------

FapUACreateTurbineTower::SignalConnector::~SignalConnector()
{
  FFaSwitchBoard::removeAllOwnerConnections(this);
}
//------------------------------------------------------------------------------

void FapUACreateTurbineTower::SignalConnector::onModelMemberDisconnected(FmModelMemberBase* item)
{
  if (dynamic_cast<FmTurbine*>(item) == FmDB::getTurbineObject())
    owner->ui->updateUIValues();
  else if (dynamic_cast<FmTower*>(item) && item->getParentAssembly() == FmDB::getTurbineObject())
    owner->ui->updateUIValues();
}
//------------------------------------------------------------------------------
