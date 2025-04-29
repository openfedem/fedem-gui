// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_FILE_DIALOG_H
#define FFU_FILE_DIALOG_H

#include <string>
#include <vector>
#include <set>
#include <map>

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuToggleButton;
class FFuIOField;


class FFuFileDialog : virtual public FFuTopLevelShell
{
  Fmd_HEADER_INIT();

public:
  enum DialogType {
    FFU_NEW_FILE,
    FFU_OPEN_FILE,
    FFU_OPEN_FILES,
    FFU_SAVE_FILE,
    FFU_DIRECTORY,
    FFU_DIRECTORY_ONLY
  };

  using Strings = std::vector<std::string>;

  FFuFileDialog(DialogType type, bool modal);

  //! \brief Creates a file selection dialog with the following parameters:
  //! \param[in] curDir Initial directory
  //! \param[in] name Dialog caption
  //! \param[in] type Dialog type (Affects selection policy)
  //! \param[in] modal \e true if modal dialog
  static FFuFileDialog* create(const std::string& curDir, const char* name,
                               DialogType type, bool modal = true);

  //! Adds a filter with description \a descr and with one \a extension.
  //! Set \a id to something unique for this filter or leave as default.
  //! id -1 is reserved for non-user-set filters, i.e., "All files", etc.
  void addFilter(const std::string& descr, const std::string& extension,
                 bool defaultFilter = false, int id = -2);
  //! Adds a filter with description and several extensions.
  void addFilter(const std::string& descr, const Strings& extensions,
                 bool defaultFilter = false, int id = -2);
  //! Adds a filter with description and several extensions.
  void addFilter(const std::string& descr,
                 const std::set<std::string>& exts,
                 bool defaultFilter = false, int id = -2)
  {
    this->addFilter(descr,Strings(exts.begin(),exts.end()),defaultFilter,id);
  }

  //! Adds "All Files" filter
  void addAllFilesFilter(bool val) { showAllFilesFilter = val; }

  //! Clears file filters
  virtual void clearFileFilters() = 0;

  //! Return filter id, from addFilter()'s id argument
  virtual int getSelectedFilter() const = 0;

  //! Set initital file name (can be absolute)
  void setDefaultName(const std::string& aName) { myDefaultFile = aName; }

  //! Remembers information on last location, last used filter etc.
  void remember(const std::string& memorizer) { myMemorizer = memorizer; }

  //! Launch a modal file dialog. Returns the selected file names or directory.
  virtual Strings execute() = 0;

  //! Set the callback to be used when the dialog is done.
  void setSelectedFilesCB(const FFaDynCB1<Strings>& cb) { mySelectedFilesCB = cb; }
  //! Set the callback to be used when the dialog is done.
  void setSelectedDirectoryCB(const FFaDynCB1<std::string>& cb) { mySelectedDirCB = cb; }

  //! Adds an option menu. Give the menu an unique identifier with \a optID
  virtual void addUserOptionMenu(const std::string& optID,
                                 const std::string& label) = 0;
  //! Adds an option to option menu with id \a optID
  virtual void addUserOption(const std::string& optID,
                             const std::string& text,
                             bool defaultOption = false) = 0;
  //! Returns the selected option std::string in option menu with id \a optID
  virtual std::string getSelectedUserOption(const std::string& optID) const = 0;

  //! Adds a toggle button with identifier \a optID
  virtual FFuToggleButton* addUserToggle(const std::string& optID,
                                         const std::string& label,
                                         bool defaultOn = false) = 0;
  //! Returns toggle value of toggle button with id \a optID
  virtual bool getUserToggleSet(const std::string& optID) const = 0;

  //! Adds a double field with label \a label
  virtual FFuIOField* addUserField(const std::string& label,
                                   double defaultValue = 1.0) = 0;
  //! Returns field value of field with label \a label
  virtual double getUserFieldValue(const std::string& label) const = 0;

  //! Adds an integer field with label \a label
  virtual FFuIOField* addIntField(const std::string& label,
                                  int defaultValue = 0) = 0;
  //! Returns field value of field with label \a label
  virtual int getIntFieldValue(const std::string& label) const = 0;

  //! Adds a text field with label \a label
  virtual FFuIOField* addTextField(const std::string& label,
                                   const std::string& defaultValue = "") = 0;
  //! Returns field value of field with label \a label
  virtual std::string getTextFieldValue(const std::string& label) const = 0;

  //! Resets the memorized directories (on Save As...)
  static void resetMemoryMap(const std::string& newDir,
                             const std::string& oldDir = "");

protected:
  DialogType myDlgType;
  bool iAmModal;
  bool showAllFilesFilter;
  std::string myMemorizer;
  std::string myDefaultFile;

  FFaDynCB1<Strings>     mySelectedFilesCB;
  FFaDynCB1<std::string> mySelectedDirCB;

  Strings mySelectedFiles;

  struct FileFilter
  {
    int         id = -2;
    std::string descr;
    Strings     extensions;
  };

  std::map<std::string,FileFilter> myFilterMap;
  std::string                      myDefaultFilter;

  struct Memory
  {
    std::string currentDir;
    std::string currentFilter;
    std::map<std::string,std::string> userOpts;
  };

  static std::map<std::string,Memory> ourMemory;
};

#endif
