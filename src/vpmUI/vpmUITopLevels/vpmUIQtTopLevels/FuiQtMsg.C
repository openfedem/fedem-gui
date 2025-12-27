// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/FuiMsg.H"
#include "FFuLib/FFuQtComponents/FFuQtSelectionDialog.H"
#include "FFuLib/FFuAuxClasses/FFuaApplication.H"


namespace
{
  int translateType(FFaMsg::FFaDialogType dType)
  {
    switch (dType) {
    case FFaMsg::DISMISS_WARNING:
    case FFaMsg::WARNING:
      return FFuDialog::WARNING;
    case FFaMsg::DISMISS_ERROR:
    case FFaMsg::ERROR:
      return FFuDialog::ERROR;
    case FFaMsg::DISMISS_INFO:
    case FFaMsg::OK:
      return FFuDialog::INFO;
    default:
      return FFuDialog::QUESTION;
    }

    return 0;
  }
}


int FuiMsg::dialogVt(const std::string& message, const FFaDialogType dType,
                     const std::vector<std::string>& buttonTexts)
{
  FFuQtSelectionDialog dialog(translateType(dType),message,buttonTexts);
  FFuaApplication::breakUserEventBlock(true);
  int resultBt = dialog.exec();
  FFuaApplication::breakUserEventBlock(false);
  return resultBt;
}


int FuiMsg::dialogVt(int& selectionIdx,
                     const std::string& message, const FFaDialogType dType,
                     const std::vector<std::string>& buttonTexts,
                     const std::vector<std::string>& selectionList)
{
  FFuQtSelectionDialog dialog(translateType(dType),message,buttonTexts,
                              selectionList);
  FFuaApplication::breakUserEventBlock(true);
  int resultBt = dialog.exec();
  selectionIdx = dialog.getSelection();
  FFuaApplication::breakUserEventBlock(false);
  return resultBt;
}
