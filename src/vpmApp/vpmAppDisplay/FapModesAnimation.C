// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppDisplay/FapAnimationCreator.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmAnimation.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmPart.H"
#ifdef USE_INVENTOR
#include "vpmDisplay/FdPart.H"
#include "vpmDisplay/FdAnimateModel.H"
#include "vpmDisplay/FdFEModel.H"
#endif
#include "vpmPM/FpRDBExtractorManager.H"
#include "FFrLib/FFrExtractor.H"
#include "FFrLib/FFrVariableReference.H"
#include "FFlLib/FFlLinkHandler.H"
#include "FFlLib/FFlVertex.H"
#include "FFlLib/FFlVisualization/FFlGroupPartCreator.H"
#include "FFlLib/FFlVisualization/FFlVisEdge.H"
#include "FFlLib/FFlFEParts/FFlNode.H"
#include "FFaLib/FFaAlgebra/FFaMath.H"
#include "FFaLib/FFaOperation/FFaOperation.H"
#include "FFaLib/FFaDefinitions/FFaResultDescription.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "Admin/FedemAdmin.H"


bool FapAnimationCreator::modesAnimation (FmAnimation* animation,
					  FdAnimateModel* animator,
					  bool& userCancelled)
{
  userCancelled = false;
  if (!animation || !animator)
    return false;
  else if (!animation->isModesAnimation.getValue())
    return false;

  // Get the results extractor
  FFrExtractor* rdb = FpRDBExtractorManager::instance()->getModelExtractor();
  if (!rdb) return false;

  FFaMsg::pushStatus("Loading Animation Data");
  FFaMsg::setSubTask("Initializing");

  // Get the links to be searched for eigenvalue results
  std::vector<FmModelMemberBase*> links;
  int modeType = animation->eigenmodeType.getValue();
  if (modeType == FmAnimation::SYSTEM_MODES)
    links.push_back(FmDB::getMechanismObject());
  else if (animation->eigenmodePart.isNull())
    FmDB::getAllOfType(links,FmLink::getClassTypeID());
  else
    links.push_back(animation->eigenmodePart.getPointer());

  // Get frequency and damping rate for the desired mode,
  // and the time range and time increment for the animation
  double ksi, omega, Tmax = 0.0, Tinc = 0.0;
  int nComp = modesInitAnim(animation,animator,rdb,links,ksi,omega,Tmax,Tinc);
  if (nComp < 1)
  {
#ifdef USE_INVENTOR
    animator->setLegendDescription(std::vector<std::string>());
#endif
    FFaMsg::popStatus();
    FFaMsg::setSubTask("");
    FFaMsg::list("No mode shape results found for " +
		 animation->getIdString(true) + "\n");
    return false;
  }


  // Declare all local variables here to avoid reallocation within the loops
  int n, baseId, vtxId;
  double R, c, s, t;
  FaMat33 linkCS, rotMat;
  FaMat34 linkPos, triRelPos, triPos, curPos;
  std::array<FmTriad*,3>  refTriad;
  std::array<FaMat34,3>   triadPos;
  std::array<FaVec3,3>    offset, point, tra, rot;
  std::array<FaVec3Vec,3> traVec, rotVec;
  if (modeType == FmAnimation::SYSTEM_MODES)
    for (n = 0; n < 3; n++)
    {
      traVec[n].resize(nComp);
      rotVec[n].resize(nComp);
    }

  const int   modeNr = animation->eigenmodeNr.getValue();
  const double scale = animation->eigenmodeAmplitude.getValue();
  const double epssq = 1.0e-8;

  if (modeType == FmAnimation::SYSTEM_MODES)
    FmDB::getAllOfType(links,FmLink::getClassTypeID());

  FFaMsg::setSubTask("Creating Frames");
  FFaMsg::enableProgress(links.size());

  // Loop over all links in the model
  int count = 0;
  for (FmModelMemberBase* obj : links)
  {
    FFaMsg::setProgress(++count);

    FmLink* link = static_cast<FmLink*>(obj);
    if (link->isSuppressed()) continue; // skip suppressed links

    linkPos = link->getGlobalCS();

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Caution: The following code for calculation of triRelPos has to match the
    // equivalent solver code (see updateSupElCorot in supElTypeModule.f90, etc)
    // If this procedure is modified, the other one must be updated accordingly!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    if (modeType == FmAnimation::SYSTEM_MODES)
    {
      // Get co-rotated coordinate system reference triads of this link
      link->getRefPoints (refTriad[0],refTriad[1],refTriad[2],
			  offset[0],  offset[1],  offset[2]);

      // Compute global coordinates of the three reference points and transform
      // the offset vectors, if any, to the associated triad coordinate system
      linkCS = linkPos.direction();
      for (n = 0; n < 3; n++)
      {
	triadPos[n] = refTriad[n]->getGlobalCS();
	if (offset[n].sqrLength() > epssq)
	{
	  offset[n] = linkCS * offset[n];
	  point[n]  = triadPos[n].translation() + offset[n];
	  offset[n] = triadPos[n].direction().transpose() * offset[n];
	}
	else
	  point[n] = triadPos[n].translation();
      }

      // Compute the position of the triangle spanned by the three points
      // relative to the computational link coordinate system
      triPos.makeCS_X_YX(point[0],point[1],point[2]);
      triRelPos = triPos.inverse() * linkPos;
#ifdef FAP_DEBUG
      std::cout <<"\nFapAnimationCreator: Link "<< link->getBaseID()
		<<" triRelPos:"<< triRelPos << std::endl;
#endif
    }


    //////////////////////////////////////////////////////////////////
    // Read dynamic response and mode results for current time step //
    //////////////////////////////////////////////////////////////////

    // Get position matrix of current link
    baseId = obj->getBaseID();
    FmPart* part = dynamic_cast<FmPart*>(obj);
    const char* typeName = part ? "Part" : "Beam";
    getPosition(rdb,typeName,baseId,linkPos);
#ifdef FAP_DEBUG
    std::cout <<"\nFapAnimationCreator: "<< typeName <<" "<< baseId
	      <<" linkPos:"<< linkPos << std::endl;
#endif

    // Get position matrices and eigenvector components of the reference triads
    if (modeType == FmAnimation::SYSTEM_MODES)
      for (n = 0; n < 3; n++)
      {
	baseId = refTriad[n]->getBaseID();
	getPosition(rdb,"Triad",baseId,triadPos[n]);
	getEigenVector(rdb,"Triad",baseId,modeNr,traVec[n],rotVec[n]);
#ifdef FAP_DEBUG
	std::cout <<"\nFapAnimationCreator: Triad "<< baseId << triadPos[n]
		  <<"\nRe(eigVec)="<< traVec[n][0] <<" "<< rotVec[n][0];
	if (nComp == 2)
	  std::cout <<"\nIm(eigVec)="<< traVec[n][1] <<" "<< rotVec[n][1];
	std::cout << std::endl;
#endif
      }


    ///////////////////////////////////////////////////
    // Now create the animation frames for this link //
    ///////////////////////////////////////////////////

    if (part && animation->loadDeformationData.getValue())
    {
      // Animate the expanded eigenmode shape, if present
      std::vector<FaVec3Vec> eigVec(1+nComp);
      if (getEigenVector(rdb,part,modeNr,modeType,eigVec))
      {
        // Expanded mode shape was found
        const VertexVec& vertices = part->getLinkHandler()->getVertexes();
        FaVec3Vec vtxFrame(vertices.size());
#ifdef USE_INVENTOR
        FdPart* fdpart = static_cast<FdPart*>(part->getFdPointer());
        if (fdpart->updateSpecialLines(-1.0))
          fdpart->updateFdDetails(); // Hide local beam system markers during animation
        FdFEModel* visMod = fdpart->getVisualModel();
#endif

#ifdef FAP_DEBUG
	std::cout <<"\nFapAnimationCreator: Creating expanded mode shape frames"
		  << std::endl;
#endif
        for (t = 0.0; t < Tmax+0.5*Tinc; t += Tinc)
        {
#ifdef USE_INVENTOR
          int frameId = animator->addFrame((float)t);
#endif
	  if (nComp == 2)
	  {
	    // Damped modes
	    R = scale*exp(ksi*t);
	    c = R*cos(omega*t);
	    s = R*sin(omega*t);
	    if (modeType == FmAnimation::SYSTEM_MODES)
	      for (n = 0; n < 3; n++)
	      {
		tra[n] = c*traVec[n][0] + s*traVec[n][1];
		rot[n] = c*rotVec[n][0] + s*rotVec[n][1];
	      }
	  }
	  else
	  {
	    // Undamped modes
	    c = scale*sin(omega*t);
            s = 0.0;
	    if (modeType == FmAnimation::SYSTEM_MODES)
	      for (n = 0; n < 3; n++)
	      {
		tra[n] = c*traVec[n][0];
		rot[n] = c*rotVec[n][0];
	      }
	  }

	  if (modeType == FmAnimation::SYSTEM_MODES)
	  {
	    // Compute the link transformation 'curPos' of this frame.
	    // Caution: This has to be the same procedure as in updateSupElCorot
	    for (n = 0; n < 3; n++)
	    {
	      point[n] = triadPos[n].translation() + tra[n];
	      if (offset[n].sqrLength() > epssq)
	      {
		rotMat.incRotate(rot[n]);
		point[n] += rotMat * (triadPos[n].direction() * offset[n]);
	      }
	    }
	    triPos.makeCS_X_YX(point[0],point[1],point[2]);
	    curPos = triPos*triRelPos;

#ifdef USE_INVENTOR
	    // Set frame transformation for this link
	    visMod->setResultTransform(frameId,curPos);
#endif
	  }
#ifdef USE_INVENTOR
	  else
	    // Use the same link transformation in all frames when animating
	    // component modes or free-free modes of the reduced part
	    visMod->setResultTransform(frameId,linkPos);
#endif

          // Compute the vertex deformations for this frame
          for (const FaVec3* vec : vertices)
          {
            vtxId = static_cast<const FFlVertex*>(vec)->getRunningID();
            vtxFrame[vtxId] = eigVec[0][vtxId] + c*eigVec[1][vtxId];
            if (nComp == 2)   vtxFrame[vtxId] += s*eigVec[2][vtxId];
          }

          // Transform vtxFrame from the 'linkPos' system to the 'curPos' system
	  if (modeType == FmAnimation::SYSTEM_MODES)
	  {
	    vtxId = 0;
	    curPos = curPos.inverse()*linkPos;
	    for (FaVec3& dis : vtxFrame)
	    {
	      const FaVec3& x = *vertices[vtxId++];
	      dis = curPos * (x + dis) - x;
	    }
	  }

#ifdef USE_INVENTOR
          // Set frame deformations for this part
          visMod->setResultDeformation(frameId,vtxFrame);
#endif
        }
        continue; // go on with the next part
      }
      else
      {
        FFaMsg::list("No expanded mode shape for Part " +
                     part->baseFTLFile.getValue() + "\n");
        if (modeType != FmAnimation::SYSTEM_MODES)
          continue;
      }
    }

    // No part deformation, only animate rigid body motion for this part
    for (t = 0.0; t < Tmax+0.5*Tinc; t += Tinc)
    {
#ifdef USE_INVENTOR
      int frameId = animator->addFrame((float)t);
#endif
      if (nComp == 2)
      {
        // Damped modes
        R = scale*exp(ksi*t);
        c = R*cos(omega*t);
        s = R*sin(omega*t);
        for (n = 0; n < 3; n++)
        {
          tra[n] = c*traVec[n][0] + s*traVec[n][1];
          rot[n] = c*rotVec[n][0] + s*rotVec[n][1];
        }
      }
      else
      {
        // Undamped modes
        c = scale*sin(omega*t);
        for (n = 0; n < 3; n++)
        {
          tra[n] = c*traVec[n][0];
          rot[n] = c*rotVec[n][0];
        }
      }

      // Compute the link transformation 'curPos' of this frame.
      // Caution: This has to be the same procedure as in updateSupElCorot.
      for (n = 0; n < 3; n++)
      {
        point[n] = triadPos[n].translation() + tra[n];
        if (offset[n].sqrLength() > epssq)
        {
          rotMat.incRotate(rot[n]);
          point[n] += rotMat * (triadPos[n].direction() * offset[n]);
        }
      }
      triPos.makeCS_X_YX(point[0],point[1],point[2]);
      curPos = triPos*triRelPos;

#ifdef USE_INVENTOR
      // Set frame transformation for this link
      FdLink* fdlink = static_cast<FdLink*>(link->getFdPointer());
      fdlink->getVisualModel()->setResultTransform(frameId,curPos);
#endif
    }
  }


  // We are through ...
  rdb->closeFiles();
  FFaMsg::disableProgress();
  FFaMsg::popStatus();
  FFaMsg::setSubTask("");
  return true;
}


