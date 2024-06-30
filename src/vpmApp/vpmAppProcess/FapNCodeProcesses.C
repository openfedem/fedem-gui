// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <sstream>
#include <fstream>

#include "vpmApp/vpmAppProcess/FapNCodeProcesses.H"
#include "vpmApp/vpmAppProcess/FapSolutionProcessMgr.H"
#include "vpmApp/vpmAppProcess/FapSolverID.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "vpmDB/FmModelFileQuery.H"
#include "vpmDB/FmResultStatusData.H"
#include "vpmDB/FmDutyCycleOptions.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmDB.H"
#include "vpmPM/FpProcessOptions.H"
#include "vpmPM/FpFileSys.H"

#ifdef win32
#define ADD_EXE(program) program + std::string(".exe")
#else
#define ADD_EXE(program) program
#endif


FapFPPHCOPY::FapFPPHCOPY(const std::string& master, const std::string& current)
{
  this->masterFpp = master;
  this->currentFpp = current;

  myGroupID = FapSolverID::FAP_DC_FPPHCOPY;
  mySolverName = this->npath + ADD_EXE("fpphcopy");

  tmpMaster = tmpDir + "master.fpp";
  tmpCurrent = tmpDir + "current.fpp";
}


void FapFPPHCOPY::setFinishedCB(const FFaDynCB1<int>& dynCB)
{
  myFinishedCB = dynCB;
}


int FapFPPHCOPY::checkDependencies()
{
#ifdef FAP_DEBUG
  std::cout <<"FapFPPHCOPY::checkDependencies()"<< std::endl;
#endif

  if (currentFpp == masterFpp)
    return FAP_RESULTS_OK;

  if (!this->isExecutable(mySolverName))
    return FAP_NOT_EXECUTABLE;

  bool canReadMaster  = FpFileSys::isReadable(masterFpp);
  bool canReadCurrent = FpFileSys::isReadable(currentFpp);
  if (!canReadMaster || !canReadCurrent)
  {
    FFaMsg::list("===> ERROR: Could not read input fpp-files:\n",true);
    if (!canReadMaster)  FFaMsg::list("            "+ masterFpp +"\n");
    if (!canReadCurrent) FFaMsg::list("            "+ currentFpp +"\n");
    return FAP_NOT_EXECUTABLE;
  }
  else if (!FpFileSys::isWritable(currentFpp))
  {
    FFaMsg::list("===> ERROR: fpp-file "+ currentFpp +" is write-protected\n",true);
    return FAP_NOT_EXECUTABLE;
  }

  if (!this->createTmpDir())
    return FAP_NOT_EXECUTABLE;

  canReadMaster  = FpFileSys::copyFile(masterFpp,tmpMaster);
  canReadCurrent = FpFileSys::copyFile(currentFpp,tmpCurrent);
  if (canReadCurrent && canReadMaster) return FAP_READY_TO_RUN;

  FFaMsg::list("===> ERROR: Could not copy fpp-files to temporary directory ["+ this->tmpDir +"]\n",true);
  if (!canReadCurrent) FFaMsg::list("            "+ currentFpp +"\n");
  if (!canReadMaster)  FFaMsg::list("            "+ masterFpp +"\n");
  this->deleteTmpDir();
  return FAP_NOT_EXECUTABLE;
}


int FapFPPHCOPY::execute()
{
#ifdef FAP_DEBUG
  std::cout <<"FapFPPHCOPY::execute()"<< std::endl;
#endif

  int depVal = this->checkDependencies();
  if (depVal != FAP_READY_TO_RUN)
    return depVal;

  FpProcessOptions opts;
  opts.name = mySolverName;
  opts.args.push_back("-i");
  opts.args.push_back(tmpMaster);
  opts.args.push_back("-p");
  opts.args.push_back(tmpCurrent);
  opts.args.push_back("-o");
  opts.deathHandler = FFaDynCB1M(FapFPPHCOPY,this,onActualProcessDeath,int);

  FFaMsg::list("\n===> Copying fpp-file header...\n");
  if (this->run(opts,"fppcopy"))
    return FAP_NOT_EXECUTABLE;

  FFaMsg::pushStatus("Running nCode fpphcopy");
  FFaMsg::setSubTask(FFaFilePath::getFileName(currentFpp));

  return FAP_STARTED;
}


