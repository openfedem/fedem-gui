// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtJointSummary.H"


FuiQtJointSummary::FuiQtJointSummary(QWidget* parent, int xpos, int ypos,
				     int width, int height, const char* name)
  : FFuQtMultUIComponent(parent,xpos,ypos,width,height,name)
{
  mySummaryTable      = FuiSummaryTable::createTable(this);

  myAddBCLabel        = new FFuQtLabel(this);
  myDefDamperLabel    = new FFuQtLabel(this);
  myDOF_TZ_Toggle     = new FFuQtToggleButton(this);
  mySetAllFreeButton  = new FFuQtPushButton(this);
  mySetAllFixedButton = new FFuQtPushButton(this);
  mySwapJointButton   = new FFuQtPushButton(this);
  myFrictionFrame     = new FFuQtLabelFrame(this);
  myFriction          = new FuiQtQueryInputField(this);
  myFrictionLabel     = new FFuQtLabel(this);
  myFrictionDof       = new FFuQtOptionMenu(this);
  myScrewFrame        = new FFuQtLabelFrame(this);
  myScrewToggle       = new FFuQtToggleButton(this);
  myScrewRatioField   = new FFuQtLabelField(this);
  myCamThicknessField = new FFuQtLabelField(this);
  myCamWidthField     = new FFuQtLabelField(this);
  myRadialToggle      = new FFuQtToggleButton(this);

  this->initWidgets();
}
