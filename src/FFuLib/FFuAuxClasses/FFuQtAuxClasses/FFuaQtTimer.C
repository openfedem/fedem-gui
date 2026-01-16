// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QTimerEvent>

#include "FFuaQtTimer.H"


FFuaTimer* FFuaTimer::create(const FFaDynCB0& cb)
{
  return new FFuaQtTimer(cb);
}


void FFuaQtTimer::start(int msecInterval, bool isSShot)
{
  this->stop();

  myMsecInterval = msecInterval;
  amISShot = isSShot;
  myTimerID = this->startTimer(myMsecInterval);
}


void FFuaQtTimer::restart()
{
  this->stop();

  myTimerID = this->startTimer(myMsecInterval);
}


void FFuaQtTimer::stop()
{
  if (myTimerID > 0)
    this->killTimer(myTimerID);

  myTimerID = -1;
}


void FFuaQtTimer::timerEvent(QTimerEvent* e)
{
  // Ignore other all other timer events
  if (e->timerId() == myTimerID)
  {
    // Stop single-shot timer
    if (amISShot)
      this->stop();

    myTimerCB.invoke();
  }
}

