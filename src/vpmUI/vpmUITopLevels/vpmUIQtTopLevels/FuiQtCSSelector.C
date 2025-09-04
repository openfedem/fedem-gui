// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtItemsListViews.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"

#include "FuiQtCSSelector.H"


FuiCSSelector* FuiCSSelector::create(int xpos, int ypos, int width, int height,
                                     const char* title, const char* name)
{
  return new FuiQtCSSelector(xpos,ypos,width,height,title,name);
}


FuiQtCSSelector::FuiQtCSSelector(int xpos, int ypos, int width, int height,
                                 const char* title, const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,
                       title,name,Qt::WindowStaysOnTopHint)
{
  lvCS = new FuiQtCrossSectionListView(NULL);
  lvMat = new FuiQtMaterialListView(NULL);
  notes = new FFuQtNotes();
  importButton = new FFuQtPushButton();
  closeButton = new FFuQtPushButton();

  this->initWidgets();

  QBoxLayout* buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0,0,0,0);
  buttonLayout->addWidget(importButton->getQtWidget());
  buttonLayout->addStretch(1);
  buttonLayout->addWidget(closeButton->getQtWidget());

  QBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(new QLabel("Beam cross sections"));
  layout->addWidget(lvCS->getQtWidget(),3);
  layout->addWidget(new QLabel("Material"));
  layout->addWidget(lvMat->getQtWidget(),1);
  layout->addWidget(notes->getQtWidget());
  layout->addLayout(buttonLayout);
}
