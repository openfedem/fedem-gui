// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QMdiSubWindow>
#include <QPixmap>
#include <QIcon>

#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtGraphView.H"

extern const char* graph_xpm[];

//----------------------------------------------------------------------------

//  Qt implementation of static create method in FuiGraphViewTLS:
FuiGraphView* FuiGraphViewTLS::create(FFuComponentBase* parent,
                                      int xpos, int ypos,
                                      int width, int height,
                                      const char* title,
                                      const char* name)
{
  FuiGraphViewTLS* tls = new FuiQtGraphViewTLS(dynamic_cast<QWidget*>(parent),
                                               xpos,ypos,width,height,title,name);
  tls->popUp();

  return tls->getGraphViewComp();
}
//----------------------------------------------------------------------------

FuiQtGraphViewTLS::FuiQtGraphViewTLS(QWidget* parent,
                                     int xpos, int ypos,
                                     int width, int height,
                                     const char* title,
                                     const char* name)
  : FFuQtMDIWindow(parent,xpos,ypos,width,height,title,name)
{
  myView = new FuiQtGraphView(this);

  myQtSubWindow->setWindowIcon(QIcon(QPixmap(graph_xpm)));

  this->setFocusProxy(dynamic_cast<QWidget*>(myView));

  FFuUAExistenceHandler::invokeCreateUACB(this);

  myView->updateSession();
}
//----------------------------------------------------------------------------

FuiQtGraphView::FuiQtGraphView(QWidget* parent, const char* name)
  : FFuQt2DPlotter(parent,name)
{
  this->setFocusPolicy(Qt::StrongFocus);

  this->initWidgets();

  //this->onPoppedUpFromMem();//hack -> causes crash...
}
//----------------------------------------------------------------------------

void FuiQtGraphView::setVisible(bool visible)
{
  //TODO isVisible/isMinimized doesn't work on a mdi window yet,
  //hack :onPoppedUpFrom/ToMem on construction/destr
  //bool wasvisible = this->isVisible();
  this->FFuQt2DPlotter::setVisible(visible);
  /*
  if (!wasvisible && visible)
    this->onPoppedUpFromMem();
  else if (wasvisible && !visible)
    this->onPoppedDownToMem();
  */
}
//----------------------------------------------------------------------------

void FuiQtGraphView::showEvent(QShowEvent* e)
{
  this->FFuQt2DPlotter::showEvent(e);
  this->onPoppedUp();
}
//----------------------------------------------------------------------------

void FuiQtGraphView::hideEvent(QHideEvent* e)
{
  this->FFuQt2DPlotter::hideEvent(e);
  this->onPoppedDown();
}
//----------------------------------------------------------------------------

void FuiQtGraphViewTLS::resizeEvent(QResizeEvent* e)
{
  this->QWidget::resizeEvent(e);

  myView->setSizeGeometry(0,0,this->getWidth(),this->getHeight());
}
