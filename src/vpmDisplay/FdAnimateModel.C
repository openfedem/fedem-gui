// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdAnimateModel.H"
#include "vpmDisplay/FdAnimationInfo.H"
#include "vpmDisplay/qtViewers/FdQtViewer.H"
#include "vpmDisplay/FdAnimatedBase.H"
#include "vpmDisplay/FdDB.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmApp/vpmAppUAMap/FapUAGraphView.H"
#endif
#include "vpmApp/vpmAppDisplay/FFaLegendMapper.H"
#include "FFuLib/FFuProgressDialog.H"
#include "FFuLib/FFuAuxClasses/FFuaTimer.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"

#ifdef USE_SMALLCHANGE
#include <SmallChange/nodekits/LegendKit.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/SbString.h>
#endif

#ifdef USE_SIMAGE
#include <Simage/simage.h> // For mpeg (and avi on windows) export
#include <Inventor/SoOffscreenRenderer.h>
#include <Inventor/SbVec2s.h>
#endif

#ifdef win32
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif


FdAnimateModel::FdAnimateModel(float starttime, float endtime)
{
  // Initialize data for keeping track of the timestep structures.

  this->myTimeStepCount = 0;
  this->ts_head = this->ts_tail = this->ts_nextCandidate = NULL;
  this->playRunner = this->lastdisplayed = NULL;

  // Set default values for playback control variables.

  this->scaleFrequency  = 1.0f;
  this->skipFrames      = true;

  this->animationType   = FdAnimateModel::ONESHOT;
  this->reversed        = false;
  this->continousPlay   = false;
  this->pauseModus      = false;
  this->resetFlag       = false;
  this->IHaveInitedAnimObjs = false;

  this->nrOfSeconds     = -1;
  this->startTime       = starttime;
  this->endTime         = endtime;

  this->maxTimeStep     = -1;
  this->minTimeStep     = -1;

  myTimer = NULL;

  IAmShowingProgress     = true;
  IAmShowingLinkMotion   = false;
  IAmShowingTriadMotion  = false;
  IAmShowingFringes      = false;
  IAmShowingDeformations = false;
  myDeformationScale     = -1.0;
}


FdAnimateModel::~FdAnimateModel(void)
{
  this->stop();

  amTimestepNode *runner = this->ts_head;
  while(runner) {
    this->ts_head = runner->next;
    delete runner;
    runner = this->ts_head;
  }

  FdAnimationInfo *infonode = FdDB::getAnimInfoNode();
  if(infonode) infonode->isOn.setValue(false);
  this->showLegend(false);
}


void FdAnimateModel::setAnimationObjects(const std::vector<FdAnimatedBase*>& objs)
{
  myObjsToAnimate = objs;
  for (FdAnimatedBase* obj : objs)
  {
    obj->showFringes(IAmShowingFringes);
    obj->showDeformations(IAmShowingDeformations);
    obj->showLinkMotion(IAmShowingLinkMotion);
    obj->showTriadMotion(IAmShowingTriadMotion);
  }
}

unsigned long FdAnimateModel::addFrame(float time,  bool doShowIt)
{
  amTimestepNode *newnode = this->insertFrameInList(time);
   
  if (doShowIt)
    {
      this->initAnimation();
      this->setFrame(newnode);
    }

  return newnode->frameIdx;
}

/*!
  An initialization routine to be called when
  all frames are loaded, and we are finished 
  with progress animation.
*/

bool FdAnimateModel::postProcess(void)
{
  // Turn off Animation info if there :

  FdAnimationInfo *infonode = FdDB::getAnimInfoNode();
  if(infonode) infonode->isOn.setValue(false);

  // Set the playrunner to first frame

  //this->lastdisplayed = this->playRunner = this->ts_head;
  
  // Find max/min timestep to be used by realtime anim.
  
  this->findMaxMinTimeStep();

  // Reset the animated objects too show initial configuration

  this->resetAnimation();

  return true;
}

/*
  Sets the animation to display the frame closest to the 
  provided time. If isProgressMove is true, then
  it is considered whether the user has allowed progress
  animation.
*/

