// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAAnimationDefine.H"
#include "vpmApp/vpmAppUAMap/FapUAQuery.H"
#include "vpmApp/vpmAppCmds/FapAnimationCmds.H"
#include "vpmApp/FapLicenseManager.H"
#include "vpmUI/vpmUIComponents/FuiAnimationDefine.H"
#include "vpmUI/Fui.H"

#include "vpmPM/FpPM.H"
#include "vpmPM/FpRDBExtractorManager.H"
#include "FFrLib/FFrExtractor.H"
#include "FFaLib/FFaDefinitions/FFaResultDescription.H"
#include "FFaLib/FFaOperation/FFaOpUtils.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmAnimation.H"
#include "vpmDB/FmAnalysis.H"
#include "vpmDB/FmElementGroupProxy.H"
#include "vpmDB/FmPart.H"

#include <algorithm>


Fmd_SOURCE_INIT(FAPUAANIMATIONDEFINE, FapUAAnimationDefine, FapUAExistenceHandler);


//----------------------------------------------------------------------------

FapUAAnimationDefine::FapUAAnimationDefine(FuiAnimationDefine* uic)
  : FapUAExistenceHandler(uic), FapUADataHandler(uic),
    rdbSignalConnector(this), signalConnector(this)
{
  Fmd_CONSTRUCTOR_INIT(FapUAAnimationDefine);

  this->dbanimation = NULL;

  uic->setLoadAnimCB(FFaDynCB0S(FapAnimationCmds::show));
}
//----------------------------------------------------------------------------

FFuaUIValues* FapUAAnimationDefine::createValuesObject()
{
  return new FuaAnimationDefineValues();
}
//----------------------------------------------------------------------------

static bool weAreUpdatingDB = false;

void FapUAAnimationDefine::setDBValues(FFuaUIValues* values)
{
  if (!this->dbanimation) return;

  FuaAnimationDefineValues* animValues = (FuaAnimationDefineValues*) values;

  // COMMON
  // #2965: Turn on fringes if toggeling to summary animation
  if (animValues->isSummaryAnim && !this->dbanimation->isSummaryAnimation.getValue())
    this->dbanimation->showFringes = true;

  else if (this->dbanimation->loadFringeData.getValue()     != animValues->loadFringe ||
           this->dbanimation->loadLineFringeData.getValue() != animValues->loadLineFringe)
    this->dbanimation->showFringes = animValues->loadFringe || animValues->loadLineFringe;

  this->dbanimation->isModesAnimation   = animValues->isModesAnim;
  this->dbanimation->isSummaryAnimation = animValues->isSummaryAnim;

  this->dbanimation->loadFringeData = animValues->loadFringe;
  this->dbanimation->loadLineFringeData = animValues->loadLineFringe;
  this->dbanimation->loadDeformationData = animValues->loadDef;

  if (animValues->autoExport >= 0)
    this->dbanimation->autoExport = animValues->autoExport;

  // FRINGE
  // value
  this->dbanimation->fringeResultClass = animValues->selResultClass;
  this->dbanimation->fringeVariableName = animValues->selResult;
  this->dbanimation->fringeToScalarOpName = animValues->selResultOper;
  // result set
  this->dbanimation->resultSetSelectionByName = animValues->resultSetByName;
  this->dbanimation->resSetSelectionOpName = animValues->selResultSetOper;
  this->dbanimation->resSetName = animValues->selResultSet;
  // averaging
  this->dbanimation->averagingItem = animValues->selAverOn;
  this->dbanimation->averagingOpName = animValues->selAverOper;
  this->dbanimation->maxMembraneAngleToAverage = animValues->shellAngle;
  this->dbanimation->averagingAcrossElmTypes = animValues->acrossElemType;
  this->dbanimation->averagingAcrossMaterials = animValues->acrossMatBorder;
  this->dbanimation->averagingAcrossProperties = animValues->acrossPropBorder;
  this->dbanimation->multiFaceAveragingOpName = animValues->selElemGrpOper;

  // TIME
  this->dbanimation->usingTimeInterval = !animValues->complSimul;
  this->dbanimation->timeRange = std::make_pair(animValues->start,animValues->stop);
  this->dbanimation->makeFrameForMostFrequentResult = animValues->mostFrames;

  // MODES
  this->dbanimation->eigenmodeType = (FmAnimation::EigenModeType)animValues->modeTyp;
  this->dbanimation->eigenmodePart = static_cast<FmPart*>(animValues->selLink);
  this->dbanimation->eigenmodeTime = animValues->selTime;
  this->dbanimation->eigenmodeNr = animValues->selMode;
  this->dbanimation->eigenmodeAmplitude = animValues->modeScale;
  this->dbanimation->eigenmodeFramesPrCycle = animValues->framesPrCycle;
  this->dbanimation->eigenmodeDurationUseTime = animValues->timeLength;
  this->dbanimation->eigenmodeDurationTime = animValues->timeLengthVal;
  this->dbanimation->eigenmodeDurationUseNCycles = animValues->nCycles;
  this->dbanimation->eigenmodeDurationNCycles = animValues->nCyclesVal;
  this->dbanimation->eigenmodeDurationUseUntilDamped = animValues->untilDamped;
  this->dbanimation->eigenmodeDurationUntilDamped = animValues->untilDampedVal;

  FpPM::touchModel(); // To indicate that model has changed, etc.

  weAreUpdatingDB = true;
}
//----------------------------------------------------------------------------

