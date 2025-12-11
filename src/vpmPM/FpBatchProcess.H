// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FP_BATCH_PROCESS_H
#define FP_BATCH_PROCESS_H


namespace FpBatchProcess
{
  //! \brief Returns true if the user wants to run batch.
  bool userWantsBatch();
  //! \brief Runs the batch-preparation stuff.
  bool setupBatch();
}

#endif
