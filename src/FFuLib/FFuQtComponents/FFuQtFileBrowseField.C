// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtFileBrowseField.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtFileDialog.H"


FFuQtFileBrowseField::FFuQtFileBrowseField(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  FFuQtLabel* qLabel;
  FFuQtIOField* qField;
  FFuQtPushButton* qButton;
  fileLabel = qLabel = new FFuQtLabel();
  fileField = qField = new FFuQtIOField();
  browseButton = qButton = new FFuQtPushButton();

  this->initWidgets();

  QLayout* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qLabel);
  layout->addWidget(qField);
  layout->addWidget(qButton);
}


void FFuQtFileBrowseField::createFileDialog(const std::string& fileName)
{
  if (!fileDialog)
  {
    std::string dir;
    this->invokeGetDefaultDirCB(dir);
    fileDialog = new FFuQtFileDialog(dir.c_str(),"Select file",myDialogType,true);
    if (!myAbsToRelPath.empty())
      fileDialog->addUserToggle("relToggle","Use path relative to model-file location",true);
    fileDialog->remember("FFuFileBrowseField");
  }

  if (!fileName.empty())
  {
    if (myAbsToRelPath.empty() || QFileInfo(fileName.c_str()).isAbsolute())
      fileDialog->setDefaultName(fileName);
    else
      fileDialog->setDefaultName(myAbsToRelPath + fileName);
  }
}


std::string FFuQtFileBrowseField::getSelectedFile() const
{
  std::vector<std::string> fileNames = fileDialog->execute();
  if (fileNames.empty()) return "";

  const std::string& fileName = fileNames.front();
  if (!fileName.empty() && fileDialog->getUserToggleSet("relToggle"))
  {
    QString relativePath = QDir(myAbsToRelPath.c_str()).relativeFilePath(fileName.c_str());
    return QDir::toNativeSeparators(relativePath).toStdString();
  }

  return fileName;
}
