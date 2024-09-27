// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <functional>
#include <algorithm>

#include "vpmApp/vpmAppCmds/FapOilWellCmds.H"
#include "vpmApp/vpmAppUAMap/FapUALinkRamSettings.H"
#include "vpmApp/FapLicenseManager.H"

#include "assemblyCreators/assemblyCreators.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmBeam.H"
#include "vpmDB/FmBeamProperty.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmPipeSurface.H"
#include "vpmDB/FmPipeStringDataExporter.H"
#include "vpmDB/FmJointMotion.H"
#include "vpmDB/FmJointSpring.H"
#include "vpmDB/FmJointDamper.H"
#include "vpmDB/FmBallJoint.H"
#include "vpmDB/FmFreeJoint.H"
#include "vpmDB/FmStraightMaster.H"
#include "vpmDB/FmCylJoint.H"
#include "vpmDB/FmCamJoint.H"
#include "vpmDB/FmCamFriction.H"
#include "vpmDB/FmfConstant.H"
#include "vpmDB/FmfLimRamp.H"
#include "vpmDB/FmfLinVar.H"
#include "vpmDB/FmfSpline.H"
#include "vpmDB/FmfSinusoidal.H"
#include "vpmDB/FmLoad.H"
#include "vpmDB/FmRiser.H"
#include "vpmDB/FmSoilPile.H"

#include "vpmUI/Fui.H"
#include "FFuLib/FFuProgressDialog.H"

#include "FFaLib/FFaDefinitions/FFaResultDescription.H"
#include "FFaLib/FFaDefinitions/FFaViewItem.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFaLib/FFaString/FFaParse.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaAlgebra/FFaMath.H"
#include "FFaLib/FFaAlgebra/FFa3PArc.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"

#include "vpmPM/FpRDBExtractorManager.H"
#include "FFrLib/FFrExtractor.H"
#include "FFrLib/FFrVariableReference.H"
#include "FFrLib/FFrReadOp.H"


struct Data
{
  double MD, Inc, Azi;
  double TVD, North, East;
  double Dogleg, VerticalSection;
};


void FapOilWellCmds::createPipeSurface(const std::string& fileName)
{
  if (!FapLicenseManager::checkLicense("FA-OWL"))
    return;

  std::ifstream is(fileName.c_str());

  Fui::noUserInputPlease();
  ListUI <<"===> Reading pipe surface definition from "<< fileName <<"\n";
  FFaMsg::pushStatus("Reading pipe surface");

  std::vector<Data> myPoints;

  while (FaParse::skipWhiteSpaceAndComments(is))
    {
      Data data;
      is >> data.MD >> data.Inc >> data.Azi;
#ifdef USE_TVD_NORTH_EAST
      char N, E;
      is >> data.TVD >> data.North >> N >> data.East >> E >> data.Dogleg >> data.VerticalSection;
      data.TVD = -data.TVD;
      if (N == 'S')
	data.North = -data.North;
      if (E == 'W')
	data.East = -data.East;
#else
      std::string dummy;
      std::getline(is,dummy);
#endif
      myPoints.push_back(data);
    }

  is.close();
  ListUI <<"     Read "<< (int)myPoints.size()
	 <<" surface records, giving "<< 2*((int)myPoints.size()-1) <<" Triads\n";
  FFaMsg::popStatus();

  FFaMsg::pushStatus("Creating pipe surface");
  FmPipeSurface* surf = new FmPipeSurface();
  surf->connect();

  std::vector<FFa3PArc> arcs;
  FaVec3 tangent, tangent2, point;

  size_t i, j;
  for (i = 1; i < myPoints.size(); i++)
    {
#ifdef USE_TVD_NORTH_EAST
      FaVec3 start(myPoints[i-1].East, myPoints[i-1].North, myPoints[i-1].TVD);
      FaVec3 end(myPoints[i].East, myPoints[i].North, myPoints[i].TVD);
      arcs.push_back(FFa3PArc::makeFromTangentP1P2(end-start,start,end));
#else
      tangent.setBySphCoords(FaVec3(1.0, RAD(90.0-myPoints[i-1].Azi), RAD(180.0-myPoints[i-1].Inc)));
      tangent2.setBySphCoords(FaVec3(1.0, RAD(90.0-myPoints[i].Azi), RAD(180.0-myPoints[i].Inc)));
      arcs.push_back(FFa3PArc::makeFromP1T1T2L(point, tangent, tangent2, myPoints[i].MD - myPoints[i-1].MD));
      point = arcs.back().back();
#endif
    }

  double length = myPoints[0].MD;
  for (i = 0; i < arcs.size(); i++)
    {
      for (j = 0; j < 2; j++) {
	FmTriad* t = new FmTriad();
	if (j == 0)
	  t->setUserDescription(FFaNumStr(myPoints[i].MD) + " " + FFaNumStr(length));
	t->connect();

	// Some more intelligent horizontal could be needed if well is in e-w heading
	t->setGlobalCS(arcs[i].getCtrlPointMatrix(j, FaVec3(-1,0,0), false));
	surf->addTriad(t);
	t->draw();
      }
      length += arcs[i].getArcLength();
    }

  FmTriad* t = new FmTriad();
  t->setUserDescription(FFaNumStr(length));
  t->connect();
  t->setGlobalCS(arcs[i-1].getCtrlPointMatrix(2, FaVec3(-1,0,0), false));
  surf->addTriad(t);
  t->draw();

  surf->draw();

  FFaMsg::popStatus();
  Fui::okToGetUserInput();
}


typedef std::pair<double,double> FmPair;

struct PipeData
{
  std::string description;
  std::string fileName;
  int repeats;
  double length;
  double dStart;
  double dEnd;
  double time;
  double dampingR;
  double dampingA;
  std::vector<FmPair> additionalContactP;
};

#define USE_JOINT_CHAIN

