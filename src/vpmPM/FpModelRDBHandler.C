// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmPM/FpModelRDBHandler.H"
#include "vpmPM/FpRDBExtractorManager.H"
#include "vpmPM/FpFileSys.H"
#include "vpmPM/FpPM.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmResultStatusData.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmTriad.H"
#include "vpmUI/Fui.H"
#include "FFrLib/FFrExtractor.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include <algorithm>
#include <iterator>
#include <map>


namespace
{
  using StringSet = std::set<std::string>;
  using StringVec = std::vector<std::string>;

  StringSet ourReducerFRSs;
  StringSet ourAddedRESs;

  std::map<std::string,FmPart*> ourPartIdMap;


  /*!
    Returns the ID name of parts as it appears in the Results File browser.
    and used as key in the \a ourPartIdMap.
  */

  std::string getIdName(const FmPart* part)
  {
    return FFaNumStr("%d_",part->getID()) +
      FFaFilePath::getBaseName(part->baseFTLFile.getValue());
  }


  /*!
    \brief Finds the part that a result file is related to.

    \details The search is based on the supplied full filename,
    by analysing the filename and path, using an id-to-pointer map.
    The map is supposed to contain text keys (partIdName) on the format:
        <idNr>_<PartFileName>

    This algorithm first looks for the partIdName in the name of the directory
    that the file reside in. If not found, it tries the filename itself.
  */

  FmPart* getPartRelatedToResFile(const std::string& resultFileName)
  {
    if (ourPartIdMap.empty())
    {
      std::vector<FmPart*> parts;
      FmDB::getAllParts(parts);
      for (FmPart* part : parts)
        ourPartIdMap[getIdName(part)] = part;
    }

    std::string partPath = FFaFilePath::getPath(resultFileName,false);
    std::string baseName = FFaFilePath::getFileName(partPath);

    for (int i = 0; i < 2; i++)
    {
      if (!baseName.empty())
      {
        // Find the start of any solvetask id numbers
        size_t pos = baseName.size() - 1;
        while (pos > 0 && isdigit(baseName[pos])) pos--;

        // And then the underscore
        if (baseName[pos] == '_' && pos < baseName.size()-1) pos--;

        // Strip the solvetask number off, and search for partIdName in map
        std::string partIdName = baseName.substr(0,pos+1);
        std::map<std::string,FmPart*>::const_iterator it = ourPartIdMap.find(partIdName);
        if (it != ourPartIdMap.end()) return it->second;
      }

      // When we could not find it based on the directory it resided in
      // try the file name itself
      baseName = FFaFilePath::getBaseName(resultFileName,true);
    }

    return NULL;
  }


#ifdef FP_DEBUG
  template<class Set> void reportSet(const std::string& head, const Set& items)
  {
    if (items.empty()) return;
    std::cout <<"\n"<< head;
    for (const std::string& item : items) std::cout <<"\n"<< item;
    std::cout << std::endl;
  }
#endif


  /*!
    Removes \a files from the extractor, and deletes them from disk.
  */

  void remove_result_files(const std::set<std::string>& files)
  {
#if FP_DEBUG > 3
    reportSet("Removing these files from the extractor:",files);
#endif

    // Remove from extractor
    FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();
    if (extr && !extr->removeFiles(files))
      FFaMsg::list("  -> Problems removing files from result extractor.\n");

    // Remove from disk
    for (const std::string& file : files)
      if (FpFileSys::isFile(file) && !FpFileSys::deleteFile(file))
        FFaMsg::list("  -> Problems deleting file " + file + "\n");
  }


  void reportRDBFiles(const StringVec& files, const std::string& rdbPath)
  {
    std::string solver1("timehist_prim");
    std::string solver2("timehist_sec");
    std::string solverEig("eigval_");
    std::string solverFreq("freqdomain_");
    std::string stress("timehist_rcy");
    std::string modes("eigval_rcy");
    std::string gage("timehist_gage_rcy");
    std::string fpp("summary_rcy");
    std::string reducerFiles, solver1Files, solver2Files;
    std::string solverEigFiles, solverFreqFiles;
    std::string stressFiles, modesFiles, gageFiles, fppFiles;

    for (const std::string& file : files)
      if (file.size() > rdbPath.size())
      {
        // Remove rdbPath/ from current file name
        std::string current = FFaFilePath::getRelativeFilename(rdbPath,file);

        if (current.find(stress) != std::string::npos)
          stressFiles += "\t" + current + "\n";
        else if (current.find(modes) != std::string::npos)
          modesFiles += "\t" + current + "\n";
        else if (current.find(gage) != std::string::npos)
          gageFiles += "\t" + current + "\n";
        else if (current.find(fpp) != std::string::npos)
          fppFiles += "\t" + current + "\n";
        else if (current.find(solver1) != std::string::npos)
          solver1Files += "\t" + current + "\n";
        else if (current.find(solver2) != std::string::npos)
          solver2Files += "\t" + current + "\n";
        else if (current.find(solverEig) != std::string::npos)
          solverEigFiles += "\t" + current + "\n";
        else if (current.find(solverFreq) != std::string::npos)
          solverFreqFiles += "\t" + current + "\n";
        else // Nothing else fit, assume it is a reducer file
          reducerFiles += "\t" + current + "\n";
      }

    // Report the identified files to output list

    if (!reducerFiles.empty())
      ListUI <<"  -> Found Reducer results:\n"<< reducerFiles;
    if (!solver1Files.empty())
      ListUI <<"  -> Found Primary Time History results:\n"<< solver1Files;
    if (!solver2Files.empty())
      ListUI <<"  -> Found Secondary Time History results:\n"<< solver2Files;
    if (!solverEigFiles.empty())
      ListUI <<"  -> Found System Eigenmode results:\n"<< solverEigFiles;
    if (!solverFreqFiles.empty())
      ListUI <<"  -> Found Frequency Domain results:\n"<< solverFreqFiles;
    if (!stressFiles.empty())
      ListUI <<"  -> Found Stress Recovery results:\n"<< stressFiles;
    if (!modesFiles.empty())
      ListUI <<"  -> Found Eigenmode Recovery results:\n"<< modesFiles;
    if (!gageFiles.empty())
      ListUI <<"  -> Found Strain Rosette Recovery results:\n"<< gageFiles;
    if (!fppFiles.empty())
      ListUI <<"  -> Found Strain Coat Recovery results:\n"<< fppFiles;
  }


