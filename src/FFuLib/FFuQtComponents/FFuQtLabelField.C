// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"


FFuQtLabelField::FFuQtLabelField(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  FFuQtLabel* qLabel;
  FFuQtIOField* qField;
  myLabel = qLabel = new FFuQtLabel();
  myField = qField = new FFuQtIOField();

  QLayout* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qLabel);
  layout->addWidget(qField);
}