bool FdAnimateModel::moveToTime(float time, bool isProgressMove)
{
  if (!IAmShowingProgress && isProgressMove) return true;

  amTimestepNode *node = this->ts_head;
  if (!node) return false;

  while (node->next && node->next->accumTime < time)
    node = node->next;

  // find the closest node of the two (node and node->next)
  if (node->next)
    if (fabs(node->next->accumTime - time) < fabs(node->accumTime - time))
      node = node->next;

  this->playRunner = node;
  this->initAnimation();
  this->setFrame(this->playRunner);
  return true;
}

bool FdAnimateModel::moveToTimeStep(int stepNo)
{
  if (stepNo > this->myTimeStepCount) return false;

  amTimestepNode* node = this->ts_head;
  for (int i = 0; i < stepNo && node; i++)
    node = node->next;

  this->playRunner = node;
  this->initAnimation();
  this->setFrame(this->playRunner);
  return true;
}


// Uses a value from 0 to 100 to specify the scale factor. 
// 50 is real time.
// (values are bound to the scale in FuiAnimationControl)

void FdAnimateModel::setSmartScaleFactor(float factor)
{
  // Specifies the factor by which the clock is scaled with.
  
  const float maxAnimationTime = 2.5f;
  const float maxSpeedFactor   = 3.0f;
  const float maxFrameTime     = 0.5f;

  float totalTime = this->endTime - this->startTime;

  float fastFactor = maxSpeedFactor;
  if (maxAnimationTime*maxSpeedFactor < totalTime)
    fastFactor = totalTime / maxAnimationTime;

  if (this->maxTimeStep < 0)
    this->findMaxMinTimeStep();

  float slowFactor = 1.0f;
  if (maxFrameTime > maxTimeStep)
    slowFactor = maxTimeStep / maxFrameTime;
  
  if (factor >= 50.0f) {
    // Normalize to [0,1].
    factor = factor/50.0f - 1.0f;
    // Exponential ^2 control.
    factor *= factor;
    // Normalize to [1, fastFactor]
    factor = factor * (fastFactor-1.0f) + 1.0f;
  }
  else {
    // Normalize to [1,0].
    factor = (99.0f-factor)/50.0f - 1.0f;
    // Exponential ^3 control.
    factor *= factor*factor;
    // Normalize to [slowFactor, 1].
    factor = 1.0f/(factor * (1.0f/slowFactor - 1.0f) + 1.0f);
  }

  this->scaleFrequency = factor;
}

/*!
  Uses percentage of real time for scaling.  100% is real time.
*/

void FdAnimateModel::setScaleFrequency(float freqscale)
{
  this->scaleFrequency = freqscale/100.0f;
}

/*!
  Set whether to skip frames or not if display framerate can't keep up.
*/

void FdAnimateModel::setSkipFrames(bool flag)
{
  this->skipFrames = flag;
  if (flag) return;

  this->nextFrameSwapTime = 0.0f;
  this->resetTime();
}

/*!
  Set animation type (which is either loop-around, oneshot or ping-pong).
*/

void FdAnimateModel::setAnimationType(FdAnimType animType)
{
  this->animationType = animType;
}

float FdAnimateModel::getCurrentTime(void)
{
  if (this->lastdisplayed && this->myTimeStepCount)
    return this->lastdisplayed->accumTime;
  else
    return 0.0f;
}

unsigned long FdAnimateModel::getCurrentStep(void)
{
  if (this->lastdisplayed && this->myTimeStepCount)
    return this->lastdisplayed->stepNr;
  else
    return 0;
}

float FdAnimateModel::getProgressValue(void)
{
  if (!this->myTimeStepCount || !this->lastdisplayed || !this->ts_head)
    return 0.0f;
  else if (this->ts_tail && this->ts_tail != this->ts_head)
    return ((this->lastdisplayed->accumTime - this->ts_head->accumTime)/
            (this->endTime - this->ts_head->accumTime));
  else if (this->endTime > this->startTime)
    return ((this->lastdisplayed->accumTime - this->ts_head->accumTime)/
            (this->endTime - this->startTime));
  else
    return 0.0f;
}


