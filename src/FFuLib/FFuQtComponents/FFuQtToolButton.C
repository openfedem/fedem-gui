// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuAuxClasses/FFuQtAuxClasses/FFuaQtPixmapCache.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "FFuLib/FFuQtComponents/FFuQtPopUpMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtToolButton.H"


FFuQtToolButton::FFuQtToolButton(QWidget* parent) : QToolButton(parent)
{
  this->init();
}

FFuQtToolButton::FFuQtToolButton(QWidget* parent, FFuaCmdItem* cmd) : QToolButton(parent)
{
  this->init();
  this->cmdItem = cmd;
  FFuaCmdHeaderItem* header = dynamic_cast<FFuaCmdHeaderItem*>(cmd);

  //header item
  if (header) {
    const std::vector<FFuaCmdItem*>& children = header->getChildren();
    //set active
    if (!children.empty())
      this->setActiveCmdItem(cmd->hasIcon() ? cmd : children.front());
    //set popup
    FFuQtPopUpMenu* popup = new FFuQtPopUpMenu(dynamic_cast<QWidget*>(this));
    popup->setCommonCB(FFaDynCB1M(FFuQtToolButton,this,onPopUpSelected,FFuaCmdItem*));
    this->setMenu(popup);
    for (FFuaCmdItem* child : children)
      popup->insertCmdItem(child);
  }
  //regular item
  else if (!dynamic_cast<FFuaCmdSeparatorItem*>(cmd))
    this->setActiveCmdItem(cmd);
}

void FFuQtToolButton::updateButton(bool sensitivity)
{
  FFuaCmdHeaderItem* header = dynamic_cast<FFuaCmdHeaderItem*>(this->cmdItem);

  //header item
  if (header) {
    //get popup
    FFuQtPopUpMenu* popup = dynamic_cast<FFuQtPopUpMenu*>(this->menu());
    for (FFuaCmdItem* child : header->getChildren())
      popup->updateCmdItem(child,sensitivity);
  }

  //active item
  if (sensitivity)
    this->setSensitivity(this->activeCmdItem->getSensitivity());
  else if (this->activeCmdItem->getToggleAble())
    this->setToggle(this->activeCmdItem->getToggled());
}

void FFuQtToolButton::init()
{
  this->setButtonWidget(this);

  QObject::connect(this,SIGNAL(pressed()), this, SLOT(arm()));
  QObject::connect(this,SIGNAL(released()),this, SLOT(unarm()));
  QObject::connect(this,SIGNAL(clicked()), this, SLOT(activate()));
  QObject::connect(this,SIGNAL(toggled(bool)), this, SLOT(toggle(bool)));
}

void FFuQtToolButton::setActiveCmdItem(FFuaCmdItem* cmd)
{
  this->activeCmdItem = cmd;

  if (cmd->getSvgIcon())
    this->setIcon(QIcon(cmd->getSvgIcon()));
  else if (cmd->getSmallIcon())
    this->setIcon(QIcon(FFuaQtPixmapCache::getPixmap(cmd->getSmallIcon())));
  else
    this->setIcon(QIcon());

  this->setText(cmd->getText().c_str());
  this->FFuQtButton::setToolTip(cmd->getToolTip().c_str());
  this->setSensitivity(cmd->getSensitivity());
  if (this->getToggleAble() && !cmd->getToggleAble())
    this->setToggle(false);
  this->setToggleAble(cmd->getToggleAble());
  if (cmd->getToggleAble())
    this->setToggle(cmd->getToggled());
}

void FFuQtToolButton::onPopUpSelected(FFuaCmdItem* cmd)
{
  if (this->popupMode() != QToolButton::InstantPopup)
    this->setActiveCmdItem(cmd);
}

void FFuQtToolButton::setToggleAble(bool able)
{
  this->setCheckable(able);
}

void FFuQtToolButton::setToggle(bool toggle)
{
  this->blockLibSignals(true);
  if (!this->isCheckable())
    this->setCheckable(true);
  this->setChecked(toggle);
  this->blockLibSignals(false);
}

void FFuQtToolButton::arm()
{
  this->callArmCB();
}

void FFuQtToolButton::unarm()
{
  this->callUnarmCB();
}

void FFuQtToolButton::activate()
{
  if (this->activeCmdItem->getToggleAble()) return;

  this->callActivateCB();

  // activeCmdItem is not toggle item
  this->activeCmdItem->invokeActivatedCB();
}

void FFuQtToolButton::toggle(bool value)
{
  if (!this->activeCmdItem->getToggleAble()) return;

  if (this->activeCmdItem->getToggleBehaveAsRadio() && !value)
    this->setToggle(true);
  else
  {
    this->callToggleCB(value);
    // activeCmdItem is toggle item
    this->activeCmdItem->setToggled(value);
    this->activeCmdItem->invokeToggledCB(value);
  }
}
