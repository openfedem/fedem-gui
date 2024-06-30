// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FP_PM_H
#define FP_PM_H

#include <map>
#include <vector>
#include <string>

class FmModelMemberBase;
class FmAnalysis;
class FmMechanism;
class FmPart;


class FpPM
{
public:

  //////////////////
  // Initialisation
  //////////////////

  static void init(const char* program);
  static void start();

  static void CopyrightNotice();
  static void loadUnitConvertionFile();
  static void loadResultPosFiles();
  static void loadSNCurveFile();
  static void loadPropertyLibraries();
  static void loadAllPlugins();

  struct PluginLib { std::string name; std::string sign; bool loaded = false; };
  static void getPluginList(std::vector<PluginLib>& plugins);
  static void getActivePlugins(std::vector<std::string>& plugins);
  static bool togglePlugin(const std::string& plugin, bool toggleOn);

  static const char* getVpmVersion();

  static std::string getFullFedemPath(const std::string& fileName,
                                      bool checkHomeFirst = false);

  /////////////////////
  // File And Quitting
  /////////////////////

  static void quitVPM(int exitCode = 0); // Nicely quit the application

  static bool vpmModelOpen(const std::string& name, bool doLoadLinks = true,
			   const std::string& newModelFileNameForLogFile = "");
  static bool vpmAssemblyOpen(const std::string& name, bool doLoadLinks = true);

  static bool openTemplateFile(const std::string& modelPath);

  static bool vpmModelSaveAs(const std::string& name, bool saveReducedParts = true,
                             bool saveRes = true, double atTime = -1.0);

  static bool vpmModelExport(const std::string& name,
                             FmAnalysis* analysis = NULL, const char* model = NULL);

  static void openCmdLineFile();
  static void vpmModelNew();
  static bool vpmModelSave(bool pruneEmptyDirs = true);
  static bool closeModel(bool saveOnBatchExit = true,
			 bool pruneEmptyDirs = true,
			 bool isExiting = false);

  // Undo
  static void vpmSetUndoPoint(const char* title);
  static void vpmUndo();
  static void vpmGetUndoSensitivity(bool& isSensitive);

  // Result and model accessibility interface

  //! \note These enum values are octal numbers (base 8).
  //! They must start with leading 0-digit.
  enum RDBType {
    ANY         = 0000,
    PRIMARY     = 0001,
    SECONDARY   = 0002,
    EIGVAL      = 0004,
    STRESS_RCY  = 0010,
    EIGVAL_RCY  = 0020,
    GAGE_RCY    = 0040,
    SUMMARY_RCY = 0100,
    DUTYCYCLE   = 0200,
    RES_FILE    = 0400
  };

  static void setResultFlag();
  static int  getResultFlag() { return resultsFlag; }
  static bool hasResults(RDBType mask = ANY);

  static int  dontTouchModel();
  static void resetTouchedFlag(int flag) { FpPM::touchedFlag = flag; }
  static void touchModel(bool resultsOnly = false);
  static void unTouchModel(bool setReadOnly = false);
  static bool isModelTouched(bool ignoreResults = false);
  static bool isModelTouchable();
  static bool isModelRunable();
  static bool isModelEditable();
  static bool isModelRestartable();
  static bool isUsingDefaultName() { return isUsingDefaultModelName; }

  static void updateGuiTitle();

  static std::string createUuid();

  static const std::vector<std::string>& recentModels() { return recentFiles; }
  static void addRecent(const std::string& name);
  static void removeRecent(size_t idx);

private:
  static bool loadParts(const std::vector<FmPart*>& allParts);

  // Plugin libraries, and whether they are currently loaded or not
  enum PluginType { NONE, UDEF_FUNC, UDEF_CTRL, UDEF_ELM, UDEF_TYPES };
  struct PluginId {
    PluginType type;
    std::string sign;
    bool loaded;
    PluginId() : type(NONE), loaded(false) {}
    PluginId(PluginType t, const char* s) : type(t), sign(s), loaded(false) {}
  };
  typedef std::map<std::string,PluginId> PluginMap;
  static PluginMap ourPlugins;

  static bool togglePlugin(FmMechanism*, PluginMap::iterator it, char toggle);
  static bool togglePlugin(PluginMap::iterator it, bool toggleOn, FmMechanism*);
  static void toggleActivePlugins(FmMechanism* mech);
  static void saveActivePlugins(FmMechanism* mech);

  // Whether a default name is used, makes "Save" go "Save As..."
  static bool isUsingDefaultModelName;

  // Whether a "Save" is needed
  static int touchedFlag;

  // Application process id
  static int vpmPID;

  // Do we have results or not, and what kind
  static int resultsFlag;

  static std::vector<std::string> recentFiles;

  // Signal handling interface
  static void signalHandler(int sig);

  static void onModelMemberConnected(FmModelMemberBase*);
  static void onModelMemberChanged(FmModelMemberBase*);
};

#endif
