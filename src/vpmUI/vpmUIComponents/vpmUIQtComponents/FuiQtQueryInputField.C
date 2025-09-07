// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"

#include "FuiQtQueryInputField.H"


FuiQtQueryInputField::FuiQtQueryInputField(QWidget* parent,
                                           const char* label, bool above,
                                           const char* name)
  : FFuQtWidget(parent,name)
{
  myOptions = new FFuQtOptionMenu();
  myButton  = new FFuQtPushButton();

  this->initWidgets();

  QWidget* qField = label && above ? new QWidget() : this;
  QBoxLayout* layout = new QHBoxLayout(qField);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  if (label && !above)
  {
    layout->addWidget(new QLabel(label),0,Qt::AlignVCenter);
    layout->addSpacing(6);
  }
  layout->addWidget(myOptions->getQtWidget(),1,Qt::AlignVCenter);
  layout->addWidget(myButton->getQtWidget(),0,Qt::AlignVCenter);

  if (label && above)
  {
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(new QLabel(label));
    layout->addWidget(qField);
  }
}


void FuiQtQueryInputField::setBehaviour(Policy behaviour, bool noButton)
{
  if (behaviour <= REF_TEXT && !myIOField)
  {
    FFuQtIOField* iof = new FFuQtIOField(this);
    iof->setFrame(false);
    iof->toFront();
    myIOField = iof;
  }
  else if (behaviour >= REF_NONE)
  {
    delete myIOField;
    myIOField = NULL;
  }

  this->FuiQueryInputField::setBehaviour(behaviour,noButton);
}


void FuiQtQueryInputField::resizeEvent(QResizeEvent* e)
{
  this->QWidget::resizeEvent(e);

  if (myIOField)
  {
    const int border = 2;
    const int arrowWidth = 17;
    int x = myOptions->getXPos() + border;
    int y = myOptions->getYPos() + border;
    int width  = myOptions->getWidth() - arrowWidth - border;
    int height = myOptions->getHeight() - 2*border;
    myIOField->setSizeGeometry(x,y,width,height);
  }
}
