// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiPlugins.H"
#include "vpmUI/Fui.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuDialogButtons.H"
#include "vpmPM/FpPM.H"
#include "vpmDB/FmFuncAdmin.H"
#include "vpmApp/vpmAppUAMap/FapUAFunctionProperties.H"


enum {
  OK = FFuDialogButtons::LEFTBUTTON,
  CANCEL = FFuDialogButtons::MIDBUTTON,
  HELP = FFuDialogButtons::RIGHTBUTTON
};

Fmd_SOURCE_INIT(FUI_PLUGINS, FuiPlugins, FFuTopLevelShell);


FuiPlugins::FuiPlugins()
{
  Fmd_CONSTRUCTOR_INIT(FuiPlugins);
}


void FuiPlugins::initWidgets()
{
  dialogButtons->setButtonClickedCB(FFaDynCB1M(FuiPlugins,this,
                                               onDialogButtonClicked,int));
  dialogButtons->setButtonLabel(OK,    "OK");
  dialogButtons->setButtonLabel(CANCEL,"Cancel");
  dialogButtons->setButtonLabel(HELP,  "Help");
}


void FuiPlugins::onPoppedUp()
{
  std::vector<FpPM::PluginLib> plugins;
  FpPM::getPluginList(plugins);
  this->setNoPlugins(plugins.size());

  for (size_t i = 0; i < plugins.size(); i++)
  {
    myPlugins[i].name = plugins[i].name;
    myPlugins[i].toggl->setLabel(plugins[i].name.c_str());
    myPlugins[i].toggl->setValue(plugins[i].loaded);
    myPlugins[i].label->setLabel(plugins[i].sign.c_str());
  }
}


bool FuiPlugins::onClose()
{
  this->closeDialog(false);
  return false;
}


void FuiPlugins::onDialogButtonClicked(int button)
{
  bool changed = false;
  switch (button) {
  case OK:
    for (Plugin& plugin : myPlugins)
      if (FpPM::togglePlugin(plugin.name,plugin.toggl->getValue()))
        changed = true;

  case CANCEL:
    this->closeDialog(button == OK);
    break;

  case HELP:
    Fui::showCHM("plugins/plugin-architecture.htm");
  }

  if (changed) {
    FmFuncAdmin::clearInfoTable();
    FapUAExistenceHandler::doUpdateUI(FapUAFunctionProperties::getClassTypeID());
    Fui::updateUICommands();
  }
}


FuiPlugins* FuiPlugins::getUI(bool onScreen)
{
  FuiPlugins* plg = NULL;
  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiPlugins::getClassTypeID());
  if (!uic)
    uic = plg = FuiPlugins::create(100,100,400,400);
  else
    plg = dynamic_cast<FuiPlugins*>(uic);

  uic->manage(onScreen,true);

  return plg;
}
