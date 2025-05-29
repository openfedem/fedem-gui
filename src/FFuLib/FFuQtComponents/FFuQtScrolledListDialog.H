// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_SCROLLED_LIST_DIALOG_H
#define FFU_QT_SCROLLED_LIST_DIALOG_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "FFuLib/FFuScrolledListDialog.H"


class FFuQtScrolledListDialog : public FFuQtTopLevelShell,
                                public FFuScrolledListDialog
{
public:
  FFuQtScrolledListDialog(QWidget* parent, bool withNotes = false,
                          int xpos  = 200, int ypos   = 200,
                          int width = 314, int height = 300,
                          const char* title = "Fedem",
                          const char* name  = "FFuQtScrolledListDialog");

protected:
  virtual void closeEvent(QCloseEvent*);
};

#endif