  /*!
    Helper used to sort the RDB directories w.r.t. their task version.
  */

  bool TaskDirLess(const std::string& lhs, const std::string& rhs)
  {
    size_t lpos = lhs.find_last_of('_');
    size_t rpos = rhs.find_last_of('_');

    int lver = -1;
    if (lpos != std::string::npos)
      if (isdigit(lhs[lpos+1]))
        lver = atoi(lhs.substr(lpos+1).c_str());

    int rver = -1;
    if (rpos != std::string::npos)
      if (isdigit(rhs[rpos+1]))
        rver = atoi(rhs.substr(rpos+1).c_str());

    return lver >= rver; // we want decreasing task version order
  }


  /*!
    \brief Helper used to filter out older versions of each frs-file.

    \details Assuming the frs-files have the date/time stamp as part of
    their names on the form response_xxxx/<path>/<name>_YYYY-MND-DD_hhmmss.frs.
    Thus, file names which differ only in the last 22 characters are assumed
    to be different versions of the same result file, and we keep the most
    recent of those files only.
  */

  void filterMostRecentOnly(StringVec& frsFiles)
  {
    if (frsFiles.empty()) return;

    StringVec::reverse_iterator rit = frsFiles.rbegin();
    StringVec filtered(1,*rit);
    filtered.reserve(frsFiles.size());
    for (++rit; rit != frsFiles.rend(); ++rit)
      if (rit->find("link_DB") != std::string::npos || rit->length() < 23)
        filtered.push_back(*rit);
      else if (rit->find_last_of("\\/") > rit->length()-23)
        filtered.push_back(*rit);
      else if (filtered.back().find(rit->substr(0,rit->length()-22)) > 0)
        filtered.push_back(*rit);
      else
        ListUI <<"  -> Not loading "<< *rit <<"\n";

    if (filtered.size() == frsFiles.size()) return;

    std::reverse(filtered.begin(),filtered.end());
    frsFiles = filtered;
  }
}


void FpModelRDBHandler::clearPartIdMap()
{
  ourPartIdMap.clear();
}


/*!
  Manages the open process with regards to the RDB and RSD handling.
*/

