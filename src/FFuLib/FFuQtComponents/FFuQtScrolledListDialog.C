// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QVBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"
#include "FFuLib/FFuQtComponents/FFuQtScrolledList.H"
#include "FFuLib/FFuQtComponents/FFuQtScrolledListDialog.H"


FFuQtScrolledListDialog::FFuQtScrolledListDialog(QWidget*, bool withNotes,
                                                 int xpos, int ypos,
                                                 int width, int height,
                                                 const char* title,
                                                 const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,title,name)
{
  myItemSelector = new FFuQtScrolledList();
  if (withNotes) myNotes = new FFuQtNotes();
  myDialogButtons = new FFuQtDialogButtons(NULL,withNotes);

  this->initWidgets();

  QBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(myItemSelector->getQtWidget(),1);
  if (withNotes) layout->addWidget(myNotes->getQtWidget());
  layout->addWidget(myDialogButtons->getQtWidget());
}


void FFuQtScrolledListDialog::closeEvent(QCloseEvent*)
{
  this->onDlgButtonClicked(FFuDialogButtons::RIGHTBUTTON);
}
