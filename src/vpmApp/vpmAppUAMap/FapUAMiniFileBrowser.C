// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAMiniFileBrowser.H"
#include "vpmUI/vpmUITopLevels/FuiMiniFileBrowser.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "vpmUI/Fui.H"
#include "vpmPM/FpPM.H"
#include "vpmPM/FpFileSys.H"
#include "vpmPM/FpRDBExtractorManager.H"
#include "vpmPM/FpModelRDBHandler.H"
#include "vpmDB/FmResultStatusData.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmTriad.H"
#include "FFrLib/FFrExtractor.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFuLib/FFuAuxClasses/FFuaIdentifiers.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"

#include "vpmApp/vpmAppCmds/FapEditCmds.H"
#include "vpmApp/vpmAppProcess/FapSimEventHandler.H"
#include "vpmApp/vpmAppProcess/FapSolutionProcessMgr.H"
#include "vpmApp/vpmAppProcess/FapSolverID.H"
#include "vpmApp/vpmAppProcess/FapLinkReducer.H"

#include <algorithm>
#include <functional>
#include <fstream>


/*!
  \class FapUAMiniFileBrowser FapUAMiniFileBrowser.H

  \brief An autonomous result file browser, with delete, enable and disable.

  Will update itself whenever RDB changes, and a solver exits.
  Updating is suspended while it is popped down to memory
  (user clicked Close or the X in the top right corner).
  It will update itself when it pops up again, if necessary.

  \author Sven-Kaare Evenseth \date March 2003
  \sa FuiMiniFileBrowser
*/


Fmd_SOURCE_INIT(FcFAPUAMINIFILEBROWSER, FapUAMiniFileBrowser, FapUAExistenceHandler)


FapUAMiniFileBrowser::FapUAMiniFileBrowser(FuiMiniFileBrowser* uic)
  : FapUAExistenceHandler(uic), FapUAFinishHandler(uic), signalConnector(this)
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser constructor"<< std::endl;
#endif
  Fmd_CONSTRUCTOR_INIT(FapUAMiniFileBrowser);

  this->ui = uic;

  this->ui->setTempSelectionChangedCB(FFaDynCB1M(FapUAMiniFileBrowser,this,
						 tmpSelectionChanged,int));
  this->ui->setPermSelectionChangedCB(FFaDynCB0M(FapUAMiniFileBrowser,this,
						 permSelectionChanged));
  this->ui->setAppearanceOnScreenCB(FFaDynCB1M(FapUAMiniFileBrowser,this,
					       uiPoppedUp,bool));
  this->ui->setItemExpandedCB(FFaDynCB2M(FapUAMiniFileBrowser,this,
					 onItemExpanded,int,bool));

  this->ui->setKillCB(FFaDynCB0M(FapUAMiniFileBrowser,this,kill));
  this->ui->setRebuildCB(FFaDynCB0M(FapUAMiniFileBrowser,this,rebuildAll));

  FapSolutionProcessManager::instance()->setProcessDeathCB(FFaDynCB3M(FapUAMiniFileBrowser,this,
								      onSolverFinished,int,int,const std::string&));

  FileSpec spec;
  spec.uiLabel = "Reduction";
  spec.icon = reduceLinks_xpm;
  reducerHeader = this->insertUIItem(-1, -1, spec);

  spec.uiLabel = "Dynamics";
  spec.icon = solve_xpm;
  dynamicsHeader = this->insertUIItem(-1, reducerHeader, spec);

  spec.uiLabel = "Recovery";
  spec.icon = solveStress_xpm;
  recoveryHeader = this->insertUIItem(-1, dynamicsHeader, spec);

  this->ui->openListViewItem(dynamicsHeader, true, true);
  this->ui->openListViewItem(recoveryHeader, true, true);

  this->reductionNeedsRebuild = true;
  this->needsRebuild = true;
  this->needsRefresh = false;
  this->isUIPoppedUp = false;
  this->inInteractiveErase = false;
  this->myMonitoredFileStream = NULL;

  FFuaCmdItem* deleteCmd = new FFuaCmdItem();
  deleteCmd->setSmallIcon(erase_xpm);
  deleteCmd->setAccelKey(FFuaKeyCode::Delete);
  deleteCmd->setText("Delete");
  deleteCmd->setToolTip("Delete");
  deleteCmd->setActivatedCB(FFaDynCB0M(FapUAMiniFileBrowser,this,deleteResultFiles));
  deleteCmd->setGetSensitivityCB(FFaDynCB1M(FapUAMiniFileBrowser,this,
					    getDeleteResultFilesSensitivity,bool&));

  FFuaCmdItem* enableCmd = new FFuaCmdItem();
  enableCmd->setSmallIcon(results_enabled_xpm);
  enableCmd->setText("Enable Result");
  enableCmd->setToolTip("Enable Result");
  enableCmd->setActivatedCB(FFaDynCB0M(FapUAMiniFileBrowser,this,enableResultFiles));
  enableCmd->setGetSensitivityCB(FFaDynCB1M(FapUAMiniFileBrowser,this,
					    getEnableDisableSensitivity,bool&));

  FFuaCmdItem* disableCmd = new FFuaCmdItem();
  disableCmd->setSmallIcon(results_disabled_xpm);
  disableCmd->setText("Disable Result");
  disableCmd->setToolTip("Disable Result");
  disableCmd->setActivatedCB(FFaDynCB0M(FapUAMiniFileBrowser,this,disableResultFiles));
  disableCmd->setGetSensitivityCB(FFaDynCB1M(FapUAMiniFileBrowser,this,
					     getEnableDisableSensitivity,bool&));

//   FFuaCmdItem* sortByPartCmd = new FFuaCmdItem();
//   sortByPartCmd->setText("Sort By Part");
//   sortByPartCmd->setToolTip("Sort Recovery Section By Part");
//   sortByPartCmd->setActivatedCB(FFaDynCB0M(FapUAMiniFileBrowser,this,sortByPart));
//   sortByPartCmd->setGetSensitivityCB(FFaDynCB1M(FapUAMiniFileBrowser,this,
//						   FapCmdBase::alwaysSensitive,bool&));

//   FFuaCmdItem* sortByProcessCmd = new FFuaCmdItem();
//   sortByProcessCmd->setText("Sort By Process");
//   sortByProcessCmd->setToolTip("Sort Recovery Section By Process");
//   sortByProcessCmd->setActivatedCB(FFaDynCB0M(FapUAMiniFileBrowser,this,sortByProcess));
//   sortByProcessCmd->setGetSensitivityCB(FFaDynCB1M(FapUAMiniFileBrowser,this,
//						      FapCmdBase::alwaysSensitive,bool&));

  popUpCmds.push_back(enableCmd);
  popUpCmds.push_back(disableCmd);
  popUpCmds.push_back(new FFuaCmdSeparatorItem());
  popUpCmds.push_back(deleteCmd);

  this->ui->setPopUpCmds(popUpCmds);
}


FapUAMiniFileBrowser::~FapUAMiniFileBrowser()
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser destructor"<< std::endl;
#endif
  FapSolutionProcessManager::instance()->clearProcessDeathCB();
  this->cleanFileMonitoring();
}


/*!
  CB from solution process manager when a solver has died/exited.
*/

void FapUAMiniFileBrowser::onSolverFinished(int groupID, int eventID,
#ifdef FAP_DEBUG
					    const std::string& procSign)