void FapOilWellCmds::createPipeString(const std::string& fileName)
{
  if (!FapLicenseManager::checkLicense("FA-OWL"))
    return;

  std::ifstream is(fileName.c_str());
  if (!FaParse::skipWhiteSpaceAndComments(is)) return;

  Fui::noUserInputPlease();
  ListUI <<"===> Reading pipe string definition from "<< fileName <<"\n";
  FFaMsg::pushStatus("Reading pipe string");

  // First read positions

  FaVec3 platformPos;
  is >> platformPos;
  FaParse::skipWhiteSpaceAndComments(is);

  FaVec3 wellHeadPos;
  is >> wellHeadPos; // Actually no used. Should be removed, possibly.
  FaParse::skipWhiteSpaceAndComments(is);

  double lowerTime;
  is >> lowerTime;
  if (!is.good()) {
    ListUI <<"     Note : Lowering time set to 50s.\n";
    lowerTime = 50;
    is.clear();
  }

  // Read all the lines

  std::vector<PipeData> pipes;
  double totLength = 0;
  int nPipeRecords = 0;
  int nPipes = 0;
  while (FaParse::skipWhiteSpaceAndComments(is,true))
    {
      PipeData pipe;
      std::string dummy;
      char c;

      // Description
      is.get(c); // "
      std::getline(is, pipe.description, '"');
      std::getline(is, dummy, '"');

      // File name
      std::getline(is, pipe.fileName, '"');

      is >> pipe.repeats;
      is >> pipe.length;
      is >> pipe.dStart;
      is >> pipe.dEnd;
      is >> pipe.time;
      pipe.time *= 60;
      is >> pipe.dampingR;
      is >> pipe.dampingA;

      while (is) {
        FmPair dAndPos;
        is >> dAndPos.first;
        if (is)
          is >> dAndPos.second;
        if (is)
          pipe.additionalContactP.push_back(dAndPos);
      }
      is.clear();
      nPipeRecords++;
      nPipes += pipe.repeats;
      pipes.push_back(pipe);

      // Calculate total pipe length and depth from Drill platform
      totLength += pipe.repeats * pipe.length;
    }

  is.close();
  ListUI <<"     Read "<< nPipeRecords <<" pipe records, giving "<< nPipes <<" pipes.\n";
  FFaMsg::popStatus();


  /////////////////////////////////////////////////
  // Prepare to create string.
  // Create all the stuff used by all the pipes etc.

  FFaMsg::pushStatus("Creating model");

  FmPipeStringDataExporter* pse = new FmPipeStringDataExporter();
  pse->connect();

  // Find all selected Pipe surfaces to hook the pipe string onto it

  std::vector<FmPipeSurface*> pipeSurfs;
  FapCmdsBase::getSelected(pipeSurfs);

  // Create Drill platform

  FmPart* drillPlatform = new FmPart(platformPos);
  drillPlatform->setUserDescription("Drill platform");
  drillPlatform->connect();
  drillPlatform->useGenericProperties.setValue(true);

  drillPlatform->myGenericPartStiffType = FmPart::NODE_STIFFNESS;
  drillPlatform->kt = 1e9;
  drillPlatform->kr = 1e10;
  drillPlatform->alpha2 = 0.5;
  drillPlatform->alpha1 = 0.5;
  drillPlatform->inertia = FFaTensor3(0.1,0.1,0.1);

  drillPlatform->draw();

  // Create default heave motion function

  FmfSinusoidal* heaveFunc = new FmfSinusoidal();
  heaveFunc->setFunctionUse(FmMathFuncBase::GENERAL);
  heaveFunc->setUserDescription("Default Heave Function");
  heaveFunc->connect();
  heaveFunc->setFrequency(0.04);
  heaveFunc->setAmplitude(0.25);
  heaveFunc->setMaxTime(0.0);

  FmEngine* motionEngine = new FmEngine();
  motionEngine->setUserDescription("Platform motion");
  motionEngine->connect();
  motionEngine->setFunction(heaveFunc);
  pse->hivEngine = motionEngine;

  FmTriad* TDP = new FmTriad(platformPos);
  TDP->setUserDescription("Drill platform input motion triad");
  TDP->connect(drillPlatform);
  for (int dof = 0; dof < 6; dof++)
    TDP->setStatusForDOF(dof,FmHasDOFsBase::FIXED);
  TDP->setStatusForDOF(2,FmHasDOFsBase::PRESCRIBED);
  TDP->getMotionAtDOF(2,true)->setEngine(motionEngine);
  TDP->draw();

  // Create Cam Friction

  FmCamFriction* camFriction = new FmCamFriction();
  camFriction->setUserDescription("Pipe String Friction");
  camFriction->setCoulombCoeff(0.2);
  camFriction->connect();

  // Create contact spring

  FmfLinVar* contactSpring = new FmfLinVar();
  contactSpring->setFunctionUse(FmMathFuncBase::SPR_TRA_STIFF);
  contactSpring->setUserDescription("Contact spring characteristics (Stiffness)");
  contactSpring->connect();
  contactSpring->addXYset(0.0,0.0);
  contactSpring->addXYset(0.009,3.0e6);
  contactSpring->setExtrapolationType(1);

  // Create stabilizing zrot spring for the cam

  FmfConstant* radSpring = new FmfConstant();
  radSpring->setFunctionUse(FmMathFuncBase::SPR_ROT_STIFF);
  radSpring->setUserDescription("ZRot spring characteristics (Stiffness)");
  radSpring->connect();
  radSpring->setConstant(1.0e3);

  // Create Depth vs Time engine

  FmfLinVar* depthFunc = new FmfLinVar();
  depthFunc->setFunctionUse(FmMathFuncBase::GENERAL);
  depthFunc->connect();

  FmEngine* depthEngine = new FmEngine();
  depthEngine->setUserDescription("String depth vs time");
  depthEngine->connect();
  depthEngine->setFunction(depthFunc);

  // Todo, create depth time curve from input file


  ////////////////////////////////////////////////////////////////
  // Create the complete pipe string

  FmPart*      Lc;   // Current Part
  FmPart*      Lcf = NULL; // First instance of current part
  FmFreeJoint* FJDc; // Fastener of pipe to Drill platform
  FmFreeJoint* FJCc; // Connection between pipes

  FmTriad*     TBLc; // Triad Bottom current part
  FmTriad*     TTLc; // Triad Top current part
  FmTriad*     TDc;  // Triad on Drill platform for current part
  FmTriad*     TBLn = 0; // Triad on bottom of next part

  double currentStringLength = 0.0; // Accumulated length of pipes first one to enter well
  double MD = totLength; // Accumulated length along mounted string from Drill platform to bottom of first pipe

  const char* MDfrmt = " MD: %.6g";
  int currentPipeNum = 0;
  double totalTime = 0.0;

  FFaMsg::enableSubSteps(nPipes);
  FFaMsg::enableProgress(3*nPipes);
  FFaMsg::setProgress(0);

  depthFunc->addXYset(totalTime, currentStringLength);

  for (int i = pipes.size()-1; i >= 0; i--)
    for (int repeat = 0; repeat < pipes[i].repeats; repeat++)
      {
	FFaMsg::setSubStep(currentPipeNum+1);
	FFaMsg::setSubTask(FFaFilePath::getFileName(pipes[i].fileName));

	// Create the current part

	Lc = new FmPart(platformPos);
	Lc->connect();

	if (pipes[i].fileName.empty() || Lc->setVisualizationFile(pipes[i].fileName,false))
	  {
	    // Generic part, make mass and inertia of a cylinder and 30 Pounds pr feet

	    Lc->useGenericProperties.setValue(true);

	    double D  = pipes[i].dStart;
	    double L  = pipes[i].length;
	    double m  = L * 44.6; // 30 Pounds Pr Feet
	    double Iz = (m/2.0)  * (0.25*D*D);
	    double Ix = (m/12.0) * (0.75*D*D + L*L);

	    Lc->mass = m;
	    Lc->inertia = FFaTensor3(Ix,Ix,Iz);
	    Lc->setPositionCG(FaVec3(0.0,0.0,0.5*L),true);
	  }
	else if (repeat == 0)
	  {
	    ListUI <<" ==> Open FE data file: "<< pipes[i].fileName <<"\n";
	    FFaMsg::pushStatus("Loading FE part");
	    Lc->importPart(FmPart::locateFEfile(pipes[i].fileName),0,true);
	    Lc->onChanged();
	    FFaMsg::popStatus();
	    Lcf = Lc;
	  }
	else if (Lcf)
	  {
	    FFaMsg::pushStatus("Copying FE part");
	    Lcf->updateCachedCheckSum();
	    Lc->clone(Lcf, FmBase::SHALLOW);
	    Lc->createElemGroupProxies();
	    FFaMsg::popStatus();
	  }

	FFaMsg::setProgress(3*currentPipeNum+1);

	Lc->setUserDescription(FFaNumStr("P%d : ",currentPipeNum) + pipes[i].description);
	Lc->setMeshType(FmPart::SIMPLIFIED);
	Lc->setModelType(FmPart::SIMPLIFIED);

	// Create Triads to be on the pipe

	if (TBLn)
	  TBLc = TBLn; // Use the previously created bottom triad
	else {
	  TBLc = new FmTriad(platformPos);
	  TBLc->setUserDescription(FFaNumStr("Bottom of P%d",currentPipeNum) + FFaNumStr(MDfrmt,MD));
	  TBLc->connect();
	  TBLc->draw();
	}

	TTLc = new FmTriad(platformPos + FaVec3(0,0,pipes[i].length));
	TTLc->setUserDescription(FFaNumStr("Top of P%d",currentPipeNum) + FFaNumStr(MDfrmt,MD-pipes[i].length));
	TTLc->connect();
	Lc->attach(TTLc);
	TTLc->draw();

	TBLn = new FmTriad(platformPos);
	TBLn->setUserDescription(FFaNumStr("Bottom of P%d",currentPipeNum+1) + FFaNumStr(MDfrmt,MD-pipes[i].length));
	TBLn->connect();
	Lc->attach(TBLc);
	TBLn->draw();

	// Attach last next triad to drill platform

	if (currentPipeNum+1 == nPipes) {
	  TBLn->disconnect();
	  TBLn->connect(drillPlatform);
	  TBLn->updateTopologyInViewer();
	}

	// Create connecting freejoint

	FJCc = new FmFreeJoint();
	FJCc->setUserDescription(FFaNumStr("P%d",currentPipeNum) + FFaNumStr("-P%d",currentPipeNum+1));
	FJCc->setAsMasterTriad(TBLn);
	FJCc->setAsSlaveTriad(TTLc);
	FJCc->updateLocation();

	for (int dof = 0; dof < 6; dof++)
	  FJCc->setStatusForDOF(dof, FmHasDOFsBase::FIXED);
	FJCc->setStatusForDOF(2, FmHasDOFsBase::PRESCRIBED);
	FJCc->connect();

	// Create downward motion control for this freejoint

	FmfSpline* lramp = new FmfSpline();
	lramp->setFunctionUse(FmMathFuncBase::GENERAL);
	lramp->connect();

	FmEngine* moveDownMotion = new FmEngine();
	moveDownMotion->setUserDescription(FFaNumStr("MoveDown of pipe %d",currentPipeNum));
	moveDownMotion->connect();
	moveDownMotion->setFunction(lramp);
	moveDownMotion->setSensor(depthEngine->getSimpleSensor(true));

	double lnt = pipes[i].length;
	double csl = currentStringLength;
	lramp->addXYset(0         + csl, 0);
	lramp->addXYset(0.1 * lnt + csl, -0.028 * lnt);
	lramp->addXYset(0.9 * lnt + csl, -0.972 * lnt);
	lramp->addXYset(1   * lnt + csl, -1     * lnt);
	lramp->setExtrapolationType(1);

	depthFunc->addXYset(totalTime + pipes[i].time, csl);
	depthFunc->addXYset(totalTime + pipes[i].time + lowerTime, csl + lnt);
	depthFunc->setExtrapolationType(1);
	totalTime += pipes[i].time + lowerTime;

	pse->stringFrontDepths.getValue().push_back(csl + lnt);
	pse->times.getValue().push_back(totalTime);

	FmJointMotion* pm = static_cast<FmJointMotion*>(FJCc->getMotionAtDOF(2,true));
	pm->setEngine(moveDownMotion);
	pm->setInitLengthOrDefl(0.0,true);

	FJCc->draw();

	// Create freejoint fastener to drill platform

	if (currentPipeNum > 0)
	  {
	    TDc = new FmTriad(platformPos);
	    TDc->setUserDescription(FFaNumStr("Fastener of P%d to drill platform",currentPipeNum));
#ifndef USE_JOINT_CHAIN
	    //TDc->connect(drillPlatform);
#else
	    TDc->connect(FmDB::getEarthLink());
#endif
	    TDc->updateTopologyInViewer();

	    FJDc = new FmFreeJoint();
	    FJDc->setUserDescription(FFaNumStr("P%d-Drill platform #GlobalSpring",currentPipeNum));
#ifndef USE_JOINT_CHAIN
	    FJDc->setAsMasterTriad(TBLc);
	    FJDc->setAsSlaveTriad(TDc);
#else
	    FJDc->setAsMasterTriad(TDc);
	    FJDc->setAsSlaveTriad(TBLc);
#endif
	    FJDc->updateLocation();
	    FJDc->connect();

	    // Create Off switch control for this freejoint

	    FmfLimRamp* step = new FmfLimRamp();
	    step->setFunctionUse(FmMathFuncBase::GENERAL);
	    step->connect();
	    step->setAmplitudeDisplacement(1.0);   // Start displacement
	    step->setSlope(-0.2);                  // Slope
	    step->setDelay(currentStringLength-5); // Start of ramp
	    step->setRampEnd(currentStringLength); // End of ramp

	    FmEngine* offEng = new FmEngine();
	    offEng->setUserDescription(FFaNumStr("Turn off pipe %d fastener",currentPipeNum));
	    offEng->connect();
	    offEng->setFunction(step);
	    offEng->setSensor(depthEngine->getSimpleSensor());

	    for (int dof = 0; dof < 6; dof++) {
	      FJDc->setStatusForDOF(dof,FmHasDOFsBase::SPRING_CONSTRAINED);
	      FJDc->getSpringAtDOF(dof,true)->setInitStiff(dof<3 ? 1e9 : 1e10);
	      FJDc->getSpringAtDOF(dof)->setScaleEngine(offEng);
#ifdef USE_JOINT_CHAIN
	      if (dof == 2)
		FJDc->getSpringAtDOF(dof)->setEngine(motionEngine);
#endif
	      FJDc->getDamperAtDOF(dof,true)->setInitDamp(dof<3 ? 1e3 : 1e4);
	      FJDc->getDamperAtDOF(dof)->setDampEngine(offEng);
	    }

	    FJDc->draw();
	  }

	// Create cam joint on front end of first pipe in hole

	if (currentPipeNum == 0)
	  for (FmPipeSurface* surf : pipeSurfs) {
	    FmCamJoint* cJc = new FmCamJoint();
	    cJc->setAsSlaveTriad(TBLc);
	    cJc->setUserDescription("Front" + FFaNumStr(MDfrmt,MD));
	    cJc->connect();
	    cJc->setThickness(4*pipes[i].length);
	    cJc->setWidth(0);

	    cJc->setFriction(camFriction);
	    cJc->setUsingRadialContact(true);

	    cJc->getSpringAtDOF(0,true)->setInitLengthOrDefl(surf->getPipeRadius() - pipes[i].dEnd/2);
	    cJc->getSpringAtDOF(0)->setSpringCharOrStiffFunction(contactSpring);

	    cJc->setStatusForDOF(5,FmHasDOFsBase::SPRING_CONSTRAINED);
	    cJc->getSpringAtDOF(5,true)->setSpringCharOrStiffFunction(radSpring);

	    cJc->getDamperAtDOF(0,true)->setInitDamp(pipes[i].dampingR);
	    cJc->getDamperAtDOF(1,true)->setInitDamp(pipes[i].dampingR);
	    cJc->getDamperAtDOF(2,true)->setInitDamp(pipes[i].dampingA);

	    cJc->draw();
	    pse->contactPoints.push_back(cJc);
	    pse->jointMDPositions.getValue().push_back(MD);
	  }

	// Create cam joint on top of each pipe, except for the last one

	MD -= pipes[i].length;
	if (currentPipeNum+1 < nPipes)
	  for (FmPipeSurface* surf : pipeSurfs) {
	    FmCamJoint* cJc = new FmCamJoint();
	    cJc->setAsSlaveTriad(TTLc);
	    cJc->setUserDescription(FFaNumStr("P%d",currentPipeNum) + FFaNumStr("-P%d",currentPipeNum+1) + FFaNumStr(MDfrmt,MD));
	    cJc->connect();
	    cJc->setThickness(4*pipes[i].length);
	    cJc->setWidth(0);

	    cJc->setFriction(camFriction);
	    cJc->setUsingRadialContact(true);
	    cJc->setMaster(surf);

	    cJc->getSpringAtDOF(0,true)->setInitLengthOrDefl(surf->getPipeRadius() - pipes[i].dEnd/2);
	    cJc->getSpringAtDOF(0)->setSpringCharOrStiffFunction(contactSpring);

	    cJc->setStatusForDOF(5,FmHasDOFsBase::SPRING_CONSTRAINED);
	    cJc->getSpringAtDOF(5,true)->setSpringCharOrStiffFunction(radSpring);

	    cJc->getDamperAtDOF(0,true)->setInitDamp(pipes[i].dampingR);
	    cJc->getDamperAtDOF(1,true)->setInitDamp(pipes[i].dampingR);
	    cJc->getDamperAtDOF(2,true)->setInitDamp(pipes[i].dampingA);

	    cJc->draw();
	    pse->contactPoints.push_back(cJc);
	    pse->jointMDPositions.getValue().push_back(MD);
	  }

	FFaMsg::setProgress(3*currentPipeNum+2);

	if (repeat > 0) {
	  FFaMsg::pushStatus("Clearing memory");
	  FapUALinkRamSettings::changeRamUsageLevel(Lc, FmPart::NOTHING);
	  FFaMsg::popStatus();
	}
	FFaMsg::pushStatus("Creating visualization");
	FFaMsg::setSubTask(FFaFilePath::getFileName(pipes[i].fileName));
	Lc->draw();
	FFaMsg::popStatus();

	FFaMsg::setProgress(3*currentPipeNum+3);

	currentPipeNum++;
	currentStringLength += pipes[i].length;
      }

  FFaMsg::setSubTask("");
  FFaMsg::disableSubSteps();
  FFaMsg::disableProgress();
  FFaMsg::popStatus();
  Fui::okToGetUserInput();
}


