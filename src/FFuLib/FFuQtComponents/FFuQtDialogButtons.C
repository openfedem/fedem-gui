// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"


FFuQtDialogButtons::FFuQtDialogButtons(QWidget* parent, bool addSeparator,
                                       const char* name)
  : QWidget(parent)
{
  this->setObjectName(name);
  this->setWidget(this);

  QWidget* myButtons = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(myButtons);
  layout->setContentsMargins(0,0,0,0);
  FFuQtPushButton* myButton;
  FFuQtFrame* myEmptyFrame;
  for (size_t i = 0; i < buttons.size(); i++)
  {
    buttons[i] = myButton = new FFuQtPushButton();
    layout->addWidget(myButton);
    // Add an empty frame in case the button is hidden (not labelled)
    spacers[i] = myEmptyFrame = new FFuQtFrame();
    myEmptyFrame->setFrameStyle(QFrame::NoFrame);
    myEmptyFrame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    layout->addWidget(myEmptyFrame);
  }

  layout = new QVBoxLayout(this);
  if (addSeparator)
  {
    QFrame* mySeparator = new QFrame();
    mySeparator->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    mySeparator->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    layout->addWidget(mySeparator);
  }
  layout->addWidget(myButtons);

  this->initWidgets();
}


FFuQtDialogButtonsF::FFuQtDialogButtonsF(QWidget* parent) : QFrame(parent)
{
  this->setWidget(this);

  QWidget* myButtons = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(myButtons);
  layout->setContentsMargins(0,0,0,0);
  FFuQtPushButton* myButton;
  for (size_t i = 0; i < buttons.size(); i++)
  {
    buttons[i] = myButton = new FFuQtPushButton();
    layout->addWidget(myButton);
  }

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  QFrame* mySeparator = new QFrame();
  mySeparator->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  mySeparator->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
  layout->addWidget(mySeparator);
  layout->addWidget(myButtons);

  this->initWidgets();
}