#else
					    const std::string&)
#endif
{
  // Using a very brute force method here. For most cases these
  // rebuilds aren't necessary, because the changes they
  // represent are already covered by the extractor signals.
  // The reason for doing this at all is:
  // - Reducer. Used to call onChanged when reducer finished,
  //   just to get things in here. Now that is not necessary.
  // - Running recovery on disabled parts.
  // - Catch changes when solvers have exited without creating frs files.
  //
  // TODO: Be far more selective regarding what is updated and how.

#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::onSolverFinished()"
	    <<"\n\tsignature: "<< procSign
	    <<"\n\tgroup ID: "<< groupID <<"\tevent ID: "<< eventID << std::endl;
#endif

  // Rebuild only if the process was for the active event
  if (groupID != FapSolverID::FAP_REDUCER &&
      eventID != FapSimEventHandler::getActiveEventID()) return;

  switch (groupID) {
  case FapSolverID::FAP_REDUCER:

    if (isUIPoppedUp && this->ui->isItemExpanded(reducerHeader)) {
      this->buildReduction();
      this->updateExpandedItems();
      this->updateSelection();
    }
    else
      reductionNeedsRebuild = true;
    break;

  case FapSolverID::FAP_DYN_SOLVER:

    if (isUIPoppedUp) {
      this->buildDynamics();
      this->updateExpandedItems();
      this->updateSelection();
      needsRebuild = false;
      needsRefresh = false;
    }
    else
      needsRebuild = true;
    break;

  case FapSolverID::FAP_STRESS:
  case FapSolverID::FAP_MODES:
  case FapSolverID::FAP_GAGE:
  case FapSolverID::FAP_FPP:
  case FapSolverID::FAP_FEFATIGUE:
  case FapSolverID::FAP_DC_FEFCOM:  // These are the only two duty cycle processes
  case FapSolverID::FAP_DC_FEF2FRS: // that actually produce results for this model

    if (isUIPoppedUp) {
      this->buildRecovery();
      this->updateExpandedItems();
      this->updateSelection();
      needsRebuild = false;
      needsRefresh = false;
    }
    else
      needsRebuild = true;
    break;
  }

  if (!myMonitoredFileStream) return;

  // If the monitored res-file was written by the process that finished...
  FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();
  if (extr && !extr->getResultContainer(myMonitoredFileName))
    this->cleanFileMonitoring();
}


/*!
  Called whenever the UI is popped up or down from memory
  (i.e., not from an iconised position).
*/

void FapUAMiniFileBrowser::uiPoppedUp(bool poppedUp)
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::uiPoppedUp() "
	    << std::boolalpha << poppedUp << std::endl;
#endif

  this->isUIPoppedUp = poppedUp;
  if (!poppedUp) return;

  // Setting title
  std::string oldModel = this->modelName;
  this->modelName = FmDB::getMechanismObject()->getModelFileName();
  if (oldModel != this->modelName) needsRebuild = true;

  std::string title = "Result File Browser - " + FFaFilePath::getFileName(this->modelName);
  this->ui->setTitle(title.c_str());

  // Rebuilding reduction section each time, if it is expanded.
  // To catch manual deletion of matrix files, etc.
  if (this->ui->isItemExpanded(reducerHeader))
    this->buildReduction();
  else if (FmDB::getObjectCount(FmPart::getClassTypeID()) > 0)
    // Insert a dummy node such that we get the "+" in the listview.
    // The actual rebuilding is delayed until the "+" is pressed.
    if (this->insertUIItem(reducerHeader,-1,FileSpec()) >= 0)
      reductionNeedsRebuild = true;

  if (needsRebuild) {
    this->buildDynamics();
    this->buildRecovery();
  }
  else if (needsRefresh)
    this->refreshAll();

  this->updateExpandedItems();

  needsRefresh = needsRebuild = false;
}


/*!
  Slots from extractor manager.
*/

void FapUAMiniFileBrowser::onModelExtractorDeleted(FFrExtractor*)
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::onModelExtractorDeleted()"<< std::endl;
#endif

  this->cleanFileMonitoring();

  if (isUIPoppedUp) {
    if (!inInteractiveErase)
      this->rebuildAll();
    needsRebuild = false;
  }
  else
    needsRebuild = true;
}


void FapUAMiniFileBrowser::onModelExtractorHeaderChanged(FFrExtractor*)
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::onModelExtractorHeaderChanged()"<< std::endl;
#endif

  if (isUIPoppedUp) {
    if (!inInteractiveErase)
      this->rebuildAll();
    needsRebuild = false;
  }
  else
    needsRebuild = true;
}


void FapUAMiniFileBrowser::onModelExtractorDataChanged(FFrExtractor*)
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::onModelExtractorDataChanged()"<< std::endl;
#endif

  if (isUIPoppedUp)
  {
    std::vector<FapProcID> runningProcs;
    FapSolutionProcessManager::instance()->getRunningGroups(FapSimEventHandler::getActiveEvent(),runningProcs);

    std::set<FmLink*> reducingParts;
    std::set<FmLink*> recoveringParts;
    bool isDynamicsRunning = false;

    for (const FapProcID& proc : runningProcs)
      switch (proc.first)
      {
        case FapSolverID::FAP_REDUCER:
          reducingParts.insert(proc.second);
          break;

        case FapSolverID::FAP_DYN_SOLVER:
          isDynamicsRunning = true;
          break;

        case FapSolverID::FAP_STRESS:
        case FapSolverID::FAP_MODES:
        case FapSolverID::FAP_GAGE:
        case FapSolverID::FAP_FPP:
        case FapSolverID::FAP_FEFATIGUE:
        case FapSolverID::FAP_DC_FEFCOM:  // These are the only two duty cycle processes
        case FapSolverID::FAP_DC_FEF2FRS: // that actually produce results for this model
          recoveringParts.insert(proc.second);
          break;
      }

    for (FmLink* link : reducingParts)
      this->refresh(reducerHeader,dynamic_cast<FmPart*>(link));
    if (isDynamicsRunning)
      this->refresh(dynamicsHeader);
    for (FmLink* link : recoveringParts)
      this->refresh(recoveryHeader,dynamic_cast<FmPart*>(link));
    needsRefresh = false;
  }
  else
    needsRefresh = true;
}


void FapUAMiniFileBrowser::onModelExtractorNew(FFrExtractor*)
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::onModelExtractorNew()"<< std::endl;
#endif
}


/*!
  Slot from DB.
  \note This is also emitted <b>after<\b> a part has been connected
  and part data is read.
*/

void FapUAMiniFileBrowser::onModelMemberChanged(FmModelMemberBase* item)
{
  if (!item->isOfType(FmPart::getClassTypeID())) return;

  // This is being performed too often, because we can't know what has changed
  // on the part. Wouldn't it be cool with a "part checksum changed" signal?

#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::onModelMemberChanged()"<< std::endl;
#endif

  FpModelRDBHandler::clearPartIdMap();

  if (isUIPoppedUp) {
    this->updatePartEntry(static_cast<FmPart*>(item));
    this->updateExpandedItems();
  }
}


/*!
  Slot from DB.
*/

