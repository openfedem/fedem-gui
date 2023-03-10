// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QStyleFactory>

#include "FFuLib/FFuQtComponents/FFuQtListView.H"
#include "FFuLib/FFuQtComponents/FFuQtFrame.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtTopologyView.H"


FuiQtTopologyView::FuiQtTopologyView(QWidget* parent, const char* name)
  : FFuQtMultUIComponent(parent,name)
{
  FFuQtFrame* qfr;
  myFrame = qfr = new FFuQtFrame(this);
  qfr->setFrameStyle(QFrame::NoFrame);

  FFuQtListView* qlv;
  myView = qlv = new FFuQtListView(qfr,3);
  qlv->setFocusPolicy(Qt::NoFocus);
  qlv->setStyle(QStyleFactory::create("windows")); // enable connector lines

  this->initWidgets();
}