int FapAnimationCreator::modesInitAnim (FmAnimation* animation,
#ifdef USE_INVENTOR
                                        FdAnimateModel* animator,
#else
                                        FdAnimateModel*,
#endif
                                        FFrExtractor* rdb,
					const std::vector<FmModelMemberBase*>& objs,
					double& ksi,  double& omega,
					double& Tmax, double& Tinc)
{
  // Get physical time for the system mode animation.
  // It is zero for component mode and free-free mode animations.
  double t, requestedTime = 0.0;
  int modeType = animation->eigenmodeType.getValue();
  if (modeType == FmAnimation::SYSTEM_MODES)
    requestedTime = animation->eigenmodeTime.getValue();

  // Position the results database.
  // Use getNextHigher=true to match the computed time steps.
  if (!rdb->positionRDB(requestedTime,t,true))
    return 0;

#ifdef FAP_DEBUG
  std::cout <<"FapAnimationCreator: Mode shape animation at t="<< t << std::endl;
#endif

  // Get eigenvalue (damping rate and frequency) of the requested mode
  FmLink* foundLink = NULL;
  int modeNr = animation->eigenmodeNr.getValue();
  int nComp = getEigenValue(rdb,foundLink,objs,modeNr,modeType,ksi,omega);
  if (nComp == 1)
    omega = fabs(ksi);   // omega = |ReVal| (angular frequency)
  else if (nComp == 2)
    omega = fabs(omega); // ksi = ReVal (damping rate), omega = |ImVal| (freq.)
  else
    return 0;

  // Get period and requested amplitude of the mode shape
  double Tper = omega > 0.0 ? 2.0*M_PI/omega : 0.0;
  double Amp = animation->eigenmodeAmplitude.getValue();
#ifdef FAP_DEBUG
  if (Tper > 0.0) std::cout <<"FapAnimationCreator: Frequency="<< 1.0/Tper << std::endl;
  if (nComp == 2) std::cout <<"FapAnimationCreator: Damping rate="<< ksi << std::endl;
#endif

  // Define the legend description
  std::vector<std::string> descr;
  descr.push_back(std::string("Fedem ") + FedemAdmin::getVersion() +
		  ": - " + animation->getUserDescription() + " -");

  if (modeType == FmAnimation::SYSTEM_MODES)
    descr.push_back(FFaNumStr("System Mode %d ",modeNr) +
		    FFaNumStr("at time %g",t));
  else if (foundLink)
  {
    if (modeType == FmAnimation::COMPONENT_MODES)
      descr.push_back(FFaNumStr("Component Mode %d of ",modeNr) +
		      foundLink->getIdString());
    else if (modeType == FmAnimation::FREE_FREE_REDUCED)
      descr.push_back(FFaNumStr("Free-free Mode %d of reduced ",modeNr) +
		      foundLink->getIdString());
    else
      descr.push_back(FFaNumStr("Eigenmode %d of non-reduced ",modeNr) +
		      foundLink->getIdString());
  }
  else
  {
    if (modeType == FmAnimation::COMPONENT_MODES)
      descr.push_back(FFaNumStr("Component Mode %d ",modeNr));
    else if (modeType == FmAnimation::FREE_FREE_REDUCED)
      descr.push_back(FFaNumStr("Free-free Mode %d of reduced parts",modeNr));
    else
      descr.push_back(FFaNumStr("Eigenmode %d of non-reduced parts",modeNr));
  }

  if (modeType == FmAnimation::SYSTEM_MODES || foundLink)
  {
    if (Tper > 0.0) descr.push_back(FFaNumStr("Frequency : %g Hz",1.0/Tper));
    if (nComp == 2) descr.push_back(FFaNumStr("Damping rate : %g",ksi));
  }
  if (Amp != 1.0) descr.push_back(FFaNumStr("Scale : %g",Amp));

  // Determine the time-duration of the animation sequence
  if (modeType == FmAnimation::SYSTEM_MODES)
  {
    if (animation->eigenmodeDurationUseTime.getValue())
      Tmax = animation->eigenmodeDurationTime.getValue();

    if (animation->eigenmodeDurationUseNCycles.getValue() && Tper > 0.0)
    {
      t = animation->eigenmodeDurationNCycles.getValue()*Tper;
      if (Tmax <= 0.0 || t < Tmax) Tmax = t;
    }

    if (nComp == 2 && ksi < 0.0)
      if (animation->eigenmodeDurationUseUntilDamped.getValue())
      {
	t = animation->eigenmodeDurationUntilDamped.getValue();
	t = log(1.0-t/100.0)/ksi;
	if (Tmax <= 0.0 || t < Tmax) Tmax = t;
      }
  }
  else
    Tmax = Tper; // always use one full cycle for the undamped part-wise modes

  int nFrame = animation->eigenmodeFramesPrCycle.getValue();
  if (modeType == FmAnimation::FREE_FREE_REDUCED)
    if (omega*nFrame < 4.0*M_PI) // frequency is lower than 2 frames per second
    {
      // This is most likely a zero-frequency rigid body mode, use pseudo time
      // Animate one full cycle lasting 1 sec real time
      Tmax = Tper = 1.0;
      omega = 2.0*M_PI;
      descr.push_back("Using pseudo time");
    }

  // Determine the time increment of the animation sequence
  if (nFrame > 1)
    Tinc = (Tper > 0.0 ? Tper : Tmax) / (double)(nFrame-1);
  else
    std::cerr <<"FapAnimationCreator: Invalid number of frames "
	      << nFrame << std::endl;

#ifdef FAP_DEBUG
  std::cout <<"FapAnimationCreator: Tmax="<< Tmax <<" Tinc="<< Tinc << std::endl;
#endif
  if (Tmax <= 0.0 || Tinc <= 0.0)
    return 0;

#ifdef USE_INVENTOR
  // Update animation legend and progress indicator
  animator->setLegendDescription(descr);
  animator->setProgressIntv(0.0,(float)Tmax);
#endif
  return nComp;
}


