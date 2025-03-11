// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapAnimationCmds.H"
#include "vpmApp/vpmAppDisplay/FapAnimationCreator.H"
#include "vpmApp/vpmAppDisplay/FFaLegendMapper.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUACommandHandler.H"
#include "vpmApp/vpmAppProcess/FapSolutionProcessMgr.H"
#include "vpmApp/vpmAppProcess/FapSolverID.H"
#include "vpmApp/FapEventManager.H"

#include "vpmDB/FmAnimation.H"
#include "vpmDB/FmAnalysis.H"
#include "vpmDB/FmLink.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmSeaState.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmGlobalViewSettings.H"
#include "vpmPM/FpRDBExtractorManager.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "vpmUI/Fui.H"

#include "FFrLib/FFrExtractor.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFuLib/FFuAuxClasses/FFuaIdentifiers.H"
#include "FFuLib/FFuProgressDialog.H"
#include "FFaLib/FFaDefinitions/FFaListViewItem.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "Admin/FedemAdmin.H"

#ifdef USE_INVENTOR
#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdLink.H"
#include "vpmDisplay/FdAnimateModel.H"
#endif

#ifdef FT_USE_PROFILER
#include "FFaLib/FFaProfiler/FFaProfiler.H"
#endif

#if defined(win32) || defined(win64)
#include <windows.h>
#define SLEEP(ms) Sleep(ms)
#else
#include <unistd.h>
#define SLEEP(ms) usleep(ms*1000)
#endif

FapAnimationCmds::SignalConnector FapAnimationCmds::signalConnector;
FdAnimateModel* FapAnimationCmds::ourAnimator = NULL;
FmAnimation* FapAnimationCmds::ourCurrentAnimation = NULL;
FapAnimationCreator* FapAnimationCmds::ourAnimationCreator = NULL;


void FapAnimationCmds::init()
{
  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_animation_show");
  cmdItem->setSmallIcon(animationLoad_xpm);
  cmdItem->setText("Load Animation");
  cmdItem->setToolTip("Load Animation");
  cmdItem->setAccelKey(FFuaKeyCode::CtrlAccel+FFuaKeyCode::A);
  cmdItem->setActivatedCB(FFaDynCB0S(FapAnimationCmds::show));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapAnimationCmds::getShowSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_animation_hide");
  cmdItem->setSmallIcon(animationExit_xpm);
  cmdItem->setText("End Animation Session");
  cmdItem->setToolTip("End Animation Session");
  cmdItem->setAccelKey(FFuaKeyCode::CtrlAccel+FFuaKeyCode::X);
  cmdItem->setActivatedCB(FFaDynCB0S(FapAnimationCmds::hide));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapAnimationCmds::getHideSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_animation_showControls");
  cmdItem->setSmallIcon(animationControl_xpm);
  cmdItem->setText("Show Animation Controls...");
  cmdItem->setToolTip("Show Animation Controls");
  cmdItem->setActivatedCB(FFaDynCB0S([](){ Fui::animationControlUI(); }));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapAnimationCmds::getHideSensitivity,bool&));
}


void FapAnimationCmds::show()
{
  FapAnimationCmds::show(FapAnimationCmds::findSelectedAnimation(),false);
}


void FapAnimationCmds::animationToggleSkip(bool flag)
{
#ifdef USE_INVENTOR
  if (FapAnimationCmds::ourAnimator)
    FapAnimationCmds::ourAnimator->setSkipFrames(flag);
#else
  std::cout <<"FapAnimationCmds::animationToggleSkip("
            << std::boolalpha << flag <<") does nothing."<< std::endl;
#endif
}

void FapAnimationCmds::animationToLastFrame()
{
#ifdef USE_INVENTOR
  if (FapAnimationCmds::ourAnimator)
    FapAnimationCmds::ourAnimator->stepLast();
#endif
}

void FapAnimationCmds::animationToFirstFrame()
{
#ifdef USE_INVENTOR
  if (FapAnimationCmds::ourAnimator)
    FapAnimationCmds::ourAnimator->stepFirst();
#endif
}

void FapAnimationCmds::animationPlayReverse()
{
#ifdef USE_INVENTOR
  if (FapAnimationCmds::ourAnimator)
    FapAnimationCmds::ourAnimator->playReverse();
#endif
}

void FapAnimationCmds::animationFrequencyScale(float freqscale)
{
#ifdef USE_INVENTOR
  if (FapAnimationCmds::ourAnimator)
    FapAnimationCmds::ourAnimator->setSmartScaleFactor(freqscale);
#else
  std::cout <<"FapAnimationCmds::animationFrequencyScale("
            << freqscale <<") does nothing."<< std::endl;
#endif
}