void FapFPPHCOPY::onActualProcessDeath(int exitValue)
{
  if (exitValue != 0)
    FFaMsg::list("===> ERROR: Unable to copy fpp-file header\n", true);
  else if (!FpFileSys::copyFile(tmpCurrent, currentFpp))
  {
    exitValue = 1;
    FFaMsg::list("===> ERROR: Unable to copy fpp-file back into RDB from " + this->tmpDir + "\n", true);
  }
  else
  {
    FFaMsg::list("===> Done\n");
    this->deleteTmpDir();
  }

  FFaMsg::popStatus();
  FFaMsg::setSubTask("");

  myFinishedCB.invoke(exitValue);

  // Kill me
  FapSolutionProcessManager::instance()->onSolverProcessDeath(this->getProcessSignature(), exitValue);
}


std::string FapFPPHCOPY::getProcessSignature() const
{
  if (myProcessSignature.empty())
    myProcessSignature = mySolverName + "@" + currentFpp;

#ifdef FAP_DEBUG
  std::cout <<"\nProcess sign: "<< myProcessSignature << std::endl;
#endif

  return myProcessSignature;
}


FapFATFE::FapFATFE(const std::string& fpp, FmPart* part)
{
  currentFpp = fpp;
  myPart = part;
  myGroupID = FapSolverID::FAP_DC_FATFE;

  mySolverName = this->npath + ADD_EXE("fatfe");
  hcopyFinished = false;
  hcopyStarted = false;
}


int FapFATFE::checkDependencies()
{
#ifdef FAP_DEBUG
  std::cout <<"FapFATFE::checkDependencies()"<< std::endl;
#endif

  if (!this->isExecutable(mySolverName))
    return FAP_NOT_EXECUTABLE;

  if (FapSolutionProcessManager::instance()->isGroupRunning(FapSolverID::FAP_DC_FPPHCOPY))
  {
#if FAP_DEBUG > 1
    std::cout <<"fpphcopy is running, please wait"<< std::endl;
#endif
    return FAP_PENDING_DEPENDENCIES_BUT_WAIT;
  }

  if (FpFileSys::isReadable(FFaFilePath::getBaseName(currentFpp) + ".fef"))
    return FAP_RESULTS_OK;

  if (!FpFileSys::isReadable(currentFpp)) {
    FFaMsg::list("===> ERROR: Cannot read " + currentFpp + "\n", true);
    return FAP_NOT_EXECUTABLE;
  }

  if (hcopyFinished) {

    if (!this->createTmpDir())
      return FAP_NOT_EXECUTABLE;

    this->tmpFile = this->tmpDir + FFaLowerCaseString(FFaFilePath::getFileName(currentFpp));

    if (!FpFileSys::copyFile(currentFpp, this->tmpFile)) {
      FFaMsg::list("===> ERROR: Unable to copy fpp-file to temporary location\n", true);
      return FAP_NOT_EXECUTABLE;
    }

    return FAP_READY_TO_RUN;
  }

  if (!hcopyStarted) {
    std::string masterEvent = FmDB::getDutyCycleOptions()->getMasterEvent();
    if (masterEvent.empty()) {
      FFaMsg::list("===> ERROR: Could not find master event\n", true);
      return FAP_NOT_EXECUTABLE;
    }

    if (!FpFileSys::isReadable(masterEvent)) {
      FFaMsg::list("===> ERROR: Cannot read master event model file\n", true);
      FFaMsg::list("            "+ masterEvent +"\n");
      return FAP_NOT_EXECUTABLE;
    }

    FmModelFileQuery query(masterEvent);
    std::string rsdString = query.getFirstEntry("RESULT_STATUS_DATA");
    if (rsdString == "") {
      FFaMsg::list("===> ERROR: Master event doesn't seem to have any results\n", true);
      return FAP_NOT_EXECUTABLE;
    }

    FmResultStatusData topRsd;
    topRsd.setPath(FFaFilePath::getBaseName(masterEvent) + "_RDB");
    std::istringstream is(rsdString.c_str());
    topRsd.read(is);

    FmResultStatusData* subRsd = topRsd.getSubTask("summary_rcy");
    if (subRsd->isEmpty()) {
      FFaMsg::list("===> ERROR: Strain Coat Recovery is not run on master event\n", true);
      return FAP_NOT_EXECUTABLE;
    }

    subRsd = subRsd->getSubTask(myPart->getTaskName());
    if (subRsd->isEmpty()) {
      FFaMsg::list("===> ERROR: Strain Coat Recovery is not run on master event\n", true);
      return FAP_NOT_EXECUTABLE;
    }

    std::set<std::string> files;
    subRsd->getAllFileNames(files,"fpp");

    if (files.empty()) {
      FFaMsg::list("===> ERROR: Strain Coat Recovery is not run on master event\n", true);
      return FAP_NOT_EXECUTABLE;
    }

    std::string masterFpp = *files.rbegin();
#ifdef FAP_DEBUG
    std::cout <<"master fpp-file: "<< masterFpp
	      <<"\nslave fpp-file : "<< currentFpp << std::endl;
#endif

    if (!FpFileSys::isReadable(masterFpp)) {
      FFaMsg::list("===> ERROR: Strain Coat Recovery is not run on master event\n", true);
      return FAP_NOT_EXECUTABLE;
    }

    if (masterFpp == currentFpp)
      hcopyFinished = true;
    else {
      FapFPPHCOPY* hcopy = new FapFPPHCOPY(masterFpp, currentFpp);
      hcopy->setFinishedCB(FFaDynCB1M(FapFATFE,this,onHeaderCopyFinished,int));
      hcopyStarted = true;
      FapSolutionProcessManager::instance()->pushSolverProcess(hcopy);
    }
    return FAP_PENDING_DEPENDENCIES;
  }

  return FAP_PENDING_DEPENDENCIES_BUT_WAIT;
}


