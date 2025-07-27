// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QGridLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtBeamPropHydro.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"


FuiQtBeamPropHydro::FuiQtBeamPropHydro(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  myToggle = new FFuQtToggleButton();
  for (FFuLabelField*& fld : myFields)
    fld = new FFuQtLabelField();

  this->initWidgets();

  QGridLayout* layout = new QGridLayout(this);
  layout->setHorizontalSpacing(20);
  layout->addWidget(myToggle->getQtWidget(), 0,0,1,3);
  for (int i = 0; i < NFIELD; i++)
    layout->addWidget(myFields[i]->getQtWidget(), 1+i%4, i/4);
}
