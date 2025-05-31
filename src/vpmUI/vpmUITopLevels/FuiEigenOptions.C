// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiEigenOptions.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuListView.H"
#include "FFuLib/FFuListViewItem.H"
#include "FFuLib/FFuPopUpMenu.H"
#include "FFuLib/FFuFileBrowseField.H"
#include "FFuLib/FFuFileDialog.H"
#include "FFuLib/FFuDialogButtons.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include <algorithm>


enum { MODE, TIME };

Fmd_SOURCE_INIT(FUI_EIGENMODEOPTIONS, FuiEigenOptions, FFuTopLevelShell);

//----------------------------------------------------------------------------

FuiEigenOptions::FuiEigenOptions()
{
  Fmd_CONSTRUCTOR_INIT(FuiEigenOptions);
}
//----------------------------------------------------------------------------

void FuiEigenOptions::initWidgets()
{
  this->addButton->setActivateCB(FFaDynCB0M(FuiEigenOptions,this,
					    onAddButtonClicked));
  this->deleteButton->setActivateCB(FFaDynCB0M(FuiEigenOptions,this,
					       onDeleteButtonClicked));
  this->selectedModesView->setPermSelectionChangedCB(FFaDynCB0M(FuiEigenOptions,this,
								onPermSelectionChanged));
  this->selectedModesView->setTmpSelectionChangedCB(FFaDynCB1M(FuiEigenOptions,this,
							       onTmpSelectionChanged,
							       FFuListViewItem*));

  this->addButton->setLabel("Add");
  this->deleteButton->setLabel("Delete");
  this->deleteButton->setSensitivity(false);

  FFuaCmdItem* item = new FFuaCmdItem();//TODO garbage (should be member,but..)
  item->setText("Delete");
  item->setActivatedCB(FFaDynCB0M(FuiEigenOptions,this,onDeleteButtonClicked));
  this->selectedModesView->getPopUpMenu()->insertCmdItem(item);

  this->selectedModesView->setHeaderClickEnabled(-1,false);//disable header click
  this->selectedModesView->setSglSelectionMode(false);
  this->selectedModesView->setListColumns({"  Mode","Time"});

  this->autoVTFToggle->setLabel("Automatic export to GLview VTF file");
  this->autoVTFToggle->setToggleCB(FFaDynCB1M(FuiEigenOptions,this,
					      onAutoVTFToggled,bool));
  this->autoVTFField->setLabel("Express File");
  this->autoVTFField->setAbsToRelPath("yes");
  this->autoVTFField->setDialogType(FFuFileDialog::FFU_SAVE_FILE);
  this->autoVTFField->setDialogRememberKeyword("AutoVTFField");

  this->autoVTFField->addDialogFilter("Express VTF file","vtf",true,0);
  this->autoVTFField->addDialogFilter("Binary VTF file","vtf",false,1);
  this->autoVTFField->addDialogFilter("ASCII VTF file","vtf",false,2);
  this->autoVTFField->setFileOpenedCB(FFaDynCB2M(FuiEigenOptions,this,
						 onAutoVTFFileChanged,
						 const std::string&,int));

  this->FuiTopLevelDialog::initWidgets();
}
//----------------------------------------------------------------------------

bool FuiEigenOptions::updateDBValues(bool)
{
  this->invokeSetAndGetDBValuesCB(&myValues);
  this->setUIValues(NULL);
  return true;
}
//-----------------------------------------------------------------------------

void FuiEigenOptions::addModeAtTime(int mode, double time)
{
  std::vector<FuiModeData>::const_iterator i1 = myValues.selectedModes.begin();
  std::vector<FuiModeData>::const_iterator i2 = myValues.selectedModes.end();
  FuiModeData newMode{mode,time};
  if (std::find(i1,i2,newMode) == i2)
  {
    myValues.selectedModes.push_back(newMode);
    this->setUISelectedModes();
  }
}
//----------------------------------------------------------------------------

void FuiEigenOptions::addModeAtAllTimes(int mode)
{
  for (double time : myValues.times)
    this->addModeAtTime(mode,time);
}
//----------------------------------------------------------------------------

void FuiEigenOptions::addAllModesAtTime(double time)
{
  for (int mode : myValues.modes)
    this->addModeAtTime(mode,time);
}
//----------------------------------------------------------------------------

void FuiEigenOptions::addAllModes()
{
  myValues.selectedModes.clear();

  for (int mode : myValues.modes)
    for (double time : myValues.times)
      myValues.selectedModes.push_back(FuiModeData{mode,time});

  this->setUISelectedModes();
}
//----------------------------------------------------------------------------

void FuiEigenOptions::onAddButtonClicked()
{
  //get mode
  size_t iMode = modeMenu->getSelectedOption();
  if (iMode > myValues.modes.size())
    return;

  //get time
  size_t iTime = timeMenu->getSelectedOption();
  if (iTime > myValues.times.size())
    return;

  if (iMode < myValues.modes.size() && iTime < myValues.times.size())
    this->addModeAtTime(myValues.modes[iMode],myValues.times[iTime]);
  else if (iMode < myValues.modes.size())
    this->addModeAtAllTimes(myValues.modes[iMode]);
  else if (iTime < myValues.times.size())
    this->addAllModesAtTime(myValues.times[iTime]);
  else
    this->addAllModes();
}
//----------------------------------------------------------------------------

