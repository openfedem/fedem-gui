// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FP_PROCESS_MANAGER_H
#define FP_PROCESS_MANAGER_H

#include "FFaLib/FFaPatterns/FFaSingelton.H"
#include "FFaLib/FFaDynCalls/FFaSwitchBoard.H"
#include <set>
#include <map>

class FpProcess;
class FFuaTimer;

typedef std::set<FpProcess*>     ProcessSet;
typedef std::map<int,ProcessSet> ProcessMap;


class FpProcessManager : public FFaSingelton<FpProcessManager>,
                         public FFaSwitchBoardConnector
{
public:
  FpProcessManager();
  virtual ~FpProcessManager();

  enum Signals { STARTED, FINISHED, GROUP_STARTED, GROUP_FINISHED };

  void killAll();

  void setInterval(int msek);
  void checkOnInterval(bool turnOnCheck);

  bool empty() const { return myProcesses.empty(); }

  bool isChecking() const;
  int getInterval() const;

  void explicitCheck() { this->check(); }

protected:
  // used from the process
  void addProcess(FpProcess* aProc);
  void removeProcess(FpProcess* aProc);
  bool haveProcess(FpProcess* aProc) const;

  void check();

  friend class FpProcess;

private:
  ProcessMap myProcesses;
  FFuaTimer* myTimer;
};


template <class T>
class FpProcessGroupChangedReceiver : public FFaSwitchBoardConnector
{
public:
  FpProcessGroupChangedReceiver(T* owner) : myOwner(owner)
  {
    FFaSwitchBoard::connect(FpProcessManager::instance(),
                            FpProcessManager::GROUP_STARTED,
                            FFaSlot1M(FpProcessGroupChangedReceiver,this,onChanged,int));
    FFaSwitchBoard::connect(FpProcessManager::instance(),
                            FpProcessManager::GROUP_FINISHED,
                            FFaSlot1M(FpProcessGroupChangedReceiver,this,onChanged,int));
  }
  virtual ~FpProcessGroupChangedReceiver()
  {
    FFaSwitchBoard::removeAllOwnerConnections(this);
  }

private:
  void onChanged(int gpid) { myOwner->onProcessGroupChanged(gpid); }

  T* myOwner;
};

#endif