void FapOilWellCmds::exportPipeWearData(const std::string& fileName,
					double startPeriod, double endPeriod)
{
  if (!FapLicenseManager::checkLicense("FA-OWL"))
    return;

  FFaViewItem* obj = FapCmdsBase::findSelectedSingleObject();
  FmPipeStringDataExporter* pipeStr = dynamic_cast<FmPipeStringDataExporter*>(obj);
  if (!pipeStr) return;

  FFrExtractor* ex = FpRDBExtractorManager::instance()->getModelExtractor();
  if (!ex) return;

  std::ofstream outputFile(fileName.c_str());

  // Find heave period and amplitude

  double period = 10;
  double hiv = 0;

  FmfSinusoidal* f = dynamic_cast<FmfSinusoidal*>(pipeStr->hivEngine->getFunction());
  if (f) {
    hiv    = 2.0 * f->getAmplitude();
    period = 1.0 / f->getFrequency();
  }

  const std::vector<double>& times = pipeStr->times.getValue();

  size_t nCol = times.size();                  // Number of mountage stops
  size_t nRow = pipeStr->contactPoints.size(); // Number of contact points

  FFaMsg::pushStatus("Exporting Wear Data");

  FFuProgressDialog* progDlg = FFuProgressDialog::create("Exporting Wear Data",
							 "Cancel", "Please wait...", nRow + nCol);
  progDlg->setCurrentProgress(0);

  ListUI <<"  -> Exporting wear matrix to "<< fileName
	 <<"\n     Using wear data from period "<< startPeriod <<" to "<< endPeriod <<"\n";

  // Print header of output file

  outputFile <<"Wear data on pipe string joints computed by Fedem. "
             <<"Data is wear energy (F*L) for one heave-period.\n"
             <<"Heave: "<< hiv <<"\n             String Front Depth\n"
             <<"MD_of_joint ";
  for (double depth : pipeStr->stringFrontDepths.getValue())
    outputFile <<" "<< depth;
  outputFile << std::endl;

  // Set up RDB variable search

  FFaResultDescription contactForce("Cam joint",1);
  FFaResultDescription akkContactLength("Cam joint",1);
  FFaResultDescription wearAngle("Cam joint",1);

  contactForce.varDescrPath.push_back("Tx spring variables");
  contactForce.varDescrPath.push_back("Force value");

  akkContactLength.varDescrPath.push_back("Accumulated contact distance");

  wearAngle.varDescrPath.push_back("Ty joint variables");
  wearAngle.varDescrPath.push_back("Length");

  std::vector< std::vector<double> > wearMatrix     (nRow,std::vector<double>(nCol));
  std::vector< std::vector<double> > wearAngleMatrix(nRow,std::vector<double>(nCol));

  std::vector<FFrReadOp<float>*> cfReadOps;
  std::vector<FFrReadOp<float>*> alReadOps;
  std::vector<FFrReadOp<float>*> waReadOps;

  bool hasFatalError = false;
  bool dataMissingError = false;

  // Loop over all pipe contact points
  for (size_t i = 0; i < nRow && !hasFatalError; i++)
    {
      progDlg->setCurrentProgress(i*0.95);

      // Create and store read operations

      contactForce.baseId = pipeStr->contactPoints[i]->getBaseID();
      FFrEntryBase* cfEntry = ex->search(contactForce);
      akkContactLength.baseId = pipeStr->contactPoints[i]->getBaseID();
      FFrEntryBase* alEntry = ex->search(akkContactLength);
      wearAngle.baseId = pipeStr->contactPoints[i]->getBaseID();
      FFrEntryBase* waEntry = ex->search(wearAngle);

      if (cfEntry && alEntry && waEntry && cfEntry->isVarRef() && alEntry->isVarRef() && waEntry->isVarRef())
        {
          FFrVariableReference* cfVarRef = (FFrVariableReference*)cfEntry;
          FFrVariableReference* alVarRef = (FFrVariableReference*)alEntry;
          FFrVariableReference* waVarRef = (FFrVariableReference*)waEntry;

          FFrReadOp<float>* cfReadOp = dynamic_cast<FFrReadOp<float>*>(cfVarRef->getReadOperation());
          cfReadOps.push_back(cfReadOp);
          FFrReadOp<float>* alReadOp = dynamic_cast<FFrReadOp<float>*>(alVarRef->getReadOperation());
          alReadOps.push_back(alReadOp);
          FFrReadOp<float>* waReadOp = dynamic_cast<FFrReadOp<float>*>(waVarRef->getReadOperation());
          waReadOps.push_back(waReadOp);
        }
      else // Could not get all operations. Clean up and Quit I guess...
        hasFatalError = true;
    }

  // Loop over all mountage stops

  for (size_t j = 0; j < nCol && !hasFatalError; j++)
    {
      if (progDlg->userCancelled())
        break;

      for (size_t i = 0; i < nRow; i++)
	wearMatrix[i][j] = wearAngleMatrix[i][j] = 0;

      // Find start and stop for integration
      double tStart = times[j] + period*(startPeriod-1);
      double tEnd   = times[j] + period*endPeriod;

      // Find closest timestep in RDB (find closest before and after and choose)
      double beforeTime = 0, afterTime = 0;
      ex->positionRDB(tStart, beforeTime);
      ex->positionRDB(tStart, afterTime, true);
      bool getNextHigher = fabs(tStart-afterTime) < fabs(tStart-beforeTime);

      // Set the RDB to that time
      double currentTime = -HUGE_VAL;
      if (ex->positionRDB(tStart, currentTime, getNextHigher) && fabs(tStart-currentTime) < 0.01)
        {
          // Integrate the contact force over all time steps in period

          std::vector<float> currentForce(nRow);
          std::vector<float> nextForce(nRow);
          std::vector<float> currentALength(nRow);
          std::vector<float> nextALength(nRow);

          // First initialize at start time of period

          for (size_t i = 0; i < nRow; i++)
            {
              float wearAngleValue;
              waReadOps[i]->evaluate(wearAngleValue);
              wearAngleMatrix[i][j] = wearAngleValue;
              alReadOps[i]->evaluate(currentALength[i]);
              cfReadOps[i]->evaluate(currentForce[i]);
            }

          // Integrate over time period using simple trapezoidal integration

          while (currentTime < tEnd && !hasFatalError)
	    if (!ex->incrementRDB())
	      hasFatalError = true;
	    else if ((currentTime = ex->getCurrentRDBPhysTime()) <= tEnd)
	      {
		for (size_t i = 0; i < nRow; i++)
		  {
		    alReadOps[i]->evaluate(nextALength[i]);
		    cfReadOps[i]->evaluate(nextForce[i]);
		    wearMatrix[i][j] += (nextALength[i] - currentALength[i]) * (currentForce[i] + nextForce[i])/2;
		  }

		currentForce.swap(nextForce);
		currentALength.swap(nextALength);
	      }
        }
      else // Could not find the correct timestep
	dataMissingError = true;

      progDlg->setCurrentProgress((nRow+j)*0.95);
    }

  if (!hasFatalError)
    {
      int i;
      for (i = nRow-1; i >= 0; i--) {
        progDlg->setCurrentProgress(nRow*0.975 - i*0.025);
        outputFile << pipeStr->jointMDPositions.getValue()[i] <<" ";
        for (size_t j = 0; j < nCol; j++)
          outputFile <<" "<< wearMatrix[i][j];
        outputFile << std::endl;
      }
      outputFile <<"WEAR_ANGLES\n";
      for (i = nRow-1; i >= 0; i--) {
        progDlg->setCurrentProgress(nRow - i*0.025);
        outputFile << pipeStr->jointMDPositions.getValue()[i] <<" ";
        for (size_t j = 0; j < nCol; j++)
          outputFile <<" "<< wearAngleMatrix[i][j];
        outputFile << std::endl;
      }
    }

  delete progDlg;

  for (FFrReadOp<float>* op : cfReadOps) op->unref();
  for (FFrReadOp<float>* op : alReadOps) op->unref();
  for (FFrReadOp<float>* op : waReadOps) op->unref();

  FFaMsg::popStatus();

  if (hasFatalError)
    FFaMsg::dialog("Could not export wear data.",FFaMsg::ERROR);

  if (dataMissingError)
    FFaMsg::dialog("Some data could not be exported.");
}


