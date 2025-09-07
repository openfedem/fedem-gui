// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QVBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtTable.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtFileBrowseField.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"

#include "FuiQtExtCtrlSysProperties.H"


FuiQtExtCtrlSysProperties::FuiQtExtCtrlSysProperties(QWidget* parent)
  : FFuQtWidget(parent,"FuiQtExtCtrlSysProperties")
{
  fileBrowseField = new FFuQtFileBrowseField(NULL);
  myTable         = new FFuQtTable(NULL);

  this->initWidgets();

  QLayout* layout = new QVBoxLayout(this);
  layout->addWidget(fileBrowseField->getQtWidget());
  layout->addWidget(myTable->getQtWidget());
}


void FuiQtExtCtrlSysProperties::makeNewFields()
{
  myInpEngineFields.push_back(new FuiQtQueryInputField(myTable->getQtWidget()));
  myNameLabels.push_back(new FFuQtLabel(myTable->getQtWidget()));
}
