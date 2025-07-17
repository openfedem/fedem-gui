// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppDisplay/FapAnimationCreator.H"
#include "vpmApp/vpmAppDisplay/FFaLegendMapper.H"
#include "vpmApp/vpmAppDisplay/FapVTFFile.H"
#include "vpmApp/vpmAppProcess/FapSimEventHandler.H"
#include "FFlrLib/FapFringeSetup.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmBeam.H"
#include "vpmDB/FmAnimation.H"
#include "vpmDB/FmAnalysis.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmElementGroupProxy.H"
#include "vpmDB/FmGlobalViewSettings.H"

#ifdef USE_INVENTOR
#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdPart.H"
#include "vpmDisplay/FdTriad.H"
#include "vpmDisplay/FdFEModel.H"
#include "vpmDisplay/FdFEGroupPart.H"
#include "vpmDisplay/FdAnimateModel.H"
#endif

#include "vpmPM/FpRDBExtractorManager.H"
#include "vpmPM/FpModelRDBHandler.H"

#include "FFlrLib/FFlrFringeCreator.H"
#include "FFlrLib/FFlrResultResolver.H"
#include "FFlrLib/FFlrFEResult.H"
#include "FFlLib/FFlVisualization/FFlGroupPartCreator.H"
#include "FFlLib/FFlLinkHandler.H"
#include "FFlLib/FFlGroup.H"
#include "FFlLib/FFlElementBase.H"
#include "FFlLib/FFlFEParts/FFlNode.H"
#include "FFrLib/FFrExtractor.H"
#include "FFuLib/FFuProgressDialog.H"
#include "FFaLib/FFaOperation/FFaOperation.H"
#include "FFaLib/FFaOperation/FFaOpUtils.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "FFaLib/FFaDefinitions/FFaAppInfo.H"

#ifdef FT_USE_PROFILER
#include "FFaLib/FFaProfiler/FFaProfiler.H"
#endif

#include <functional>


/*!
  \brief An operation for calculating deformations at eccentric nodes.
*/

class FapEccTransform : public FFaOperation<FaVec3>
{
public:
  FapEccTransform(FFaOperation<FaVec3>* traOp,
                  FFaOperation<FaVec3>* rotOp, const FaVec3& eccVec)
    : tOp(traOp), rOp(rotOp), ecc(eccVec)
  {
    tOp->ref();
    rOp->ref();
  }

  virtual void invalidate()
  {
    if (!this->IAmEvaluated) return;

    tOp->invalidate();
    rOp->invalidate();
    this->IAmEvaluated = false;
  }

  virtual bool hasData() const { return tOp->hasData() && rOp->hasData(); }

  virtual bool evaluate(FaVec3& value)
  {
    FaVec3 tra, rot;
    if (!tOp->evaluate(tra) || !rOp->evaluate(rot))
      return false; // read failure

    // Calculate the eccentric deformation
    FaMat33 rotMat;
    rotMat.incRotate(rot);
    value = tra + rotMat*ecc - ecc;
    return true;
  }

protected:
  virtual ~FapEccTransform()
  {
    tOp->unref();
    rOp->unref();
  }

private:
  FFaOperation<FaVec3>* tOp;
  FFaOperation<FaVec3>* rOp;
  FaVec3                ecc;

#ifdef FT_USE_MEMPOOL
  FFA_MAKE_MEMPOOL;
#endif
};

#ifdef FT_USE_MEMPOOL
FFaMemPool FapEccTransform::ourMemPool = FFaMemPool(sizeof(FapEccTransform),
                                                    FFaOperationBase::getMemPoolMgr());
#endif


FapAnimationCreator::FapAnimationCreator()
{
#ifdef FAP_DEBUG
  std::cout <<"\n"<< std::string(80,'=')
            <<"\nFapAnimationCreator() constructor"<< std::endl;
#endif

  myAnimator  = NULL;
  myExtractor = NULL;
  myStartTime = 0.0;
  myStopTime  = 1.0;
  myMinDeltaT = 1.0e-12;

  IHaveOneColorPrFace = false;
  IAmUsingMostFrames = false;
  IAmLoadingDeformData = false;
  IAmLoadingFringeData = false;
  IAmSummaryAnimation = false;

  myLastReadTime   = -HUGE_VAL;
  myMaxFringeValue = -HUGE_VAL;
  myMinFringeValue =  HUGE_VAL;

  mySpecialValue = 1.0e20;
  mySpValConvertValue = mySpecialValue;

  IHaveInitedAllPosMxReading = false;

#ifdef FT_USE_PROFILER
  myProfiler = new FFaProfiler("Animation");
#endif
}


FapAnimationCreator::~FapAnimationCreator()
{
#ifdef FAP_DEBUG
  std::cout <<"\n"<< std::string(80,'=')
            <<"\n~FapAnimationCreator() destructor"<< std::endl;
#endif
#ifdef FT_USE_PROFILER
  delete myProfiler;
#endif
  FapAnimationCreator::finishAllPosMxReading();
#ifdef USE_INVENTOR
  FdDB::setFEBeamSysScale(FmDB::getActiveViewSettings()->getSymbolScale());
#endif
}


bool FapAnimationCreator::initReading(FmAnimation* animation)
{
  if (!animation || animation->isModesAnimation.getValue()) return false;

  // Get extractor:
  myExtractor = FpRDBExtractorManager::instance()->getModelExtractor();
  if (!myExtractor) return false;

#ifdef FAP_DEBUG
  std::cout <<"\nFapAnimationCreator::initReading("
            << animation->getIdString(true) <<")"<< std::endl;
#endif
#ifdef FT_USE_PROFILER
  FFaMemoryProfiler::reportMemoryUsage("Anim Init Start");
  myProfiler->startTimer("Init");
#endif

  // Reset the extractor:
  myExtractor->resetRDBPositioning();

  // Setting up control variables:
  IAmSummaryAnimation  = animation->isSummaryAnimation.getValue();
  IAmUsingMostFrames   = animation->makeFrameForMostFrequentResult.getValue();
  IAmLoadingDeformData = animation->loadDeformationData.getValue();
  IAmLoadingFringeData = animation->loadFringeData.getValue();
  if (animation->loadLineFringeData.getValue())
    IAmLoadingFringeData += 2;

  // Setting up animation time window:
  if (animation->usingTimeInterval.getValue())
  {
    myStartTime = animation->timeRange.getValue().first;
    myStopTime  = animation->timeRange.getValue().second;
  }
  else
  {
    myStartTime = FmDB::getActiveAnalysis()->startTime.getValue();
    myStopTime  = FmDB::getActiveAnalysis()->stopTime.getValue();
  }
  myMinDeltaT = FmDB::getActiveAnalysis()->minTimeIncr.getValue();

  // Initialize the animation progress bar:
#ifdef USE_INVENTOR
  if (myAnimator)
    myAnimator->setProgressIntv(myStartTime,myStopTime);
#endif
#ifdef FAP_DEBUG
  std::cout <<"\tUsing time window ["<< myStartTime <<","<< myStopTime <<"]"<< std::endl;
#endif

  // Get all links that may have results, optionally skip all blade elements
  bool skipBlades = animation->getUserDescription().find("#skipBlade") != std::string::npos;
  std::function<bool(FmLink*)> filterBlade = [skipBlades](FmLink* link) -> bool
  {
    if (link->isSuppressed())
      return true;
    else if (!skipBlades)
      return false;

    FmBeam* beam = dynamic_cast<FmBeam*>(link);
    return beam ? beam->getBladeProperty() != NULL : false;
  };

  myParts.clear();
  FmDB::getAllLinks(myLinks);
  for (size_t i = 0; i < myLinks.size();)
    if (filterBlade(myLinks[i]))
      myLinks.erase(myLinks.begin()+i);
    else if (myLinks[i]->isFEPart())
      myParts.push_back(static_cast<FmPart*>(myLinks[i++]));
    else
      i++;

  // Get all triads that may have results:
  FmDB::getAllTriads(myTriads);
  for (size_t i = 0; i < myTriads.size();)
    if (myTriads[i]->getOwnerLink(0))
      myTriads.erase(myTriads.begin()+i);
    else
      i++;

#ifdef FAP_DEBUG
  std::cout <<"\nFapAnimationCreator::initReading: Links with results:";
  for (FmLink* link : myLinks) std::cout <<"\n\t"<< link->getIdString(true);
  std::cout <<"\nFapAnimationCreator::initReading: Triads with results:";
  for (FmTriad* triad : myTriads) std::cout <<"\n\t"<< triad->getIdString(true);
  std::cout << std::endl;
#endif
#ifdef FT_USE_PROFILER
  myProfiler->stopTimer("Init");
#endif
  return true;
}