void FapAnimationCmds::absoluteAnimationFrequency(float percentage)
{
#ifdef USE_INVENTOR
  if (FapAnimationCmds::ourAnimator)
    FapAnimationCmds::ourAnimator->setScaleFrequency(percentage);
#else
  std::cout <<"FapAnimationCmds::absoluteAnimationFrequency("
            << percentage <<") does nothing."<< std::endl;
#endif
}

void FapAnimationCmds::animationPlay()
{
#ifdef USE_INVENTOR
  if (FapAnimationCmds::ourAnimator)
    FapAnimationCmds::ourAnimator->playForward();
#endif
}

void FapAnimationCmds::animationPause()
{
#ifdef USE_INVENTOR
  if (FapAnimationCmds::ourAnimator)
    FapAnimationCmds::ourAnimator->pause();
#endif
}

void FapAnimationCmds::animationStop()
{
#ifdef USE_INVENTOR
  if (FapAnimationCmds::ourAnimator)
    FapAnimationCmds::ourAnimator->stop();
#endif
}

void FapAnimationCmds::animationStepFwd()
{
#ifdef USE_INVENTOR
  if (FapAnimationCmds::ourAnimator)
    FapAnimationCmds::ourAnimator->stepForward();
#endif
}

void FapAnimationCmds::animationStepRwd()
{
#ifdef USE_INVENTOR
  if (FapAnimationCmds::ourAnimator)
    FapAnimationCmds::ourAnimator->stepReverse();
#endif
}

void FapAnimationCmds::animationType(int type)
{
#ifdef USE_INVENTOR
  if (FapAnimationCmds::ourAnimator)
    FapAnimationCmds::ourAnimator->setAnimationType((FdAnimateModel::FdAnimType)type);
#else
  std::cout <<"FapAnimationCmds::animationType("
            << type <<") does nothing."<< std::endl;
#endif
}


void FapAnimationCmds::animationShowPos(double time)
{
#ifdef USE_INVENTOR
  if (FapAnimationCmds::ourAnimator)
    FapAnimationCmds::ourAnimator->moveToTime((float)time);
#else
  std::cout <<"FapAnimationCmds::animationShowPos("
            << time <<") does nothing."<< std::endl;
#endif
}


bool FapAnimationCmds::exportAnim(bool useAllFrames, bool useRealTime,
				  bool omitNthFrame, bool includeNthFrame,
				  int nthFrameToOmit, int nThFrameToInclude,
				  const std::string& fileName, int fileFormat)
{
  if (!ourCurrentAnimation || !ourAnimator) return false;

#ifdef USE_INVENTOR
  return ourAnimator->exportAnim(useAllFrames,useRealTime,
                                 omitNthFrame,includeNthFrame,
                                 nthFrameToOmit,nThFrameToInclude,
                                 fileName,fileFormat);
#else
  std::cerr <<" *** FapAnimationCmds::exportAnim: Not available "
            << useAllFrames << useRealTime << omitNthFrame << includeNthFrame
            <<" "<< nthFrameToOmit << nThFrameToInclude
            <<" "<< fileName <<" "<< fileFormat << std::endl;
  return false;
#endif
}


bool FapAnimationCmds::exportVTF(FmAnimation* anim,
                                 const std::string& fileName, int fileFormat,
                                 bool firstOrder, double timeInc)
{
  // Create an animation creator if we've got none

  if (!ourAnimationCreator)
    ourAnimationCreator = new FapAnimationCreator();

  // Create the VTF file

  FFaMsg::pushStatus("Exporting Animation Data to VTF");
  bool retVar = ourAnimationCreator->exportToVTF(anim,fileName,
						 (VTFFileType)fileFormat,
						 firstOrder,timeInc);
  FFaMsg::popStatus();
  return retVar;
}


