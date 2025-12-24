// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FP_MODEL_RDB_HANDLER_H
#define FP_MODEL_RDB_HANDLER_H

#include <string>
#include <vector>
#include <set>

class FmPart;
class FmMechanism;
class FmResultStatusData;


namespace FpModelRDBHandler
{
  // Scans RDB sub-directories and compares with stored RDB data.
  // Option on what to do with non-stored files.
  // Warning for missing files.
  void RDBOpen(FmResultStatusData* rsd, FmMechanism* mechData,
               bool includeReducerFiles = false, bool askMissingInRSD = false);

  // Closes the active model.
  void RDBClose(FmResultStatusData* currentRSD, FmResultStatusData* initialRSD,
                bool deleteFilesNotInRSD = true, bool clearExtractor = true);

  // Removes non-current result directories.
  // Syncronizes current directories (look for not-know-files).
  void RDBSave(FmResultStatusData* currentRSD, FmResultStatusData* initialRSD,
               bool pruneEmptyDirs = true);

  // Saves data in current RDB to a new location.
  // Resets the RDB counters and copies the data.
  void RDBSaveAs(const std::string& RDBPath,
                 FmResultStatusData* currentRSD, FmResultStatusData* initialRSD,
                 FmMechanism* mech, const std::string& subPath = "");

  // Used by SaveAs to update the model with positions at specified time.
  double updateModel(double atTime);

  // Syncronizes the RDB and extractor with data found on disk.
  void RDBSync(FmResultStatusData* currentRSD, FmMechanism* mech,
               bool updateExtractor = false, bool addResFiles = false);
  void RDBSync(FmResultStatusData* currentRSD, FmMechanism* mech,
               std::vector<std::string>& newFrsFiles,
               bool updateExtractor = true, bool addResFiles = false,
               bool checkExistingRSD = false);
  void RDBSync(FmPart* part, FmMechanism* mech, bool addResFiles = false,
               const std::string& RDBPath = "");
  void RDBSyncOnParts(FmResultStatusData* rsd, FmMechanism* mech);

  void RDBIncrement(FmResultStatusData* currentRSD, FmMechanism* mech,
                    bool updateExtractor = true);
  void RDBRelease(bool reducerFilesToo = false, bool noRenewal = false);

  void getKeys(FmResultStatusData* currentRSD, std::set<double>& validRdbTimes,
               const std::string& rdbResultGroup);
  bool hasResults(FmResultStatusData* currentRSD,
                  const std::string& rdbResultGroup = "");

  // Does the same as RDBIncrement, but only for the selected result group.
  void removeResults(const std::string& rdbResultGroup,
                     FmResultStatusData* currentRSD,
                     FmPart* filterOnPart = NULL);

  // Removes all existing result files.
  bool removeAllFiles(FmResultStatusData* rsd);

  // Removes all frs-files that have been truncated by a solver process.
  void purgeTruncatedResultFiles(FmResultStatusData* rsd);

  void removeResultFiles(const std::set<std::string>& files, FmPart* part);
  void removeResultFiles(const std::set<std::string>& files,
                         FmResultStatusData* rsd);

  void removeResFiles();

  // Enables/disables result files.
  void changeResultFilesState(FmMechanism* mech,
                              const std::set<std::string>& files,
                              bool enable = true);

  void removeDisabledFiles(FmMechanism* mech,
                           const std::set<std::string>& files = {});

  // Caching of time step data.
  void enableTimeStepPreRead(FmResultStatusData* currentRSD,
                             const std::string& rdbResultGroup);
  void disableTimeStepPreRead();
  void clearPreReadTimeStep();
  void clearPartIdMap();
}

#endif