double FapAnimationCreator::initRDB(DoubleSet& timeSteps,
				    DoubleSet::const_iterator& startIt)
{
#ifdef FAP_DEBUG
  std::cout <<"\nFapAnimationCreator::initRDB()"<< std::endl;
#endif
  // Enable time step cache
  FmResultStatusData* rsd = FapSimEventHandler::getActiveRSD();
  if (IAmLoadingDeformData || IAmLoadingFringeData) {
    if (IAmSummaryAnimation)
      FpModelRDBHandler::enableTimeStepPreRead(rsd,"summary_rcy");
    else
      FpModelRDBHandler::enableTimeStepPreRead(rsd,"timehist_rcy");
  }
  // Find the time steps to load (all or just some)
  bool useTimeSet = true;
  if (IAmSummaryAnimation)
    FpModelRDBHandler::getKeys(rsd,timeSteps,"summary_rcy");
  else if ((IAmLoadingDeformData || IAmLoadingFringeData) && !IAmUsingMostFrames)
    FpModelRDBHandler::getKeys(rsd,timeSteps,"timehist_rcy");
  else
    useTimeSet = false;

  if (!timeSteps.empty())
  {
#ifdef FAP_DEBUG
    std::cout <<"Valid data times: "<< timeSteps.size();
    size_t istep = 0;
    for (double t : timeSteps)
      std::cout << ((++istep)%10 == 1 ? "\n" : " ") << t;
    std::cout << std::endl;
#endif

    // Find the time to start reading from
    if (IAmSummaryAnimation)
      startIt = timeSteps.begin(); // always use whole time domain
    else
      startIt = timeSteps.lower_bound(myStartTime);

    if (startIt == timeSteps.end())
      return HUGE_VAL;
    else
      myStartTime = *startIt;
  }
  else if (useTimeSet)
    return HUGE_VAL;

  // Position the RDB to desired start position
  double gottenTime = -100.0;
  if (!myExtractor->positionRDB(myStartTime,gottenTime))
    return HUGE_VAL;

  // Check whether to stop reading at last solved timestep
  if (IAmSummaryAnimation)
    myStopTime = *(timeSteps.rbegin()); // always use whole time domain
  else {
    double newDataEndTime = myExtractor->getLastWrittenTime();
    if (newDataEndTime < myStopTime && newDataEndTime != -HUGE_VAL)
      myStopTime = newDataEndTime;
  }
#ifdef FAP_DEBUG
  std::cout <<"\tUsing time window ["<< gottenTime <<","<< myStopTime <<"]"<< std::endl;
#endif

  return gottenTime;
}


double FapAnimationCreator::incrementRDB(const DoubleSet& timeSteps,
					 DoubleSet::const_iterator& it)
{
  double gottenTime = HUGE_VAL;
  if (timeSteps.empty())
  {
    // Using all time steps
    if (myExtractor->incrementRDB())
      gottenTime = myExtractor->getCurrentRDBPhysTime();
  }
  else
  {
    // Using a given time range
    if (++it != timeSteps.end())
      myExtractor->positionRDB(*it,gottenTime);
  }

  return gottenTime;
}


/*!
  Initialize the reading used when progress animating.
*/

void FapAnimationCreator::initAllPosMxReading(FmAnimation* animation,
                                              FdAnimateModel* animator)
{
#ifdef FAP_DEBUG
  std::cout <<"\n"<< std::string(80,'=')
            <<"\nFapAnimationCreator::initAllPosMxReading() "
            << std::boolalpha << IHaveInitedAllPosMxReading << std::endl;
#endif
  if (IHaveInitedAllPosMxReading)
    return;

  myAnimator = animator;

  if (!this->initReading(animation))
    return;

  myLastReadTime = myStartTime;

#ifdef FT_USE_PROFILER
  myProfiler->startTimer("AllPosMx Init");
#endif

  for (FmLink* link : myLinks)
    FapAnimationCreator::initPosMxReading(link,myExtractor);

  for (FmTriad* triad : myTriads)
    FapAnimationCreator::initPosMxReading(triad,myExtractor);

  IHaveInitedAllPosMxReading = true;

#ifdef FT_USE_PROFILER
  myProfiler->stopTimer("AllPosMx Init");
#endif
}


/*!
  Read method used for animation during solving.
*/

