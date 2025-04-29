// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QLabel>
#include <QComboBox>
#include <QCloseEvent>
#include <QGridLayout>

#include "FFuLib/FFuAuxClasses/FFuaApplication.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtFileDialog.H"


FFuFileDialog* FFuFileDialog::create(const std::string& curDir, const char* name,
				     DialogType type, bool modal)
{
  return new FFuQtFileDialog(curDir.c_str(), name, type, modal);
}


FFuQtFileDialog::FFuQtFileDialog(const char* dirName, const char* name,
				 DialogType type, bool modal)
  : QFileDialog(NULL, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    FFuFileDialog(type,modal)
{
  this->setWidget(this);
  this->setWindowTitle(name);
  this->setDirectory(QDir::fromNativeSeparators(dirName));
  this->setModal(modal);

  switch (type)
    {
    case FFU_NEW_FILE:
      this->setFileMode(QFileDialog::AnyFile);
      break;
    case FFU_OPEN_FILES:
      this->setFileMode(QFileDialog::ExistingFiles);
      break;
    case FFU_OPEN_FILE:
      this->setFileMode(QFileDialog::ExistingFile);
      break;
    case FFU_SAVE_FILE:
      this->setFileMode(QFileDialog::AnyFile);
      this->setAcceptMode(QFileDialog::AcceptSave);
      break;
    case FFU_DIRECTORY:
      this->setFileMode(QFileDialog::Directory);
      break;
    case FFU_DIRECTORY_ONLY:
      this->setFileMode(QFileDialog::Directory);
      this->setOption(ShowDirsOnly, true);
      break;
    }

  if (this->isModal())
    this->setOptions(QFileDialog::DontUseNativeDialog);
  else
    this->exec();
}


void FFuQtFileDialog::setTitle(const char* title)
{
  this->setWindowTitle(title);
}


void FFuQtFileDialog::setFileFilter()
{
  bool appendAllFilter = showAllFilesFilter;
  const char* allFiles = "All files (*)";
  QStringList filters;

  // workaround Qt problem in setting a remembered filter
  std::map<std::string,Memory>::const_iterator it = ourMemory.find(myMemorizer);
  if (it != ourMemory.end())
  {
    const std::string& memFilt = it->second.currentFilter;
    for (const std::pair<const std::string,FileFilter>& filter : myFilterMap)
      if (filter.first != memFilt)
      {
        if (myDefaultFilter != filter.first)
          filters.append(filter.first.c_str());
        else
          filters.prepend(filter.first.c_str());
      }

    if (myFilterMap.find(memFilt) != myFilterMap.end())
      filters.prepend(memFilt.c_str());

    if (showAllFilesFilter && memFilt == allFiles)
    {
      filters.prepend(allFiles);
      appendAllFilter = false;
    }
  }

  if (appendAllFilter)
    filters.append(allFiles);

  this->setNameFilters(filters);
}


void FFuQtFileDialog::clearFileFilters()
{
  myFilterMap.clear();
  myDefaultFilter = "";

  this->setNameFilters(QStringList());
}


int FFuQtFileDialog::getSelectedFilter() const
{
  std::map<std::string,FileFilter>::const_iterator fit;
  fit = myFilterMap.find(this->selectedNameFilter().toStdString());
  return fit == myFilterMap.end() ? -1 : fit->second.id;
}


void FFuQtFileDialog::addUserOptionMenu(const std::string& optID,
                                        const std::string& label)
{
  if (myUserOptions.find(optID) != myUserOptions.end())
    return;

  myUserOptions[optID] = new QComboBox(this);
  myUserOptions[optID]->setMinimumWidth(310);
  this->addField(label,myUserOptions[optID]);
}


void FFuQtFileDialog::addUserOption(const std::string& optID,
                                    const std::string& text, bool defaultOption)
{
  std::map<std::string,QWidget*>::const_iterator it = myUserOptions.find(optID);
  if (it == myUserOptions.end())
    return;

  QComboBox* pt = dynamic_cast<QComboBox*>(it->second);
  if (pt) pt->insertItem(defaultOption ? 0 : -1, text.c_str());
}


std::string FFuQtFileDialog::getSelectedUserOption(const std::string& optID) const
{
  std::map<std::string,QWidget*>::const_iterator it = myUserOptions.find(optID);
  if (it == myUserOptions.end())
    return "";

  QComboBox* pt = dynamic_cast<QComboBox*>(it->second);
  return pt ? pt->currentText().toStdString() : "";
}


FFuToggleButton* FFuQtFileDialog::addUserToggle(const std::string& optID,
                                                const std::string& label,
                                                bool defaultOn)
{
  std::map<std::string,QWidget*>::const_iterator it = myUserOptions.find(label);
  if (it != myUserOptions.end())
    return dynamic_cast<FFuToggleButton*>(it->second);

  FFuQtToggleButton* qToggle = new FFuQtToggleButton(this,label.c_str());
  qToggle->setChecked(defaultOn);
  qToggle->setMinimumWidth(310);
  myUserOptions[optID] = qToggle;

  QGridLayout* layout = static_cast<QGridLayout*>(this->layout());
  layout->addWidget(qToggle,layout->rowCount(),1);

  return qToggle;
}


bool FFuQtFileDialog::getUserToggleSet(const std::string& optID) const
{
  std::map<std::string,QWidget*>::const_iterator it = myUserOptions.find(optID);
  if (it == myUserOptions.end())
    return false;

  QCheckBox* pt = dynamic_cast<QCheckBox*>(it->second);
  return pt ? pt->isChecked() : false;
}


FFuIOField* FFuQtFileDialog::addUserField(const std::string& label,
                                          double defaultValue)
{
  std::map<std::string,QWidget*>::const_iterator it = myUserOptions.find(label);
  if (it != myUserOptions.end())
    return dynamic_cast<FFuIOField*>(it->second);

  FFuIOField* qField = new FFuQtIOField(this);
  qField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  qField->setValue(defaultValue);
  myUserOptions[label] = dynamic_cast<QWidget*>(qField);
  this->addField(label,myUserOptions[label]);

  return qField;
}


double FFuQtFileDialog::getUserFieldValue(const std::string& label) const
{
  std::map<std::string,QWidget*>::const_iterator it = myUserOptions.find(label);
  if (it == myUserOptions.end())
    return 0.0;

  FFuQtIOField* qField = dynamic_cast<FFuQtIOField*>(it->second);
  return qField ? qField->getDouble() : 0.0;
}


FFuIOField* FFuQtFileDialog::addIntField(const std::string& label,
                                         int defaultValue)
{
  std::map<std::string,QWidget*>::const_iterator it = myUserOptions.find(label);
  if (it != myUserOptions.end())
    return dynamic_cast<FFuIOField*>(it->second);

  FFuIOField* qField = new FFuQtIOField(this);
  qField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  qField->setValue(defaultValue);
  myUserOptions[label] = dynamic_cast<QWidget*>(qField);
  this->addField(label,myUserOptions[label]);

  return qField;
}


int FFuQtFileDialog::getIntFieldValue(const std::string& label) const
{
  std::map<std::string,QWidget*>::const_iterator it = myUserOptions.find(label);
  if (it == myUserOptions.end())
    return 0;

  FFuQtIOField* qField = dynamic_cast<FFuQtIOField*>(it->second);
  return qField ? qField->getInt() : 0;
}


FFuIOField* FFuQtFileDialog::addTextField(const std::string& label,
                                          const std::string& defaultValue)
{
  std::map<std::string,QWidget*>::const_iterator it = myUserOptions.find(label);
  if (it != myUserOptions.end())
    return dynamic_cast<FFuIOField*>(it->second);

  FFuIOField* qField = new FFuQtIOField(this);
  qField->setValue(defaultValue);
  myUserOptions[label] = dynamic_cast<QWidget*>(qField);
  this->addField(label,myUserOptions[label]);

  return qField;
}


std::string FFuQtFileDialog::getTextFieldValue(const std::string& label) const
{
  std::map<std::string,QWidget*>::const_iterator it = myUserOptions.find(label);
  if (it == myUserOptions.end())
    return "";

  FFuQtIOField* qField = dynamic_cast<FFuQtIOField*>(it->second);
  return qField ? qField->getValue() : "";
}


std::vector<std::string> FFuQtFileDialog::execute()
{
  // set up filters here.
  this->setFileFilter();

  // recall what we did last time:
  this->recallMemory();

  if (this->isModal())
  {
    FFuaApplication::breakUserEventBlock(true);
    this->exec();
    FFuaApplication::breakUserEventBlock(false);
  }

  return mySelectedFiles;
}


void FFuQtFileDialog::done(int r)
{
  mySelectedFiles.clear();

  if (r == QDialog::Accepted)
  {
    if (this->fileMode() == QFileDialog::ExistingFiles)
      for (const QString& file : this->selectedFiles())
        mySelectedFiles.push_back(QDir::toNativeSeparators(file).toStdString());
    else
      mySelectedFiles.push_back(QDir::toNativeSeparators(this->selectedFiles().first()).toStdString());

    mySelectedFilesCB.invoke(mySelectedFiles);
  }

  // store memory:
  this->storeMemory();
  this->QFileDialog::done(r);

  if (!this->isModal())
    delete this;
}


void FFuQtFileDialog::closeEvent(QCloseEvent* e)
{
  e->accept();
}


void FFuQtFileDialog::storeMemory()
{
  if (!myMemorizer.empty())
  {
    ourMemory[myMemorizer].currentDir    = this->directory().path().toStdString();
    ourMemory[myMemorizer].currentFilter = this->selectedNameFilter().toStdString();
  }
}


void FFuQtFileDialog::recallMemory()
{
  if (!myDefaultFile.empty())
  {
    QDir dir(myDefaultFile.c_str());
    QString cp = dir.absolutePath();
    if (!cp.isNull())
      this->selectFile(cp);
  }
  else
  {
    std::map<std::string,Memory>::const_iterator it = ourMemory.find(myMemorizer);
    if (it != ourMemory.end())
      this->setDirectory(it->second.currentDir.c_str());
  }
}


void FFuQtFileDialog::addField(const std::string& label, QWidget* field)
{
  QGridLayout* layout = static_cast<QGridLayout*>(this->layout());
  int row = layout->rowCount();

  layout->addWidget(new QLabel(label.c_str(),this),row,0);
  layout->addWidget(field,row,1);
}
