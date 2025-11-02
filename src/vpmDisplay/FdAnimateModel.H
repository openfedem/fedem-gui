// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_ANIMATE_MODEL_H
#define FD_ANIMATE_MODEL_H

#include <vector>

#include "vpmApp/vpmAppDisplay/FFaLegendMapper.H"

class FFuaTimer;
class FdAnimatedBase;


/*!
  This class was made by Morten Eriksen, Norsk Simulator Senter, 13.Dec.1996
  Redesigned by Jacob Storen, 26.Dec.2000
*/

class FdAnimateModel
{
public:
  FdAnimateModel(float starttime, float endtime);
  ~FdAnimateModel(void);

  // Initialization

  void setAnimationObjects(const std::vector<FdAnimatedBase*>& objsToAnimate);
  unsigned long addFrame(float time, bool doShowIt = false);
  bool postProcess(void);

  void setProgressIntv(float t0, float t1) { startTime = t0; endTime = t1; }

  // Cleaning up

  void controlpanelClosed(void);
  void deleteAnimationData();

  // User interaction

  void playForward(void);
  void playReverse(void);
  void stepFirst(void);
  void stepLast(void);
  void stepForward(void);
  void stepReverse(void);
  void pause(void);
  void stop(void);

  bool moveToTimeStep(int stepNo);
  bool moveToTime(float timeStep, bool isProgressMove = false);

  void setScaleFrequency(float ft);
  void setSmartScaleFactor(float ft);
  void setSkipFrames(bool flag);

  enum FdAnimType {ONESHOT, LOOP, PINGPONG};
  void setAnimationType(FdAnimType animType);

  void linkToFollowField(); // Follow-me camera

  void showLinkMotion(bool doShow);
  void showTriadMotion(bool doShow);
  void showDeformations(bool doShow);
  void setDeformationScale(double scale);

  void showFringes(bool doShow);
  void setFringeLegendMapping(const FFaLegendMapper& lmap, bool update = true);
  const FFaLegendMapper& getLegendMapping() const { return myLegendMapping; }
  void showLegendBar(bool doShow);
  void showLegend(bool doShow);
  void setLegendDescription(const std::vector<std::string>& lines);

  // State information

  float getCurrentTime(void);
  unsigned long getCurrentStep(void);
  float getProgressValue(void);
  void  showProgressAnimation(bool doShowIt) { IAmShowingProgress = doShowIt; }

  bool hasMultiSteps() const { return myTimeStepCount > 1; }

  bool exportAnim(bool useAllFrames, bool useRealTime,
                  bool omitNthFrame, bool includeNthFrame,
                  int nthFrameToOmit, int nThFrameToInclude,
                  const std::string& fileName, int fileFormat);

private:
  struct amTimestepNode
  {
    amTimestepNode *next;
    amTimestepNode *prev;
    float activeTime;
    float accumTime;
    unsigned long stepNr;
    unsigned long frameIdx;
  };

  void  resetAnimation(void);
  void  initAnimation(void);
  void  setFrame(amTimestepNode *node);
  void  runAnimation(void);

  void  resetTime() { this->readTime(true); }
  float readTime(bool reset = false);

  void  findMaxMinTimeStep();
  void  renumberStepNodes();
  void  removeAnimationTimer(void);
  void  addAnimationTimer(void);

  FFaLegendMapper myLegendMapping;

  bool   IAmShowingLinkMotion;
  bool   IAmShowingTriadMotion;
  bool   IAmShowingFringes;
  bool   IAmShowingDeformations;
  double myDeformationScale;
  bool   IAmShowingProgress;

  // Animation frame list management

  int myTimeStepCount;
  amTimestepNode *ts_head;
  amTimestepNode *ts_tail;
  amTimestepNode *ts_nextCandidate;
  amTimestepNode *insertFrameInList(float time);

  // Current, and last shown frame

  amTimestepNode *playRunner;
  amTimestepNode *lastdisplayed;

  // The objects to animate

  std::vector<FdAnimatedBase*> myObjsToAnimate;

  // Animation mode and state variables

  FdAnimType animationType;
  bool reversed;
  bool continousPlay;
  bool pauseModus;
  bool resetFlag;
  bool skipFrames;

  bool IHaveInitedAnimObjs;

  // Time control variables

  float scaleFrequency;
  float lastTime;
  float accumTime;
  long  nrOfSeconds;
  long  nrOfMilliSeconds;
  float nextFrameSwapTime;

  float startTime;
  float endTime;
  float maxTimeStep;
  float minTimeStep;

  FFuaTimer* myTimer;
};

#endif