void FapAnimationCreator::readAllNewPosMx(FmAnimation* animation)
{
  // Position RDB to last read position

  double gottenTime = HUGE_VAL;
  double wantTime = myLastReadTime < myStartTime ? myStartTime : myLastReadTime;
#ifdef FAP_DEBUG
  std::cout <<"\n"<< std::string(80,'=')
            <<"\nFapAnimationCreator::readAllNewPosMx(t="<< wantTime <<")\n";
#endif
  if (myExtractor->positionRDB(wantTime,gottenTime) && myLastReadTime != myStartTime)
  {
    if (myExtractor->incrementRDB())
      gottenTime = myExtractor->getCurrentRDBPhysTime();
    else
    {
#ifdef FAP_DEBUG
      std::cout <<"End of time "<< gottenTime << std::endl;
#endif
      return;
    }
  }

  // Check whether to stop reading at last solved timestep

  double newDataEndTime = myExtractor->getLastWrittenTime();
  if (newDataEndTime == -HUGE_VAL)
  {
#ifdef FAP_DEBUG
    std::cout <<"No new results."<< std::endl;
#endif
    return;
  }

  double stopTime = newDataEndTime < myStopTime ? newDataEndTime : myStopTime;
  double prevLastReadTime = myLastReadTime;
#ifdef FAP_DEBUG
  std::cout <<"New results found."
            <<"\nmyStartTime: "<< myStartTime
            <<"\nmyLastReadTime: "<< myLastReadTime
            <<"\nLastWrittenTime: "<< newDataEndTime
            <<"\ngottenTime: "<< gottenTime
            <<"\nstopTime: "<< stopTime << std::endl;
#endif

  // Reading loop :

  stopTime += myMinDeltaT;
  while (gottenTime < stopTime)
  {
#ifdef USE_INVENTOR
    // Add animator frame
    int frameIdx = myAnimator->addFrame(gottenTime);
#ifdef FAP_DEBUG
    std::cout <<"Reading frame "<< frameIdx <<" at time "<< gottenTime << std::endl;
#endif

    // Actually load results
    bool gotAllData = true;
    for (FmLink* link : myLinks)
      gotAllData &= FapAnimationCreator::readPosMx(frameIdx,link);

    for (FmTriad* triad : myTriads)
      gotAllData &= FapAnimationCreator::readPosMx(frameIdx,triad);

    // If any of the links or triads lack data, we need to start from
    // this time step again, so exit the read loop without storing gottenTime
    if (!gotAllData) break;
#endif

    // Store the time when we should start reading next time
    myLastReadTime = gottenTime;

    // Set RDB to next time step to be loaded
    if (myExtractor->incrementRDB())
      gottenTime = myExtractor->getCurrentRDBPhysTime();
    else
      break;
  }

  if ((IAmLoadingFringeData || IAmLoadingDeformData) && myLastReadTime > prevLastReadTime)
    for (FmPart* part : myParts)
      if (part->isFELoaded())
      {
        // Reposition the RDB to the first time step to read
        if (myExtractor->positionRDB(wantTime,gottenTime)) {
          if (myExtractor->incrementRDB())
            gottenTime = myExtractor->getCurrentRDBPhysTime();
          else
            break;
        }

        // Load deformation and fringes for this part
        if (IAmLoadingFringeData)
        {
          FapAnimationCreator::initFringeReading(part,myExtractor,animation);
#ifdef FT_USE_MEMPOOL
          FFlFEElmResult::freePool();
          FFlFENodeResult::freePool();
#endif
        }
        if (IAmLoadingDeformData)
          FapAnimationCreator::initDeformationReading(part,myExtractor);

        while (gottenTime <= myLastReadTime)
        {
#ifdef USE_INVENTOR
          int frameIdx = myAnimator->addFrame(gottenTime);

          if (IAmLoadingFringeData)
            FapAnimationCreator::readFringeData(frameIdx,part,
                                                myAnimator->getLegendMapping());
          if (IAmLoadingDeformData)
            FapAnimationCreator::readDeformations(frameIdx,part);
#endif
          if (myExtractor->incrementRDB())
            gottenTime = myExtractor->getCurrentRDBPhysTime();
          else
            break;
        }

        if (IAmLoadingFringeData)
          FapAnimationCreator::finishFringeReading(part);

        if (IAmLoadingDeformData)
          FapAnimationCreator::finishDeformationReading(part);
      }

  // If the user has turned on progress animation, move to the new frame
#ifdef FAP_DEBUG
  std::cout <<"Animate to "<< myLastReadTime << std::endl;
#endif
#ifdef USE_INVENTOR
  myAnimator->moveToTime(myLastReadTime,true);
#endif
}


void FapAnimationCreator::finishAllPosMxReading()
{
#ifdef FAP_DEBUG
  std::cout <<"\n"<< std::string(80,'=')
            <<"\nFapAnimationCreator::finishAllPosMxReading() "
            << std::boolalpha << IHaveInitedAllPosMxReading << std::endl;
#endif
  if (!IHaveInitedAllPosMxReading)
    return;

  for (FmLink* link : myLinks)
    FapAnimationCreator::finishPosMxReading(link);

  for (FmTriad* triad : myTriads)
    FapAnimationCreator::finishPosMxReading(triad);

  IHaveInitedAllPosMxReading = false;
#ifdef FT_USE_MEMPOOL
  FFaOperationBase::freeMemPools();
#endif
}


/*!
  The main function to load an animation. Called when the user invokes
  the "Load Animation" command. Supposed to read everything.
*/

