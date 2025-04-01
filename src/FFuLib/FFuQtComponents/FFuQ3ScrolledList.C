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

#include "FFuLib/FFuQtComponents/FFuQ3ScrolledList.H"


FFuQ3ScrolledList::FFuQ3ScrolledList(QWidget* parent) : Q3ListBox(parent)
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


void FFuQ3ScrolledList::setItems(const std::vector<std::string>& items)
{
  this->clear();
  for (const std::string& item : items)
    this->insertItem(item.c_str());
  this->repaint();
}


int FFuQ3ScrolledList::getSelectedItemIndex() const
{
  return this->currentItem();
}


bool FFuQ3ScrolledList::selectItem(int index)
{
  if (index < 0 || index >= static_cast<int>(this->count()))
    return false;

  this->setCurrentItem(index);
  return true;
}


std::string FFuQ3ScrolledList::getItemText(int index) const
{
  if (index < 0 || index >= static_cast<int>(this->count()))
    return "";

  return this->text(index).toStdString();
}


bool FFuQ3ScrolledList::isItemSelected(int index) const
{
  if (index < 0 || index >= static_cast<int>(this->count()))
    return false;

  return this->isSelected(index);
}


void FFuQ3ScrolledList::setSensitivity(bool isSensitive)
{
  int rgb = isSensitive ? 0 : 160;
  this->setPaletteForegroundColor(QColor(rgb,rgb,rgb));
  this->setSelectionMode(isSensitive ? Single : NoSelection);
  IAmEnabled = isSensitive;
}


void FFuQ3ScrolledList::browseSelect(int index)
{
  myBrowseSelectCB.invoke(index);
}

void FFuQ3ScrolledList::activate(int index)
{
  myActivateCB.invoke(index);
}

void FFuQ3ScrolledList::deleteAll()
{
  if (IAmEnabled)
    myClearCB.invoke();
}

void FFuQ3ScrolledList::paste()
{
  if (IAmEnabled)
    myPasteCB.invoke(QApplication::clipboard()->text().toStdString());
}

void FFuQ3ScrolledList::dropEvent(QDropEvent* e)
{
  if (IAmEnabled)
    myPasteCB.invoke(e->mimeData()->text().toStdString());
}

void FFuQ3ScrolledList::dragEnterEvent(QDragEnterEvent* e)
{
  if (IAmEnabled && e->mimeData()->hasText())
    e->accept();
}

bool FFuQ3ScrolledList::event(QEvent* e)
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

  return this->Q3ListBox::event(e);
}

void FFuQ3ScrolledList::keyPressEvent(QKeyEvent* e)
{
  if (IAmEnabled && e->key() == Qt::Key_Delete)
  {
    myDeleteButtonCB.invoke();
    e->accept();
  }
}

void FFuQ3ScrolledList::mousePressEvent(QMouseEvent* e)
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
