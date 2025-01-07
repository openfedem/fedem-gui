// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <fstream>

#include "vpmApp/vpmAppProcess/FapStressExpander.H"
#include "vpmApp/vpmAppProcess/FapSolverID.H"
#include "vpmApp/vpmAppDisplay/FapVTFFile.H"
#include "vpmApp/FapLicenseManager.H"

#include "vpmDB/FmPart.H"
#include "vpmDB/FmElementGroupProxy.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmAnalysis.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmFileSys.H"

#include "FFaLib/FFaCmdLineArg/FFaOptionFileCreator.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"


/*! fedem_stress options:
  -Bmatfile      Name of B-matrix file
  -Bramsize      In-core size (MB) of displacement recovery matrix
                 < 0: Use the same as in the reducer (default)
                 = 0: Store full matrix in core
  -SR            Save stress resultants to results database file
  -VTFavgelm     Write averaged element results to VTF-file
  -VTFdscale     Deformation scaling factor for VTF output
  -VTFfile       Name of VTF output file
  -VTFinit       Write initial state to VTF-file
  -VTFoffset     VTF result block id offset
  -VTFparts      Number of parts in VTF-file
  -consolemsg    Output error messages to console
  -cwd           Change working directory
  -debug         Debug print switch
  -deformation   Save deformation to results database file
  -dispfile      Name of internal displacement file
  -double        Save results in double precision
  -dumpDefNas    Save deformations to Nastran bulk data files
  -eigfile       Name of eigenvector file
  -fao           Read additional options from this file
  -fco           Read calculation options from this file
  -fop           Read output options from this file
  -frsfile       Name of solver results database file
  -fsifile       Name of solver input file
  -group         List of element groups to do calculation for
  -help          Print out this help text
  -licenseinfo   Print out license information at startup
  -licensepath   License file directory
  -linkId        Part base-ID number
  -linkfile      Name of part input file
  -maxPStrain    Save max principal strain to results database
  -maxPStress    Save max principal stress to results database
  -maxSStrain    Save max shear strain to results database
  -maxSStress    Save max shear stress to results database
  -minPStrain    Save min principal strain to results database
  -minPStress    Save min principal stress to results database
  -nodalForces   Compute nodal forces
  -rdbfile       Name of stress results database file
  -rdbinc        Increment number for the results database file
  -resStressFile Name of residual stress input file
  -resStressSet  Name of residual stress set
  -resfile       Name of result output file
  -samfile       Name of SAM data file
  -statm         Start time
  -stotm         Stop time
  -strain        Save strain tensors to results database
  -stress        Save stresse tensors to results database
  -terminal      File unit number for terminal output
  -tinc          Time increment
  -version       Print out program version
  -vmStrain      Save von Mises strain to results database
  -vmStress      Save von Mises stress to results database
  -write_nodes   Save deformations as nodal data
  -write_vector  Save deformations as vector data
*/


bool   FapStressExpander::initStateOnVTF = false;
size_t FapStressExpander::startVTFblock  = 0;
size_t FapStressExpander::nPartsVTF      = 0;


FapStressExpander::FapStressExpander(FmPart* aPart, FmSimulationEvent* event,
                                     bool preBatch, bool ImSolvingLastPart)
  : FapRecoveryBase(aPart,event)
{
  mySolverName = "fedem_stress";
  mySubTask = "timehist_rcy";
  myGroupID = FapSolverID::FAP_STRESS;
  amIPreparingForBatch = preBatch;
  iAmLast = ImSolvingLastPart;
}


FapStressExpander::FapStressExpander(FmPart* aPart,
                                     const std::vector<FmElementGroupProxy*>& groups,
                                     bool ImSolvingLastPart)
  : FapRecoveryBase(aPart)
{
  mySelectedGroups = groups;
  mySolverName = "fedem_stress";
  mySubTask = "timehist_rcy";
  myGroupID = FapSolverID::FAP_STRESS;
  iAmLast = ImSolvingLastPart;
}