/*!
  Static helper function to check that an FFr-entry has data of interest.
*/

static FFrVariableReference* castVarRef (FFrEntryBase* entry)
{
  FFrVariableReference* vRef = dynamic_cast<FFrVariableReference*>(entry);
  if (!vRef)
  {
    std::cerr <<"FapAnimationCreator: Logic error, expected a variable reference";
    if (entry) std::cerr <<", found "<< entry->getDescription();
    std::cerr << std::endl;
    return NULL;
  }

  if (!vRef->hasDataForCurrentKey(true)) // use positioned time step
  {
#if FAP_DEBUG > 1
    std::cerr <<"FapAnimationCreator: Variable \""<< vRef->getDescription()
              <<"\" has no data at the requested time, distance = "
              << vRef->getDistanceFromResultPoint() << std::endl;
#endif
    return NULL;
  }

  return vRef;
}


/*!
  Static template function reading a variable from the results file.
*/

template<class T> static bool readVar (FFrEntryBase* entry, T& value)
{
  FFrVariableReference* vRef = castVarRef(entry);
  if (!vRef) return false;

  bool ok = false;
  FFaOperationBase* readOp = vRef->getReadOperation();
  if (readOp)
  {
    FFaOperation<T>* p = dynamic_cast<FFaOperation<T>*>(readOp);
    if (p) ok = p->evaluate(value);
    readOp->unref();
  }
  if (!ok)
    std::cerr <<" *** FapAnimationCreator: Invalid read operation for \""
              << vRef->getDescription() <<"\"."<< std::endl;
  return ok;
}