void FapOilWellCmds::getExportPipeWearSensitivity(bool& sensitivity)
{
  sensitivity = FapLicenseManager::hasFeature("FA-OWL");
  if (!sensitivity) return;

  FFaViewItem* obj = FapCmdsBase::findSelectedSingleObject();
  sensitivity = dynamic_cast<FmPipeStringDataExporter*>(obj) ? true : false;
}


/*!
  This function is used to create and hook-up cam-joints to a beamstring,
  acting as a drilling string in a casing.
  It takes as input three triads for a straight master-arc,
  and requires the user to have pre-selected the triads on the beamstring.
*/

void FapOilWellCmds::createDrillString(const std::string& fileName)
{
  if (!FapLicenseManager::checkLicense("FA-OWL"))
    return;

  // Get all selected triads.
  // These are supposed to be the triads on the drilling string.
  std::vector<FmTriad*> selectedTriads;
  if (!FapCmdsBase::getSelected(selectedTriads)) {
    FFaMsg::dialog("No triads selected!", FFaMsg::ERROR);
    Fui::okToGetUserInput();
    return;
  }

  // Sort the triads by id such that the cam joints are placed
  // at correct triad based on the discretization level
  std::map<int,FmTriad*> triadsSorted;
  for (FmTriad* triad : selectedTriads)
    triadsSorted[triad->getID()] = triad;

  Fui::noUserInputPlease();
  ListUI << "===> Reading drillString definition from " << fileName << "\n";
  FFaMsg::pushStatus("Reading drillString definition");

  std::vector<FaVec3> points;
  double jointRadius = 0.0;
  double pipeRadius = 0.0;
  double hydroFricCoeff = 0.0;
  double skinFricCoeff = 0.0;
  double coloumbCoeff = 0.0;
  double coloumbCoeffPipes = 0.0;
  double stribeckSpeed = 0.0;
  double stribeckMagnitude = 0.0;
  double stressFreeLengthConnectors = 0.0;
  double stressFreeLengthPipes = 0.0;
  double radFricCoeff = 0.0;
  double radFricCoeffPipes = 0.0;
  int discretizationLevel = 1;

  // Read file containing positions for master-arc triads, and other parameters
  std::ifstream is(fileName.c_str());

  FaParse::skipWhiteSpaceAndComments(is);
  FaVec3 point1;
  is >> point1[0] >> point1[1] >> point1[2];
  points.push_back(point1);

  FaParse::skipWhiteSpaceAndComments(is);
  FaVec3 point2;
  is >> point2[0] >> point2[1] >> point2[2];
  points.push_back(point2);

  FaParse::skipWhiteSpaceAndComments(is);
  FaVec3 point3;
  is >> point3[0] >> point3[1] >> point3[2];
  points.push_back(point3);

  FaParse::skipWhiteSpaceAndComments(is);
  is >> jointRadius;
  FaParse::skipWhiteSpaceAndComments(is);
  is >> pipeRadius;
  FaParse::skipWhiteSpaceAndComments(is);
  is >> stressFreeLengthConnectors;
  FaParse::skipWhiteSpaceAndComments(is);
  is >> stressFreeLengthPipes;
  FaParse::skipWhiteSpaceAndComments(is);
  is >> hydroFricCoeff;
  FaParse::skipWhiteSpaceAndComments(is);
  is >> coloumbCoeff;
  FaParse::skipWhiteSpaceAndComments(is);
  is >> coloumbCoeffPipes;
  FaParse::skipWhiteSpaceAndComments(is);
  is >> radFricCoeff;
  FaParse::skipWhiteSpaceAndComments(is);
  is >> radFricCoeffPipes;
  FaParse::skipWhiteSpaceAndComments(is);
  is >> stribeckSpeed;
  FaParse::skipWhiteSpaceAndComments(is);
  is >> stribeckMagnitude;
  FaParse::skipWhiteSpaceAndComments(is);
  is >> discretizationLevel;
  FaParse::skipWhiteSpaceAndComments(is);
  is >> skinFricCoeff;

  ListUI << "===> Creating drillString components\n";
  FFaMsg::pushStatus("Creating drillString components");
  // ****Create master-arc segment****
  //Create cam-arc triads
  FmTriad* camArcTriad1 = new FmTriad(points.front());
  FmTriad* camArcTriad2 = new FmTriad(points.at(points.size() / 2));
  FmTriad* camArcTriad3 = new FmTriad(points.back());

  camArcTriad1->setUserDescription("master-arc 1");
  camArcTriad2->setUserDescription("master-arc 2");
  camArcTriad3->setUserDescription("master-arc 3");

  camArcTriad1->connect();
  camArcTriad2->connect();
  camArcTriad3->connect();

  camArcTriad1->draw();
  camArcTriad2->draw();
  camArcTriad3->draw();

  //Create arc
  FmArcSegmentMaster* arcSegmentMaster = new FmArcSegmentMaster();
  arcSegmentMaster->addTriad(camArcTriad1);
  arcSegmentMaster->addTriad(camArcTriad2);
  arcSegmentMaster->addTriad(camArcTriad3);
  arcSegmentMaster->connect();

  // Connect cam-arc to earth
  camArcTriad1->connect(FmDB::getEarthLink());
  camArcTriad2->connect(FmDB::getEarthLink());
  camArcTriad3->connect(FmDB::getEarthLink());

  arcSegmentMaster->draw();

  // Create Cam Friction for connectors
  FmCamFriction* camFriction = new FmCamFriction();
  std::ostringstream strs;
  strs << "Tool joint Friction #PipeRadius " << jointRadius << " #HydroFric " << hydroFricCoeff << " #SkinFric " << skinFricCoeff << " #RadFric " << radFricCoeff;
  camFriction->setCoulombCoeff(coloumbCoeff);
  camFriction->setUserDescription(strs.str());
  camFriction->setStribeckMagn(stribeckMagnitude);
  camFriction->setStribeckSpeed(stribeckSpeed);
  camFriction->connect();

  // Create Cam Friction for pipes
  FmCamFriction* camFrictionPipes = new FmCamFriction();
  std::ostringstream strsPipes;
  strsPipes << "Pipe Friction #PipeRadius " << pipeRadius << " #HydroFric " << hydroFricCoeff << " #SkinFric " << skinFricCoeff << " #RadFric " << radFricCoeffPipes;
  camFrictionPipes->setCoulombCoeff(coloumbCoeffPipes);
  camFrictionPipes->setUserDescription(strsPipes.str());
  camFrictionPipes->setStribeckMagn(stribeckMagnitude);
  camFrictionPipes->setStribeckSpeed(stribeckSpeed);
  camFrictionPipes->connect();

  // Create contact spring
  FmfLinVar* contactSpring = new FmfLinVar();
  contactSpring->setFunctionUse(FmMathFuncBase::SPR_TRA_STIFF);
  contactSpring->setUserDescription("Contact spring characteristics (Stiffness)");
  contactSpring->connect();
  contactSpring->addXYset(0.0,0.0);
  contactSpring->addXYset(0.009,3.0e6);
  contactSpring->setExtrapolationType(1);

  // ****Create cam-joints******

  ListUI << "===> Connecting cam-joints\n";
  FFaMsg::pushStatus("Connecting cam-joints");
  // Create camjoints, and connect them to string-triads and master-arc segment
  for (size_t i = 0; i < triadsSorted.size(); i+=discretizationLevel) {
    //****Connector-joints***
    FmCamJoint* joint = new FmCamJoint();
    joint->setAsSlaveTriad(triadsSorted.find(i+1)->second);

    joint->setThickness(50.0);
    joint->setWidth(0.0);

    joint->getSpringAtDOF(0,true)->setInitLengthOrDefl(stressFreeLengthConnectors);
    joint->getSpringAtDOF(0)->setSpringCharOrStiffFunction(contactSpring);

    joint->setFriction(camFriction);
    joint->setUsingRadialContact(true);
    joint->setMaster(arcSegmentMaster);
    joint->setUserDescription("connector joint");

    joint->connect();
    joint->draw();

    // Create generic parts on triads
    FmPart* genPart = new FmPart(triadsSorted.find(i+1)->second->getGlobalCS().translation());
    genPart->useGenericProperties.setValue(true);
    genPart->connect();
    genPart->setGlobalCS(triadsSorted.find(i+1)->second->getGlobalCS());
    triadsSorted.find(i+1)->second->connect(genPart);

    //****Pipe-joints***
    if(i < (triadsSorted.size() - discretizationLevel) && stressFreeLengthPipes != -1)
      for(int j = 1; j < discretizationLevel; j++){
	FmCamJoint* joint = new FmCamJoint();
	joint->setAsSlaveTriad(triadsSorted.find(i+j+1)->second);

	joint->setThickness(50.0);
	joint->setWidth(0.0);

	joint->getSpringAtDOF(0,true)->setInitLengthOrDefl(stressFreeLengthPipes);
	joint->getSpringAtDOF(0)->setSpringCharOrStiffFunction(contactSpring);

	joint->setFriction(camFrictionPipes);
	joint->setUsingRadialContact(true);
	joint->setMaster(arcSegmentMaster);
	joint->setUserDescription("pipe joint");

	joint->connect();
	joint->draw();
      }
  }

  Fui::okToGetUserInput();
}


