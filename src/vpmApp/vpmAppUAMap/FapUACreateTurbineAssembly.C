// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUACreateTurbineAssembly.H"
#include "vpmApp/vpmAppUAMap/FapUAQuery.H"
#include "vpmUI/vpmUITopLevels/FuiCreateTurbineAssembly.H"
#include "vpmUI/Fui.H"
#include "vpmPM/FpPM.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmTurbine.H"
#include "vpmDB/FmBladeProperty.H"
#include "FFaLib/FFaAlgebra/FFaMath.H"


Fmd_SOURCE_INIT(FAPUACREATETURBINEASSEMBLY, FapUACreateTurbineAssembly, FapUAExistenceHandler);


//----------------------------------------------------------------------------

FapUACreateTurbineAssembly::FapUACreateTurbineAssembly(FuiCreateTurbineAssembly* uic)
  : FapUAExistenceHandler(uic), FapUADataHandler(uic), FapUAFinishHandler(uic),
    signalConnector(this)
{
  Fmd_CONSTRUCTOR_INIT(FapUACreateTurbineAssembly);

  this->ui = uic;
  this->IAmErasingTurbine = false;
}
//----------------------------------------------------------------------------

FFuaUIValues* FapUACreateTurbineAssembly::createValuesObject()
{
  return new FuaCreateTurbineAssemblyValues();
}
//----------------------------------------------------------------------------

void FapUACreateTurbineAssembly::setDBValues(FFuaUIValues* values)
{
  FuaCreateTurbineAssemblyValues* ctaValues = (FuaCreateTurbineAssemblyValues*) values;

  // Create the turbine object and sub-assemblies here, if it doesn't exist yet.
  // A tower assembly is created only if the given tower height is greater than zero
  FmTurbine* turbine = FmDB::getTurbineObject(ctaValues->geom[FWP::TOWER_T] > 0.0 ? -1 : -2);
  FmTower* tower = NULL;
  FmNacelle* nacelle = NULL;
  FmGenerator* generator = NULL;
  FmGearBox* gearbox = NULL;
  FmShaft* lsShaft = NULL;
  FmShaft* hsShaft = NULL;
  FmRotor* rotor = NULL;

  // Get the other sub-assemblies of the turbine
  turbine->getParts(tower,nacelle,generator,gearbox,lsShaft,hsShaft,rotor);

  // Update the turbine name
  turbine->setUserDescription(ctaValues->name);

  // Update the coordinate system of the whole turbine
  FaMat34 tCS = turbine->toGlobal(FaMat34());
  tCS[3].x(ctaValues->geom[FWP::TOWER_X]);
  tCS[3].y(ctaValues->geom[FWP::TOWER_Y]);
  tCS[3].z(ctaValues->geom[FWP::TOWER_Z]);
  turbine->setGlobalCS(tCS,false);
  turbine->updateLocation('A');
  turbine->draw();

  // Use the gear transmission ratio to flag the presense of a gearbox
  if (gearbox)
  {
    if (ctaValues->drivelineType == 1)
      gearbox->Ratio.setValue(0.0); // direct drive (no gearbox)
    else if (gearbox->Ratio.getValue() == 0.0)
      gearbox->Ratio.setValue(1.0); // with gearbox, set default transmission ratio to 1.0
  }

  // Update bearing locations depending on configuration
  if (nacelle)
  {
    if (ctaValues->bearings == 0)
    {
      nacelle->B1.setValue(0.0);
      nacelle->B2.setValue(0.0);
    }
    else if (ctaValues->bearings == 1)
    {
      nacelle->B1.setValue(ctaValues->geom[FWP::B1]);
      nacelle->B2.setValue(0.0);
    }
    else
    {
      nacelle->B1.setValue(ctaValues->geom[FWP::B1]);
      nacelle->B2.setValue(ctaValues->geom[FWP::B2]);
    }
  }

  // Update the remaining geometry parameters

  if (tower)
    tower->Height.setValue(ctaValues->geom[FWP::TOWER_T]);

  if (rotor)
  {
    rotor->HubApex.setValue(ctaValues->geom[FWP::H1]);
    rotor->HubDiam.setValue(ctaValues->geom[FWP::HR]*2.0);
    rotor->PreCone.setValue(-ctaValues->geom[FWP::BETA]); // note the minus sign
  }

  if (lsShaft)
  {
    lsShaft->Tilt.setValue(ctaValues->geom[FWP::ALPHA]);
    lsShaft->Length.setValue(ctaValues->geom[FWP::D3]);
  }

  if (hsShaft)
  {
    hsShaft->Length.setValue(ctaValues->geom[FWP::D5]);
    hsShaft->Tilt.setValue(ctaValues->geom[FWP::ALPHA]);
  }

  if (generator)
    generator->Length.setValue(ctaValues->geom[FWP::D6]);

  if (nacelle)
  {
    nacelle->M3.setValue(ctaValues->geom[FWP::D2]);
    nacelle->M2.setValue(ctaValues->geom[FWP::D1]);
    nacelle->CoG.setValue(FaVec3(ctaValues->geom[FWP::COG_X],ctaValues->geom[FWP::COG_Y],ctaValues->geom[FWP::COG_Z]));
  }

  if (gearbox)
  {
    gearbox->Length.setValue(ctaValues->geom[FWP::D4]);
    gearbox->O1.setValue(ctaValues->geom[FWP::S]*sin(ctaValues->geom[FWP::THETA]*M_PI/180.0));
    gearbox->O2.setValue(ctaValues->geom[FWP::S]*cos(ctaValues->geom[FWP::THETA]*M_PI/180.0));
  }

  turbine->nBlade.setValue(ctaValues->bladesNum);
  turbine->bladeDef.setRef(static_cast<FmSubAssembly*>(ctaValues->bladesDesign));
  turbine->ctrlSys.setValue(ctaValues->incCtrlSys);

  FpPM::touchModel(); // Indicate that the model needs save
}
//----------------------------------------------------------------------------

