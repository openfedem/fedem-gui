// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_SELECTION_DIALOG_H
#define FFU_QT_SELECTION_DIALOG_H

#include <QDialog>
#include <string>
#include <vector>

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "FFuLib/FFuDialogType.H"

class QListWidget;


class FFuQtSelectionDialog : public QDialog, public FFuQtComponentBase
{
  Q_OBJECT;

  using Strings = std::vector<std::string>;

public:
  FFuQtSelectionDialog(int dialogType, const std::string& msgText,
                       const Strings& buttonTexts, const Strings& selList = {});
  int getSelection() const;

private:
  QListWidget* myList;
};

#endif
