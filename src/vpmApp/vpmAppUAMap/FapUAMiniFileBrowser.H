// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_MINI_FILE_BROWSER_H
#define FAP_UA_MINI_FILE_BROWSER_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAFinishHandler.H"
#include "FFaLib/FFaDynCalls/FFaSwitchBoard.H"
#include <string>
#include <vector>
#include <map>

class FuiMiniFileBrowser;
class FmModelMemberBase;
class FmPart;
class FFrExtractor;
class FFuaCmdItem;


class FapUAMiniFileBrowser : public FapUAExistenceHandler,
                             public FapUAFinishHandler
{
  Fmd_HEADER_INIT();

  //! \brief Struct to keep info about one UI-item (not necessarily a file).
  struct FileSpec
  {
    enum ItemType { HEADER, PART, RESULT_TYPE, FILE };

    FileSpec(ItemType type = HEADER,
             const std::string& name = "",
             const std::string& path = "",
             const std::string& size = "",
             const std::string& mod = "",
             const std::string& res = "",
             FmPart* part_ptr = NULL,
             const char** ico = NULL)
      : itemType(type), uiLabel(name), absPath(path),
        sizeString(size), modified(mod), rdbType(res),
        part(part_ptr), icon(ico) {}

    void clear() {
      uiLabel.clear();
      absPath.clear();
      sizeString.clear();
      modified.clear();
      rdbType.clear();
      part = NULL;
      icon = NULL;
    }

    bool setup(const std::string& file = "", const char* rbd = NULL);
    bool isFile(bool frsOnly = false) const;

    ItemType     itemType;   //!< Type of item
    std::string  uiLabel;    //!< Label in first column in list view
    std::string  absPath;    //!< Absolute file path, if applicable
    std::string  sizeString; //!< Size in string representation
    std::string  modified;   //!< Date and time of last modification
    std::string  rdbType;    //!< RDB type string
    FmPart*      part;       //!< Pointer to part, if applicable
    const char** icon;       //!< Icon in list view
  };

public:
  FapUAMiniFileBrowser(FuiMiniFileBrowser* ui);
  virtual ~FapUAMiniFileBrowser();

protected:
  virtual void finishUI();

private:
  void init();

  void rebuildAll();
  void buildReduction();
  void buildDynamics();
  void buildRecovery();

  void refreshAll();
  void refresh(int headerUiID, FmPart* part = 0);

  void updatePartEntry(FmPart* part);
  void updateExpandedItems();
  void updateSelection();
  void kill();

  // map query
  std::string getMapFileName(int item) const;
  std::string getMapLabel(int item) const;
  FmPart* getMapPart(int item) const;
  int getMapRdbGroup(const std::string& grp) const;
  std::string getItemPath(int id) const;
  bool isDescendantOf(int ancestor, int descendant,
                      bool includeAnscestor = false) const;

  bool filesInSelection(bool frsOnly) const;

  // Insert/remove/update from UI and map
  int insertUIItem(int parent, int after, const FileSpec& spec);
  void updateUIItem(int item, const FileSpec& spec);
  void removeUIItem(int id);
  void removeAllChildrenOf(int parent);

  // Helpers
  void setFrsFileText(const std::string& file);
  void setTxtFileText(const std::string& file);
  void setResFileText(const std::string& file);
  void setText(std::istream& stream, const std::string& file);

  // slots from signal connector
  void onModelExtractorDeleted(FFrExtractor* extr);
  void onModelExtractorHeaderChanged(FFrExtractor* extr);
  void onModelExtractorDataChanged(FFrExtractor* extr);
  void onModelExtractorNew(FFrExtractor* extr);
  void onModelMemberChanged(FmModelMemberBase* item);
  void onModelMemberDisconnected(FmModelMemberBase* item);

  void onSolverFinished(int groupID, int eventID,
                        const std::string& processSign);

  // slots from ui
  void tmpSelectionChanged(int idx);
  void permSelectionChanged();
  void uiPoppedUp(bool poppedUp);
  void onItemExpanded(int item, bool expanded);

  // Commands
  void deleteResultFiles();
  void getDeleteResultFilesSensitivity(bool& sensitivity);

  void enableResultFiles() { this->changeResultState(true); }
  void disableResultFiles() { this->changeResultState(false); }
  void changeResultState(bool enable);
  void getEnableDisableSensitivity(bool& sensitivity);

  enum SortMode { BY_PART, BY_PROCESS } sortMode;

  void sortByPart();
  void sortByProcess();

  class SignalConnector : public FFaSwitchBoardConnector
  {
  public:
    SignalConnector(FapUAMiniFileBrowser* ua);
    virtual ~SignalConnector();

  private:
    void onModelExtractorDeleted(FFrExtractor* extr)
    {this->owner->onModelExtractorDeleted(extr);}
    void onModelExtractorHeaderChanged(FFrExtractor* extr)
    {this->owner->onModelExtractorHeaderChanged(extr);}
    void onModelExtractorDataChanged(FFrExtractor* extr)
    {this->owner->onModelExtractorDataChanged(extr);}
    void onModelExtractorNew(FFrExtractor* extr)
    {this->owner->onModelExtractorNew(extr);}
    void onModelMemberChanged(FmModelMemberBase* item)
    {this->owner->onModelMemberChanged(item);}
    void onModelMemberDisconnected(FmModelMemberBase* item)
    {this->owner->onModelMemberDisconnected(item);}

    FapUAMiniFileBrowser* owner;
  };

  FuiMiniFileBrowser* ui;
  SignalConnector signalConnector;

  std::map<int,FileSpec>      fileMap;
  std::map<std::string,bool>  expandedMap;
  std::map<FmPart*,bool>      expandedPartsMap;
  std::vector<FFuaCmdItem*>   popUpCmds;

  int  dynamicsHeader, recoveryHeader, reducerHeader;
  bool isUIPoppedUp, needsRebuild, needsRefresh, reductionNeedsRebuild;
  bool inInteractiveErase;

  std::string    modelName;
  std::string    myPathToSelectedItem;
  std::string    myMonitoredFileName;
  std::ifstream* myMonitoredFileStream;

  void updateFileMonitoring();
  void cleanFileMonitoring();

  typedef std::map<int,FileSpec>::const_iterator ItemMapCIterator;
};

#endif
