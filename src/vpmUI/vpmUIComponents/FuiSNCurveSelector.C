// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "vpmUI/vpmUIComponents/FuiSNCurveSelector.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFpLib/FFpFatigue/FFpSNCurveLib.H"


void FuiSNCurveSelector::initWidgets()
{
  stdTypeMenu->setOptionSelectedCB(FFaDynCB1M(FuiSNCurveSelector,this,
                                              onStdValueChanged,int));
  curveTypeMenu->setOptionSelectedCB(FFaDynCB1M(FuiSNCurveSelector,this,
                                                onCurveValueChanged,int));
}


void FuiSNCurveSelector::setSensitivity(bool isSensitive)
{
  stdTypeMenu->setSensitivity(isSensitive);
  curveTypeMenu->setSensitivity(isSensitive);
}


void FuiSNCurveSelector::onStdValueChanged(int)
{
  std::string selectedCurve = curveTypeMenu->getSelectedOptionStr();
  this->populateCurveMenu(stdTypeMenu->getSelectedOptionStr());

  if (!curveTypeMenu->selectOption(selectedCurve))
    curveTypeMenu->selectOption(0);

  this->dataChangedCB.invoke();
}


void FuiSNCurveSelector::onCurveValueChanged(int)
{
  this->dataChangedCB.invoke();
}


std::string FuiSNCurveSelector::getSelectedStd() const
{
  return stdTypeMenu->getSelectedOptionStr();
}


std::string FuiSNCurveSelector::getSelectedCurve() const
{
  return curveTypeMenu->getSelectedOptionStr();
}


void FuiSNCurveSelector::getValues(int& stdIdx, int& curveIdx)
{
  stdIdx   = stdTypeMenu->getSelectedOption();
  curveIdx = curveTypeMenu->getSelectedOption();
}


void FuiSNCurveSelector::setValues(int stdIdx, int curveIdx)
{
  int numStd = stdTypeMenu->getOptionCount();
  if (numStd < 1) return; // The S-N standard menu has not been populated yet

  if (stdIdx < 0 || stdIdx >= numStd)
  {
    if (FFpSNCurveLib::allocated())
      std::cout <<"Warning: SN-curve library has changed since you last saved your model"
                <<"\n         stdIdx = "<< stdIdx << std::endl;
    stdIdx = 0;
  }
  stdTypeMenu->selectOption(stdIdx);

  this->populateCurveMenu(stdTypeMenu->getSelectedOptionStr());

  if (curveIdx < 0 || curveIdx >= curveTypeMenu->getOptionCount())
  {
    if (FFpSNCurveLib::allocated())
      std::cout <<"Warning: SN-curve library has changed since you last saved your model"
                <<"\n         curveIdx = "<< curveIdx << std::endl;
    curveIdx = 0;
  }

  curveTypeMenu->selectOption(curveIdx);
}


void FuiSNCurveSelector::onPoppedUpFromMem()
{
  if (!FFpSNCurveLib::allocated()) return;

  std::vector<std::string> curveStds;
  FFpSNCurveLib::instance()->getCurveStds(curveStds);
  stdTypeMenu->setOptions(curveStds);

  this->populateCurveMenu(stdTypeMenu->getSelectedOptionStr());
}


void FuiSNCurveSelector::populateCurveMenu(const std::string& stdName)
{
  if (!FFpSNCurveLib::allocated()) return;

  std::vector<std::string> curves;
  FFpSNCurveLib::instance()->getCurveNames(curves,stdName);
  curveTypeMenu->setOptions(curves);
}
