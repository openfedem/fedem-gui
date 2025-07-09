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

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQt3DPoint.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtSeaEnvironment.H"


FuiSeaEnvironment* FuiSeaEnvironment::create(int xpos, int ypos,
					     int width, int height,
					     const char* title,
					     const char* name)
{
  return new FuiQtSeaEnvironment(xpos,ypos,width,height,title,name);
}


FuiQtSeaEnvironment::FuiQtSeaEnvironment(int xpos, int ypos,
					 int width, int height,
					 const char* title,
					 const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,title,name)
{
  FFuQtLabelFrame* qGrav;
  FFuQtLabelFrame* qWdir;
  FFuQtLabelFrame* qMG;

  waterDensityField  = new FFuQtIOField();
  meanSeaLevelField  = new FFuQtIOField();
  seaDepthField      = new FFuQtIOField();

  gravitationFrame  = qGrav = new FFuQtLabelFrame();
  gravitationVector = new FuiQt3DPoint(NULL,"g",false);

  waveDirectionFrame  = qWdir = new FFuQtLabelFrame();
  waveDirectionVector = new FuiQt3DPoint(NULL,"Xw",false);

  marineGrowthFrame = qMG     = new FFuQtLabelFrame();
  marineGrowthDensityField    = new FFuQtIOField();
  marineGrowthThicknessField  = new FFuQtIOField();
  marineGrowthUpperLimitField = new FFuQtIOField();
  marineGrowthLowerLimitField = new FFuQtIOField();

  waveQueryField      = new FuiQtQueryInputField(NULL);
  currQueryField      = new FuiQtQueryInputField(NULL);
  currDirQueryField   = new FuiQtQueryInputField(NULL);
  currScaleQueryField = new FuiQtQueryInputField(NULL);
  hdfScaleQueryField  = new FuiQtQueryInputField(NULL);

  dialogButtons = new FFuQtDialogButtons();

  this->initWidgets();

  QWidget* qWater = new QWidget();
  QGridLayout* gl = new QGridLayout(qWater);
  gl->setContentsMargins(0,0,0,10);
  gl->addWidget(new QLabel("Water density"), 0,0);
  gl->addWidget(new QLabel("Mean sea level"),1,0);
  gl->addWidget(new QLabel("Water depth"),   1,2);
  gl->addWidget(static_cast<FFuQtIOField*>(waterDensityField),0,1);
  gl->addWidget(static_cast<FFuQtIOField*>(meanSeaLevelField),1,1);
  gl->addWidget(static_cast<FFuQtIOField*>(seaDepthField),    1,3);
  
  QBoxLayout* layout = new QHBoxLayout(qGrav);
  layout->setContentsMargins(0,0,0,1);
  layout->addWidget(static_cast<FuiQt3DPoint*>(gravitationVector));

  layout = new QHBoxLayout(qWdir);
  layout->setContentsMargins(0,0,0,1);
  layout->addWidget(static_cast<FuiQt3DPoint*>(waveDirectionVector));

  QWidget* qGravAndWD = new QWidget();
  layout = new QHBoxLayout(qGravAndWD);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qGrav);
  layout->addWidget(qWdir);

  gl = new QGridLayout(qMG);
  gl->addWidget(new QLabel("Density"),0,0);
  gl->addWidget(new QLabel("Thickness"),1,0);
  gl->addWidget(new QLabel("   Upper limit"),0,2);
  gl->addWidget(new QLabel("   Lower limit"),1,2);
  gl->addWidget(static_cast<FFuQtIOField*>(marineGrowthDensityField),0,1);
  gl->addWidget(static_cast<FFuQtIOField*>(marineGrowthThicknessField),1,1);
  gl->addWidget(static_cast<FFuQtIOField*>(marineGrowthUpperLimitField),0,3);
  gl->addWidget(static_cast<FFuQtIOField*>(marineGrowthLowerLimitField),1,3);

  QWidget* qFuncs = new QWidget();
  gl = new QGridLayout(qFuncs);
  gl->setContentsMargins(0,10,0,10);
  gl->setColumnStretch(1,1);
  gl->addWidget(new QLabel("Wave function"),0,0);
  gl->addWidget(new QLabel("Current function"),1,0);
  gl->addWidget(new QLabel("Current direction"),2,0);
  gl->addWidget(new QLabel("Current direction"),3,0);
  gl->addWidget(static_cast<FuiQtQueryInputField*>(waveQueryField),0,1);
  gl->addWidget(static_cast<FuiQtQueryInputField*>(currQueryField),1,1);
  gl->addWidget(static_cast<FuiQtQueryInputField*>(currDirQueryField),2,1);
  gl->addWidget(static_cast<FuiQtQueryInputField*>(currScaleQueryField),3,1);

  QWidget* qHDFscale = new QWidget();
  layout = new QHBoxLayout(qHDFscale);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Hydrodynamic force scale"));
  layout->addWidget(static_cast<FuiQtQueryInputField*>(hdfScaleQueryField));

  layout = new QVBoxLayout(this);
  layout->addWidget(qWater);
  layout->addWidget(qGravAndWD,4);
  layout->addWidget(qMG,3);
  layout->addWidget(qFuncs);
  layout->addWidget(qHDFscale);
  layout->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons));
}
