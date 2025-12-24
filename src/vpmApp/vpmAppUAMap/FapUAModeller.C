// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAModeller.H"
#include "vpmApp/vpmAppCmds/FapAnimationCmds.H"

#include "vpmUI/vpmUITopLevels/FuiModeller.H"
#include "vpmUI/vpmUIComponents/Fui3DPoint.H"
#include "vpmUI/vpmUIComponents/FuiPlayPanel.H"
#include "vpmUI/FuiModes.H"

#ifdef USE_INVENTOR
#include "vpmDisplay/FdAnimateModel.H"
#include "vpmDisplay/FdDB.H"
#include "FFaLib/FFaAlgebra/FFaVec3.H"
#endif


namespace
{
  enum PointType { POINT_1, POINT_2 };

  int         whichPoint = 0;
  Fui3DPoint* ourPointUI = NULL;
}


void FapUAModeller::init(FuiModeller* ui)
{
  if (!ui) return;

  ourPointUI = ui->getPickedPointUIC();
  FuiPlayPanel* pp = ui->getPlayPanelUIC();
  if (!ourPointUI || !pp) return;

  ourPointUI->setRefChangedCB(FFaDynCB1S([](bool) { updatePointUI(); }, bool));
#ifdef USE_INVENTOR
  ourPointUI->setPointChangedCB(FFaDynCB2S([](const FaVec3& newPoint, bool isGlobal) {
                                             if (FdDB::setPPoint(whichPoint,isGlobal,newPoint))
                                               FuiModes::notifyCreatePointChange();
                                             updatePointUI();
                                           }, const FaVec3&,bool));

  pp->setAnimTypeChangedCB(FFaDynCB1S([](int animationType) {
                                        switch (animationType) {
                                        case FuiPlayPanel::ONESHOT:
                                          FapAnimationCmds::animationType(FdAnimateModel::ONESHOT);
                                          break;
                                        case FuiPlayPanel::CONTINOUS:
                                          FapAnimationCmds::animationType(FdAnimateModel::LOOP);
                                          break;
                                        case FuiPlayPanel::CONTINOUS_CYCLE:
                                          FapAnimationCmds::animationType(FdAnimateModel::PINGPONG);
                                          break;
                                        }
                                      }, int));
#endif
  pp->setAnimationPressCB(FFaDynCB1S([](int action) {
                                       switch (action) {
                                       case FuiPlayPanel::TO_LAST:
                                         FapAnimationCmds::animationToLastFrame();
                                         break;
                                       case FuiPlayPanel::TO_FIRST:
                                         FapAnimationCmds::animationToFirstFrame();
                                         break;
                                       case FuiPlayPanel::PLAY_REW:
                                         FapAnimationCmds::animationPlayReverse();
                                         break;
                                       case FuiPlayPanel::STEP_REW:
                                         FapAnimationCmds::animationStepRwd();
                                         break;
                                       case FuiPlayPanel::STOP:
                                         FapAnimationCmds::animationStop();
                                         break;
                                       case FuiPlayPanel::PAUSE:
                                         FapAnimationCmds::animationPause();
                                         break;
                                       case FuiPlayPanel::PLAY_FWD:
                                         FapAnimationCmds::animationPlay();
                                         break;
                                       case FuiPlayPanel::STEP_FWD:
                                         FapAnimationCmds::animationStepFwd();
                                         break;
                                       }
                                     }, int));

  pp->setRealTimeCB(FFaDynCB0S([]() {
                                 FapAnimationCmds::absoluteAnimationFrequency(100);
                               }));

  pp->setSpeedScaleChangeCB(FFaDynCB1S([](int value) {
                                         FapAnimationCmds::animationFrequencyScale((float)value);
                                       }, int));

  pp->setCloseAnimCB(FFaDynCB0S(FapAnimationCmds::hide));

  pp->setAllFramesCB(FFaDynCB1S([](bool onOrOff) {
                                  FapAnimationCmds::animationToggleSkip(!onOrOff);
                                }, bool));
}


void FapUAModeller::setNodeChangedCB(const FFaDynCB1<int>& cb)
{
  if (ourPointUI)
    ourPointUI->setNodeChangedCB(cb);
}


void FapUAModeller::updateMode()
{
  switch (FuiModes::getMode())
    {
      // Modes that initially have a default global point
    case FuiModes::MAKEGENERALSPIDER_MODE:
    case FuiModes::MAKEFORCE_MODE:
    case FuiModes::MAKETORQUE_MODE:
    case FuiModes::MAKETRIAD_MODE:
    case FuiModes::MAKEREVJOINT_MODE:
    case FuiModes::MAKEBALLJOINT_MODE:
    case FuiModes::MAKERIGIDJOINT_MODE:
    case FuiModes::MAKESPRING_MODE:
    case FuiModes::MAKEDAMPER_MODE:
    case FuiModes::MAKEFREEJOINT_MODE:
    case FuiModes::MAKECYLJOINT_MODE:
    case FuiModes::MAKEPRISMJOINT_MODE:
    case FuiModes::MAKECAMJOINT_MODE:
    case FuiModes::COMPICKPOINT_MODE:
      whichPoint = POINT_1;
      FapUAModeller::updatePointUI(true);
      break;
    default:
      break;
    }
}