/*!
  Template specialization for double variables,
  also accounting for float data on file.
*/

template<> bool readVar (FFrEntryBase* entry, double& value)
{
  FFrVariableReference* vRef = castVarRef(entry);
  if (!vRef) return false;

  bool ok = false;
  FFaOperationBase* readOp = vRef->getReadOperation();
  if (readOp)
  {
    FFaOperation<float>* opf = dynamic_cast<FFaOperation<float>*>(readOp);
    if (opf)
    {
      float floatVal;
      if ((ok = opf->evaluate(floatVal)))
        value = (double)floatVal;
    }
    else
    {
      FFaOperation<double>* opd = dynamic_cast<FFaOperation<double>*>(readOp);
      if (opd) ok = opd->evaluate(value);
    }
    readOp->unref();
  }
  if (!ok)
    std::cerr <<" *** FapAnimationCreator: Invalid read operation for \""
              << vRef->getDescription() <<"\"."<< std::endl;
  return ok;
}


/*!
  Template specialization for arrays of FaVec3 objects.
*/

template<> bool readVar (FFrEntryBase* entry, std::vector<FaVec3>& vec)
{
  FFrVariableReference* vRef = castVarRef(entry);
  if (!vRef) return false;

  bool ok = false;
  FFaOperationBase* readOp = vRef->getReadOperation();
  if (readOp)
  {
    FFaOperation<DoubleVec>* p = dynamic_cast<FFaOperation<DoubleVec>*>(readOp);
    if (p)
    {
      DoubleVec data;
      if ((ok = p->evaluate(data)))
      {
        vec.resize(data.size()/3);
        DoubleVec::const_iterator dit = data.begin();
        for (FaVec3& X : vec)
          for (int i = 0; i < 3; i++, ++dit)
            X[i] = *dit;
      }
    }
    readOp->unref();
  }
  if (!ok)
    std::cerr <<" *** FapAnimationCreator: Invalid read operation for \""
              << vRef->getDescription() <<"\"."<< std::endl;
  return ok;
}


