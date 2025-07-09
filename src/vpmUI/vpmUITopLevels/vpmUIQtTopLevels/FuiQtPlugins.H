// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_PLUGINS_H
#define FUI_QT_PLUGINS_H

#include "FFuLib/FFuQtBaseClasses/FFuQtModalDialog.H"
#include "vpmUI/vpmUITopLevels/FuiPlugins.H"

class QBoxLayout;


class FuiQtPlugins : public FFuQtModalDialog,
		     virtual public FuiPlugins
{
public:
  FuiQtPlugins(int xpos = 100, int ypos  = 100,
	       int width= 100, int height= 100,
	       const char* title= "Fedem",
	       const char* name = "FuiQtPlugins");

  virtual void setNoPlugins(unsigned int nPlugins);

private:
  QBoxLayout* myLayout;
};

#endif
