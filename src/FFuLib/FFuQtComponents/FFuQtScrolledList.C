// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QShortcut>
#include <QMimeData>
#include <QApplication>
#include <QClipboard>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QKeyEvent>

#include "FFuLib/FFuQtComponents/FFuQtScrolledList.H"


FFuQtScrolledList::FFuQtScrolledList(QWidget* parent) : QListWidget(parent)
{
  this->setWidget(this);
  this->setAcceptDrops(true);
  this->setFocusPolicy(Qt::StrongFocus);

  QObject::connect(this, SIGNAL(activated(const QModelIndex&)),
                   this, SLOT(activate(const QModelIndex&)));
  QObject::connect(this, SIGNAL(pressed(const QModelIndex&)),
                   this, SLOT(browseSelect(const QModelIndex&)));

  QShortcut* qPaste = new QShortcut(Qt::CTRL + Qt::Key_V, this);
  QObject::connect(qPaste, SIGNAL(activated()), this, SLOT(paste()));

  QShortcut* qDelete = new QShortcut(Qt::CTRL + Qt::Key_X, this);
  QObject::connect(qDelete, SIGNAL(activated()), this, SLOT(deleteAll()));

  IAmEnabled = true;
}


void FFuQtScrolledList::setItems(const std::vector<std::string>& items)
{
  this->clear();
  for (const std::string& item : items)
    this->addItem(item.c_str());
  this->repaint();
}


int FFuQtScrolledList::getSelectedItemIndex() const
{
  return this->indexFromItem(this->currentItem()).row();
}


bool FFuQtScrolledList::selectItem(int index)
{
  if (index < 0 || index >= this->count())
    return false;

  this->setCurrentRow(index);
  return true;
}


std::string FFuQtScrolledList::getItemText(int index) const
{
  QListWidgetItem* item = this->item(index);
  return item ? item->text().toStdString() : "";
}


bool FFuQtScrolledList::isItemSelected(int index) const
{
  if (index < 0 || index >= this->count())
    return false;

  return this->getSelectedItemIndex() == index;
}


void FFuQtScrolledList::setSensitivity(bool isSensitive)
{
  int rgb = isSensitive ? 0 : 160;
  QPalette currPalette = this->palette();
  currPalette.setColor(QPalette::Text, QColor(rgb,rgb,rgb));
  this->setPalette(currPalette);

  this->setSelectionMode(isSensitive ? SingleSelection : NoSelection);
  IAmEnabled = isSensitive;
}


void FFuQtScrolledList::browseSelect(const QModelIndex& index)
{
  myBrowseSelectCB.invoke(index.row());
}

void FFuQtScrolledList::activate(const QModelIndex& index)
{
  myActivateCB.invoke(index.row());
}

void FFuQtScrolledList::deleteAll()
{
  if (IAmEnabled)
    myClearCB.invoke();
}

void FFuQtScrolledList::paste()
{
  if (IAmEnabled)
    myPasteCB.invoke(QApplication::clipboard()->text().toStdString());
}

void FFuQtScrolledList::dropEvent(QDropEvent* e)
{
  if (IAmEnabled)
    myPasteCB.invoke(e->mimeData()->text().toStdString());
}

void FFuQtScrolledList::dragEnterEvent(QDragEnterEvent* e)
{
  if (IAmEnabled && e->mimeData()->hasText())
    e->accept();
}

bool FFuQtScrolledList::event(QEvent* e)
{
  if (IAmEnabled && e->type() == QEvent::ShortcutOverride)
    switch (static_cast<QKeyEvent*>(e)->key())
      {
      case Qt::Key_Delete:
      case Qt::Key_Home:
      case Qt::Key_End:
      case Qt::Key_Backspace:
        static_cast<QKeyEvent*>(e)->accept();
      default:
        break;
      }

  return this->QListWidget::event(e);
}

void FFuQtScrolledList::keyPressEvent(QKeyEvent* e)
{
  if (IAmEnabled && e->key() == Qt::Key_Delete)
  {
    myDeleteButtonCB.invoke();
    e->accept();
  }
}

void FFuQtScrolledList::mousePressEvent(QMouseEvent* e)
{
#if !defined(win32) && !defined(win64)
  if (IAmEnabled && e->button() == Qt::MidButton)
  {
    this->paste();
    e->accept();
  }
#endif

  this->QListWidget::mousePressEvent(e);
}