bool FapAnimationCreator::loadAnimation(FmAnimation* animation,
					FdAnimateModel* animator,
					bool& userCancelled)
{
  // Modes animation is handled by a separate function

  if (animation && animation->isModesAnimation.getValue())
    return FapAnimationCreator::modesAnimation(animation,animator,userCancelled);

#ifdef FAP_DEBUG
  std::cout <<"\n"<< std::string(80,'=')
            <<"\nFapAnimationCreator::loadAnimation()"<< std::endl;
#endif
  userCancelled = false;
  myAnimator = animator;

  // Set up for RDB reading
  if (!this->initReading(animation))
    return false;

  DoubleSet validDataTimes;
  DoubleSet::const_iterator it, startTimeIt;
  double gottenStartTime = this->initRDB(validDataTimes,startTimeIt);
  if (gottenStartTime > myStopTime)
    return false;

  // Make progress dialog

  FFuProgressDialog* progressDlg = FFuProgressDialog::create("Please wait...","Cancel",
							     "Loading Animation Data");
  progressDlg->setCurrentProgress(0);

  // Progress calculation initialization

  size_t nLinks = myLinks.size();
  size_t nTriads = myTriads.size();

  const double XfWeight = 1.0;
  const double FrWeight = 500.0;
  const double DfWeight = 30.0;

  int isFr = (int)IAmLoadingFringeData;
  int isDf = (int)IAmLoadingDeformData;

  double Xfp =        XfWeight / (XfWeight + FrWeight * isFr + DfWeight * isDf);
  double Frp = isFr * FrWeight / (XfWeight + FrWeight        + DfWeight * isDf);
  double Dfp = isDf * DfWeight / (XfWeight + FrWeight * isFr + DfWeight);

  double stopTime  = myStopTime + myMinDeltaT;
  double totTime   = myStopTime - gottenStartTime;
  double totXfProg = nLinks * totTime * Xfp;
  double totFrProg = nLinks * totTime * Frp;
  double totDfProg = nLinks * totTime * Dfp;

  double totProgress = totXfProg + totFrProg + totDfProg;

  double XfProg = 0.0;
  double FrProg = 0.0;
  double DfProg = 0.0;
  double prevLinkProg = 0.0;
  bool noColoredParts = IAmLoadingFringeData;

  // Read loooooop

  for (size_t i = 0; i < nLinks && !userCancelled; i++)
  {
    FmPart* FEpart = NULL;
    try
    {
      it = startTimeIt;
      double gottenTime = -100.0;
      myExtractor->positionRDB(gottenStartTime,gottenTime);

      if (!IHaveInitedAllPosMxReading)
        FapAnimationCreator::initPosMxReading(myLinks[i], myExtractor);

      while (gottenTime < stopTime)
	if ((userCancelled = progressDlg->userCancelled()))
	  break;
	else {
#ifdef USE_INVENTOR
	  int frameIdx = myAnimator->addFrame(gottenTime);
	  FapAnimationCreator::readPosMx(frameIdx,myLinks[i]);
#endif
	  myLastReadTime = gottenTime;
	  XfProg = prevLinkProg + (totTime-(myStopTime-gottenTime)) * Xfp;
	  progressDlg->setCurrentProgress(100.0*XfProg/totProgress);
	  gottenTime = this->incrementRDB(validDataTimes,it);
	}

      if (!IHaveInitedAllPosMxReading)
        FapAnimationCreator::finishPosMxReading(myLinks[i]);

      if (myLinks[i]->isFEPart())
      {
        FEpart = static_cast<FmPart*>(myLinks[i]);
        if (!FEpart->isFELoaded()) FEpart = NULL;
      }

#ifdef READ_FRINGE_AND_DEF_SEQUENTIALLY

      if (IAmLoadingFringeData && FEpart && !userCancelled)
        {
          it = startTimeIt;
          myExtractor->positionRDB(gottenStartTime,gottenTime);

	  if (FapAnimationCreator::initFringeReading(FEpart,myExtractor,animation))
	    noColoredParts = false;

          while (gottenTime < stopTime)
	    if (userCancelled = progressDlg->userCancelled())
	      break;
	    else {
#ifdef USE_INVENTOR
	      int frameIdx = myAnimator->addFrame(gottenTime);
	      FapAnimationCreator::readFringeData(frameIdx,FEpart,
						  animator->getLegendMapping());
#endif
	      FrProg = XfProg + (totTime-(myStopTime-gottenTime)) * Frp;
	      progressDlg->setCurrentProgress(100.0*FrProg/totProgress);
	      gottenTime = this->incrementRDB(validDataTimes,it);
	    }

	  FapAnimationCreator::finishFringeReading(FEpart);
        }

      if (IAmLoadingDeformData && FEpart && !userCancelled)
        {
          it = startTimeIt;
          myExtractor->positionRDB(gottenStartTime,gottenTime);

          FapAnimationCreator::initDeformationReading(FEpart,myExtractor);

          while (gottenTime < stopTime)
	    if (userCancelled = progressDlg->userCancelled())
	      break;
	    else {
#ifdef USE_INVENTOR
	      int frameIdx = myAnimator->addFrame(gottenTime);
	      FapAnimationCreator::readDeformations(frameIdx,FEpart);
#endif
	      if (IAmLoadingFringeData)
		DfProg = FrProg + (totTime-(myStopTime-gottenTime)) * Dfp;
	      else
		DfProg = XfProg + (totTime-(myStopTime-gottenTime)) * Dfp;
	      progressDlg->setCurrentProgress(100.0*DfProg/totProgress);
	      gottenTime = this->incrementRDB(validDataTimes,it);
	    }

          FapAnimationCreator::finishDeformationReading(FEpart);
        }

#else // READ_FRINGE_AND_DEF_SIMULTANEOUSLY

      if ((IAmLoadingFringeData || IAmLoadingDeformData) && FEpart && !userCancelled)
        {
          it = startTimeIt;
          myExtractor->positionRDB(gottenStartTime,gottenTime);

          if (IAmLoadingFringeData) {
            if (FapAnimationCreator::initFringeReading(FEpart,myExtractor,animation))
	      noColoredParts = false;
#ifdef FT_USE_MEMPOOL
            FFlFEElmResult::freePool();
            FFlFENodeResult::freePool();
#endif
          }

          if (IAmLoadingDeformData)
            FapAnimationCreator::initDeformationReading(FEpart,myExtractor);

          while (gottenTime < stopTime)
	    if ((userCancelled = progressDlg->userCancelled()))
	      break;
	    else {
#ifdef USE_INVENTOR
	      int frameIdx = myAnimator->addFrame(gottenTime);

	      if (IAmLoadingFringeData)
		FapAnimationCreator::readFringeData(frameIdx,FEpart,
						    animator->getLegendMapping());
	      if (IAmLoadingDeformData)
		FapAnimationCreator::readDeformations(frameIdx,FEpart);
#endif
	      FrProg = XfProg + (totTime-(myStopTime-gottenTime)) * (Frp+Dfp);
	      progressDlg->setCurrentProgress(100.0*FrProg/totProgress);
	      gottenTime = this->incrementRDB(validDataTimes,it);
	    }

          if (IAmLoadingFringeData)
            FapAnimationCreator::finishFringeReading(FEpart);

          if (IAmLoadingDeformData)
            FapAnimationCreator::finishDeformationReading(FEpart);
        }

#endif // READ_FRINGE_AND_DEF_SIMULTANEOUSLY

#ifdef FT_USE_MEMPOOL
      if (!IHaveInitedAllPosMxReading)
        FFaOperationBase::freeMemPools();
#endif
      FpModelRDBHandler::clearPreReadTimeStep();

      prevLinkProg = XfProg > FrProg ? XfProg : FrProg;
      if (DfProg > prevLinkProg) prevLinkProg = DfProg;
    }

    catch (const std::bad_alloc&)
    {
      // Not enough memory, clean up

      if (!IHaveInitedAllPosMxReading)
        FapAnimationCreator::finishPosMxReading(myLinks[i]);

      if (IAmLoadingFringeData) {
#ifdef FT_USE_MEMPOOL
        FFlFEElmResult::freePool();
        FFlFENodeResult::freePool();
#endif
        if (FEpart)
          FapAnimationCreator::finishFringeReading(FEpart);
      }

      if (IAmLoadingDeformData && FEpart)
        FapAnimationCreator::finishDeformationReading(FEpart);

#ifdef FT_USE_MEMPOOL
      if (!IHaveInitedAllPosMxReading)
        FFaOperationBase::freeMemPools();
#endif

      FpModelRDBHandler::clearPreReadTimeStep();
      FFaMsg::dialog("Not enough memory!\n"
                     "Some of the animation data could not be read.",
                     FFaMsg::DISMISS_ERROR);
      break;
    }
  }

  // Read loop TRIADS
  for (size_t i = 0; i < nTriads && !userCancelled; i++)
  {
    try
    {
      it = startTimeIt;
      double gottenTime = -100.0;
      myExtractor->positionRDB(gottenStartTime,gottenTime);

      if (!IHaveInitedAllPosMxReading)
        FapAnimationCreator::initPosMxReading(myTriads[i],myExtractor);

      while (gottenTime < stopTime)
        if ((userCancelled = progressDlg->userCancelled()))
          break;
        else {
#ifdef USE_INVENTOR
          int frameIdx = myAnimator->addFrame(gottenTime);
          FapAnimationCreator::readPosMx(frameIdx,myTriads[i]);
#endif
          myLastReadTime = gottenTime;
          gottenTime = this->incrementRDB(validDataTimes,it);
        }

      if (!IHaveInitedAllPosMxReading)
        FapAnimationCreator::finishPosMxReading(myTriads[i]);
    }
    catch (const std::bad_alloc&)
    {
      // Not enough memory, clean up
      if (!IHaveInitedAllPosMxReading)
        FapAnimationCreator::finishPosMxReading(myTriads[i]);

#ifdef FT_USE_MEMPOOL
      if (!IHaveInitedAllPosMxReading)
        FFaOperationBase::freeMemPools();
#endif

      FpModelRDBHandler::clearPreReadTimeStep();
      FFaMsg::dialog("Not enough memory!\n"
                     "Some of the animation data could not be read.",
                     FFaMsg::DISMISS_ERROR);
      break;
    }
  }

  progressDlg->setCurrentProgress(100);
  delete progressDlg;

  // Disable and delete timestep cache

  FpModelRDBHandler::disableTimeStepPreRead();

  if (noColoredParts)
    FFaMsg::dialog("There was no visible geometry to display contours on.",
		   FFaMsg::DISMISS_INFO);

  return true;
}


//////////////////////////////////
//
//  Finite Element Deformations
//
//////////////////////////////////

