// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_SEA_ENVIRONMENT_H
#define FUI_QT_SEA_ENVIRONMENT_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiSeaEnvironment.H"


class FuiQtSeaEnvironment : public FFuQtTopLevelShell, public FuiSeaEnvironment
{
public:
  FuiQtSeaEnvironment(int xpos =100, int ypos  =100,
		      int width=100, int height=100,
		      const char* title= "Fedem",
		      const char* name = "FuiQtSeaEnvironment");

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiSeaEnvironment::setSensitivity(isSensitive);
  }
};

#endif
