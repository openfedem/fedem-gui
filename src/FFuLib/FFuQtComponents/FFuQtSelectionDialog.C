// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QButtonGroup>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QFrame>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <algorithm>

#include "FFuLib/Icons/infoDialog.xpm"
#include "FFuLib/Icons/errorDialog.xpm"
#include "FFuLib/Icons/warningDialog.xpm"
#include "FFuLib/Icons/questionDialog.xpm"

#include "FFuLib/FFuQtComponents/FFuQtSelectionDialog.H"
#include "FFuLib/FFuAuxClasses/FFuaApplication.H"
#include "FFuLib/FFuAuxClasses/FFuQtAuxClasses/FFuaQtPixmapCache.H"
#include "FFuLib/FFuDialogType.H"


FFuQtSelectionDialog::FFuQtSelectionDialog(const std::string& msgText, int dialogType,
                                           const std::vector<std::string>& buttonTexts)
  : QDialog(NULL, Qt::Dialog)
{
  this->setWidget(this);
  this->setModal(true);

  QBoxLayout* column = new QVBoxLayout(this);
  column->setContentsMargins(10,10,10,10);
  column->setSpacing(5);
  QBoxLayout* messgRow  = new QHBoxLayout();
  QBoxLayout* buttonRow = new QHBoxLayout();

  QLabel* messageTextLabel = new QLabel(this);
  messageTextLabel->setText(msgText.c_str());
  messageTextLabel->setAlignment(Qt::AlignLeft|Qt::AlignTop);

  //set icon
  QPixmap pixmap;
  if (dialogType == FFuDialog::INFO)
    pixmap = FFuaQtPixmapCache::getPixmap(infoDialog);
  else if (dialogType == FFuDialog::ERROR)
    pixmap = FFuaQtPixmapCache::getPixmap(errorDialog);
  else if (dialogType == FFuDialog::WARNING)
    pixmap = FFuaQtPixmapCache::getPixmap(warningDialog);
  else if (dialogType == FFuDialog::QUESTION)
    pixmap = FFuaQtPixmapCache::getPixmap(questionDialog);

  column->addLayout(messgRow);
  QLabel* iconLabel = new QLabel(this);
  iconLabel->setPixmap(pixmap);
  iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
  iconLabel->setAlignment(Qt::AlignTop);
  messgRow->addWidget(iconLabel);
  messgRow->addWidget(messageTextLabel);
  myList = new QListWidget(this);
  myList->setSelectionMode(QAbstractItemView::SingleSelection);
  myList->hide();

  column->addWidget(myList);

  mySeparator = new QFrame(this);
  mySeparator->setFrameShape(QFrame::HLine);
  column->addWidget(mySeparator);
  mySeparator->hide();

  QButtonGroup* buttonGroup = new QButtonGroup(this);
  this->connect(buttonGroup, SIGNAL(idClicked(int)), this, SLOT(onButtonClicked(int)));
  column->addLayout(buttonRow);
  int id = 0;
  for (const std::string& btext : buttonTexts)
  {
    buttonRow->addItem(new QSpacerItem(0,0, QSizePolicy::Expanding));
    QPushButton* button = new QPushButton(this);
    button->setText(btext.c_str());
    buttonRow->addWidget(button);
    buttonGroup->addButton(button,id++);
  }
  buttonRow->addItem(new QSpacerItem(0,0, QSizePolicy::Expanding));
}
//----------------------------------------------------------------------------

int FFuQtSelectionDialog::execute()
{
  this->myClickedButtonId = -1;

  FFuaApplication::breakUserEventBlock(true);
  this->exec();
  FFuaApplication::breakUserEventBlock(false);

  return this->myClickedButtonId;
}

void FFuQtSelectionDialog::onButtonClicked(int buttonId)
{
  this->myClickedButtonId = buttonId;
  this->done(buttonId);
}

void FFuQtSelectionDialog::setList(const std::vector<std::string>& selectionList)
{
  myList->clear();
  myItems.clear();
  if (selectionList.empty())
    return;

  myItems.reserve(selectionList.size());
  for (const std::string& item : selectionList)
    myItems.push_back(new QListWidgetItem(item.c_str()));

  myList->show();
  mySeparator->show();
}

/*!
  Return -1 if nothing is selected. Else the index of the selection
*/
int FFuQtSelectionDialog::getSelection() const
{
  QList<QListWidgetItem*> selected = myList->selectedItems();
  if (selected.isEmpty())
    return -1;

  std::vector<QListWidgetItem*>::const_iterator it = std::find(myItems.begin(),myItems.end(),selected.first());
  return it == myItems.end() ? -1 : it - myItems.begin();
}
