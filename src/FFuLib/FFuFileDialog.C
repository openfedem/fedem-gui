// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

/*!
  \class FFuFileDialog FFuFileDialog.H
  \brief File selector with multi-select capabilities (modal and non-modal).

  The dialog is intended used as a modal dialog. Create the dialog with the
  FFuFileDialog::create factory method, and use the return argument from
  execute() to e.g. open files.

  \code

  // Create a modal file dialog
  FFuFileDialog* aDialog = FFuFileDialog::create("./", "Dialog",
                                                 FFuFileDialog::FFU_OPEN_FILE);

  // Extensions
  std::vector<std::string> asciiExtensions;
  asciiExtensions.push_back(".asc");
  asciiExtensions.push_back(".txt");

  aDialog->addFilter("ASCII Files", asciiExtensions);
  aDialog->addFilter("DAC Files", ".dac");
  aDialog->addAllFilesFilter(true);

  // Add a toggle
  aDialog->addUserToggle("MyToggle", "This is a toggle button", true);

  // vector will be empty if the user cancelled
  std::vector<std::string> retFiles = aDialog->execute();

  bool toggleValue = aDialog->getUserToggleSet("MyToggle");

  // Non-modal example to come, maybe.

  \endcode

  \author Jens Lien
*/

#include "FFuLib/FFuFileDialog.H"


Fmd_SOURCE_INIT(FUI_FILESELECTOR, FFuFileDialog, FFuTopLevelShell);


std::map<std::string,FFuFileDialog::Memory> FFuFileDialog::ourMemory;


FFuFileDialog::FFuFileDialog(DialogType type, bool modal)
{
  Fmd_CONSTRUCTOR_INIT(FFuFileDialog);

  myDlgType = type;
  iAmModal = modal;
  showAllFilesFilter = false;
}


void FFuFileDialog::addFilter(const std::string& descr,
                              const std::string& extension,
                              bool defaultFilter, int id)
{
  std::string tmpFilter = descr + " (*." + extension + ")";

  myFilterMap[tmpFilter].id = id;
  myFilterMap[tmpFilter].descr = descr;
  myFilterMap[tmpFilter].extensions.push_back(extension);

  if (defaultFilter)
    myDefaultFilter = tmpFilter;
}


void FFuFileDialog::addFilter(const std::string& descr,
                              const Strings& extensions,
                              bool defaultFilter, int id)
{
  std::string tmpFilter = descr + " ( ";
  for (const std::string& ext : extensions)
    tmpFilter += "*." + ext + " ";
  tmpFilter += ")";

  myFilterMap[tmpFilter].id = id;
  myFilterMap[tmpFilter].descr = descr;
  myFilterMap[tmpFilter].extensions = extensions;

  if (defaultFilter)
    myDefaultFilter = tmpFilter;
}


void FFuFileDialog::resetMemoryMap(const std::string& newDir,
                                   const std::string& oldDir)
{
  for (std::pair<const std::string,Memory>& mem : ourMemory)
    if (oldDir.empty() || mem.second.currentDir == oldDir)
      mem.second.currentDir = newDir;
}