float FdAnimateModel::readTime(bool reset)
{
#ifdef win32
  struct _timeb timebuffer;
  _ftime(&timebuffer);
  long int seconds  = timebuffer.time;
  long int millisec = timebuffer.millitm;
#else
  struct timeval currenttime;
  gettimeofday(&currenttime,NULL);
  long int seconds  = currenttime.tv_sec;
  long int millisec = currenttime.tv_usec/1000;
#endif

  // If this is the first call of this method,
  // remember the number of seconds for subtraction

  if (nrOfSeconds < 0 || reset)
  {
    nrOfSeconds = seconds;
    nrOfMilliSeconds = millisec;
    return accumTime = lastTime = 0.0f;
  }

  float currentTime = ((float)(seconds  - nrOfSeconds) +
                       (float)(millisec - nrOfMilliSeconds) / 1000.0f);
  accumTime += (currentTime - lastTime) * scaleFrequency;
  lastTime = currentTime;
  return accumTime;
}


void FdAnimateModel::runAnimation(void)
{
  // Make sure we wait until the next frame should be displayed.

  if (this->readTime() < this->nextFrameSwapTime) return;

  if (!this->playRunner)
  {
    if (this->ts_head)
      this->playRunner = this->ts_head;
    else
      return;
  }

  this->nextFrameSwapTime += this->playRunner->activeTime;
  
  // Set the frame:

  this->setFrame(playRunner);
 
  // Set current time to a small value to make the do-while loop
  // exit on first iteration, or update after showing the frame
  float currentTime = this->skipFrames ? this->readTime() : -1.0f;
  float currentFrameSwapTime = this->nextFrameSwapTime;
  
  // Loop to find a frame that fits the time 

  do {
    // Set play runner to the correct "next frame"
    playRunner = reversed ? playRunner->prev : playRunner->next;

    // If we came to an end, decide how to wrap the animation:
      
    if (!this->playRunner) 
      {
        // One Shot Animation done?
        
        if (this->animationType == FdAnimateModel::ONESHOT) 
          {  
            // Then select the last frame

            if (reversed)
              this->playRunner = this->ts_head;
            else {
              this->playRunner = this->ts_tail;
              this->showProgressAnimation(true);
            }
            
            // And stop it all if we already show it:
            // (if not we'll stop next time)

            if (this->lastdisplayed == this->playRunner)
              {
                this->pauseModus = false;  
                this->continousPlay = false;
                removeAnimationTimer();
              }

            // Jump out of the do-while

            break;
          }
        else
          {
            if (animationType == FdAnimateModel::PINGPONG)
              reversed = !reversed;

            playRunner = reversed ? ts_tail : ts_head;
            
            // Update time control by resetting time and time to next frame swap:
            
            this->nextFrameSwapTime = this->playRunner->activeTime;
            this->resetTime();
            
            // Add this frame time up for the while in the bottom 
            
            currentFrameSwapTime += this->playRunner->activeTime;
          }
      }
    else 
      {
        this->nextFrameSwapTime += this->playRunner->activeTime;
        currentFrameSwapTime += this->playRunner->activeTime;
      }
  }
  while (currentTime + 0.025f*this->scaleFrequency >= currentFrameSwapTime);

  if (this->skipFrames) return;

  this->resetTime();
  this->nextFrameSwapTime = this->playRunner->activeTime;
}


/*!
  Set the animated objects to a specified frame of the animation
  The "core" of the animations.
*/