void FapUAMiniFileBrowser::onModelMemberDisconnected(FmModelMemberBase* item)
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::onModelMemberDisconnected()"<< std::endl;
#endif

  if (item->isOfType(FmPart::getClassTypeID()))
  {
    FpModelRDBHandler::clearPartIdMap();

    // find item id for this part
    for (int part : ui->getListViewChildren(reducerHeader))
      if (item == this->getMapPart(part))
      {
        // Remove part item and children
        this->removeAllChildrenOf(part);
        this->removeUIItem(part);
        return;
      }
  }

  else if (item->isOfType(FmTriad::getClassTypeID()) && isUIPoppedUp)
  {
    FmPart* ownerPart = static_cast<FmTriad*>(item)->getOwnerFEPart();
    if (ownerPart && !FapEditCmds::isBeingErased(ownerPart))
    {
      this->updatePartEntry(ownerPart);
      this->updateExpandedItems();
    }
  }
}


/*!
  CBs from UI.
*/

void FapUAMiniFileBrowser::tmpSelectionChanged(int)
{
  for (FFuaCmdItem* cmd : popUpCmds)
    ui->updateCmdSensitivity(cmd);
}


void FapUAMiniFileBrowser::permSelectionChanged()
{
  ui->clearTextInfo();
  this->cleanFileMonitoring();

  for (FFuaCmdItem* cmd : popUpCmds)
    ui->updateCmdSensitivity(cmd);

  std::vector<int> permSelected;
  ui->getListViewSelection(permSelected);
  if (permSelected.size() != 1) return;

  ItemMapCIterator it = fileMap.find(permSelected.front());
  if (it == fileMap.end()) return;

  myPathToSelectedItem = this->getItemPath(it->first);
  const FileSpec& spec = it->second;

  if (spec.itemType == FileSpec::FILE)
  {
    const std::string& file = spec.absPath;
    if (file.empty()) return;

    if (!FpFileSys::isFile(file)) return;

    std::string ext = FFaFilePath::getExtension(file);
    if (ext == "frs")
      this->setFrsFileText(file);
    else if (ext == "fco" ||
	     ext == "fop" ||
	     ext == "fao" ||
	     ext == "fsi" ||
	     ext == "fef" ||
	     ext == "ipt" ||
	     ext == "wnd")
      this->setTxtFileText(file);
    else if (ext == "res")
      this->setResFileText(file);
  }

  else if (spec.itemType == FileSpec::PART)
  {
    if (!this->isDescendantOf(reducerHeader,permSelected.front()))
      return;

    FmPart* part = this->getMapPart(permSelected.front());
    if (!part) return;

    // Display some general information about the selected part

    std::string str("--- File Info ---\n");
    if (!part->originalFEFile.getValue().empty())
      str += "Imported FE file: " + part->originalFEFile.getValue() + "\n";
    if (!part->baseFTLFile.getValue().empty())
      str += "Repository file:  " + part->getBaseFTLFile() + "\n";

    int nnod, nel, ndof;
    std::string elmTypeCount;
    if (part->suppressInSolver.getValue())
      str += "                  This part is ignored, used for visualization only.\n";
    else if (part->useGenericProperties.getValue())
      str += "                  This is a Generic part.\n";
    else if (part->externalSource.getValue())
      str += "                  This part has been reduced externally.\n";
    else if (part->getMeshParams(&nnod,&nel,NULL,&ndof,&elmTypeCount))
      str += FFaNumStr("\nNumber of DOFs: %10d\n",ndof) +
             FFaNumStr("Number of nodes: %9d\n",nnod) +
             FFaNumStr("Number of elements:%7d\n",nel) + elmTypeCount;

    std::map<int,int> nTriads;
    std::vector<FmTriad*> triads;
    part->getTriads(triads);
    for (FmTriad* triad : triads)
      ++nTriads[triad->getNDOFs()];

    if (!nTriads.empty()) str += "\n";
    for (const std::pair<int,int>& n : nTriads)
      str += FFaNumStr("Number of %d-DOF Triads:",n.first) +
             FFaNumStr("%5d\n",n.second);

    ui->setText(str);
  }
}


/*!
  Used to restore selection after a rebuild of the tree.
*/

void FapUAMiniFileBrowser::updateSelection()
{
  for (const std::pair<const int,FileSpec>& file : fileMap)
    if (this->getItemPath(file.first) == myPathToSelectedItem) {
      this->ui->selectItemId(file.first);
      return;
    }

  // The selection is no longer in the tree,
  // so clear the associated info view too.
  this->ui->clearTextInfo();
}


/*!
  \return file name associated with UI item \a id.
*/

std::string FapUAMiniFileBrowser::getMapFileName(int id) const
{
  ItemMapCIterator it = fileMap.find(id);
  return it == fileMap.end() ? "" : it->second.absPath;
}


/*!
  \return label (uiLabel) associated with item \a id.
*/

std::string FapUAMiniFileBrowser::getMapLabel(int id) const
{
  ItemMapCIterator it = fileMap.find(id);
  return it == fileMap.end() ? "" : it->second.uiLabel;
}


/*!
  \return part associated with UI item \a id. Might be null.
*/

FmPart* FapUAMiniFileBrowser::getMapPart(int id) const
{
  ItemMapCIterator it = fileMap.find(id);
  return it == fileMap.end() ? NULL : it->second.part;
}


/*!
  Get item id of ui item with rdb group name \a grp (e.g. timehist_rcy).
  \note Works only for the recovery entries for now.
*/

int FapUAMiniFileBrowser::getMapRdbGroup(const std::string& grp) const
{
  for (const std::pair<const int,FileSpec>& file : fileMap)
    if (file.second.itemType == FileSpec::RESULT_TYPE && file.second.rdbType == grp)
      return file.first;

  return -1;
}


/*!
  \return slash-separated path from top level to item with \a id.
*/

std::string FapUAMiniFileBrowser::getItemPath(int id) const
{
  if (id < 0) return "";

  std::string path;
  std::vector<int> parents = this->ui->getListViewAncestors(id);
  ItemMapCIterator it;
  while (!parents.empty())
  {
    it = fileMap.find(parents.back());
    if (it != fileMap.end())
      FFaFilePath::appendToPath(path,it->second.uiLabel);
    parents.pop_back();
  }
  it = fileMap.find(id);
  if (it != fileMap.end())
    FFaFilePath::appendToPath(path,it->second.uiLabel);

#if FAP_DEBUG > 4
  std::cout <<"FapUAMiniFileBrowser::getItemPath("<< id <<") --> "<< path << std::endl;
#endif
  return path;
}


/*!
  Removes item \a id from UI and from internal book keeping.
*/

void FapUAMiniFileBrowser::removeUIItem(int id)
{
  ui->deleteItem(id);
  fileMap.erase(id);
}


/*!
  Checks if \a ancestor is parent, grand-parent, great-grand-parent, etc. of
  \a descendant. Returns true if that is the case. If \a includeAncestor is
  true, it also checks if the descendant is the ancestor itself.
*/

bool FapUAMiniFileBrowser::isDescendantOf(int ancestor, int descendant,
					  bool includeAncestor) const
{
  if (includeAncestor)
    if (descendant == ancestor)
      return true;

  std::vector<int> children = ui->getAllListViewChildren(ancestor);
  return (std::find(children.begin(), children.end(), descendant) != children.end());
}


/*!
  Updates all file items, with possibly new size and modification date.
  Called when extractor data has changed.
*/

void FapUAMiniFileBrowser::refreshAll()
{
  this->refresh(-1,NULL);
}


/*!
  Updates all file items, with possibly new size and modification date.
  Called when extractor data has changed.
  use \a headerUiID = -1 and \a part = NULL for all.
*/

