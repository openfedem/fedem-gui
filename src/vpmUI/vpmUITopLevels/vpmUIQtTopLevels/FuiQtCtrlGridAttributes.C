// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"

#include "FuiQtCtrlGridAttributes.H"


class FFuQtSeparator : public QFrame
{
public:
  FFuQtSeparator(QWidget* parent = NULL);
};


FuiQtCtrlGridAttributes::FuiQtCtrlGridAttributes(QWidget* parent)
  : FFuQtTopLevelShell(parent,100,100,250,300,"Grid and Snap")
{
  myGridToggleButton = new FFuQtToggleButton();
  myGXField = new FFuQtIOField();
  myGYField = new FFuQtIOField();

  mySnapToggleButton = new FFuQtToggleButton();
  mySXField = new FFuQtIOField();
  mySYField = new FFuQtIOField();

  myCloseButton = new FFuQtPushButton(this);

  this->initWidgets();

  QWidget* qSize = new QWidget();
  QGridLayout* gl = new QGridLayout(qSize);
  gl->addWidget(new QLabel("X"),0,0);
  gl->addWidget(new QLabel("Y"),1,0);
  gl->addWidget(static_cast<FFuQtIOField*>(myGXField),0,1);
  gl->addWidget(static_cast<FFuQtIOField*>(myGYField),1,1);

  QWidget* qSnap = new QWidget();
  gl = new QGridLayout(qSnap);
  gl->addWidget(new QLabel("X"),0,0);
  gl->addWidget(new QLabel("Y"),1,0);
  gl->addWidget(static_cast<FFuQtIOField*>(mySXField),0,1);
  gl->addWidget(static_cast<FFuQtIOField*>(mySYField),1,1);

  QWidget* qClose = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qClose);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(myCloseButton));
  layout->addStretch(1);

  layout = new QVBoxLayout(this);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myGridToggleButton));
  layout->addWidget(new QLabel("Grid size"));
  layout->addWidget(qSize);
  layout->addWidget(new FFuQtSeparator());
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(mySnapToggleButton));
  layout->addWidget(new QLabel("Snap distance"));
  layout->addWidget(qSnap);
  layout->addWidget(new FFuQtSeparator());
  layout->addWidget(qClose,0,Qt::AlignBottom);
}