void FapAnimationCreator::initDeformationReading(FmPart* part, FFrExtractor* extr,
                                                 const std::vector<int>* nodeFilter)
{
  const FFrEntryVec* nrf = FFlrResultResolver::findFEResults(part->getBaseID(), extr, "Nodes");
  if (!nrf) return; // No nodal results for this part

#ifdef FT_USE_PROFILER
  myProfiler->startTimer("Init");
  myProfiler->startTimer("Deform Init");
  FFaMemoryProfiler::reportMemoryUsage("Deform Init start");
#endif

  // Make room for deformation read operations

  FFlLinkHandler* feData = part->getLinkHandler();
  FFlrFELinkResult* feRes = feData->getResults();
  feRes->deformationOps.clear();
  feRes->deformationOps.resize(feData->getVertexCount(),(FFaOperation<FaVec3>*)NULL);

#ifdef USE_INVENTOR
  FdPart* fdpart = static_cast<FdPart*>(part->getFdPointer());
  if (fdpart->updateSpecialLines(-1.0))
    fdpart->updateFdDetails(); // Hide local beam system marker during animation
#endif


  // Loop over all nodes in the RDB

  for (FFrEntryBase* rdbNode : *nrf)
  {
    if (!rdbNode) continue;
    if (!rdbNode->hasUserID()) continue; // Unexpected RDB type, node results IG expected

    FFlNode* node = feData->getNode(rdbNode->getUserID());
    if (!node) continue; // Could node find RDB node in the FE data

    if (nodeFilter && !FFlrFringeCreator::filterNodes(node,*nodeFilter))
      continue; // This is a higher-order node which here is ignored

    // Find read operation for nodal deformation and store it on the right vertex
    FFaOperationBase* tOp = FFlrResultResolver::getNodeReadOp(rdbNode,"VEC3",
                                                              "Translational deformation",
                                                              "Dynamic response");
    if (!tOp) continue; // No results for this node

    int vtxId = node->getVertexID();
    feRes->deformationOps[vtxId] = static_cast<FFaOperation<FaVec3>*>(tOp);
    tOp->ref();

#ifdef USE_INVENTOR
    // Now check if this node is on an eccentric beam element.
    // If so, use the nodal rotation to derive the eccentric deformation.
    for (FFlVisEdge* eccEdge : fdpart->getGroupPartCreator()->getEccEd())
      if (eccEdge->getFirstVxIdx() == vtxId)
      {
        // Create a read operation for the eccentric node based on the FE results
        int eccVtxId = eccEdge->getSecondVxIdx();
        FFaOperationBase* rOp = FFlrResultResolver::getNodeReadOp(rdbNode,"ROT3",
                                                                  "Angular deformation",
                                                                  "Dynamic response");
        if (rOp)
        {
          FFaOperation<FaVec3>* eOp = new FapEccTransform(feRes->deformationOps[vtxId],
                                                          static_cast<FFaOperation<FaVec3>*>(rOp),
                                                          eccEdge->getVector());
          feRes->deformationOps[eccVtxId] = eOp;
          eOp->ref();
        }
        else
        {
          // Missing rotation (unusual), just use translation from first vertex
          feRes->deformationOps[eccVtxId] = feRes->deformationOps[vtxId];
          tOp->ref();
        }
      }
#endif
  }

  if (!FFlrResultResolver::errMsg.empty()) {
    for (const std::pair<const std::string,int>& msg : FFlrResultResolver::errMsg)
      ListUI <<"  ** "<< msg.first <<" ("<< msg.second <<").\n";
    ListUI <<" *** "<< part->getIdString(true) <<" will lack some deformations"
           <<" due to the above error(s).\n";
  }
  FFlrResultResolver::clearLinkInFocus();

#ifdef FT_USE_PROFILER
  myProfiler->stopTimer("Deform Init");
  myProfiler->stopTimer("Init");
  FFaMemoryProfiler::reportMemoryUsage("Deform Init end");
#endif
}


void FapAnimationCreator::readDeformations(int frameIdx, FmPart* part)
{
#ifdef USE_INVENTOR
  FdFEModel* visMod = static_cast<FdLink*>(part->getFdPointer())->getVisualModel();
  if (!visMod) return;

  if (visMod->hasResultDeformation(frameIdx)) return;

#ifdef FT_USE_PROFILER
  myProfiler->startTimer("Deform Read");
#endif

  FFlLinkHandler* feData = part->getLinkHandler();
  FFlrFELinkResult* feRes = feData->getResults();

  bool hasData = false;
  for (FFaOperation<FaVec3>* readOp : feRes->deformationOps)
    if (readOp && (hasData = readOp->hasData()))
      break;

  if (hasData)
  {
    FaVec3Vec vertexFrame(feData->getVertexCount());
    size_t vxIdx = 0;

    for (FFaOperation<FaVec3>* readOp : feRes->deformationOps)
      if (readOp && readOp->hasData())
        readOp->evaluate(vertexFrame[vxIdx++]);
      else
        vxIdx++;

    visMod->setResultDeformation(frameIdx, vertexFrame);
  }

#ifdef FT_USE_PROFILER
  myProfiler->stopTimer("Deform Read");
#endif
#else
  std::cout <<"FapAnimationCreator::readDeformations("
            << frameIdx <<","<< part->getBaseID()
            <<") does nothing."<< std::endl;
#endif
}


bool FapAnimationCreator::readDeformations(FaVec3Vec& def, FmPart* part)
{
#ifdef USE_INVENTOR
#ifdef FT_USE_PROFILER
  myProfiler->startTimer("Deform Read");
#endif

  FFlLinkHandler* lh = part->getLinkHandler();
  FFlrFELinkResult* linkRes = lh->getResults();
  def.reserve(lh->getNodeCount(FFlLinkHandler::FFL_FEM));

  FFaOperation<FaVec3>* readOp = NULL;
  for (NodesCIter nit = lh->nodesBegin(); nit != lh->nodesEnd(); ++nit)
    if ((readOp = linkRes->deformationOps[(*nit)->getVertexID()]))
    {
      FaVec3 dval;
      if (readOp->hasData())
        readOp->evaluate(dval);
      def.push_back(dval);
    }

#ifdef FT_USE_PROFILER
  myProfiler->stopTimer("Deform Read");
#endif
#else
  std::cout <<"FapAnimationCreator::readDeformations("
            << part->getBaseID() <<") does nothing."<< std::endl;
#endif
  return !def.empty();
}


void FapAnimationCreator::finishDeformationReading(FmPart* part)
{
#ifdef FT_USE_PROFILER
  FFaMemoryProfiler::reportMemoryUsage("Deforms Finish start");
  myProfiler->startTimer("Deform Finish");
#endif

  part->getLinkHandler()->deleteResults();

#ifdef FT_USE_PROFILER
  FFaMemoryProfiler::reportMemoryUsage("Deforms Finish end ");
  myProfiler->stopTimer("Deform Finish");
#endif
}


//////////////////////////////////
//
//  Fringes
//
//////////////////////////////////

typedef FFlGroupPartCreator::GroupPartMap::value_type FFlGroupPartItem;

/*!
  Initializes operations etc for an FE part, filling its group part data objects
  with the operations neccesary to extract the data from file.

  Returns the number of positions made ready for data retrieval.
*/