void FpModelRDBHandler::RDBOpen(FmResultStatusData* rsd,
				FmMechanism* mechData,
				bool includeReducerFiles,
				bool askMissingInRSD)
{
#if FP_DEBUG > 3
  std::cout <<"\nFpModelRDBHandler::RDBOpen()"
	    <<"\n\tpath\t= "<< rsd->getPath() << std::endl;
#endif

  FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();

  FFaMsg::setSubTask("Initializing");

  const std::string& mainRDBPath = rsd->getPath();
  ListUI <<"===> Scanning "<< mainRDBPath <<" for results.\n";

  bool noResults = true;
  std::string rdbPath, listWarning, dialogWarning;

  // In batch mode, always ignore found files not present in the RSD
  if (!Fui::hasGUI()) askMissingInRSD = false;

  if (rsd->isEmpty())
  {
    // Check for lost data when RSD is empty - the situation after a crash, etc.
    StringVec modelDir;
    if (FpFileSys::getDirs(modelDir,mainRDBPath,"response_*"))
    {
      // We found something, sort the directories on decreasing task version
      if (modelDir.size() > 1)
	std::sort(modelDir.begin(),modelDir.end(),TaskDirLess);

      // Check if the directories actually contain any results,
      // starting with the highest (latest) task version
      for (const std::string& mdir : modelDir)
      {
#if FP_DEBUG > 3
	std::cout <<"Checking for result in "<< mdir << std::endl;
#endif
	std::string taskName; int taskVer;
	FmResultStatusData::splitRDBName(mdir,taskName,taskVer);
	rdbPath = FFaFilePath::appendFileNameToPath(mainRDBPath,mdir);
	StringSet obsoleteFiles;
	if (!rsd->syncFromRDB(rdbPath,taskName,taskVer,&obsoleteFiles))
	  continue; // this sub-directory is empty

	std::string msg = "The results in " + mdir +
	  " are not listed in the modelfile.\n" +
	  "Do you want to add those results to your model?";

	// Check whether the user wants to update from the abandoned files.
	// But only when running interactively (always ignore in batch runs).
	if (askMissingInRSD && Fui::yesNoDialog(msg.c_str()))
	{
	  ListUI <<"  -> Including results found in "<< mdir <<"\n";

	  if (!obsoleteFiles.empty())
	  {
	    ListUI <<"  -> The following obsolete files are ignored:\n";
	    for (const std::string& file : obsoleteFiles)
	      ListUI <<"     "<< file <<"\n";

	    msg = "Some obsolete files were also found in " + mdir +
	      "\nThese files are ignored. Refer to Output List for details.";
	    Fui::dismissDialog(msg.c_str());
	  }
	}
	else
	{
	  ListUI <<"  -> Ignoring results found in "<< mdir <<"\n";

	  rsd->clear();
	  rsd->setTaskName("response");
	  rsd->setTaskVer(taskVer+1); // incremented to spare the existing dirs
	}
	FpPM::touchModel(true); // Indicate that the model has changed
	noResults = false;
	break; // exit loop
      }
    }

    if (noResults)
    {
      ListUI <<"  -> No simulation results present.\n";
      rsd->setTaskName("response");
      rsd->setTaskVer(1);
    }
  }

  else
  {
    // The RSD is not empty - compare it with the RDB on disk

    StringSet obsoleteFiles;
    rdbPath = rsd->getCurrentTaskDirName(true);
    FmResultStatusData diskRSD;
    diskRSD.setPath(mainRDBPath);
    diskRSD.syncFromRDB(rdbPath, rsd->getTaskName(), rsd->getTaskVer(),
                        &obsoleteFiles);

    StringSet rsdfiles, rdbfiles;
    rsd->getAllFileNames(rsdfiles);
    diskRSD.getAllFileNames(rdbfiles);
#if FP_DEBUG > 3
    reportSet("RSD files:",rsdfiles);
    reportSet("RDB files:",rdbfiles);
#endif

    if (rdbfiles == rsdfiles)
    {
      // We have a match in all files
      if (rdbfiles.empty())
	ListUI <<"  -> No simulation results present.\n";
      else if (extr)
	ListUI <<"  -> Loading results from "<< rdbPath <<"\n";
    }
    else
    {
      // Report what we have found, do we have a Time History?
      ListUI <<"  -> Difference between RSD in model file and on disk.\n"
	     <<"     Trying to find results in "<< rdbPath <<"\n";

      // Find missing files in RDB
      StringVec missingInRDB;
      std::set_difference(rsdfiles.begin(), rsdfiles.end(),
			  rdbfiles.begin(), rdbfiles.end(),
			  std::back_inserter(missingInRDB));
      if (!missingInRDB.empty())
      {
	// Files are missing on disk
	if (FpFileSys::verifyDirectory(rdbPath,false))
	{
	  listWarning = "\nWARNING: The following result files listed in the"
	    "\n         model file are missing in the results database:";
	  for (const std::string& file : missingInRDB)
	    listWarning += std::string("\n\t") +
	      FFaFilePath::getRelativeFilename(mainRDBPath,file);
	  listWarning += "\n";

	  dialogWarning = "The result directory " + rdbPath +
	    "\ncontains less files than specified in the model file.\n"
	    "Refer to the Output List for details.";
	}
	else // The directory does not exist at all
	{
	  listWarning = "\nWARNING: The result directory " + rdbPath +
	    "\n         referred by the model file does not exist.\n";

	  dialogWarning = "The result directory " + rdbPath +
	    "\nreferred by the model file does not exist.";
	}
	rsd->removeFiles(missingInRDB);
	FpPM::touchModel(true); // Indicate that the model has changed
      }

      // Find missing files in RSD
      StringVec missingInRSD;
      std::set_difference(rdbfiles.begin(), rdbfiles.end(),
			  rsdfiles.begin(), rsdfiles.end(),
			  std::back_inserter(missingInRSD));

      if (!missingInRSD.empty())
      {
	size_t nFile = 0;
	ListUI <<"\nWARNING: The following result files found on disk are not"
	  " listed in the model file:";
	std::string msg = "These additional result files were found"
	  " in the result directory:\n";
	for (const std::string& file : missingInRSD)
	{
	  ListUI <<"\n\t"
		 << FFaFilePath::getRelativeFilename(rdbPath,file);
	  if (++nFile < 31)
	    msg += file + "\n";
	  else if (nFile == 31)
	    msg += "[...]\n";
	}

	ListUI <<"\n";
	msg += "\nDo you want to add the additional files to your model?";
	if (askMissingInRSD && Fui::yesNoDialog(msg.c_str()))
	  rsd->addFiles(missingInRSD);
	else
	{
	  ListUI <<" --> Ignored.\n";
	  rsd->ignoreFiles(missingInRSD);
	}
	FpPM::touchModel(true); // Indicate that the model has changed
      }
    }

    if (!obsoleteFiles.empty())
    {
      ListUI <<"  -> Found obsolete files in "<< rdbPath <<" (ignored):";
      for (const std::string& file : obsoleteFiles)
	ListUI <<"\n\t"<< FFaFilePath::getRelativeFilename(rdbPath,file);
      ListUI <<"\n";

      std::string msg = "Some obsolete files were also found in\n" + rdbPath +
	".\nThese files are ignored. Refer to the Output List for details.";
      Fui::dismissDialog(msg.c_str());
    }
  }

  FpPM::setResultFlag(); // Check for results and update UI-sensitivities

  StringSet rsdfiles;
  rsd->getAllFileNames(rsdfiles,"frs");
#if FP_DEBUG > 3
  reportSet("frs-files found in RSD:",rsdfiles);
#endif

  if (!extr) return; // No model extractor, probably batch execution

  // Create a new vector of file names with the correct path,
  // and only of the files that are to be added to the extractor

  StringVec addCandidates;

  // First check for frs-files in the part RSDs (but only for the loaded parts)
  if (includeReducerFiles)
  {
    std::vector<FmPart*> parts;
    FmDB::getAllParts(parts);

    for (FmPart* part : parts)
      if (part->isFELoaded())
      {
	StringSet lnkFrsFiles;
	part->myRSD.getValue().getAllFileNames(lnkFrsFiles,"frs");
	for (const std::string& file : lnkFrsFiles)
	  if (mechData->isEnabled(file) && FpFileSys::isFile(file))
	    addCandidates.push_back(file);
      }

    // Keep track of the frs-files from part reductions
    if (addCandidates.empty())
      includeReducerFiles = false;
    else
      ourReducerFRSs.insert(addCandidates.begin(),addCandidates.end());
  }

  // Then do the solver and recovery files
  for (const std::string& file : rsdfiles)
    if (mechData->isEnabled(file))
    {
      FmPart* part = getPartRelatedToResFile(file);
      if (!part)
	addCandidates.push_back(file); // This file is from the dynamics solver
      else if (file.find("timehist_gage_rcy") != std::string::npos)
	addCandidates.push_back(file); // Always add files from gage recovery
      else if (part->isFELoaded())
	addCandidates.push_back(file); // Only when the part FE data is loaded
    }

  // Report files and warnings
  if (includeReducerFiles)
    reportRDBFiles(addCandidates,mainRDBPath);
  else
    reportRDBFiles(addCandidates,rdbPath);
  filterMostRecentOnly(addCandidates);
#if FP_DEBUG > 3
  reportSet("Adding these files to the extractor:",addCandidates);
#endif

  if (!listWarning.empty())
    FFaMsg::list(listWarning + "\n");

  if (!dialogWarning.empty())
    Fui::dismissDialog(dialogWarning.c_str());

  // Add the files to the extractor
  FFaMsg::enableSubSteps(addCandidates.size());
  extr->addFiles(addCandidates,true);
  FFaMsg::disableSubSteps();
  FFaMsg::setSubTask("");
}


