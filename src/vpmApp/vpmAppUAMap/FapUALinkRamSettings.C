// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUALinkRamSettings.H"
#include "vpmApp/vpmAppCmds/FapFileCmds.H"
#include "vpmPM/FpModelRDBHandler.H"
#include "vpmPM/FpPM.H"
#include "vpmUI/vpmUITopLevels/FuiLinkRamSettings.H"
#include "vpmUI/Fui.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmStrainRosette.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmPart.H"
#ifdef USE_INVENTOR
#include "vpmDisplay/FdPart.H"
#endif
#include "FFaLib/FFaDefinitions/FFaMsg.H"


Fmd_SOURCE_INIT(FAPUALINKRAMSETTINGS, FapUALinkRamSettings, FapUAExistenceHandler);


FapUALinkRamSettings::FapUALinkRamSettings(FuiLinkRamSettings* uic)
  : FapUAExistenceHandler(uic), FapUADataHandler(uic), FapUAFinishHandler(uic), signalConnector(this)
{
  Fmd_CONSTRUCTOR_INIT(FapUALinkRamSettings);

  this->ui = uic;
}


FFuaUIValues* FapUALinkRamSettings::createValuesObject()
{
  return new FuaLinkRamSettingsValues();
}


void FapUALinkRamSettings::setDBValues(FFuaUIValues* values)
{
  FuaLinkRamSettingsValues* uiVals = (FuaLinkRamSettingsValues*) values;

  Fui::noUserInputPlease();

  std::vector<FmPart*> parts;
  FmDB::getAllParts(parts);

  std::vector< std::pair<FmPart*,FmPart::RamLevel> > partsToChange;
  for (const FuaLinkRamSettingsValues::RamSetting& lrs : uiVals->rowData)
    for (FmPart* part : parts)
      if (part->getIdPath(false) == lrs.id)
      {
        switch (lrs.ramLevel)
          {
          case 0:
            partsToChange.push_back(std::make_pair(part,FmPart::NOTHING));
            break;
          default:
            partsToChange.push_back(std::make_pair(part,FmPart::FULL_FE));
            break;
          }
        break;
      }

  // check if there is a change in the parts that would require a save

  std::string partList;
  for (const std::pair<FmPart*,FmPart::RamLevel>& part : partsToChange)
    if (part.first && part.second != FmPart::FULL_FE)
      if (part.first->ramUsageLevel.getValue() == FmPart::FULL_FE)
        if (part.first->hasChangedFEdata())
	{
          partList += "\n    " + part.first->getIdPath(true);
          std::string descr = part.first->getUserDescription();
          if (!descr.empty()) partList += " \"" + descr + "\"";
        }

  bool cont = true;
  if (!partList.empty())
  {
    std::string msg = "The following FE part(s) needs to be saved:" + partList
      + "\nPlease save you model file before unloading any of them.\n";
    const char* texts[2] = {"Save Model","Cancel"};
    int retVar = Fui::genericDialog(msg.c_str(),texts,2);
    if (retVar == 0) // save
      FapFileCmds::save();
    else if (retVar == 1) // cancel
      cont = false;
  }

  if (cont && !partsToChange.empty())
  {
    int ipart = 0;
    FFaMsg::pushStatus("Loading/Clearing FE data");
    FFaMsg::enableSubSteps(partsToChange.size());
    for (const std::pair<FmPart*,FmPart::RamLevel>& part : partsToChange)
    {
      FFaMsg::setSubStep(++ipart);
      FapUALinkRamSettings::changeRamUsageLevel(part.first,part.second);
    }
    FFaMsg::disableSubSteps();
    FFaMsg::popStatus();

    FFaMsg::pushStatus("Updating result info");
    FpModelRDBHandler::RDBSyncOnParts(FmDB::getMechanismObject()->getResultStatusData(),
                                      FmDB::getMechanismObject());
    FFaMsg::popStatus();

    FpPM::touchModel(); // Indicate that the model needs save
  }

  Fui::okToGetUserInput();
}