int FapAnimationCreator::initFringeReading(FmPart* part,
                                           FFrExtractor* extr,
                                           FmAnimation* animation,
                                           const std::vector<int>* nodeFilter)
{
  FapFringeSetup fringeSetup;

  if (animation->fringeResultClass.getValue() == "Node")
    fringeSetup.resultClass = FapFringeSetup::NODE;
  else if(animation->fringeResultClass.getValue() == "Element")
    fringeSetup.resultClass = FapFringeSetup::ELM;
  else if (animation->fringeResultClass.getValue() == "Element node")
    fringeSetup.resultClass = FapFringeSetup::ELM_NODE;

  fringeSetup.variableName                 = animation->fringeVariableName.getValue();
  fringeSetup.toScalarOpName               = animation->fringeToScalarOpName.getValue();

  fringeSetup.getOnlyExactResSetMatches    = animation->resultSetSelectionByName.getValue();
  fringeSetup.resultSetName                = animation->resSetName.getValue();
  fringeSetup.resSetMergeOpName            = animation->resSetSelectionOpName.getValue();

  fringeSetup.isLoadingLineFringes         = animation->loadLineFringeData.getValue();
  fringeSetup.isLoadingFaceFringes         = animation->loadFringeData.getValue();

  fringeSetup.averagingOpName              = animation->averagingOpName.getValue();
  if (fringeSetup.averagingOpName == "None")
    fringeSetup.doAverage = FapFringeSetup::NONE;
  else if (animation->averagingItem.getValue() == "Node")
    fringeSetup.doAverage = FapFringeSetup::NODE;
  else if (animation->averagingItem.getValue() == "Element")
    fringeSetup.doAverage = FapFringeSetup::ELM;
  else if (animation->averagingItem.getValue() == "Element face")
    fringeSetup.doAverage = FapFringeSetup::ELM_FACE;

  fringeSetup.maxMembraneAngle             = animation->maxMembraneAngleToAverage.getValue();
  fringeSetup.materialsMustBeEqual         = animation->averagingAcrossMaterials.getValue();
  fringeSetup.propsMustBeEqual             = animation->averagingAcrossProperties.getValue();
  fringeSetup.elmTypesMustBeEqual          = !animation->averagingAcrossElmTypes.getValue();

  fringeSetup.prefGrp             = NULL;
  fringeSetup.geomAveragingOpName = animation->multiFaceAveragingOpName.getValue();
  if (fringeSetup.geomAveragingOpName[0] == '[') {
    // An element group name was chosen, find a matching group within this Part
    std::vector<FmElementGroupProxy*> groups;
    part->getElementGroups(groups);
    for (FmElementGroupProxy* grp : groups)
      if (fringeSetup.geomAveragingOpName == grp->getInfoString() &&
          (fringeSetup.prefGrp = dynamic_cast<FFlGroup*>(grp->getRealObject())))
        break;
    // Always use the first averaging option when an element group is chosen.
    // Normally it should not matter anyway, unless there are more than one
    // group with the same ID+name sharing an element face (very unlikely).
    fringeSetup.geomAveragingOpName = FFaOpUtils::findOpers("VECTOR").front();
  }
  if (fringeSetup.variableName == "Damage")
    mySpValConvertValue = 1.0e-20;
  else if (fringeSetup.variableName == "Log Damage")
    mySpValConvertValue = -20.0;
  else if (fringeSetup.variableName == "Log Life (repeats)")
    mySpValConvertValue = 20.0;
  else if (fringeSetup.variableName == "Log Life (equnits)")
    mySpValConvertValue = 20.0;
  else
    mySpValConvertValue = mySpecialValue;

  FFa::setSpecialResultValue(mySpecialValue,mySpValConvertValue);

  // Setting up how to color faces

  IHaveOneColorPrFace = fringeSetup.isOneColorPrFace();

#ifdef FT_USE_PROFILER
  myProfiler->startTimer("Fringe Init");
  myProfiler->startTimer("Init");
  FFaMemoryProfiler::reportMemoryUsage("Fringe Init Start");
  char scanc; scanf("%c",&scanc);
  FFaMemoryProfiler::reportMemoryUsage("Fringe resolve");
#endif

  // Build Fringe Transformations

  FFlrResultResolver::setLinkInFocus(part->getBaseID(),extr);
  FFlLinkHandler* lh = part->getLinkHandler();
  int nOperations = 0;

  if (nodeFilter)
    nOperations = FFlrFringeCreator::buildColorXfs(lh,fringeSetup,*nodeFilter);
  else
  {
#ifdef USE_INVENTOR
    FdPart* fdpart = static_cast<FdPart*>(part->getFdPointer());
    if (IAmLoadingFringeData%2)
      for (const FFlGroupPartItem& it : fdpart->getGroupPartCreator()->getLinkParts())
        if (it.first == FFlGroupPartCreator::SURFACE_FACES)
          nOperations += FFlrFringeCreator::buildColorXfs(*(it.second),lh,fringeSetup);

    if (IAmLoadingFringeData/2)
      for (const FFlGroupPartItem& it : fdpart->getGroupPartCreator()->getLinkParts())
        if (it.first == FFlGroupPartCreator::OUTLINE_LINES ||
            it.first == FFlGroupPartCreator::SURFACE_LINES)
          nOperations += FFlrFringeCreator::buildColorXfs(*(it.second),lh,fringeSetup);
#endif
  }
#ifdef FAP_DEBUG
  std::cout <<"Number of read operations for "<< part->getIdString() <<": "<< nOperations << std::endl;
#endif

  if (!FFlrResultResolver::errMsg.empty()) {
    for (const std::pair<const std::string,int>& msg : FFlrResultResolver::errMsg)
      ListUI <<"  ** "<< msg.first <<" ("<< msg.second <<").\n";
    ListUI <<" *** "<< part->getIdString(true) <<" will lack some fringe values"
           <<" due to the above error(s).\n";
  }
  FFlrResultResolver::clearLinkInFocus();

#ifdef FT_USE_PROFILER
  myProfiler->stopTimer("Fringe Init");
  FFaMemoryProfiler::reportMemoryUsage("Fringe transform");
  scanf("%c",&scanc);
#endif

  // Delete temporary data on the elements and nodes

  for (ElementsCIter eIt = lh->elementsBegin(); eIt != lh->elementsEnd(); ++eIt)
    (*eIt)->deleteResults();

  for (NodesCIter nIt = lh->nodesBegin(); nIt != lh->nodesEnd(); ++nIt)
    (*nIt)->deleteResults();

#ifdef FT_USE_PROFILER
  myProfiler->stopTimer("Init");
  FFaMemoryProfiler::reportMemoryUsage("Fringe Init End");
  scanf("%c",&scanc);
#endif

  if (!nodeFilter)
    return nOperations;
  else if (fringeSetup.isOneColorPrVertex())
    return 0; // nodal result
  else if (fringeSetup.isOneColorPrFace())
    return 1; // element result
  else
    return 2; // element-nodal result
}


void FapAnimationCreator::readFringeData(int frameIdx, FmPart* part,
                                         const FFaLegendMapper&
#ifdef USE_INVENTOR
                                         legendMapping
#endif
                                         )
{
#ifdef USE_INVENTOR
#ifdef FT_USE_PROFILER
  myProfiler->startTimer("Fringe Read");
#endif

  FdFEGroupPart::lookPolicy look = IHaveOneColorPrFace ? FdFEGroupPart::PR_FACE : FdFEGroupPart::PR_FACE_VERTEX;

  // Lambda function for assigning color fringe values to an FE group part.
  auto&& setResult = [this,look,&legendMapping](FFlGroupPartData* gpd, int fidx)
  {
    if (!gpd->visualModel)
      return;

    if (gpd->visualModel->hasResultLook(fidx))
      return;

    DoubleVec colors;
    if (!FFlrFringeCreator::getColorData(colors,*gpd,IHaveOneColorPrFace))
      return;

    for (double& color : colors)
      // TT #2904: Check equality within 7 most significant digits only,
      // to avoid round-off errors when results are stored as float
      if (fabs(color - mySpecialValue) < mySpecialValue/1.0e7)
        color = mySpValConvertValue;
      else if (color != HUGE_VAL) {
        if (color > myMaxFringeValue) myMaxFringeValue = color;
        if (color < myMinFringeValue) myMinFringeValue = color;
      }

    gpd->visualModel->setResultLook(fidx,look,colors,legendMapping);
  };

  FdPart* fdpart = static_cast<FdPart*>(part->getFdPointer());
  if (IAmLoadingFringeData%2)
    for (const FFlGroupPartItem& it : fdpart->getGroupPartCreator()->getLinkParts())
      if (it.first == FFlGroupPartCreator::SURFACE_FACES)
        setResult(it.second,frameIdx);

  if (IAmLoadingFringeData/2)
    for (const FFlGroupPartItem& it : fdpart->getGroupPartCreator()->getLinkParts())
      if (it.first == FFlGroupPartCreator::OUTLINE_LINES ||
          it.first == FFlGroupPartCreator::SURFACE_LINES)
        setResult(it.second,frameIdx);

#ifdef FT_USE_PROFILER
  myProfiler->stopTimer("Fringe Read");
#endif
#else
  std::cout <<"FapAnimationCreator::readFringeData("
            << frameIdx <<","<< part->getBaseID()
            <<") does nothing."<< std::endl;
#endif
}


