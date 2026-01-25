// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFaLib/FFaProfiler/FFaMemoryProfiler.H"
#include "FFaLib/FFaCmdLineArg/FFaCmdLineArg.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"


namespace FapMemoryProfiler
{
  bool usage(const char* task)
  {
    int memPoll = 0;
    FFaCmdLineArg::instance()->getValue("memPoll",memPoll);
    if (memPoll < 1) return false;

    FFaMsg::dialog(task,FFaMsg::OK);
    FFaMemoryProfiler::reportMemoryUsage(task);
    return true;
  }
}