int FapFATFE::execute()
{
#ifdef FAP_DEBUG
  std::cout <<"FapFATFE::execute()"<< std::endl;
#endif
  int depVal = this->checkDependencies();
  if (depVal != FAP_READY_TO_RUN)
    return depVal;

  FpProcessOptions opts;
  opts.name = mySolverName;
  opts.args.push_back("/job=" + tmpFile);
  opts.args.push_back("/ana=y");
  opts.args.push_back("/out=" + FFaFilePath::getBaseName(tmpFile,true));
  opts.args.push_back("/eopt=l");
  opts.args.push_back("/qload=yes");
  opts.workingDir = this->tmpDir;
  opts.deathHandler = FFaDynCB1M(FapFATFE,this,onActualProcessDeath,int);

  FFaMsg::list("===> Running FE-Fatigue in batch mode on\n     " + currentFpp + "\n");
  if (this->run(opts,"fatfe") < 0)
    return FAP_NOT_EXECUTABLE;

  FFaMsg::pushStatus("Running nCode FE-Fatigue (batch)");
  FFaMsg::setSubTask(FFaFilePath::getFileName(currentFpp));

  return FAP_STARTED;
}


std::string FapFATFE::getProcessSignature() const
{
  if (myProcessSignature.empty())
    myProcessSignature = mySolverName + "@" + currentFpp;

#ifdef FAP_DEBUG
  std::cout <<"\nProcess sign: "<< myProcessSignature << std::endl;
#endif
  return myProcessSignature;
}


void FapFATFE::onActualProcessDeath(int exitValue)
{
  if (exitValue != 0)
    FFaMsg::list("===> ERROR: FE-Fatigue returned with an unknown error\n", true);

  else {
    // Copy fef-file into rdb
    std::string fefFile = FFaFilePath::getBaseName(tmpFile) + ".fef";
    std::string fef1File = FFaFilePath::getBaseName(tmpFile) + "_1.fef";
    // TODO copy all fef- AND frs-files

    if (FpFileSys::isReadable(fefFile) &&
	!FpFileSys::copyFile(fefFile, FFaFilePath::getBaseName(currentFpp) + ".fef")) {
      exitValue = 1;
      FFaMsg::list("===> ERROR: Unable to copy fef-file back into RDB from " + this->tmpDir + "\n", true);
    }

    if (FpFileSys::isReadable(fef1File) &&
	!FpFileSys::copyFile(fef1File, FFaFilePath::getBaseName(currentFpp) + "_1.fef")) {
      exitValue = 1;
      FFaMsg::list("===> ERROR: Unable to copy fef-file back into RDB from " + this->tmpDir + "\n", true);
    }

    std::string frsFile = FFaFilePath::getBaseName(fefFile) + ".frs";

    if (FpFileSys::isReadable(frsFile) &&
	!FpFileSys::copyFile(frsFile, FFaFilePath::getBaseName(currentFpp) + ".frs")) {
      exitValue = 1;
      FFaMsg::list("===> ERROR: Unable to copy frs-file back into RDB from " + this->tmpDir + "\n", true);
    }
  }

  FFaMsg::list("===> Done\n");
  FFaMsg::popStatus();
  FFaMsg::setSubTask("");
  if (exitValue == 0)
    this->deleteTmpDir();

  // Kill me
  FapSolutionProcessManager::instance()->onSolverProcessDeath(this->getProcessSignature(), exitValue);
}


