// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"

// UI's and UA's for dyncast in createUA

#include "vpmUI/vpmUITopLevels/FuiAdvAnalysisOptions.H"
#include "vpmUI/vpmUITopLevels/FuiOutputList.H"
#include "vpmUI/vpmUITopLevels/FuiCtrlModeller.H"
#include "vpmUI/vpmUITopLevels/FuiCtrlGridAttributes.H"
#include "vpmUI/vpmUITopLevels/FuiEigenOptions.H"
#include "vpmUI/vpmUITopLevels/FuiAppearance.H"
#include "vpmUI/vpmUITopLevels/FuiPreferences.H"
#include "vpmUI/vpmUITopLevels/FuiStressOptions.H"
#include "vpmUI/vpmUITopLevels/FuiGageOptions.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmUI/vpmUITopLevels/FuiGraphView.H"
#endif
#include "vpmUI/vpmUITopLevels/FuiProperties.H"
#include "vpmUI/vpmUITopLevels/FuiMiniFileBrowser.H"
#include "vpmUI/vpmUITopLevels/FuiLinkRamSettings.H"
#include "vpmUI/vpmUITopLevels/FuiModelPreferences.H"
#include "vpmUI/vpmUITopLevels/FuiSeaEnvironment.H"
#ifdef FT_HAS_WND
#include "vpmUI/vpmUITopLevels/FuiAirEnvironment.H"
#include "vpmUI/vpmUITopLevels/FuiCreateTurbineAssembly.H"
#include "vpmUI/vpmUITopLevels/FuiCreateTurbineTower.H"
#endif
#include "vpmUI/vpmUITopLevels/FuiCreateBeamstringPair.H"
#include "vpmUI/vpmUITopLevels/FuiObjectBrowser.H"
#include "vpmUI/vpmUITopLevels/FuiFppOptions.H"
#include "vpmUI/vpmUITopLevels/FuiViewSettings.H"
#ifdef FT_HAS_NCODE
#include "vpmUI/vpmUITopLevels/FuiDutyCycleOptions.H"
#endif
#include "vpmUI/vpmUITopLevels/FuiRDBSelector.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmUI/vpmUITopLevels/FuiRDBMEFatigue.H"
#endif
#include "vpmUI/vpmUITopLevels/FuiMainWindow.H"
#include "vpmUI/vpmUITopLevels/FuiAnimationControl.H"
#include "vpmUI/vpmUITopLevels/FuiCSSelector.H"
#include "vpmUI/vpmUITopLevels/FuiModelExport.H"

