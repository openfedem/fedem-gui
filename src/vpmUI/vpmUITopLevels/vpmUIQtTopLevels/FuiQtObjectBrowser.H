// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_OBJECT_BROWSER_H
#define FUI_QT_OBJECT_BROWSER_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiObjectBrowser.H"


class FuiQtObjectBrowser : public FFuQtTopLevelShell, public FuiObjectBrowser
{
public:
  FuiQtObjectBrowser(int xpos, int ypos, int width, int height,
		     const char* titl, const char* name);
  virtual ~FuiQtObjectBrowser() {}

protected:
  virtual void onCopyDataButtonClicked();

private:
  virtual void setSensitivity(bool) {} // this dialog should always be sensitive
};

#endif
