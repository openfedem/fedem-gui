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
  notesText = new FFuQtLabel();
  importButton = new FFuQtPushButton();
  closeButton = new FFuQtPushButton();

  this->initWidgets();

  QWidget* buttons = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(buttons);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(importButton));
  layout->addStretch(1);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(closeButton));

  layout = new QVBoxLayout(this);
  layout->addWidget(new QLabel("Beam cross sections"));
  layout->addWidget(static_cast<FuiQtCrossSectionListView*>(lvCS),3);
  layout->addWidget(new QLabel("Material"));
  layout->addWidget(static_cast<FuiQtMaterialListView*>(lvMat),1);
  layout->addWidget(new FFuQtNotesLabel());
  layout->addWidget(static_cast<FFuQtLabel*>(notesText));
  layout->addWidget(buttons);
}
