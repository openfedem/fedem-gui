// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/FapLicenseManager.H"

#include "vpmPM/FpModelRDBHandler.H"
#include "vpmPM/FpProcessOptions.H"
#include "vpmDB/FmSolverInput.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmFileSys.H"
#include "vpmDB/FmResultStatusData.H"
#include "vpmDB/FmAnalysis.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmPart.H"

#include "FFaLib/FFaCmdLineArg/FFaOptionFileCreator.H"
#include "FFaLib/FFaAlgebra/FFaCheckSum.H"
#include "FFaLib/FFaDefinitions/FFaAppInfo.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "FFaLib/FFaOS/FFaFilePath.H"

#include "vpmApp/vpmAppProcess/FapSolutionProcessMgr.H"
#include "vpmApp/vpmAppProcess/FapSolverID.H"
#include "vpmApp/vpmAppProcess/FapLinkReducer.H"


/*! fedem_reducer options:
  -Bmatfile               Name of B-matrix file
  -Bmatprecision          Storage precision of the B-matrix on disk
                          = 1: Single precision
                          = 2: Double precision
  -Bramsize               In-core size (MB) of displacement recovery matrix
                          <= 0: Store full matrix in core
  -autoMassScale          Scale factor for auto-added masses
  -autoStiffMethod        Method for automatic stiffness computations
                          = 1: Use k = Min(diag(K)) * 0.1/<tolFactorize>
                          = 2: Use k = Mean(diag(K)) * <autoStiffScale>
                          = 3: Use k = Max(diag(K)) * <autoStiffScale>
  -autoStiffScale         Scale factor for auto-added springs
  -bufsize_rigid          Buffer size (in DP-words) per rigid element
                          <= 0: Use conservative estimate computed internally
  -cachesize              Cache size (KB) to be used by the SPR solver
  -consolemsg             Output error messages to console
  -cwd                    Change working directory
  -datacheck              Do data check only (exiting after data input)
  -debug                  Debug print switch
  -denseSolver            Use LAPACK dense matrix equation solver
  -dispfile               Name of internal displacement file
  -eigenshift             Shift value for eigenvalue analysis
                          (target frequency for generalized DOFs)
  -eigfile                Name of eigenvector file
  -extNodes               List of external nodes
                          (in addition to those specified in the part file)
  -factorMass             Factorize mass matrix in the eigenvalue solver
  -fao                    Read additional options from this file
  -fco                    Read calculation options from this file
  -fop                    Read output options from this file
  -frsfile                Name of results database file for mode shapes
  -ftlout                 Name of part output file in FTL-format
  -gravfile               Name of gravity force vector file
  -gsfSolver              Use the GSF equation solver
                          = 0: Use SPR for both stiffness and mass matrix
                          = 1: Use GSF for stiffness matrix, and SPR for mass
                          = 2: Use GSF for both stiffness and mass matrix
  -help                   Print out this help text
  -licenseinfo            Print out license information at startup
  -licensepath            License file directory
  -linkId                 Part base-ID number
  -linkfile               Name of part input file (must be specified)
  -loadfile               Name of load vector file
  -lumpedmass             Use lumped element mass matrices
  -massfile               Name of mass matrix file
  -neval                  Number of eigenvalues/eigenvectors to compute
  -nevred                 Number of eigenvalues to compute for reduced system
  -ngen                   Number of generalized modes
  -nograv                 Skip gravity force and mass matrix calculation
  -nomass                 Skip mass matrix reduction
  -rdbinc                 Increment number for the results database files
  -resfile                Name of result output file
  -samfile                Name of SAM data file
  -singularityHandler     Option on how to treat singular matrices
                          = 0: Abort on all occurring singularities
                          = 1: Suppress true zero pivots,
                               abort on reduced-to-zero pivots
                          > 1: Suppress all occurring singularities of any kind
  -skylineSolver          Use the skyline equation solver
  -stiffile               Name of stiffness matrix file
  -terminal               File unit number for terminal output
  -tolEigval              Max acceptable relative error in eigenvalues
  -tolFactorize           Equation solver singularity criterion
                          (smaller values are less restrictive)
                          The lowest value allowed is 1e-20
  -tolWAVGM               Geometric tolerance for WAVGM elements
  -version                Print out program version
*/


