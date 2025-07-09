// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_QUERY_INPUT_FIELD_H
#define FUI_QT_QUERY_INPUT_FIELD_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"


class FuiQtQueryInputField : public FFuQtWidget, public FuiQueryInputField
{
public:
  FuiQtQueryInputField(QWidget* parent,
                       const char* name = "FuiQtQueryInputField");

  virtual void setBehaviour(Policy behaviour, bool noButton);

  virtual void setToolTip(const char* tip)
  { this->FuiQueryInputField::setToolTip(tip); }

  virtual void setSensitivity(bool makeSensitive)
  { this->FuiQueryInputField::setSensitivity(makeSensitive); }

protected:
  void resizeEvent(QResizeEvent* e);
};

#endif