void FpModelRDBHandler::RDBClose(FmResultStatusData* currentRSD,
				 FmResultStatusData* initialRSD,
				 bool deleteFilesNotInRSD,
				 bool clearExtrator)
{
#if FP_DEBUG > 3
  std::cout <<"\nFpModelRDBHandler::RDBClose()"
	    <<"\n\tpath\t= "<< currentRSD->getPath() << std::endl;
#endif

  // Clear the part ID mapping
  ourPartIdMap.clear();

  // Clear the result extractors
  if (clearExtrator) RDBRelease(true);

  // Check if the RDB directory actually exists - nothing to do here if not.
  // Also return if mainRDBPath is empty, otherwise we will try to clean up
  // the current working directory.
  const std::string& mainRDBPath = currentRSD->getPath();
  if (mainRDBPath.empty() || !FpFileSys::verifyDirectory(mainRDBPath,false))
    return;

  if (deleteFilesNotInRSD)
  {
    // Check task version if the current task is the same as the initial
    FmResultStatusData* fromRSD = initialRSD;
    if (currentRSD->getTaskVer() == initialRSD->getTaskVer())
      fromRSD = currentRSD;

    // Update current from disk - to be sure that we've got everything
    std::string rdbPath = fromRSD->getCurrentTaskDirName(true);
    FmResultStatusData diskRSD;
    diskRSD.setPath(mainRDBPath);
    diskRSD.syncFromRDB(rdbPath, fromRSD->getTaskName(), fromRSD->getTaskVer());

    // Find the files that should be removed
    StringSet initrsdfiles, rdbfiles;
    initialRSD->getAllFileNames(initrsdfiles);
    diskRSD.getAllFileNames(rdbfiles);

    StringVec addedFiles;
    std::set_difference(rdbfiles.begin(), rdbfiles.end(),
			initrsdfiles.begin(), initrsdfiles.end(),
			std::back_inserter(addedFiles));

#if FP_DEBUG > 3
    reportSet("Files to be removed:",addedFiles);
#endif
    // Delete new files on disk
    for (const std::string& file : addedFiles)
      if (FpFileSys::isFile(file))
	if (!FpFileSys::deleteFile(file))
	  ListUI <<"  -> Problems deleting file "<< file <<"\n";

    // Delete empty directories
    FpFileSys::removeDir(rdbPath,false);
  }

  // Delete all other RDB dirs:
  size_t nDirs = 0;
  StringVec modelDir;
  std::string nameFilter = currentRSD->getTaskName() + "_*";
  if (FpFileSys::getDirs(modelDir,mainRDBPath,nameFilter.c_str()))
    for (std::string& dir : modelDir)
    {
      int taskVer = FmResultStatusData::getTaskVer(dir);
      if (taskVer > initialRSD->getTaskVer() &&
	  taskVer <= currentRSD->getTaskVer())
      {
	if (!FpFileSys::removeDir(FFaFilePath::makeItAbsolute(dir,mainRDBPath)))
	  ListUI <<"  -> Problems removing directory "<< dir <<"\n";
	else
	  nDirs++;
      }
    }

  // If the RDB dir should now be empty, try to remove it before leaving it
  if (nDirs == modelDir.size()) FpFileSys::removeDir(mainRDBPath,false);
}


void FpModelRDBHandler::RDBSync(FmResultStatusData* currentRSD,
				FmMechanism* mech,
				bool updateExtrator, bool addResFiles)
{
  StringVec newFrsFiles;
  RDBSync(currentRSD,mech,newFrsFiles,updateExtrator,addResFiles,false);
}


void FpModelRDBHandler::RDBSync(FmResultStatusData* currentRSD,
				FmMechanism* mech, StringVec& newFrsFiles,
				bool updateExtrator, bool addResFiles,
				bool checkExistingRSD)
{
#if FP_DEBUG > 5
  std::cout <<"\nFpModelRDBHandler::RDBSync() "<< std::boolalpha
	    << updateExtrator <<" "<< addResFiles <<" "<< checkExistingRSD << std::endl;
#endif

  FmResultStatusData diskRSD;
  diskRSD.setPath(currentRSD->getPath());
  diskRSD.syncFromRDB(currentRSD->getCurrentTaskDirName(true),
                      currentRSD->getTaskName(), currentRSD->getTaskVer());

  StringSet rsdfiles, rdbfiles;
  currentRSD->getAllFileNames(rsdfiles);
  diskRSD.getAllFileNames(rdbfiles);

#if FP_DEBUG > 5
  reportSet("Disk RDB:",rdbfiles);
  reportSet("Mem  RSD:",rsdfiles);
#endif

  // Find extras, add them to the RSD
  StringVec missingInRSD;
  std::set_difference(rdbfiles.begin(), rdbfiles.end(),
		      rsdfiles.begin(), rsdfiles.end(),
		      std::back_inserter(missingInRSD));

  if (!missingInRSD.empty())
  {
#if FP_DEBUG > 4
    reportSet("New files in RSD:",missingInRSD);
#endif
    currentRSD->addFiles(missingInRSD);
    FpPM::touchModel(true); // Indicate that the model has changed
  }

  FpPM::setResultFlag(); // Check for results and update UI-sensitivities

  if (!updateExtrator) return;

  // Create a new vector with the absolute path to the new files.
  // Filter out the frs-files and add these to the extractor.
  // Add files for enabled parts only, unless they contain gage results.

  FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();
  if (!extr) return;

  rsdfiles.clear();
  if (checkExistingRSD) // to catch new files with already an entry in the RSD
    currentRSD->getAllFileNames(rsdfiles);
  else if (!missingInRSD.empty()) // only consider files newly added to the RSD
    rsdfiles.insert(missingInRSD.begin(),missingInRSD.end());
  if (rsdfiles.empty()) return;

  StringVec newResFiles;
  for (const std::string& file : rsdfiles)
    if (FFaFilePath::isExtension(file,"frs"))
    {
      // If we are checking existing RSD files too, only add files not
      // already present in the extractor, unless it is a disabled file
      if (!checkExistingRSD || (!extr->getResultContainer(file) && mech->isEnabled(file)))
      {
	FmPart* part = getPartRelatedToResFile(file);
	if (!part)
	  newFrsFiles.push_back(file); // frsFile is from the dynamics solver
	else if (file.find("timehist_gage_rcy") != std::string::npos)
	  newFrsFiles.push_back(file); // frsFile is from a gage recovery process
	else if (part->isFELoaded())
	  newFrsFiles.push_back(file); // frsFile is associated with an enabled part
#if FP_DEBUG > 3
	else
	  std::cout <<"Ignoring "<< file <<" because "
		    << part->getIdString(true) <<" is not loaded."<< std::endl;
#endif
      }
    }
    else if (addResFiles && FFaFilePath::isExtension(file,"res"))
    {
      if (!checkExistingRSD || !extr->getResultContainer(file))
	newResFiles.push_back(file);
    }

#if FP_DEBUG > 4
  reportSet("New frs-files added to extractor:",newFrsFiles);
  reportSet("New res-files added to extractor:",newResFiles);
#endif
  if (!newFrsFiles.empty())
    extr->addFiles(newFrsFiles);

  // Add the new res-files found to the extractor also, for progress polling
  if (!newResFiles.empty())
  {
    extr->addFiles(newResFiles);
    ourAddedRESs.insert(newResFiles.begin(),newResFiles.end());
  }
}


