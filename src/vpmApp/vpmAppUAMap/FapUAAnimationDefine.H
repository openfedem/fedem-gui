// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_ANIMATION_DEFINE_H
#define FAP_UA_ANIMATION_DEFINE_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUADataHandler.H"
#include "vpmApp/FapEventManager.H"
#include <string>
#include <set>
#include <map>

class FuiAnimationDefine;
class FmAnimation;
class FFrExtractor;
class FFaViewItem;


class FapUAAnimationDefine : public FapUAExistenceHandler, public FapUADataHandler
{
  Fmd_HEADER_INIT();

public:
  FapUAAnimationDefine(FuiAnimationDefine* ui);
  virtual ~FapUAAnimationDefine() {}

protected:
  // from datahandler
  virtual FFuaUIValues* createValuesObject();
  virtual void setDBValues(FFuaUIValues* values);
  virtual void getDBValues(FFuaUIValues* values);

private:
  // slots from EventManager
  void onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
			      const std::vector<FFaViewItem*>& selectedSinceLast,
			      const std::vector<FFaViewItem*>& unselectedSinceLast);

  typedef std::set<std::string>            ResultSet;
  typedef std::pair<std::string,ResultSet> ResultItem;
  typedef std::map<std::string,ResultItem> ResultMap;
  const ResultMap& findResults(const std::string& resultClass);
  static bool findResults(ResultMap& results, const std::string& resultClass);

private:
  FmAnimation* dbanimation;

  std::map<std::string,ResultMap> currentResults; // Stores the current result items available
  // for fringe value selection, depending on the current RDB extractor contents

  // Signal receivers
  class SignalConnector : public FFaSwitchBoardConnector
  {
  public:
    SignalConnector(FapUAAnimationDefine* ua);
    virtual ~SignalConnector();

  private:
    // slot from rdb extractor manager, to force a rebuild of the fringe value menus
    // when the extractor header has changed (and only then)
    void onModelExtrHeaderChanged(FFrExtractor*) { owner->currentResults.clear(); }

    FapUAAnimationDefine* owner;
  };

  SignalConnector rdbSignalConnector;

  FapPermSelChangedReceiver<FapUAAnimationDefine> signalConnector;
  friend class FapPermSelChangedReceiver<FapUAAnimationDefine>;
};

#endif
