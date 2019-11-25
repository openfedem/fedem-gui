// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmPM/FpProcessManager.H"
#include "vpmPM/FpProcess.H"
#include "FFuLib/FFuAuxClasses/FFuaTimer.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"

#ifdef FP_DEBUG
#include <iostream>
#endif


FpProcessManager::FpProcessManager()
  : FFaSwitchBoardConnector("FpProcessManager")
{
  myTimer = FFuaTimer::create(FFaDynCB0M(FpProcessManager, this, check));
}


FpProcessManager::~FpProcessManager()
{
  myTimer->stop();
  delete myTimer;
}


void FpProcessManager::check()
{
	if (myProcesses.empty()) return;

	ProcessSet::iterator it, next;
	ProcessMap::iterator pgIt, nextPgIt;

	// Loop over all processes, taking care that the current process
	// and process group might be deleted when evaluated.

	pgIt = nextPgIt = myProcesses.begin();
	it   = next     = pgIt->second.begin();
	while (pgIt != myProcesses.end())
	{
		// check if we have a valid process
		if (it != pgIt->second.end()) next++;

		if (next == pgIt->second.end())
		{
			// found the end of the project group set
			nextPgIt = pgIt;
			nextPgIt++;
			if (nextPgIt != myProcesses.end())
				next = nextPgIt->second.begin();
		}

		if (it != pgIt->second.end())
			if (*it)
				(*it)->update();

		it   = next;
		pgIt = nextPgIt;
	}
}


void FpProcessManager::killAll()
{
  for (ProcessMap::value_type& pg : myProcesses)
    for (FpProcess* proc : pg.second)
      proc->kill();
}


void FpProcessManager::setInterval(int msek)
{
  myTimer->changeInterval(msek);
}


void FpProcessManager::checkOnInterval(bool turnOnCheck)
{
  if (turnOnCheck)
    myTimer->restart();
  else
    myTimer->stop();
}


bool FpProcessManager::isChecking() const
{
  return myTimer->isActive();
}


int FpProcessManager::getInterval() const
{
  return myTimer->getInterval();
}


void FpProcessManager::addProcess(FpProcess* aProc)
{
#ifdef FP_DEBUG
  std::cout <<"FpProcessManager::addProcess(FpProcess*)"<< std::endl;
#endif

  bool doEmitStarted = myProcesses.empty();
  if (doEmitStarted)
    myTimer->start(1000);

  ProcessSet& processGroup = myProcesses[aProc->getGroupID()];
  bool doEmitGroupStarted = processGroup.empty();

  processGroup.insert(aProc);

  if (doEmitStarted)
  {
#ifdef FP_DEBUG
    std::cout <<"Emitting FpProcessManager::STARTED"<< std::endl;
#endif
    FFaSwitchBoardCall(this,STARTED);
  }
  if (doEmitGroupStarted)
  {
#ifdef FP_DEBUG
    std::cout <<"Emitting FpProcessManager::GROUP_STARTED : "
              << aProc->getGroupID() << std::endl;
#endif
    FFaSwitchBoardCall(this,GROUP_STARTED,aProc->getGroupID());
  }
}


void FpProcessManager::removeProcess(FpProcess* proc)
{
#ifdef FP_DEBUG
  std::cout <<"FpProcessManager::removeProcess(FpProcess*)"<< std::endl;
#endif

  ProcessSet& processGroup = myProcesses[proc->getGroupID()];
  processGroup.erase(proc);

  if (processGroup.empty())
  {
    myProcesses.erase(proc->getGroupID());
#ifdef FP_DEBUG
    std::cout <<"Emitting FpProcessManager::GROUP_FINISHED : "
              << proc->getGroupID() << std::endl;
#endif
    FFaSwitchBoardCall(this,GROUP_FINISHED,proc->getGroupID());
  }
  if (myProcesses.empty())
  {
    myTimer->stop();
#ifdef FPPROCESS_DEBUG
    std::cout <<"Emitting FpProcessManager::FINISHED"<< std::endl;
#endif
    FFaSwitchBoardCall(this,FINISHED);
    FFaMsg::list("================================================================================\n\n");
    FFaMsg::setSubTask("");
    FFaMsg::disableSubSteps();
    FFaMsg::disableProgress();
  }
}


bool FpProcessManager::haveProcess(FpProcess* proc) const
{
  if (myProcesses.empty()) return false;

  ProcessMap::const_iterator pgIt = myProcesses.find(proc->getGroupID());
  if (pgIt == myProcesses.end()) return false;
  if (pgIt->second.empty()) return false;

  return pgIt->second.find(proc) != pgIt->second.end();
}