void FdAnimateModel::setFrame(amTimestepNode *node)
{
  // Turn off automatical redrawing.

  FdQtViewer *viewer = FdDB::getViewer();
  SbBool autoredraw = viewer->isAutoRedraw();
  viewer->setAutoRedraw(false);

  // Set animated objs to new frame

  this->lastdisplayed = node;

  if (node)
    {
      for (FdAnimatedBase* obj : myObjsToAnimate)
	obj->selectAnimationFrame(node->frameIdx);
#ifdef FT_HAS_GRAPHVIEW
      FapUAGraphView::setAnimationTimeAllGraphs(node->accumTime);
#endif

      // Update information node in the Inventor scene graph.
      
      FdAnimationInfo *infonode = FdDB::getAnimInfoNode();
      if(infonode) 
	{
	  infonode->isOn.setValue(true);
	  infonode->step.setValue(node->stepNr);
	  infonode->time.setValue(node->accumTime);
	  
	  if((this->ts_head) && (this->ts_tail != this->ts_head))
	    infonode->progress.setValue((node->accumTime -
					 this->ts_head->accumTime)/
					(this->endTime -
					 this->ts_head->accumTime));
	  else if (this->ts_head && (this->endTime != this->startTime) )
	    infonode->progress.setValue((node->accumTime - this->ts_head->accumTime)/
					(this->endTime - this->startTime));
	  else 
	    infonode->progress.setValue(0);
	}
    } 
  // Render current Inventor scene.
  
  // viewer->render(); // Not needed because setAutoRedr does it
  
  // Reset autoredraw state.
  
  viewer->setAutoRedraw(autoredraw);
}

//////////////////////////////////////////////////////////////////////////
//
// These are the functions (used from FpPM-callbacks) which interfaces
// the animation class against it's GUI.
//
//////////////////////////////////////////////////////////////////////////

/*!
  Play animation continously in the forward direction.
*/

void FdAnimateModel::playForward(void)
{
  if(!this->myTimeStepCount) return;

  this->initAnimation();
  this->showProgressAnimation(false);

  this->reversed = false;
  this->pauseModus = false;
  
  if (!this->playRunner || (this->playRunner == this->ts_tail))
    this->playRunner = this->ts_head;
  
  if(!this->continousPlay) this->addAnimationTimer();
}

/*!
  Play animation continously in the reverse direction.
*/

void FdAnimateModel::playReverse(void)
{
  if(!this->myTimeStepCount) return;

  this->initAnimation();
  this->showProgressAnimation(false);

  this->reversed = true;
  this->pauseModus = false;
   
  if (!this->playRunner || (this->playRunner == this->ts_head))
    this->playRunner = this->ts_tail;

  if(!this->continousPlay) this->addAnimationTimer();
}

/*!
  Jump to the first animation frame.
*/

void FdAnimateModel::stepFirst(void)
{
  if(!this->myTimeStepCount) return;

  this->initAnimation();
  this->showProgressAnimation(false);

  this->removeAnimationTimer();
  this->continousPlay = false;

  this->playRunner = this->ts_head;
  this->setFrame(this->playRunner);
}
/*!
  Jump to the last animation frame.
*/

void FdAnimateModel::stepLast(void)
{
  if(!this->myTimeStepCount) return;

  this->initAnimation();
  this->removeAnimationTimer();
  this->continousPlay = false;
  this->showProgressAnimation(true);

  this->playRunner = this->ts_tail;
  this->setFrame(this->playRunner);
}

/*!
  Step one animation frame forward.
*/

void FdAnimateModel::stepForward(void)
{
  if(!this->myTimeStepCount) return;

  this->initAnimation();
  this->showProgressAnimation(false);

  this->removeAnimationTimer();
  this->continousPlay = false;

  // Find correct node in list.

  if (this->lastdisplayed)
    this->playRunner = this->lastdisplayed->next;
  else
    this->playRunner = this->ts_head;
  
  if(!this->playRunner) this->playRunner = this->ts_head;

  this->setFrame(this->playRunner);
}

/*!
  Step one animation frame in the reverse direction.
*/

void FdAnimateModel::stepReverse(void)
{
  if(!this->myTimeStepCount) return;

  this->initAnimation();
  this->showProgressAnimation(false);

  this->removeAnimationTimer();
  this->continousPlay = false;

  // Find correct node in list.

  if (this->lastdisplayed)
    this->playRunner = this->lastdisplayed->prev;
  else
    this->playRunner = this->ts_tail;

   if(!this->playRunner) this->playRunner = this->ts_tail;

  this->setFrame(this->playRunner);
}

/*!
  Pause the animation replay.
*/