FapLinkReducer::FapLinkReducer(FmPart* redPart, bool chkRecovery, bool preBatch)
{
  mySolverName = "fedem_reducer";
  myWorkPart = redPart;
  amICheckingRecovery = chkRecovery;
  amIPreparingForBatch = preBatch;
  myGroupID = FapSolverID::FAP_REDUCER;
}


int FapLinkReducer::checkDependency(bool silence) const
{
  if (myWorkPart->useGenericProperties.getValue())
    return FAP_RESULTS_OK;
  else if (myWorkPart->suppressInSolver.getValue())
    return FAP_RESULTS_OK;

#ifdef FAP_DEBUG
  std::cout <<"FapLinkReducer::checkDependencies()"<< std::endl;
#endif

  // Check if we have references to external part matrix files
  if (myWorkPart->externalSource.getValue())
  {
    if (amICheckingRecovery)
    {
      ListUI <<" --> External FE matrix files cannot be used for recovery\n";
      return FAP_NOT_EXECUTABLE;
    }

    // Convert the Nastran OP2-files (if any) to fedem FMX-files
    if (!myWorkPart->convertOP2files(myWorkPart->getAbsFilePath(true)))
    {
      ListUI <<" --> Cannot use externally reduced FE matrix files\n";
      return FAP_NOT_EXECUTABLE;
    }

    // Lambda function checking if an externally reduced file exists on disk.
    auto&& existFile = [this](const std::string& file)
    {
      if (file.empty()) return false;

      std::string fullName(file);
      FFaFilePath::makeItAbsolute(fullName,myWorkMech->getAbsModelFilePath());
      return FmFileSys::isReadable(fullName);
    };

    if (existFile(myWorkPart->SMatFile.getValue()) &&
        existFile(myWorkPart->MMatFile.getValue()) &&
        existFile(myWorkPart->GMatFile.getValue()))
      return FAP_RESULTS_OK;

    ListUI <<" --> Missing external FE matrix files\n";
    return FAP_NOT_EXECUTABLE;
  }

  // Check if usage of this part has been blocked for various reasons
  if (myWorkPart->lockLevel.getValue() == FmPart::FM_DENY_LINK_USAGE)
  {
    ListUI <<" --> No FE data for "<< myWorkPart->getIdString(true) <<"\n";
    return FAP_NOT_EXECUTABLE;
  }

  // Verify that we have a valid (and unique) FTL file name for this part
  const std::string& ftlFileName = myWorkPart->setValidBaseFTLFile();
  if (ftlFileName.empty())
  {
    ListUI <<" --> No FE data file for "<< myWorkPart->getIdString(true) <<"\n";
    return FAP_NOT_EXECUTABLE;
  }

  bool needMassMatrix = false;
  if (!amICheckingRecovery)
    needMassMatrix = FmDB::getActiveAnalysis()->needMassMatrix();

  int ngen = myWorkPart->nGenModes.getValue();


  // Build the part checksum - used for verification of the found files.
  // Be careful to add to the checksum in the same order as in the reducer.
  //////////////////////////////////////////////////////////////////////////////

  FFaCheckSum cs;
  myWorkPart->getCheckSum(cs);
  myWantedCS = cs.getCurrent();
  if (myWantedCS) // may be zero when loading an old model file without FE-data
  {
#ifdef FAP_DEBUG
    std::cout <<" --> Part checksum for "<< ftlFileName <<" = "<< myWantedCS;
#endif

    // Modify the checksum argument from nEigvalsCalc
    // to reflect what is being done in the reducer
    int neval = 0;
    if (ngen > 0)
    {
      neval = myWorkPart->nEigvalsCalc.getValue();
      if (ngen > neval) neval = ngen;
    }

    cs.add(ngen > 0 ? ngen : 0);
    cs.add(neval);
    cs.add(2); // always count the sparse solver (this was updated for R3.1)
    cs.add(myWorkPart->tolEigenval.getValue());
    cs.add(myWorkPart->tolFactorize.getValue());
    if (!myWorkPart->useConsistentMassMatrix.getValue()) cs.add(2);
    if (!myWorkPart->factorizeMassMxEigSol.getValue() && ngen > 0) cs.add(1);

    myWantedCS = cs.getCurrent();
#ifdef FAP_DEBUG
    std::cout <<", with reducer options = "<< myWantedCS << std::endl;
#endif
  }


  // Check for files in the current Results Status Data (RSD) object of the part
  // using the part checksum to verify whether the found files are OK or not.
  // The needed files for the dynamics solver are the S-, M- and G-matrix files.
  // For recovery processes the B-matrix and SAM data files are needed.
  //////////////////////////////////////////////////////////////////////////////

  int stat = Fedem::checkReducerFiles(myWorkPart, needMassMatrix,
                                      amICheckingRecovery ? 'R' : 'S',
                                      amIPreparingForBatch, myWantedCS);
  if (stat == 2) // All files found, but no checksum available
  {
    WEmessage(1,"     The needed matrix files were found for this part, but\n"
              "     since the FE-data is not loaded it can not be checked\n"
              "     whether they are consistent with the current part or not.\n"
              "     It is recommened to load the FE-data of this part\n"
              "     at this point to verify the matrix file validity.\n");
    return FAP_RESULTS_OK;
  }

  else if (stat == 3) // All files found, with acceptable checksum mismatch
  {
    WEmessage(1,"     The needed matrix files were found for this part.\n"
              "     However, they were produced by an older version for which\n"
              "     it could not be determined whether or not they are"
              " consistent with the current part.\n"
              "     It is recommended to force a new reduction of this part to"
              " resolve this uncertainty.\n");
    return FAP_RESULTS_OK;
  }

  else if (stat > 0) // All files found with correct checksum
    return FAP_RESULTS_OK;

  else if (stat < 0) // User overrides the checksum, but some files are missing
  {
    WEmessage(2,"     The \"Ignore checksum test\" option is selected for this part, but the\n"
              "     needed matrix files could not be found in the FE part repository.\n");
    return FAP_NOT_EXECUTABLE;
  }


  // Not ready to run yet.
  // Check if we have valid solutions elsewhere in suitable directories on disk.
  //////////////////////////////////////////////////////////////////////////////

  // Just skip the following if we don't have a checksum available for this part
  if (!myWantedCS) return FAP_READY_TO_RUN;

  std::vector<std::string> modelDir;
  std::string baseName = FFaFilePath::getBaseName(ftlFileName,true);
  std::string partPath = myWorkPart->getAbsFilePath();
  std::string nameFilter(baseName+"_*");
  FmFileSys::getDirs(modelDir,partPath,nameFilter.c_str());
  const std::string& modelPath = myWorkMech->getAbsModelFilePath();

  for (const std::string& dir : modelDir)
  {
    // We have candidates for reduced part matrix files

    std::string rdbPath = FFaFilePath::appendFileNameToPath(partPath,dir);
    if (!silence)
      ListUI <<" --> Checking for reduced files in "
	     << FFaFilePath::getRelativeFilename(modelPath,rdbPath) <<"\n";

    rdbPath += FFaFilePath::getPathSeparator();
    bool valid = true, batch = amIPreparingForBatch;
    if (!batch)
      batch = Fedem::validFileCheck(rdbPath + baseName + ".chk", myWantedCS);

    // Lambda function checking the presense and validity of a file.
    auto&& checkOtherFile = [this,&valid,batch,rdbPath](std::string& file,
                                                        bool required = true)
    {
      bool isValid = Fedem::validFileCheck(rdbPath + file, myWantedCS);
      if (isValid)
        return; // The file exists and is valid for this part
      else if (!required)
        file.clear(); // The file is absent, but not required ==> OK
      else if (!batch)
        valid = false; // When preparing for batch, assume the file appears later
    };

    // Compose search names for the new files
    std::string tmpBMatFile = baseName + "_B.fmx";
    std::string tmpEMatFile = baseName + "_E.fmx";
    std::string tmpDMatFile = baseName + "_D.fmx";
    std::string tmpGMatFile = baseName + "_G.fmx";
    std::string tmpLMatFile = baseName + "_L.fmx";
    std::string tmpMMatFile = baseName + "_M.fmx";
    std::string tmpSMatFile = baseName + "_S.fmx";
    std::string tmpFMatFile = baseName + "_F.fmx";
    std::string tmpSAMdataFile = baseName + "_SAM.fsm";

    // Check files needed for dynamics/quasi-static simulation
    checkOtherFile(tmpSMatFile,!amICheckingRecovery);
    checkOtherFile(tmpMMatFile,!amICheckingRecovery && needMassMatrix);
    checkOtherFile(tmpGMatFile,!amICheckingRecovery && FmDB::getGrav().length() > 1.0e-8);
    checkOtherFile(tmpLMatFile,!amICheckingRecovery && myWorkPart->hasLoads());
    if (myWorkPart->useNonlinearReduction.getValue())
    {
      checkOtherFile(tmpFMatFile,!amICheckingRecovery);
      checkOtherFile(tmpDMatFile,!amICheckingRecovery);
    }
    else
    {
      tmpFMatFile.clear();
      if (ngen >= 0)
        tmpDMatFile.clear();
    }

    // Check files needed for stress and/or modes recovery
    checkOtherFile(tmpBMatFile,amICheckingRecovery);
    checkOtherFile(tmpSAMdataFile,amICheckingRecovery);
    if (ngen > 0)
      checkOtherFile(tmpEMatFile,amICheckingRecovery);
    else
    {
      tmpEMatFile.clear();
      if (ngen < 0 && !myWorkPart->useNonlinearReduction.getValue())
        checkOtherFile(tmpDMatFile,amICheckingRecovery);
    }

    if (valid)
    {
      myWorkPart->reducedFTLFile = baseName + ".ftl";
      myWorkPart->BMatFile = tmpBMatFile;
      myWorkPart->EMatFile = tmpEMatFile;
      myWorkPart->DMatFile = tmpDMatFile;
      myWorkPart->FMatFile = tmpFMatFile;
      myWorkPart->GMatFile = tmpGMatFile;
      myWorkPart->LMatFile = tmpLMatFile;
      myWorkPart->MMatFile = tmpMMatFile;
      myWorkPart->SMatFile = tmpSMatFile;
      myWorkPart->SAMdataFile = tmpSAMdataFile;
      FpModelRDBHandler::RDBSync(myWorkPart,myWorkMech,false,rdbPath);
      if (!silence)
	ListUI <<" --> Found valid reduced FE data for "
	       << myWorkPart->getIdString(true) <<"\n";
      return FAP_RESULTS_OK;
    }
  }

#ifdef FAP_DEBUG
  std::cout <<" --> Matrix files non-existent or with wrong checksum."<< std::endl;
#endif
  return FAP_READY_TO_RUN;
}


