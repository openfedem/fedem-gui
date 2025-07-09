// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_PLAY_PANEL_H
#define FUI_PLAY_PANEL_H

#include <array>

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuToggleButton;
class FFuPushButton;
class FFuScale;


class FuiPlayPanel : public virtual FFuComponentBase
{
public:
  void makeSpeedControllsInactive(bool yesOrNo);
  void setSpeedScale(int value);

  bool isPingPong();
  bool isContinous();
  bool isShowAllFrames();
  int  getSpeedScaleValue();

  enum { PLAY_FWD,
	 PLAY_REW,
	 PAUSE,
	 TO_FIRST,
	 TO_LAST,
	 STEP_FWD,
	 STEP_REW,
	 STOP,
	 REALTIME,
	 NBUTTON };

  enum { ONESHOT,
	 CONTINOUS,
	 CONTINOUS_CYCLE,
	 NTOGGLE };

  void setAnimationPressCB  (const FFaDynCB1<int>& aDynCB);
  void setAnimTypeChangedCB (const FFaDynCB1<int>& aDynCB);
  void setRealTimeCB        (const FFaDynCB0& aDynCB);
  void setSpeedScaleChangeCB(const FFaDynCB1<int>& aDynCB);
  void setAllFramesCB       (const FFaDynCB1<bool> & aDynCB);
  void setCloseAnimCB       (const FFaDynCB0& dynCB);

protected:

  void init();

  void playFwdCB();
  void playRwdCB();
  void pauseCB  ();
  void toFirstCB();
  void toLastCB ();
  void stepFwdCB();
  void stepRwdCB();
  void stopCB   ();

  void onPoppedUp();
  void onAnimTypeChanged(bool val);
  void onRealTimePressed();
  void onAllFramesToggled(bool val);
  void onSpeedScaleChanged(int val);

  std::array<FFuToggleButton*,NTOGGLE> myToggles;
  std::array<FFuPushButton*  ,NBUTTON> myButtons;

  FFuScale*      mySpeedScale;
  FFuPushButton* myCloseButton;

private:
  FFaDynCB1<int>  myAnimationPressCB;
  FFaDynCB1<int>  myAnimTypeChangedCB;
  FFaDynCB0       myRealtimePressedCB;
  FFaDynCB1<bool> myAllFramesToggeledCB;
  FFaDynCB1<int>  mySpeedScaleChangedCB;
  FFaDynCB0       closeAnimCB;
};

#endif
