// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_SOLVER_ID_H
#define FAP_SOLVER_ID_H

namespace FapSolverID
{
  //! \brief Numbers used to identify the running sub-processes.
  enum {
    FAP_DYN_SOLVER = 1,
    FAP_STRESS     = 2,
    FAP_REDUCER    = 3,
    FAP_MODES      = 4,
    FAP_FPP        = 5,
    FAP_GAGE       = 6
  };
};

#endif
