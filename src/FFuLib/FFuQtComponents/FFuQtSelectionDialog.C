// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QFrame>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>

#include "FFuLib/Icons/infoDialog.xpm"
#include "FFuLib/Icons/errorDialog.xpm"
#include "FFuLib/Icons/warningDialog.xpm"
#include "FFuLib/Icons/questionDialog.xpm"

#include "FFuLib/FFuQtComponents/FFuQtSelectionDialog.H"
#include "FFuLib/FFuAuxClasses/FFuQtAuxClasses/FFuaQtPixmapCache.H"
#include "FFuLib/FFuDialogType.H"


FFuQtSelectionDialog::FFuQtSelectionDialog(int dialogType,
                                           const std::string& msgText,
                                           const Strings& buttonTexts,
                                           const Strings& selList)
  : QDialog(NULL, Qt::Dialog)
{
  this->setWidget(this);
  this->setModal(true);

  QBoxLayout* column = new QVBoxLayout(this);

  QLabel* iconLabel = new QLabel();
  iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
  iconLabel->setAlignment(Qt::AlignTop);
  switch (dialogType) {
  case FFuDialog::INFO:
    iconLabel->setPixmap(FFuaQtPixmapCache::getPixmap(infoDialog));
    break;
  case FFuDialog::ERROR:
    iconLabel->setPixmap(FFuaQtPixmapCache::getPixmap(errorDialog));
    break;
  case FFuDialog::WARNING:
    iconLabel->setPixmap(FFuaQtPixmapCache::getPixmap(warningDialog));
    break;
  case FFuDialog::QUESTION:
    iconLabel->setPixmap(FFuaQtPixmapCache::getPixmap(questionDialog));
    break;
  }

  QLabel* messageTextLabel = new QLabel(msgText.c_str());
  messageTextLabel->setAlignment(Qt::AlignLeft|Qt::AlignTop);

  QBoxLayout* row = new QHBoxLayout();
  row->addWidget(iconLabel);
  row->addWidget(messageTextLabel);

  column->addLayout(row);

  if (!selList.empty())
  {
    myList = new QListWidget();
    myList->setSelectionMode(QAbstractItemView::SingleSelection);
    for (const std::string& item : selList)
      myList->addItem(item.c_str());

    column->addWidget(myList);

    QFrame* separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);

    column->addWidget(separator);
  }

  QButtonGroup* buttonGroup = new QButtonGroup();
  for (size_t id = 0; id < buttonTexts.size(); id++)
    buttonGroup->addButton(new QPushButton(buttonTexts[id].c_str()),id);

  row = new QHBoxLayout();
  for (size_t id = 0; id < buttonTexts.size(); id++)
  {
    row->addStretch(1);
    row->addWidget(buttonGroup->button(id));
  }
  row->addStretch(1);

  column->addLayout(row);

  this->connect(buttonGroup, SIGNAL(idClicked(int)), this, SLOT(done(int)));
}


int FFuQtSelectionDialog::getSelection() const
{
  QListWidgetItem* item = myList->currentItem();
  return item ? myList->indexFromItem(item).row() : -1;
}
