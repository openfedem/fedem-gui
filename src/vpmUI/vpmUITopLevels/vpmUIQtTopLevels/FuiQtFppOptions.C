// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtTimeInterval.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtFppOptions.H"

//----------------------------------------------------------------------------

FuiFppOptions* FuiFppOptions::create(int xpos, int ypos,
				     int width, int height,
				     const char* title,
				     const char* name)
{
  return new FuiQtFppOptions(xpos,ypos,width,height,title,name);
}
//----------------------------------------------------------------------------

FuiQtFppOptions::FuiQtFppOptions(int xpos, int ypos,
				 int width, int height,
				 const char* title,
				 const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,title,name)
{
  time = new FuiQtTimeInterval(NULL);

  maxElemsLabel = new FFuQtLabel();
  maxElemsField = new FFuQtIOField();

  biaxGateLabel = new FFuQtLabel();
  biaxGateField = new FFuQtIOField();

  rainflowFrame = new FFuQtLabelFrame();
  rainflowButton = new FFuQtToggleButton();

  typeLabel = new FFuQtLabel();
  typeMenu = new FFuQtOptionMenu();

  pvxGateLabel = new FFuQtLabel();
  pvxGateField = new FFuQtIOField();

  histFrame  = new FFuQtLabelFrame();
  nBinsField = new FFuQtIOField();
  maxField   = new FFuQtIOField();
  minField   = new FFuQtIOField();

  dialogButtons = new FFuQtDialogButtons(NULL,false);

  this->initWidgets();

  QWidget* qElGate = new QWidget();
  QGridLayout* grl = new QGridLayout(qElGate);
  grl->setContentsMargins(0,10,0,10);
  grl->addWidget(static_cast<FFuQtLabel*>(maxElemsLabel),0,0);
  grl->addWidget(static_cast<FFuQtLabel*>(biaxGateLabel),1,0);
  grl->addWidget(static_cast<FFuQtIOField*>(maxElemsField),0,1);
  grl->addWidget(static_cast<FFuQtIOField*>(biaxGateField),1,1);

  QWidget* qPVX = new QWidget();
  grl = new QGridLayout(qPVX);
  grl->setContentsMargins(0,0,0,0);
  grl->addWidget(static_cast<FFuQtLabel*>(typeLabel),0,0);
  grl->addWidget(static_cast<FFuQtLabel*>(pvxGateLabel),1,0);
  grl->addWidget(static_cast<FFuQtOptionMenu*>(typeMenu),0,1);
  grl->addWidget(static_cast<FFuQtIOField*>(pvxGateField),1,1);

  QWidget* qBins = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qBins);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Number of bins"));
  layout->addWidget(static_cast<FFuQtIOField*>(nBinsField));

  QWidget* qRange = new QWidget();
  layout = new QHBoxLayout(qRange);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Min"));
  layout->addWidget(static_cast<FFuQtIOField*>(minField));
  layout->addSpacing(20);
  layout->addWidget(new QLabel("Max"));
  layout->addWidget(static_cast<FFuQtIOField*>(maxField));

  layout = new QVBoxLayout(static_cast<FFuQtLabelFrame*>(histFrame));
  layout->setContentsMargins(10,5,10,10);
  layout->addWidget(qBins);
  layout->addWidget(new QLabel("Initial range"));
  layout->addWidget(qRange);

  layout = new QVBoxLayout(static_cast<FFuQtLabelFrame*>(rainflowFrame));
  layout->setContentsMargins(10,5,10,10);
  layout->setSpacing(5);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(rainflowButton));
  layout->addWidget(qPVX);
  layout->addWidget(static_cast<FFuQtLabelFrame*>(histFrame));

  layout = new QVBoxLayout(this);
  layout->addWidget(static_cast<FuiQtTimeInterval*>(time));
  layout->addWidget(qElGate);
  layout->addWidget(static_cast<FFuQtLabelFrame*>(rainflowFrame));
  layout->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons),
                    0, Qt::AlignBottom);
}
