// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_EXT_CTRL_SYS_PROPERTIES_H
#define FUI_EXT_CTRL_SYS_PROPERTIES_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include <string>
#include <vector>

class FFuFileBrowseField;
class FFuTable;
class FFuLabel;
class FFuaQueryBase;
class FuiQueryInputField;
class FuiQueryInputFieldValues;
class FmModelMemberBase;


class FuiExtCtrlSysProperties : virtual public FFuComponentBase,
                                public FFuUAExistenceHandler,
                                public FFuUADataHandler
{
public:
  FuiExtCtrlSysProperties() { IAmSensitive = true; }

  virtual void buildDynamicWidgets(const FFuaUIValues* values);
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);
  virtual void setSensitivity(bool isSensitive);

  void setEngineSelectedCB(const FFaDynCB1<FuiQueryInputFieldValues&>& aDynCB);

protected:
  void initWidgets();
  virtual void makeNewFields() = 0;

  // UI objects
  FFuFileBrowseField*              fileBrowseField;
  FFuTable*                        myTable;
  std::vector<FFuLabel*>           myNameLabels;
  std::vector<FuiQueryInputField*> myInpEngineFields;

private:
  // CBs from query input field
  void onEngineButtonPressed(FuiQueryInputFieldValues& query);
  void onFieldSelectionChanged(int);

  // CB from file browse field
  void onFileOpened(const std::string&, int);

  // From datahandler
  virtual FFuaUIValues* createValuesObject();

  bool IAmSensitive;
  FFaDynCB1<FuiQueryInputFieldValues&> myButtonPressedCB;
};


class FuaExtCtrlSysPropertiesValues : public FFuaUIValues
{
public:
  FuaExtCtrlSysPropertiesValues() { myEngineQuery = NULL; }

  FFuaQueryBase*                  myEngineQuery;
  std::vector<FmModelMemberBase*> mySelectedQueries;
  std::string filePath;
  std::string modelFilePath;
  std::vector<std::string> fromWorkspace;
  std::vector<std::string> toWorkspace;
};

#endif
