// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtPlugins.H"


FuiPlugins* FuiPlugins::create(int xpos, int ypos, int width, int height,
                               const char* title, const char* name)
{
  return new FuiQtPlugins(xpos,ypos,width,height,title,name);
}


FuiQtPlugins::FuiQtPlugins(int xpos, int ypos, int width, int height,
                           const char* title, const char* name)
  : FFuQtModalDialog(xpos,ypos,width,height,title,name)
{
  dialogButtons = new FFuQtDialogButtons();
  myLayout = new QVBoxLayout(this);
  myLayout->setSpacing(0);
  myLayout->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons));

  this->initWidgets();
}


void FuiQtPlugins::setNoPlugins(unsigned int nPlugins)
{
  size_t irow = myPlugins.size()*2;
  while (myPlugins.size() < nPlugins)
  {
    FFuQtToggleButton* qButton = new FFuQtToggleButton();
    FFuQtLabel*        qLabel  = new FFuQtLabel();
    myPlugins.emplace_back(qButton,qLabel);
    QWidget* qDescr = new QWidget();
    QBoxLayout* layout = new QHBoxLayout(qDescr);
    layout->setContentsMargins(0,0,0,5);
    layout->addSpacing(19);
    layout->addWidget(qLabel,1);
    myLayout->insertWidget(irow++,qButton);
    myLayout->insertWidget(irow++,qDescr);
  }
  while (irow > nPlugins*2)
  {
    QLayoutItem* item = myLayout->takeAt(--irow);
    delete item->widget();
    delete item;
  }
  if (myPlugins.size() > nPlugins)
    myPlugins.resize(nPlugins);
}
