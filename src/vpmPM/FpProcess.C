// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

// Rewritten and updated to Qt4. 22.05.2013 RHR

#include <iostream>
#include <cstring>
#include <cstdio>

#include <QStringList>

#if defined(win32) || defined(win64)
#include <windows.h>
#include <winbase.h> // Needed for the PROCESS_INFORMATION struct
#endif

#include "vpmPM/FpProcess.H"
#include "vpmPM/FpProcessOptions.H"
#include "vpmPM/FpProcessManager.H"
#include "FFuLib/FFuAuxClasses/FFuaTimer.H"
#include "FFaLib/FFaDefinitions/FFaAppInfo.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"


FpProcess::FpProcess(const char* name, int groupID)
{
  myQProcess = new QProcess(this);
  myPID = 0;
  myName = name;
  myGroupID = groupID;
  myTimer = FFuaTimer::create(FFaDynCB0M(FpProcess,this,updateElapsedTime));
  myElapsedTime = 0;
  mFinished = false;
}


FpProcess::~FpProcess()
{
  if (myQProcess->state() == QProcess::Running)
    myQProcess->kill();

  FpProcessManager::instance()->removeProcess(this);
  delete myQProcess;

  myTimer->stop();
  delete myTimer;
}


void FpProcess::update()
{	
  // If process is still running. Do nothing
  if (!mFinished) return;

  // Process has finished/was shut down. Clean up
  int exitStatus = myQProcess->exitCode() ? -1 : myQProcess->exitStatus();

  // Stop timer, print elapsed time, and clear time-display
  myTimer->stop();
  int hour =  myElapsedTime/3600;
  int min  = (myElapsedTime/60)%60;
  int sec  =  myElapsedTime%60;
  char buf[64];
  sprintf(buf, " [%d]: Finished. Wall time elapsed: %02d:%02d:%02d\n",
	  myPID, hour, min, sec);
  ListUI << myName << buf;
  FFaMsg::clearTime();

  myDeathHandler.invoke(exitStatus);
  delete this;
}


/*!
  Spawns the child process with the given \a options.
  Returns the PID of the child process if succesful launch.
*/

int FpProcess::run(const FpProcessOptions& options)
{
  // Check if process is not already running
  if (myQProcess->state() != QProcess::NotRunning)
    return -1;

  std::string program(FFaAppInfo::checkProgramPath(options.name));
  if (program.empty())
  {
    FFaMsg::list(" *** Error: Non-existing program file "+ options.name +"\n",true);
    return -1;
  }

  myDeathHandler = options.deathHandler;

  QString     command;
  QStringList arguments;
  if (options.prefix.empty())
    command = program.c_str();
  else
  {
    // Split the process prefix (typically ssh -n user@host) into single words
    char* s = strdup(options.prefix.c_str());
    char* p = strtok(s," ");
    command = p;
    while ((p = strtok(NULL," ")))
      arguments.append(p);
    free(s);
  }
  for (const std::string& arg : options.args)
    arguments.append(arg.c_str());

  if (!options.workingDir.empty())
    myQProcess->setWorkingDirectory(options.workingDir.c_str());

#ifdef linux64
  QProcessEnvironment env;
  env.insert("LD_LIBRARY_PATH",FFaAppInfo::getProgramPath().c_str());
  myQProcess->setProcessEnvironment(env);
#endif

  connect(myQProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdOut()));
  connect(myQProcess,SIGNAL(readyReadStandardError()),this,SLOT(readStdErr()));
  connect(myQProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(processFinished(int,QProcess::ExitStatus)));

  // Try to start the process
  myQProcess->start(command,arguments);
  if (myQProcess->state() == QProcess::NotRunning)
  {
    ListUI <<" *** Error: Failed to start "<< command.toStdString();
    for (const std::string& arg : options.args) ListUI <<" "<< arg;
    FFaMsg::list("\n",true);
    return -1;
  }

  myQProcess->closeWriteChannel();

  myTimer->start(1000);
  FFaMsg::displayTime(0,0,0);

  FpProcessManager::instance()->addProcess(this);

  // Find process identification
  myPID = myQProcess->processId();
  ListUI << myName <<" ["<< myPID <<"]: Started.";
  if (program != myName) ListUI <<" ("<< program <<")";
  ListUI <<"\n";
  return myPID;
}


bool FpProcess::kill(bool noDeathHandling)
{
  if (noDeathHandling)
    myDeathHandler.erase();

  // Let the process manager loop catch this, if possible
  myQProcess->kill();

  // Check if the process manager actually does have this process
  // (it doesn't if the process failed to start, etc.)
  return FpProcessManager::instance()->haveProcess(this);
}


void FpProcess::updateElapsedTime()
{
  myElapsedTime++;
  int hour =  myElapsedTime/3600;
  int min  = (myElapsedTime/60)%60;
  int sec  =  myElapsedTime%60;
  FFaMsg::displayTime(hour,min,sec);
}


void FpProcess::readChannel(QProcess::ProcessChannel channel)
{
  char buf[1024];
  myQProcess->setReadChannel(channel);
  while (myQProcess->canReadLine())
    if (myQProcess->readLine(buf,sizeof(buf)) > 0)
    {
      if (channel == QProcess::StandardError)
        std::cerr << myName <<" ["<< myPID <<"]: "<< buf;
      else
        ListUI << myName <<" ["<< myPID <<"]: "<< buf;
    }
}
