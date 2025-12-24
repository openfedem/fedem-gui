// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_EVENT_H
#define FD_EVENT_H

#include <Inventor/nodes/SoEventCallback.h>

class SoSeparator;


namespace FdEvent
{
  void init();
  void start(SoSeparator* dbRoot);
  void addEventCB(SoEventCallbackCB* cb);
  void removeEventCB(SoEventCallbackCB* cb);
}

#endif