void FapFATFE::onHeaderCopyFinished(int)
{
  hcopyFinished = true;
}


FapFEFCOM::FapFEFCOM(FmPart* part, bool bottom)
{
  myPart = part;
  myGroupID = FapSolverID::FAP_DC_FEFCOM;

  mySolverName = this->npath + ADD_EXE("fefcom");

  IAmRunningBottom = bottom;
  FmDutyCycleOptions* opts = FmDB::getDutyCycleOptions();
  if (opts)
    myEvents = opts->getEvents();
};


bool FapFEFCOM::areAllFilesOk(std::vector<std::string>& noFmm,
			      std::vector<std::string>& noFpp,
			      std::vector<std::string>& noPart,
			      std::vector<std::string>& multiFpp)
{
  if (myEvents.empty())
    return false;

  const std::string& absPath = FmDB::getMechanismObject()->getAbsModelFilePath();
  for (const std::pair<std::string,EventData>& event : myEvents)
  {
    std::string fmm = event.first;
    FFaFilePath::makeItAbsolute(fmm,absPath);
    if (!FpFileSys::isReadable(fmm)) {
      noFmm.push_back(fmm);
      continue;
    }

    bool partExists = true;
    std::set<std::string> fppFiles = this->getFppFiles(fmm, partExists);

    if (fppFiles.empty()) {
      if (partExists)
	noFpp.push_back(fmm);
      else
	noPart.push_back(fmm);
    }

    if (fppFiles.size() > 1)
      multiFpp.push_back(fmm);

    for (const std::string& file : fppFiles)
      if (!FpFileSys::isReadable(file)) {
	noFpp.push_back(fmm);
	break;
      }
  }

  return (noFmm.empty() && noFpp.empty() && noPart.empty() && multiFpp.empty());
}


