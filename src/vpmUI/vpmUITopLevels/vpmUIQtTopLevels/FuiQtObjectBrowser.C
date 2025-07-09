// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QApplication>
#include <QClipboard>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPixmap>
#include <QLabel>

#include "vpmUI/Pixmaps/objectBrowser1.xpm"
#include "vpmUI/Pixmaps/objectBrowser2.xpm"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"

#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtListView.H"
#include "FFuLib/FFuQtComponents/FFuQtMemo.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"

#include "FuiQtObjectBrowser.H"


FuiObjectBrowser* FuiObjectBrowser::create(int xpos, int ypos,
					   int width, int height,
					   const char* title,
					   const char* name)
{
  return new FuiQtObjectBrowser(xpos,ypos,width,height,title,name);
}


FuiQtObjectBrowser::FuiQtObjectBrowser(int xpos, int ypos,
				       int width, int height,
				       const char* title,
				       const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,title,name)
{
  searchField    = new FFuQtIOField();
  searchButton   = new FFuQtPushButton();
  searchView     = new FFuQtListView();
  outputMemo     = new FFuQtMemo();
  closeButton    = new FFuQtPushButton();
  helpButton     = new FFuQtPushButton();
  copyDataButton = new FFuQtPushButton();

  this->initWidgets();

  QLabel* qImage1 = new QLabel();
  qImage1->setPixmap(QPixmap(objectBrowser1_xpm));
  // Crop the 2nd image (size 827x77 pixels) using only the lower 5 pixel rows
  QLabel* qImage2 = new QLabel();
  qImage2->setPixmap(QPixmap(objectBrowser2_xpm).copy(0,72,827,5));
  qImage2->setScaledContents(true);
  qImage2->setMinimumWidth(200);
  // Use a stretchable white frame to extend the 1st image to the full width
  QFrame* qImage3 = new QFrame();
  qImage3->setFrameStyle(QFrame::NoFrame);
  qImage3->setStyleSheet("background-color: rgb(255,255,255)");
  // Make the stretchable image
  QWidget* qImage0 = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qImage0);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  layout->addWidget(qImage1);
  layout->addWidget(qImage3,1);
  // Make the total heading composed of two stretchable images
  QWidget* qHeading = new QWidget();
  layout = new QVBoxLayout(qHeading);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  layout->addWidget(qImage0);
  layout->addWidget(qImage2);

  QWidget* qSearch = new QWidget();
  layout = new QHBoxLayout(qSearch);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(static_cast<FFuQtIOField*>(searchField),1);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(searchButton));

  QWidget* qButtons = new QWidget();
  layout = new QHBoxLayout(qButtons);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(closeButton));
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(helpButton));
  QFrame* qSep = new QFrame();
  qSep->setFrameStyle(QFrame::VLine | QFrame::Sunken);
  qSep->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
  layout->addWidget(qSep);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(copyDataButton));

  QWidget* qBrowser = new QWidget();
  layout = new QVBoxLayout(qBrowser);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qSearch);
  layout->addWidget(dynamic_cast<FFuQtListView*>(searchView),1);
  layout->addWidget(qButtons);

  QWidget* qMainW = new QWidget();
  QGridLayout* gl = new QGridLayout(qMainW);
  gl->setContentsMargins(10,0,10,8);
  gl->addWidget(new QLabel("Search and browse objects:"),0,0);
  gl->addWidget(new QLabel("Object details:"),0,1);
  gl->addWidget(qBrowser,1,0);
  gl->addWidget(static_cast<FFuQtMemo*>(outputMemo),1,1);

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qHeading);
  layout->addWidget(qMainW,1);
}


void FuiQtObjectBrowser::onCopyDataButtonClicked()
{
  QApplication::clipboard()->setText(outputMemo->getText().c_str());
}