void FapUAMiniFileBrowser::refresh(int headerUiID, FmPart* part)
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::refresh()"<< std::endl;
#endif

  std::vector<int> itemsToBeDeleted;
  for (std::pair<const int,FileSpec>& file : fileMap)
    if (file.second.itemType == FileSpec::FILE)
      if (headerUiID == -1 || this->isDescendantOf(headerUiID,file.first))
        if (!part || file.second.part == part) {
          if (file.second.setup())
	    ui->updateItem(file.first,
		           file.second.uiLabel, file.second.sizeString,
		           file.second.modified, file.second.icon);
          else
	    itemsToBeDeleted.push_back(file.first);
        }

  // Remove those not present anymore
  for (int item : itemsToBeDeleted)
    this->removeUIItem(item);

  this->updateFileMonitoring();
}


/*!
  Inserts an item into list view in UI.
*/

int FapUAMiniFileBrowser::insertUIItem(int parent, int after, const FileSpec& spec)
{
  int item = ui->createItem(parent, after, spec.uiLabel, spec.sizeString, spec.modified, spec.icon);
  if (item < 0)
  {
    std::cerr <<"FapUAMiniFileBrowser::insertUIItem() - Could not create list view item"<< std::endl;
    return -99;
  }

  fileMap[item] = spec;
  return item;
}


void FapUAMiniFileBrowser::updateUIItem(int item, const FileSpec& spec)
{
  if (ui->updateItem(item, spec.uiLabel, spec.sizeString, spec.modified, spec.icon))
    fileMap[item] = spec;
  else
    std::cerr <<"FapUAMiniFileBrowser::updateUIItem() - Could not update list view item"<< std::endl;
}


/*!
  Removes all descendants of \a parent in list view.
*/

void FapUAMiniFileBrowser::removeAllChildrenOf(int parent)
{
  for (int item : ui->getAllListViewChildren(parent))
    this->removeUIItem(item);
}


/*!
  Utility method to set up the file specification for the given \a file.
  If \a file is blank, it is assumed that \a *this already contains a
  file name, and the specification for that file is updated instead.
  Gets info from the actual file in question.

  \return true if ok, false if file is not existing.
*/

bool FapUAMiniFileBrowser::FileSpec::setup(const std::string& file, const char* rdb)
{
  if (!file.empty()) absPath = file;
  if (absPath.empty()) return false;

  if (!FpFileSys::isFile(absPath)) {
#ifdef FAP_DEBUG
    std::cout <<"FileSpec::setup() "<< absPath <<" does not exist"<< std::endl;
#endif
    return false;
  }

  itemType = FileSpec::FILE;
  uiLabel  = FFaFilePath::getFileName(absPath);

  const double KB = 1024.0;
  const double MB = KB*KB;
  const double GB = MB*KB;

  unsigned int mySize = FpFileSys::getFileSize(absPath);
  double fSize = (double)mySize;

  // Convert the file size (in bytes) into a string with a more convenient unit.
  char buf[16];
  if (fSize < KB)
    sprintf(buf,"%d Bytes",mySize);
  else if (fSize < MB)
    sprintf(buf,"%.1f KB",fSize/KB);
  else if (fSize < GB)
    sprintf(buf,"%.1f MB",fSize/MB);
  else
    sprintf(buf,"%.1f GB",fSize/GB);

  sizeString = buf;
  modified   = FpFileSys::fileLastModified(absPath);
  rdbType    = rdb ? rdb : "";

  std::string ext = FFaFilePath::getExtension(absPath);
  if (ext == "frs")
  {
    FmMechanism* mech = FmDB::getMechanismObject();
    std::string tmp = FFaFilePath::getRelativeFilename(mech->getAbsModelRDBPath(),absPath);
    icon = mech->isEnabled(tmp) ? results_enabled_xpm : results_disabled_xpm;
  }
  else if (ext == "fmx" || ext == "fsm")
    icon = matrixfile_xpm;
  else if (ext == "fpp")
    icon = summaryfile_xpm;
  else if (ext == "fco" || ext == "fop" || ext == "fao")
    icon = textfile_xpm;
  else if (ext == "fsi" || ext == "res" || ext == "fef" || ext == "ipt" || ext == "wnd")
    icon = textfile_xpm;
  else
    icon = NULL;

  return true;
}


/*!
  Builds the list view tree for dynamics simulation.
*/

void FapUAMiniFileBrowser::buildDynamics()
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::buildDynamics()"<< std::endl;
#endif

  // Clear previous entries
  this->removeAllChildrenOf(dynamicsHeader);

  FmResultStatusData* topRSD = FapSimEventHandler::getActiveRSD();
  if (topRSD->isEmpty()) return;

  // Blocking ui
  Fui::noUserInputPlease();

  FmResultStatusData* eigRSD  = topRSD->getSubTask("eigval");
  FmResultStatusData* freqRSD = topRSD->getSubTask("freqdomain");
  FmResultStatusData* primRSD = topRSD->getSubTask("timehist_prim");
  FmResultStatusData* secRSD  = topRSD->getSubTask("timehist_sec");

  // Make top level entry
  int prevItem = -1;
  FileSpec  fileSpec;
  StringSet files;

  // Check for fco, fop, fao, fsi and res files
  topRSD->getAllFileNames(files, "fco", true, false);
  topRSD->getAllFileNames(files, "fop", true, false);
  topRSD->getAllFileNames(files, "fao", true, false);
  topRSD->getAllFileNames(files, "fsi", true, false);
  topRSD->getAllFileNames(files, "ipt", true, false);
  topRSD->getAllFileNames(files, "wnd", true, false);
  topRSD->getAllFileNames(files, "res", true, false);

  for (const std::string& file : files)
    if (fileSpec.setup(file))
      prevItem = this->insertUIItem(dynamicsHeader, prevItem, fileSpec);

  if (eigRSD && !eigRSD->isEmpty()) {
    files.clear();
    eigRSD->getAllFileNames(files, "frs");
    for (const std::string& file : files)
      if (fileSpec.setup(file,"eigval"))
        prevItem = this->insertUIItem(dynamicsHeader, prevItem, fileSpec);
  }

  if (freqRSD && !freqRSD->isEmpty()) {
    files.clear();
    freqRSD->getAllFileNames(files, "frs");
    for (const std::string& file : files)
      if (fileSpec.setup(file,"freqdomain"))
        prevItem = this->insertUIItem(dynamicsHeader, prevItem, fileSpec);
  }

  if (primRSD && !primRSD->isEmpty()) {
    files.clear();
    primRSD->getAllFileNames(files, "frs");
    for (const std::string& file : files)
      if (fileSpec.setup(file,"timehist_prim"))
        prevItem = this->insertUIItem(dynamicsHeader, prevItem, fileSpec);
  }

  if (secRSD && !secRSD->isEmpty()) {
    files.clear();
    secRSD->getAllFileNames(files, "frs");
    for (const std::string& file : files)
      if (fileSpec.setup(file,"timehist_sec"))
        prevItem = this->insertUIItem(dynamicsHeader, prevItem, fileSpec);
  }

  // Opening UI
  Fui::okToGetUserInput();
}


/*!
  Helper method that will find all elements in \a files
  containing the substring \a startsWith and return them in \a subSet.
  \a subSet is cleared.
*/

