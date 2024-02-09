// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <cmath>

#include "vpmApp/vpmAppProcess/FapModeShapeExpander.H"
#include "vpmApp/vpmAppProcess/FapSolverID.H"
#include "vpmApp/vpmAppDisplay/FapVTFFile.H"
#include "vpmApp/FapLicenseManager.H"

#include "vpmDB/FmPart.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmModesOptions.H"
#include "vpmDB/FmAnalysis.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmFileSys.H"

#include "FFaLib/FFaCmdLineArg/FFaOptionFileCreator.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"


/*! fedem_modes options:
  -Bmatfile       Name of B-matrix file
  -Bramsize       In-core size (MB) of displacement recovery matrix
                  < 0: Use the same as in the reducer (default)
                  = 0: Store full matrix in core
  -VTFdscale      Deformation scaling factor for VTF output
  -VTFexpress     Write express VTF-files (one file per mode)
  -VTFfile        Name of VTF output file
  -VTFoffset      VTF result block id offset
  -VTFparts       Number of parts in VTF-file
  -consolemsg     Output error messages to console
  -cwd            Change working directory
  -damped         Complex modes are calculated
  -debug          Debug print switch
  -dispfile       Name of gravitation displacement file
  -double         Save all results in double precision
  -eigfile        Name of eigenvector file
  -energy_density Write scaled strain energy density to results database
  -fao            Read additional options from this file
  -fco            Read calculation options from this file
  -fop            Read output options from this file
  -frsfile        Name of solver results database file
  -fsifile        Name of solver input file
  -help           Print out this help text
  -licenseinfo    Print out license information at startup
  -licensepath    License file directory
  -linkId         Part base-ID number
  -linkfile       Name of part input file
  -rdbfile        Name of modes results database file
  -rdbinc         Increment number for the results database file
  -recover_modes  List of mode numbers to expand
  -resfile        Name of result output file
  -samfile        Name of SAM data file
  -terminal       File unit number for terminal output
  -version        Print out program version
  -write_nodes    Save results as nodal data
  -write_vector   Save results as vector data
*/


size_t FapModeShapeExpander::startVTFblock = 0;
size_t FapModeShapeExpander::nPartsVTF     = 0;


FapModeShapeExpander::FapModeShapeExpander(FmPart* aPart, FmSimulationEvent* event,
					   bool preBatch, bool ImSolvingLastPart)
  : FapRecoveryBase(aPart,event)
{
  mySolverName = "fedem_modes";
  mySubTask = "eigval_rcy";
  myGroupID = FapSolverID::FAP_MODES;
  amIPreparingForBatch = preBatch;
  iAmLast = ImSolvingLastPart;
}


