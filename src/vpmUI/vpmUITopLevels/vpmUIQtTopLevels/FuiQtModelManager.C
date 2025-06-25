// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QTabWidget>
#include <QVBoxLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtItemsListViews.H"
#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtModelManager.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"


FuiQtModelManager::FuiQtModelManager(QWidget* parent)
  : FFuQtWidget(parent,"FuiQtModelManager")
{
  FuiQtSimModelListView* qModel = new FuiQtSimModelListView(NULL);
  FuiQtResultListView* qResult = new FuiQtResultListView(NULL);

  QTabWidget* qTabs = new QTabWidget();
  qTabs->addTab(qModel,QIcon(QPixmap(treeObjects_xpm)),"Objects");
  qTabs->addTab(qResult,QIcon(QPixmap(treeResults_xpm)),"Results");

  QBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(5,5,2,0);
  layout->addWidget(qTabs);

  modelManager  = qModel;
  resultManager = qResult;
}
