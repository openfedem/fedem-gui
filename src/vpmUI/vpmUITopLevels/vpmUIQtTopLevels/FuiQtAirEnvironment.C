// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtFileBrowseField.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtAirEnvironment.H"


FuiAirEnvironment* FuiAirEnvironment::create(int xpos, int ypos,
					     int width, int height,
					     const char* title,
					     const char* name)
{
  return new FuiQtAirEnvironment(xpos,ypos,width,height,title,name);
}


FuiQtAirEnvironment::FuiQtAirEnvironment(int xpos, int ypos,
					 int width, int height,
					 const char* title,
					 const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,title,name)
{
  stallModMenu   = new FFuQtOptionMenu();
  useCMToggle    = new FFuQtToggleButton();
  infModMenu     = new FFuQtOptionMenu();
  indModMenu     = new FFuQtOptionMenu();
  aTolerField    = new FFuQtIOField();
  tipLossMenu    = new FFuQtOptionMenu();
  hubLossMenu    = new FFuQtOptionMenu();

  windFrame      = new FFuQtLabelFrame();
  windToggle     = new FFuQtToggleButton();
  windTurbButton = new FFuQtPushButton();
  windSpeedField = new FFuQtLabelField();
  windDirField   = new FFuQtLabelField();
  windField      = new FFuQtFileBrowseField(NULL);

  towerFrame     = new FFuQtLabelFrame();
  towerPotToggle = new FFuQtToggleButton();
  towerShdToggle = new FFuQtToggleButton();
  towerField     = new FFuQtFileBrowseField(NULL);

  airDensField   = new FFuQtLabelField();
  kinViscField   = new FFuQtLabelField();
  dtAeroField    = new FFuQtLabelField();
  dtAeroToggle   = new FFuQtToggleButton();

  dialogButtons  = new FFuQtDialogButtons();

  this->initWidgets();

  static_cast<FFuQtLabelField*>(windSpeedField)->setFixedHeight(22);
  static_cast<FFuQtLabelField*>(windDirField)->setFixedHeight(22);
  static_cast<FFuQtLabelField*>(airDensField)->setFixedHeight(22);
  static_cast<FFuQtLabelField*>(kinViscField)->setFixedHeight(22);
  static_cast<FFuQtLabelField*>(dtAeroField)->setFixedHeight(22);

  QWidget* qADoptions = new QWidget();
  QGridLayout* gl = new QGridLayout(qADoptions);
  gl->setContentsMargins(0,0,0,0);
  gl->addWidget(new QLabel("Dynamic stall model"), 0,0);
  gl->addWidget(new QLabel("Inflow model"), 2,0);
  gl->addWidget(new QLabel("Induction-factor model"), 3,0);
  gl->addWidget(new QLabel("Induction-factor tolerance"), 4,0);
  gl->addWidget(new QLabel("Tip-loss stall model"), 5,0);
  gl->addWidget(new QLabel("Hub-loss model"), 6,0);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(stallModMenu), 0,1);
  gl->addWidget(dynamic_cast<FFuQtToggleButton*>(useCMToggle), 1,0,1,2);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(infModMenu), 2,1);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(indModMenu), 3,1);
  gl->addWidget(static_cast<FFuQtIOField*>(aTolerField), 4,1);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(tipLossMenu), 5,1);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(hubLossMenu), 6,1);

  QWidget* qWindButtons = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qWindButtons);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(windToggle),1);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(windTurbButton));

  QWidget* qWindFields = new QWidget();
  layout = new QHBoxLayout(qWindFields);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(20);
  layout->addWidget(static_cast<FFuQtLabelField*>(windSpeedField));
  layout->addWidget(static_cast<FFuQtLabelField*>(windDirField));

  layout = new QVBoxLayout(static_cast<FFuQtLabelFrame*>(windFrame));
  layout->addWidget(qWindButtons);
  layout->addWidget(qWindFields);
  layout->addWidget(static_cast<FFuQtFileBrowseField*>(windField));

  QWidget* qTwrButtons = new QWidget();
  layout = new QHBoxLayout(qTwrButtons);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(towerPotToggle));
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(towerShdToggle));

  layout = new QVBoxLayout(static_cast<FFuQtLabelFrame*>(towerFrame));
  layout->addWidget(qTwrButtons);
  layout->addWidget(qWindFields);
  layout->addWidget(static_cast<FFuQtFileBrowseField*>(towerField));

  QWidget* qAirFields = new QWidget();
  layout = new QHBoxLayout(qAirFields);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(20);
  layout->addWidget(static_cast<FFuQtLabelField*>(airDensField));
  layout->addWidget(static_cast<FFuQtLabelField*>(kinViscField));

  QWidget* qADsetup = new QWidget();
  layout = new QVBoxLayout(qADsetup);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qAirFields);
  layout->addWidget(static_cast<FFuQtLabelField*>(dtAeroField));
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(dtAeroToggle));

  layout = new QVBoxLayout(this);
  layout->addWidget(qADoptions,7);
  layout->addWidget(static_cast<FFuQtLabelFrame*>(windFrame),3);
  layout->addWidget(static_cast<FFuQtLabelFrame*>(towerFrame),4);
  layout->addWidget(qADsetup,2);
  layout->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons));
}
