// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtTimeInterval.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtGageOptions.H"

//----------------------------------------------------------------------------

FuiGageOptions* FuiGageOptions::create(int xpos, int ypos,
				       int width, int height, 
				       const char* title,
				       const char* name)
{
  return new FuiQtGageOptions(xpos,ypos,width,height,title,name);
}
//----------------------------------------------------------------------------

FuiQtGageOptions::FuiQtGageOptions(int xpos, int ypos,
				   int width, int height,
				   const char* title,
				   const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,title,name)
{
  time = new FuiQtTimeInterval(NULL);
  autoDacExportToggle = new FFuQtToggleButton();
  dacSampleRateField = new FFuQtIOField();
  rainflowToggle = new FFuQtToggleButton();
  binSizeField = new FFuQtIOField();
  importRosFileButton = new FFuQtPushButton();
  dialogButtons = new FFuQtDialogButtons();

  this->initWidgets();

  QWidget* qSampleRate = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qSampleRate);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("DAC file sample rate"));
  layout->addWidget(static_cast<FFuQtIOField*>(dacSampleRateField));

  QWidget* qBinSize = new QWidget();
  layout = new QHBoxLayout(qBinSize);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Stress cycle bin size"));
  layout->addWidget(static_cast<FFuQtIOField*>(binSizeField));

  layout = new QVBoxLayout(this);
  layout->setSpacing(0);
  layout->addWidget(static_cast<FuiQtTimeInterval*>(time));
  layout->addSpacing(5);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(autoDacExportToggle));
  layout->addWidget(qSampleRate);
  layout->addSpacing(5);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(rainflowToggle));
  layout->addWidget(qBinSize);
  layout->addSpacing(10);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(importRosFileButton));
  layout->addSpacing(5);
  layout->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons),
                    0, Qt::AlignBottom);
}