void FpModelRDBHandler::RDBSave(FmResultStatusData* currentRSD,
				FmResultStatusData* initialRSD,
				bool pruneEmptyDirs)
{
#if FP_DEBUG > 3
  std::cout <<"\nFpModelRDBHandler::RDBSave()"
	    <<"\n\tpath\t= "<< currentRSD->getPath()
	    <<"\n\ttaskdir\t= "<< currentRSD->getCurrentTaskDirName(true) << std::endl;
#endif

  const std::string& mainRDBPath = currentRSD->getPath();
  std::string rdbPath = currentRSD->getCurrentTaskDirName(true);

  StringSet obsoleteFiles;
  FmResultStatusData diskRSD;
  diskRSD.setPath(mainRDBPath);
  diskRSD.syncFromRDB(rdbPath,
                      currentRSD->getTaskName(), currentRSD->getTaskVer(),
                      &obsoleteFiles);

  StringSet rsdfiles, rdbfiles;
  currentRSD->getAllFileNames(rsdfiles);
  diskRSD.getAllFileNames(rdbfiles);

  initialRSD->copy(&diskRSD);
  currentRSD->copy(&diskRSD);

#if FP_DEBUG > 3
  reportSet("RSD files:",rsdfiles);
  reportSet("Disk files:",rdbfiles);
  reportSet("Obsolete files:",obsoleteFiles);
#endif

  // warn if disk and current are different
  // add difference between disk and mem files to obsoleteFiles:
  std::set_difference(rdbfiles.begin(), rdbfiles.end(),
		      rsdfiles.begin(), rsdfiles.end(),
		      std::inserter(obsoleteFiles,obsoleteFiles.end()));

  if (rdbfiles != rsdfiles)
    FFaMsg::list("  -> Differences in stored RSD and the RSD on disk. Saving based on disk RSD.\n");

  // Remove the obsolete files:
  for (const std::string& file : obsoleteFiles)
    if (!FpFileSys::deleteFile(file))
      FFaMsg::list("  -> Problems deleting file " + file + "\n");

  if (pruneEmptyDirs)
    FpFileSys::removeDir(rdbPath,false);

  // Delete all other RDB dirs:
  size_t nDirs = 0;
  StringVec modelDir;
  std::string nameFilter = currentRSD->getTaskName() + "_*";
  if (FpFileSys::getDirs(modelDir,mainRDBPath,nameFilter.c_str()))
    for (std::string& dir : modelDir)
      if (FmResultStatusData::getTaskVer(dir) != currentRSD->getTaskVer())
      {
	if (!FpFileSys::removeDir(FFaFilePath::makeItAbsolute(dir,mainRDBPath)))
	  ListUI <<"  -> Problems removing directory "<< dir <<"\n";
	else
	  nDirs++;
      }

  // If the RDB dir now is empty, try to remove it
  if (nDirs == modelDir.size()) FpFileSys::removeDir(mainRDBPath,false);
}


void FpModelRDBHandler::RDBSaveAs(const std::string& RDBPath,
				  FmResultStatusData* currentRSD,
				  FmResultStatusData* initialRSD,
				  FmMechanism* mech, const std::string& subPath)
{
#if FP_DEBUG > 3
  std::cout <<"\nFpModelRDBHandler::RDBSaveAs()\n\tpath\t="
	    << FFaFilePath::appendFileNameToPath(RDBPath,subPath) << std::endl;
#endif

  // Define the new task directory
  std::string newTaskName("response");
  const int newTaskVer = 1;
  FmResultStatusData diskRSD(newTaskName);
  diskRSD.setPath(FFaFilePath::appendFileNameToPath(RDBPath,subPath));

  std::string newRdbPath = diskRSD.getCurrentTaskDirName(true) + FFaFilePath::getPathSeparator();
  std::string newTaskDir = FFaFilePath::getRelativeFilename(RDBPath,newRdbPath);

  // Find current task directory
  std::string oldRDBPath = mech->getAbsModelRDBPath();
  std::string oldRdbPath = currentRSD->getCurrentTaskDirName(true) + FFaFilePath::getPathSeparator();
  std::string oldTaskDir = FFaFilePath::getRelativeFilename(oldRDBPath,oldRdbPath);

  if (FpFileSys::verifyDirectory(oldRdbPath,false))
  {
    // We have an existing RDB directory, find all files in it
    StringSet filesToCopy, dirsToCreate;
    diskRSD.syncFromRDB(oldRdbPath,newTaskName,newTaskVer);
    diskRSD.getAllFileNames(filesToCopy);
    if (!filesToCopy.empty())
    {
      dirsToCreate.insert(RDBPath);
      if (!subPath.empty())
	dirsToCreate.insert(FFaFilePath::appendFileNameToPath(RDBPath,subPath));
      diskRSD.getAllDirNames(dirsToCreate);
    }

    // Create new RDB directories
    for (const std::string& dir : dirsToCreate)
      if (!FpFileSys::verifyDirectory(dir))
	ListUI <<"  -> Problems creating "<< dir <<"\n";

    // Now copy all RDB files to the new location
    // while keeping track of disabled files, if any
    for (const std::string& file : filesToCopy)
    {
      std::string fileName = FFaFilePath::getRelativeFilename(newRdbPath,file);
      if (!FpFileSys::copyFile(oldRdbPath+fileName,file))
	ListUI <<"  -> Problems copying file "<< oldTaskDir+fileName <<"\n";
      if (mech->enableResultFile(oldTaskDir+fileName))
	mech->disableResultFile(newTaskDir+fileName);
    }
  }

  // Closing the current RDB directory.
  // The result extrator is cleared only if subPath is empty.
  bool purgeRSD = subPath.empty() || !initialRSD->isEmpty();
  RDBClose(currentRSD,initialRSD,purgeRSD,subPath.empty());

  initialRSD->copy(&diskRSD);
  currentRSD->copy(&diskRSD);

  // The RDB is not reopened here because we first need to
  // do some translation of relative path names, etc.
}


