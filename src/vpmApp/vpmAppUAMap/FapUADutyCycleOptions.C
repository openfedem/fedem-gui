// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUADutyCycleOptions.H"
#include "vpmUI/vpmUITopLevels/FuiDutyCycleOptions.H"
#include "vpmUI/Fui.H"
#include "vpmDB/FmDutyCycleOptions.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmLink.H"
#include "vpmDB/FmDB.H"
#include "vpmPM/FpPM.H"
#include "FFuLib/FFuFileDialog.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

#include <algorithm>


Fmd_SOURCE_INIT(FAPUADUTYCYCLEOPTIONS, FapUADutyCycleOptions, FapUAExistenceHandler);


FapUADutyCycleOptions::FapUADutyCycleOptions(FuiDutyCycleOptions* uic)
  : FapUAExistenceHandler(uic), FapUADataHandler(uic), FapUAFinishHandler(uic), signalConnector(this)
{
  Fmd_CONSTRUCTOR_INIT(FapUADutyCycleOptions);

  this->ui = uic;
  this->ui->setAddEventCB(FFaDynCB0M(FapUADutyCycleOptions,this,addEvent));
}


FFuaUIValues* FapUADutyCycleOptions::createValuesObject()
{
  return new FuaDutyCycleOptionsValues();
}


void FapUADutyCycleOptions::setDBValues(FFuaUIValues* values)
{
  FmDutyCycleOptions* db = FmDB::getDutyCycleOptions();

  FuaDutyCycleOptionsValues* v = (FuaDutyCycleOptionsValues*) values;

  // Clearing
  db->removeAllEvents();
  db->clearLinks();

  // Adding links
  for (size_t i = 0; i < v->links.size(); i++)
    if (v->links[i].second)
      db->addLink((FmLink*) allLinks[i]);

  // Adding events
  for (const FuiEventData& event : v->events)
    db->addEvent(event.fmmPath, event.name, event.repeats, event.master);

  db->setEquivalentUnit(v->equivUnit.first, v->equivUnit.second);

  FpPM::touchModel(); // Indicate that the model needs save
}


void FapUADutyCycleOptions::getDBValues(FFuaUIValues* values)
{
  // Don't create a dutycycle options object here, if we don't have any yet
  FmDutyCycleOptions* dbDCOptions = FmDB::getDutyCycleOptions(false);
  // Create a temporary object to get default values
  if (!dbDCOptions) dbDCOptions = new FmDutyCycleOptions();

  FuaDutyCycleOptionsValues* v = (FuaDutyCycleOptionsValues*) values;

  // Building list of links
  FmDB::getAllOfType(this->allLinks,FmLink::getClassTypeID());
  v->links = this->getUILinks();
  v->events.clear();
  for (const std::pair<const std::string,EventData>& ev : dbDCOptions->getEvents())
    v->events.push_back(FuiEventData(ev.first,ev.second.name,ev.second.repeats,ev.second.isMaster));

  v->equivUnit.first = dbDCOptions->getEquivUnitScale();
  v->equivUnit.second = dbDCOptions->getEquivUnit();

  // Delete the temporary dutycycle options object
  if (!FmDB::getDutyCycleOptions(false)) dbDCOptions->erase();
}


void FapUADutyCycleOptions::finishUI()
{
  Fui::dutyCycleOptionsUI(false,true);
}


void FapUADutyCycleOptions::addEvent()
{
  const std::string& absPath = FmDB::getMechanismObject()->getAbsModelFilePath();
  FFuFileDialog* dialog = FFuFileDialog::create(absPath,"Dialog",FFuFileDialog::FFU_OPEN_FILES,true);
  dialog->setTitle("Add event");
  dialog->addFilter("Fedem Model File","fmm");
  dialog->remember("DutyCycle");
  std::vector<std::string> files = dialog->execute();
  delete dialog;

  for (const std::string& file : files) {
    std::string fileName = FFaFilePath::getRelativeFilename(absPath,file);
    if (!this->ui->eventPresent(fileName))
      this->ui->addEvent(FuiEventData(fileName,"",1,false));
  }
}


void FapUADutyCycleOptions::onModelMemberChanged(FmModelMemberBase* item)
{
  if (!item->isOfType(FmLink::getClassTypeID())) return;

  this->ui->setLinks(this->getUILinks());
}


void FapUADutyCycleOptions::onModelMemberConnected(FmModelMemberBase* item)
{
  if (!item->isOfType(FmLink::getClassTypeID())) return;

  this->allLinks.push_back(item);
  this->ui->setLinks(this->getUILinks());
}


void FapUADutyCycleOptions::onModelMemberDisconnected(FmModelMemberBase* item)
{
  std::vector<FmModelMemberBase*>::iterator it = std::find(allLinks.begin(),allLinks.end(),item);
  if (it == allLinks.end()) return;

  this->allLinks.erase(it);
  this->ui->setLinks(this->getUILinks());
}


std::vector< std::pair<std::string,bool> > FapUADutyCycleOptions::getUILinks()
{
  std::vector< std::pair<std::string,bool> > retVal;

  // Don't create a dutycycle options object here, if we don't have any yet
  FmDutyCycleOptions* db = FmDB::getDutyCycleOptions(false);
  if (!db) return retVal;

  std::vector<FmLink*> dbLinks = db->getLinks();
  for (FmModelMemberBase* link : this->allLinks) {
    FmLink* tmp = (FmLink*)link;
    bool toggleOn = std::find(dbLinks.begin(),dbLinks.end(),tmp) != dbLinks.end();
    retVal.push_back(std::make_pair(tmp->getLinkIDString(),toggleOn));
  }

  return retVal;
}


FapUADutyCycleOptions::SignalConnector::SignalConnector(FapUADutyCycleOptions* ua) : owner(ua)
{
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_CHANGED,
			  FFaSlot1M(SignalConnector,this,onModelMemberChanged,FmModelMemberBase*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_CONNECTED,
			  FFaSlot1M(SignalConnector,this,onModelMemberConnected,FmModelMemberBase*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_DISCONNECTED,
			  FFaSlot1M(SignalConnector,this,onModelMemberDisconnected,FmModelMemberBase*));
}


FapUADutyCycleOptions::SignalConnector::~SignalConnector()
{
  FFaSwitchBoard::removeAllOwnerConnections(this);
}