void FuiEigenOptions::onDeleteButtonClicked()
{
  std::vector<FFuListViewItem*> items = selectedModesView->getSelectedListItems();
  if (items.empty()) return;

  for (FFuListViewItem* item : items)
    myValues.eraseModeData(item->getItemId());

  this->setUISelectedModes();
}
//----------------------------------------------------------------------------

void FuiEigenOptions::onTmpSelectionChanged(FFuListViewItem* item)
{
  bool sensitive = item ? true : false;
  this->deleteButton->setSensitivity(sensitive);
  this->selectedModesView->getPopUpMenu()->setSensitivity(sensitive);
}
//----------------------------------------------------------------------------

void FuiEigenOptions::onPermSelectionChanged()
{
  bool sensitive = !this->selectedModesView->getSelectedListItems().empty();
  this->deleteButton->setSensitivity(sensitive);
  this->selectedModesView->getPopUpMenu()->setSensitivity(sensitive);
}
//----------------------------------------------------------------------------

void FuiEigenOptions::setVTFLabel(int id)
{
  const char* vtf_label[3] = { "Express File",
			       " Binary File",
			       "  ASCII File" };

  autoVTFField->setLabel(id >= 0 && id < 3 ? vtf_label[id] : "File");
}
//-----------------------------------------------------------------------------

void FuiEigenOptions::onAutoVTFFileChanged(const std::string& vtfFile, int id)
{
  this->setVTFLabel(id);
  myValues.autoVTFFileName = vtfFile;
  myValues.autoVTFFileType = id;
}
//----------------------------------------------------------------------------

void FuiEigenOptions::onAutoVTFToggled(bool value)
{
  myValues.autoVTFSwitch = value;
  autoVTFField->setSensitivity(value);
}
//----------------------------------------------------------------------------

void FuiEigenOptions::setUIValues(const FFuaUIValues* vals)
{
  if (vals)
    myValues = *((FuaEigenOptionsValues*)vals);

  size_t i, nItems;

  //modes
  modeMenu->clearOptions();
  nItems = myValues.modes.size();
  for (i = 0; i < nItems; i++)
    modeMenu->addOption(FFaNumStr(myValues.modes[i]).c_str());
  if (nItems == 2)
    modeMenu->addOption("(Both)");
  else if (nItems > 2)
    modeMenu->addOption("(All)");
  modeMenu->selectOption(0);

  //times
  timeMenu->clearOptions();
  nItems = myValues.times.size();
  for (i = 0; i < nItems; i++)
    timeMenu->addOption(FFaNumStr(myValues.times[i],1).c_str());
  if (nItems == 2)
    timeMenu->addOption("(Both)");
  else if (nItems > 2)
    timeMenu->addOption("(All)");
  timeMenu->selectOption(0);

  //selectedModes
  this->setUISelectedModes();

  //auto-VTF export
  if (myValues.autoVTFSwitch >= 0) {
    autoVTFToggle->setValue(myValues.autoVTFSwitch);
    autoVTFField->setAbsToRelPath(myValues.modelFilePath);
    autoVTFField->setFileName(myValues.autoVTFFileName);
    autoVTFField->setSensitivity(myValues.autoVTFSwitch);
    this->setVTFLabel(myValues.autoVTFFileType);
    autoVTFToggle->popUp();
    autoVTFField->popUp();
  }
  else {
    autoVTFToggle->popDown();
    autoVTFField->popDown();
  }

  this->setSensitivity(myValues.isSensitive);
}
//----------------------------------------------------------------------------

void FuiEigenOptions::setUISelectedModes()
{
  std::sort(myValues.selectedModes.begin(),myValues.selectedModes.end());

  deleteButton->setSensitivity(false);
  selectedModesView->clearList();
  selectedModesView->getPopUpMenu()->setSensitivity(false);

  for (int i = myValues.selectedModes.size()-1; i >= 0; i--) {
    FFuListViewItem* listItem = selectedModesView->createListItem();
    myValues.selectedModes[i].listId = listItem->getItemId();
    listItem->setItemText(MODE,FFaNumStr("  %d",myValues.selectedModes[i].mode).c_str());
    listItem->setItemText(TIME,("  "+FFaNumStr(myValues.selectedModes[i].time,1)).c_str());
  }
}
//----------------------------------------------------------------------------

bool FuiModeData::operator<(const FuiModeData& m) const
{
  if (this->mode > m.mode)
    return false;
  else if (this->mode < m.mode)
    return true;

  return this->time < m.time;
}
//----------------------------------------------------------------------------

bool FuiModeData::operator==(const FuiModeData& m) const
{
  return this->mode == m.mode && this->time == m.time;
}

//----------------------------------------------------------------------------

bool FuaEigenOptionsValues::eraseModeData(int idToErase)
{
  std::vector<FuiModeData>::iterator it;
  for (it = this->selectedModes.begin(); it != this->selectedModes.end(); it++)
    if (it->listId == idToErase) {
      this->selectedModes.erase(it);
      return true;
    }

  return false;
}
