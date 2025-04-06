// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_LINK_RAM_SETTINGS_H
#define FUI_LINK_RAM_SETTINGS_H

#include <vector>
#include <string>

#include "vpmUI/vpmUITopLevels/FuiTopLevelDialog.H"

class FFuTable;


class FuiLinkRamSettings : public FuiTopLevelDialog
{
  Fmd_HEADER_INIT();

public:
  FuiLinkRamSettings();
  virtual ~FuiLinkRamSettings() {}

  static FuiLinkRamSettings* create(int xpos = 100, int ypos = 100,
                                    int width = 100, int height = 100,
                                    const char* title= "FE-Data Settings",
                                    const char* name = "FuiLinkRamSettings");

protected:
  virtual void initWidgets();

private:
  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);
  virtual void placeWidgets(int width, int height);

protected:
  FFuTable* myTable;
};


class FuaLinkRamSettingsValues : public FFuaUIValues
{
public:
  FuaLinkRamSettingsValues() {}
  virtual ~FuaLinkRamSettingsValues() {}

  struct RamSetting
  {
    std::string id;
    std::string description;
    int         ramLevel = 0;
  };

  std::vector<RamSetting>  rowData;
  std::vector<std::string> ramLevelNames;
};

#endif