int FapFEFCOM::checkDependencies()
{
#ifdef FAP_DEBUG
  std::cout <<"FapFEFCOM::checkDependencies()"<< std::endl;
#endif

  if (!this->isExecutable(mySolverName))
    return FAP_NOT_EXECUTABLE;

  if (FapSolutionProcessManager::instance()->isGroupRunning(FapSolverID::FAP_DC_FPPHCOPY) ||
      FapSolutionProcessManager::instance()->isGroupRunning(FapSolverID::FAP_DC_FATFE))
  {
#if FAP_DEBUG > 1
    std::cout <<"fpphcopy or fatfe is running, please wait"<< std::endl;
#endif
    return FAP_PENDING_DEPENDENCIES_BUT_WAIT;
  }

  // Creating std::map of fef-files
  for (const std::pair<std::string,EventData>& event : myEvents)
  {
    bool dummy = true;
    std::set<std::string> fppFiles = this->getFppFiles(event.first, dummy);

    if (fppFiles.empty()) {
      // We will never get here, because this error
      // should have already been catched
      FFaMsg::list("===> ERROR: Cannot find any fpp-files\n", true);
      return FAP_NOT_EXECUTABLE;
    }

    const std::string& fppFile = *fppFiles.rbegin();
    std::string fefFile = FFaFilePath::getBaseName(fppFile);
    if (IAmRunningBottom) fefFile += "_1";
    fefFile += ".fef";

    myFefMap[fppFile] = fefFile;
    dataMap[fefFile] = (double)event.second.repeats;
  }

  // Now check if fef-files exist. If not, run a fatfe process on them

  int retVal = FAP_READY_TO_RUN;

  for (const std::pair<std::string,std::string>& fef : myFefMap)
  {
    const std::string& fppFile = fef.first;
    const std::string& fefFile = fef.second;

    // Cannot find fef-file, maybe start a fatfe proc
    if (!FpFileSys::isReadable(fefFile)) {

      // If we haven't already started a fatfe for this process, then start one
      if (startedFatFe.insert(fppFile).second) {
	FapFATFE* fatfe = new FapFATFE(fppFile, myPart);
	FapSolutionProcessManager::instance()->pushSolverProcess(fatfe);
	retVal = FAP_PENDING_DEPENDENCIES;
      }

      // We have already started the process
      else {

	// In fact we have started all the processes
	if (startedFatFe.size() == myFefMap.size()) {

	  // Is one of them still running?
	  if (FapSolutionProcessManager::instance()->isGroupRunning(FapSolverID::FAP_DC_FATFE) ||
	      FapSolutionProcessManager::instance()->isGroupRunning(FapSolverID::FAP_DC_FPPHCOPY)) {
	    retVal = FAP_PENDING_DEPENDENCIES;
	  }

	  // None of them are running, they may have failed.
	  else {
#ifdef FAP_DEBUG
	    std::cout <<"Started all fatfe , but none are running"<< std::endl;
#endif
	    FFaMsg::list("===> ERROR: Unable to find any results from FE-Fatigue\n", true);
	    return FAP_NOT_EXECUTABLE;
	  }
	}
#ifdef FAP_DEBUG
	else
	  std::cout <<"#### How the h*** did we get here???" << std::endl;
#endif
      }
    }
  }

  // Copying files if we are ready to run
  if (retVal == FAP_READY_TO_RUN) {
    FmResultStatusData* rsd = myWorkMech->getResultStatusData();
    std::string rdbPath = rsd->getCurrentTaskDirName(true);
    if (!FpFileSys::verifyDirectory(rdbPath)) {
      FFaMsg::list("===> Duty Cycle: Could not access directory\n\t" + rdbPath + "\n");
      return FAP_NOT_EXECUTABLE;
    }

    rsd = rsd->addSubTask("dutycycle_rcy");
    rdbPath = rsd->getCurrentTaskDirName(true);
    if (!FpFileSys::verifyDirectory(rdbPath)) {
      FFaMsg::list("===> Duty Cycle: Could not access directory\n\t" + rdbPath + "\n");
      return FAP_NOT_EXECUTABLE;
    }

    rsd = rsd->addSubTask(myPart->getTaskName());
    rdbPath = rsd->getCurrentTaskDirName(true);
    if (!FpFileSys::verifyDirectory(rdbPath)) {
      FFaMsg::list("===> Duty Cycle: Could not access directory\n\t" + rdbPath + "\n");
      return FAP_NOT_EXECUTABLE;
    }

    this->targetDir = rdbPath + FFaFilePath::getPathSeparator();

    if (!this->createTmpDir())
      return FAP_NOT_EXECUTABLE;

    int counter = 0;
    std::string dir, file;
    for (const std::pair<std::string,std::string>& fff : myFefMap)
    {
      dir = tmpDir + FFaNumStr("ft%d",counter++) + FFaFilePath::getPathSeparator();

      // create dir
      if (!FpFileSys::verifyDirectory(dir)) {
	FFaMsg::list("===> ERROR: Unable to create temporary directory [" + dir + "]\n", true);
	return FAP_NOT_EXECUTABLE;
      }

      file = dir + FFaLowerCaseString(FFaFilePath::getFileName(fff.second));

      // Copy fef-file
      if (!FpFileSys::copyFile(fff.second, file)) {
	FFaMsg::list("===> ERROR: Unable to copy file to temporary location\n", true);
	return FAP_NOT_EXECUTABLE;
      }
      tmpFiles[fff.second] = file;
    }

    // Write input file...
    this->inpFile = this->tmpDir + "fefcom.inp";
#ifdef FAP_DEBUG
    std::cout <<"input file: "<< this->inpFile << std::endl;
#endif
    std::ofstream os(inpFile.c_str());

    if (!os) {
      FFaMsg::list("===> ERROR: Failed to create input file for Duty Cycle\n", true);
      return FAP_NOT_EXECUTABLE;
    }

    resultFile = this->tmpDir + FFaLowerCaseString(FFaFilePath::getFileName(myFefMap.begin()->second));

    FmDutyCycleOptions* opts = FmDB::getDutyCycleOptions();
    if (!opts) {
      FFaMsg::list("===> ERROR running Duty Cycle: Internal data structure error\n", true);
      return FAP_NOT_EXECUTABLE;
    }

    // write output file info
    os << resultFile <<","<< opts->getEquivUnitScale() <<","<< opts->getEquivUnit() <<"\n";

    if (tmpFiles.size() != dataMap.size()) {
      FFaMsg::list("===> ERROR in Duty Cycle. You have encountered a bug.\n"
		   "     Please send an e-mail to developers@openfedem.org\n"
		   "     with a description of the problem.\n", true);
      return FAP_NOT_EXECUTABLE;
    }

    for (const std::pair<std::string,std::string>& fff : tmpFiles)
      os << fff.second <<","<< dataMap[fff.first] <<"\n";
  }

  return retVal;
}


