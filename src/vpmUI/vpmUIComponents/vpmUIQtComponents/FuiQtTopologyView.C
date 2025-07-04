// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QStyleFactory>
#include <QVBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtListView.H"

#include "FuiQtTopologyView.H"


FuiQtTopologyView::FuiQtTopologyView(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  FFuQtListView* qlv = new FFuQtListView(NULL,3);
  qlv->setFocusPolicy(Qt::NoFocus);
  qlv->setStyle(QStyleFactory::create("windows")); // enable connector lines
  myView = qlv;

  this->initWidgets();

  QBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qlv);
}