bool FapAnimationCreator::readFringeData(DoubleVec& values, FmPart* part)
{
#ifdef FT_USE_PROFILER
  myProfiler->startTimer("Fringe Read");
#endif

  values.clear();

  FFlrFELinkResult* linkRes = part->getLinkHandler()->getResults();
  size_t nRes = linkRes->scalarOps.size();

  for (size_t i = 0; i < nRes; i++)
  {
    FFaOperation<double>* readOp = linkRes->scalarOps[i];
    if (readOp && readOp->hasData())
    {
      if (values.empty())
        values.resize(nRes,HUGE_VAL);
      readOp->evaluate(values[i]);
      if (fabs(values[i] - mySpecialValue) < mySpecialValue/1.0e7)
	values[i] = mySpValConvertValue;
    }
  }

#ifdef FT_USE_PROFILER
  myProfiler->stopTimer("Fringe Read");
#endif

  return !values.empty();
}


bool FapAnimationCreator::readFringeData(std::vector<DoubleVec>& values, FmPart* part)
{
#ifdef FT_USE_PROFILER
  myProfiler->startTimer("Fringe Read");
#endif

  values.clear();

  bool gotData = false;
  FFlrFELinkResult* linkRes = part->getLinkHandler()->getResults();
  int nel = linkRes->elmStart.size()-1;

  values.resize(nel);
  for (int i = 0; i < nel; i++)
  {
    int k = linkRes->elmStart[i];
    int n = linkRes->elmStart[i+1] - k;
    values[i].resize(n);
    for (int j = 0; j < n; j++)
    {
      FFaOperation<double>* readOp = linkRes->scalarOps[k+j];
      if (readOp && readOp->hasData())
      {
	gotData = true;
	readOp->evaluate(values[i][j]);
	if (fabs(values[i][j] - mySpecialValue) < mySpecialValue/1.0e7)
	  values[i][j] = mySpValConvertValue;
      }
      else
	values[i][j] = HUGE_VAL;
    }
  }

#ifdef FT_USE_PROFILER
  myProfiler->stopTimer("Fringe Read");
#endif

  if (!gotData) values.clear();
  return gotData;
}


void FapAnimationCreator::finishFringeReading(FmPart* part)
{
#ifdef FT_USE_PROFILER
  FFaMemoryProfiler::reportMemoryUsage("Fringe Finish start");
  myProfiler->startTimer("Fringe Finish");
  char scanc; scanf("%c",&scanc);
#endif

  // Delete temporary data on the parts/elements/nodes

  part->getLinkHandler()->deleteResults();

  // Delete temporary data on the faces and edges

#ifdef USE_INVENTOR
  FdPart* fdpart = static_cast<FdPart*>(part->getFdPointer());
  if (IAmLoadingFringeData%2)
    for (const FFlGroupPartItem& it : fdpart->getGroupPartCreator()->getLinkParts())
      if (it.first == FFlGroupPartCreator::SURFACE_FACES)
        FFlrFringeCreator::deleteColorsXfs(*it.second);

  if (IAmLoadingFringeData/2)
    for (const FFlGroupPartItem& it : fdpart->getGroupPartCreator()->getLinkParts())
      if (it.first == FFlGroupPartCreator::OUTLINE_LINES ||
          it.first == FFlGroupPartCreator::SURFACE_LINES)
        FFlrFringeCreator::deleteColorsXfs(*it.second);
#endif

#ifdef FT_USE_PROFILER
  myProfiler->stopTimer("Fringe Finish");
  FFaMemoryProfiler::reportMemoryUsage("Fringes Finish end");
  scanf("%c",&scanc);
#endif
}


//////////////////////////////////
//
//  Link position matrices
//
//////////////////////////////////

void FapAnimationCreator::initPosMxReading(FmLink* link, FFrExtractor* rdb)
{
#ifdef FT_USE_PROFILER
  myProfiler->startTimer("PosMx Init");
  myProfiler->startTimer("Init");
#endif

  FFaOperationBase* pop = FFlrResultResolver::findPosition(link->getItemName(),
                                                           link->getBaseID(),rdb);
  if (pop)
  {
#ifdef USE_INVENTOR
    FdFEModel* visMod = static_cast<FdLink*>(link->getFdPointer())->getVisualModel();
    visMod->setPosMxReadOp(dynamic_cast<FFaOperation<FaMat34>*>(pop));
#else
    std::cout <<"FapAnimationCreator::initPosMxReading() does nothing."<< std::endl;
#endif
  }

#ifdef FT_USE_PROFILER
  myProfiler->stopTimer("PosMx Init");
  myProfiler->stopTimer("Init");
#endif
}


//////////////////////////////////
//
//  Triad position matrices
//
//////////////////////////////////

void FapAnimationCreator::initPosMxReading(FmTriad* triad, FFrExtractor* rdb)
{
#ifdef FT_USE_PROFILER
  myProfiler->startTimer("PosMx Init");
  myProfiler->startTimer("Init");
#endif

  FFaOperationBase* pop = FFlrResultResolver::findPosition("Triad",
                                                           triad->getBaseID(),rdb);
  if (pop)
  {
#ifdef USE_INVENTOR
    FdTriad* visMod = static_cast<FdTriad*>(triad->getFdPointer());
    visMod->setPosMxReadOp(dynamic_cast<FFaOperation<FaMat34>*>(pop));
#else
    std::cout <<"FapAnimationCreator::initPosMxReading() does nothing."<< std::endl;
#endif
  }

#ifdef FT_USE_PROFILER
  myProfiler->stopTimer("PosMx Init");
  myProfiler->stopTimer("Init");
#endif
}


/*!
  Read position matrix for the supplied link, and add it into the
  animation data at the frameIdx frame position.

  Returns false if data seems to be missing.
  Returns true if data has been read, or there are no reason to do so
  (no operation allocated, or the results seem to be loaded already).
*/

bool FapAnimationCreator::readPosMx(int frameIdx, FmLink* link)
{
  bool status = true;
#ifdef USE_INVENTOR
#ifdef FT_USE_PROFILER
  myProfiler->startTimer("PosMx Read");
#endif

  FdFEModel* visMod = static_cast<FdLink*>(link->getFdPointer())->getVisualModel();

  if (!visMod->hasResultTransform(frameIdx))
    if (visMod->getPosMxReadOp())
      if ((status = visMod->getPosMxReadOp()->hasData()))
      {
#if FAP_DEBUG > 2
        std::cout <<" L["<< link->getID() <<"] "<< frameIdx;
#endif
        FaMat34 xfmx;
        visMod->getPosMxReadOp()->evaluate(xfmx);
        visMod->setResultTransform(frameIdx,xfmx);
      }

#ifdef FT_USE_PROFILER
  myProfiler->stopTimer("PosMx Read");
#endif
#else
  std::cout <<"FapAnimationCreator::readPosMx("
            << frameIdx <<","<< link->getBaseID()
            <<") does nothing."<< std::endl;
#endif
  return status;
}

bool FapAnimationCreator::readPosMx(int frameIdx, FmTriad* triad)
{
  bool status = true;
#ifdef USE_INVENTOR
#ifdef FT_USE_PROFILER
  myProfiler->startTimer("PosMx Read");
#endif

  FdTriad* visMod = static_cast<FdTriad*>(triad->getFdPointer());

  if (!visMod->hasResultTransform(frameIdx))
    if (visMod->getPosMxReadOp())
      if ((status = visMod->getPosMxReadOp()->hasData()))
      {
#if FAP_DEBUG > 2
        std::cout <<" T["<< triad->getID() <<"] "<< frameIdx;
#endif
        FaMat34 xfmx;
        visMod->getPosMxReadOp()->evaluate(xfmx);
        visMod->setResultTransform(frameIdx,xfmx);
      }

#ifdef FT_USE_PROFILER
  myProfiler->stopTimer("PosMx Read");
#endif
#else
  std::cout <<"FapAnimationCreator::readPosMx("
            << frameIdx <<","<< triad->getBaseID()
            <<") does nothing."<< std::endl;
#endif
  return status;
}