void FapUAAnimationDefine::getDBValues(FFuaUIValues* values)
{
  if (!this->dbanimation) return;

#ifdef FAP_DEBUG
  std::cout <<"\nFapUAAnimationDefine::getDBValues(): "
            << this->dbanimation->getIdString(true) << std::endl;
#endif

  FuaAnimationDefineValues* animValues = (FuaAnimationDefineValues*) values;

  // COMMON
  animValues->isModesAnim = this->dbanimation->isModesAnimation.getValue();
  animValues->isSummaryAnim = this->dbanimation->isSummaryAnimation.getValue();

  bool changedDB = false;
  if (animValues->isModesAnim)
    changedDB = this->dbanimation->loadFringeData.setValue(animValues->loadFringe = false);
  else if (animValues->isSummaryAnim)
    changedDB = this->dbanimation->loadFringeData.setValue(animValues->loadFringe = true);
  else
    animValues->loadFringe = this->dbanimation->loadFringeData.getValue();

  if (animValues->isModesAnim || animValues->isSummaryAnim)
    changedDB |= this->dbanimation->loadLineFringeData.setValue(animValues->loadLineFringe = false);
  else
    animValues->loadLineFringe = this->dbanimation->loadLineFringeData.getValue();

  if (animValues->isModesAnim && this->dbanimation->eigenmodeType.getValue() > 0)
    changedDB |= this->dbanimation->loadDeformationData.setValue(animValues->loadDef = true);
  else if (animValues->isSummaryAnim)
    changedDB |= this->dbanimation->loadDeformationData.setValue(animValues->loadDef = false);
  else
    animValues->loadDef = this->dbanimation->loadDeformationData.getValue();

  if (changedDB)
    FpPM::touchModel(); // Indicate that the model has changed

  if (FapLicenseManager::hasFeature("FA-VTF"))
    animValues->autoExport = this->dbanimation->autoExport.getValue();
  else // hide toggle, feature not available
    animValues->autoExport = -1;

  // FRINGE
  // value
  animValues->selResultClass = this->dbanimation->fringeResultClass.getValue();
  animValues->selResult = this->dbanimation->fringeVariableName.getValue();
  animValues->selResultOper = this->dbanimation->fringeToScalarOpName.getValue();
  // result set
  animValues->resultSetByName = this->dbanimation->resultSetSelectionByName.getValue();
  animValues->selResultSetOper = this->dbanimation->resSetSelectionOpName.getValue();
  animValues->selResultSet = this->dbanimation->resSetName.getValue();
  // averaging
  animValues->selAverOn = this->dbanimation->averagingItem.getValue();
  animValues->selAverOper = this->dbanimation->averagingOpName.getValue();
  animValues->shellAngle = this->dbanimation->maxMembraneAngleToAverage.getValue();
  animValues->acrossElemType = this->dbanimation->averagingAcrossElmTypes.getValue();
  animValues->acrossMatBorder = this->dbanimation->averagingAcrossMaterials.getValue();
  animValues->acrossPropBorder = this->dbanimation->averagingAcrossProperties.getValue();
  animValues->selElemGrpOper = this->dbanimation->multiFaceAveragingOpName.getValue();

  // TIME
  animValues->complSimul = !this->dbanimation->usingTimeInterval.getValue();
  animValues->start = this->dbanimation->timeRange.getValue().first;
  animValues->stop = this->dbanimation->timeRange.getValue().second;
  animValues->mostFrames = this->dbanimation->makeFrameForMostFrequentResult.getValue();

  // MODES
  static FapUAQuery partQuery;
  partQuery.clear();
  partQuery.typesToFind[FmPart::getClassTypeID()] = true;
  animValues->linkQuery = &partQuery;

  animValues->modeTyp = this->dbanimation->eigenmodeType.getValue();
  animValues->selLink = this->dbanimation->eigenmodePart;
  animValues->selTime = this->dbanimation->eigenmodeTime.getValue();
  animValues->selMode = this->dbanimation->eigenmodeNr.getValue();
  animValues->modeScale = this->dbanimation->eigenmodeAmplitude.getValue();
  animValues->framesPrCycle = this->dbanimation->eigenmodeFramesPrCycle.getValue();
  animValues->timeLength = this->dbanimation->eigenmodeDurationUseTime.getValue();
  animValues->timeLengthVal = this->dbanimation->eigenmodeDurationTime.getValue();
  animValues->nCycles = this->dbanimation->eigenmodeDurationUseNCycles.getValue();
  animValues->nCyclesVal = this->dbanimation->eigenmodeDurationNCycles.getValue();
  animValues->untilDamped = this->dbanimation->eigenmodeDurationUseUntilDamped.getValue();
  animValues->untilDampedVal = (int)this->dbanimation->eigenmodeDurationUntilDamped.getValue();

  // Option Menus fill-in

  std::string oldClass(animValues->selResultClass);
  std::string oldName(animValues->selResult);
  std::string oldOper(animValues->selResultOper);
  std::string oldSet(animValues->selResultSet);

  // static content

  // FRINGE
  if (animValues->isSummaryAnim) {
    animValues->resultClasses = { "Element" };
    animValues->selResultClass = animValues->resultClasses.front();
  }
  else if (animValues->isModesAnim) {
    animValues->resultClasses.clear();
    animValues->selResultClass.clear();
  }
  else {
    animValues->resultClasses = { "Element node", "Node" };
    if (animValues->selResultClass != animValues->resultClasses.back())
      animValues->selResultClass = animValues->resultClasses.front();
  }

  // 1->N opers
  animValues->resultSetOpers = FFaOpUtils::findOpers("VECTOR");

  // Averaging
  animValues->averOns.clear();
  animValues->averOpers.clear();
  if (!animValues->isModesAnim) {
    animValues->averOns = { "Node", "Element" };
    animValues->averOpers.reserve(1+animValues->resultSetOpers.size());
    animValues->averOpers = { "None" };
    animValues->averOpers.insert(animValues->averOpers.end(),
				 animValues->resultSetOpers.begin(),animValues->resultSetOpers.end());
  }

  // dynamic content

  // Element groups
  std::set<std::string> groupNames;
  std::vector<FmModelMemberBase*> grps;
  FmDB::getAllOfType(grps,FmElementGroupProxy::getClassTypeID());
  for (FmModelMemberBase* group : grps)
    groupNames.insert(group->getInfoString());
  animValues->elemGrps.assign(groupNames.begin(),groupNames.end());
  animValues->elemGrpOpers = animValues->resultSetOpers;

  // Results, Result Sets & Result Operations
  animValues->results.clear();
  animValues->resultSets.clear();
  animValues->resultOpers.clear();

  // Make Result Variable choices
  const ResultMap& resultMap = this->findResults(animValues->selResultClass);
  ResultMap::const_iterator rit = resultMap.end();
  for (const ResultMap::value_type& result : resultMap)
    animValues->results.insert(result.first);

  if (animValues->results.empty())
  {
    animValues->selResult.clear();
    animValues->selResultOper.clear();
    animValues->selResultSet.clear();
  }
  else
  {
    // Select a valid Result Variable description
    if (animValues->results.find(animValues->selResult) == animValues->results.end()) {
      // Current result selection no longer valid
      if (animValues->selResultClass == "Element" &&
          animValues->results.find("Max von Mises stress") != animValues->results.end())
        animValues->selResult = "Max von Mises stress";
      else if (animValues->selResultClass == "Element node" &&
               animValues->results.find("Stress") != animValues->results.end())
        animValues->selResult = "Stress";
      else
        animValues->selResult = *animValues->results.begin();
    }

    // Make Result Operations
    if ((rit = resultMap.find(animValues->selResult)) != resultMap.end())
    {
      animValues->resultOpers = FFaOpUtils::findOpers(rit->second.first);

      // Select a valid Result Operation
      if (std::find(animValues->resultOpers.begin(),
                    animValues->resultOpers.end(),
                    animValues->selResultOper) == animValues->resultOpers.end()) {
        if (!animValues->resultOpers.empty() && !animValues->selResult.empty())
          animValues->selResultOper = FFaOpUtils::getDefaultOper(rit->second.first);
        else
          animValues->selResultOper.clear();
      }

      // Make Result Set choices
      animValues->resultSets = rit->second.second;
      if (animValues->resultSets.size() > 1 && animValues->resultSets.find("Basic") != animValues->resultSets.end()) {
        // Add combined choices "Bottom + Basic", "Top + Basic", ...
        ResultSet combset;
        for (const std::string& rset : animValues->resultSets)
          if (rset != "Basic") combset.insert(rset + " + Basic");
        animValues->resultSets.insert(combset.begin(),combset.end());
      }

      // Select a valid Result Set
      if (animValues->resultSets.find(animValues->selResultSet) == animValues->resultSets.end()) {
        if (!animValues->resultSets.empty())
          animValues->selResultSet = *animValues->resultSets.begin();
        else
          animValues->selResultSet.clear();
      }
    }
  }

  // MODES
  int maxModes = 0;
  animValues->times.clear();
  animValues->modes.clear();

  if (animValues->modeTyp == FmAnimation::SYSTEM_MODES)
  {
    FmAnalysis* analysis = FmDB::getActiveAnalysis();
    if (analysis->solveEigenvalues.getValue())
    {
      // Eigenvalue times
      analysis->getEigenvalueSamples(animValues->times);
      // Eigenmodes
      maxModes = analysis->numEigenmodes.getValue();
    }
  }
  else if (animValues->selLink)
  {
    if (dbanimation->eigenmodePart->expandModeShapes.getValue()) {
      if (animValues->modeTyp == FmAnimation::FREE_FREE_REDUCED)
        maxModes = 12; // Default value of the -nevred reducer option
      else
        maxModes = dbanimation->eigenmodePart->nGenModes.getValue();
    }
  }
  else if (animValues->modeTyp == FmAnimation::FREE_FREE_REDUCED)
    maxModes = 12; // Default value of the -nevred reducer option
  else
  {
    std::vector<FmPart*> parts;
    FmDB::getAllParts(parts);
    for (FmPart* part : parts)
      if (part->expandModeShapes.getValue())
	if (part->nGenModes.getValue() > maxModes)
	  maxModes = part->nGenModes.getValue();
  }

  animValues->modes.reserve(maxModes);
  for (int i = 1; i <= maxModes; i++)
    animValues->modes.push_back(i);

  bool haveFringeResults = false;
  if (animValues->isSummaryAnim)
    haveFringeResults = !resultMap.empty() && FpPM::hasResults(FpPM::SUMMARY_RCY);
  else if (!animValues->isModesAnim)
    haveFringeResults = !resultMap.empty() && FpPM::hasResults(FpPM::STRESS_RCY);

  // Check for DB changes, but only if we have results
  bool checkDBchange = false;
  if (haveFringeResults && (animValues->loadFringe || animValues->loadLineFringe))
    checkDBchange = (dbanimation->fringeResultClass.setValue(animValues->selResultClass) |
                     dbanimation->fringeVariableName.setValue(animValues->selResult) |
                     dbanimation->fringeToScalarOpName.setValue(animValues->selResultOper) |
                     dbanimation->resSetName.setValue(animValues->selResultSet));
  if (checkDBchange)
  {
    // Some DB fields must be modified to be in sync with the disk content.
    // But ask user for approval, since we are not in a DB update state.
    std::string msg("The settings in " + dbanimation->getIdString(true) +
                    "\ndoes not match with the results available on disk."
                    "\nThe following will be changed:");
    if (animValues->selResultClass != oldClass)
      msg += "\n\tResult Class: " + oldClass + " --> " + animValues->selResultClass;
    if (animValues->selResult != oldName)
      msg += "\n\tResult: " + oldName + " --> " + animValues->selResult;
    if (animValues->selResultOper != oldOper)
      msg += "\n\tOperation: " + oldOper + " --> " + animValues->selResultOper;
    if (animValues->selResultSet != oldSet)
      msg += "\n\tResult Set: " + oldSet + " --> " + animValues->selResultSet;
    if (weAreUpdatingDB) // User toggled some changes, OK to modify the DB fields
      FFaMsg::dialog(msg,FFaMsg::DISMISS_INFO);
    else if (FFaMsg::dialog(msg,FFaMsg::OK_CANCEL))
      FpPM::touchModel(); // Indicate that the model has changed
    else
    {
      // User cancelled, discard the DB changes
      dbanimation->fringeResultClass.setValue(oldClass);
      dbanimation->fringeVariableName.setValue(oldName);
      dbanimation->fringeToScalarOpName.setValue(oldOper);
      dbanimation->resSetName.setValue(oldSet);
    }
  }
  weAreUpdatingDB = false;
}
//----------------------------------------------------------------------------