std::set<std::string> FapFEFCOM::getFppFiles(const std::string& event, bool& partExists)
{
  partExists = true;

  FmModelFileQuery query(event);

  // Looking for part file
  std::vector<std::string> parts = query.getEntries("REDUCED_FTL_FILE");
  std::string descr = "\"" + myPart->reducedFTLFile.getValue() + "\"";

  if (std::find(parts.begin(),parts.end(),descr) == parts.end()) {
    partExists = false;
    return std::set<std::string>();
  }

  // Trying to find fpp-file
  std::string rsdString = query.getFirstEntry("RESULT_STATUS_DATA");

  FmResultStatusData rsd;
  rsd.setPath(FFaFilePath::getBaseName(event) + "_RDB");
  std::istringstream is(rsdString.c_str());
  rsd.read(is);

  std::set<std::string> files;
  FmResultStatusData* subRsd = rsd.getSubTask("summary_rcy");
  if (!subRsd->isEmpty()) {
    subRsd = subRsd->getSubTask(myPart->getTaskName());
    if (!subRsd->isEmpty())
      subRsd->getAllFileNames(files,"fpp");
  }
  return files;
}


int FapFEFCOM::execute()
{
#ifdef FAP_DEBUG
  std::cout <<"FapFEFCOM::execute()"<< std::endl;
#endif
  int depVal = this->checkDependencies();
  if (depVal != FAP_READY_TO_RUN)
    return depVal;

  FpProcessOptions opts;
#ifdef win32
  opts.name = mySolverName;
#else
  // For some very strange reason, fefcom can't be run with abs path on unix.
  // A report has been sent to nCode about the problem.
  // Until they fix it, we trust that the users have $npath/nsoft in their path.
  // SKE
  opts.name = FFaFilePath::getFileName(mySolverName);
#endif
  opts.args.push_back("-c");
  opts.args.push_back("2");
  opts.args.push_back("-o");
  opts.args.push_back(this->inpFile);
  opts.workingDir = tmpDir;
  opts.deathHandler = FFaDynCB1M(FapFEFCOM,this,onActualProcessDeath,int);
#ifdef FAP_DEBUG
  std::cout <<"Commamd line: "<< opts.name;
  for (const std::string& arg : opts.args) std::cout <<" "<< arg;
  std::cout <<"\nRunning in: "<< opts.workingDir << std::endl;
#endif

  FFaMsg::list("===> Duty Cycle: Adding damages from fef-files\n");
  if (this->run(opts,"fefcom") < 0)
    return FAP_NOT_EXECUTABLE;

  FFaMsg::pushStatus("Running nCode fefcom");
  FFaMsg::setSubTask(FFaFilePath::getFileName(myFefMap.begin()->second));

  return FAP_STARTED;
}


std::string FapFEFCOM::getProcessSignature() const
{
  if (myProcessSignature.empty()) {
    myProcessSignature = mySolverName + "@";
    for (const std::pair<std::string,EventData>& event : myEvents)
      myProcessSignature += event.first;
  }
#ifdef FAP_DEBUG
  std::cout <<"\nProcess sign: "<< myProcessSignature << std::endl;
#endif

  return myProcessSignature;
}


void FapFEFCOM::onActualProcessDeath(int exitValue)
{
  // UPDATE: nCode has released (at least to us) fefcom 5.3 A01
  // which fixes the problem
  // We trust that the users use this version of fefcom,
  // or else, you get an error return, even if fefcom is successful

#ifdef FAP_DEBUG
  std::cout <<"process death, exit val: "<< exitValue << std::endl;
#endif

  if (exitValue == 0) {
    std::string realResultFile = FFaFilePath::getBaseName(myPart->baseFTLFile.getValue());
    if (IAmRunningBottom)
      realResultFile = targetDir + realResultFile + "_1.fef";
    else
      realResultFile = targetDir + realResultFile + ".fef";

    // Copy file back into rdb structure
    if (!FpFileSys::copyFile(resultFile, realResultFile)) {
      exitValue = 1;
      FFaMsg::list("===> ERROR: Unable to copy result file back into RDB from " + this->tmpDir + "\n", true);
    }
    else {
      FFaMsg::list("===> Done\n");
      this->deleteTmpDir(true);
    }
  }
  else
    FFaMsg::list("===> ERROR: Duty Cycle failed!\n", true);

  FFaMsg::popStatus();
  FFaMsg::setSubTask("");

  // Kill me
  FapSolutionProcessManager::instance()->onSolverProcessDeath(this->getProcessSignature(), exitValue);
}