static bool getSubSet(const StringSet& files, StringSet& subSet,
		      const std::string& startsWith)
{
  subSet.clear();

  for (const std::string& file : files)
    if (file.find(startsWith) != std::string::npos)
      subSet.insert(file);

  return !subSet.empty();
}


/*!
  Builds the list view tree for recovery processes.
*/

void FapUAMiniFileBrowser::buildRecovery()
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::buildRecovery()"<< std::endl;
#endif

  // Clears previous
  this->removeAllChildrenOf(recoveryHeader);

  FmResultStatusData* topRSD = FapSimEventHandler::getActiveRSD();
  if (topRSD->isEmpty()) return;

  // Blocking UI
  Fui::noUserInputPlease();

  std::map<std::string,FmPart*> parts; // maps id string to actual part
  std::vector<FmPart*> partPtrs;
  FmDB::getFEParts(partPtrs);
  for (FmPart* part : partPtrs)
    if (!part->baseFTLFile.getValue().empty())
      parts[part->getTaskName()] = part;

  FmResultStatusData* recRSD[5];
  recRSD[0] = topRSD->getSubTask("timehist_rcy");
  recRSD[1] = topRSD->getSubTask("eigval_rcy");
  recRSD[2] = topRSD->getSubTask("timehist_gage_rcy");
  recRSD[3] = topRSD->getSubTask("summary_rcy");
  recRSD[4] = topRSD->getSubTask("dutycycle_rcy");

  StringSet resultFiles[5];
  for (int j = 0; j < 5; j++)
    if (recRSD[j] && !recRSD[j]->isEmpty()) {
      recRSD[j]->getAllFileNames(resultFiles[j], "fco");
      recRSD[j]->getAllFileNames(resultFiles[j], "fop");
      recRSD[j]->getAllFileNames(resultFiles[j], "fao");
      recRSD[j]->getAllFileNames(resultFiles[j], "fsi");
      recRSD[j]->getAllFileNames(resultFiles[j], "res");
      recRSD[j]->getAllFileNames(resultFiles[j], "frs");
      recRSD[j]->getAllFileNames(resultFiles[j], "fpp");
      recRSD[j]->getAllFileNames(resultFiles[j], "fef");
    }

  // Loop over parts and create items as we go

  int part = -1;
  int proc = -1;
  int item = -1;

  FileSpec  spec;
  StringSet subSet;
  for (const std::pair<std::string,FmPart*>& p : parts)
  {
    // Create part header
    part = this->insertUIItem(recoveryHeader, part,
			      FileSpec(FileSpec::PART,
				       p.second->getTaskName("[%d] "), "","","",
				       "", p.second));
    proc = -1;

    // Check for stress files
    if (getSubSet(resultFiles[0], subSet, p.first))
    {
      // Create header
      proc = this->insertUIItem(part, proc,
				FileSpec(FileSpec::RESULT_TYPE,
					 "Stress Recovery", "", "", "",
					 "timehist_rcy", NULL, solveStress_xpm));
      item = -1;
      for (const std::string& file : subSet)
        if (spec.setup(file))
          item = this->insertUIItem(proc, item, spec);
    }

    // Check for modes files
    if (getSubSet(resultFiles[1], subSet, p.first))
    {
      // Create header
      proc = this->insertUIItem(part, proc,
				FileSpec(FileSpec::RESULT_TYPE,
					 "Modes Recovery", "", "", "",
					 "eigval_rcy", NULL, solveModes_xpm));
      item = -1;
      for (const std::string& file : subSet)
        if (spec.setup(file))
          item = this->insertUIItem(proc, item, spec);
    }

    // check for gage rec files
    if (getSubSet(resultFiles[2], subSet, p.first))
    {
      // Create header
      proc = this->insertUIItem(part, proc,
				FileSpec(FileSpec::RESULT_TYPE,
					 "Strain Gage Recovery", "", "", "",
					 "timehist_gage_rcy", NULL, solveRosette_xpm));
      item = -1;
      for (const std::string& file : subSet)
        if (spec.setup(file))
          item = this->insertUIItem(proc, item, spec);
    }

    // Check for fpp files
    if (getSubSet(resultFiles[3], subSet, p.first))
    {
      // Create header
      proc = this->insertUIItem(part, proc,
				FileSpec(FileSpec::RESULT_TYPE,
					 "Strain Coat Recovery", "", "", "",
					 "summary_rcy", NULL, solveStrainCoat_xpm));
      item = -1;
      for (const std::string& file : subSet)
        if (spec.setup(file))
	  item = this->insertUIItem(proc, item, spec);
    }

    // Check for duty cycle files
    if (getSubSet(resultFiles[4], subSet, p.first))
    {
      // Create header
      proc = this->insertUIItem(part, proc,
				FileSpec(FileSpec::RESULT_TYPE,
					 "Duty Cycle", "", "", "",
					 "dutycycle_rcy", NULL, solveDutyCycle_xpm));
      item = -1;
      for (const std::string& file : subSet)
        if (spec.setup(file))
          item = this->insertUIItem(proc, item, spec);
    }
  }

  // Opening ui
  Fui::okToGetUserInput();
}


/*!
  Builds the list view tree for reduced parts.
  \note If a part is not reduced, its files will not be listed (currently),
  even if there are matrices in part RSD.
*/

void FapUAMiniFileBrowser::buildReduction()
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::buildReduction()"<< std::endl;
#endif

  Fui::noUserInputPlease();

  this->removeAllChildrenOf(reducerHeader);

  std::vector<FmPart*> parts;
  FmDB::getAllParts(parts);
  for (FmPart* part : parts)
    this->updatePartEntry(part);

  Fui::okToGetUserInput();
  reductionNeedsRebuild = false;
}


/*!
  CB from ui in debug mode. Rebuilds entire tree.
*/

void FapUAMiniFileBrowser::rebuildAll()
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::rebuildAll()"<< std::endl;
#endif

  if (this->ui->isItemExpanded(reducerHeader))
    this->buildReduction();
  else if (FmDB::getObjectCount(FmPart::getClassTypeID()) > 0)
    reductionNeedsRebuild = true;

  this->buildDynamics();
  this->buildRecovery();
  this->updateExpandedItems();
  this->updateSelection();
}


/*!
  CB from ui in debug mode. Will kill (as in delete) browser.
*/

void FapUAMiniFileBrowser::kill()
{
  delete this->ui;
}


/*!
  Updates or adds part entry under the reduction header.
  If part is not present, inserts according to part id.
*/