struct MatFinder : public std::unary_function<FmMaterialProperty*,bool>
{
  double myRho, myE, myG;
  MatFinder(double Rho, double E, double G) : myRho(Rho), myE(E), myG(G) {}
  bool operator() (FmMaterialProperty* mat) const
  {
    return (mat->Rho.getValue() == myRho &&
	    mat->E.getValue() == myE && mat->G.getValue() == myG);
  }
};

struct PipeFinder : public std::unary_function<FmBeamProperty*,bool>
{
  const FmMaterialProperty* myMat;
  double myDo, myDi, myDd, myDb, myCa, myCm, myCd;
  bool myToggle;
  PipeFinder(const FmMaterialProperty* mat, double Do, double Di)
    : myMat(mat), myDo(Do), myDi(Di), myDd(0.0), myDb(0.0),
      myCa(0.0), myCm(0.0), myCd(0.0), myToggle(false) {}
  PipeFinder(const FmMaterialProperty* mat,
             double Do, double Di, double Dd, double Db,
             double Ca, double Cm, double Cd)
    : myMat(mat), myDo(Do), myDi(Di), myDd(Dd), myDb(Db),
      myCa(Ca), myCm(Cm), myCd(Cd), myToggle(true) {}
  bool operator() (FmBeamProperty* prop) const
  {
    if (prop->crossSectionType.getValue() != FmBeamProperty::PIPE)
      return false;

    if (prop->material.getPointer() != myMat)
      return false;

    if (prop->Do.getValue() != myDo || prop->Di.getValue() != myDi)
      return false;

    if (prop->hydroToggle.getValue() != myToggle)
      return false;

    if (myToggle)
      return (prop->Dd.getValue() == myDd && prop->Db.getValue() == myDb &&
              prop->Ca.getValue() == myCa && prop->Cm.getValue() == myCm &&
              prop->Cd.getValue() == myCd);

    return true;
  }
};


