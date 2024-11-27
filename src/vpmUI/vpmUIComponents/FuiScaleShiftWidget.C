// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiScaleShiftWidget.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuToggleButton.H"


FuiScaleShiftWidget::FuiScaleShiftWidget() : readingDBonly(false)
{
  scaleField = shiftField = NULL;
  zeroOutBtn = NULL;
}


void FuiScaleShiftWidget::getValues(double& scale, bool& onOff, double& shift)
{
  scale = scaleField->getDouble();
  shift = shiftField->getDouble();
  onOff = zeroOutBtn->getToggle();
}


void FuiScaleShiftWidget::setValues(double scale, bool onOff, double shift)
{
  readingDBonly = true; // to avoid touching model when reading data from DB

  scaleField->setValue(scale);
  shiftField->setValue(shift);
  zeroOutBtn->setValue(onOff);

  readingDBonly = false;
}
