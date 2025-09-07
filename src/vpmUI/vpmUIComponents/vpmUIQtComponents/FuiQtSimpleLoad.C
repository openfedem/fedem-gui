// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QVBoxLayout>

#include "FuiQtSimpleLoad.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"


FuiQtSimpleLoad::FuiQtSimpleLoad(QWidget* parent, const char* name)
  : FFuQtLabelFrame(parent)
{
  this->setObjectName(name);
  this->setLabel("Load magnitude");

  myEngineField = new FuiQtQueryInputField(NULL);

  this->initWidgets();

  QLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(static_cast<FuiQtQueryInputField*>(myEngineField));
}