void FpModelRDBHandler::RDBIncrement(FmResultStatusData* currentRSD,
				     FmMechanism* mech,
				     bool updateExtrator)
{
#if FP_DEBUG > 3
  std::cout <<"\nFpModelRDBHandler::RDBIncrement()"<< std::endl;
#endif

  if (!currentRSD->isEmpty())
  {
    // Update the list of disabled result files
    StringSet responseFiles;
    currentRSD->getAllFileNames(responseFiles);
    removeDisabledFiles(mech,responseFiles);

    currentRSD->incrementTaskVer();

    FpPM::touchModel(true); // Indicate that the model has changed
    FpPM::setResultFlag();  // Check for results and update UI-sensitivities
  }

  // Renew the extrator to reflect the new RDB (retaining reducer files, if any)
  if (updateExtrator) RDBRelease(false);
}


void FpModelRDBHandler::RDBRelease(bool reducerFilesToo, bool noRenewal)
{
#if FP_DEBUG > 3
  std::cout <<"\nFpModelRDBHandler::RDBRelease()"<< std::endl;
  if (!reducerFilesToo) reportSet("Retained frs-files:",ourReducerFRSs);
#endif

  // We don't need RDB extractors when execution in batch mode
  if (noRenewal || !Fui::hasGUI())
    FpRDBExtractorManager::instance()->clearExtractors();
  else
  {
    if (reducerFilesToo) ourReducerFRSs.clear();
    FpRDBExtractorManager::instance()->renewExtractors(ourReducerFRSs);
  }
}


/*!
  Returns the valid time steps for the specified \a rdbResultGroup.
*/

void FpModelRDBHandler::getKeys(FmResultStatusData* currentRSD,
				std::set<double>& validRdbTimes,
				const std::string& rdbResultGroup)
{
  StringSet resultFiles;
  if (!currentRSD->getFrsFiles(resultFiles,rdbResultGroup)) return;

  FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();
  if (extr) extr->getValidKeys(validRdbTimes,resultFiles);
}


void FpModelRDBHandler::enableTimeStepPreRead(FmResultStatusData* currentRSD,
					      const std::string& rdbResultGroup)
{
  StringSet resultFiles;
  if (!currentRSD->getFrsFiles(resultFiles,rdbResultGroup)) return;

  FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();
  if (extr) extr->enableTimeStepPreRead(resultFiles);
}


void FpModelRDBHandler::disableTimeStepPreRead()
{
  FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();
  if (extr) extr->disableTimeStepPreRead();
}


void FpModelRDBHandler::clearPreReadTimeStep()
{
  FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();
  if (extr) extr->clearPreReadTimeStep();
}


/*!
  Checks if RDB has results within the specified \a rdbResultGroup.
*/

bool FpModelRDBHandler::hasResults(FmResultStatusData* currentRSD,
				   const std::string& rdbResultGroup)
{
  StringSet resultFiles;
  return currentRSD->getFrsFiles(resultFiles,rdbResultGroup,true);
}


/*!
  Runs over all FE parts and adds or removes result files
  from extractor according to part ram level settings
*/

void FpModelRDBHandler::RDBSyncOnParts(FmResultStatusData* rsd, FmMechanism* mech)
{
  FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();
  if (!extr) return;

  // All frs files in rsd
  StringSet frsFiles;
  rsd->getAllFileNames(frsFiles,"frs");
  if (frsFiles.empty()) return;

  // All FE parts in the model
  std::vector<FmPart*> parts;
  FmDB::getFEParts(parts);
  if (parts.empty()) return;

  // Creates set of disabled files with absolute path
  StringVec absDisabled;
  mech->getDisabledResultFiles(absDisabled,true);

  // Loop over FE parts, remove or add files to/from the extractor.

  int partNr = 0;
  FFaMsg::enableSubSteps(parts.size());
  for (FmPart* part : parts)
  {
    FFaMsg::setSubStep(++partNr);
    FFaMsg::setSubTask(part->baseFTLFile.getValue());

    // Find frs-files with the string partName in their full name, and
    // the subset among those having timehist_gage_rcy in their full name
    std::string partName = getIdName(part);
    StringSet candidates, gageRecoveryFiles;
    for (const std::string& file : frsFiles)
      if (file.find(partName) != std::string::npos)
      {
	candidates.insert(file);
	if (file.find("timehist_gage_rcy") != std::string::npos)
	  gageRecoveryFiles.insert(file);
      }

    // Remove the gage related files from the part related frs files
    StringVec candidatesWoGage;
    std::set_difference(candidates.begin(), candidates.end(),
			gageRecoveryFiles.begin(), gageRecoveryFiles.end(),
			std::inserter(candidatesWoGage,candidatesWoGage.begin()));

    // Then remove those who are disabled already
    StringSet finalCandidates;
    std::set_difference(candidatesWoGage.begin(), candidatesWoGage.end(),
			absDisabled.begin(), absDisabled.end(),
			std::inserter(finalCandidates,finalCandidates.begin()));
#if FP_DEBUG > 2
    reportSet("Adding/removing these files from the extractor:",finalCandidates);
#endif

    // Add or remove the files found from the extractor

    if (part->isFELoaded()) {
      FFaMsg::pushStatus("Reading result info");
      extr->addFiles(finalCandidates, true);
      FFaMsg::popStatus();
    }
    else {
      FFaMsg::pushStatus("Clearing result info");
      extr->removeFiles(finalCandidates);
      FFaMsg::popStatus();
    }
  }

  FFaMsg::setSubTask("");
  FFaMsg::disableSubSteps();

#if FP_DEBUG > 4
  reportSet("Files in extractor after sync:",extr->getAllResultContainerFiles());
#endif
}


/*!
  Removes the specified \a rdbResultGroup from the extractor and deletes all
  files in the directories belonging to it. If \a filterOnPart is non-zero,
  the specified part is used as a filter of which results to remove.
*/