void FapOilWellCmds::createRiser(const std::string& fileName)
{
  if (!FapLicenseManager::checkLicense("FA-WND") &&
      !FapLicenseManager::checkLicense("FA-RIS"))
    return;

  std::ifstream is(fileName.c_str());
  if (!FaParse::skipWhiteSpaceAndComments(is)) return;

  Fui::noUserInputPlease();
  ListUI <<"===> Reading beamstring definition from "<< fileName <<"\n";
  FFaMsg::pushStatus("Reading beamstring definition");

  std::vector<FmMaterialProperty*> riserMats;
  std::vector<FmBeamProperty*>     riserProps;

  FmRiser* riser = new FmRiser();
  riser->setUserDescription(FFaFilePath::getBaseName(fileName,true));
  riser->connect();

  // Initial direction of the riser is the opposite of the gravity direction
  FaVec3 dir = -FmDB::getGrav();
  dir.normalize();

  // Create the bottom triad which should be fixed to ground if it is the first
  FaVec3 bottom;
  is >> bottom[0] >> bottom[1] >> bottom[2];
  FmTriad* triad = new FmTriad(bottom);
  triad->setParentAssembly(riser);
  triad->setUserDescription("Bottom");
  triad->connect();
  bool fixBottom = FmDB::getObjectCount(FmTriad::getClassTypeID()) == 0;

  FaMat33 cs;
  FaVec3 Xaxis(dir);
  int totElms = 0;
  FmBeam* beam = NULL;
  FmBeam* bea2 = NULL;
  FmTriad* tr1 = NULL;
  FmTriad* Top = NULL;
  FmTriad* Bottom = triad;
  FmSMJointBase* sjnt = NULL;

  std::string description(riser->getUserDescription()), keyWord;
  while (is && !is.eof() && std::getline(is,keyWord))
    if (keyWord.substr(0,10) == "Ball Joint" ||
	keyWord.substr(0,10) == "Free Joint") {

      if (triad->isSlaveTriad())
      {
	FFaMsg::dialog("Invalid beamstring definition file."
		       "Chained joints are not allowed.\n"
		       "The generated model in incomplete.",FFaMsg::ERROR);
	triad = NULL;
	break;
      }

      FaVec3 slavePos = triad->getGlobalTranslation();
      if (keyWord.substr(0,10) == "Free Joint") {
	char c = 0; // Check for old-style format with explicit slave position
	if (is.get(c)) {
	  if (c == 'X' || c == 'Y' || c == 'Z')
	    FaParse::skipWhiteSpaceAndComments(is);
	  else if (c == '-' || c == '.' || isdigit(c))
	    is.unget();
	  else
	    c = 0;
        }
	if (c > 0) {
	  double Zpos;
	  is >> Zpos;
	  slavePos = dir*Zpos;
	}
	sjnt = new FmFreeJoint();
      }
      else
	sjnt = new FmBallJoint();
      sjnt->setParentAssembly(riser);

      if (triad == Bottom && fixBottom && !beam)
      {
	// This joint is at the bottom of the beamstring, and no other triads
	// exist yet, so attach its master triad to ground
	triad->connect(FmDB::getEarthLink());
	fixBottom = false;
      }
      else
      {
	triad->connect();
	if (beam)
	  beam->setTriad(triad,1);
      }

      Top = triad;
      triad = new FmTriad(slavePos);
      triad->setParentAssembly(riser);
      sjnt->setAsMasterTriad(Top);
      sjnt->setAsSlaveTriad(triad);
      sjnt->updateLocation();
      triad->connect();
      sjnt->connect();
      sjnt->draw();
      Top->draw();
      Top->onChanged();

      beam = NULL;
      tr1 = NULL;

      // Check for joint DOF status modifier (default is FREE)
      int d = 0;
      std::vector<double> springStiff(6,0.0);
      FmHasDOFsBase::DOFStatus status = FmHasDOFsBase::FREE;
      for (size_t i = 10; i < keyWord.size(); i++)
	if (!isspace(keyWord[i]))
        {
	  if (keyWord.substr(i,5) == "Fixed")
	  {
	    status = FmHasDOFsBase::FIXED;
	    break;
	  }
	  else if (keyWord.substr(i,6) == "Spring")
	  {
	    status = FmHasDOFsBase::SPRING_CONSTRAINED;
	    i += 6;
	  }
	  else if (status == FmHasDOFsBase::SPRING_CONSTRAINED && d < 6)
	  {
	    // Check if a (constant) stiffness is provided. This is optional.
	    springStiff[d++] = atof(keyWord.c_str()+i);
	    if ((i = keyWord.find_first_of(" \t",i)) > keyWord.size()) break;
	  }
	  else
	    break;
        }

      // Set the wanted DOF status (free, fixed or spring-constrained)
      for (d = 0; d < 6; d++)
	if (sjnt->isLegalDOF(d)) {
	  sjnt->setStatusForDOF(d,status);
	  if (status == FmHasDOFsBase::SPRING_CONSTRAINED)
	    sjnt->getSpringAtDOF(d,true)->setInitStiff(springStiff[d]);
	}
    }
    else if (keyWord.substr(0,15) == "Cylindric Joint") {

      if (!tr1 || tr1->isSlaveTriad() || triad->isSlaveTriad())
      {
	FFaMsg::dialog("Invalid beamstring definition file.\nA Cylindric joint "
		       "can only be added after at least two beam elements.\n"
		       "The generated model is incomplete.",FFaMsg::ERROR);
	triad = NULL;
	break;
      }

      double Zpos;
      FaVec3 slavePos;
      FaParse::skipWhiteSpaceAndComments(is);
      is >> Zpos;
      slavePos = dir*Zpos;
      tr1->disconnect();
      Fm1DMaster* mst = new FmStraightMaster();
      mst->setParentAssembly(riser);
      FmCylJoint* jnt = new FmCylJoint();
      jnt->setParentAssembly(riser);
      Top = triad;
      triad = new FmTriad(slavePos);
      triad->setParentAssembly(riser);
      triad->setOrientation(cs.makeGlobalizedCS(Xaxis).shift(-1));
      triad->connect();
      mst->setUserDescription("#Extended");
      mst->connect();
      mst->addTriad(tr1);
      mst->addTriad(Top);
      tr1->setOrientation(cs);
      Top->setOrientation(cs);
      if (bea2)
	bea2->setTriad(tr1,1);
      if (beam)
	beam->setTriads(tr1,Top);
      jnt->setMaster(mst);
      jnt->setAsSlaveTriad(triad);
      jnt->updateLocation();
      jnt->connect();
      jnt->draw();
      tr1->draw();
      Top->draw();
      triad->draw();

      beam = NULL;
      sjnt = NULL;
    }
    else if (keyWord.substr(0,3) == "RKB") {

      double Zpos;
      FaVec3 topPos;
      FaParse::skipWhiteSpaceAndComments(is);
      is >> Zpos;
      topPos = dir*Zpos;
      FmPart* pRKG = new FmPart(topPos);
      pRKG->setParentAssembly(riser);
      pRKG->setUserDescription(keyWord);
      pRKG->useGenericProperties.setValue(true);
      pRKG->connect();
      triad->connect(pRKG);
      triad->draw();

      // The top triad should have its local Z-axis along the g-vector
      triad = new FmTriad(topPos);
      triad->setParentAssembly(riser);
      triad->connect(pRKG);
      if (!dir.isParallell(FaVec3(0.0,0.0,1.0)))
	triad->setOrientation(cs.makeGlobalizedCS(dir).shift(-1));

      // The top triad should use its initial local axes as system directions.
      // This will then be the system prescribed motions are referring to.
      triad->itsLocalDir.setValue(FmTriad::LOCAL_INITIAL);

      // Create prescribed motions for the top triad
      for (int dof = 0; dof < 6; dof++) {
	triad->setDOFStatus(dof,FmHasDOFsBase::PRESCRIBED);
	FmDofMotion* tm = triad->getMotionAtDOF(dof,true);
	if (dof < 3)
	  tm->setUserDescription(std::string("Top ")+char('X'+dof)+std::string("-motion"));
	else
	  tm->setUserDescription(std::string("Top R")+char('U'+dof)+std::string("-motion"));
      }

      if (Top) {
	// Create the top tension load
	FmLink* gnd = FmDB::getEarthLink();
	FmLoad* tensionLoad = new FmLoad();
	tensionLoad->setParentAssembly(riser);
	tensionLoad->setUserDescription("Top tension");
	tensionLoad->connect(Top,gnd,FaVec3(),gnd,dir);
	tensionLoad->draw();
	Top->setUserDescription("Top of riser");
	Top->onChanged();
      }

      pRKG->draw();
      beam = NULL;
      sjnt = NULL;

      break; // The RKB-part must always be at the end
    }
    else if (!keyWord.empty()) {

      // Read beam element property parameters
      double Rho, E, G, Do, Di, Dd, Db, Cd, Cm, Ca, L = 0.0;
      int numElem = 0;

      // Check if we have numeric data on the line just read
      size_t firstAny = keyWord.find_first_not_of(" \t\n");
      size_t firstNum = keyWord.find_first_of("+-.0123456789");
      if (firstAny < keyWord.size() && firstAny == firstNum)
      {
	// Read numeric data from last line
	std::stringstream ss(keyWord);
	ss >> Rho >> E >> G >> Do >> Di >> Dd >> Db >> Cd >> Cm >> Ca;
	if (ss.good()) ss >> L >> numElem;
      }
      else
      {
	// No numeric data, assume it is a description string
	description = keyWord;
	// Ignore any comments and read from input stream instead
	FaParse::skipWhiteSpaceAndComments(is);
	is >> Rho >> E >> G >> Do >> Di >> Dd >> Db >> Cd >> Cm >> Ca;
      }
      double Ro  = 0.5*Do;
      double Ri  = 0.5*Di;
      double Ro2 = Ro*Ro;
      double Ri2 = Ri*Ri;
      double A   = M_PI*(Ro2-Ri2);
      double Iyy = M_PI*(Ro2*Ro2-Ri2*Ri2)*0.25;
      double Ip  = Iyy + Iyy;
      ListUI <<"  -> "<< keyWord <<":"
	     <<"\n\tRho\t= "<< Rho
	     <<" [kg/m^3]\n\tE\t= "<< E
	     <<" [N/m^2]\n\tG\t= "<< G
	     <<" [N/m^2]\n\tDo\t= "<< Do
	     <<" [m]\n\tDi\t= "<< Di
	     <<" [m]\n\tDd\t= "<< Dd
	     <<" [m]\n\tDb\t= "<< Db
	     <<"\n\tCd\t= "<< Cd
	     <<"\n\tCm\t= "<< Cm
	     <<"\n\tCa\t= "<< Ca;
      ListUI <<"\n\tA\t= "<< A
	     <<" [m^2]\n\tIy = Iz\t= "<< Iyy
	     <<" [m^4]\n\tIp\t= "<< Ip
	     <<" [m^4]\n\tAb\t= "<< M_PI*Db*Db*0.25 // Buoyancy area
	     <<" [m^2]\n\tAd\t= "<< M_PI*Dd*Dd*0.25 // Drag area
	     <<" [m^2]\n";

      FmMaterialProperty* elmMat;
      std::vector<FmMaterialProperty*>::const_iterator mit;
      mit = std::find_if(riserMats.begin(),riserMats.end(),MatFinder(Rho,E,G));
      if (mit == riserMats.end())
      {
	// Create a new material property object
	elmMat = new FmMaterialProperty();
	elmMat->setParentAssembly(riser);
	elmMat->setUserDescription(description + " material");
	elmMat->connect();
	elmMat->updateProperties(Rho,E,G,elmMat->nu.getValue());
	riserMats.push_back(elmMat);
      }
      else
	elmMat = *mit;

      FmBeamProperty* elmProp;
      std::vector<FmBeamProperty*>::const_iterator pit;
      pit = std::find_if(riserProps.begin(),riserProps.end(),
                         PipeFinder(elmMat,Do,Di,Dd,Db,Ca,Cm,Cd));
      if (pit == riserProps.end())
      {
        // Create a beam cross section object
        elmProp = new FmBeamProperty();
        elmProp->setParentAssembly(riser);
        elmProp->setUserDescription(description + " cross section");
        elmProp->connect();
        elmProp->material.setRef(elmMat);
        elmProp->crossSectionType.setValue(FmBeamProperty::PIPE);
        elmProp->Do.setValue(Do);
        elmProp->Di.setValue(Di);
        elmProp->Dd.setValue(Dd);
        elmProp->Db.setValue(Db);
        elmProp->Ca.setValue(Ca);
        elmProp->Cm.setValue(Cm);
        elmProp->Cd.setValue(Cd);
        elmProp->hydroToggle.setValue(true);
        elmProp->updateDependentValues();
        riserProps.push_back(elmProp);
      }
      else
        elmProp = *pit;

      if (L == 0.0 && numElem == 0)
      {
	// Now read the coordinates
	FaParse::skipWhiteSpaceAndComments(is);
	is >> L >> numElem;
      }
      if (numElem > 0)
	ListUI <<"\t"<< numElem <<" elements a "<< L/numElem <<" [m]\n";

      // Check if a visualization file is specified next
      std::string vizFile;
      FaParse::skipWhiteSpaceAndComments(is,true);
      char c; is.get(c);
      if (c == '\"') {
	std::getline(is, vizFile, '"');
	if (!vizFile.empty())
	{
	  FmPart* tempPart = new FmPart();
	  FFaFilePath::makeItAbsolute(vizFile,FFaFilePath::getPath(fileName));
	  if (tempPart->setVisualizationFile(vizFile,false))
	    vizFile = FmDB::getMechanismObject()->getRelativePath(vizFile);
	  else
	  {
	    ListUI <<" *** Ignoring invalid visualization file "
		   << vizFile <<"\n";
	    vizFile = "";
	  }
	  tempPart->erase();
	}
      }
      else
	is.unget();

      bottom = triad->getGlobalTranslation();
      FaVec3 top = bottom + dir*L;
      std::vector<FaVec3> pos;
      pos.push_back(bottom);
      for (int e = 1; e < numElem; e++)
	pos.push_back((bottom*(numElem-e) + top*e)/(double)numElem);
      pos.push_back(top);

      // Create the beam elements and triads
      FmBeam* bea1 = beam;
      for (size_t i = 1; i < pos.size(); i++) {
	Xaxis = pos[i] - pos[i-1];
	beam = new FmBeam();
	beam->setParentAssembly(riser);
	beam->setUserDescription(description);
	beam->connect();
	beam->setTriad(triad,0);
	beam->setProperty(elmProp);
	beam->visDataFile = vizFile;
	bea2 = bea1;
	bea1 = beam;
	tr1 = triad;
	triad = new FmTriad(pos[i]);
	triad->setParentAssembly(riser);
	triad->connect();
	beam->setTriad(triad,1);
	beam->draw();
	tr1->draw();
      }
      Top = triad;
      sjnt = NULL;
      totElms += numElem;
    }

  if (fixBottom)
    for (int dof = 0; dof < 6; dof++)
      Bottom->setDOFStatus(dof,FmHasDOFsBase::FIXED);

  // If a single-master joint is at the top, we need to swap slave and master
  // because we then want the last triad on the beamstring to be slave triad
  if (sjnt)
  {
    sjnt->setAsMasterTriad(triad);
    sjnt->setAsSlaveTriad(Top);
    sjnt->updateLocation();
    triad->draw();
    triad->onChanged();
    triad = Top;
  }

  if (triad)
  {
    triad->setUserDescription("Top");
    triad->draw();
    triad->onChanged();
  }

  ListUI <<"     Created "<< totElms <<" beam elements.\n";
  FFaMsg::popStatus();
  Fui::okToGetUserInput();
}