void FapUAMiniFileBrowser::updatePartEntry(FmPart* part)
{
  if (!part) return;

  const FmResultStatusData& rsd = part->myRSD.getValue();

  FileSpec spec;
  spec.part = part;
  spec.itemType = FileSpec::PART;

  if (part->useGenericProperties.getValue() || part->suppressInSolver.getValue())
  {
    spec.icon = reducerNotRun_xpm;
    spec.uiLabel = FFaNumStr("[%d] ",part->getID()) + part->getUserDescription();
  }
  else
  {
    spec.icon = FapLinkReducer::isReduced(part) ? reducerOK_xpm : reducerFail_xpm;
    spec.uiLabel = FFaNumStr("[%d] ",part->getID());
    if (rsd.isEmpty())
      spec.uiLabel += FFaFilePath::getBaseName(part->baseFTLFile.getValue());
    else
      spec.uiLabel += rsd.getCurrentTaskDirName();
  }

  // Loop over all present parts in ui
  // Looking for two things;
  // - is part item already present?
  // - if not, where should it be inserted?
  int prevID = -1, currentID = -1;
  for (int pid : ui->getListViewChildren(reducerHeader))
  {
    FmPart* currentPart = this->getMapPart(pid);
    if (!currentPart) {
#ifdef FAP_DEBUG
      std::cout <<"FapUAMiniFileBrowser::updatePartEntry() - Part pointer is NULL"<< std::endl;
#endif
      continue;
    }

    // Is this the part we are looking for?
    if (currentPart == part) {
      currentID = pid;
      break;
    }

    // Is this a part with higher ID (in a DB sense) than our part?
    if (currentPart->getID() > part->getID())
      break;

    prevID = pid;
  }

  // if currentID != -1, it is already present, and only needs updating
  // else it needs to be inserted after prevID

  if (currentID != -1) {
    this->updateUIItem(currentID,spec);
    this->removeAllChildrenOf(currentID);
  }
  else
    currentID = this->insertUIItem(reducerHeader, prevID, spec);

  // Fetching files
  StringSet files;
  if (part->externalSource.getValue())
  {
    // This part has been reduced externally, assume the
    // files are specified relative to the model file path
    const std::string& absPath = FmDB::getMechanismObject()->getAbsModelFilePath();
    auto&& addFMXfile = [&files,&absPath](std::string fmx)
    {
      if (!fmx.empty())
        files.insert(FFaFilePath::makeItAbsolute(fmx,absPath));
    };
    addFMXfile(part->SMatFile.getValue());
    addFMXfile(part->MMatFile.getValue());
    addFMXfile(part->GMatFile.getValue());
    addFMXfile(part->LMatFile.getValue());
    addFMXfile(part->DMatFile.getValue());
    addFMXfile(part->BMatFile.getValue());
    addFMXfile(part->EMatFile.getValue());
    addFMXfile(part->SAMdataFile.getValue());
  }
  else if (!rsd.isEmpty())
  {
    // Get files from the part RSD
    rsd.getAllFileNames(files, "fco");
    rsd.getAllFileNames(files, "fop");
    rsd.getAllFileNames(files, "fao");
    rsd.getAllFileNames(files, "res");
    rsd.getAllFileNames(files, "fmx");
    rsd.getAllFileNames(files, "fsm");
    rsd.getAllFileNames(files, "frs");
  }

  // Inserting files
  for (const std::string& file : files)
    if (spec.setup(file))
    {
      // Mark the "deletable" files
      std::string ext = FFaFilePath::getExtension(file);
      if (ext == "fmx" || ext == "fsm" || ext == "frs")
        spec.rdbType = "link_DB";
      this->insertUIItem(currentID, -1, spec);
    }
}


bool FapUAMiniFileBrowser::FileSpec::isFile(bool frsOnly) const
{
  if (itemType != FILE || absPath.empty())
    return false;
  else if (!frsOnly)
    return true;
  else
    return FFaFilePath::isExtension(absPath,"frs");
}


/*!
  The "work horse" for enabling and disabling results.
  Has same policy for selection as deleteResultFiles().
*/

void FapUAMiniFileBrowser::changeResultState(bool enable)
{
  Fui::noUserInputPlease();
  if (enable)
    FFaMsg::pushStatus("Reading result file info");
  else
    FFaMsg::pushStatus("Disabling result files");

  StringSet frsFiles;
  std::vector<int> selection;
  this->ui->getListViewSelection(selection);

  // Lambda function for extracting frs-files from a selected item
  std::function<void(int,bool)> getFiles = [this,&frsFiles](int item, bool skipExpanded)
  {
    ItemMapCIterator it = fileMap.find(item);
    if (it == fileMap.end()) return;

    if (it->second.isFile(true))
      frsFiles.insert(it->second.absPath);
    else if (it->second.itemType != FileSpec::FILE && !(skipExpanded && ui->isItemExpanded(item)))
      for (int child : ui->getAllListViewChildren(item))
        if ((it = fileMap.find(child)) != fileMap.end())
          if (it->second.isFile(true))
            frsFiles.insert(it->second.absPath);
  };

  // Find all frs-files among current selection
  if (selection.size() == 1)
    getFiles(selection.front(),false);
  else for (int sel : selection)
    getFiles(sel,true);

#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::changeResultState"<< (enable ? "(enable)\n" : "(disable)\n");
  for (const std::string& file : frsFiles) std::cout <<"\t"<< file << std::endl;
#endif

  if (!frsFiles.empty())
    FpModelRDBHandler::changeResultFilesState(FmDB::getMechanismObject(), frsFiles, enable);

  FFaMsg::popStatus();
  Fui::okToGetUserInput();
}


/*!
  Searches the selection for files. If \a frsOnly is true,
  will return true only if there are frs files in selection,
  else returns true for any file.
*/

bool FapUAMiniFileBrowser::filesInSelection(bool frsOnly) const
{
  std::vector<int> selection;
  this->ui->getListViewSelection(selection);

  // Lambda function checking for frs-files in selection
  std::function<bool(int,bool)> checkFiles = [this,frsOnly](int item, bool skipExpanded)
  {
    ItemMapCIterator it = fileMap.find(item);
    if (it == fileMap.end())
      return false;
    else if (it->second.isFile(frsOnly))
      return true;
    else if (it->second.itemType == FileSpec::FILE)
      return false;
    else if (skipExpanded && ui->isItemExpanded(item))
      return false;

    for (int child : ui->getAllListViewChildren(item))
      if ((it = fileMap.find(child)) != fileMap.end())
        if (it->second.isFile(frsOnly)) return true;

    return false;
  };

  if (selection.size() == 1)
    return checkFiles(selection.front(),false);
  else for (int sel : selection)
    if (checkFiles(sel,true))
      return true;

  return false;
}


/*!
  From pop up menu in list view.
  Will only remove frs-files from extractor, not res-files.
  May also remove matrix files from reducer.
*/