void FpModelRDBHandler::removeResults(const std::string& rdbResultGroup,
				      FmResultStatusData* currentRSD,
				      FmPart* filterOnPart)
{
  // Check if we have a sub-task with this name
  currentRSD = currentRSD->getSubTask(rdbResultGroup);

  // Check if we have a sub-task for the specified part
  if (currentRSD && filterOnPart)
    currentRSD = currentRSD->getSubTask(getIdName(filterOnPart));

  if (!currentRSD)
  {
    FFaMsg::list("===> No results in memory.\n");
    return;
  }

  // Get file names for this sub-task
  StringSet resultFiles, allFiles;
  currentRSD->getAllFileNames(resultFiles,"frs");
  currentRSD->getAllFileNames(allFiles);
  currentRSD->incrementTaskVer();

  // Remove from the set of disabled files
  removeDisabledFiles(FmDB::getMechanismObject(),resultFiles);

  // Remove all obsolete files from extractor and disk
  remove_result_files(allFiles);

  // Delete the directory of the old sub-task, which now should be empty
  FpFileSys::removeDir(currentRSD->getPath(),false);
}


/*!
  Removes all truncated frs files in the specified \a rsd.
*/

void FpModelRDBHandler::purgeTruncatedResultFiles(FmResultStatusData* rsd)
{
  if (!rsd) return;

  StringSet frsFiles;
  rsd->getAllFileNames(frsFiles,"frs");
  if (frsFiles.empty()) return;

  StringSet truncFiles;
  for (const std::string& file : frsFiles)
    if (FpFileSys::getFileSize(file) == 0)
      truncFiles.insert(file);

  // Remove from the set of disabled files, the RSD, extractor and disk
  removeResultFiles(truncFiles,rsd);
}


/*!
  Removes all files in the specified \a rsd.
*/

bool FpModelRDBHandler::removeAllFiles(FmResultStatusData* rsd)
{
  if (!rsd) return true;

  StringSet allFiles;
  rsd->getAllFileNames(allFiles);
  if (allFiles.empty()) return true;

  bool ok = FFaMsg::dialog("The results database will not be incremented.\nAll existing results" +
                           FFaNumStr(" (%zu files) will be overwritten.",allFiles.size()) +
                           "\n\nContinue ?",FFaMsg::OK_CANCEL);

  // Remove from the set of disabled files, the RSD, extractor and disk
  if (ok) removeResultFiles(allFiles,rsd);
  return ok;
}


/*!
  Removes \a files from part RSD and extractor, and deletes them from disk.
  \note The files must have absolute path.
*/

void FpModelRDBHandler::removeResultFiles(const StringSet& files, FmPart* part)
{
  // Remove from the set of disabled files
  removeDisabledFiles(FmDB::getMechanismObject(),files);

  // Remove from the part RSD
  if (part) part->myRSD.getValue().removeFiles(files);

  // Remove from extractor and disk
  remove_result_files(files);
}


/*!
  Removes \a files from given RSD and extractor, and deletes them from disk.
  \note The files must have absolute path.
*/

void FpModelRDBHandler::removeResultFiles(const StringSet& files,
					  FmResultStatusData* rsd)
{
  // Remove from the set of disabled files
  removeDisabledFiles(FmDB::getMechanismObject(),files);

  // Remove from the RSD
  rsd->removeFiles(files);

  // Remove from extractor and disk
  remove_result_files(files);
}


/*!
  Removes added res-files (if any) from the extractor.
*/

void FpModelRDBHandler::removeResFiles()
{
  if (ourAddedRESs.empty()) return;

#if FP_DEBUG > 3
  reportSet("Removing these res-files from the extractor:",ourAddedRESs);
#endif
  // Remove all added res-files from the extractor
  FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();
  if (extr) extr->removeFiles(ourAddedRESs);

  ourAddedRESs.clear();
}


/*!
  Removes \a files from the set of disabled result files.
  To be used internally when results are cleared.
  \note If \a files is empty \b ALL files are cleared.
*/

void FpModelRDBHandler::removeDisabledFiles(FmMechanism* mech,
					    const StringSet& files)
{
  bool changed = false;
  if (files.empty())
    changed = mech->enableAllResultFiles();
  else if (mech->hasDisabledFiles())
  {
    // The names of the current disabled result files are assumed relative
    // to the top level model RDB path stored in the mechanism object.
    std::string rdbPath = mech->getAbsModelRDBPath();
    for (const std::string& fn : files)
      if (mech->enableResultFile(FFaFilePath::getRelativeFilename(rdbPath,fn)))
	changed = true;
  }

  // Indicate whether the model has changed and needs to be saved
  if (changed) FpPM::touchModel(true);
}


/*!
  Will enable/disable the result containers \a files. This means that they will
  be added to/removed from the FFrExtractor, but will remain present in the RSD.
  A list of the currently disabled files is maintained in the Mechanism object.

  \note Expects absolute paths in, if not it will fail miserably.
  \note Result files from stress recovery might not be added to the extractor,
        depending on the RAM level setting for the associated FE Parts.
*/

void FpModelRDBHandler::changeResultFilesState(FmMechanism* mech,
					       const StringSet& files,
					       bool enable)
{
  FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();
  if (!extr || files.empty()) return;

#if FP_DEBUG > 1
  std::cout <<"FpModelRDBHandler::changeResultFilesState("<< (enable ? "ON)" : "OFF)");
  reportSet("Files in:",files);
#endif

  bool success = true;
  std::string rdbPath = mech->getAbsModelRDBPath();

  if (enable)
  {
    ListUI <<"  -> Enabling results:\n";
    StringSet filteredFiles;

    for (const std::string& file : files)
    {
      // Need to check the RAM level setting if we are enabling stress recovery files
      FmPart* part = getPartRelatedToResFile(file);
      if (!part)
	filteredFiles.insert(file);
      else if (part->isFELoaded())
	filteredFiles.insert(file);
      else if (file.find("timehist_gage_rcy") != std::string::npos)
	filteredFiles.insert(file);
      else
        continue;

      if (mech->enableResultFile(FFaFilePath::getRelativeFilename(rdbPath,file)))
        ListUI <<"     "<< FFaFilePath::getRelativeFilename(rdbPath,file) <<"\n";
    }

    success = extr->addFiles(filteredFiles,true);
  }
  else
  {
    ListUI <<"  -> Disabling results:\n";
    for (const std::string& file : files)
      if (mech->disableResultFile(FFaFilePath::getRelativeFilename(rdbPath,file)))
        ListUI <<"     "<< FFaFilePath::getRelativeFilename(rdbPath,file) <<"\n";

    success = extr->removeFiles(files);
  }

#ifdef FP_DEBUG
  reportSet("Current result containers:",extr->getAllResultContainerFiles());
#endif

  if (!success)
    ListUI <<"===> Problems "<< (enable ? "en" : "dis")
	   <<"abling the selected result files.\n";

  FpPM::touchModel(true); // Indicate that the model has changed and needs save
}


