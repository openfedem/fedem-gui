// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FuiQtCtrlElemProperties.H"
#include "FuiQtParameterView.H"

#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"


FuiQtCtrlElemProperties::FuiQtCtrlElemProperties(QWidget* parent)
  : FFuQtMultUIComponent(parent,"FuiCtrlElemProperties")
{
  FFuQtLabel* elemPixmap = new FFuQtLabel(this);
  elemPixmap->setAlignment(Qt::AlignCenter);

  this->myElemPixmap     = elemPixmap;
  this->myParameterFrame = new FFuQtLabelFrame(this);
  this->myParameterView  = new FuiQtParameterView(this);

  this->initWidgets();
}