void FapAnimationCmds::show(FmAnimation* anim, bool showUI)
{
  // If we already have an animation active, hide it :

  FapAnimationCmds::hide();

  // Do nothing if there was no animation

  if (!anim) return;

  // Store animation as new current animation

  ourCurrentAnimation = anim;

#ifdef USE_INVENTOR
#ifdef FT_USE_PROFILER
  FFaProfiler animTimer("AnimationCmds");
  animTimer.startTimer("Show");
#endif

  // Make an animator to handle the animation

  ourAnimator = new FdAnimateModel(0,1);

  // Create an animation creator if we've got none

  if (!ourAnimationCreator)
    ourAnimationCreator = new FapAnimationCreator();

  // Initialize the animator by putting into it what
  // should be animated, and the other datas :
  std::vector<FdAnimatedBase*> objs;
  std::vector<FmLink*> links;
  FmDB::getAllLinks(links);
  for (FmLink* link : links)
    if (link->getFdPointer())
      objs.push_back(dynamic_cast<FdAnimatedBase*>(link->getFdPointer()));

  if (FmDB::getActiveViewSettings()->visibleTriads())
  {
    std::vector<FmTriad*> triads;
    FmDB::getAllTriads(triads);
    // Only add triads that are not attached.
    // Triads attached to links are animated indirectly via the link
    for (FmTriad* triad : triads)
      if (triad->getFdPointer() && !triad->getOwnerLink(0))
        objs.push_back(dynamic_cast<FdAnimatedBase*>(triad->getFdPointer()));
  }

  // Sea wave visualization
  FmSeaState* seaState = FmDB::getSeaStateObject(false);
  if (seaState && seaState->getFdPointer())
    objs.push_back(dynamic_cast<FdAnimatedBase*>(seaState->getFdPointer()));

  ourAnimator->setAnimationObjects(objs);

  // Load animation

  FFaMsg::pushStatus("Loading Animation Data");
  bool userCancelled = false;
  ourAnimationCreator->loadAnimation(anim,ourAnimator,userCancelled);
  FapAnimationCmds::updateAnimator();

  // Tell animator that everything is read

  ourAnimator->postProcess();

  // Broadcast to the rest of the application that a new animation is active

  FapEventManager::setActiveAnimation(anim);

  // Show first step

  ourAnimator->stepForward();
  ourAnimator->showProgressAnimation(true);

  // Show play panel only when more than one frame
  if (ourAnimator->hasMultiSteps())
    showUI = true;

  FFaMsg::popStatus();

#ifdef FT_USE_PROFILER
  animTimer.stopTimer("Show");
  animTimer.report();
#endif
#endif

  if (showUI)
    Fui::animationUI();
}


