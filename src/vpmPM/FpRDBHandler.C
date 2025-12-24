// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmPM/FpPM.H"
#include "vpmPM/FpProcessManager.H"
#include "vpmPM/FpRDBExtractorManager.H"
#include "vpmPM/FpModelRDBHandler.H"
#include "vpmApp/vpmAppCmds/FapAnimationCmds.H"
#include "vpmApp/vpmAppProcess/FapSolutionProcessMgr.H"
#include "vpmApp/vpmAppProcess/FapSimEventHandler.H"
#include "vpmDB/FmDB.H"
#include "FFuLib/FFuAuxClasses/FFuaTimer.H"
#include "FFrLib/FFrExtractor.H"
#include "FFaLib/FFaCmdLineArg/FFaCmdLineArg.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"


namespace
{
  FFuaTimer* ourHeaderChangedTimer = NULL;
  FFuaTimer* ourDataChangedTimer = NULL;


  void checkForNewHeaders()
  {
    // Check for new res-files also (for progress polling)
    FpModelRDBHandler::RDBSync(FapSimEventHandler::getActiveRSD(),
                               FmDB::getMechanismObject(),true,true);
    FapSolutionProcessManager::instance()->syncRunningProcesses();
  }


  void checkForNewData()
  {
    FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();
    if (extr) extr->doResultFilesUpdate();
  }


  void startRDBChecking(int groupId)
  {
    ListUI <<"Starting process group "<< groupId <<".\n";

    int deltaT = 500;
    FFaCmdLineArg::instance()->getValue("checkRDBinterval",deltaT);
    ourHeaderChangedTimer->start(deltaT);
    ourDataChangedTimer->start(deltaT);

    FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
                            FpRDBExtractorManager::MODELEXTRACTOR_DATA_CHANGED,
                            FFaSlot1S(FapAnimationCmds,onModelExtrDataChanged,FFrExtractor*));

    FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
                            FpRDBExtractorManager::MODELEXTRACTOR_HEADER_CHANGED,
                            FFaSlot1S(FapAnimationCmds,onModelExtrHeaderChanged,FFrExtractor*));
  }


  void stopRDBChecking()
  {
    ListUI <<"No more processes.\n";

    ourHeaderChangedTimer->stop();
    ourDataChangedTimer->stop();

    checkForNewHeaders();
    checkForNewData();

    FpModelRDBHandler::removeResFiles();

    FFaSwitchBoard::disConnect(FpRDBExtractorManager::instance(),
                               FpRDBExtractorManager::MODELEXTRACTOR_DATA_CHANGED,
                               FFaSlot1S(FapAnimationCmds,onModelExtrDataChanged,FFrExtractor*));

    FFaSwitchBoard::disConnect(FpRDBExtractorManager::instance(),
                               FpRDBExtractorManager::MODELEXTRACTOR_HEADER_CHANGED,
                               FFaSlot1S(FapAnimationCmds,onModelExtrHeaderChanged,FFrExtractor*));
  }
}


void FpPM::start()
{
  ourHeaderChangedTimer = FFuaTimer::create(FFaDynCB0S(checkForNewHeaders));
  ourDataChangedTimer   = FFuaTimer::create(FFaDynCB0S(checkForNewData));

  FFaSwitchBoard::connect(FpProcessManager::instance(),
                          FpProcessManager::GROUP_STARTED,
                          new FFaStaticSlot1<int>(startRDBChecking));
  FFaSwitchBoard::connect(FpProcessManager::instance(),
                          FpProcessManager::FINISHED,
                          new FFaStaticSlot0(stopRDBChecking));
  FFaSwitchBoard::connect(FpProcessManager::instance(),
                          FpProcessManager::FINISHED,
                          FFaSlot0S(FapAnimationCmds,onSimulationFinished));
}