void FapUAMiniFileBrowser::deleteResultFiles()
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::deleteResultFiles()"<< std::endl;
#endif

  std::vector<int> selection;
  ui->getListViewSelection(selection);
  if (selection.empty()) return;

  std::map<FmPart*,StringSet> partFilesToDelete;
  std::map<FmPart*,StringSet> rdbGroupsToDelete;
  ItemMapCIterator it = fileMap.end();

  // Implicitly delete all groups/files below each item
  int fileCount = 0;
  int primaryFile = -1;
  bool deleteAllR = false;
  for (size_t i = 0; i < selection.size(); i++)
    if (selection[i] == dynamicsHeader)
      deleteAllR = true;

    else if (selection[i] == recoveryHeader && !deleteAllR)
      for (int child : ui->getAllListViewChildren(selection[i]))
      {
        // All recovery results should be deleted
        if ((it = fileMap.find(child)) != fileMap.end())
        {
          if (it->second.itemType == FileSpec::RESULT_TYPE)
            rdbGroupsToDelete[NULL].insert(it->second.rdbType);
          else if (it->second.itemType == FileSpec::FILE)
            fileCount++;
        }

        // To avoid deleting files twice
        std::replace(selection.begin(),selection.end(),child,-99);
      }

    else if (selection[i] == reducerHeader)
      for (int child : ui->getAllListViewChildren(selection[i]))
      {
        // Search for items FILE
        if ((it = fileMap.find(child)) != fileMap.end())
          if (it->second.itemType == FileSpec::FILE && it->second.rdbType == "link_DB")
          {
            ItemMapCIterator parentIt = fileMap.find(ui->getListViewParent(child));
            FmPart* part = parentIt == fileMap.end() ? NULL : parentIt->second.part;
            partFilesToDelete[part].insert(it->second.absPath);
          }
      }

    else if ((it = fileMap.find(selection[i])) == fileMap.end())
      continue;

    else if (it->second.itemType == FileSpec::PART && isDescendantOf(reducerHeader,selection[i]))
      for (int child : ui->getAllListViewChildren(selection[i]))
      {
        // Search for items FILE
        ItemMapCIterator childIt = fileMap.find(child);
        if (childIt != fileMap.end())
          if (childIt->second.itemType == FileSpec::FILE && childIt->second.rdbType == "link_DB")
            partFilesToDelete[it->second.part].insert(childIt->second.absPath);
      }

    else if (it->second.itemType == FileSpec::FILE)
    {
      if (it->second.rdbType == "link_DB")
      {
        ItemMapCIterator parentIt = fileMap.find(ui->getListViewParent(selection[i]));
        FmPart* part = parentIt == fileMap.end() ? NULL : parentIt->second.part;
        partFilesToDelete[part].insert(it->second.absPath);
      }
      else if (it->second.uiLabel == "th_p_1.frs")
        primaryFile = selection[i]; // primary time history result file
    }

  int redFileCount = 0;
  for (const std::pair<FmPart*,StringSet>& files : partFilesToDelete)
    redFileCount += files.second.size();

  if (redFileCount > 0)
  {
    // We have a collection of reducer files to be deleted
    // Needs special confirmation from the user first
    std::string warning = FFaNumStr("You are about to delete %d ",redFileCount) +
      "result file(s) from FE model reductions.\n"
      "This might trigger a re-reduction when starting the dynamics simulation.\n"
      "Do you wish to continue?";
#ifdef FAP_DEBUG
    for (const std::pair<FmPart*,StringSet>& files : partFilesToDelete)
      for (const std::string& file : files.second) std::cout <<"\t"<< file << std::endl;
#endif
    if (Fui::yesNoDialog(warning.c_str()))
    {
      for (const std::pair<FmPart*,StringSet>& files : partFilesToDelete)
        FpModelRDBHandler::removeResultFiles(files.second,files.first);
      this->buildReduction();
      this->updateExpandedItems();
    }
  }

  if (deleteAllR)
  {
    if (Fui::yesNoDialog("You are about to delete all dynamics results.\n"
			 "This implies that any recovery results will be deleted as well.\n"
			 "Do you wish to continue?"))
      FpModelRDBHandler::RDBIncrement(FapSimEventHandler::getActiveRSD(),
				      FmDB::getMechanismObject());
    return;
  }
  else if (primaryFile >= 0)
  {
    if (Fui::yesNoDialog("You have selected the primary time history result file for deletion.\n"
			 "If you delete this file, all other results, "
			 "including recovery results will be deleted as well.\n"
			 "Do you wish to continue?"))
    {
      FpModelRDBHandler::RDBIncrement(FapSimEventHandler::getActiveRSD(),
				      FmDB::getMechanismObject());
      return;
    }
    else
    {
      // Deselect the primary result file and continue
      ui->selectItemId(primaryFile,false);
      selection.erase(std::find(selection.begin(),selection.end(),primaryFile));
    }
  }

  // Find part-level result groups
  for (size_t i = 0; i < selection.size(); i++)
    if ((it = fileMap.find(selection[i])) == fileMap.end())
      continue;

    else if (it->second.itemType == FileSpec::RESULT_TYPE)
    {
      // Find part to filter on
      ItemMapCIterator parentIt = fileMap.find(ui->getListViewParent(selection[i]));
      FmPart* filterPart = parentIt == fileMap.end() ? NULL : parentIt->second.part;
      rdbGroupsToDelete[filterPart].insert(it->second.rdbType);

      for (int child : ui->getAllListViewChildren(selection[i]))
      {
        if ((it = fileMap.find(child)) != fileMap.end())
          if (it->second.itemType == FileSpec::FILE)
            fileCount++;

        // To avoid deleting files twice
        std::replace(selection.begin(),selection.end(),child,-99);
      }
    }

    else if (it->second.itemType == FileSpec::PART && !isDescendantOf(reducerHeader,selection[i]))
      for (int child : ui->getAllListViewChildren(selection[i]))
      {
        // This has to be re-written if we implement sorting
        ItemMapCIterator childIt = fileMap.find(child);
        if (childIt != fileMap.end())
        {
          if (childIt->second.itemType == FileSpec::RESULT_TYPE)
            rdbGroupsToDelete[it->second.part].insert(childIt->second.rdbType);
          else if (childIt->second.itemType == FileSpec::FILE)
            fileCount++;
        }

        // To avoid deleting files twice
        std::replace(selection.begin(),selection.end(),child,-99);
      }

  // Find files not already selected implicitly
  StringSet filesToDelete;
  for (int sel : selection)
    if ((it = fileMap.find(sel)) != fileMap.end() && !isDescendantOf(reducerHeader,sel))
      if (it->second.itemType == FileSpec::FILE)
        filesToDelete.insert(it->second.absPath);

  // We now have a collection of rdb groups and/or files to be deleted
  // Now get rid of them!

  fileCount += filesToDelete.size();
  if (!fileCount) return;

  std::string question("Are you sure you want to delete the ");
  if (fileCount == 1)
    question += "selected result file?";
  else
    question += FFaNumStr("%d selected result files?",fileCount);
  question += "\nThe file(s) will also be physically removed from disk.";
  if (!Fui::yesNoDialog(question.c_str())) return;

  inInteractiveErase = true;

  // Find the affected frs-files
  FmResultStatusData* rsd = FapSimEventHandler::getActiveRSD();
  for (const std::pair<FmPart*,StringSet>& files : rdbGroupsToDelete)
    for (const std::string& file : files.second)
      FpModelRDBHandler::removeResults(file,rsd,files.first);

  if (!filesToDelete.empty())
    FpModelRDBHandler::removeResultFiles(filesToDelete,rsd);

  inInteractiveErase = false;

  this->buildDynamics();
  this->buildRecovery();
  this->updateExpandedItems();
  this->updateSelection();
}


/*!
  From pop up menu in list view.
*/

void FapUAMiniFileBrowser::getDeleteResultFilesSensitivity(bool& sensitivity)
{
  sensitivity = FpPM::isModelTouchable() && this->filesInSelection(false);
}


void FapUAMiniFileBrowser::getEnableDisableSensitivity(bool& sensitivity)
{
  sensitivity = FpPM::isModelTouchable() && this->filesInSelection(true); // frs only
}


/*!
  Parses the header of an frs-file (stops at VARIABLES).
*/

void FapUAMiniFileBrowser::setFrsFileText(const std::string& file)
{
  std::ifstream stream(file.c_str());
  if (!stream) return;

  char buf[BUFSIZ];
  std::string tmp, ret;

  // Discarding first line
  stream.getline(buf,BUFSIZ);

  while (!stream.eof() && !stream.fail()) {
    stream.getline(buf,BUFSIZ);
    tmp = std::string(buf);
    if (tmp.find("VARIABLES:") < tmp.size())
      break;
    else
      ret += tmp + "\n";
  }

  this->ui->setText(ret);
}