void FdAnimateModel::pause(void)
{
  if(!this->myTimeStepCount) return;

  this->initAnimation();
  this->showProgressAnimation(false);

  if(this->continousPlay) {
    this->continousPlay = false;
    this->pauseModus = true;
    this->removeAnimationTimer();
  }
  else if(this->pauseModus) {
    this->pauseModus = false;
     this->addAnimationTimer();
     this->runAnimation();//?? JJS Really Needed ?
  }
}

/*!
  Stop the animation replay.
*/

void FdAnimateModel::stop(void)
{
  if(!this->myTimeStepCount) return;
  
  this->showProgressAnimation(false);

  this->pauseModus = false;
  if(this->continousPlay) {
    this->continousPlay = false;
    this->removeAnimationTimer();
  }
  
  this->resetAnimation();
}

////////////////////////////////////////////////////////////


void FdAnimateModel::controlpanelClosed(void)
{
  if(!this->myTimeStepCount) return;

  // Code to execute when the animation control panel is closed.

  this->continousPlay = false;
  this->pauseModus    = false;
  this->resetFlag     = true;

  this->removeAnimationTimer();

  this->resetAnimation();

  // NB!! These should *not* be called if the state for the 
  // GUI window for the animatemodel object is kept over 
  // popdown/popup calls.

  this->setScaleFrequency(100.0f);
  this->setSkipFrames(true);
  this->setAnimationType(FdAnimateModel::LOOP);
}

/*!
  Reset aniamtion on all objects to "current" 
  value set in the FdDisplay module.
*/

void FdAnimateModel::resetAnimation(void)
{
  if (!IHaveInitedAnimObjs) return;

  for (FdAnimatedBase* obj : myObjsToAnimate)
    obj->resetAnimation();

#ifdef FT_HAS_GRAPHVIEW
  FapUAGraphView::resetAnimationAllGraphs();
#endif

  IHaveInitedAnimObjs = false;

  this->lastdisplayed = this->playRunner = NULL;

  FdAnimationInfo *node = FdDB::getAnimInfoNode();
  if(node)
    {
      node->isOn.setValue(false);
      node->step.setValue(0);
      node->time.setValue(0);
      node->progress.setValue(0);
    }
}

/*!
  Turn on animation on all objects.
*/

void FdAnimateModel::initAnimation(void)
{
  if (IHaveInitedAnimObjs) return;

  for (FdAnimatedBase* obj : myObjsToAnimate)
    obj->initAnimation();

#ifdef FT_HAS_GRAPHVIEW
  FapUAGraphView::initAnimationAllGraphs();
#endif

  IHaveInitedAnimObjs = true;

  FdAnimationInfo *node = FdDB::getAnimInfoNode();
  if(node) node->isOn.setValue(true);
}

/*!
  Tell the animated objects their animation data is not needed any more.
*/

void FdAnimateModel::deleteAnimationData(void)
{
  this->resetAnimation();

  FFaMsg::enableProgress(myObjsToAnimate.size());
  int count = 0;

  for (FdAnimatedBase* obj : myObjsToAnimate) {
    obj->deleteAnimationData();
    FFaMsg::setProgress(++count);
  }

  FFaMsg::disableProgress();
}


void linkToFollowField() // Follow-me camera
{
  // Empty for now
}

void FdAnimateModel::showLinkMotion(bool doShow)
{
  if (IAmShowingLinkMotion == doShow) return;

  for (FdAnimatedBase* obj : myObjsToAnimate)
    obj->showLinkMotion(doShow);

  IAmShowingLinkMotion = doShow;
}

void FdAnimateModel::showTriadMotion(bool doShow)
{
  if (IAmShowingTriadMotion == doShow) return;

  for (FdAnimatedBase* obj : myObjsToAnimate)
    obj->showTriadMotion(doShow);

  IAmShowingTriadMotion = doShow;
}

void FdAnimateModel::showDeformations(bool doShow)
{
  if (IAmShowingDeformations == doShow) return;

  for (FdAnimatedBase* obj : myObjsToAnimate)
    obj->showDeformations(doShow);

  IAmShowingDeformations = doShow;
}

