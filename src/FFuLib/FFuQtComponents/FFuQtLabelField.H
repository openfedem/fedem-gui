// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_LABEL_FIELD_H
#define FFU_QT_LABEL_FIELD_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "FFuLib/FFuLabelField.H"


class FFuQtLabelField : public FFuQtWidget, public FFuLabelField
{
public :
  FFuQtLabelField(QWidget* parent = NULL, const char* name = "FFuQtLabelField");

  virtual void setSensitivity(bool s) { this->FFuLabelField::setSensitivity(s); }
  virtual bool getSensitivity() { return this->FFuLabelField::getSensitivity(); }
  virtual void setToolTip(const char* t) { this->FFuLabelField::setToolTip(t); }
};

#endif
