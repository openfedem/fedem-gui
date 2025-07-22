// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiMotionType.H"
#include "FFuLib/FFuRadioButton.H"
#include "FFuLib/FFuToggleButton.H"


FuiMotionType::FuiMotionType() : IAmSensitive(true)
{
  myMotionTypeLabels = {
    "Free",
    "Fixed",
    "Prescribed",
    "Spring-Damper"
  };
}


void FuiMotionType::initWidgets()
{
  for (size_t i = 0; i < myMotionTypeButtons.size(); i++)
  {
    myMotionTypeButtons[i]->setLabel(myMotionTypeLabels[i]);
    myMotionTypeToggleGroup.insert(myMotionTypeButtons[i]);
  }

  myMotionTypeToggleGroup.setGroupToggleCB(FFaDynCB2M(FuiMotionType,this,
						      onMotionTypeChanged,int,bool));

  myAddButton->setLabel("Additional BC");
  myAddButton->setToggleCB(FFaDynCB1M(FuiMotionType,this,onButtonToggled,bool));
  myAddButton->setToolTip("Fix in initial equilibirum analysis only\n"
			  "and optionally in eigenmode analysis");
}


void FuiMotionType::setChangedCB(const FFaDynCB0& aDynCB)
{
  myChangedCB = aDynCB;
}


void FuiMotionType::setValuesChangedCB(const FFaDynCB1<int>& aDynCB)
{
  myValuesChangedCB = aDynCB;
}


void FuiMotionType::setValue(unsigned int motionType)
{
  bool addBC = false;
  if (motionType == 3)
  {
    addBC = true;
    motionType = 0;
  }
  else if (motionType == 4 || motionType == 5)
  {
    addBC = motionType == 5;
    motionType = 3;
  }

  if (motionType == 1 || motionType == 2)
  {
    myAddButton->setValue(false);
    myAddButton->setSensitivity(false);
  }
  else
  {
    myAddButton->setValue(addBC);
    myAddButton->setSensitivity(IAmSensitive);
  }

  if (motionType < myMotionTypeButtons.size())
    myMotionTypeToggleGroup.setValue(myMotionTypeButtons[motionType],true);
}


unsigned int FuiMotionType::getValue() const
{
  for (size_t i = 0; i < myMotionTypeButtons.size(); i++)
    if (myMotionTypeButtons[i]->getValue())
    {
      if (myAddButton->getValue())
      {
	if (i == 0)
	  return 3;
	else if (i == 3)
	  return 5;
      }
      return i == 3 ? 4 : i;
    }

  return 0;
}


void FuiMotionType::setSensitivity(bool isSensitive)
{
  IAmSensitive = isSensitive;
  for (size_t i = 0; i < myMotionTypeButtons.size(); i++)
    myMotionTypeButtons[i]->setSensitivity(isSensitive);

  if (myMotionTypeButtons[1]->getValue() || myMotionTypeButtons[2]->getValue())
    myAddButton->setSensitivity(false);
  else
    myAddButton->setSensitivity(isSensitive);
}


void FuiMotionType::setSensitivity(unsigned int button, bool isSensitive)
{
  if (button < myMotionTypeButtons.size())
    myMotionTypeButtons[button]->setSensitivity(isSensitive);
}


void FuiMotionType::onValuesChanged()
{
  myValuesChangedCB.invoke(this->getValue());
  myChangedCB.invoke();
}