int FapAnimationCreator::getEigenValue (FFrExtractor* rdb, FmLink*& foundLink,
                                        const std::vector<FmModelMemberBase*>& objs,
                                        int modeNr, int modeType,
                                        double& Re, double& Im)
{
  FFaResultDescription entry;
  entry.varRefType = "SCALAR";
  entry.varDescrPath.reserve(3);

  if (modeType == FmAnimation::COMPONENT_MODES)
    entry.varDescrPath.push_back("Component modes");
  else if (modeType == FmAnimation::FREE_FREE_REDUCED)
    entry.varDescrPath.push_back("Free-free reduced modes");
  else if (modeType == FmAnimation::NON_REDUCED)
    entry.varDescrPath.push_back("Eigenmodes");
  entry.varDescrPath.push_back("Eigenvalues");
  entry.varDescrPath.push_back(FFaNumStr("Mode%3d",modeNr));

  // Try read the eigenvalue from all given objects until we find something
  FFrEntryBase* entryPtr = NULL;
  for (FmModelMemberBase* obj : objs)
    if (obj)
    {
      // Skip suppressed links
      if (obj->isOfType(FmLink::getClassTypeID()))
        if (static_cast<FmLink*>(obj)->isSuppressed()) continue;

      entry.baseId = obj->getBaseID();
      entry.OGType = obj->getUITypeName();
      FFrEntryBase* foundPtr = rdb->search(entry);
      if (foundPtr)
      {
        if (obj->isOfType(FmMechanism::getClassTypeID()))
          entryPtr = foundPtr;
        else if (!foundLink)
        {
          entryPtr = foundPtr;
          foundLink = dynamic_cast<FmLink*>(obj);
        }
        else
        {
          foundLink = NULL; // results found in two or more links
          break;
        }
      }
    }

  if (!entryPtr)
  {
#ifdef FAP_DEBUG
    std::cerr <<"FapAnimationCreator: Did not find eigenvalue for mode "
              << modeNr <<":\n"<< std::string(21,' ') << entry << std::endl;
#endif
    return 0;
  }

  bool foundRe = false;
  bool foundIm = false;
  for (FFrEntryBase* field : *entryPtr->getDataFields())
    if (field->getDescription() == "Eigenvalue")
    {
      // This is a real mode, read the eigenvalue
      if (readVar(field,Re))
        return 1;
    }
    else if (field->getDescription() == "Re")
    {
      // This is a complex mode, read the real eigenvalue
      foundRe = readVar(field,Re);
    }
    else if (field->getDescription() == "Im")
    {
      // This is a complex mode, read the imaginary eigenvalue
      foundIm = readVar(field,Im);
    }

  if (foundRe && foundIm)
    return 2;

#ifdef FAP_DEBUG
  std::cerr <<"FapAnimationCreator: No eigenvalue at the requested time"
	    << std::endl;
#endif
  return 0;
}