void FdAnimateModel::setDeformationScale(double scale)
{
  if (myDeformationScale == scale) return;

  for (FdAnimatedBase* obj : myObjsToAnimate)
    obj->setDeformationScale(scale);

  myDeformationScale = scale;
}

void FdAnimateModel::showFringes(bool doShow)
{
  if (IAmShowingFringes == doShow) return;

  for (FdAnimatedBase* obj : myObjsToAnimate)
    obj->showFringes(doShow);

  IAmShowingFringes = doShow;
}

void FdAnimateModel::setFringeLegendMapping(const FFaLegendMapper& lmap,
                                            bool update)
{
  if (myLegendMapping == lmap) return;

  myLegendMapping = lmap;
  if (!update) return;

#ifdef USE_SMALLCHANGE
  std::vector<FFaLegendMapper::Tick> ticks;
  myLegendMapping.getTicks(ticks);
  bool isSmoot = myLegendMapping.isSmooth();

  FdDB::getLegend()->setDiscreteMode(false);
  FdDB::getLegend()->setColorCB(myLegendMapping.getColorCB());
  FdDB::getLegend()->clearTicks();

  for (const FFaLegendMapper::Tick& tck : ticks)
    if (!tck.useDescription)
      FdDB::getLegend()->addSmallTick(tck.normalizedTickPos);
    else if (tck.text == "")
      FdDB::getLegend()->addBigTick(tck.normalizedTickPos,tck.actualValue);
    else
    {
      SbString discreteDescr(tck.text.c_str());
      FdDB::getLegend()->addBigTick(tck.normalizedTickPos,
                                    tck.actualValue,&discreteDescr);
    }

  FdDB::getLegend()->setDiscreteMode(!isSmoot);
#endif

  for (FdAnimatedBase* obj : myObjsToAnimate)
    obj->setFringeLegendMapping(myLegendMapping);
}


#ifdef USE_SMALLCHANGE
void FdAnimateModel::showLegend(bool doShow)
{
  FdDB::getLegend()->on.setValue(doShow);
}

void FdAnimateModel::showLegendBar(bool doShow)
{
  FdDB::getLegend()->enableImage(doShow);
}

void FdAnimateModel::setLegendDescription(const std::vector<std::string>& lines)
{
  SoMFString& ourDescription = FdDB::getLegend()->description;
  ourDescription.deleteValues(0);
  ourDescription.insertSpace(0,lines.size());
  for (size_t i = 0; i < lines.size(); i++)
    ourDescription.set1Value(i,lines[i].c_str());
}
#else
void FdAnimateModel::showLegend(bool) {}
void FdAnimateModel::showLegendBar(bool) {}
void FdAnimateModel::setLegendDescription(const std::vector<std::string>&){}
#endif


void FdAnimateModel::removeAnimationTimer(void)
{
  if (!myTimer) return;

  myTimer->stop();
  delete myTimer;
  myTimer = NULL;
}

void FdAnimateModel::addAnimationTimer(void)
{
  // Initialize variables before starting loop.

  this->continousPlay = true;
  this->resetTime();
  this->nextFrameSwapTime = 0.0f;
  
  if (myTimer)
    this->removeAnimationTimer();
  
  myTimer = FFuaTimer::create(FFaDynCB0M(FdAnimateModel, this, runAnimation));
  myTimer->start(25);
}


/*!
  Initializes the internal max/minTimeStep vars
*/

void FdAnimateModel::findMaxMinTimeStep()
{
  amTimestepNode *node = this->ts_head;

  if (!node) return;

  maxTimeStep = minTimeStep = node->activeTime;

  while (node->next)
    {
      if (node->activeTime > maxTimeStep) maxTimeStep = node->activeTime;
      if (node->activeTime < minTimeStep) minTimeStep = node->activeTime;
      node = node->next; 
    }
}

void FdAnimateModel::renumberStepNodes()
{
  amTimestepNode *runner = this->ts_head;
  unsigned long i = 0;
  while (runner)
    {
      runner->stepNr = i++;
      runner = runner->next;
    }
}


