// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_ANIMATION_CMDS_H
#define FAP_ANIMATION_CMDS_H

#include <string>

#include "FapCmdsBase.H"
#include "FFaLib/FFaDynCalls/FFaSwitchBoard.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"

class FapAnimationCreator;
class FdAnimateModel;
class FmAnimation;
class FmModelMemberBase;
class FFrExtractor;


class FapAnimationCmds : public FapCmdsBase
{
  static void init();

public:
  static void show();
  static void hide();

  static void animationPlay();
  static void animationPlayReverse();
  static void animationPause();
  static void animationStop();
  static void animationStepFwd();
  static void animationStepRwd();
  static void animationToLastFrame();
  static void animationToFirstFrame();

  static void animationType(int type);
  static void animationToggleSkip(bool flag);
  static void animationFrequencyScale(float freqscale);
  static void absoluteAnimationFrequency(float percentage);
  static void animationShowPos(double time);

  static bool exportAnim(bool useAllFrames, bool useRealTime,
			 bool omitNthFrame, bool includeNthFrame,
			 int nthFrameToOmit, int nThFrameToInclude,
			 const std::string& fileName, int fileFormat);

  static bool exportVTF(FmAnimation* anim,
                        const std::string& fileName, int fileFormat,
                        bool firstOrder = false, double timeInc = 0.0);

  static FdAnimateModel* getFdAnimator() { return ourAnimator; }
  static FmAnimation* getCurrentAnimation() { return ourCurrentAnimation; }
  static FmAnimation* findSelectedAnimation();

private:
  static void updateAnimator();

  static void show(FmAnimation* anim, bool showUI = true);
  static void getShowSensitivity(bool& sensitivity);
  static void getHideSensitivity(bool& sensitivity);

public:
  static void onModelExtrDataChanged(FFrExtractor*);
  static void onModelExtrHeaderChanged(FFrExtractor*);
  static void onSimulationFinished();
  static void onResultsToBeDeleted(FFrExtractor*);

private:

  // Vars to keep track of what's going on

  static FdAnimateModel      * ourAnimator;
  static FmAnimation         * ourCurrentAnimation;
  static FapAnimationCreator * ourAnimationCreator;

  // Slots from db

  static void onModelMemberConnected(FmModelMemberBase* item);
  static void onModelMemberDisconnected(FmModelMemberBase* item);
  static void onModelMemberChanged(FmModelMemberBase* item);

  // Signal Connector

  class SignalConnector : public FFaSwitchBoardConnector
  {
  public:
    SignalConnector();
    virtual ~SignalConnector() {}
  };
  static SignalConnector signalConnector;
  friend class SignalConnector;

  // Initializer

  friend class FFaInitialisation<FapAnimationCmds>;
};

static FFaInitialisation<FapAnimationCmds> FapAnimationCmds_Init;

#endif