void FapOilWellCmds::createJacket(const FFlLinkHandler* jl, const std::string& name,
				  const double* Morison, int IDoffset)
{
  if (FapLicenseManager::checkLicense("FA-WND") ||
      FapLicenseManager::checkLicense("FA-RIS"))
    FWP::createJacket(jl,name,Morison,IDoffset);
}


void FapOilWellCmds::createPile(const std::string& fileName, const FaVec3& top,
				double mudH, bool interconnectXY, bool doScale,
				int cyclic)
{
  if (!FapLicenseManager::checkLicense("FA-WND") &&
      !FapLicenseManager::checkLicense("FA-RIS"))
    return;

  std::ifstream is(fileName.c_str());
  if (!is) return;

  Fui::noUserInputPlease();
  ListUI <<"===> Reading soil pile definition from "<< fileName <<"\n";
  FFaMsg::pushStatus("Reading soil pile definition");

  std::vector<FmMaterialProperty*> pileMats;
  std::vector<FmBeamProperty*>     pileProps;

  FmSoilPile* spile = new FmSoilPile();
  spile->setUserDescription(FFaFilePath::getBaseName(fileName,true));
  spile->connect();

  // Create the top triad above the mudline
  FmTriad* triad = new FmTriad(top);
  triad->setParentAssembly(spile);
  triad->setUserDescription("Pile top");
  triad->connect();

  // Direction of the pile is in the gravity direction
  FaVec3 dir = FmDB::getGrav();
  dir.normalize();

  FaVec3 mud(top+mudH*dir); // Mudline/pile intersection point

  FaMat33 sprCS; // Pile spring coordinate system
  sprCS.makeGlobalizedCS(-dir).shift(-1); // Local Z-axis in opposite depth direction

  int bottom = 0;
  int nel, totElms = 0;
  double depth = 0.0;
  double depthP = 0.0;
  double segLen = 0.0;
  FmFreeJoint* soilSpr = NULL;
  FmBeamProperty* elmProp = NULL;
  std::vector<std::vector<double>*> lastF;

  // Parse the soil pile definition file
  bool haveReadNextKW = false; std::string keyWord;
  while (haveReadNextKW || FaParse::getKeyword(is,keyWord))
    if (keyWord == "PIPE")
    {
      // Read pile element property parameters
      double Rho, E, G, Do, Di;
      FaParse::skipWhiteSpaceAndComments(is);
      is >> Rho >> E >> G >> Do >> Di;
      double Ro  = 0.5*Do;
      double Ri  = 0.5*Di;
      double Ro2 = Ro*Ro;
      double Ri2 = Ri*Ri;
      double A   = M_PI*(Ro2-Ri2);
      double Iyy = M_PI*(Ro2*Ro2-Ri2*Ri2)*0.25;
      double Ip  = Iyy + Iyy;
      ListUI <<"  -> Pile:"
	     <<"\n\tRho\t= "<< Rho
	     <<" [kg/m^3]\n\tE\t= "<< E
	     <<" [N/m^2]\n\tG\t= "<< G
	     <<" [N/m^2]\n\tDo\t= "<< Do
	     <<" [m]\n\tDi\t= "<< Di
	     <<" [m]\n\tA\t= "<< A
	     <<" [m^2]\n\tIy = Iz\t= "<< Iyy
	     <<" [m^4]\n\tIp\t= "<< Ip
	     <<" [m^4]\n";

      FmMaterialProperty* elmMat;
      std::vector<FmMaterialProperty*>::const_iterator mit;
      mit = std::find_if(pileMats.begin(),pileMats.end(),MatFinder(Rho,E,G));
      if (mit == pileMats.end())
      {
	// Create a new material property object
	elmMat = new FmMaterialProperty();
	elmMat->setParentAssembly(spile);
	elmMat->setUserDescription("Pile material");
	elmMat->connect();
	elmMat->updateProperties(Rho,E,G,elmMat->nu.getValue());
	pileMats.push_back(elmMat);
      }
      else
	elmMat = *mit;

      std::vector<FmBeamProperty*>::const_iterator pit;
      pit = std::find_if(pileProps.begin(),pileProps.end(),PipeFinder(elmMat,Do,Di));
      if (pit == pileProps.end())
      {
        // Create a beam cross section object
        elmProp = new FmBeamProperty();
        elmProp->setParentAssembly(spile);
        elmProp->setUserDescription("Pile cross section");
        elmProp->connect();
        elmProp->material.setRef(elmMat);
        elmProp->crossSectionType.setValue(FmBeamProperty::PIPE);
        elmProp->Do.setValue(Do);
        elmProp->Di.setValue(Di);
        elmProp->updateDependentValues();
        pileProps.push_back(elmProp);
      }
      else
        elmProp = *pit;

      haveReadNextKW = false;
    }
    else if (keyWord == "DEPTH")
    {
      // Read the depth level for the next soil spring
      FaParse::skipWhiteSpaceAndComments(is);
      is >> depth;
      if (bottom)
      {
        FFaMsg::dialog(FFaNumStr("Soil springs at depth %g and below are ignored\n"
                                 "since a bottom spring as already been detected.",depth),
                       FFaMsg::ERROR);
        break;
      }
      else
        ListUI <<"  -> Depth: "<< depth <<" [m]";

      // Check if the number of beam elements is given (default is one)
      if (FaParse::getKeyword(is,keyWord) && keyWord.empty())
      {
        is >> nel;
        if (FaParse::getKeyword(is,keyWord))
          ListUI <<" ("<< nel <<" elements)";
        else
          nel = 1;
      }
      else
        nel = 1;

      haveReadNextKW = true;
      if (depth <= depthP)
      {
	totElms = -1;
	ListUI <<"\n";
	FFaMsg::dialog("Depth levels must be monotonically increasing.\n"
		       "No soil pile generated.",FFaMsg::ERROR);
	break;
      }
      else if (depthP == 0.0)
	depthP = -depth;

      // Create ground triad at current depth level
      FmTriad* gnd = new FmTriad(mud + depth*dir);
      gnd->setParentAssembly(spile);
      gnd->setOrientation(sprCS);
      if (totElms > 0)
	gnd->connect(FmDB::getEarthLink());

      // Create soil pile elements between the two last depth levels
      FmBeam* pile = NULL;
      FmTriad* triad2 = new FmTriad();
      triad2->setParentAssembly(spile);
      triad2->setGlobalCS(gnd->getGlobalCS());
      triad2->connect();
      for (int e = 1; e <= nel; e++) {
        pile = new FmBeam();
	pile->setParentAssembly(spile);
	pile->setUserDescription("Pile");
        pile->connect(triad,triad2);
	if (e < nel) {
	  // Create an intermediate triad (no soil spring here)
	  FmTriad* triad1 = new FmTriad(mud + (depth*e/nel)*dir);
	  triad1->setParentAssembly(spile);
	  triad1->connect();
	  triad->onChanged();
	  triad->draw();
	  pile->setProperty(elmProp);
	  pile->draw();
	  triad = triad1;
	}
      }
      if (totElms == 0)
	gnd->connect(FmDB::getEarthLink());
      triad->onChanged();
      triad->draw();
      pile->setProperty(elmProp);
      pile->draw();
      totElms += nel;

      // Create a soil spring to ground
      soilSpr = new FmFreeJoint();
      soilSpr->setParentAssembly(spile);
      soilSpr->setUserDescription(FFaNumStr("Depth %g",depth));
      soilSpr->setMasterMovedAlong(true);
      soilSpr->setSlaveMovedAlong(true);
      soilSpr->setAsMasterTriad(gnd);
      soilSpr->setAsSlaveTriad(triad2);
      soilSpr->updateLocation();
      soilSpr->connect();
      soilSpr->draw();
      gnd->onChanged();
      gnd->draw();

      if (lastF.empty())
        ListUI <<"\n";
      else
      {
        // Convert the spring/damper force curves (at the previous depth)
        // from [N/m] to [N]. If the next keyword is Q-Z or DQ-Z, we assume
        // it is the bottom depth. The last segment have the scaling 1.0.
        double scale = 0.5*(segLen + depth-depthP);
        if (keyWord=="Q-Z" || keyWord=="DQ-Z")
          scale += 0.5*(depth-depthP);

        ListUI <<" Working length "<< scale
               <<" [m] for soil springs at depth="<< depthP <<"\n";
        for (std::vector<double>* f : lastF)
          for (size_t i = 1; i < f->size(); i += 2)
            (*f)[i] *= scale;

        lastF.clear();
      }

      segLen = depth - depthP;
      depthP = depth;
      triad  = triad2;
    }
    else if (keyWord == "P-Y" || keyWord == "T-Z" ||
             keyWord == "Q-Z" || keyWord == "R-Z")
    {
      bool ok = bottom == 0;
      if (keyWord == "Q-Z")
      {
        ok |= bottom == 2;
        bottom += 1;
      }

      if (!ok)
      {
        FFaMsg::dialog("Only one bottom spring is allowed.\n"
                       "It has to be at the end of the pile.",FFaMsg::ERROR);
        break;
      }

      // Create a soil spring
      keyWord += FFaNumStr(" stiffness at depth=%g",depth);
      char springType = keyWord[0];
      FmfLinVar* sprf = new FmfLinVar();
      sprf->setParentAssembly(spile);
      sprf->setUserDescription(keyWord);
      sprf->setFunctionUse(FmMathFuncBase::SPR_TRA_FORCE);
      sprf->setExtrapolationType(1);
      sprf->connect();

      // Check if the Symmetric keyword is specified
      std::getline(is,keyWord);
      bool symm = FaParse::getKeyword(is,keyWord) && keyWord.substr(0,4) == "SYMM";
      FaParse::skipWhiteSpaceAndComments(is);

      std::vector<FmPair> data;

      // Read the spring data
      double x, fOfX;
      is >> x >> fOfX;
      while (is.good())
      {
        data.push_back(std::make_pair(x,fOfX));
        is >> x >> fOfX;
      }
      is.clear();

      if (data.empty())
      {
        sprf->erase();
        sprf = NULL;
      }
      else
      {
        sprf->connect();
        ListUI <<"  -> "<< sprf->getUserDescription()
               <<" : "<< (int)data.size() <<" points read";

        if (symm && data.front().first >= 0.0)
        {
          ListUI <<" [symmetric]";
          // Make a symmetric spring property
          if (data.front().first == 0.0)
          {
            if (data.front().second != 0.0)
              ListUI <<"\n  ** WARNING: Ignoring point {0.0,"
                     << data.front().second
                     <<"} in symmetric spring stiffness curve.";
            data.erase(data.begin());
          }
          size_t i, nVal = data.size();
          data.insert(data.end(),data.begin(),data.end());
          for (i = 0; i < nVal; i++)
            data[nVal-i-1] = std::make_pair(-data[nVal+i].first,-data[nVal+i].second);
        }
        ListUI <<"\n";

        for (const FmPair& XY : data)
          sprf->addXYset(XY.first,XY.second);

        if (doScale && !bottom)
          lastF.push_back(&(sprf->getData()));
      }

      // Add the stiffness function to the appropriate joint DOF(s)
      int iDof = 2;
      switch (springType) {
      case 'P': iDof = 0; break;
      case 'T': iDof = 2; break;
      case 'R': iDof = 5; break;
      }
      soilSpr->setStatusForDOF(iDof,FmHasDOFsBase::SPRING_CONSTRAINED);
      soilSpr->getSpringAtDOF(iDof,true)->setSpringCharOrStiffFunction(sprf);
      if (iDof == 0)
      {
	soilSpr->setStatusForDOF(1,FmHasDOFsBase::SPRING_CONSTRAINED);
	soilSpr->getSpringAtDOF(1,true)->setSpringCharOrStiffFunction(sprf);
	if (interconnectXY) soilSpr->tranSpringCpl.setValue(FmJointBase::XY);
	if (cyclic)
	{
	  soilSpr->getSpringAtDOF(0)->setUserDescription(FFaNumStr("#Cyclic %d",cyclic));
	  soilSpr->getSpringAtDOF(1)->setUserDescription(FFaNumStr("#Cyclic %d",cyclic));
	}
      }

      haveReadNextKW = false;
    }
    else if (keyWord == "DP-Y" || keyWord == "DT-Z" ||
             keyWord == "DQ-Z" || keyWord == "DR-Z")
    {
      bool ok = bottom == 0;
      if (keyWord == "DQ-Z")
      {
        ok |= bottom == 1;
        bottom += 2;
      }

      if (!ok)
      {
        FFaMsg::dialog("Only one bottom damper is allowed.\n"
                       "It has to be at the end of the pile.",FFaMsg::ERROR);
        break;
      }

      // Create a soil damper
      keyWord += FFaNumStr(" damping at depth=%g",depth);
      char damperType = keyWord[1];
      FmfLinVar* dmpf = new FmfLinVar();
      dmpf->setParentAssembly(spile);
      dmpf->setUserDescription(keyWord);
      dmpf->setFunctionUse(FmMathFuncBase::DA_TRA_FORCE);
      dmpf->setExtrapolationType(1);

      // Check if the Symmetric keyword is specified
      std::getline(is,keyWord);
      bool symm = FaParse::getKeyword(is,keyWord) && keyWord.substr(0,4) == "SYMM";
      FaParse::skipWhiteSpaceAndComments(is);

      std::vector<FmPair> data;

      // Read the damper data
      double x, fOfX;
      is >> x >> fOfX;
      while (is.good())
      {
        data.push_back(std::make_pair(x,fOfX));
        is >> x >> fOfX;
      }
      is.clear();

      if (data.empty())
      {
        dmpf->erase();
        dmpf = NULL;
      }
      else
      {
        dmpf->connect();
        ListUI <<"  -> "<< dmpf->getUserDescription()
               <<" : "<< (int)data.size() <<" points read";

        if (symm && data.front().first >= 0.0)
        {
          ListUI <<" [symmetric]";
          // Make a symmetric damper property
          if (data.front().first == 0.0)
          {
            if (data.front().second != 0.0)
              ListUI <<"\n  ** WARNING: Ignoring point {0.0,"
                     << data.front().second
                     <<"} in symmetric damper curve.";
            data.erase(data.begin());
          }
          size_t nVal = data.size();
          data.insert(data.end(),data.begin(),data.end());
          for (size_t i = 0; i < nVal; i++)
            data[nVal-i-1] = std::make_pair(-data[nVal+i].first,-data[nVal+i].second);
        }
        ListUI <<"\n";

        for (size_t i = 0; i < data.size(); i++)
          dmpf->addXYset(data[i].first,data[i].second);

        if (doScale && !bottom)
          lastF.push_back(&(dmpf->getData()));
      }

      // Add the damper function to the appropriate joint DOF(s)
      int iDof = 2;
      switch (damperType) {
      case 'P': iDof = 0; break;
      case 'T': iDof = 2; break;
      case 'R': iDof = 5; break;
      }
      soilSpr->setStatusForDOF(iDof,FmHasDOFsBase::SPRING_CONSTRAINED);
      soilSpr->getDamperAtDOF(iDof,true)->setFunction(dmpf);
      if (iDof == 0)
      {
	soilSpr->setStatusForDOF(1,FmHasDOFsBase::SPRING_CONSTRAINED);
	soilSpr->getDamperAtDOF(1,true)->setFunction(dmpf);
      }

      haveReadNextKW = false;
    }

  triad->setUserDescription("Pile bottom");
  triad->onChanged();
  triad->draw();

  if (totElms > 0)
    ListUI <<"     Created "<< totElms <<" soil pile elements.\n";
  else
  {
    ListUI <<"     No soil pile created (aborted).\n";
    spile->erase();
    FmBeam* obj = NULL;
    if (elmProp && !elmProp->hasReferringObjs(obj,"myProp"))
    {
      elmProp->material->erase();
      elmProp->erase();
    }
  }

  FFaMsg::popStatus();
  Fui::okToGetUserInput();
}