/*!
  Syncronizes the part RSD and result extractor for \a part with result files
  found on disk. If \a addResFiles is true, the res-files found are also added
  to the result extractor. This is mainly used for progress polling in the
  result file browser while a reducer process is running. The function is also
  invoked whenever a reducer process is terminated on \a part or after finding
  a new directory containing part matrix files with matching part checksum.
*/

void FpModelRDBHandler::RDBSync(FmPart* part, FmMechanism* mech,
				bool addResFiles, const std::string& RDBPath)
{
#if FP_DEBUG > 3
  std::cout <<"\nFpModelRDBHandler::RDBSync() "<< part->getLinkIDString();
  if (!RDBPath.empty()) std::cout <<"\n\tnew path = "<< RDBPath;
  std::cout << std::endl;
#endif

  StringSet oldFiles, oldResFiles;
  std::string partPath, taskName; int taskVer = -1;
  FmResultStatusData& partRSD = part->myRSD.getValue();
  if (!partRSD.isEmpty())
  {
    // Find existing part RDB path
    partRSD.setPath(part->getAbsFilePath());
    taskName = partRSD.getTaskName();
    taskVer  = partRSD.getTaskVer();

    // Find absolute path to the old frs- and res-files of this part
    partRSD.getAllFileNames(oldFiles,"res");
    if (!addResFiles && RDBPath.empty()) oldResFiles = oldFiles;
    partRSD.getAllFileNames(oldFiles,"frs");
  }

  if (!RDBPath.empty())
  {
    // A new part RDB path is specified
    partRSD.setPath(FFaFilePath::getPath(RDBPath,false));
    if (RDBPath[RDBPath.size()-1] == FFaFilePath::getPathSeparator())
      partPath = RDBPath.substr(0,RDBPath.size()-1);
    else
      partPath = RDBPath;
    FmResultStatusData::splitRDBName(FFaFilePath::getFileName(partPath),
				     taskName, taskVer);
  }
  else
    partPath = partRSD.getCurrentTaskDirName(true);

  // Update the part RSD from disk
  if (!partRSD.syncFromRDB(partPath,taskName,taskVer))
    if (oldFiles.empty()) return; // No change

  FpPM::touchModel(true); // Indicate that the model has changed and needs save

  FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();

  // Remove the old frs-files from the extractor and the set of disabled files
  if (!oldFiles.empty() && !RDBPath.empty())
  {
#if FP_DEBUG > 3
    reportSet("Removing these files from the extractor:",oldFiles);
#endif
    for (const std::string& file : oldFiles)
      ourReducerFRSs.erase(file);
    removeDisabledFiles(mech,oldFiles);
    if (extr) extr->removeFiles(oldFiles);
    oldFiles.clear();
  }
  else if (!oldResFiles.empty())
  {
    // Remove the res-files from the extractor
#if FP_DEBUG > 3
    reportSet("Removing these files from the extractor:",oldResFiles);
#endif
    if (extr) extr->removeFiles(oldResFiles);
  }

  // Add the new frs- and/or res-files to the extractor
  StringSet newFiles;
  if (part->isFELoaded())
    partRSD.getAllFileNames(newFiles,"frs");
  if (addResFiles)
    partRSD.getAllFileNames(newFiles,"res");

  if (newFiles.empty() || newFiles == oldFiles) return; // no new files

#if FP_DEBUG > 3
  reportSet("Adding these files to the extractor:",newFiles);
#endif
  ourReducerFRSs.insert(newFiles.begin(),newFiles.end());
#if FP_DEBUG > 3
  reportSet("Reducer files after adding:",ourReducerFRSs);
#endif
  if (extr) extr->addFiles(newFiles);
}


double FpModelRDBHandler::updateModel(double atTime)
{
  double gottenTime = atTime > 1.0e-12 ? -atTime : -999.999;
  FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();
  if (!extr) return gottenTime;

  // Position the RDB to desired time
  if (!extr->positionRDB(atTime,gottenTime))
    return gottenTime;

  int ierr = 0;
  double posMat[12];

  // Update all parts in the model
  std::vector<FmPart*> allParts;
  FmDB::getAllParts(allParts);
  for (FmPart* part : allParts)
    if (!part->isSuppressed())
    {
      FFrEntryBase* pos = extr->findVar("Part",part->getBaseID(),"Position matrix");
      if (!pos)
      {
        ++ierr;
        ListUI <<"\n *** Failed to locate updated posposition matrix for "<< part->getIdString(true);
      }
      else if (extr->getSingleTimeStepData(pos,posMat,12) < 12)
      {
        ++ierr;
        ListUI <<"\n *** Failed to read updated position matrix for "<< part->getIdString(true);
      }
      else
        part->setGlobalCS(posMat);
    }

  // Update all triads in the model
  std::vector<FmTriad*> allTriads;
  FmDB::getAllTriads(allTriads);
  for (FmTriad* triad : allTriads)
    if (triad->getNDOFs() > 0 && !triad->fullyConstrained(true))
    {
      FFrEntryBase* pos = extr->findVar("Triad",triad->getBaseID(),"Position matrix");
      if (!pos)
      {
        ++ierr;
        ListUI <<"\n *** Failed to locate updated posposition matrix for "<< triad->getIdString(true);
      }
      else if (extr->getSingleTimeStepData(pos,posMat,12) < 12)
      {
        ++ierr;
        ListUI <<"\n *** Failed to read updated position matrix for "<< triad->getIdString(true);
      }
      else
        triad->setGlobalCS(posMat);
    }

  if (ierr == 0) return gottenTime;

  ListUI <<"\n===> A total of "<< ierr <<" read failures detected. Model is not updated.\n";
  return -gottenTime;
}
