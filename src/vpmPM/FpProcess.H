// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FP_PROCESS_H
#define FP_PROCESS_H


/*!
  This is an encapsulation of the Qt class QProcess.
  Process management (process maps, etc.) are handled by other classes.

  Notes for the process management:
  - All processes are managed by a simple FpProcessManager.
    The manager singelton creates processes and stores them in a map.
  - Each process is started by the run command. The options are set as
    arguments (options = processName, arguments, DeathHandler and workingDir)
  - Destruction of processes are also done from the FpProcessManager.

  Rewritten and updated to Qt4. 22.05.2013 RHR.
*/

#include <QObject>
#include <QProcess>

#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuaTimer;
struct FpProcessOptions;


class FpProcess : public QObject
{
  Q_OBJECT

public:
  FpProcess(const char* name, int groupID = 0);
  virtual ~FpProcess();

  // Sets and gets a process group id to be used to mark a certain group
  // of processes. The Process manager will send a signal when a process with
  // a new groupId is started, and when the last process in a group is finished.
  int  getGroupID() const { return myGroupID; }
  void setGroupID(int id) { myGroupID = id; }

  // Starts the child process, returning its PID if succeeeded and -1 if not
  int run(const FpProcessOptions& options);

  // Brutally kills the child process.
  // Process termination will be caught on next update() call.
  bool kill(bool noDeathHandling = false);

  // Called by FpProcessManager each timer-tic. 
  // Cleans up and removes the FpProcess if myQProcess is no longer running.
  void update();

  //! \brief Reads from either stdout or stderr of the process.
  void readChannel(QProcess::ProcessChannel channel);

public slots:
  void readStdOut() { this->readChannel(QProcess::StandardOutput); }
  void readStdErr() { this->readChannel(QProcess::StandardError); }
  void processFinished(int, QProcess::ExitStatus) { mFinished = true; }

private:
  int myPID;
  int myGroupID;
  const char* myName;

  QProcess* myQProcess;
  FFaDynCB1<int> myDeathHandler;

  void updateElapsedTime();

  FFuaTimer*   myTimer;
  unsigned int myElapsedTime;

  bool mFinished;
};

#endif
