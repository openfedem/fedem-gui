// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QMdiSubWindow>
#include <QPixmap>
#include <QIcon>

#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtCtrlModeller.H"
#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtCtrlGridAttributes.H"
#ifdef USE_INVENTOR
#include "vpmDisplay/FdViewer.H"
#else
#include <iostream>
#endif

extern const char* ctrlSystem_xpm[];


FuiCtrlModeller* FuiCtrlModeller::create(FFuComponentBase* parent,
					 int xpos, int ypos,
					 int width,int height,
					 const char* title,
					 const char* name)
{
  return new FuiQtCtrlModeller(dynamic_cast<QWidget*>(parent),
			       xpos,ypos,width,height,title,name);
}


FuiQtCtrlModeller::FuiQtCtrlModeller(QWidget* parent,
				     int xpos, int ypos,
				     int width,int height,
				     const char* title,
				     const char* name)
  : FFuQtMDIWindow(parent,xpos,ypos,width,height,title,name)
{
#ifdef USE_INVENTOR
  myViewer = FdViewer::create(this,true);
#else
  std::cerr <<" *** FuiQtCtrlModeller: No viewer in this build."<< std::endl;
#endif
  myCtrlGridAttributes = new FuiQtCtrlGridAttributes();
  myQtSubWindow->setWindowIcon(QIcon(QPixmap(ctrlSystem_xpm)));

  this->initWidgets();
}


void FuiQtCtrlModeller::resizeEvent(QResizeEvent* e)
{
  this->QWidget::resizeEvent(e);

  if (myViewer)
    myViewer->setSizeGeometry(0,0,this->getWidth(),this->getHeight());
}
