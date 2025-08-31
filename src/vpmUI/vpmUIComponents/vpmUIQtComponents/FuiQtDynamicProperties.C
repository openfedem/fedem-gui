// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtDynamicProperties.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"


FuiQtDynamicProperties::FuiQtDynamicProperties(QWidget* parent,
                                               const char* name)
  : FFuQtWidget(parent,name)
{
  for (FFuLabelField*& fld : myFields)
    fld = new FFuQtLabelField();

  QGroupBox* qStructDamp = new QGroupBox("Structural damping");
  QLayout* layout = new QVBoxLayout(qStructDamp);
  layout->setContentsMargins(5,0,5,5);
  for (int i = 0; i < 2; i++)
    layout->addWidget(myFields[i]->getQtWidget());

  QGroupBox* qDynProp = new QGroupBox("Dynamic properties");
  layout = new QVBoxLayout(qDynProp);
  layout->setContentsMargins(5,0,5,5);
  for (int i = 2; i < 4; i++)
    layout->addWidget(myFields[i]->getQtWidget());

  layout = new QHBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qStructDamp);
  layout->addWidget(qDynProp);
}
