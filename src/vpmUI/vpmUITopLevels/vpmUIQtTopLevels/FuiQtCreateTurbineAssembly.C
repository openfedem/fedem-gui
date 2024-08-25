// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FuiQtCreateTurbineAssembly.H"
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

#include <QApplication>
#include <QClipboard>


FuiCreateTurbineAssembly* FuiCreateTurbineAssembly::create(int xpos, int ypos,
							   int width, int height,
							   const char* title,
							   const char* name)
{
  return new FuiQtCreateTurbineAssembly(xpos,ypos,width,height,title,name);
}


FuiQtCreateTurbineAssembly::FuiQtCreateTurbineAssembly(int xpos, int ypos,
						       int width, int height,
						       const char* title,
						       const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,title,name,Qt::MSWindowsFixedSizeDialogHint)
{
  this->headerImage         = new FFuQtLabel(this);
  this->modelImage          = new FFuQtLabel(this);
  this->drivelineTypeLabel  = new FFuQtLabel(this);
  this->drivelineTypeMenu   = new FFuQtOptionMenu(this);
  this->bearingsLabel       = new FFuQtLabel(this);
  this->bearingsMenu        = new FFuQtOptionMenu(this);
  this->nameLabel           = new FFuQtLabel(this);
  this->towerBaseFrame      = new FFuQtLabelFrame(this);
  this->hubFrame            = new FFuQtLabelFrame(this);
  this->drivelineFrame      = new FFuQtLabelFrame(this);
  this->nacelleFrame        = new FFuQtLabelFrame(this);
  this->nameField           = new FFuQtLabelField(this);
  for (FFuLabelField*& field : myFields) field = new FFuQtLabelField(this);
  this->copyButton          = new FFuQtPushButton(this);
  this->pasteButton         = new FFuQtPushButton(this);
  this->bladesNumLabel      = new FFuQtLabel(this);
  this->bladesNumField      = new FFuQtSpinBox(this);
  this->bladesDesignLabel   = new FFuQtLabel(this);
  this->bladesDesignField   = new FFuQtFileBrowseField(this);
  this->incCtrlSysToggle    = new FFuQtToggleButton(this);
  this->notesImage          = new FFuQtLabel(this);
  this->notesLabel          = new FFuQtLabel(this);
  this->notesSeparator      = new FFuQtLabelFrame(this);
  this->notesText           = new FFuQtLabel(this);
  this->dialogButtons       = new FFuQtDialogButtons(this);

  this->initWidgets();
}
//----------------------------------------------------------------------------

void FuiQtCreateTurbineAssembly::onCopyButtonClicked()
{
  QString text = QString::number(drivelineTypeMenu->getSelectedOption());
  text.append("\n");
  text.append(QString::number(bearingsMenu->getSelectedOption()));
  text.append("\n");
  text.append((nameField->getText()+"\n").c_str());
  for (FFuLabelField* field : myFields)
    text.append((field->getText()+"\n").c_str());

  QApplication::clipboard()->setText(text);
}
//----------------------------------------------------------------------------

void FuiQtCreateTurbineAssembly::onPasteButtonClicked()
{
  QString text = QApplication::clipboard()->text();
  QStringList list = text.split('\n');

  if (list.size() > 0 && drivelineTypeMenu->getSensitivity())
  {
    int n = list.at(0).toLong();
    drivelineTypeMenu->selectOption(n);
    this->onDrivelineTypeChanged(n);
  }

  if (list.size() > 1 && bearingsMenu->getSensitivity())
  {
    int n = list.at(1).toLong();
    bearingsMenu->selectOption(n);
    this->onBearingsChanged(n);
  }

  if (list.size() > 2 && nameField->getSensitivity())
    nameField->setValue(list.at(2).toStdString());

  for (size_t i = 3; (int)i < list.size() && i-3 < myFields.size(); i++)
    if (myFields[i-3]->getSensitivity())
      myFields[i-3]->setValue(list.at(i).toStdString());
}
