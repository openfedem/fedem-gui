// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuFileBrowseField.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuFileDialog.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuIOField.H"

//----------------------------------------------------------------------------

FFuFileBrowseField::FFuFileBrowseField()
{
  myFilterId   = -1;
  myDialogType = FFuFileDialog::FFU_OPEN_FILE;
  IAmEditable  = false;
  fileDialog   = NULL;
}
//----------------------------------------------------------------------------

FFuFileBrowseField::~FFuFileBrowseField()
{
  delete fileDialog;
}
//----------------------------------------------------------------------------

void FFuFileBrowseField::setDialogRememberKeyword(const std::string& memorizer)
{
  if (!fileDialog) this->createFileDialog();

  fileDialog->remember(memorizer);
}
//----------------------------------------------------------------------------

void FFuFileBrowseField::addDialogFilter(const std::string& descr,
					 const std::string& extension,
					 bool defaultFilter, int id)
{
  if (!fileDialog) this->createFileDialog();

  fileDialog->addFilter(descr,extension,defaultFilter,id);

  if (defaultFilter) myFilterId = 0;
}
//----------------------------------------------------------------------------

void FFuFileBrowseField::addDialogFilter(const std::string& descr,
					 const std::vector<std::string>& exts,
					 bool defaultFilter, int id)
{
  if (!fileDialog) this->createFileDialog();

  fileDialog->addFilter(descr,exts,defaultFilter,id);

  if (defaultFilter) myFilterId = 0;
}
//----------------------------------------------------------------------------

void FFuFileBrowseField::addAllFilesFilter(bool val)
{
  if (!fileDialog) this->createFileDialog();

  fileDialog->addAllFilesFilter(val);
}

//----------------------------------------------------------------------------

void FFuFileBrowseField::setDialogFilters(const std::vector<Filter>& filters)
{
  if (!fileDialog) this->createFileDialog();

  fileDialog->clearFileFilters();
  fileDialog->addAllFilesFilter(false);
  for (const Filter& filter : filters)
    fileDialog->addFilter(filter.first,filter.second);
  fileDialog->addAllFilesFilter(true);

  if (!filters.empty()) myFilterId = 0;
}
//----------------------------------------------------------------------------

void FFuFileBrowseField::onBrowseButtonClicked()
{
  browseFileCB.invoke(this);
  this->createFileDialog(fileField->getValue());

  myFilterId = fileDialog->getSelectedFilter();
  std::string fileName = this->getSelectedFile();

  fileField->setValue(fileName);
  fileOpenedCB.invoke(fileName,myFilterId);
}
//----------------------------------------------------------------------------

void FFuFileBrowseField::initWidgets()
{
  browseButton->setActivateCB(FFaDynCB0M(FFuFileBrowseField,this,onBrowseButtonClicked));

  fileLabel->setLabel("File");
  browseButton->setLabel("Browse...");

  fileField->setSensitivity(false);
}
//----------------------------------------------------------------------------

void FFuFileBrowseField::setEditable(bool onOff)
{
  IAmEditable = onOff;
  fileField->setSensitivity(onOff);
}
//----------------------------------------------------------------------------

void FFuFileBrowseField::setFileName(const std::string& name)
{
  fileField->setValue(name);
}
//----------------------------------------------------------------------------

std::string FFuFileBrowseField::getFileName() const
{
  return fileField->getValue();
}
//----------------------------------------------------------------------------

void FFuFileBrowseField::setLabel(const char* label)
{
  fileLabel->setLabel(label);
}
//----------------------------------------------------------------------------

void FFuFileBrowseField::setButtonLabel(const char* label, int width)
{
  browseButton->setLabel(label);
  browseButton->setMaxWidth(width);
}
//----------------------------------------------------------------------------

void FFuFileBrowseField::setSensitivity(bool s)
{
  fileField->setSensitivity(s && IAmEditable);
  browseButton->setSensitivity(s);
}
//----------------------------------------------------------------------------

bool FFuFileBrowseField::getSensitivity()
{
  return browseButton->getSensitivity();
}
//----------------------------------------------------------------------------

void FFuFileBrowseField::deleteFileDialog()
{
  delete fileDialog;
  fileDialog = NULL;
  myFilterId = -1;
}
//----------------------------------------------------------------------------
