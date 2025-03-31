// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"
#include "FFuLib/FFuQtComponents/FFuQtScrolledList.H"
#include "FFuLib/FFuQtComponents/FFuQtScrolledListDialog.H"


FFuQtScrolledListDialog::FFuQtScrolledListDialog(QWidget*,
						 int xpos, int ypos,
						 int width,int height,
						 const char* title,
						 const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,title,name)
{
  myItemSelector = new FFuQtScrolledList();
  myDialogButtons = new FFuQtDialogButtons();
  labNotesImage = new FFuQtLabel();
  labNotesLabel = new FFuQtLabel();
  labNotesText = new FFuQtLabel();

  this->initWidgets();

  QWidget* notesLabel = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(notesLabel);
  layout->setContentsMargins(0,0,0,0);
  layout->setAlignment(Qt::AlignLeft);
  layout->addWidget(static_cast<FFuQtLabel*>(labNotesImage));
  layout->addWidget(static_cast<FFuQtLabel*>(labNotesLabel));
  notesLabel->setLayout(layout);

  layout = new QVBoxLayout(this);
  layout->addWidget(static_cast<FFuQtScrolledList*>(myItemSelector));
  layout->addWidget(notesLabel);
  layout->addWidget(static_cast<FFuQtLabel*>(labNotesText));
  layout->addWidget(static_cast<FFuQtDialogButtons*>(myDialogButtons));
}


void FFuQtScrolledListDialog::closeEvent(QCloseEvent*)
{
  this->onDlgButtonClicked(FFuDialogButtons::RIGHTBUTTON);
}
