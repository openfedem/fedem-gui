// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_FILE_BROWSE_FIELD_H
#define FFU_FILE_BROWSE_FIELD_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFuLib/FFuFileDialog.H"
#include <vector>
#include <string>

class FFuPushButton;
class FFuIOField;
class FFuLabel;


class FFuFileBrowseField : virtual public FFuComponentBase
{
  using Filter = std::pair<std::string,std::string>; // (description,extension)
  using FileDialogType = FFuFileDialog::DialogType;

public:
  FFuFileBrowseField();
  virtual ~FFuFileBrowseField();

  void setGetDefaultDirCB(const FFaDynCB1<std::string&>& cb) { getDefaultDirCB = cb; }
  // if you choose to use this callback you probably will delete the dialog after file opened
  // (in the fileopened callback)
  void setBrowseFileCB(const FFaDynCB1<FFuFileBrowseField*>& cb) { browseFileCB = cb; }
  // int is filter id
  void setFileOpenedCB(const FFaDynCB2<const std::string&,int>& cb) { fileOpenedCB = cb; }

  // dialog settings
  // if you dont want it to memorize like all other browse dialogs
  void setDialogRememberKeyword(const std::string& memorizer);
  // all files filter default on
  void addDialogFilter(const std::string& descr, const std::string& extension,
		       bool defaultFilter=false, int id=-2);
  void addDialogFilter(const std::string& descr, const std::vector<std::string>& exts,
		       bool defaultFilter=false, int id=-2);
  void addAllFilesFilter(bool val);

  void setDialogFilters(const std::vector<Filter>& filters);
  void setDialogType(FileDialogType dlgType) { myDialogType = dlgType; }

  // iofield (updated with dialog)
  void setFileName(const std::string& absFileName);
  std::string getFileName() const;

  void setFilterID(int fid) { myFilterId = fid; }
  int  getFilterID() const { return myFilterId; }

  // specfy the path the return value from getFileName is relative if the user chooses
  // relativeNameSelection.
  void setAbsToRelPath(const std::string& absPath) { myAbsToRelPath = absPath; }

  // label, default is "File"
  void setLabel(const char* label);
  void setButtonLabel(const char* label, int width);
  FFuIOField* getIOField() const { return fileField; }

  // recommended to do to save mem. if you delete you must (off course)
  // set the settings again. usage associated with setBrowseFileCB
  void deleteFileDialog();

  void setEditable(bool s);

  virtual void setSensitivity(bool s);
  virtual bool getSensitivity(); //returns sensitivity of component browsebutton

  void onBrowseButtonClicked();

protected:
  void initWidgets();
  void invokeGetDefaultDirCB(std::string& out) { getDefaultDirCB.invoke(out); }

private:
  virtual void createFileDialog(const std::string& fileName = "") = 0;
  virtual std::string getSelectedFile() const = 0;

protected:
  FFuLabel*      fileLabel;
  FFuIOField*    fileField;
  FFuPushButton* browseButton;
  FFuFileDialog* fileDialog;

  std::string    myAbsToRelPath;
  FileDialogType myDialogType;

private:
  int  myFilterId;
  bool IAmEditable;

  FFaDynCB1<std::string&>           getDefaultDirCB;
  FFaDynCB1<FFuFileBrowseField*>    browseFileCB;
  FFaDynCB2<const std::string&,int> fileOpenedCB;
};

#endif