bool FapStressExpander::getDirectSolveResults(std::string& samFile,
                                              std::string& frsFile) const
{
  samFile.clear();
  frsFile.clear();

  // Check if we have a direct solution for this part
  FmResultStatusData* recRSD = this->getTopLevelRSD()->getSubTask(mySubTask);
  if (!recRSD) return false;
  FmResultStatusData* lnkRSD = recRSD->getSubTask(myWorkPart->getTaskName());
  if (!lnkRSD) return false;

  // Check availability of SAM-file
  std::set<std::string> files;
  if (!lnkRSD->getAllFileNames(files,"fsm",true,false) || files.size() != 1)
    return false; // Should only be one SAM-file
  else if (!FmFileSys::isReadable(samFile = *files.begin()))
    return false; // The SAM-file is not present on disk

  // Check availability of frs-file with displacement results
  files.clear();
  if (lnkRSD->getAllFileNames(files,"frs",true,false))
    for (const std::string& fileName : files)
    {
      // Find the frs-file generated by the fedem_reducer module, if any
      char cline[256];
      std::ifstream is(fileName);
      for (int line = 0; line < 10 && is.getline(cline,256); line++)
        if (line == 0 && strncmp(cline,"#FEDEM response data",20))
          break; // Invalid frs-file, ignored
        else if (strstr(cline,"Module") && strstr(cline,"fedem_reducer"))
        {
          // This is the right file
          frsFile = fileName;
          return true;
        }
    }

  return false;
}


int FapStressExpander::checkDependencies() const
{
  if (amIPreparingForBatch)
    return this->FapRecoveryBase::checkDependencies();

#ifdef FAP_DEBUG
  std::cout <<"FapStressExpander::checkDependencies()"<< std::endl;
#endif

  // Check if we have a direct solution for this part
  if (this->getDirectSolveResults(mySamFile,myFrsFile))
    return FAP_READY_TO_RUN;

  // No direct solution found, check for system solver results
  return this->FapRecoveryBase::checkDependencies();
}


