// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_CREATE_TURBINE_ASSEMBLY_H
#define FUI_QT_CREATE_TURBINE_ASSEMBLY_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiCreateTurbineAssembly.H"


class FuiQtCreateTurbineAssembly : public FFuQtTopLevelShell, public FuiCreateTurbineAssembly
{
public:
  FuiQtCreateTurbineAssembly(int xpos =100, int ypos  =100,
                             int width=100, int height=100,
                             const char* title= "Fedem",
                             const char* name = "FuiQtCreateTurbineAssembly");
  virtual ~FuiQtCreateTurbineAssembly() {}

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiCreateTurbineAssembly::setSensitivity(isSensitive);
  }

  virtual void onCopyButtonClicked();
  virtual void onPasteButtonClicked();
};

#endif