void FapAnimationCmds::updateAnimator()
{
#ifdef USE_INVENTOR
  if (!ourAnimator) return;
  if (!ourCurrentAnimation) return;

  Fui::noUserInputPlease();

  // Set new legend values
  ourAnimator->showLegend(ourCurrentAnimation->showLegend.getValue());

  bool loadFringes = (ourCurrentAnimation->loadFringeData.getValue() ||
		      ourCurrentAnimation->loadLineFringeData.getValue());

  if (loadFringes && ourCurrentAnimation->showFringes.getValue())
  {
    FFaLegendMapper legendMapping;
    legendMapping.selectValueMapping(ourCurrentAnimation->legendMappingOpName.getValue());
    legendMapping.selectColorMapping(ourCurrentAnimation->colorMappingOpName.getValue());

    double legendMax = ourCurrentAnimation->legendRange.getValue().second;
    double legendMin = ourCurrentAnimation->legendRange.getValue().first;

    // If range is zero, use calculated max min
    if (legendMax == 0.0 && legendMin == 0.0 && ourAnimationCreator)
      if (ourAnimationCreator->getMinFringeVal() !=  HUGE_VAL &&
	  ourAnimationCreator->getMaxFringeVal() != -HUGE_VAL)
      {
	legendMax = ourAnimationCreator->getMaxFringeVal();
	legendMin = ourAnimationCreator->getMinFringeVal();
      }

    legendMapping.setValueInterval  (legendMax,legendMin);

    legendMapping.setUseSmoothLegend(ourCurrentAnimation->smoothLegend.getValue());
    legendMapping.setUseTickCount   (ourCurrentAnimation->useLegendTickCount.getValue());
    legendMapping.setTickCount      (ourCurrentAnimation->legendTickCount.getValue());
    legendMapping.setTickSpacing    (ourCurrentAnimation->legendTickSpacing.getValue());

    ourAnimator->setFringeLegendMapping(legendMapping);
    ourAnimator->showLegendBar(true);
  }
  else
    ourAnimator->showLegendBar(false);

  // Set animation control variables

  FdDB::setLinkToFollow(ourCurrentAnimation->linkToFollow);

  ourAnimator->showFringes     (ourCurrentAnimation->showFringes.getValue());
  ourAnimator->showLinkMotion  (ourCurrentAnimation->showLinkMotion.getValue());
  ourAnimator->showTriadMotion (ourCurrentAnimation->showTriadMotion.getValue());
  ourAnimator->showDeformations(ourCurrentAnimation->showDeformation.getValue());

  if (ourCurrentAnimation->isModesAnimation.getValue())
  {
    // A separate deformation scaling is irrelevant for mode shape animations
    ourAnimator->setDeformationScale(1.0);
    Fui::okToGetUserInput();
    return; // Legend has been defined in FapAnimationCreator::modesAnimation
  }
  else
    ourAnimator->setDeformationScale(ourCurrentAnimation->deformationScale.getValue());

  // Construct legend description

  std::vector<std::string> descr;
  descr.push_back(std::string("Fedem ") + FedemAdmin::getVersion() +
		  ": - " + ourCurrentAnimation->getUserDescription() + " -");

  if (ourCurrentAnimation->makeFrameForMostFrequentResult.getValue())
    descr.push_back("Time steps: All");
  else
    descr.push_back("Time steps: Stress, strain or deformations only");

  if (loadFringes)
  {
    descr.push_back("  Contours: " + ourCurrentAnimation->getFringeQuantity());

    std::string maxtxt = "?.?";
    std::string mintxt = "?.?";
    if (ourAnimationCreator)
    {
      if (ourAnimationCreator->getMinFringeVal() != HUGE_VAL)
	mintxt = FFaNumStr(ourAnimationCreator->getMinFringeVal(), 'g', 7);
      if (ourAnimationCreator->getMaxFringeVal() != -HUGE_VAL)
	maxtxt = FFaNumStr(ourAnimationCreator->getMaxFringeVal(), 'g', 7);
    }
    descr.push_back("  Min, Max: " + mintxt + ", " + maxtxt);

    if (ourCurrentAnimation->resultSetSelectionByName.getValue())
      descr.push_back("Result set: " + ourCurrentAnimation->resSetName.getValue());
    else
      descr.push_back("Result set: " + ourCurrentAnimation->resSetSelectionOpName.getValue());
  }

  if (ourCurrentAnimation->loadDeformationData.getValue() && ourCurrentAnimation->showDeformation.getValue())
    descr.push_back("Def. Scale: " + FFaNumStr(ourCurrentAnimation->deformationScale.getValue()));
  else if (ourCurrentAnimation->showLinkMotion.getValue())
    descr.push_back("Rigid body animation");

  ourAnimator->setLegendDescription(descr);
  Fui::okToGetUserInput();
#endif
}


void FapAnimationCmds::hide()
{
  if (!ourCurrentAnimation)
    return;

  Fui::animationUI(false,false);

  FapEventManager::setActiveAnimation(NULL);

#ifdef USE_INVENTOR
  FdDB::setLinkToFollow(NULL);

  if (ourAnimator)
  {
    Fui::noUserInputPlease();
    FFaMsg::pushStatus("Deleting animation data");

    ourAnimator->deleteAnimationData();
    delete ourAnimator;
    ourAnimator = NULL;

    FFaMsg::popStatus();
    Fui::okToGetUserInput();
  }
#endif

  if (ourAnimationCreator)
  {
    delete ourAnimationCreator;
    ourAnimationCreator = NULL;
  }

  ourCurrentAnimation = NULL;
  FapUACommandHandler::updateAllUICommandsSensitivity();
}


void FapAnimationCmds::getShowSensitivity(bool& sensitivity)
{
  sensitivity = FapAnimationCmds::findSelectedAnimation() != NULL;
}


void FapAnimationCmds::getHideSensitivity(bool& sensitivity)
{
  sensitivity = ourCurrentAnimation != NULL;
}


FmAnimation* FapAnimationCmds::findSelectedAnimation()
{
  std::vector<FFaListViewItem*> permSelection;
  FFaListViewItem* tmpSelection = NULL;
  FapEventManager::getLVSelection(permSelection,tmpSelection);

  // If more than one animation is selected, or anything else
  // than animations is among the selection, return NULL

  if (tmpSelection)
    return dynamic_cast<FmAnimation*>(tmpSelection);
  else if (permSelection.size() == 1)
    return dynamic_cast<FmAnimation*>(permSelection.front());
  else
    return NULL;
}


void FapAnimationCmds::onModelMemberConnected(FmModelMemberBase* item)
{
  if (item->isOfType(FmLink::getClassTypeID()))
    FapAnimationCmds::updateAnimator();
}