bool FapAnimationCreator::getPosition (FFrExtractor* rdb,
                                       const char* objType, int baseId,
                                       FaMat34& posMat)
{
  FFaResultDescription entry(objType,baseId);
  entry.varRefType = "TMAT34";
  entry.varDescrPath = { "Position matrix" };

  FFrEntryBase* entryPtr = rdb->search(entry);
  if (!entryPtr)
  {
#ifdef FAP_DEBUG
    std::cerr <<"FapAnimationCreator: Did not find "<< objType
              <<" position: "<< entry << std::endl;
#endif
    return false;
  }

  if (readVar(entryPtr,posMat))
    return true;

#ifdef FAP_DEBUG
  std::cerr <<"FapAnimationCreator: No "<< objType
	    <<" position matrix at the requested time"<< std::endl;
#endif
  return false;
}


bool FapAnimationCreator::getEigenVector (FFrExtractor* rdb,
                                          const char* objType, int baseId,
                                          int modeNr,
					  FaVec3Vec& translation,
					  FaVec3Vec& rotation)
{
  FFaResultDescription entry(objType,baseId);
  entry.varRefType = "VECTOR";
  entry.varDescrPath = { "Eigenvectors", FFaNumStr("Mode%3d",modeNr) };

  FFrEntryBase* entryPtr = rdb->search(entry);
  if (!entryPtr)
  {
#ifdef FAP_DEBUG
    std::cerr <<"FapAnimationCreator: Did not find "<< objType
              <<" eigenvectors: "<< entry << std::endl;
#endif
    return false;
  }

  if (translation.size() == 1)
    return getDeformation(entryPtr,translation[0],rotation[0]);

  bool foundRe = false;
  bool foundIm = false;
  for (FFrEntryBase* field : *entryPtr->getDataFields())
    if (field->getDescription() == "Re")
      foundRe = getDeformation(field,translation[0],rotation[0]);
    else if (field->getDescription() == "Im")
      foundIm = getDeformation(field,translation[1],rotation[1]);

  return foundRe && foundIm;
}