void FapUACreateTurbineAssembly::getDBValues(FFuaUIValues* values)
{
  FuaCreateTurbineAssemblyValues* ctaValues = (FuaCreateTurbineAssemblyValues*) values;

  // Don't create a turbine object here, if we don't have any yet
  FmTurbine* turbine = IAmErasingTurbine ? NULL : FmDB::getTurbineObject(0);
  FmTower* tower = NULL;
  FmNacelle* nacelle = NULL;
  FmGenerator* generator = NULL;
  FmGearBox* gearbox = NULL;
  FmShaft* lsShaft = NULL;
  FmShaft* hsShaft = NULL;
  FmRotor* rotor = NULL;

  if (turbine)
  {
    // Get the other sub-assemblies of the turbine
    turbine->getParts(tower,nacelle,generator,gearbox,lsShaft,hsShaft,rotor);
    ctaValues->haveTurbine = turbine->isMovable() ? 1 : 2;
    ctaValues->name = turbine->getUserDescription();
  }
  else
  {
    // Create temporary objects to get default values
    tower     = new FmTower();
    nacelle   = new FmNacelle();
    turbine   = new FmTurbine();
    generator = new FmGenerator();
    gearbox   = new FmGearBox();
    lsShaft   = new FmShaft();
    hsShaft   = lsShaft;
    rotor     = new FmRotor();
    ctaValues->haveTurbine = 0;
    // Set default turbine name. At this point there is no turbine object
    // in the model so no need to number them here (only one is allowed).
    ctaValues->name = "New Turbine";
  }

  // Zero gear transmission ration is interpreted as direct drive
  if (gearbox)
    ctaValues->drivelineType = gearbox->Ratio.getValue() == 0.0 ? 1 : 0;

  // Define bearing configuration
  if (!nacelle || nacelle->B1.getValue() == 0.0)
    ctaValues->bearings     = 0;
  else if (nacelle->B2.getValue() == 0.0)
    ctaValues->bearings     = 1;
  else
    ctaValues->bearings     = 2;

  // Get the remaining geometry parameters

  FaVec3 cog = turbine->toGlobal(FaVec3());
  ctaValues->geom[FWP::TOWER_X] = cog.x();
  ctaValues->geom[FWP::TOWER_Y] = cog.y();
  ctaValues->geom[FWP::TOWER_Z] = cog.z();

  if (tower)
    ctaValues->geom[FWP::TOWER_T] = tower->Height.getValue();

  if (rotor)
  {
    ctaValues->geom[FWP::H1]   = rotor->HubApex.getValue();
    ctaValues->geom[FWP::HR]   = rotor->HubDiam.getValue()*0.5;
    ctaValues->geom[FWP::BETA] =-rotor->PreCone.getValue(); // note the minus sign
  }

  if (lsShaft)
  {
    ctaValues->geom[FWP::ALPHA] = lsShaft->Tilt.getValue();
    ctaValues->geom[FWP::D3]    = lsShaft->Length.getValue();
  }

  if (hsShaft)
    ctaValues->geom[FWP::D5] = hsShaft->Length.getValue();

  if (generator)
    ctaValues->geom[FWP::D6] = generator->Length.getValue();

  if (nacelle)
  {
    ctaValues->geom[FWP::D2] = nacelle->M3.getValue();
    ctaValues->geom[FWP::D1] = nacelle->M2.getValue();
    ctaValues->geom[FWP::B1] = nacelle->B1.getValue();
    ctaValues->geom[FWP::B2] = nacelle->B2.getValue();
    ctaValues->geom[FWP::COG_X] = nacelle->CoG.getValue().x();
    ctaValues->geom[FWP::COG_Y] = nacelle->CoG.getValue().y();
    ctaValues->geom[FWP::COG_Z] = nacelle->CoG.getValue().z();
  }

  if (gearbox)
  {
    ctaValues->geom[FWP::D4]    = gearbox->Length.getValue();
    ctaValues->geom[FWP::S]     = hypot(gearbox->O1.getValue(),gearbox->O2.getValue());
    ctaValues->geom[FWP::THETA] = atan2(gearbox->O1.getValue(),gearbox->O2.getValue())*180.0/M_PI;
  }

  ctaValues->bladesNum      = turbine->nBlade.getValue();
  ctaValues->bladesDesign   = turbine->bladeDef.getPointer();
  ctaValues->incCtrlSys     = turbine->ctrlSys.getValue();

  // Delete the temporary objects
  if (!ctaValues->haveTurbine)
  {
    turbine->erase();
    tower->erase();
    nacelle->erase();
    generator->erase();
    gearbox->erase();
    lsShaft->erase();
    rotor->erase();
  }

  ctaValues->isSensitive = FpPM::isModelEditable();
}
//----------------------------------------------------------------------------

