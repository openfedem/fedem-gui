// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_FILE_BROWSE_FIELD_H
#define FFU_QT_FILE_BROWSE_FIELD_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "FFuLib/FFuFileBrowseField.H"


class FFuQtFileBrowseField : public FFuQtWidget, public FFuFileBrowseField
{
public:
  FFuQtFileBrowseField(QWidget* parent, const char* name = "FFuQtFileBrowseField");

  virtual void setSensitivity(bool s) { this->FFuFileBrowseField::setSensitivity(s); }
  virtual bool getSensitivity() { return this->FFuFileBrowseField::getSensitivity(); }

private:
  virtual void createFileDialog(const std::string& fileName);
  virtual std::string getSelectedFile() const;
};

#endif
