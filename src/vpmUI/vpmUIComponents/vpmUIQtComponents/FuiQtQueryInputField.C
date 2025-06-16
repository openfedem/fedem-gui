// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"

#include "FuiQtQueryInputField.H"


FuiQtQueryInputField::FuiQtQueryInputField(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  myOptions = new FFuQtOptionMenu();
  myButton  = new FFuQtPushButton();

  this->initWidgets();

  QBoxLayout* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  layout->addWidget(static_cast<FFuQtOptionMenu*>(myOptions),1);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(myButton));
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
    int width  = myOptions->getWidth() - arrowWidth;
    int height = myOptions->getHeight();
    myIOField->setSizeGeometry(border,border,width-border,height-2*border);
  }
}
