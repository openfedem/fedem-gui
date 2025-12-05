// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FP_PM_H
#define FP_PM_H

#include <vector>
#include <string>


namespace FpPM
{
  //////////////////
  // Initialisation
  //////////////////

  void init(const char* program);
  void start();

  void CopyrightNotice();
  void loadUnitConvertionFile();
  void loadResultPosFiles();
  void loadSNCurveFile();
  void loadPropertyLibraries();
  void loadAllPlugins();

  struct PluginLib
  {
    std::string name;
    std::string sign;
    bool loaded = false;
  };

  void getPluginList(std::vector<PluginLib>& plugins);
  void getActivePlugins(std::vector<std::string>& plugins);
  bool togglePlugin(const std::string& plugin, bool toggleOn);

  const char* getVpmVersion();

  std::string getFullFedemPath(const std::string& fName, char check = 0);

  /////////////////////
  // File And Quitting
  /////////////////////

  void quitVPM(int exitCode = 0); // Nicely quit the application

  bool vpmModelOpen(const std::string& name, bool doLoadLinks = true,
                    const std::string& newModelFileNameForLogFile = "");
  bool vpmAssemblyOpen(const std::string& name, bool doLoadLinks = true);

  bool openTemplateFile(const std::string& modelPath);

  bool vpmModelSaveAs(const std::string& name, bool saveReducedParts = true,
                      bool saveRes = true, double atTime = -1.0);

  bool vpmModelExport(const std::string& newPath, bool solverInput = false);

  void openCmdLineFile();
  void vpmModelNew();
  bool vpmModelSave(bool pruneEmptyDirs = true);
  bool closeModel(bool saveOnBatchExit = true,
                  bool pruneEmptyDirs = true, bool isExiting = false);

  // Undo
  void vpmSetUndoPoint(const char* title);
  void vpmUndo();
  void vpmGetUndoSensitivity(bool& isSensitive);

  // Result and model accessibility interface

  //! \note These enum values are octal numbers (base 8).
  //! They must start with leading 0-digit.
  enum RDBType
  {
    ANY         = 0000,
    PRIMARY     = 0001,
    SECONDARY   = 0002,
    EIGVAL      = 0004,
    STRESS_RCY  = 0010,
    EIGVAL_RCY  = 0020,
    GAGE_RCY    = 0040,
    SUMMARY_RCY = 0100,
    RES_FILE    = 0400
  };

  void setResultFlag();
  bool hasResults(RDBType mask = ANY);

  void dontTouchModel();
  void resetTouchedFlag();
  void touchModel(bool resultsOnly = false, bool forced = false);
  void unTouchModel(bool setReadOnly = false);
  bool isModelTouched(bool ignoreResults = false);
  bool isModelTouchable();
  bool isModelRunable();
  bool isModelEditable();
  bool isModelRestartable();

  void updateGuiTitle();

  std::string createUuid();

  void addRecent(const std::string& name);
  void removeRecent(size_t idx);
  const std::vector<std::string>& recentModels();
};

#endif