bool FapAnimationCreator::getEigenVector (FFrExtractor* rdb, FmPart* part,
                                          int modeNr, int modeType,
                                          std::vector<FaVec3Vec>& eigVec)
{
  FFlLinkHandler* lh = part->getLinkHandler();
  if (!lh) return false;

  bool isComplex = eigVec.size() > 2;
  FFaNumStr cModeNr("Mode%3d",modeNr);

  FFaResultDescription entry("Part",part->getBaseID());
  entry.varRefType = "VECTOR";
  entry.varDescrPath.reserve(5);

  // First, try to read the whole vector in one operation
  if (modeType == FmAnimation::COMPONENT_MODES)
    entry.varDescrPath.push_back("Component modes");
  else if (modeType == FmAnimation::FREE_FREE_REDUCED)
    entry.varDescrPath.push_back("Free-free reduced modes");
  else if (modeType == FmAnimation::NON_REDUCED)
    entry.varDescrPath.push_back("Eigenmodes");
  entry.varDescrPath.push_back("Vectors");
  entry.varDescrPath.push_back(cModeNr);
  if (isComplex) entry.varDescrPath.push_back("Re");
  entry.varDescrPath.push_back("Translational deformation");

  size_t eVecSize = 0;
  FFrEntryBase* entryPtr = rdb->search(entry);
  if (entryPtr)
  {
    if (!readVar(entryPtr,eigVec[1]))
      return false;
    else if (isComplex)
    {
      entry.varDescrPath[2] = "Im";
      if (!readVar(rdb->search(entry),eigVec[2]))
        return false;
      else if (modeType == FmAnimation::SYSTEM_MODES)
        entry.varDescrPath.erase(entry.varDescrPath.begin()+2);
    }

    if (modeType == FmAnimation::SYSTEM_MODES)
    {
      // Read actual deformations from the dynamic response.
      // The eigenmode shapes will then be superimposed on those.
      entry.varDescrPath[1] = "Dynamic response";
      entryPtr = rdb->search(entry);
      if (!entryPtr)
      {
        eigVec.front().resize(eigVec[1].size());
        std::cout <<"FapAnimationCreator: Did not find FE part deformation:\n"
                  << std::string(21,' ') << entry << std::endl;
      }
      else if (!readVar(entryPtr,eigVec.front()))
        return false;
    }
    else
      eigVec.front().resize(eigVec[1].size());
    eVecSize = eigVec.front().size();

#if FAP_DEBUG > 3
    for (size_t i = 0; i < eVecSize; i++)
    {
      std::cout <<"FE node #"<< i+1 <<":";
      for (const FaVec3Vec& vec : eigVec)
        if (i < vec.size()) std::cout <<"  "<< vec[i];
      std::cout << std::endl;
    }
#endif

    // Check consistency
    for (size_t j = 1; j < eigVec.size(); j++)
      if (eigVec[j].size() != eVecSize)
      {
        std::cerr <<" *** Invalid part eigenvector length "<< eigVec[j].size()
                  <<" (should be "<< eVecSize <<")"<< std::endl;
        return false;
      }
  }

  if (eVecSize == lh->getVertexCount())
    return true; // Assume one-to-one correspondance between vertices and nodes

  // Check if the FE model contains eccentric beams elements.
  // Then we need the rotations too, for calculating beam end deformations
  int needRot = 0;
#ifdef USE_INVENTOR
  FdPart* fdpart = static_cast<FdPart*>(part->getFdPointer());
  if (modeType == FmAnimation::SYSTEM_MODES)
    if (!fdpart->getGroupPartCreator()->getEccEd().empty())
      needRot = 2;
#endif

  if ((int)eVecSize == lh->getNodeCount(FFlLinkHandler::FFL_FEM))
  {
    for (FaVec3Vec& vec : eigVec)
    {
      // Expand from FE nodes to visualization model vertices
      int inod = 0;
      FaVec3Vec vtxVec(lh->getVertexCount());
      for (NodesCIter nit = lh->nodesBegin(); nit != lh->nodesEnd(); ++nit)
        if ((*nit)->hasDOFs())
          vtxVec[(*nit)->getVertexID()] = vec[inod++];
      vec.swap(vtxVec);
    }
    if (!needRot) return true; // No eccentric vertices, we're done
  }
  else
  {
#ifdef FAP_DEBUG
    std::cout <<"FapAnimationCreator: Did not find FE part eigenvector:\n"
              << std::string(21,' ') << entry << std::endl;
#endif
    if (modeType == FmAnimation::SYSTEM_MODES)
      needRot /= 2;
    else
      return false;
  }

  if (needRot == 2)
  {
    // Try to read nodal rotations as whole vectors
    entry.varDescrPath.resize(1);
    entry.varDescrPath.push_back(cModeNr);
    if (isComplex) entry.varDescrPath.push_back("Re");
    entry.varDescrPath.push_back("Angular deformation");
    if ((entryPtr = rdb->search(entry)))
    {
      std::vector<FaVec3Vec> rotVec(eigVec.size());
      if (!readVar(entryPtr,rotVec[1]))
        return false;
      else if (isComplex)
      {
        entry.varDescrPath[2] = "Im";
        if (!readVar(rdb->search(entry),rotVec[2]))
          return false;
        else
          entry.varDescrPath.erase(entry.varDescrPath.begin()+2);
      }
      entry.varDescrPath[1] = "Dynamic response";
      entryPtr = rdb->search(entry);
      if (!entryPtr)
      {
        rotVec.front().resize(rotVec[1].size());
        std::cout <<"FapAnimationCreator: Did not find FE part deformation:\n"
                  << std::string(21,' ') << entry << std::endl;
      }
      else if (!readVar(entryPtr,rotVec.front()))
        return false;

      size_t nRot = 0;
      std::map<int,size_t> rotMap;
      for (NodesCIter nit = lh->nodesBegin(); nit != lh->nodesEnd(); ++nit)
        if ((*nit)->hasDOFs(6)) rotMap[(*nit)->getID()] = nRot++;

      if (rotVec.front().size() == nRot)
      {
#ifdef USE_INVENTOR
        FaMat33 rotMat;
        FFlNode* node = NULL;
        std::map<int,size_t>::const_iterator it = rotMap.end();
        for (FFlVisEdge* eccEdge : fdpart->getGroupPartCreator()->getEccEd())
          if ((node = eccEdge->getFirstVertex()->getNode()))
            if ((it = rotMap.find(node->getID())) != rotMap.end())
            {
              // Calculate deformations of the eccentric vertex
              FaVec3 ecc = eccEdge->getVector();
              int vtx1Id = eccEdge->getFirstVxIdx();
              int vtx2Id = eccEdge->getSecondVxIdx();
              for (size_t i = 0; i < rotVec.size(); i++)
                if (it->second < rotVec[i].size())
                {
                  rotMat.incRotate(rotVec[i][it->second]);
                  eigVec[i][vtx2Id] = eigVec[i][vtx1Id] + rotMat*ecc - ecc;
                }
#if FAP_DEBUG > 3
              std::cout <<"Vertex "<< vtx2Id <<" (Node "<< node->getID() <<"):";
              for (const FaVec3Vec& v : eigVec) std::cout <<"  "<< v[vtx2Id];
              std::cout << std::endl;
#endif
            }
#endif
        return true;
      }
    }
  }

  // Lambda function for reading nodal eigenvectors and deformations.
  auto&& readNodalDef = [&entry,rdb,cModeNr](int nodeId, FaVec3& def,
                                             FaVec3* ReVec, FaVec3* ImVec,
                                             bool rotations = false)
  {
    entry.varDescrPath[1] = FFaNumStr(nodeId);
    entry.varDescrPath[2] = cModeNr;
    FFrEntryBase* ffrEptr = rdb->search(entry);
    if (!ffrEptr)
    {
#ifdef FAP_DEBUG
      std::cerr <<" *** Could not find expanded eigenvector component:\n"
                << std::string(18,' ') << entry << std::endl;
#endif
      return false;
    }

    if (ReVec && ImVec)
    {
      bool foundRe = false;
      bool foundIm = false;
      for (FFrEntryBase* field : *ffrEptr->getDataFields())
        if (field->getDescription() == "Re")
          foundRe = getDeformation(field,*ReVec,rotations);
        else if (field->getDescription() == "Im")
          foundIm = getDeformation(field,*ImVec,rotations);
      if (!(foundRe && foundIm))
        return false;
    }
    else if (ReVec)
      if (!getDeformation(ffrEptr,*ReVec,rotations))
        return false;

    entry.varDescrPath[2] = "Dynamic response";
    ffrEptr = rdb->search(entry);
    if (ffrEptr)
      return getDeformation(ffrEptr,def,rotations);

    std::cout <<"FapAnimationCreator: Did not find FE part deformation:\n"
              << std::string(21,' ') << entry << std::endl;
    return true;
  };

#ifdef FAP_DEBUG
  std::cout <<"FapAnimationCreator: Trying slow method ..."<< std::endl;
#endif
  bool ok = true;
  entry.varRefType = "VEC3";
  entry.varDescrPath[0] = "Nodes";
  if (eVecSize == 0)
  {
    // Read eigenvector translations, node by node
    for (FaVec3Vec& v : eigVec) v.resize(lh->getVertexCount());
    for (NodesCIter nit = lh->nodesBegin(); nit != lh->nodesEnd() && ok; ++nit)
      if ((*nit)->hasDOFs())
      {
        int nId  = (*nit)->getID();
        int vxId = (*nit)->getVertexID();
        FaVec3* ImVec = isComplex ? &eigVec[2][vxId] : NULL;
        ok = readNodalDef(nId,eigVec.front()[vxId],&eigVec[1][vxId],ImVec);
#if FAP_DEBUG > 3
        std::cout <<"Vertex "<< vxId <<" (FE node "<< nId <<"):";
        for (const FaVec3Vec& vec : eigVec) std::cout <<"  "<< vec[vxId];
        std::cout << std::endl;
#endif
      }
    if (!ok) return false;
  }

  // Check if the FE model contains eccentric beams elements.
  // Then we need the rotations too, for calculating beam end deformations
  if (!needRot) return true;

#ifdef USE_INVENTOR
  entry.varRefType = "ROT3";
  FaMat33 rotMat;
  FFlNode* node = NULL;
  FaVec3Vec rotVec(eigVec.size());
  FaVec3* ImVec = isComplex ? &rotVec[2] : NULL;
  for (FFlVisEdge* eccEdge : fdpart->getGroupPartCreator()->getEccEd())
    if ((node = eccEdge->getFirstVertex()->getNode()))
      if (readNodalDef(node->getID(),rotVec.front(),&rotVec[1],ImVec,true))
      {
        // Calculate deformations of the eccentric vertex
        FaVec3 ecc = eccEdge->getVector();
        int vtx1Id = eccEdge->getFirstVxIdx();
        int vtx2Id = eccEdge->getSecondVxIdx();
        for (size_t i = 0; i < rotVec.size(); i++)
        {
          rotMat.incRotate(rotVec[i]);
          eigVec[i][vtx2Id] = eigVec[i][vtx1Id] + rotMat*ecc - ecc;
        }
#if FAP_DEBUG > 3
        std::cout <<"Vertex "<< vtx2Id <<" (FE node "<< node->getID() <<"):";
        for (const FaVec3Vec& v : eigVec) std::cout <<"  "<< v[vtx2Id];
        std::cout << std::endl;
#endif
      }
#endif

  return true;
}


