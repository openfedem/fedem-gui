// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtScale.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtColorDialog.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"

#include "FuiQtViewSettings.H"


//////////////////////////////////////////////////////////////////////
//
//  Qt implementation of static create method in FuiViewSettings
//  Used instead of constructor :
//

FuiViewSettings* FuiViewSettings::create(int xpos, int ypos,
					 int width,int height,
					 const char* title,
					 const char* name)
{
  return new FuiQtViewSettings(xpos,ypos,width,height,title,name);
}


//////////////////////////////////////////////////////////////////////
//
//  Constructor and destructor :
//

FuiQtViewSettings::FuiQtViewSettings(int xpos, int ypos,
				     int width,int height,
				     const char* title,
				     const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,
                       title,name,Qt::WindowStaysOnTopHint)
{
  FFuQtLabelFrame* qtSymbolsFrame;
  FFuQtLabelFrame* qtDefaultColorFrame;
  FFuQtLabelFrame* qtViewerOptionsFrame;
  mySymbolsFrame       = qtSymbolsFrame       = new FFuQtLabelFrame();
  myDefaultColorFrame  = qtDefaultColorFrame  = new FFuQtLabelFrame();
  myViewerOptionsFrame = qtViewerOptionsFrame = new FFuQtLabelFrame();

  // Symbols
  FFuQtFrame* qtColorFrames[FuiViewSettings::NSYMBOLS];

  for (int i = 0; i < FuiViewSettings::NSYMBOLS; i++)
  {
    mySymbolToggles[i]    = new FFuQtToggleButton();
    myColorFrames[i]      = qtColorFrames[i] = new FFuQtFrame();
    myColorEditButtons[i] = new FFuQtPushButton();
    myColorDialog[i]      = new FFuQtColorDialog(xpos-345,ypos+40);

    qtColorFrames[i]->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    qtColorFrames[i]->setLineWidth(2);
  }

  for (FFuToggleButton*& toggle : myJointSymbolToggles)
    toggle = new FFuQtToggleButton();

  myBeamTriadsToggle = new FFuQtToggleButton();
  myBeamsToggle = new FFuQtToggleButton();
  myBeamCSToggle = new FFuQtToggleButton();
  myPartsToggle = new FFuQtToggleButton();
  for (FFuToggleButton*& toggle : myPartSymbolToggles)
    toggle = new FFuQtToggleButton();

  myRefPlaneToggle = new FFuQtToggleButton();
  mySeaStateToggle = new FFuQtToggleButton();
  myWavesToggle = new FFuQtToggleButton();

  FFuQtScale* qtLineWidthScale;
  mySymbolSizeField = new FFuQtIOField();
  myLineWidthScale = qtLineWidthScale = new FFuQtScale();
  qtLineWidthScale->setOrientation(Qt::Horizontal);

  // Default colors
  FFuQtScale* qtFogScale;
  FFuQtFrame* qtGroundedColFrame;
  FFuQtFrame* qtUnattachedColFrame;
  FFuQtFrame* qtBackgroundColFrame;

  myGroundedColFrame   = qtGroundedColFrame   = new FFuQtFrame();
  myUnattachedColFrame = qtUnattachedColFrame = new FFuQtFrame();
  myBackgroundColFrame = qtBackgroundColFrame = new FFuQtFrame();

  qtUnattachedColFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  qtBackgroundColFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  qtGroundedColFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  qtUnattachedColFrame->setLineWidth(2);
  qtGroundedColFrame->setLineWidth(2);
  qtBackgroundColFrame->setLineWidth(2);

  myGroundedColEditButton   = new FFuQtPushButton();
  myUnattachedColEditButton = new FFuQtPushButton();
  myBackgroundColEditButton = new FFuQtPushButton();

  // color editors
  myGroundedColDialog   = new FFuQtColorDialog();
  myUnattachedColDialog = new FFuQtColorDialog();
  myBackgroundColDialog = new FFuQtColorDialog();

  // viewer options
  myFogToggle          = new FFuQtToggleButton();
  myTransparencyToggle = new FFuQtToggleButton();
  myAntialiazingToggle = new FFuQtToggleButton();

  myFogScale = qtFogScale = new FFuQtScale();
  qtFogScale->setOrientation(Qt::Horizontal);

  myCloseButton = new FFuQtPushButton();

  // call to the UI independent widget manipulation
  this->initWidgets();

  // Lambda function making an indented VBox layout
  auto&& indentedVBox = [](QWidget* owner)
  {
    QBoxLayout* layout = new QVBoxLayout(owner);
    layout->setContentsMargins(20,0,0,0);
    layout->setSpacing(0);
    return layout;
  };

  // Create separate parent widgets for some componets
  // with their own layouts to position them properly

  QWidget* qJointToggles = new QWidget();
  QBoxLayout* layout = indentedVBox(qJointToggles);
  for (FFuToggleButton* toggle : myJointSymbolToggles)
    layout->addWidget(dynamic_cast<FFuQtToggleButton*>(toggle));

  QWidget* qTriadToggles = new QWidget();
  layout = indentedVBox(qTriadToggles);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myBeamTriadsToggle));

  QWidget* qBeamToggles = new QWidget();
  layout = indentedVBox(qBeamToggles);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myBeamCSToggle));

  QWidget* qPartToggles = new QWidget();
  layout = indentedVBox(qPartToggles);
  for (FFuToggleButton* toggle : myPartSymbolToggles)
    layout->addWidget(dynamic_cast<FFuQtToggleButton*>(toggle));

  QWidget* qSeaToggles = new QWidget();
  layout = indentedVBox(qSeaToggles);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myWavesToggle));

  QWidget* qSize = new QWidget();
  QGridLayout* gl = new QGridLayout(qSize);
  gl->setContentsMargins(0,5,0,0);
  gl->setColumnStretch(1,1);
  gl->addWidget(new QLabel("Size:"),      0,0);
  gl->addWidget(new QLabel("Line width:"),1,0);
  gl->addWidget(static_cast<FFuQtIOField*>(mySymbolSizeField),0,1);
  gl->addWidget(qtLineWidthScale,1,1);

  QWidget* qFogToggle = new QWidget();
  layout = new QHBoxLayout(qFogToggle);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myFogToggle));
  layout->addWidget(new QLabel("   Visibility:"));
  layout->addWidget(qtFogScale);

  // Need a separate parent widget for the Close button also
  // with layout to make it appear in the lower left corner
  QWidget* qCloseBtn = new QWidget();
  layout = new QHBoxLayout(qCloseBtn);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(myCloseButton));
  layout->addStretch(1);

  // Layout for the mechanism symbols frame
  // with toggles and color edit buttons
  int r = 0;
  gl = new QGridLayout(qtSymbolsFrame);
  gl->setContentsMargins(10,5,10,10);
  gl->setColumnStretch(0,1);
  gl->setVerticalSpacing(0);
  gl->addWidget(new QLabel("Visible"),r,0);
  gl->addWidget(new QLabel("Color"),r++,1);
  for (int i = 0; i < FuiViewSettings::NSYMBOLS; i++)
  {
    gl->addWidget(dynamic_cast<FFuQtToggleButton*>(mySymbolToggles[i]),r,0);
    gl->addWidget(dynamic_cast<FFuQtPushButton*>(myColorEditButtons[i]),r,2);
    gl->addWidget(qtColorFrames[i],r++,1);
    if (i == FuiViewSettings::JOINT)
      gl->addWidget(qJointToggles,r++,0,1,-1);
  }
  gl->addWidget(qTriadToggles,r++,0,1,-1);
  gl->addWidget(dynamic_cast<FFuQtToggleButton*>(myBeamsToggle),r++,0,1,-1);
  gl->addWidget(qBeamToggles,r++,0,1,-1);
  gl->addWidget(dynamic_cast<FFuQtToggleButton*>(myPartsToggle),r++,0,1,-1);
  gl->addWidget(qPartToggles,r++,0,1,-1);
  gl->addWidget(dynamic_cast<FFuQtToggleButton*>(myRefPlaneToggle),r++,0,1,-1);
  gl->addWidget(dynamic_cast<FFuQtToggleButton*>(mySeaStateToggle),r++,0,1,-1);
  gl->addWidget(qSeaToggles,r++,0,1,-1);
  gl->addWidget(qSize,r++,0,1,-1);

  // Layout for the frame with default color edit buttons
  gl = new QGridLayout(qtDefaultColorFrame);
  gl->setContentsMargins(10,5,10,10);
  gl->setVerticalSpacing(0);
  gl->setColumnStretch(0,1);
  gl->setColumnMinimumWidth(1,30);
  gl->addWidget(new QLabel("Grounded triads"),   0,0);
  gl->addWidget(new QLabel("Unattached symbols"),1,0);
  gl->addWidget(new QLabel("Modeler background"),2,0);
  gl->addWidget(qtGroundedColFrame  ,0,1);
  gl->addWidget(qtUnattachedColFrame,1,1);
  gl->addWidget(qtBackgroundColFrame,2,1);
  gl->addWidget(dynamic_cast<FFuQtPushButton*>(myGroundedColEditButton),0,2);
  gl->addWidget(dynamic_cast<FFuQtPushButton*>(myUnattachedColEditButton),1,2);
  gl->addWidget(dynamic_cast<FFuQtPushButton*>(myBackgroundColEditButton),2,2);

  // Layout for the frame with viewer option toggles
  layout = new QVBoxLayout(qtViewerOptionsFrame);
  layout->addWidget(qFogToggle);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myTransparencyToggle));
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(myAntialiazingToggle));

  // The main layout
  layout = new QVBoxLayout(this);
  layout->setContentsMargins(10,5,10,10);
  layout->addWidget(qtSymbolsFrame,6);
  layout->addWidget(qtDefaultColorFrame,1);
  layout->addWidget(qtViewerOptionsFrame);
  layout->addWidget(qCloseBtn);
}