void FapAnimationCreator::finishPosMxReading(FmLink* link)
{
#ifdef USE_INVENTOR
#ifdef FT_USE_PROFILER
  myProfiler->startTimer("PosMx Finish");
#endif

  static_cast<FdLink*>(link->getFdPointer())->getVisualModel()->setPosMxReadOp(NULL);

#ifdef FT_USE_PROFILER
  myProfiler->startTimer("PosMx Finish");
#endif
#else
  std::cout <<"FapAnimationCreator::finishPosMxReading("
            << link->getBaseID() <<") does nothing."<< std::endl;
#endif
}

void FapAnimationCreator::finishPosMxReading(FmTriad* triad)
{
#ifdef USE_INVENTOR
#ifdef FT_USE_PROFILER
  myProfiler->startTimer("PosMx Finish");
#endif

  static_cast<FdTriad*>(triad->getFdPointer())->setPosMxReadOp(NULL);

#ifdef FT_USE_PROFILER
  myProfiler->startTimer("PosMx Finish");
#endif
#else
  std::cout <<"FapAnimationCreator::finishPosMxReading("
            << triad->getBaseID() <<") does nothing."<< std::endl;
#endif
}


/*!
  This function does basically much the same as loadAnimation,
  but writes data to VTF file instead of displaying the animation.
*/

bool FapAnimationCreator::exportToVTF(FmAnimation* animation,
				      const std::string& vtfFile,
				      VTFFileType type,
				      bool convTo1stOrder, double timeInc)
{
  // Set up for RDB reading
  myAnimator = NULL;
  if (!this->initReading(animation))
    return false;

  DoubleSet validDataTimes;
  DoubleSet::const_iterator timeIt;
  double gottenTime = this->initRDB(validDataTimes,timeIt);
  if (gottenTime > myStopTime)
    return false;

  // Make progress dialog
  FFuProgressDialog* progressDlg = NULL;
  if (!FFaAppInfo::isConsole()) {
    progressDlg = FFuProgressDialog::create("Please wait...","Cancel","Creating VTF file");
    progressDlg->setCurrentProgress(0);
  }

  // Progress calculation initialization
  double totTime = myStopTime - myStartTime;
  double totProg = totTime*1.1; // estimate 10% extra for geometry output

  // Open the VTF file and write link geometry and visualization properties
  FapVTFFile vtf;
  FapExpProp vtfProp;
  vtfProp.deformation = IAmLoadingDeformData;
  vtfProp.fringe = IAmLoadingFringeData%2;
  vtfProp.range = animation->legendRange.getValue();
  FFaMsg::pushStatus("Writing FE geometry to VTF");
  bool status = (vtf.open(vtfFile,type) &&
		 vtf.writeGeometry(myLinks,true,convTo1stOrder) &&
		 vtf.writeProperties(vtfProp));
  FFaMsg::popStatus();

  if (!status || (progressDlg && progressDlg->userCancelled()))
  {
    delete progressDlg;
    return false;
  }
  if (progressDlg)
    progressDlg->setCurrentProgress(50.0*(totProg-totTime)/totProg);

  FFaMsg::pushStatus("Initializing results extraction");

  int iResMap = 0;
  size_t i, nLinks = myLinks.size();

  // Get RDB variable reference pointers
  FFrEntryBase* stepPtr = myExtractor->getTopLevelVar("Time step number");
  FFrEntryVec mxVarRef(nLinks);
  for (i = 0; i < nLinks; i++)
    mxVarRef[i] = myExtractor->findVar(myLinks[i]->getItemName(),
                                       myLinks[i]->getBaseID(),
                                       "Position matrix");

  for (FmPart* part : myParts)
  {
    const std::vector<int>& nodes = vtf.get1stOrderNodes(part->getBaseID());
    if (IAmLoadingDeformData)
      FapAnimationCreator::initDeformationReading(part,myExtractor,&nodes);
    if (IAmLoadingFringeData%2)
      iResMap = FapAnimationCreator::initFringeReading(part,myExtractor,animation,&nodes);
  }

  FFaMsg::changeStatus("Writing results VTF");
  if (validDataTimes.empty())
    FFaMsg::enableSubSteps(totTime/FmDB::getActiveAnalysis()->timeIncr.getValue());
  else
    FFaMsg::enableSubSteps(validDataTimes.size());

  // Time step loop
  double nxtTime = myStartTime;
  double endTime = myStopTime + myMinDeltaT;
  for (int jStep = 1; gottenTime < endTime; jStep++)
  {
    FFaMsg::setSubStep(jStep);
    if (progressDlg) {
      if ((status = !progressDlg->userCancelled()))
	progressDlg->setCurrentProgress(100.0*(totProg+gottenTime-myStopTime)/totProg);
      else
	break;
    }

    if (timeInc > 0.0 && jStep > 1 && gottenTime < nxtTime+timeInc)
    {
      // Skip this time step
      gottenTime = this->incrementRDB(validDataTimes,timeIt);
      continue;
    }
    else
      nxtTime = nxtTime + timeInc;

    // Read and write time step data
    int iStep = 0;
    myExtractor->getSingleTimeStepData(stepPtr,&iStep,1);
    status = vtf.writeStep(iStep,gottenTime);

    // Read and write link transformations
    std::map<int,FaMat34> mxLink;
    for (i = 0; i < nLinks; i++)
      FapAnimationCreator::readMatrix(mxVarRef[i],mxLink[myLinks[i]->getBaseID()]);
    status = vtf.writeTransformations(mxLink);
    if (!status) break;

    // Read and write FE part deformations
    if (IAmLoadingDeformData)
      for (i = 0; i < myParts.size() && status; i++)
      {
        FaVec3Vec dis;
        if (FapAnimationCreator::readDeformations(dis,myParts[i]))
          status = vtf.writeDeformations(myParts[i]->getBaseID(),dis);
      }

    // Read and write fringe results
    if (IAmLoadingFringeData%2)
      for (i = 0; i < myParts.size() && status; i++)
      {
        if (iResMap == 2)
        {
          // Element-nodal results
          std::vector<DoubleVec> values;
          if (FapAnimationCreator::readFringeData(values,myParts[i]))
            status = vtf.writeFringes(myParts[i]->getBaseID(),values,
                                      animation->getFringeQuantity());
        }
        else
        {
          // Element or nodal results
          DoubleVec values;
          if (FapAnimationCreator::readFringeData(values,myParts[i]))
            status = vtf.writeFringes(myParts[i]->getBaseID(),values,
                                      animation->getFringeQuantity(),
                                      iResMap == 1);
        }
      }

    gottenTime = this->incrementRDB(validDataTimes,timeIt);
  }

  FFaMsg::disableSubSteps();
  FFaMsg::popStatus();

  if (progressDlg) {
    progressDlg->setCurrentProgress(100);
    delete progressDlg;
  }
  if (status)
    status = vtf.close();

  // Clean up
  if (IAmLoadingDeformData || IAmLoadingFringeData%2)
    for (FmPart* part : myParts)
      FapAnimationCreator::finishDeformationReading(part);

  FpModelRDBHandler::disableTimeStepPreRead();

  return status;
}
