// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

/*!
  \file FFuQtPopUpMenu.H
  \brief Qt implementation of a popup menu.
  \details The menu is modal when it executes.
  \author Jacob Storen
  \date 22 Feb 1998
*/

#ifndef FFU_QT_POP_UP_MENU_H
#define FFU_QT_POP_UP_MENU_H

#include <QMenu>

#include "FFuLib/FFuPopUpMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtMenuBase.H"
#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"

class FFuPopUpMenu;


/*!
  \brief Helper class for connecting menu commands to action signals.
  \details Since Qt4 the QMenu class does not have a signal method telling which
  menu item was triggered. Instead the QAction object of the actual menu item
  has a triggered() signal that needs to be connected to the command that shall
  be executed. We therefore need this class to set up this connection since the
  action object itself does not know which menu item it is associated with.
*/

class FFuQtAction : public QObject
{
  Q_OBJECT

  FFuPopUpMenu* menu;
  FFuaCmdItem*  item;

public:
  FFuQtAction(FFuPopUpMenu* m, FFuaCmdItem* i) : menu(m), item(i) {}
  virtual ~FFuQtAction() {}

private slots:
  void activated();
  void toggled();
};


class FFuQtPopUpMenu : public QMenu, public FFuPopUpMenu,
                       public FFuQtMenuBase, public FFuQtComponentBase
{
  Q_OBJECT

  std::vector<FFuQtAction*> myActions;

public:
  // Although a popup menu is always a top level widget,
  // if a parent is passed, the popup menu will be deleted
  // on destruction of that parent (as with any other QObject).
  FFuQtPopUpMenu(QWidget* parent = NULL);
  virtual ~FFuQtPopUpMenu() { for (FFuQtAction* a : myActions) delete a; }

  virtual FFuaCmdItem* executeAtCursorPos();

  virtual bool isMenuEmpty() const;

protected:
  virtual int  basicNewItem(FFuaCmdItem* item, FFuPopUpMenu* menu);
  virtual void basicDeleteItems();
  virtual void insertSeparator();
  virtual void setItemSensitivity(int id, bool sensitive);
  virtual void setItemToggled(int id, bool toggle);
};

#endif