void FapUAAnimationDefine::onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
						  const std::vector<FFaViewItem*>&,
						  const std::vector<FFaViewItem*>&)
{
  // Show last selected one if animation
  if (!totalSelection.empty())
    this->dbanimation = dynamic_cast<FmAnimation*>(totalSelection.back());
  else
    this->dbanimation = NULL;

  if (this->dbanimation)
    this->updateUIValues();
}
//----------------------------------------------------------------------------

const FapUAAnimationDefine::ResultMap& FapUAAnimationDefine::findResults(const std::string& resultClass)
{
  if (!resultClass.empty() && currentResults.find(resultClass) == currentResults.end())
    if (!FapUAAnimationDefine::findResults(currentResults[resultClass],resultClass))
      FFaMsg::dialog("No " + resultClass + " results found.",FFaMsg::DISMISS_WARNING);

#ifdef FAP_DEBUG
  std::cout <<"FapUAAnimationDefine::findResults("<< resultClass
            <<"): "<< currentResults[resultClass].size() << std::endl;
#endif
  return currentResults[resultClass];
}
//----------------------------------------------------------------------------

bool FapUAAnimationDefine::findResults(ResultMap& results,
				       const std::string& resultClass)
{
  // Use the model extractor if results of the relevant kind exist.
  // Otherwise, use the possibility extractor.
  FFrExtractor* extr = FpRDBExtractorManager::instance()->getPossibilityExtractor();
  if (resultClass == "Element") {
    if (FpPM::hasResults(FpPM::SUMMARY_RCY))
      extr = FpRDBExtractorManager::instance()->getModelExtractor();
  }
  else {
    if (FpPM::hasResults(FpPM::STRESS_RCY))
      extr = FpRDBExtractorManager::instance()->getModelExtractor();
  }
  if (!extr)
  {
    std::cerr <<" *** FapUAAnimationDefine::findResults: No results extractor."<< std::endl;
    return false;
  }
#ifdef FAP_DEBUG
  else
    std::cout <<"FapUAAnimationDefine::findResults: Using "<< extr->getName() << std::endl;
#endif

  // Set up the search path with wild-cards "*" to get all relevant entries
  FFaResultDescription IGDescr("Part");
  if (resultClass == "Node")
    IGDescr.varDescrPath = { "Nodes", "*", "*" };
  else if (resultClass == "Element")
    IGDescr.varDescrPath = { "Elements", "*", "*", "Element", "*" };
  else if (resultClass == "Element node")
    IGDescr.varDescrPath = { "Elements", "*", "*", "Element nodes", "*", "1" };
  else
  {
    std::cerr <<" *** FapUAAnimationDefine::findResults: Unknown result class "
              << resultClass << std::endl;
    return false;
  }

  Fui::noUserInputPlease();
  FFaMsg::pushStatus("Rebuilding contour value menus");
#ifdef FAP_DEBUG
  std::cout <<"FapUAAnimationDefine::findResults: Searching for "<< IGDescr.getText() << std::endl;
#endif
  // Search the results database for matching entries
  std::vector<FFrEntryBase*> entries;
  extr->search(entries,IGDescr);

  // Build result map
  for (FFrEntryBase* entry : entries)
    if (entry->hasDataFields()) {
      FFrEntryBase* owner = resultClass == "Element node" ? entry->getOwner() : entry;
      for (FFrEntryBase* var : *(entry->getDataFields())) {
	ResultItem& result = results[var->getDescription()];
	if (result.first.empty())
	  result.first = var->getType();
	else if (result.first < var->getType())
	  result.first = var->getType();
	result.second.insert(owner->getDescription());
      }
    }

#ifdef FAP_DEBUG
  std::cout <<"FapUAAnimationDefine::findResults: Found "<< entries.size()
            <<" --> "<< results.size() <<" matches."<< std::endl;
  for (const ResultMap::value_type& r : results)
    std::cout << r.first <<" "<< r.second.first <<" "<< r.second.second.size() << std::endl;
#endif
  FFaMsg::popStatus();
  Fui::okToGetUserInput();
  return !results.empty();
}
//------------------------------------------------------------------------------

FapUAAnimationDefine::SignalConnector::SignalConnector(FapUAAnimationDefine* ua) : owner(ua)
{
  FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
                          FpRDBExtractorManager::MODELEXTRACTOR_HEADER_CHANGED,
                          FFaSlot1M(SignalConnector,this,
				    onModelExtrHeaderChanged,FFrExtractor*));
}
//------------------------------------------------------------------------------

FapUAAnimationDefine::SignalConnector::~SignalConnector()
{
  FFaSwitchBoard::removeAllOwnerConnections(this);
}
//------------------------------------------------------------------------------
