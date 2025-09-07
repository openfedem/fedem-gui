// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"

#include "FuiQtCtrlElemProperties.H"


FuiQtCtrlElemProperties::FuiQtCtrlElemProperties(QWidget* parent)
  : FFuQtWidget(parent,"FuiQtCtrlElemProperties")
{
  myElemPixmap     = new FFuQtLabel();
  myElemFrame      = new FFuQtLabelFrame();
  myParameterFrame = new FFuQtLabelFrame();

  this->initWidgets();

  QBoxLayout* layout = new QHBoxLayout(myElemFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,0);
  layout->addWidget(myElemPixmap->getQtWidget());

  layout = new QHBoxLayout(this);
  layout->setSpacing(20);
  layout->addWidget(myElemFrame->getQtWidget());
  layout->addWidget(myParameterFrame->getQtWidget(),5);
  layout->addStretch(1);
}
