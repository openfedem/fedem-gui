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


FFuQtScrolledList::FFuQtScrolledList(QWidget* parent) : Q3ListBox(parent,"FFuQtScrolledList")
{
  this->setWidget(this);
  this->setAcceptDrops(true);
  this->setFocusPolicy(Qt::StrongFocus);

  QObject::connect(this,SIGNAL(selected(int)),this,SLOT(activate(int)));
  QObject::connect(this,SIGNAL(highlighted(int)),this,SLOT(browseSelect(int)));

  QShortcut* qPaste = new QShortcut(Qt::CTRL + Qt::Key_V, this);
  QObject::connect(qPaste, SIGNAL(activated()), this, SLOT(paste()));

  QShortcut* qDelete = new QShortcut(Qt::CTRL + Qt::Key_X, this);
  QObject::connect(qDelete, SIGNAL(activated()), this, SLOT(deleteAll()));

  IAmEnabled = true;
}


void FFuQtScrolledList::addItem(const std::string& item)
{
  this->insertItem(item.c_str());
}


void FFuQtScrolledList::setItems(const std::vector<std::string>& items)
{
  this->setAutoUpdate(false);
  this->clear();
  for (const std::string& item : items)
    this->insertItem(item.c_str());
  this->setAutoUpdate(true);
  this->repaint();
}


void FFuQtScrolledList::deleteItem(int index)
{
  this->removeItem(index);
}


void FFuQtScrolledList::deleteAllItems()
{
  this->clear();
}


int FFuQtScrolledList::getSelectedItemIndex() const
{
  return this->currentItem();
}

void FFuQtScrolledList::selectItem(int index, bool notify)
{
  this->setCurrentItem(index);
  if (notify)
    this->browseSelect(index);
}

std::string FFuQtScrolledList::getItemText(int index) const
{
  return this->text(index).toStdString();
}

int FFuQtScrolledList::getNumberOfItems() const
{
  return this->count();
}

bool FFuQtScrolledList::isItemSelected(int index) const
{
  if (index < (int)this->count())
    return this->isSelected(index);
  else
    return false;
}

void FFuQtScrolledList::setSensitivity(bool isSensitive)
{
  if (isSensitive) {
    this->setSelectionMode(Single);
    QPalette currPalette = this->palette();
    currPalette.setColor(QPalette::Text, QColor(0, 0, 0));
    this->setPalette(currPalette);
  }
  else {
    this->setSelectionMode(NoSelection);
    QPalette currPalette = this->palette();
    currPalette.setColor(QPalette::Text, QColor(160, 160, 160));
    this->setPalette(currPalette);
  }
  IAmEnabled = isSensitive;
}


void FFuQtScrolledList::browseSelect(int index)
{
  myBrowseSelectCB.invoke(index);
}

void FFuQtScrolledList::activate(int index)
{
  myActivateCB.invoke(index);
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

  return this->QWidget::event(e);
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

  this->Q3ListBox::mousePressEvent(e);
}
