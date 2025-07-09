// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_FILE_DIALOG_H
#define FFU_QT_FILE_DIALOG_H

#include <QFileDialog>

#include "FFuLib/FFuFileDialog.H"
#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"

class QCloseEvent;


class FFuQtFileDialog : public QFileDialog, public FFuFileDialog,
		        public FFuQtComponentBase
{
public:
  FFuQtFileDialog(const char* dirName, const char* name,
		  FFuFileDialog::DialogType type, bool modal = false);

  virtual void setTitle(const char* title);

  virtual void clearFileFilters();
  virtual int getSelectedFilter() const;

  // return a vector of file names
  virtual std::vector<std::string> execute();

  // add option menus:
  virtual void addUserOptionMenu(const std::string& optID, const std::string& label);
  virtual void addUserOption(const std::string& optID, const std::string& text,
			     bool defaultOption = false);
  virtual std::string getSelectedUserOption(const std::string& optID) const;

  // and add toggles:
  virtual FFuToggleButton* addUserToggle(const std::string& optID,
                                         const std::string& label,
                                         bool defaultOn);
  virtual bool getUserToggleSet(const std::string& optID) const;

  // and add fields:
  virtual FFuIOField* addUserField(const std::string& label, double defaultValue);
  virtual double getUserFieldValue(const std::string& label) const;
  virtual FFuIOField* addIntField(const std::string& label, int defaultValue);
  virtual int getIntFieldValue(const std::string& label) const;
  virtual FFuIOField* addTextField(const std::string& label,
                                   const std::string& defaultValue);
  virtual std::string getTextFieldValue(const std::string& label) const;

private:
  void setFileFilter();
  void storeMemory();
  void recallMemory();
  void addField(const std::string& label, QWidget* field);

  virtual void closeEvent(QCloseEvent* e);
  virtual void done(int r);

  std::map<std::string,QWidget*> myUserOptions;
};

#endif