int FapLinkReducer::createInput(std::string& rdbPath)
{
#ifdef FAP_DEBUG
  std::cout <<"FapLinkReducer::createInput()"<< std::endl;
#endif

  // Check the dependencies for this part
  int depVal = this->checkDependencies();
  if (depVal != FAP_READY_TO_RUN)
    return depVal;

  // Create directory structure with reducer input files
  rdbPath = Fedem::createReducerInput(FmDB::getActiveAnalysis(),
                                      myWorkMech,myWorkPart,mySolverName,
                                      amIPreparingForBatch,myWantedCS);
  if (rdbPath.find("===> Could not access FE data file") == 0)
    depVal = FAP_NOT_EXECUTABLE;
  else if (rdbPath.find("===> ") == 0)
    return FAP_NOT_EXECUTABLE;

  FpModelRDBHandler::RDBSync(myWorkPart,myWorkMech,false,rdbPath);
  return depVal;
}


int FapLinkReducer::execute()
{
#ifdef FAP_DEBUG
  std::cout <<"\nFapLinkReducer::execute()"<< std::endl;
#endif

  FFaMsg::pushStatus("Creating Reduction input");
  FFaMsg::setSubTask(myWorkPart->baseFTLFile.getValue());
  std::string rdbPath;
  int depVal = this->createInput(rdbPath);
  FFaMsg::popStatus();
  FFaMsg::setSubTask("");

  if (depVal != FAP_READY_TO_RUN)
    return depVal;
  else if (amIPreparingForBatch)
  {
    ListUI <<"===> Input files for batch execution of "
	   << mySolverName <<" created in:\n     "<< rdbPath <<"\n";
    FapSolutionProcessManager::instance()->afterBatchPreparation(myGroupID);
    return depVal;
  }

  // Create argument string:
  FFaOptionFileCreator processOpts;
  if (FapSolverBase::doRemoteSolve())
    processOpts.add("-cwd", this->findRemotePath(rdbPath));

  processOpts.add("-fco", mySolverName + ".fco");
  processOpts.add("-fop", mySolverName + ".fop");
  std::string addOptions = FmDB::getActiveAnalysis()->reducerAddOpts.getValue();
  if (!addOptions.empty())
    processOpts.add("-fao", mySolverName + ".fao");

  // Enable console output of error messages
  processOpts.add("-consolemsg", true);

  // Retain terminal output only when running batch on one processor
  if (!FFaAppInfo::isConsole())
    processOpts.add("-terminal",7); // Write terminal output to a file in $PWD
  else if (FmDB::getActiveAnalysis()->maxConcurrentProcesses.getValue() > 1)
    if (addOptions.find("-terminal") == std::string::npos)
      processOpts.add("-terminal",-1); // Redirect terminal output to /dev/null

  // Set up the actual process options
  FpProcessOptions options;
  options.name = mySolverName;
  options.args = processOpts.getOptVector();
  if (FapSolverBase::doRemoteSolve())
    options.prefix = FmDB::getActiveAnalysis()->processPrefix.getValue();
  else
    options.workingDir = rdbPath;
  options.deathHandler = FFaDynCB1M(FapLinkReducer, this, onActualProcessDeath, int);

  const std::string& modelPath = myWorkMech->getAbsModelFilePath();
  ListUI <<"\n===> Reducing: "<< myWorkPart->getIdString(true) <<" ...\n  -> Results in "
	 << FFaFilePath::getRelativeFilename(modelPath,rdbPath) <<"\n";

  if (this->run(options) < 0)
    return FAP_NOT_EXECUTABLE;

  FFaMsg::pushStatus("Reducing FE part");
  FFaMsg::setSubTask(myWorkPart->baseFTLFile.getValue());

  myWorkPart->lockLevel.setValue(FmPart::FM_DENY_ALL_LINK_MOD);
  myWorkPart->sendSignal(FmModelMemberBase::MODEL_MEMBER_CHANGED);

  return FAP_STARTED;
}