/*!
  Inserts the newnode into the frame list, If it is not
  there already. In that case the original node are returned.
*/

FdAnimateModel::amTimestepNode * FdAnimateModel::insertFrameInList(float time)
{
  amTimestepNode *runner = this->ts_nextCandidate;

  amTimestepNode *newnode = new amTimestepNode;
  newnode->prev = newnode->next = NULL;
  newnode->stepNr = newnode->frameIdx = myTimeStepCount;
  newnode->accumTime = time;

  if (this->ts_head)
    {
      // check if we have a time earler that last timestep - move
      // to front of list if we have that.
      if (runner && (runner->accumTime > time))
	runner = this->ts_head;

      // move forward until we pass the previous timestep. Usually only one step...
      while (runner  && (runner->accumTime < time))
	  runner = runner->next;
      
      if (runner) // insert in list
	{
	  if (runner->accumTime ==  time) // exact position
            {
	      delete newnode;
	      this->ts_nextCandidate = runner;
              return runner;
            }
	  else // middle of list
	    {
	      newnode->prev = runner;
	      newnode->next = runner->next;
	      runner->next = newnode;
	      if (newnode->next)
		newnode->next->prev = newnode;
	      else
		this->ts_tail = newnode;

	      myTimeStepCount++;
	    }
	}
      else // end of list
	{
	  newnode->prev = this->ts_tail;
	  this->ts_tail->next = newnode;
	  this->ts_tail = newnode;

	  myTimeStepCount++;
	}
    }
  else // first object 
    {
      this->ts_head = this->ts_tail = newnode;
      // this->playRunner = this->lastdisplayed = this->ts_head;
      myTimeStepCount ++;
    }
 
  // calculate node information
  if(newnode->prev)
    {
      newnode->prev->activeTime = newnode->accumTime - newnode->prev->accumTime;
      newnode->activeTime       = newnode->prev->activeTime;
    }
  else
    newnode->activeTime = 0.1f;

  // remember the last position
  this->ts_nextCandidate = newnode;
  return newnode;
}


