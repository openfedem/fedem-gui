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
#include <QLabel>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtSpinBox.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtFileBrowseField.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"

#include "FuiQtCreateTurbineAssembly.H"


FuiCreateTurbineAssembly* FuiCreateTurbineAssembly::create(int xpos, int ypos,
							   int width,int height,
							   const char* title,
							   const char* name)
{
  return new FuiQtCreateTurbineAssembly(xpos,ypos,width,height,title,name);
}


FuiQtCreateTurbineAssembly::FuiQtCreateTurbineAssembly(int xpos, int ypos,
						       int width,int height,
						       const char* title,
						       const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,
                       title,name,Qt::MSWindowsFixedSizeDialogHint)
{
  headerImage = new FFuQtLabel();
  modelImage  = new FFuQtLabel();
  notesText   = new FFuQtLabel();

  towerBaseFrame = new FFuQtLabelFrame();
  hubFrame       = new FFuQtLabelFrame();
  drivelineFrame = new FFuQtLabelFrame();
  nacelleFrame   = new FFuQtLabelFrame();

  drivelineTypeMenu = new FFuQtOptionMenu();
  bearingsMenu      = new FFuQtOptionMenu();
  nameField         = new FFuQtIOField();
  for (FFuLabelField*& field : myFields)
    field = new FFuQtLabelField();

  copyButton  = new FFuQtPushButton();
  pasteButton = new FFuQtPushButton();

  bladesNumField    = new FFuQtSpinBox();
  bladesDesignField = new FFuQtFileBrowseField(NULL);
  incCtrlSysToggle  = new FFuQtToggleButton();

  dialogButtons = new FFuQtDialogButtons();

  this->initWidgets();

  for (FFuLabelField*& field : myFields)
  {
    field->setLabelWidth(15);
    static_cast<FFuQtLabelField*>(field)->setFixedHeight(20);
  }

  QBoxLayout* layout = NULL;
  QGridLayout* gl = NULL;
  int i, row = 0;

  layout = new QVBoxLayout(static_cast<FFuQtLabelFrame*>(towerBaseFrame));
  for (i = FWP::TOWER_X; i <= FWP::TOWER_T; i++)
    layout->addWidget(static_cast<FFuQtLabelField*>(myFields[i]));

  layout = new QVBoxLayout(static_cast<FFuQtLabelFrame*>(hubFrame));
  layout->setContentsMargins(10,2,10,10);
  layout->setSpacing(2);
  for (i = FWP::H1; i <= FWP::BETA; i++)
    layout->addWidget(static_cast<FFuQtLabelField*>(myFields[i]));

  gl = new QGridLayout(static_cast<FFuQtLabelFrame*>(drivelineFrame));
  gl->setContentsMargins(10,2,10,10);
  gl->setHorizontalSpacing(20);
  gl->setVerticalSpacing(2);
  for (i = FWP::D1, row = 0; i <= FWP::D5; i++)
    gl->addWidget(static_cast<FFuQtLabelField*>(myFields[i]),row++,0);
  for (i = FWP::D6, row = 0; i <= FWP::THETA; i++)
    gl->addWidget(static_cast<FFuQtLabelField*>(myFields[i]),row++,1);

  layout = new QVBoxLayout(static_cast<FFuQtLabelFrame*>(nacelleFrame));
  for (i = FWP::COG_X; i <= FWP::COG_Z; i++)
  {
    myFields[i]->setLabelWidth(30);
    layout->addWidget(static_cast<FFuQtLabelField*>(myFields[i]));
  }
  
  QWidget* qCopyPaste = new QWidget();
  layout = new QVBoxLayout(qCopyPaste);
  layout->setContentsMargins(10,10,0,0);
  layout->addStretch();
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(copyButton));
  layout->addWidget(dynamic_cast<FFuQtPushButton*>(pasteButton));

  QWidget* qNumBlades = new QWidget();
  layout = new QHBoxLayout(qNumBlades);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Number of blades"));
  layout->addWidget(static_cast<FFuQtSpinBox*>(bladesNumField));
  layout->addSpacing(20);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(incCtrlSysToggle));

  QWidget* qLeft = new QWidget();
  gl = new QGridLayout(qLeft);
  gl->setContentsMargins(0,0,0,0);
  for (int col = 0; col < 3; col++)
    gl->setColumnStretch(col,1);
  gl->addWidget(new QLabel("Driveline type"), row=0,0);
  gl->addWidget(new QLabel("Bearings"), row,1);
  gl->addWidget(new QLabel("Name"), row++,2);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(drivelineTypeMenu), row,0);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(bearingsMenu), row,1);
  gl->addWidget(static_cast<FFuQtIOField*>(nameField), row++,2);
  gl->addWidget(static_cast<FFuQtLabelFrame*>(towerBaseFrame), row,0);
  gl->addWidget(static_cast<FFuQtLabelFrame*>(drivelineFrame), row++,1,1,-1);
  gl->addWidget(static_cast<FFuQtLabelFrame*>(hubFrame), row,0);
  gl->addWidget(static_cast<FFuQtLabelFrame*>(nacelleFrame), row,1);
  gl->addWidget(qCopyPaste, row++,2);
  gl->addWidget(static_cast<FFuQtFileBrowseField*>(bladesDesignField),
		row++,0,1,-1);
  gl->addWidget(qNumBlades, row++,0,1,-1);
  gl->addWidget(new FFuQtNotesLabel(), row++,0);
  gl->addWidget(static_cast<FFuQtLabel*>(notesText), row++,0,1,-1);
  gl->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons), row++,0,1,-1);

  QWidget* qMain = new QWidget();
  layout = new QHBoxLayout(qMain);
  layout->setSpacing(0);
  layout->setContentsMargins(10,0,0,10);
  layout->addWidget(qLeft);
  layout->addWidget(static_cast<FFuQtLabel*>(modelImage));

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(static_cast<FFuQtLabel*>(headerImage));
  layout->addWidget(qMain);
}
//----------------------------------------------------------------------------

void FuiQtCreateTurbineAssembly::onCopyButtonClicked()
{
  std::string text = std::to_string(drivelineTypeMenu->getSelectedOption());
  text.append("\n" + std::to_string(bearingsMenu->getSelectedOption()));
  text.append("\n" + nameField->getValue());
  for (FFuLabelField* field : myFields)
    text.append("\n" + field->getText());
  text.append("\n");

  QApplication::clipboard()->setText(text.c_str());
}
//----------------------------------------------------------------------------

void FuiQtCreateTurbineAssembly::onPasteButtonClicked()
{
  QString text = QApplication::clipboard()->text();
  QStringList list = text.split('\n');
  const size_t len = list.size();

  if (len > 0 && drivelineTypeMenu->getSensitivity())
  {
    int n = list.at(0).toLong();
    drivelineTypeMenu->selectOption(n);
    this->onDrivelineTypeChanged(n);
  }

  if (len > 1 && bearingsMenu->getSensitivity())
  {
    int n = list.at(1).toLong();
    bearingsMenu->selectOption(n);
    this->onBearingsChanged(n);
  }

  if (len > 2 && nameField->getSensitivity())
    nameField->setValue(list.at(2).toStdString());

  for (size_t i = 3; i < len && i-3 < myFields.size(); i++)
    if (myFields[i-3]->getSensitivity())
      myFields[i-3]->setValue(list.at(i).toStdString());
}
