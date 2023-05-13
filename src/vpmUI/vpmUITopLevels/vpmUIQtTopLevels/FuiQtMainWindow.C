// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <cstdio>

#include <QProgressBar>
#include <QFrame>
#include <QLabel>
#include <QGridLayout>
#include <QStackedWidget>
#include <QCloseEvent>
#include <QStatusBar>

#include "FFuLib/FFuQtComponents/FFuQtMenuBar.H"
#include "FFuLib/FFuQtComponents/FFuQtToolBar.H"
#include "FFuLib/FFuQtComponents/FFuQtSplitter.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"

#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtMainWindow.H"
#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtProperties.H"
#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtModelManager.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtWorkSpace.H"
#include "vpmUI/Fui.H"
#include "vpmUI/FuiModes.H"
#include "vpmUI/FuiCtrlModes.H"


//----------------------------------------------------------------------------
FuiMainWindow* FuiMainWindow::create(int xpos, int ypos,
				     int width,int height,
				     const char* title)
{
  FuiMainWindow* mw = new FuiQtMainWindow();
  mw->setSizeGeometry(xpos, ypos, width, height);
  mw->setTitle(title);
  return mw;
}
//----------------------------------------------------------------------------

FuiQtMainWindow::FuiQtMainWindow()
  // TODO dagc it must be toplevel
  // if one want the menubar to get mdiwindow's winbuttons (min,max..)
  // (from qt) QDialog and QMainWindow widgets are by default top-level,
  // even if a parent widget is specified in the constructor.
  // This behavior is specified by the \c WType_TopLevel widget flag.
  // (thats why we use 0 instead of WType_TopLevel)
{
  mySubStepCount = 0;

  this->setWidget(this);

  FFuQtMenuBar* qmbar = new FFuQtMenuBar(this);
  this->mainMenuBar = qmbar;
  this->setMenuBar(qmbar);

  // Lambda function for creating a new toolbar
  auto&& newToolBar = [this](Qt::ToolBarArea where)
  {
    FFuQtToolBar* qtbar = new FFuQtToolBar(this);
    this->addToolBar(where,qtbar);
    return qtbar;
  };

  this->toolBars[STD]                = newToolBar(Qt::TopToolBarArea);
  this->toolBars[WINDOWS]            = newToolBar(Qt::TopToolBarArea);
  this->toolBars[THREEDVIEWS]        = newToolBar(Qt::TopToolBarArea);
  this->addToolBarBreak(Qt::TopToolBarArea);
  this->toolBars[MECHWIND]           = newToolBar(Qt::TopToolBarArea);
  this->toolBars[MECHCREATE]         = newToolBar(Qt::TopToolBarArea);
  this->toolBars[MECHMODELLINGTOOLS] = newToolBar(Qt::TopToolBarArea);
  this->toolBars[VIEWCTRL]           = newToolBar(Qt::TopToolBarArea);
  this->toolBars[VIEWCTRL1]          = newToolBar(Qt::TopToolBarArea);
  this->toolBars[VIEWCTRL2]          = newToolBar(Qt::TopToolBarArea);
  this->addToolBarBreak(Qt::TopToolBarArea);
#ifdef FT_HAS_SOLVERS
  this->toolBars[CTRLCREATE]         = newToolBar(Qt::TopToolBarArea);
  this->toolBars[CTRLMODELLINGTOOLS] = newToolBar(Qt::TopToolBarArea);
#else
  this->toolBars[CTRLCREATE]         = newToolBar(Qt::RightToolBarArea);
  this->toolBars[CTRLMODELLINGTOOLS] = newToolBar(Qt::RigthToolBarArea);
#endif
  this->toolBars[SOLVE]              = newToolBar(Qt::RightToolBarArea);

  this->toolBars[VIEWCTRL]->popDown();
  this->toolBars[VIEWCTRL1]->popDown();
  this->toolBars[VIEWCTRL2]->popDown();
  this->toolBars[CTRLCREATE]->popDown();
  this->toolBars[CTRLMODELLINGTOOLS]->popDown();

  this->myStatusLabel = new QLabel(this->QMainWindow::statusBar());
  this->QMainWindow::statusBar()->addWidget(this->myStatusLabel,3);

  this->mySubStepLabel = new QLabel(this->QMainWindow::statusBar());
  this->QMainWindow::statusBar()->addWidget(this->mySubStepLabel,1);

  this->mySubTaskLabel = new QLabel(this->QMainWindow::statusBar());
  this->QMainWindow::statusBar()->addWidget(this->mySubTaskLabel,3);

  this->myProgBar = new QProgressBar(this->QMainWindow::statusBar());
  this->QMainWindow::statusBar()->addWidget(this->myProgBar,10);
  this->myProgBar->hide();

  FFuQtSplitter* items_ws__props = new FFuQtSplitter(Qt::Vertical,this);
  FFuQtSplitter* items__ws       = new FFuQtSplitter(Qt::Horizontal,items_ws__props);
  FFuQtSplitter* leftColumn      = new FFuQtSplitter(Qt::Vertical,items__ws);

  UIgeo geo = Fui::getUIgeo(Fui::FUI_PROPERTIES_GEO);
  FuiQtProperties* qtproperties = new FuiQtProperties(items_ws__props,geo.xPos,geo.yPos,geo.width,geo.height);
  this->properties = qtproperties;

  this->leftSplitter = leftColumn;
  myMainHintFrame = new QFrame(leftColumn);
  QGridLayout* hintLayout = new QGridLayout(myMainHintFrame);
  hintLayout->setContentsMargins(5,5,5,5);
  hintLayout->setSpacing(5);

  myHintLabel = new QLabel(myMainHintFrame);
  myHintLabel->setFrameStyle(QFrame::Box);
  myHintLabel->setLineWidth(2);
  myHintLabel->setStyleSheet("background-color: rgb(255,171,2);");
  myHintLabel->setMinimumHeight(4+4*this->getFontHeigth());
  myHintLabel->setAutoFillBackground(true);
  myHintLabel->setWordWrap(true);

  hintLayout->addWidget(myHintLabel, 0,0, 1,4);

  FFuQtPushButton* doneButton = new FFuQtPushButton(myMainHintFrame);
  doneButton->setLabel("Done");

  FFuQtPushButton* cancelButton = new FFuQtPushButton(myMainHintFrame);
  cancelButton->setLabel("Cancel");

  hintLayout->addWidget(doneButton, 1,1);
  hintLayout->addWidget(cancelButton, 1,2);

  doneButton->setActivateCB(FFaDynCB0M(FuiQtMainWindow, this, onHintDone));
  cancelButton->setActivateCB(FFaDynCB0M(FuiQtMainWindow, this, onHintCancel));

  myDefinitionWidgetStack = new QStackedWidget(myMainHintFrame);
  hintLayout->addWidget(myDefinitionWidgetStack, 2,0, 1,4);
  myDefinitionWidgetStack->hide();

  hintLayout->addItem(new QSpacerItem(50, 10, QSizePolicy::Preferred, QSizePolicy::Fixed), 3,0, 1,4);

  geo = Fui::getUIgeo(Fui::FUI_MODELMANAGER_GEO);
  this->modelManager = new FuiQtModelManager(leftColumn, geo.xPos,geo.yPos,geo.width,geo.height);
  this->workSpace = new FuiQtWorkSpace(items__ws);

  this->setCentralWidget(items_ws__props);
  items_ws__props->setSizes({ 765, 235 });
  items__ws->setSizes({ 180, 800 });

  this->initWidgets();
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::removeToolBar(int toolBar)
{
  this->QMainWindow::removeToolBar(dynamic_cast<FFuQtToolBar*>(this->toolBars[toolBar]));
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::closeEvent(QCloseEvent* e)
{
  this->invokeFinishedCB();
  e->ignore();
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::setTitle(const char* title)
{
  this->setWindowTitle(title);
}
//----------------------------------------------------------------------------

const char* FuiQtMainWindow::getTitle() const
{
  static std::string title;
  title = this->windowTitle().toStdString();
  return title.c_str();
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::setStatusBarMessage(const char* message, int ms)
{
  if (!message) {
    this->QMainWindow::statusBar()->clearMessage();
    this->showHintPanel(false);
  }
  else if (ms > 0)
    this->QMainWindow::statusBar()->showMessage(message,ms);
  else {
    this->QMainWindow::statusBar()->showMessage(message);
    this->setHintText(message);
    this->showHintPanel(true);
  }
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::clearStatusBarMessage()
{
  this->QMainWindow::statusBar()->clearMessage();
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::enableProgress(int steps)
{
  myProgBar->show();
  myProgBar->setMaximum(steps);
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::setProgress(int step)
{
  myProgBar->setValue(step);
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::disableProgress()
{
  myProgBar->reset();
  myProgBar->hide();
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::setStatus(const char* statusText)
{
  myStatusLabel->setText(statusText);
  myStatusLabel->repaint();
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::enableSubSteps(int steps)
{
  mySubStepCount = steps;
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::setSubStep(int step)
{
  char buf[16];
  sprintf(buf,"%d/%d", step, mySubStepCount);

  mySubStepLabel->setText(buf);
  mySubStepLabel->repaint();
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::disableSubSteps()
{
  mySubStepCount = 0;
  mySubStepLabel->setText("");
  mySubStepLabel->repaint();
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::displayTime(int hour, int min, int sec)
{
  char buf[16];
  sprintf(buf,"%02d:%02d:%02d", hour, min, sec);

  mySubStepLabel->setText(buf);
  mySubStepLabel->repaint();
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::clearTime()
{
  mySubStepLabel->setText("");
  mySubStepLabel->repaint();
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::setSubTask(const char* taskText)
{
  mySubTaskLabel->setText(taskText);
  mySubTaskLabel->repaint();
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::setHintText(const char* text)
{
  myHintLabel->setText(text);
  myHintLabel->setMinimumHeight(myHintLabel->sizeHint().height());
  myHintLabel->hide();
  myHintLabel->show();
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::showHintPanel(bool b)
{
  if (b)
    myMainHintFrame->show();
  else
    myMainHintFrame->hide();
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::onHintDone()
{
  FuiModes::done();
  FuiCtrlModes::done(); // TT #2740
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::onHintCancel()
{
  FuiModes::cancel();
  FuiCtrlModes::cancel(); // TT #2740
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::addDefinitionWidget(QWidget* w)
{
  myDefinitionWidgetStack->addWidget(w);
  myDefinitionWidgetStack->show();
  myDefinitionWidgetStack->setCurrentWidget(w);
}
//----------------------------------------------------------------------------

void FuiQtMainWindow::removeDefinitionWidget(QWidget* w)
{
  myDefinitionWidgetStack->removeWidget(w);
  myDefinitionWidgetStack->hide();
}
