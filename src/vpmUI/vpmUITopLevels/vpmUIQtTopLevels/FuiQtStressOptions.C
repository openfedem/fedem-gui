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

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtTimeInterval.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtFileBrowseField.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtStressOptions.H"

//----------------------------------------------------------------------------

FuiStressOptions* FuiStressOptions::create(int xpos, int ypos,
					   int width, int height,
					   const char* title,
					   const char* name)
{
  return new FuiQtStressOptions(xpos,ypos,width,height,title,name);
}
//----------------------------------------------------------------------------

FuiQtStressOptions::FuiQtStressOptions(int xpos, int ypos,
                                       int width, int height,
                                       const char* title,
                                       const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,title,name)
{
  time = new FuiQtTimeInterval(NULL);
  outputFrame = new FFuQtLabelFrame();
  for (int i = 0; i < 6; i++) {
    stressToggle[i] = new FFuQtToggleButton();
    strainToggle[i] = new FFuQtToggleButton();
  }
  vtfFrame       = new FFuQtLabelFrame();
  autoVTFToggle  = new FFuQtToggleButton();
  autoVTFField   = new FFuQtFileBrowseField(NULL);
  minFringeField = new FFuQtIOField();
  maxFringeField = new FFuQtIOField();
  dialogButtons  = new FFuQtDialogButtons(NULL,false);

  this->initWidgets();

  QGridLayout* gl = new QGridLayout(static_cast<FFuQtLabelFrame*>(outputFrame));
  for (int i = 0; i < 6; i++)
  {
    gl->addWidget(dynamic_cast<FFuQtToggleButton*>(strainToggle[i]),i,0);
    gl->addWidget(dynamic_cast<FFuQtToggleButton*>(stressToggle[i]),i,1);
  }

  QWidget* fringeFields = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(fringeFields);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Min"));
  layout->addWidget(static_cast<FFuQtIOField*>(minFringeField));
  layout->addSpacing(20);
  layout->addWidget(new QLabel("Max"));
  layout->addWidget(static_cast<FFuQtIOField*>(maxFringeField));

  layout = new QVBoxLayout(static_cast<FFuQtLabelFrame*>(vtfFrame));
  layout->setContentsMargins(10,5,10,10);
  layout->setSpacing(5);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(autoVTFToggle));
  layout->addWidget(static_cast<FFuQtFileBrowseField*>(autoVTFField));
  layout->addSpacing(10);
  layout->addWidget(new QLabel("Contour range for Express VTF file (von Mises stress only)"));
  layout->addWidget(fringeFields);

  layout = new QVBoxLayout(this);
  layout->addWidget(static_cast<FuiQtTimeInterval*>(time));
  layout->addWidget(static_cast<FFuQtLabelFrame*>(outputFrame));
  layout->addWidget(static_cast<FFuQtLabelFrame*>(vtfFrame));
  layout->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons),
		    0, Qt::AlignBottom);
}
