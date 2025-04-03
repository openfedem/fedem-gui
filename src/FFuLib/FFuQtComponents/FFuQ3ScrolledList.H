// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_Q3_SCROLLED_LIST_H
#define FFU_Q3_SCROLLED_LIST_H

#include <Qt3Support/Q3ListBox>

#include "FFuLib/FFuScrolledList.H"
#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"


class FFuQ3ScrolledList : public Q3ListBox, public FFuScrolledList,
                          public FFuQtComponentBase
{
  Q_OBJECT

public:
  FFuQ3ScrolledList(QWidget* parent = NULL);
  virtual ~FFuQ3ScrolledList() {}

  virtual void setItems(const std::vector<std::string>& items);

  virtual int  getSelectedItemIndex() const;
  virtual bool isItemSelected(int index) const;
  virtual bool selectItem(int index);

  virtual std::string getItemText(int index) const;

  virtual void setSensitivity(bool isSensitive);

private slots:
  void browseSelect(int index);
  void activate(int index);
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