#include "vpmApp/vpmAppUAMap/FapUAAdvAnalysisOptions.H"
#include "vpmApp/vpmAppUAMap/FapUAOutputList.H"
#include "vpmApp/vpmAppUAMap/FapUACtrlModeller.H"
#include "vpmApp/vpmAppUAMap/FapUACtrlElemProperties.H"
#include "vpmApp/vpmAppUAMap/FapUACtrlGridAttributes.H"
#include "vpmApp/vpmAppUAMap/FapUAEigenOptions.H"
#include "vpmApp/vpmAppUAMap/FapUAPreferences.H"
#include "vpmApp/vpmAppUAMap/FapUAStressOptions.H"
#include "vpmApp/vpmAppUAMap/FapUAGageOptions.H"
#include "vpmApp/vpmAppUAMap/FapUAFppOptions.H"
#include "vpmApp/vpmAppUAMap/FapUAViewSettings.H"
#include "vpmApp/vpmAppUAMap/FapUAMainWindow.H"
#include "vpmApp/vpmAppUAMap/FapUAProperties.H"
#include "vpmApp/vpmAppUAMap/FapUAModelPreferences.H"
#include "vpmApp/vpmAppUAMap/FapUASeaEnvironment.H"
#ifdef FT_HAS_WND
#include "vpmApp/vpmAppUAMap/FapUAAirEnvironment.H"
#include "vpmApp/vpmAppUAMap/FapUACreateTurbineAssembly.H"
#include "vpmApp/vpmAppUAMap/FapUACreateTurbineTower.H"
#endif
#include "vpmApp/vpmAppUAMap/FapUACreateBeamstringPair.H"
#include "vpmApp/vpmAppUAMap/FapUAObjectBrowser.H"
#include "vpmApp/vpmAppUAMap/FapUAAppearance.H"
#ifdef FT_HAS_EXTCTRL
#include "vpmApp/vpmAppUAMap/FapUAExtCtrlSysProperties.H"
#endif
#include "vpmApp/vpmAppUAMap/FapUAAnimationDefine.H"
#include "vpmApp/vpmAppUAMap/FapUAAnimationControl.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmApp/vpmAppUAMap/FapUACurveDefine.H"
#include "vpmApp/vpmAppUAMap/FapUAGraphDefine.H"
#include "vpmApp/vpmAppUAMap/FapUACurveAxisDefinition.H"
#include "vpmApp/vpmAppUAMap/FapUAGraphView.H"
#include "vpmApp/vpmAppUAMap/FapUAGraphViewTLS.H"
#endif
#include "vpmApp/vpmAppUAMap/FapUAMainWindow.H"
#include "vpmApp/vpmAppUAMap/FapUAQueryInputField.H"
#include "vpmApp/vpmAppUAMap/FapUAFunctionProperties.H"
#include "vpmApp/vpmAppUAMap/FapUASimModelListView.H"
#include "vpmApp/vpmAppUAMap/FapUAResultListView.H"
#include "vpmApp/vpmAppUAMap/FapUARDBListView.H"
#include "vpmApp/vpmAppUAMap/FapUASimModelRDBListView.H"
#include "vpmApp/vpmAppUAMap/FapUARDBSelector.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmApp/vpmAppUAMap/FapUARDBMEFatigue.H"
#endif
#ifdef FT_HAS_NCODE
#include "vpmApp/vpmAppUAMap/FapUADutyCycleOptions.H"
#endif
#include "vpmApp/vpmAppUAMap/FapUAMiniFileBrowser.H"
#include "vpmApp/vpmAppUAMap/FapUALinkRamSettings.H"
#include "vpmApp/vpmAppUAMap/FapUACSSelector.H"
#include "vpmApp/vpmAppUAMap/FapUACSListView.H"
#include "vpmApp/vpmAppUAMap/FapUAModelExport.H"

#include "vpmUI/vpmUIComponents/FuiCtrlElemProperties.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmUI/vpmUIComponents/FuiCurveDefine.H"
#include "vpmUI/vpmUIComponents/FuiGraphDefine.H"
#include "vpmUI/vpmUIComponents/FuiCurveAxisDefinition.H"
#endif
#include "vpmUI/vpmUIComponents/FuiAnimationDefine.H"
#include "vpmUI/vpmUIComponents/FuiFunctionProperties.H"
#ifdef FT_HAS_EXTCTRL
#include "vpmUI/vpmUIComponents/FuiExtCtrlSysProperties.H"
#endif
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "vpmUI/vpmUIComponents/FuiItemsListViews.H"


std::set<FapUAExistenceHandler*> FapUAExistenceHandler::ourSelfSet;
std::map<FFuUAExistenceHandler*,FapUAExistenceHandler*> FapUAExistenceHandler::ourUIToUAMap;


Fmd_SOURCE_INIT(FAPUAEXISTENCEHANDLER, FapUAExistenceHandler, FapUAExistenceHandler);

//----------------------------------------------------------------------------

FapUAExistenceHandler::FapUAExistenceHandler(FFuUAExistenceHandler* uic)
{
  Fmd_CONSTRUCTOR_INIT(FapUAExistenceHandler);

  this->ui = uic;

  if (uic)
  {
    uic->setDestructUACB(FFaDynCB0M(FapUAExistenceHandler,this,destruct));
    uic->setGetUACB(FFaDynCB1M(FapUAExistenceHandler,this,getUA,FapUAExistenceHandler*&));
    FapUAExistenceHandler::ourUIToUAMap[uic] = this;
  }

  FapUAExistenceHandler::ourSelfSet.insert(this);
}
//----------------------------------------------------------------------------