int FapStressExpander::createInput(std::string& rdbPath)
{
#ifdef FAP_DEBUG
  std::cout <<"FapStressExpander::createInput()"<< std::endl;
#endif

  // Check the dependencies for this part
  int depVal = this->checkDependencies();
  if (depVal != FAP_READY_TO_RUN)
    return depVal;

  bool directSol = !myFrsFile.empty();

  // Get paths to all directories accessed by this process
  std::string mechRDBPath, partPath;
  if (!this->getInputAndRunDirs(mechRDBPath, directSol ? NULL : &partPath, &rdbPath))
  {
    FFaMsg::dialog("Failed to set up directory structure for recovery process.\n"
                   "Check Output List for details.",FFaMsg::ERROR);
    return FAP_NOT_EXECUTABLE;
  }

  // Calculation options
  FFaOptionFileCreator fcoArgs(rdbPath + mySolverName + ".fco");
  fcoArgs.add("-linkId", myWorkPart->getBaseID());
  if (myWorkPart->isFELoaded(true))
  {
    // Write a local copy of the FE part data currently in core to file
    myWorkPart->exportPart(rdbPath + myWorkPart->baseFTLFile.getValue(),
                           true, false, true);
    fcoArgs.add("-linkfile", myWorkPart->baseFTLFile.getValue());
  }
  else // use the FE part data file from the part repository
    fcoArgs.add("-linkfile", FFaFilePath::getRelativeFilename(rdbPath, myWorkPart->getBaseFTLFile()));

  if (directSol)
  {
    fcoArgs.add("-samfile", FFaFilePath::getRelativeFilename(rdbPath, mySamFile));
    fcoArgs.add("-frsfile", FFaFilePath::getRelativeFilename(rdbPath, myFrsFile));
    fcoArgs.add("-fsifile", std::string(), false);
  }
  else
  {
    fcoArgs.add("-Bmatfile", FFaFilePath::getRelativeFilename(rdbPath, partPath + myWorkPart->BMatFile.getValue()));
    if (myWorkPart->nGenModes.getValue() > 0)
      fcoArgs.add("-eigfile",FFaFilePath::getRelativeFilename(rdbPath, partPath + myWorkPart->EMatFile.getValue()));
    else if (myWorkPart->nGenModes.getValue() < 0)
      fcoArgs.add("-dispfile",FFaFilePath::getRelativeFilename(rdbPath, partPath + myWorkPart->DMatFile.getValue()));
    fcoArgs.add("-samfile",  FFaFilePath::getRelativeFilename(rdbPath, partPath + myWorkPart->SAMdataFile.getValue()));
    fcoArgs.add("-fsifile",  FFaFilePath::getRelativeFilename(rdbPath, mechRDBPath + "fedem_solver.fsi"));
    fcoArgs.add("-frsfile", "<"+getFRSfiles("timehist_prim")+">", false);
  }

  if (!mySelectedGroups.empty())
  {
    std::string groupDef;
    for (FmElementGroupProxy* group : mySelectedGroups)
      if (groupDef.empty())
        groupDef = group->getGroupId();
      else
        groupDef += "," + group->getGroupId();
    groupDef = "<" + groupDef + ">";
    fcoArgs.add("-group", groupDef, false);
  }
  if (myWorkPart->useExternalResFile.getValue())
  {
    std::string extFile = this->eventName(myWorkPart->externalResFileName.getValue());
    fcoArgs.add("-resStressFile",FFaFilePath::getRelativeFilename(rdbPath,extFile));
  }
  fcoArgs.writeOptFile();

  // Output options
  FFaOptionFileCreator fopArgs(rdbPath + mySolverName + ".fop");
  fopArgs.add("-resfile",mySolverName + ".res");
  fopArgs.add("-rdbfile",FFaFilePath::getBaseName(myWorkPart->baseFTLFile.getValue(),true) + ".frs");
  fopArgs.add("-rdbinc",FmFileSys::getNextIncrement(rdbPath,"frs"));
  FmAnalysis* curAn = FmDB::getActiveAnalysis();
  if (curAn->autoStressVTFExport.getValue())
  {
    std::string vtfFile = this->eventName(curAn->stressVTFname.getValue());
    fopArgs.add("-VTFfile",FFaFilePath::getRelativeFilename(rdbPath,vtfFile));
    fopArgs.add("-VTFinit",initStateOnVTF);
    fopArgs.add("-VTFoffset",(int)startVTFblock);
    if (iAmLast) fopArgs.add("-VTFparts",(int)nPartsVTF);
    int maxStep = curAn->getNumberOfStressTimeSteps();
    if (initStateOnVTF) maxStep++;
    startVTFblock += (size_t)pow(10.0,ceil(log10((double)(2*maxStep))));
  }
  fopArgs.add("-deformation",curAn->stressDeformation.getValue());
  fopArgs.add("-stress",curAn->stressStressTensor.getValue());
  fopArgs.add("-strain",curAn->stressStrainTensor.getValue());
  fopArgs.add("-SR",curAn->stressSRTensor.getValue());
  fopArgs.add("-maxPStrain",curAn->stressMaxPstrain.getValue());
  fopArgs.add("-maxPStress",curAn->stressMaxPstress.getValue());
  fopArgs.add("-maxSStrain",curAn->stressMaxSstrain.getValue());
  fopArgs.add("-maxSStress",curAn->stressMaxSstress.getValue());
  fopArgs.add("-minPStrain",curAn->stressMinPstrain.getValue());
  fopArgs.add("-minPStress",curAn->stressMinPstress.getValue());
  fopArgs.add("-vmStrain",curAn->stressVMstrain.getValue());
  fopArgs.add("-vmStress",curAn->stressVMstress.getValue());
  fopArgs.add("-statm",curAn->stressStartTime.getValue());
  fopArgs.add("-stotm",curAn->stressStopTime.getValue());
  fopArgs.add("-tinc",curAn->stressAllTimeSteps.getValue() ? 0.0 : curAn->stressTimeIncr.getValue());
  fopArgs.writeOptFile();

  // Additional options, if any
  std::string addOptions = curAn->stressAddOpts.getValue();
  if (!addOptions.empty())
  {
    FFaOptionFileCreator faoArgs(rdbPath + mySolverName + ".fao");
    faoArgs.addComment("Additional user defined options to " + mySolverName);
    faoArgs.add(addOptions,"",false);
    faoArgs.writeOptFile();
  }

  return FAP_READY_TO_RUN;
}


int FapStressExpander::startProcess(const std::string& rdbPath)
{
#ifdef FAP_DEBUG
  std::cout <<"FapStressExpander::startProcess()"<< std::endl;
#endif

  return this->startRecovery("Stress",rdbPath,
                             FmDB::getActiveAnalysis()->stressAddOpts.getValue());
}


bool FapStressExpander::writeVTFHeader(std::vector<FmPart*>& feParts,
                                       const std::string& vtfName)
{
  startVTFblock = 0;
  initStateOnVTF = false;
  int nPart = feParts.size();
  std::vector<FmLink*> feLinks(nPart,NULL);

  // Check if the initial state with residual stresses
  // should be written to the VTF-file too
  for (FmPart* part : feParts)
  {
    if (part->useExternalResFile.getValue())
      initStateOnVTF = true;
    feLinks[--nPart] = part; // Restore forward order
  }

  FmAnalysis* analysis = FmDB::getActiveAnalysis();
  FapExpProp prop;
  prop.deformation = true;
  prop.fringe = true;
  prop.range = analysis->stressVTFrange.getValue();
  return FapSolverBase::writeVTFHeader(feLinks,vtfName,
                                       analysis->stressVTFtype.getValue(),prop);
}
