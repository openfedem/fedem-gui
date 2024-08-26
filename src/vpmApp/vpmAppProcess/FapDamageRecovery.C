// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppProcess/FapDamageRecovery.H"
#include "vpmApp/vpmAppCmds/FapStrainCoatCmds.H"
#include "vpmApp/vpmAppProcess/FapSolverID.H"
#include "vpmApp/FapLicenseManager.H"

#include "vpmPM/FpPM.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmFileSys.H"
#include "vpmDB/FmAnalysis.H"
#include "vpmDB/FmFppOptions.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmElementGroupProxy.H"

#include "FFaLib/FFaCmdLineArg/FFaOptionFileCreator.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"


/*! fedem_fpp options:
  -Bmatfile      Name of B-matrix file
  -Bramsize      In-core size (MB) of displacement recovery matrix
                 < 0: Use the same as in the reducer (default)
                 = 0: Store full matrix in core
  -BufSizeInc    Buffer increment size
  -HistDataType  Histogram data type
                 = 0: None
                 = 1: Signed abs max stress
                 = 2: Signed abs max strain
  -HistXBins     Histogram number of X-bins
  -HistXMax      Histogram max X-value
  -HistXMin      Histogram min X-value
  -HistYBins     Histogram number of Y-bins
  -HistYMax      Histogram max Y-value
  -HistYMin      Histogram min Y-value
  -PVXGate       Gate value for the Peak Valley extension
                 (MPa or microns depending on HistDataType)
  -SNfile        Name of SN-curve definition file
  -angleBins     Number of bins in search for most popular angle
  -biAxialGate   Gate value for the biaxiality calculation
  -blockSize     Max number of elements processed together
  -consolemsg    Output error messages to console
  -cwd           Change working directory
  -debug         Debug print switch
  -double        Save results in double precision
  -dispfile      Name of gravitation displacement file
  -eigfile       Name of eigenvector file
  -fao           Read additional options from this file
  -fco           Read calculation options from this file
  -fop           Read output options from this file
  -fppfile       Name of fpp output file
  -frsfile       Name of solver results database file
  -fsifile       Name of solver input file
  -group         List of element groups to do calculation for
  -help          Print out this help text
  -licenseinfo   Print out license information at startup
  -licensepath   License file directory
  -linkId        Part base-ID number
  -linkfile      Name of part input file
  -rdbfile       Name of strain coat results database file
  -rdbinc        Increment number for the results database file
  -resStressFile Name of residual stress input file
  -resStressSet  Name of residual stress set
  -resfile       Name of result output file
  -samfile       Name of SAM data file
  -statm         Start time
  -stotm         Stop time
  -stressToMPa   Scale factor to convert stresses to MPa
  -surface       Surface selection option
                 = 0: All element surfaces
                 = 1: Bottom shell surfaces only
                 = 2: Middle shell surfaces only
                 = 3: Top shell surfaces only
  -terminal      File unit number for terminal output
  -tinc          Time increment (= 0.0: process all time steps)
  -version       Print out program version
*/


FapDamageRecovery::FapDamageRecovery(FmPart* aPart, FmSimulationEvent* event,
                                     bool preBatch)
  : FapRecoveryBase(aPart,event)
{
  mySolverName = "fedem_fpp";
  mySubTask = "summary_rcy";
  myGroupID = FapSolverID::FAP_FPP;
  amIPreparingForBatch = preBatch;
  autoCreateStrainCoat = false;
}

FapDamageRecovery::FapDamageRecovery(FmPart* aPart, const std::vector<FmElementGroupProxy*>& groups)
  : FapRecoveryBase(aPart)
{
  mySolverName = "fedem_fpp";
  mySubTask = "summary_rcy";
  mySelectedGroups = groups;
  myGroupID = FapSolverID::FAP_FPP;
  autoCreateStrainCoat = true;
}


