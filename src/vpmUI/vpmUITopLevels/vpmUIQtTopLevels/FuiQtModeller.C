// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QMdiSubWindow>
#include <QPixmap>
#include <QIcon>

#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtModeller.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtPlayPanel.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQt3DPoint.H"
#ifdef USE_INVENTOR
#include "vpmDisplay/FdViewer.H"
#else
#include <iostream>
#endif

extern const char* openMechModeller_xpm[];


FuiModeller* FuiModeller::create(FFuComponentBase* parent,
				 int xpos, int ypos,
				 int width,int height,
				 const char* title,
				 const char* name)
{
  return new FuiQtModeller(parent->getQtWidget(),
                           xpos,ypos,width,height,title,name);
}


FuiQtModeller::FuiQtModeller(QWidget* parent,
			     int xpos, int ypos,
			     int width,int height,
			     const char* title,
			     const char* name)
  : FFuQtMDIWindow(parent,xpos,ypos,width,height,title,name)
{
  this->myPlayPanel = new FuiQtPlayPanel(this);
  this->my3DpointUI = new FuiQt3DPoint(this,"XYZ",2,true);
#ifdef USE_INVENTOR
  this->myViewer = FdViewer::create(this);
#else
  std::cerr <<" *** FuiQtModeller: No viewer in this build."<< std::endl;
#endif
  myQtSubWindow->setWindowIcon(QIcon(QPixmap(openMechModeller_xpm)));

  this->initWidgets();
}


void FuiQtModeller::resizeEvent(QResizeEvent* e)
{
  this->QWidget::resizeEvent(e);

  int w = this->getWidth();
  int h = this->getHeight();

  myPlayPanel->setSizeGeometry(w-130,h-200,130,200);
  my3DpointUI->setSizeGeometry(w-180,h-125,180,125);

  if (myViewer)
    myViewer->setSizeGeometry(0,0,w,h);
}
