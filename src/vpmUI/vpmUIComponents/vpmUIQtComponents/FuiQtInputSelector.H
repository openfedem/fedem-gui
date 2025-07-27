// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_INPUT_SELECTOR_H
#define FUI_QT_INPUT_SELECTOR_H

#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "vpmUI/vpmUIComponents/FuiInputSelector.H"


class FuiQtInputSelector : public FFuQtLabelFrame, public FuiInputSelector
{
public:
  FuiQtInputSelector(QWidget* parent, int iArg = -1);

  virtual void setLabel(const std::string& text)
  {
    this->FFuQtLabelFrame::setLabel(text.c_str());
  }

  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiInputSelector::setSensitivity(isSensitive);
  }
};


class FuiQtInputSelectors : public FuiInputSelectors
{
public:
  FuiQtInputSelectors(QWidget* parent) : myParent(parent) {}

  virtual Sheet* createSheet(int numArg, const char* name) const;

private:
  QWidget* myParent;
};

#endif