void FapAnimationCmds::onModelMemberDisconnected(FmModelMemberBase* item)
{
  if (item == ourCurrentAnimation)
    FapAnimationCmds::hide();
}


void FapAnimationCmds::onModelMemberChanged(FmModelMemberBase* item)
{
  if (item == ourCurrentAnimation)
    FapAnimationCmds::updateAnimator();
}


void FapAnimationCmds::onModelExtrDataChanged(FFrExtractor* extr)
{
  if (extr->empty())
    return; // Nothing to do here, before we have actual results

  if (!ourAnimator || !ourCurrentAnimation || !ourAnimationCreator)
    return;

  if (!FapSolutionProcessManager::instance()->isGroupRunning(FapSolverID::FAP_DYN_SOLVER))
    return;

  // Unless the current animation is a time history animation, close it,
  // since the dynamics solver will not produce any data for it anyway
  if (ourCurrentAnimation->isHistoryAnimation())
  {
    Fui::noUserInputPlease();
    ourAnimationCreator->readAllNewPosMx(ourCurrentAnimation);
    FapAnimationCmds::updateAnimator();
    Fui::okToGetUserInput();
  }
  else
    FapAnimationCmds::hide();
}


void FapAnimationCmds::onModelExtrHeaderChanged(FFrExtractor* extr)
{
  if (!FapSolutionProcessManager::instance()->isGroupRunning(FapSolverID::FAP_DYN_SOLVER))
    return;

  if (ourCurrentAnimation)
    // Unless the current animation is a time history animation, close it,
    // since the dynamics solver will not produce any data for it anyway
    if (!ourCurrentAnimation->isHistoryAnimation())
      FapAnimationCmds::hide();

  if (extr->empty())
    return; // Nothing to do here, before we have actual results

  if (!ourCurrentAnimation && FmDB::getActiveAnalysis()->autoAnimateSwitch.getValue())
  {
    // Automatically open a rigid body animation for progress animation
    FmAnimation* candidate = NULL;
    std::vector<FmModelMemberBase*> animations;
    FmDB::getAllOfType(animations,FmAnimation::getClassTypeID());
    for (FmModelMemberBase* obj : animations)
    {
      FmAnimation* anim = static_cast<FmAnimation*>(obj);
      if (anim->isHistoryAnimation())
      {
	if (!anim->loadFringeData.getValue() &&
	    !anim->loadLineFringeData.getValue() &&
	    !anim->loadDeformationData.getValue())
	{
	  // perfect..., use this one
	  candidate = anim;
	  break;
	}
	else if (!candidate)
	  candidate = anim;
      }
    }
    FapAnimationCmds::show(candidate);
  }

  else if (ourAnimator && ourCurrentAnimation && ourAnimationCreator)
  {
    Fui::noUserInputPlease();
    ourAnimationCreator->finishAllPosMxReading();
    ourAnimationCreator->initAllPosMxReading(ourCurrentAnimation,ourAnimator);
    Fui::okToGetUserInput();
  }
}


void FapAnimationCmds::onSimulationFinished()
{
  if (ourAnimator && ourAnimationCreator && ourCurrentAnimation)
    if (ourCurrentAnimation->isHistoryAnimation())
    {
      Fui::noUserInputPlease();
      SLEEP(500); // To ensure we get the latest results written
      ourAnimationCreator->readAllNewPosMx(ourCurrentAnimation);
      ourAnimationCreator->finishAllPosMxReading();
      FapAnimationCmds::updateAnimator();
#ifdef USE_INVENTOR
      // Tell animator that everything is read
      ourAnimator->postProcess();
#endif
      Fui::okToGetUserInput();
    }
}


void FapAnimationCmds::onResultsToBeDeleted(FFrExtractor*)
{
  FapAnimationCmds::hide();
}


FapAnimationCmds::SignalConnector::SignalConnector() : FFaSwitchBoardConnector("FapAnimationCmds")
{
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_CONNECTED,
			  FFaSlot1S(FapAnimationCmds, onModelMemberConnected, FmModelMemberBase*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_DISCONNECTED,
			  FFaSlot1S(FapAnimationCmds, onModelMemberDisconnected, FmModelMemberBase*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_CHANGED,
			  FFaSlot1S(FapAnimationCmds, onModelMemberChanged, FmModelMemberBase*));
  FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
			  FpRDBExtractorManager::MODELEXTRACTOR_ABOUT_TO_DELETE,
			  FFaSlot1S(FapAnimationCmds, onResultsToBeDeleted, FFrExtractor*));
}
