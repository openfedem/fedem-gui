// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_POINT_EDITOR_H
#define FUI_QT_POINT_EDITOR_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiPointEditor.H"


class FuiQtPointEditor : public FFuQtWidget, public FuiPointEditor
{
public:
  FuiQtPointEditor(QWidget* parent, const char* name = "FuiQtPointEditor");

  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiPointEditor::setSensitivity(isSensitive);
  }
};

#endif
