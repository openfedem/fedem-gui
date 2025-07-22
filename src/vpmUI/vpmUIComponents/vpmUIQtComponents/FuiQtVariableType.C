// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QVBoxLayout>

#include "FuiQtVariableType.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"


FuiQtVariableType::FuiQtVariableType(QWidget* parent, const char* name)
  : FFuQtLabelFrame(parent)
{
  this->setObjectName(name);
  this->setLabel("Prescribed quantity");

  myTypeMenu = new FFuQtOptionMenu();

  this->initWidgets();

  QLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(static_cast<FFuQtOptionMenu*>(myTypeMenu));
}
