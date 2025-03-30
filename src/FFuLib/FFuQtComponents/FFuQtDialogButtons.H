// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_DIALOG_BUTTONS_H
#define FFU_QT_DIALOG_BUTTONS_H

#include <QFrame>

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "FFuLib/FFuDialogButtons.H"


class FFuQtDialogButtons : public QWidget,
                           public FFuQtComponentBase, public FFuDialogButtons
{
public:
  FFuQtDialogButtons(QWidget* parent = NULL, bool addSeparator = true,
		     const char* name = "FFuQtDialogButtons");
};


/*!
  Same as FFuQtDialogButtons but inheriting QFrame instead of QWidget,
  for better control on the spacing when used in a layout.
*/

class FFuQtDialogButtonsF : public QFrame,
                            public FFuQtComponentBase, public FFuDialogButtons
{
public:
  FFuQtDialogButtonsF(QWidget* parent = NULL);
};

#endif