std::string FapLinkReducer::getProcessSignature() const
{
  if (myProcessSignature.empty())
  {
    myProcessSignature = mySolverName + "@" + myWorkPart->getAbsFilePath();
    std::string task = myWorkPart->myRSD.getValue().getCurrentTaskDirName(false,true);
    if (task.empty())
    {
      // Create a unique signature in case the task has not been named yet
      static std::string ver("a_");
      task = ver + myWorkPart->baseFTLFile.getValue();
      if (ver[0] == 'z') ver[0] = 'a';
      else ++ver[0];
    }
    FFaFilePath::appendToPath(myProcessSignature,task);
#if FAP_DEBUG > 1
    std::cout <<"Process sign: "<< myProcessSignature << std::endl;
#endif
  }
  return myProcessSignature;
}


void FapLinkReducer::syncRDB()
{
  FpModelRDBHandler::RDBSync(myWorkPart,myWorkMech,true);
}


void FapLinkReducer::onActualProcessDeath(int exitValue)
{
  myWorkPart->lockLevel.setValue(FmPart::FM_ALLOW_MODIFICATIONS);
  myWorkPart->sendSignal(FmModelMemberBase::MODEL_MEMBER_CHANGED);

  FpModelRDBHandler::RDBSync(myWorkPart,myWorkMech);

  bool valid = true;
  std::string missFile;
  std::string taskDir = myWorkPart->myRSD.getValue().getCurrentTaskDirName(false,true);
  std::string rdbPath = FFaFilePath::appendFileNameToPath(myWorkPart->getAbsFilePath(),taskDir);
  std::string baseName = FFaFilePath::getBaseName(myWorkPart->reducedFTLFile.getValue());
  rdbPath += FFaFilePath::getPathSeparator();

  // Lambda function checking the presense and validity of a file.
  auto&& checkFile = [this,&valid,rdbPath,&missFile](FFaField<std::string>& field,
                                                     const std::string& file,
                                                     bool required = true)
  {
    bool isValid;
    if (required)
      valid &= isValid = Fedem::validFileCheck(rdbPath+file,myWantedCS,&missFile);
    else
      isValid = Fedem::validFileCheck(rdbPath+file,myWantedCS);

    field.setValue(isValid ? file : std::string());
  };

  if (exitValue == 0 && !myWorkPart->overrideChecksum.getValue())
  {
    // Check that the matrix checksums are as expected
    checkFile(myWorkPart->SMatFile, baseName + "_S.fmx");
    checkFile(myWorkPart->MMatFile, baseName + "_M.fmx",
              FmDB::getActiveAnalysis()->needMassMatrix());
    checkFile(myWorkPart->GMatFile, baseName + "_G.fmx",
              FmDB::getGrav().length() > 1.0e-8);
    checkFile(myWorkPart->LMatFile, baseName + "_L.fmx",
              myWorkPart->hasLoads());
    checkFile(myWorkPart->SAMdataFile, baseName + "_SAM.fsm");
    checkFile(myWorkPart->BMatFile, baseName + "_B.fmx");
    checkFile(myWorkPart->EMatFile, baseName + "_E.fmx",
              myWorkPart->nGenModes.getValue() > 0);
    if (myWorkPart->useNonlinearReduction.getValue())
    {
      checkFile(myWorkPart->DMatFile, baseName + "_D.fmx");
      checkFile(myWorkPart->FMatFile, baseName + "_F.fmx");
    }
    else if (myWorkPart->nGenModes.getValue() < 0)
      checkFile(myWorkPart->DMatFile, baseName + "_D.fmx");
  }
  if (!valid)
  {
    exitValue = -1;
    if (missFile.empty())
    {
      WEmessage(2,"     The checksum for the reduced matrices are incorrect.\n"
		"     This may happen when you, e.g., import a Nastran bulk data file\n"
		"     and reduce it in the same session without saving the model first.\n"
		"     It may help to save the model at this point and then reopen it.\n"
		"     Contact the OpenFEDEM team (developers@openfedem.org) for assistance\n"
		"     if the problem persists.\n");
      myWorkPart->forceSave(); // Ensure that the part file is rewritten on next save
    }
    else
      WEmessage(2,"     A re-reduction is needed. The following matrix files are missing:\n"
		"     "+ missFile +"\n");
  }
  else if (myWorkPart->useNonlinearReduction.getValue())
  {
    // Get the actual number of states from the non-linear reduction
    int numOfNonLinearStates = myWorkPart->numberOfNonlinearSolutions.getValue();
    std::string numStateFile = rdbPath + baseName + "_numStates.txt";
    FILE* fp = fopen(numStateFile.c_str(),"r");
    if (fp)
    {
      if (fscanf(fp,"%d",&numOfNonLinearStates))
        ListUI <<"  => Number of reduced nonlinear states: "<< numOfNonLinearStates <<"\n";
      fclose(fp);
    }
    myWorkPart->nonLinStates.setValue(numOfNonLinearStates);
  }

  ListUI <<"===> "<< mySolverName;
  if (IWasKilled)
    ListUI <<" terminated by user";
  else if (exitValue)
    ListUI <<" failed";
  else
    ListUI <<" done";

  ListUI <<" for "<< myWorkPart->getIdString(true) <<"\n";

  if (exitValue && valid && !IWasKilled)
  {
    std::string resFile = rdbPath + mySolverName + ".res";
    if (FmFileSys::isReadable(resFile))
      ListUI <<"     See "<< resFile <<" for further details.\n";
  }

  FFaMsg::list("\n",exitValue);
  FFaMsg::popStatus();
  FFaMsg::setSubTask("");

  std::string sign = this->getProcessSignature();
  FapSolutionProcessManager::instance()->onSolverProcessDeath(sign,exitValue);
}


/*!
  Convenience method to output an error or warning message to the Output list.
  It also pops up the Output list UI to grab the user's attention.
*/

void FapLinkReducer::WEmessage(int severity, const std::string& theMessage) const
{
  if (severity == 1)
    ListUI <<"===> WARNING: ";
  else
    ListUI <<"===> ERROR: ";

  ListUI << myWorkPart->getIdString(true) <<"\n";
  FFaMsg::list(theMessage,true);
}


/*!
  Static method to check whether \a part has valid reduced data on disk or not.
  The part RSD is updated, if needed.
*/

bool FapLinkReducer::isReduced(FmPart* part, bool silence)
{
  if (part->useGenericProperties.getValue()) return false;
  if (part->suppressInSolver.getValue()) return false;

  FapLinkReducer reducer(part);
  return reducer.checkDependency(silence) == FAP_RESULTS_OK ? true : false;
}