void FapUAModeller::updateState(int newState)
{
  switch (FuiModes::getMode())
    {
    case FuiModes::MAKEGENERALSPIDER_MODE:
      switch (newState)
        {
        case 0:
          whichPoint = POINT_1;
          FapUAModeller::updatePointUI(false);
          break;
        case 1:
          FapUAModeller::cancel();
          break;
        }
      break;

    case FuiModes::PTPMOVE_MODE:
      switch (newState)
        {
        case 0:
          FapUAModeller::cancel();
          break;
        case 1:
          whichPoint = POINT_1;
          FapUAModeller::updatePointUI(false);
          break;
        case 2:
          whichPoint = POINT_2;
          FapUAModeller::updatePointUI(true);
          break;
        case 3:
          whichPoint = POINT_2;
          FapUAModeller::updatePointUI(false);
          break;
        }
      break;

    case FuiModes::MAKEFORCE_MODE:
    case FuiModes::MAKETORQUE_MODE:
    case FuiModes::MAKETRIAD_MODE:
    case FuiModes::MAKEREVJOINT_MODE:
    case FuiModes::MAKEBALLJOINT_MODE:
    case FuiModes::MAKERIGIDJOINT_MODE:
    case FuiModes::COMPICKPOINT_MODE:
      switch (newState)
        {
        case 0:
          whichPoint = POINT_1;
          FapUAModeller::updatePointUI(true);
          break;
        case 1:
          whichPoint = POINT_1;
          FapUAModeller::updatePointUI(false);
          break;
        }
      break;

    case FuiModes::MAKEFREEJOINTBETWEENTRIADS_MODE:
      switch (newState)
        {
        case 2:
          whichPoint = POINT_1;
          FapUAModeller::updatePointUI(true);
          break;
        default:
          FapUAModeller::cancel();
          break;
        }
      break;

    case FuiModes::MAKESPRING_MODE:
    case FuiModes::MAKEDAMPER_MODE:
    case FuiModes::MAKEFREEJOINT_MODE:
    case FuiModes::MAKECYLJOINT_MODE:
    case FuiModes::MAKEPRISMJOINT_MODE:
      switch (newState)
        {
        case 0:
          whichPoint = POINT_1;
          FapUAModeller::updatePointUI(true);
          break;
        case 1:
          whichPoint = POINT_1;
          FapUAModeller::updatePointUI(false);
          break;
        case 2:
          whichPoint = POINT_2;
          FapUAModeller::updatePointUI(true);
          break;
        case 3:
          whichPoint = POINT_2;
          FapUAModeller::updatePointUI(false);
          break;
        }
      break;

    case FuiModes::MAKECAMJOINT_MODE:
      switch (newState)
        {
        case 0:
          whichPoint = POINT_1;
          FapUAModeller::updatePointUI(true);
          break;
        case 1:
          whichPoint = POINT_1;
          FapUAModeller::updatePointUI(false);
          break;
        case 2:
          whichPoint = POINT_1;
          FapUAModeller::updatePointUI(true);
          break;
        case 3:
          whichPoint = POINT_1;
          FapUAModeller::updatePointUI(false);
          break;
        }
      break;

    case FuiModes::MAKESTICKER_MODE:
      switch (newState)
        {
        case 0:
          FapUAModeller::cancel();
          break;
        case 1:
          whichPoint = POINT_1;
          FapUAModeller::updatePointUI(false);
          break;
	}
      break;

    default:
      break;
    }
}


void FapUAModeller::cancel()
{
  if (ourPointUI)
  {
    ourPointUI->setValue(-1);
    ourPointUI->popDown();
  }
}


void FapUAModeller::updatePointUI(int globalOnly, int idx)
{
  if (!ourPointUI) return;

  if (idx >= 0)
    whichPoint = idx;

  if (globalOnly > 0)
    ourPointUI->setGlobal(true);
  else if (globalOnly == 0)
    ourPointUI->setLocal();

#ifdef USE_INVENTOR
  FaVec3 point = FdDB::getPPoint(whichPoint,ourPointUI->isGlobal());

  // Trim small numbers (due to round-off in global-to-local transformations?)
  const double ptol = 1.0e-8;
  for (int i = 0; i < 3; i++)
    if (point[i] < ptol && -point[i] < ptol)
      point[i] = 0.0;

  ourPointUI->setValue(point);
#endif
  ourPointUI->popUp();
}


void FapUAModeller::updateNodeUI(int nodeId, const FaVec3& nodePos)
{
  if (!ourPointUI) return;

  ourPointUI->setLocal(true);
  ourPointUI->setValue(nodePos);
  ourPointUI->setValue(nodeId);
  ourPointUI->popUp();
}
