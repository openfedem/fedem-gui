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


FFuQtSeparator::FFuQtSeparator (QWidget* parent) : QFrame(parent)
{
  this->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
}


FFuQtDialogButtons::FFuQtDialogButtons(QWidget* parent, bool addSeparator,
                                       const char* name)
  : FFuQtWidget(parent,name)
{
  QWidget* qButtons = addSeparator ? new QWidget() : this;
  QBoxLayout* layout = new QHBoxLayout(qButtons);
  layout->setContentsMargins(0,0,0,0);
  for (size_t i = 0; i < buttons.size(); i++)
  {
    FFuQtPushButton* qButton = new FFuQtPushButton();
    layout->addWidget(qButton);
    buttons[i] = qButton;

    // Add an empty frame in case the button is hidden (not labelled)
    FFuQtFrame* qFrame = new FFuQtFrame();
    qFrame->setFrameStyle(QFrame::NoFrame);
    qFrame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    layout->addWidget(qFrame);
    spacers[i] = qFrame;
  }

  if (addSeparator)
  {
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addStretch(1);
    layout->addWidget(new FFuQtSeparator());
    layout->addWidget(qButtons);
  }

  this->initWidgets();
}
