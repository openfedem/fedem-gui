// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtTriadSummary.H"


FuiQtTriadSummary::FuiQtTriadSummary(QWidget* parent,
				     int xpos, int ypos,
				     int width,int height, const char* name)
  : FFuQtMultUIComponent(parent,xpos,ypos,width,height,name)
{
  myFENodeField = new FFuQtLabelField(this);
  mySetAllFreeButton = new FFuQtPushButton(this);
  mySetAllFixedButton = new FFuQtPushButton(this);

  myMassFrame = new FFuQtLabelFrame(this);
  myMassField = new FFuQtLabelField(this);
  myIxField = new FFuQtLabelField(this);
  myIyField = new FFuQtLabelField(this);
  myIzField = new FFuQtLabelField(this);

  myTriadLabel = new FFuQtLabel(this);
  mySysDirFrame = new FFuQtLabelFrame(this);
  mySysDirMenu = new FFuQtOptionMenu(this);
  myConnectorFrame = new FFuQtLabelFrame(this);
  myConnectorMenu = new FFuQtOptionMenu(this);
  myAddBCLabel = new FFuQtLabel(this);

  mySummaryTable = FuiSummaryTable::createTable(this);

  this->initWidgets();
}
