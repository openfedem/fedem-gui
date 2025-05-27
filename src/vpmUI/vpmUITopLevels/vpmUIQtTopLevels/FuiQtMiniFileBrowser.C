// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QStyleFactory>
#include <QVBoxLayout>
#include <QSplitter>

#include "FFuLib/FFuQtComponents/FFuQtMemo.H"
#include "FFuLib/FFuQtComponents/FFuQtListView.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtMiniFileBrowser.H"


FuiMiniFileBrowser* FuiMiniFileBrowser::create(int xpos, int ypos,
					       int width, int height)
{
  return new FuiQtMiniFileBrowser(xpos, ypos, width, height);
}


FuiQtMiniFileBrowser::FuiQtMiniFileBrowser(int xpos, int ypos,
					   int width, int height,
					   const char* title, const char* name)
  : FFuQtTopLevelShell(NULL, xpos, ypos, width, height, title, name)
{
  QSplitter*     qSplitter = new QSplitter(Qt::Horizontal);
  FFuQtListView* qListView = new FFuQtListView(qSplitter);
  FFuQtMemo*     qInfoView = new FFuQtMemo(qSplitter);

  qListView->setStyle(QStyleFactory::create("windows"));
  qInfoView->setFont({"Courier",8});

  listView = qListView;
  infoView = qInfoView;
  dialogButtons = new FFuQtDialogButtons(NULL,false);

  qSplitter->setSizes({360,400});
  qSplitter->setStretchFactor(0,0);
  qSplitter->setStretchFactor(1,1);

  this->initWidgets();

  QBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(5,0,5,10);
  layout->addWidget(qSplitter,1);
  layout->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons));
}
