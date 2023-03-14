// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_SCROLLED_LIST_H
#define FFU_QT_SCROLLED_LIST_H

#include <QListWidget>

#include "FFuLib/FFuScrolledList.H"
#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"

class QModelIndex;


class FFuQtScrolledList : public QListWidget, public FFuScrolledList, public FFuQtComponentBase
{
  Q_OBJECT

public:
  FFuQtScrolledList(QWidget* parent = NULL);
  virtual ~FFuQtScrolledList() {}

  // List items interface

  virtual void addItem(const std::string& item); // Appending item to end of list
  virtual void setItems(const std::vector<std::string>& items); // Replace whole list

  virtual void deleteItem(int index); // index = 0,1,...,n
  virtual void deleteAllItems();

  virtual int  getSelectedItemIndex() const;
  virtual void selectItem(int index, bool notify); // Selection slot invoked if notify==True
  virtual bool isItemSelected(int index) const;

  virtual std::string getItemText(int index) const;

  virtual int getNumberOfItems() const;

  virtual void setSensitivity(bool isSensitive);

private slots:
  void browseSelect(const QModelIndex& index);
  void activate(const QModelIndex& index);
  void deleteAll();
  void paste();

protected:
  virtual void dragEnterEvent(QDragEnterEvent* e);
  virtual void dropEvent(QDropEvent* e);
  virtual bool event(QEvent* e);
  virtual void keyPressEvent(QKeyEvent* e);
  virtual void mousePressEvent(QMouseEvent* e);

private:
  bool IAmEnabled;
};

#endif
