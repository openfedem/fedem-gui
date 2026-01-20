// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFUA_QT_TIMER_H
#define FFUA_QT_TIMER_H

#include <QObject>

#include "FFuLib/FFuAuxClasses/FFuaTimer.H"


class FFuaQtTimer : public QObject, public FFuaTimer
{
public:
  FFuaQtTimer(const FFaDynCB0& cb) : FFuaTimer(cb) {}
  virtual ~FFuaQtTimer() { this->stop(); }

  virtual void start(int interval, bool isSShot);
  virtual void restart();
  virtual void stop();

protected:
  virtual void timerEvent(QTimerEvent* e);

private:
  // disable copy constructor and assignment operator
  FFuaQtTimer(const FFuaQtTimer&) = delete;
  FFuaQtTimer& operator=(const FFuaQtTimer&) = delete;
};

#endif
