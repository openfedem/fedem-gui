// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QLabel>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtSNCurveSelector.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"


FuiQtSNCurveSelector::FuiQtSNCurveSelector(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  stdTypeMenu   = new FFuQtOptionMenu();
  curveTypeMenu = new FFuQtOptionMenu();

  this->initWidgets();

  QBoxLayout* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Standard"));
  layout->addWidget(stdTypeMenu->getQtWidget());
  layout->addSpacing(10);
  layout->addWidget(new QLabel("S-N curve"));
  layout->addWidget(curveTypeMenu->getQtWidget());
}