bool FapOilWellCmds::degradeSoilSprings(double wantedTime)
{
  FFrExtractor* ex = FpRDBExtractorManager::instance()->getModelExtractor();
  if (!ex) return false;

  // Find the degradable soils springs (cyclic springs with secant stiffness)
  std::vector<FmModelMemberBase*> objs;
  std::vector<FmSpringBase*> soilSprings;
  FmDB::getAllOfType(objs,FmSpringBase::getClassTypeID());
  for (FmModelMemberBase* obj : objs)
    if (FFaString(obj->getUserDescription()).getIntAfter("#Cyclic") == 3)
      soilSprings.push_back(static_cast<FmSpringBase*>(obj));

  if (soilSprings.empty()) return false;

  double gotTime = 0.0;
  if (ex->positionRDB(wantedTime,gotTime))
    ListUI <<"===> Degrading "<< (int)soilSprings.size()
	   <<" soil springs with secant stiffness at time="<< gotTime <<".\n";
  else
  {
    ListUI <<"  -> ERROR: Failed to position result database at time "
	   << wantedTime <<".\n";
    return false;
  }

  // Set up RDB variable search
  FFaResultDescription jointSpr("Joint spring",1);
  FFaResultDescription axialSpr("Axial spring",1);
  jointSpr.varDescrPath.push_back("Tx spring variables");
  jointSpr.varDescrPath.push_back("Secant stiffness");
  axialSpr.varDescrPath.push_back("Secant stiffness");

  // Read secant stiffnesses from the results database
  // and update the initial stiffness value for each spring
  FmJointSpring* jntSpr = NULL;
  FmJointBase* joint = NULL;
  int nRead = 0;
  double s0 = 0.0;
  for (FmSpringBase* spr : soilSprings)
  {
    if (!(jntSpr = dynamic_cast<FmJointSpring*>(spr)))
      nRead = 0;
    else if ((joint = jntSpr->getOwnerJoint()))
    {
      int iDof = joint->atWhatDOF(jntSpr);
      jointSpr.baseId = joint->getBaseID();
      jointSpr.varDescrPath[0][0] = iDof < 3 ? 'T' : 'R';
      jointSpr.varDescrPath[0][1] = char('x' + iDof%3);
      nRead = ex->getSingleTimeStepData(ex->search(jointSpr),&s0,1);
    }
    else
    {
      axialSpr.baseId = spr->getBaseID();
      nRead = ex->getSingleTimeStepData(ex->search(axialSpr),&s0,1);
    }
    if (nRead == 1)
    {
      ListUI <<"     "<< spr->getIdString()
	     <<" s0 = "<< s0 <<"\n";
      spr->setInitStiff(s0);
    }
    else
      ListUI <<"  -> ERROR: Failed to read secant stiffness for "
	     << spr->getIdString() <<"\n";
  }

  return true;
}