FapUAExistenceHandler::~FapUAExistenceHandler()
{
  FapUAExistenceHandler::ourSelfSet.erase(this);

  if (this->ui)
    FapUAExistenceHandler::ourUIToUAMap.erase(this->ui);
}
//----------------------------------------------------------------------------

void FapUAExistenceHandler::init()
{
  FFuUAExistenceHandler::setCreateUACB(FFaDynCB1S(FapUAExistenceHandler::createUA,
						  FFuUAExistenceHandler*));
}
//----------------------------------------------------------------------------

void FapUAExistenceHandler::doUpdateState(int oldState, int newState, int mode)
{
  for (FapUAExistenceHandler* self : ourSelfSet)
    self->updateState(oldState,newState,mode);
}
//----------------------------------------------------------------------------

void FapUAExistenceHandler::doUpdateUI(int typeToUpdate)
{
  for (FapUAExistenceHandler* self : ourSelfSet)
    if (typeToUpdate < 0 || self->getTypeID() == typeToUpdate)
    {
      FapUADataHandler* dataHandler = dynamic_cast<FapUADataHandler*>(self);
      if (dataHandler) dataHandler->updateUI();
    }
}
//----------------------------------------------------------------------------

void FapUAExistenceHandler::doUpdateSession()
{
  for (FapUAExistenceHandler* self : ourSelfSet)
  {
    FapUAItemsViewHandler* ivHandler = dynamic_cast<FapUAItemsViewHandler*>(self);
    if (ivHandler) ivHandler->updateSession();
  }
}
//----------------------------------------------------------------------------

void FapUAExistenceHandler::getAllOfType(int typeId, std::vector<FapUAExistenceHandler*>& toFillUp)
{
  toFillUp.clear();
  for (FapUAExistenceHandler* self : ourSelfSet)
    if (self->isOfType(typeId))
      toFillUp.push_back(self);
}
//----------------------------------------------------------------------------

FapUAExistenceHandler* FapUAExistenceHandler::getFirstOfType(int typeId)
{
  for (FapUAExistenceHandler* self : ourSelfSet)
    if (self->isOfType(typeId))
      return self;
  return NULL;
}
//----------------------------------------------------------------------------

bool FapUAExistenceHandler::isUIPoppedUp()
{
  FFuComponentBase* aUi = this->getCompBaseUI();
  return aUi ? aUi->isPoppedUp() : false;
}
//----------------------------------------------------------------------------