bool FapAnimationCreator::getDeformation (const FFrEntryBase* itemGroup,
					  FaVec3& translation, FaVec3& rotation)
{
  bool foundTra = false;
  bool foundRot = false;
  for (FFrEntryBase* field : *itemGroup->getDataFields())
    if (field->getDescription() == "Translational deformation")
      foundTra = readVar(field,translation);
    else if (field->getDescription() == "Angular deformation")
      foundRot = readVar(field,rotation);

#ifdef FAP_DEBUG
  if (!foundTra)
    std::cerr <<"FapAnimationCreator: Did not find \"Translational deformation\""
	      <<" within item group \""<< itemGroup->getDescription()
	      <<"\" at the requested time"<< std::endl;

  if (!foundRot)
    std::cerr <<"FapAnimationCreator: Did not find \"Angular deformation\""
	      <<" within item group \""<< itemGroup->getDescription()
	      <<"\" at the requested time"<< std::endl;
#endif

  return foundTra && foundRot;
}


bool FapAnimationCreator::getDeformation (const FFrEntryBase* itemGroup,
                                          FaVec3& deformation, bool rotation)
{
  const char* tag = rotation ? "Angular deformation" : "Translational deformation";
  for (FFrEntryBase* field : *itemGroup->getDataFields())
    if (field->getDescription() == tag)
    {
      if (readVar(field,deformation))
	return true;
      else
	break;
    }

#ifdef FAP_DEBUG
  std::cerr <<"FapAnimationCreator: Did not find \""<< tag <<"\""
	    <<" within item group \""<< itemGroup->getDescription()
	    <<"\" at the requested time"<< std::endl;
#endif

  return false;
}


bool FapAnimationCreator::readMatrix (FFrEntryBase* entry, FaMat34& mat)
{
  return readVar(entry,mat);
}