int FapDamageRecovery::createInput(std::string& rdbPath)
{
#ifdef FAP_DEBUG
  std::cout <<"FapDamageRecovery::createInput()"<< std::endl;
#endif

  // Check the dependencies for this part
  int depVal = this->checkDependencies();
  if (depVal != FAP_READY_TO_RUN)
    return depVal;

  // If the FE data is loaded, create a strain coat on the current selection
  if (myWorkPart->isFELoaded(true))
    if (FapStrainCoatCmds::makeStrainCoat(autoCreateStrainCoat,myWorkPart,mySelectedGroups) < 1)
    {
      ListUI <<"===> "<< myWorkPart->getIdString() <<" "
             << myWorkPart->baseFTLFile.getValue()
             <<" does not have a Strain Coat.\n";
      return FAP_RESULTS_OK;
    }

  // Get paths to all directories accessed by this process
  std::string mechRDBPath, partPath;
  if (!this->getInputAndRunDirs(mechRDBPath,&partPath,&rdbPath))
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
  else // use FE data file from the part repository
    // (relying on that it also contains strain coat elements)
    fcoArgs.add("-linkfile", FFaFilePath::getRelativeFilename(rdbPath, myWorkPart->getBaseFTLFile()));

  fcoArgs.add("-Bmatfile", FFaFilePath::getRelativeFilename(rdbPath, partPath + myWorkPart->BMatFile.getValue()));
  if (myWorkPart->nGenModes.getValue() > 0)
    fcoArgs.add("-eigfile",FFaFilePath::getRelativeFilename(rdbPath, partPath + myWorkPart->EMatFile.getValue()));
  else if (myWorkPart->nGenModes.getValue() < 0)
    fcoArgs.add("-dispfile",FFaFilePath::getRelativeFilename(rdbPath, partPath + myWorkPart->DMatFile.getValue()));
  fcoArgs.add("-samfile",  FFaFilePath::getRelativeFilename(rdbPath, partPath + myWorkPart->SAMdataFile.getValue()));
  fcoArgs.add("-fsifile",  FFaFilePath::getRelativeFilename(rdbPath, mechRDBPath + "fedem_solver.fsi"));
  fcoArgs.add("-frsfile", "<"+getFRSfiles("timehist_prim")+">", false);

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
    fcoArgs.add("-resStressFile", FFaFilePath::getRelativeFilename(rdbPath, extFile));
  }

  FmFppOptions* fpp = FmDB::getFppOptions();
  fcoArgs.add("-blockSize", fpp->nElemsTogether.getValue());
  fcoArgs.add("-biAxialGate", fpp->biaxGate.getValue());
  if (fpp->performRainflow.getValue())
  {
    fcoArgs.add("-PVXGate", fpp->pvxGate.getValue());
    if (fpp->useNCode())
    {
      // Using nCode for rainflow analysis and fatigue
      fcoArgs.add("-HistDataType", -fpp->histType.getValue());
      fcoArgs.add("-HistXMin", fpp->histRange.getValue().first);
      fcoArgs.add("-HistXMax", fpp->histRange.getValue().second);
      fcoArgs.add("-HistYMin", fpp->histRange.getValue().first);
      fcoArgs.add("-HistYMax", fpp->histRange.getValue().second);
      fcoArgs.add("-HistXBins", fpp->histNBins.getValue());
      fcoArgs.add("-HistYBins", fpp->histNBins.getValue());
    }
    else
    {
      // Using Fedem's internal fatigue solver
      fcoArgs.add("-HistDataType", fpp->histType.getValue());
      fcoArgs.add("-SNfile", FpPM::getFullFedemPath("sn_curves.fsn",'h'));
    }
    double scale = 1.0e-6;
    if (myWorkMech->modelDatabaseUnits.getValue().convert(scale,"FORCE/AREA"))
      fcoArgs.add("-stressToMPaScale", scale);
  }
  else
    fcoArgs.add("-HistDataType", 0);

  fcoArgs.add("-statm",fpp->startTime.getValue());
  fcoArgs.add("-stotm",fpp->stopTime.getValue());
  fcoArgs.add("-tinc",fpp->allTimeSteps.getValue() ? 0.0 : fpp->timeIncr.getValue());
  fcoArgs.writeOptFile();

  // Output options
  FFaOptionFileCreator fopArgs(rdbPath + mySolverName + ".fop");
  std::string taskName = FFaFilePath::getBaseName(myWorkPart->baseFTLFile.getValue(),true);
  int increment = FmFileSys::getNextIncrement(rdbPath,"frs");
  fopArgs.add("-resfile",mySolverName + ".res");
  fopArgs.add("-fppfile",taskName + FFaNumStr("_fpp_%03d.fpp",increment+1));
  fopArgs.add("-rdbfile",taskName + ".frs");
  fopArgs.add("-rdbinc",increment);
  fopArgs.writeOptFile();

  // Additional options, if any
  std::string addOptions = fpp->addOptions.getValue();
  if (!addOptions.empty())
  {
    FFaOptionFileCreator faoArgs(rdbPath + mySolverName + ".fao");
    faoArgs.addComment("Additional user defined options to " + mySolverName);
    faoArgs.add(addOptions,"",false);
    faoArgs.writeOptFile();
  }

  return FAP_READY_TO_RUN;
}


int FapDamageRecovery::startProcess(const std::string& rdbPath)
{
#ifdef FAP_DEBUG
  std::cout <<"FapDamageRecovery::startProcess()"<< std::endl;
#endif

  return this->startRecovery("Strain Coat",rdbPath,
                             FmDB::getFppOptions()->addOptions.getValue());
}