void FapUACreateTurbineAssembly::finishUI()
{
  Fui::turbineAssemblyUI(false,true);
}
//------------------------------------------------------------------------------

FapUACreateTurbineAssembly::SignalConnector::SignalConnector(FapUACreateTurbineAssembly* ua) : owner(ua)
{
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_CONNECTED,
			  FFaSlot1M(SignalConnector,this,
				    onModelMemberConnected,FmModelMemberBase*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_DISCONNECTED,
			  FFaSlot1M(SignalConnector,this,
				    onModelMemberDisconnected,FmModelMemberBase*));
}
//------------------------------------------------------------------------------

FapUACreateTurbineAssembly::SignalConnector::~SignalConnector()
{
  FFaSwitchBoard::removeAllOwnerConnections(this);
}
//------------------------------------------------------------------------------

void FapUACreateTurbineAssembly::SignalConnector::onModelMemberConnected(FmModelMemberBase* item)
{
  FmTurbine* turbine = FmDB::getTurbineObject();
  if (turbine)
  {
    if (dynamic_cast<FmBladeDesign*>(item) == dynamic_cast<FmBladeDesign*>(turbine->bladeDef.getPointer()))
      owner->ui->updateUIValues();
    else if (item->getParentAssembly() == turbine)
      owner->ui->updateUIValues();
  }
}
//------------------------------------------------------------------------------

void FapUACreateTurbineAssembly::SignalConnector::onModelMemberDisconnected(FmModelMemberBase* item)
{
  if (dynamic_cast<FmTurbine*>(item) == FmDB::getTurbineObject())
  {
    owner->IAmErasingTurbine = true;
    owner->ui->updateUIValues();
    owner->IAmErasingTurbine = false;
  }
  else if (item->getParentAssembly() == FmDB::getTurbineObject())
    owner->ui->updateUIValues();
}
//------------------------------------------------------------------------------
