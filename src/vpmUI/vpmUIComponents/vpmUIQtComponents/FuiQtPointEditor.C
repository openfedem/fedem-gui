// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtPointEditor.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQt3DPoint.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"


FuiQtPointEditor::FuiQtPointEditor(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  myIdField = new FFuQtLabelField();
  myPointUI = new FuiQt3DPoint();
  myPickButton = new FFuQtPushButton();
  myViewPointButton = new FFuQtPushButton();
  myViewWhatButton = new FFuQtPushButton();
  myApplyButton = new FFuQtPushButton();
  myApplyButton->setMaxWidth(50);

  this->initWidgets();

  QWidget* qOnWhat = new QWidget();
  QLayout* layout = new QHBoxLayout(qOnWhat);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(myIdField->getQtWidget());
  layout->addWidget(myViewWhatButton->getQtWidget());

  QWidget* qButtons = new QWidget();
  layout = new QVBoxLayout(qButtons);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(myPickButton->getQtWidget());
  layout->addWidget(myViewPointButton->getQtWidget());
  layout->addWidget(myApplyButton->getQtWidget());

  QWidget* qPoint = new QWidget();
  layout = new QHBoxLayout(qPoint);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(myPointUI->getQtWidget());
  layout->addWidget(qButtons);

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qOnWhat);
  layout->addWidget(qPoint);
}
