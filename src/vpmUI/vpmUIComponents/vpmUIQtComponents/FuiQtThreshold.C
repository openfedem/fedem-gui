// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtThreshold.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"


FuiQtThreshold::FuiQtThreshold(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  myToggle = new FFuQtToggleButton();
  myDescription = new FFuQtLabelField();
  myValueField = new FFuQtLabelField();
  myMinField = new FFuQtLabelField();
  mySkipField = new FFuQtLabelField();
  mySeverityMenu = new FFuQtOptionMenu();

  this->initWidgets();

  QWidget* qSeverity = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qSeverity);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Severity"));
  layout->addWidget(mySeverityMenu->getQtWidget(),1);

  layout = new QVBoxLayout(this);
  layout->addWidget(myToggle->getQtWidget());
  layout->addWidget(myDescription->getQtWidget());
  layout->addWidget(myValueField->getQtWidget());
  layout->addWidget(myMinField->getQtWidget());
  layout->addWidget(mySkipField->getQtWidget());
  layout->addWidget(qSeverity);
}