bool FdAnimateModel::exportAnim(bool useAllFrames, bool useRealTime,
                                bool omitNthFrame, bool includeNthFrame,
                                int nthFrameToOmit, int nThFrameToInclude,
                                const std::string& fileName, int fileFormat)
{
  // Available export formats
  enum { MPEG1, MPEG2, AVI };

  int numFrames = myTimeStepCount;
  if (numFrames < 1) return false;

#ifdef USE_SIMAGE
  float frameRate = 30.0f; // [Hz]
  float invFrameRate = 1.0f/frameRate;
  float stepSize = this->ts_head ? this->ts_head->activeTime : 0.0f;

  // Check if we can read the first frame
  if (!this->moveToTimeStep(0)) return false;

  FdQtViewer* viewer = FdDB::getViewer();
  viewer->render();
  SbBool autoRedraw = viewer->isAutoRedraw();
  viewer->setAutoRedraw(false);

  int width  = viewer->getWidth();
  int height = viewer->getHeight();

  // mpeg: width & height divisible by 2
  // avi:  width & height divisible by 4
  int divisor = fileFormat == AVI ? 4 : 2;
  width  -= (width%divisor);
  height -= (height%divisor);

  // Creating offscreen renderer
  SoOffscreenRenderer* renderer = new SoOffscreenRenderer(SbVec2s(width,height));
  renderer->setBackgroundColor(viewer->getBackgroundColor());

  std::vector<int> frameCounts;

  if (useAllFrames)
    frameCounts.resize(numFrames,1);
  else if (useRealTime)
  {
    if (invFrameRate < stepSize) // must repeat frames
      frameCounts.resize(numFrames,(int)(stepSize/invFrameRate));
    else // must leave out some frames
    {
      frameCounts.resize(numFrames,0);
      double onlyNth = invFrameRate/stepSize;
      for (double c = 0.0; (size_t)c < frameCounts.size(); c += onlyNth)
        frameCounts[(size_t)c] = 1;
    }
  }
  else if (omitNthFrame)
  {
    frameCounts.resize(numFrames,1);
    for (size_t i = 0; i < frameCounts.size(); i += nthFrameToOmit)
      frameCounts[i] = 0;
  }
  else if (includeNthFrame)
  {
    frameCounts.resize(numFrames,0);
    for (size_t i = 0; i < frameCounts.size(); i += nThFrameToInclude)
      frameCounts[i] = 1;
  }

  // Calculating total number of frames to export
  int totalFrames = 0;
  for (int nfr : frameCounts) totalFrames += nfr;

  // Setting up export parameters
  s_params* params = s_params_create();

  switch (fileFormat) {
  case MPEG1:
    s_params_set(params,
                 "mime-type", S_STRING_PARAM_TYPE, "video/mpeg",
                 "width", S_INTEGER_PARAM_TYPE, width,
                 "height", S_INTEGER_PARAM_TYPE, height,
                 "num frames", S_INTEGER_PARAM_TYPE, totalFrames,
                 "mpeg1", S_BOOL_PARAM_TYPE, 1,
                 NULL);
    break;

  case MPEG2:
    s_params_set(params,
                 "mime-type", S_STRING_PARAM_TYPE, "video/mpeg",
                 "width", S_INTEGER_PARAM_TYPE, width,
                 "height", S_INTEGER_PARAM_TYPE, height,
                 "num frames", S_INTEGER_PARAM_TYPE, totalFrames,
                 "mpeg1", S_BOOL_PARAM_TYPE, 0,
                 NULL);
    break;

  case AVI:
    s_params_set(params,
                 "mime-type", S_STRING_PARAM_TYPE, "video/avi",
                 "width", S_INTEGER_PARAM_TYPE, width,
                 "height", S_INTEGER_PARAM_TYPE, height,
                 "num frames", S_INTEGER_PARAM_TYPE, totalFrames,
                 "fps", S_INTEGER_PARAM_TYPE,frameRate,
                 NULL);
    break;

  default:
    this->stop();
    viewer->setAutoRedraw(autoRedraw);
    delete renderer;
    return false;
  }

  s_params* imgparams = s_params_create();
  s_params_set(imgparams,
               "allow image modification", S_INTEGER_PARAM_TYPE, 1,
               NULL);

  s_movie* movie = s_movie_create(fileName.c_str(), params);
  if (!movie)
  {
    this->stop();
    viewer->setAutoRedraw(autoRedraw);
    FFaMsg::dialog("Could not export animation to\n" + fileName,
                   FFaMsg::DISMISS_ERROR);
    remove(fileName.c_str());
    if (imgparams)
      s_params_destroy(imgparams);
    if (params)
      s_params_destroy(params);
    delete renderer;
    return false;
  }

  FFaMsg::pushStatus("Exporting Animation");

  FFuProgressDialog* progDlg = FFuProgressDialog::create("Please wait...", "Cancel",
                                                         "Exporting Animation", numFrames);

  for (int frameIdx = 0; frameIdx < numFrames; frameIdx++)
  {
    progDlg->setCurrentProgress(frameIdx);
    if (progDlg->userCancelled())
      break;

    int repeat = frameCounts[frameIdx];
    if (repeat > 0)
    {
      if (!this->moveToTimeStep(frameIdx))
        break;

      renderer->render(viewer->getSceneManager()->getSceneGraph());
      s_image* image = s_image_create(width, height, 1, renderer->getBuffer());
      for (int count = 0; count < repeat; count++)
        s_movie_put_image(movie, image, imgparams);
      s_image_destroy(image);
    }
  }
  progDlg->setCurrentProgress(numFrames);

  this->stop();
  viewer->setAutoRedraw(autoRedraw);

  // Clean-up

  delete progDlg;

  s_movie_close(movie);
  s_movie_destroy(movie);

  if (imgparams)
    s_params_destroy(imgparams);
  if (params)
    s_params_destroy(params);

  delete renderer;

  FFaMsg::popStatus();
  return true;
#else
  std::cerr <<" *** FdAnimateModel::export: Not available."<< std::endl;
  return false;
#endif
}
