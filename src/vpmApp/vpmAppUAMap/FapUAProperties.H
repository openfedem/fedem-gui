// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_PROPERTIES_H
#define FAP_UA_PROPERTIES_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUADataHandler.H"
#include "vpmApp/FapEventManager.H"
#include "vpmUI/vpmUIComponents/FuiJointDOF.H"
#include "vpmUI/vpmUIComponents/FuiTopologyView.H"
#include "vpmDB/FmModelMemberConnector.H"

class FmEngine;
class FmLink;
class FmJointBase;
class FuiProperties;
class FuaPropertiesValues;
class FFuaCmdItem;
class FFaViewItem;
class FaVec3;


class FapUAProperties : public FapUAExistenceHandler, public FapUADataHandler
{
  Fmd_HEADER_INIT();

public:
  FapUAProperties(FuiProperties* ui);
  virtual ~FapUAProperties() {}

  void setIgnorePickNotify(bool ignore = true)
  { IAmIgnoringPickNotify = ignore; }
  void setIgnorePickNotifyNotCurves(bool ignore = true)
  { IAmIgnoringPickNotifyNotCurves = ignore; }

  static FapUAProperties* getPropertiesHandler();

  // Mode and state handling

private:
  void onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
                              const std::vector<FFaViewItem*>&,
                              const std::vector<FFaViewItem*>&);
  void onModelMemberChanged(FmModelMemberBase* changedObj);

protected:
  virtual void setDBValues(FFuaUIValues* values);
  virtual void getDBValues(FFuaUIValues* values);

private:
  virtual FFuaUIValues* createValuesObject();

  static bool setDBValues(FmModelMemberBase* fmItem,
                          FuaPropertiesValues* pv,
                          int selectedTab = -1);

  // Callbacks distributed to the creating ui :

  void topologyHighlightCB(int i, bool onOrOff);
  void topologyActivatedCB(int i);
  void onTopViewRightClick(int i, std::vector<FFuaCmdItem*>& cmds);

  void linkMeshCB(bool);
  void linkChangeCB();
  void linkChangeVizCB(const std::string&, bool);

  void swapMasterSlaveCB();
  void addMasterCB();
  void reverseMasterCB();
  void swapJointCB();

  void dofStatusToggledCB(int dof, int stat);
  void setAllDofs(bool fixed);
  void dofSetAllFixed() { this->setAllDofs(true); }
  void dofSetAllFree() { this->setAllDofs(false); }

  void onQIFieldButtonCB(FuiQueryInputFieldValues& v);

  void loadViewAttackPointCB(bool);
  void loadViewAttackWhatCB(bool);
  void loadPickAttackPointCB();
  void applyAttackPointCB(bool isGlobal, FaVec3 point);

  void loadViewFromPointCB(bool);
  void loadViewFromWhatCB(bool);
  void loadPickFromPointCB();

  void loadViewToPointCB(bool);
  void loadViewToWhatCB(bool);
  void loadPickToPointCB();

  void tireDataFileBrowseCB();
  void roadDataFileBrowseCB();
  void raoFileBrowseCB();

  void pickEngineInputSensorCB();

  void simEventSelectedCB();

  // Convenience methods :

public:
  static void onQIFieldButtonCBS(FuiQueryInputFieldValues& v);
private:

  void addTopologyItem(std::vector<FuiTopologyItem>& topology,
		       FmModelMemberBase* item, int level = 0,
		       const std::string& typeAddString = "");
  void addTriadTopology(std::vector<FuiTopologyItem>& topol,
			FmLink* item, int level = 0);
  void addEngineArgumentTopology(std::vector<FuiTopologyItem>& topology,
				 FmEngine* item, const std::string& arg);
  void addEngineUsedByTopology(std::vector<FuiTopologyItem>& topology,
			       FmEngine* item, int level = 0);
  void addJointDescendantTopology(std::vector<FuiTopologyItem>& topology,
                                  FmJointBase* item, int level = 0);

  void getDBJointVariables(FmJointBase* item,
			   std::vector<FuiJointDOFValues>& uiVars);

  static void setDBJointVariables(FmJointBase* item,
				  const std::vector<FuiJointDOFValues>& uiVars,
				  int dofToUpdate = -1);

  // CBs from query fields :

  static void verifyTransSpringFunction(bool& isOK, FmModelMemberBase* item);
  static void verifyRotSpringFunction  (bool& isOK, FmModelMemberBase* item);
  static void verifyTransDamperFunction(bool& isOK, FmModelMemberBase* item);
  static void verifyRotDamperFunction  (bool& isOK, FmModelMemberBase* item);

  // Variables :

  bool IAmIgnoringPickNotify;
  bool IAmIgnoringPickNotifyNotCurves;

  FmModelMemberBase* mySelectedFmItem;
  FuiProperties* myPropertiesUI;

  std::vector<FmModelMemberBase*> mySelectedFmItems;
  std::vector<FmModelMemberBase*> myTopologyViewList;

  // Signal Receiver

  FapPermSelChangedReceiver<FapUAProperties> permSignalConnector;
  FmModelMemberChangedReceiver<FapUAProperties> modelMemberChangedConnector;

  friend class FapPermSelChangedReceiver<FapUAProperties>;
  friend class FmModelMemberChangedReceiver<FapUAProperties>;
};

#endif