FapFEF2FRS::FapFEF2FRS(FmPart* part)
{
  myPart = part;
  myGroupID = FapSolverID::FAP_DC_FEF2FRS;

  mySolverName = this->npath + ADD_EXE("fef2frs");
}


int FapFEF2FRS::checkDependencies()
{
#ifdef FAP_DEBUG
  std::cout <<"FapFEF2FRS::checkDependencies()"<< std::endl;
#endif

  if (!this->isExecutable(mySolverName))
    return FAP_NOT_EXECUTABLE;

  if (FapSolutionProcessManager::instance()->isGroupRunning(FapSolverID::FAP_DC_FPPHCOPY) ||
      FapSolutionProcessManager::instance()->isGroupRunning(FapSolverID::FAP_DC_FATFE) ||
      FapSolutionProcessManager::instance()->isGroupRunning(FapSolverID::FAP_DC_FEFCOM))
  {
#if FAP_DEBUG > 1
    std::cout <<"fpphcopy, fatfe or fefcom is running, please wait"<< std::endl;
#endif
    return FAP_PENDING_DEPENDENCIES_BUT_WAIT;
  }

  // Figure out which files we are looking for
  FmResultStatusData* rsd = myWorkMech->getResultStatusData();
  rsd = rsd->addSubTask("dutycycle_rcy");
  if (!rsd->isEmpty())
  {
    rsd = rsd->addSubTask(myPart->getTaskName());
    std::string fefPath = rsd->getCurrentTaskDirName(true);
    std::string partName = FFaFilePath::getBaseName(myPart->baseFTLFile.getValue());
    fefFile  = fefPath + FFaFilePath::getPathSeparator() + partName + ".fef";
    fef1File = fefPath + FFaFilePath::getPathSeparator() + partName + "_1.fef";
    frsFile  = fefPath + FFaFilePath::getPathSeparator() + partName + ".frs";
  }

  // If we can read both files, we are ready to run
  bool fefFileIsReadable  = FpFileSys::isReadable(fefFile);
  bool fef1FileIsReadable = FpFileSys::isReadable(fef1File);
  if (fefFileIsReadable && fef1FileIsReadable)
  {
    if (!this->createTmpDir())
      return FAP_NOT_EXECUTABLE;

    tmpFef  = this->tmpDir + FFaLowerCaseString(FFaFilePath::getFileName(fefFile));
    tmpFef1 = this->tmpDir + FFaLowerCaseString(FFaFilePath::getFileName(fef1File));
    tmpFrs  = this->tmpDir + FFaLowerCaseString(FFaFilePath::getFileName(frsFile));

    // Copy files
    if (FpFileSys::copyFile(fefFile, tmpFef))
      if (FpFileSys::copyFile(fef1File, tmpFef1))
	return FAP_READY_TO_RUN;

    FFaMsg::list("===> ERROR: Unable to copy fef-files to temporary location\n", true);
    return FAP_NOT_EXECUTABLE;
  }

  // If we can't read first fef-file, then create it
  if (!fefFileIsReadable)
  {
    FapFEFCOM* fefcom = new FapFEFCOM(myPart);
    std::vector<std::string> missingFmm, missingPart, missingFpp, multiFpp;
    if (fefcom->areAllFilesOk(missingFmm, missingFpp, missingPart, multiFpp))
      FapSolutionProcessManager::instance()->pushSolverProcess(fefcom);

    // Some events are not ok, then we can't run
    else if (missingFmm.size() || missingPart.size() || missingFpp.size())
    {
      std::string msg = "===> ERROR running Duty Cycle on "
	+ myPart->getIdString() + " "
	+ FFaFilePath::getBaseName(myPart->baseFTLFile.getValue());
      if (missingFmm.size()) {
	msg += "\n  -> Could not find the following files:";
	for (const std::string& missing : missingFmm)
	  msg += "\n     " + missing;
      }
      if (missingPart.size()) {
	msg += "\n  -> This part is not a part of the following models:";
	for (const std::string& missing : missingPart)
	  msg += "\n     " + missing;
      }
      if (missingFpp.size()) {
	msg += "\n  -> You must run Strain Coat Recovery on this part for the following models:";
	for (const std::string& missing : missingFpp)
	  msg += "\n     " + missing;
      }
      msg += "\n===> Duty Cycle failed\n";
      FFaMsg::list(msg, true);
      delete fefcom;
      return FAP_NOT_EXECUTABLE;
    }

    // Multiple fpp-files found in event, need user confirmation
    else if (multiFpp.size())
    {
      std::string msg = "The following events contain more than one fpp-file:\n";
      for (const std::string& file : multiFpp)
	msg += "   " + file + "\n";
      msg += "This indicates that a Strain Coat Recovery has been run "
	"incrementally.\nContinuing with Duty Cycle calculation may fail.\n"
	"NOTE: Only the last fpp-file will be taken into account during the "
	"calculation.\n\nDo you want to continue?";
      const char* btnTexts[3] = {"Continue","Abort",0};
      if (FFaMsg::dialog(msg,FFaMsg::GENERIC,btnTexts) == 0)
	FapSolutionProcessManager::instance()->pushSolverProcess(fefcom);
      else
      {
	FFaMsg::list("===> Duty Cycle aborted\n");
	delete fefcom;
	return FAP_NOT_EXECUTABLE;
      }
    }

    else // No events defined
    {
      FFaMsg::list("===> Duty Cycle aborted. No events have been defined\n");
      delete fefcom;
      return FAP_NOT_EXECUTABLE;
    }
  }

  // If we can't read second fef-file, then create it
  if (!fef1FileIsReadable) {
    FapFEFCOM* fef1com = new FapFEFCOM(myPart, true);
    FapSolutionProcessManager::instance()->pushSolverProcess(fef1com);
  }

  return FAP_PENDING_DEPENDENCIES;
}