int FapModeShapeExpander::createInput(std::string& rdbPath)
{
#ifdef FAP_DEBUG
  std::cout <<"FapModeShapeExpander::createInput()"<< std::endl;
#endif

  // Check if eigenvalue analysis is activated
  FmAnalysis* curAn = FmDB::getActiveAnalysis();
  if (!curAn->solveEigenvalues.getValue())
    return FAP_RESULTS_OK;

  // Get the list of mode shapes to expand
  FmModesOptions* currEig = FmDB::getModesOptions();
  const FmModeVec& eigArr = currEig->modesArray.getValue();
  if (eigArr.empty())
  {
    FFaMsg::list("===> No modes are specified for shape expansion\n",true);
    return FAP_RESULTS_OK;
  }

  // Check the dependencies for this part
  int depVal = this->checkDependencies();
  if (depVal != FAP_READY_TO_RUN)
    return depVal;

  // Get paths to all directories accessed by this process
  std::string mechRDBPath, partPath;
  if (!this->getInputAndRunDirs(mechRDBPath,&partPath,&rdbPath))
  {
    FFaMsg::dialog("Failed to set up directory structure for recovery process.\n"
                   "Check Output List for details.",FFaMsg::ERROR);
    return FAP_NOT_EXECUTABLE;
  }

  std::string partFile = this->getFEDataFile(partPath);
  if (partFile.empty())
    return FAP_NOT_EXECUTABLE;

  // Calculation options
  FFaOptionFileCreator fcoArgs(rdbPath + mySolverName + ".fco");
  fcoArgs.add("-linkId",   myWorkPart->getBaseID());
  fcoArgs.add("-linkfile", FFaFilePath::getRelativeFilename(rdbPath, partFile));
  fcoArgs.add("-Bmatfile", FFaFilePath::getRelativeFilename(rdbPath, partPath + myWorkPart->BMatFile.getValue()));
  if (myWorkPart->nGenModes.getValue() > 0)
    fcoArgs.add("-eigfile",FFaFilePath::getRelativeFilename(rdbPath, partPath + myWorkPart->EMatFile.getValue()));
  else if (myWorkPart->nGenModes.getValue() < 0)
    fcoArgs.add("-dispfile",FFaFilePath::getRelativeFilename(rdbPath, partPath + myWorkPart->DMatFile.getValue()));
  fcoArgs.add("-samfile",  FFaFilePath::getRelativeFilename(rdbPath, partPath + myWorkPart->SAMdataFile.getValue()));
  fcoArgs.add("-fsifile",  FFaFilePath::getRelativeFilename(rdbPath, mechRDBPath + "fedem_solver.fsi"));
  fcoArgs.add("-frsfile", "<"+getFRSfiles("timehist_prim")+","+getFRSfiles("eigval")+">", false);
  fcoArgs.add("-damped",   curAn->dampedEigenvalues.getValue());
  fcoArgs.writeOptFile();

  // Compose the mode shape extraction string
  std::map<double, std::set<int> > requestedModes;
  for (const FmModeType& mode : eigArr)
    requestedModes[mode.second].insert(mode.first);
  std::string modstr;
  for (std::pair<const double, std::set<int> >& mode : requestedModes)
  {
    if (modstr.empty())
      modstr = "<<";
    else
      modstr += ",<";
    modstr += FFaNumStr(mode.first);
    for (int mode : mode.second)
      modstr += FFaNumStr(",%d",mode);
    modstr += ">";
  }
  modstr += ">";

  // Output options
  FFaOptionFileCreator fopArgs(rdbPath + mySolverName + ".fop");
  fopArgs.add("-resfile",mySolverName + ".res");
  fopArgs.add("-rdbfile",FFaFilePath::getBaseName(partFile,true) + ".frs");
  fopArgs.add("-rdbinc",FmFileSys::getNextIncrement(rdbPath,"frs"));
  fopArgs.add("-recover_modes",modstr,false);
  if (currEig->autoVTFExport.getValue())
  {
    std::string vtfFile = this->eventName(currEig->vtfFileName.getValue());
    fopArgs.add("-VTFfile",FFaFilePath::getRelativeFilename(rdbPath,vtfFile));
    fopArgs.add("-VTFexpress",(currEig->vtfFileType.getValue()==VTF_EXPRESS));
    fopArgs.add("-VTFoffset",(int)startVTFblock);
    if (iAmLast) fopArgs.add("-VTFparts",(int)nPartsVTF);
    size_t numBlocks = requestedModes.size() + eigArr.size();
    startVTFblock += (size_t)pow(10.0,ceil(log10((double)numBlocks)));
  }
  fopArgs.writeOptFile();

  // Additional options, if any
  std::string addOptions = currEig->addOptions.getValue();
  if (!addOptions.empty())
  {
    FFaOptionFileCreator faoArgs(rdbPath + mySolverName + ".fao");
    faoArgs.addComment("Additional user defined options to " + mySolverName);
    faoArgs.add(addOptions,"",false);
    faoArgs.writeOptFile();
  }

  return FAP_READY_TO_RUN;
}


int FapModeShapeExpander::startProcess(const std::string& rdbPath)
{
#ifdef FAP_DEBUG
  std::cout <<"FapModeShapeExpander::startProcess()"<< std::endl;
#endif

  return this->startRecovery("Mode Shapes",rdbPath,
                             FmDB::getModesOptions()->addOptions.getValue());
}


bool FapModeShapeExpander::writeVTFHeader(std::vector<FmPart*>& feParts,
				          const std::string& vtfName)
{
  startVTFblock = 0;

  FmModesOptions* opts = FmDB::getModesOptions();
  VTFFileType vtfFtype = opts->vtfFileType.getValue();
  std::vector<std::string> vtfNames;
  if (vtfFtype == VTF_EXPRESS)
  {
    // Create one file for each expanded mode shape
    std::set<int> modeNums;
    for (const FmModeType& mode : opts->modesArray.getValue())
      modeNums.insert(mode.first);
    for (int mode : modeNums)
    {
      vtfNames.push_back(vtfName);
      vtfNames.back().insert(vtfName.find_last_of('.'),FFaNumStr("_%d",mode));
    }
  }
  else // Put all mode shapes into the same file
    vtfNames.push_back(vtfName);

  int nPart = feParts.size();
  std::vector<FmLink*> feLinks(nPart,NULL);
  for (FmPart* part : feParts)
    feLinks[--nPart] = part; // Restore forward order

  FapExpProp prop;
  prop.modeShape = true;
  return FapSolverBase::writeVTFHeader(feLinks,vtfNames,vtfFtype,prop,false);
}
