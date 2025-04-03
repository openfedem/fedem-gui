// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"

#include <iostream>
#include <fstream>


std::map<std::string,FFuaCmdItem*>* FFuaCmdItem::cmdItemMap = NULL;
bool FFuaCmdItem::weAreLoggingCmds = false;

//--------------------------------------------------------------------

void FFuaCmdItem::printCmdListToFile()
{
  if (!cmdItemMap) return;

  std::ofstream liste("FedemCmdList.txt");
  for (const std::pair<const std::string,FFuaCmdItem*>& cmd : *cmdItemMap)
    liste <<"@"<< cmd.first
          <<"$"<< cmd.second->getText()
          <<"$"<< cmd.second->getToolTip() << std::endl;
}
//--------------------------------------------------------------------

void FFuaCmdItem::invokeActivatedCB()
{
  if (weAreLoggingCmds)
    std::cout << this->getCmdItemId() << std::endl;

  this->activatedCB.invoke();
}
//--------------------------------------------------------------------

void FFuaCmdItem::invokeToggledCB(bool toggle)
{
  if (weAreLoggingCmds)
    std::cout << this->getCmdItemId() << std::endl;

  this->toggledCB.invoke(toggle);
}
//--------------------------------------------------------------------

void FFuaCmdItem::init()
{
  if (!cmdItemMap)
    cmdItemMap = new std::map<std::string,FFuaCmdItem*>();
}
//----------------------------------------------------------------------------

FFuaCmdItem* FFuaCmdItem::getCmdItem(const std::string& id)
{
  if (!cmdItemMap || id.empty())
    return NULL;

  std::map<std::string,FFuaCmdItem*>::iterator it = cmdItemMap->find(id);
  if (it != cmdItemMap->end())
    return it->second;

  std::cerr <<" *** FFuaCmdItem: \""<< id <<"\" does not exist"<< std::endl;
  return NULL;
}
//----------------------------------------------------------------------------

FFuaCmdItem::FFuaCmdItem(const std::string& id) : cmdItemId(id)
{
  this->svgIcon = NULL;
  this->smallIcon = NULL;
  this->pixmap = NULL;
  this->accelKey = 0;
  this->toggleAble = false;
  this->behaveAsRadio = false;
  this->toggled = false;
  this->menuButtonPopupMode = false;
  if (!cmdItemMap || id.empty()) return;

  if (cmdItemMap->find(id) == cmdItemMap->end())
    (*cmdItemMap)[id] = this; // unique id
  else
  {
    std::cerr <<" *** FFuaCmdItem: \""<< id <<"\" already exists"<< std::endl;
    abort();
  }
}
//----------------------------------------------------------------------------

FFuaCmdItem::~FFuaCmdItem()
{
  if (cmdItemMap && !cmdItemId.empty()) // subject to static management
    cmdItemMap->erase(cmdItemId);
}
//----------------------------------------------------------------------------

bool FFuaCmdItem::getSensitivity()
{
  bool sensitivity = true;
  this->getSensitivityCB.invoke(sensitivity);
  return sensitivity;
}
//----------------------------------------------------------------------------

bool FFuaCmdItem::getToggled()
{
  bool toggle = this->toggled;
  if (!this->getToggledCB.empty())
    this->getToggledCB.invoke(toggle);
  return toggle;
}
//----------------------------------------------------------------------------
