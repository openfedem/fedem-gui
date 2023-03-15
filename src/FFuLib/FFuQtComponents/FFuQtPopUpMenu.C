// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

/*!
  \file FFuQtPopUpMenu.C
  \brief Qt implementation of a popup menu.
  \details The menu is modal when it executes.
  \author Jacob Storen
  \date 22 Feb 1998
*/

#include "FFuLib/FFuQtComponents/FFuQtPopUpMenu.H"
#include "FFuLib/FFuAuxClasses/FFuQtAuxClasses/FFuaQtPixmapCache.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"


void FFuQtAction::activated()
{
  menu->invokeCommonCB(item);
  item->invokeActivatedCB();
}


void FFuQtAction::toggled()
{
  if (item->getToggleBehaveAsRadio() && item->getToggled())
    return;

  menu->invokeCommonCB(item);
  bool newtoggled = !item->getToggled();
  item->setToggled(newtoggled);
  menu->setCmdItemToggled(item,item->getToggled());
  item->invokeToggledCB(newtoggled);
}


FFuQtPopUpMenu::FFuQtPopUpMenu(QWidget* parent) : QMenu(parent)
{
  this->setWidget(this);
}


FFuaCmdItem* FFuQtPopUpMenu::executeAtCursorPos()
{
  return this->findCmdItem(this->actions().indexOf(this->exec(QCursor::pos())));
}


bool FFuQtPopUpMenu::isMenuEmpty() const
{
  return this->isEmpty();
}


int FFuQtPopUpMenu::basicNewItem(FFuaCmdItem* item, FFuPopUpMenu* menu)
{
  QMenu* qMenu = dynamic_cast<QMenu*>(menu);
  QAction* action = NULL;

  if (item->hasIcon() && !item->getText().empty())
  {
    // px + text
    QIcon* ic;
    if (item->getBigIcon()) {
      ic = new QIcon(FFuaQtPixmapCache::getPixmap(item->getBigIcon()));
      if (item->getSmallIcon())
        ic->addPixmap(FFuaQtPixmapCache::getPixmap(item->getSmallIcon()));
    }
    else if (item->getSmallIcon())
      ic = new QIcon(FFuaQtPixmapCache::getPixmap(item->getSmallIcon()));
    else
      ic = new QIcon();
    if (qMenu)
    {
      qMenu->setIcon(*ic);
      qMenu->setTitle(item->getText().c_str());
      action = this->addMenu(qMenu);
    }
    else
      action = this->addAction(*ic,item->getText().c_str());
    delete ic;
  }
  else if (item->getPixmap())
  {
    // px only
    QIcon icon(FFuaQtPixmapCache::getPixmap(item->getPixmap()));
    if (qMenu)
    {
      qMenu->setIcon(icon);
      action = this->addMenu(qMenu);
    }
    else
      action = this->addAction(icon,"");
  }
  else if (qMenu)
  {
    // text only or neither text nor px
    if (!item->getText().empty())
      qMenu->setTitle(item->getText().c_str());
    action = this->addMenu(qMenu);
  }
  else if (!item->getText().empty())
    action = this->addAction(item->getText().c_str());
  else
    return -1;

  myActions.push_back(new FFuQtAction(this,item));

  if (item->getAccelKey())
    action->setShortcut(item->getAccelKey());

  action->setEnabled(item->getSensitivity());

  if (item->getToggleAble()) {
    action->setChecked(item->getToggled());
    QObject::connect(action, SIGNAL(triggered()), myActions.back(), SLOT(toggled()));
  }
  else
    QObject::connect(action, SIGNAL(triggered()), myActions.back(), SLOT(activated()));

  return this->actions().size();
}


void FFuQtPopUpMenu::basicDeleteItems()
{
  this->clear();

  for (FFuQtAction* p : myActions)
    delete p;
  myActions.clear();
}


void FFuQtPopUpMenu::insertSeparator()
{
  this->addSeparator();
}


void FFuQtPopUpMenu::setItemSensitivity(int id, bool sensitive)
{
  if (id > 0 && id <= this->actions().size())
    this->actions().at(id-1)->setEnabled(sensitive);
}


void FFuQtPopUpMenu::setItemToggled(int id, bool toggle)
{
  if (id > 0 && id <= this->actions().size())
    this->actions().at(id-1)->setChecked(toggle);
}