/*!
  Reads a text file and sets the contents in the ui.
*/

void FapUAMiniFileBrowser::setTxtFileText(const std::string& file)
{
  // The setFileToShow method is slow, so use for small files only
  if (FpFileSys::getFileSize(file) < 5120)
    this->ui->setFileToShow(file);
  else
  {
    std::ifstream stream(file.c_str());
    if (stream) this->setText(stream,file);
  }
}


/*!
  Reads an input stream and sets the contents in the ui.
*/

void FapUAMiniFileBrowser::setText(std::istream& stream, const std::string& file)
{
#define FILE_BUFFER_SIZE 1048576

  Fui::noUserInputPlease();
  char* buf = new char[FILE_BUFFER_SIZE];

  // Bugfix #160: Only read the first 1MB (and the last 20KB)
  // of large text files to avoid memory exhaustion
  stream.read(buf,FILE_BUFFER_SIZE-1);
  std::streamsize nReadChar = stream.gcount();
  if (nReadChar > 0) {
    buf[nReadChar] = '\0';
    this->ui->appendText(buf);
    if (nReadChar+1 == FILE_BUFFER_SIZE) {
      // Rewind 20 KB from end of file
      std::streampos prevPos = stream.tellg();
      stream.seekg(-20480,std::ios::end);
      std::streampos newPos = stream.tellg();
      // If we, by doing this, have rewinded to before the position
      // we had, set file position back to what it was before rewinding
      if (newPos < prevPos)
        stream.seekg(prevPos);
      else {
        this->ui->appendText("\n ... Skipping file content ...\n"
                             " The entire content can be found in ");
        this->ui->appendText(file.c_str());
        this->ui->appendText("\n\n");
        stream.getline(buf,FILE_BUFFER_SIZE);
      }
      stream.read(buf,FILE_BUFFER_SIZE);
      nReadChar = stream.gcount();
      if (nReadChar > 0) {
        buf[nReadChar] = '\0';
        this->ui->appendText(buf);
      }
    }
  }

  delete[] buf;
  Fui::okToGetUserInput();
}


/*!
  Makes the contents of the file be set in the ui.
  When called several times on the same file, it appends the new text to the ui.
  If the ui was scrolled to the end, it scrolls to the end after appending.
*/

void FapUAMiniFileBrowser::setResFileText(const std::string& file)
{
  if (file != myMonitoredFileName) {
    this->cleanFileMonitoring();
    FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();
    if (extr && extr->getResultContainer(file)) {
      // This res-file is currently being written by a solver process
      std::ifstream* newStream = new std::ifstream(file.c_str());
      if (*newStream) {
	myMonitoredFileStream = newStream;
	myMonitoredFileName = file;
      }
    }
    else
      this->setTxtFileText(file);
  }

  this->updateFileMonitoring();
}


void FapUAMiniFileBrowser::updateFileMonitoring()
{
  if (!myMonitoredFileStream) return;
  if (this->ui->isDraggingVScroll()) return;

  bool isViewingEnd = this->ui->isViewingTextEnd();

  this->setText(*myMonitoredFileStream,myMonitoredFileName);

  myMonitoredFileStream->clear();
  if (isViewingEnd)
    this->ui->scrollTextToBottom();
}


/*!
  Cleans up the stored data for file monitoring.
*/

void FapUAMiniFileBrowser::cleanFileMonitoring()
{
  if (myMonitoredFileStream)
    delete myMonitoredFileStream;
  myMonitoredFileStream = NULL;
  myMonitoredFileName = "";
}


/*!
  From pop up menu in list view. It is supposed to set
  sorting flag, and rebuild the tree, sorted on part, e.g.:
  \verbatim
  Recovery
     |
     +-- Part
          |
          +- Stress
   \endverbatim

   \note Not implemented yet
*/

void FapUAMiniFileBrowser::sortByPart()
{
  this->sortMode = BY_PART;
  this->buildRecovery();
}


/*!
  From pop up menu in list view. It is supposed to set
  sorting flag, and rebuild the tree, sorted on part, e.g.:
  \verbatim
  Recovery
    |
    +-- Stress
         |
         +-- Part
  \endverbatim

  \note Not implemented yet
*/

void FapUAMiniFileBrowser::sortByProcess()
{
  this->sortMode = BY_PROCESS;
  this->buildRecovery();
}


/*!
  CB from list view in UI.
*/

void FapUAMiniFileBrowser::onItemExpanded(int item, bool expanded)
{
  if (item < 0) return;

  if (item == reducerHeader && expanded && reductionNeedsRebuild)
  {
    this->buildReduction();
    this->updateExpandedItems();
  }

  if (isDescendantOf(reducerHeader,item))
    expandedPartsMap[this->getMapPart(item)] = expanded;
  else
    expandedMap[this->getItemPath(item)] = expanded;
}


/*!
  Updates which items are expanded based on UI label
  (label being complete with path in tree).
*/

void FapUAMiniFileBrowser::updateExpandedItems()
{
  for (const std::pair<std::string,bool>& exp : expandedMap)
    if (exp.second)
      for (const std::pair<const int,FileSpec>& file : fileMap)
	if (this->getItemPath(file.first) == exp.first)
	{
	  ui->openListViewItem(file.first,true,false);
	  break;
	}

  std::map<FmPart*,bool>::const_iterator mapIt;
  for (int pid : ui->getListViewChildren(this->reducerHeader))
    if ((mapIt = expandedPartsMap.find(this->getMapPart(pid))) != expandedPartsMap.end())
      if (mapIt->second)
        ui->openListViewItem(pid,true,false);
}


/*!
  From finish handler.
*/

void FapUAMiniFileBrowser::finishUI()
{
  this->cleanFileMonitoring();
  Fui::resultFileBrowserUI(false,true);
}


/*!
  Signal connector constructor.
  Connects to signals in FpRDBExtractorManager and FmModelMemberBase.
*/

FapUAMiniFileBrowser::SignalConnector::SignalConnector(FapUAMiniFileBrowser* ua) : owner(ua)
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::SignalConnector constructor"<< std::endl;
#endif
  FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
			  FpRDBExtractorManager::MODELEXTRACTOR_ABOUT_TO_DELETE,
			  FFaSlot1M(SignalConnector,this,onModelExtractorDeleted,FFrExtractor*));
  FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
			  FpRDBExtractorManager::MODELEXTRACTOR_HEADER_CHANGED,
			  FFaSlot1M(SignalConnector,this,onModelExtractorHeaderChanged,FFrExtractor*));
  FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
			  FpRDBExtractorManager::MODELEXTRACTOR_DATA_CHANGED,
			  FFaSlot1M(SignalConnector,this,onModelExtractorDataChanged,FFrExtractor*));
  FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
			  FpRDBExtractorManager::NEW_MODELEXTRACTOR,
			  FFaSlot1M(SignalConnector,this,onModelExtractorNew,FFrExtractor*));

  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_DISCONNECTED,
			  FFaSlot1M(SignalConnector,this,onModelMemberDisconnected,FmModelMemberBase*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_CHANGED,
			  FFaSlot1M(SignalConnector,this,onModelMemberChanged,FmModelMemberBase*));
}


FapUAMiniFileBrowser::SignalConnector::~SignalConnector()
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAMiniFileBrowser::SignalConnector destructor"<< std::endl;
#endif
  FFaSwitchBoard::removeAllOwnerConnections(this);
}