void FapUAExistenceHandler::createUA(FFuUAExistenceHandler* ui)
{
  FuiAdvAnalysisOptions* adv;
  FuiEigenOptions* eigen;
  FuiOutputList* outputlist;
  FuiCtrlModeller* ctrlmodeller;
  FuiCtrlElemProperties* ctrlelem;
  FuiCtrlGridAttributes* ctrlatt;
  FuiPreferences* pref;
  FuiStressOptions* stress;
  FuiGageOptions* gage;
  FuiFppOptions* fpp;
  FuiViewSettings* view;
  FuiAppearance* appear;
#ifdef FT_HAS_EXTCTRL
  FuiExtCtrlSysProperties* externalCtrlSysProperties;
#endif
  FuiAnimationDefine* animationDefine;
  FuiAnimationControl* animationControl;
#ifdef FT_HAS_GRAPHVIEW
  FuiCurveDefine* curveDefine;
  FuiGraphDefine* graphDefine;
  FuiCurveAxisDefinition* curveAxis;
  FuiGraphView* graphView;
  FuiGraphViewTLS* graphViewTLS;
  FuiRDBMEFatigue* rdbMEFatigue;
#endif
  FuiMainWindow* mainWindow;
  FuiProperties* properties;
  FuiQueryInputField* engField;
  FuiFunctionProperties* funcProp;
  FuiSimModelListView* simModLV;
  FuiResultListView* resultLV;
  FuiRDBListView* rdbLV;
  FuiSimModelRDBListView* simRDBLV;
  FuiRDBSelector* rdbSel;
#ifdef FT_HAS_NCODE
  FuiDutyCycleOptions* dutyCycle;
#endif
  FuiMiniFileBrowser* fileBrowser;
  FuiLinkRamSettings* linkRamSettings;
  FuiModelPreferences* modelPreferences;
  FuiSeaEnvironment* seaEnv;
#ifdef FT_HAS_WND
  FuiAirEnvironment* airEnv;
  FuiCreateTurbineAssembly* cta;
  FuiCreateTurbineTower* ctt;
#endif
  FuiCreateBeamstringPair* cbp;
  FuiObjectBrowser* fob;
  FuiCSSelector* csSel;
  FuiCrossSectionListView* csLV;
  FuiMaterialListView* matLV;
  FuiModelExport* modelExport;

  if      ((adv = dynamic_cast<FuiAdvAnalysisOptions*>(ui)))
    new FapUAAdvAnalysisOptions(adv);

  else if ((eigen = dynamic_cast<FuiEigenOptions*>(ui)))
    new FapUAEigenOptions(eigen);

  else if ((outputlist = dynamic_cast<FuiOutputList*>(ui)))
    new FapUAOutputList(outputlist);

  else if ((ctrlmodeller = dynamic_cast<FuiCtrlModeller*>(ui)))
    new FapUACtrlModeller(ctrlmodeller);

  else if ((ctrlelem = dynamic_cast<FuiCtrlElemProperties*>(ui)))
    new FapUACtrlElemProperties(ctrlelem);

  else if ((ctrlatt = dynamic_cast<FuiCtrlGridAttributes*>(ui)))
    new FapUACtrlGridAttributes(ctrlatt);

  else if ((pref = dynamic_cast<FuiPreferences*>(ui)))
    new FapUAPreferences(pref);

  else if ((stress = dynamic_cast<FuiStressOptions*>(ui)))
    new FapUAStressOptions(stress);

  else if ((gage = dynamic_cast<FuiGageOptions*>(ui)))
    new FapUAGageOptions(gage);

  else if ((fpp = dynamic_cast<FuiFppOptions*>(ui)))
    new FapUAFppOptions(fpp);

  else if ((view = dynamic_cast<FuiViewSettings*>(ui)))
    new FapUAViewSettings(view);

  else if ((appear = dynamic_cast<FuiAppearance*>(ui)))
    new FapUAAppearance(appear);

#ifdef FT_HAS_EXTCTRL
  else if ((externalCtrlSysProperties = dynamic_cast<FuiExtCtrlSysProperties*>(ui)))
    new FapUAExtCtrlSysProperties(externalCtrlSysProperties);
#endif

  else if ((animationDefine = dynamic_cast<FuiAnimationDefine*>(ui)))
    new FapUAAnimationDefine(animationDefine);

  else if ((animationControl = dynamic_cast<FuiAnimationControl*>(ui)))
    new FapUAAnimationControl(animationControl);

#ifdef FT_HAS_GRAPHVIEW
  else if ((curveDefine = dynamic_cast<FuiCurveDefine*>(ui)))
    new FapUACurveDefine(curveDefine);

  else if ((curveAxis = dynamic_cast<FuiCurveAxisDefinition*>(ui)))
    new FapUACurveAxisDefinition(curveAxis);

  else if ((graphDefine = dynamic_cast<FuiGraphDefine*>(ui)))
    new FapUAGraphDefine(graphDefine);

  else if ((graphView = dynamic_cast<FuiGraphView*>(ui)))
    new FapUAGraphView(graphView);

  else if ((graphViewTLS = dynamic_cast<FuiGraphViewTLS*>(ui)))
    new FapUAGraphViewTLS(graphViewTLS);
#endif

  else if ((mainWindow = dynamic_cast<FuiMainWindow*>(ui)))
    new FapUAMainWindow(mainWindow);

  else if ((properties = dynamic_cast<FuiProperties*>(ui)))
    new FapUAProperties(properties);

  else if ((engField = dynamic_cast<FuiQueryInputField*>(ui)))
    new FapUAQueryInputField(engField);

  else if ((funcProp = dynamic_cast<FuiFunctionProperties*>(ui)))
    new FapUAFunctionProperties(funcProp);

  else if ((simModLV = dynamic_cast<FuiSimModelListView*>(ui)))
    new FapUASimModelListView(simModLV);

  else if ((resultLV = dynamic_cast<FuiResultListView*>(ui)))
    new FapUAResultListView(resultLV);

  else if ((rdbLV = dynamic_cast<FuiRDBListView*>(ui)))
    new FapUARDBListView(rdbLV);

  else if ((simRDBLV = dynamic_cast<FuiSimModelRDBListView*>(ui)))
    new FapUASimModelRDBListView(simRDBLV);

  else if ((rdbSel = dynamic_cast<FuiRDBSelector*>(ui)))
    new FapUARDBSelector(rdbSel);

#ifdef FT_HAS_GRAPHVIEW
  else if ((rdbMEFatigue = dynamic_cast<FuiRDBMEFatigue*>(ui)))
    new FapUARDBMEFatigue(rdbMEFatigue);
#endif

#ifdef FT_HAS_NCODE
  else if ((dutyCycle = dynamic_cast<FuiDutyCycleOptions*>(ui)))
    new FapUADutyCycleOptions(dutyCycle);
#endif

  else if ((fileBrowser = dynamic_cast<FuiMiniFileBrowser*>(ui)))
    new FapUAMiniFileBrowser(fileBrowser);

  else if ((linkRamSettings = dynamic_cast<FuiLinkRamSettings*>(ui)))
    new FapUALinkRamSettings(linkRamSettings);

  else if ((modelPreferences = dynamic_cast<FuiModelPreferences*>(ui)))
    new FapUAModelPreferences(modelPreferences);

  else if ((seaEnv = dynamic_cast<FuiSeaEnvironment*>(ui)))
    new FapUASeaEnvironment(seaEnv);

#ifdef FT_HAS_WND
  else if ((airEnv = dynamic_cast<FuiAirEnvironment*>(ui)))
    new FapUAAirEnvironment(airEnv);

  else if ((cta = dynamic_cast<FuiCreateTurbineAssembly*>(ui)))
    new FapUACreateTurbineAssembly(cta);

  else if ((ctt = dynamic_cast<FuiCreateTurbineTower*>(ui)))
    new FapUACreateTurbineTower(ctt);
#endif

  else if ((cbp = dynamic_cast<FuiCreateBeamstringPair*>(ui)))
    new FapUACreateBeamstringPair(cbp);

  else if ((fob = dynamic_cast<FuiObjectBrowser*>(ui)))
    new FapUAObjectBrowser(fob);

  else if ((csSel = dynamic_cast<FuiCSSelector*>(ui)))
    new FapUACSSelector(csSel);

  else if ((csLV = dynamic_cast<FuiCrossSectionListView*>(ui)))
    new FapUACSListView(csLV);

  else if ((matLV = dynamic_cast<FuiMaterialListView*>(ui)))
    new FapUACSListView(matLV);

  else if ((modelExport = dynamic_cast<FuiModelExport*>(ui)))
    new FapUAModelExport(modelExport);
}
//----------------------------------------------------------------------------

void FapUAExistenceHandler::destruct()
{
  delete this;
}
//----------------------------------------------------------------------------

FFuComponentBase* FapUAExistenceHandler::getCompBaseUI()
{
  return dynamic_cast<FFuComponentBase*>(this->ui);
}
//----------------------------------------------------------------------------