int FapFEF2FRS::execute()
{
  int depVal = this->checkDependencies();
  if (depVal != FAP_READY_TO_RUN)
    return depVal;

  FpProcessOptions opts;
  opts.name = mySolverName;
  opts.args.push_back("/tinp=" + FFaFilePath::getFileName(tmpFef));
  opts.args.push_back("/binp=" + FFaFilePath::getFileName(tmpFef1));
  opts.args.push_back("/out=" + FFaFilePath::getFileName(tmpFrs));
  opts.args.push_back("/ent=element");
  opts.args.push_back("/step=1");
  opts.args.push_back("/time=1.0");
  opts.args.push_back("/lid=" + FFaNumStr(myPart->getBaseID()));
  opts.args.push_back("/uid=" + FFaNumStr(myPart->getID()));
#ifdef FAP_DEBUG
  std::cout <<"fef2frs args:";
  for (const std::string& arg : opts.args) std::cout <<" "<< arg;
  std::cout << std::endl;
#endif
  opts.workingDir = this->tmpDir;
  opts.deathHandler = FFaDynCB1M(FapFEF2FRS,this,onActualProcessDeath,int);

  FFaMsg::list("===> Duty Cycle: Creating " +
	       FFaFilePath::getFileName(frsFile) + "\n");
  if (this->run(opts,"fef2frs") < 0)
    return FAP_NOT_EXECUTABLE;

  FFaMsg::pushStatus("Running nCode fef2frs");
  FFaMsg::setSubTask(FFaFilePath::getFileName(frsFile));

  return FAP_STARTED;
}


std::string FapFEF2FRS::getProcessSignature() const
{
  if (myProcessSignature.empty())
    myProcessSignature = mySolverName + "@" + this->tmpDir;

#ifdef FAP_DEBUG
  std::cout <<"\nProcess sig: "<< myProcessSignature << std::endl;
#endif
  return myProcessSignature;
}


void FapFEF2FRS::onActualProcessDeath(int exitValue)
{
#ifdef FAP_DEBUG
  std::cout <<"FapFEF2FRS::onActualProcessDeath, exit val" << exitValue << std::endl;
#endif

  if (exitValue != 0) // Need to delete tmp files anyhow
    FFaMsg::list("===> ERROR: Unable to create frs-file from fef-file\n", true);
  else {
    // Copy frs-file back into where it belongs...
    if (!FpFileSys::copyFile(tmpFrs, frsFile)) {
      exitValue = 1;
      FFaMsg::list("===> ERROR: Unable to copy frs-file into RDB from " + this->tmpDir + "\n", true);
    }
    FFaMsg::list("===> Duty Cycle done.\n\n");
  }

  FFaMsg::popStatus();
  FFaMsg::setSubTask("");

  if (exitValue == 0)
    this->deleteTmpDir();

  // Please kill me...
  FapSolutionProcessManager::instance()->onSolverProcessDeath(this->getProcessSignature(), exitValue);
}