void FapUALinkRamSettings::getDBValues(FFuaUIValues* values)
{
  FuaLinkRamSettingsValues* uiVals = (FuaLinkRamSettingsValues*) values;

  std::vector<FmPart*> parts;
  FmDB::getAllParts(parts);

  uiVals->rowData.resize(parts.size());
  for (size_t i = 0; i < parts.size(); i++)
  {
    uiVals->rowData[i].id          = parts[i]->getIdPath(false);
    uiVals->rowData[i].description = parts[i]->getUserDescription();
    uiVals->rowData[i].ramLevel    = parts[i]->ramUsageLevel.getValue() ? 1 : 0;
  }

  uiVals->ramLevelNames = { "Not Loaded", "Loaded" };
}


void FapUALinkRamSettings::changeRamUsageLevel(FmPart* part, const FmPart::RamLevel& level)
{
  if (!part)
    return;

  if (part->ramUsageLevel.getValue() == level && part->getLinkHandler())
    return;

  if (level == FmPart::NOTHING)
    {
      Fui::noUserInputPlease();
      part->ramUsageLevel.setValue(level);
#ifdef USE_INVENTOR
      ((FdPart*)(part->getFdPointer()))->removeVisualizationData();
#endif
      part->updateCachedCheckSum();
      part->setLinkHandler(NULL);
      part->draw();
      Fui::okToGetUserInput();
    }

  else if (level == FmPart::FULL_FE)
    {
      Fui::noUserInputPlease();
      part->ramUsageLevel.setValue(level);
#ifdef USE_INVENTOR
      ((FdPart*)(part->getFdPointer()))->removeVisualizationData();
#endif
      part->setLinkHandler(NULL);
      part->openFEData();

      if (!part->useGenericProperties.getValue()) {
        Fui::okToGetUserInput();
        part->updateTriadTopologyRefs(true,2);
        Fui::noUserInputPlease();
        FmStrainRosette::syncStrainRosettes(part);
      }
      part->draw();
      Fui::okToGetUserInput();
    }

  else
    {
      if (part->ramUsageLevel.getValue() == FmPart::NOTHING ||
          part->ramUsageLevel.getValue() == FmPart::REDUCED_VIZ)
        {
          Fui::noUserInputPlease();
          part->ramUsageLevel.setValue(level);
#ifdef USE_INVENTOR
          ((FdPart*)(part->getFdPointer()))->removeVisualizationData();
#endif
          part->setLinkHandler(NULL);
          part->openFEData();
          part->draw();
          Fui::okToGetUserInput();
        }

      if (level == FmPart::REDUCED_VIZ)
        {
          // Delete link handler + internal and surface visual representations
        }
      else // level == FmPart::SURFACE_FE
        {
          // Delete internal elements and internal visual representation.
        }
    }

}


void FapUALinkRamSettings::finishUI()
{
  Fui::linkRamSettingsUI(false,true);
}


void FapUALinkRamSettings::onModelMemberChanged(FmModelMemberBase* item)
{
  if (dynamic_cast<FmPart*>(item))
    this->updateUI();
}


void FapUALinkRamSettings::onModelMemberConnected(FmModelMemberBase* item)
{
  if (dynamic_cast<FmPart*>(item))
    this->updateUI();
}


void FapUALinkRamSettings::onModelMemberDisconnected(FmModelMemberBase* item)
{
  if (dynamic_cast<FmPart*>(item))
    this->updateUI();
}


FapUALinkRamSettings::SignalConnector::SignalConnector(FapUALinkRamSettings* lrs) : owner(lrs)
{
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_CHANGED,
			  FFaSlot1M(SignalConnector,this,onModelMemberChanged,FmModelMemberBase*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_CONNECTED,
			  FFaSlot1M(SignalConnector,this,onModelMemberConnected,FmModelMemberBase*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_FINISHED_DISCONNECTED,
			  FFaSlot1M(SignalConnector,this,onModelMemberDisconnected,FmModelMemberBase*));
}


FapUALinkRamSettings::SignalConnector::~SignalConnector()
{
  FFaSwitchBoard::removeAllOwnerConnections(this);
}
